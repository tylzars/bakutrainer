#include <Main/System/qrk_decode.h>

#pragma thumb off

// Work area of the "QRK" unpacker: 11-bit bit probabilities (0x400 = even) for its range decoder, filled with
// 0x04000400 words before each file. 0x1588 bytes (GetQrkWorkSize).
typedef struct QrkProbs {
    /* 000 */ u16 mDistSlotProbs[8][32];         // 5-bit trees: distance slot, by match length
    /* 200 */ u16 mDistLowBitProbs[64];          // 6-bit tree: low distance bits
    /* 280 */ u16 mOneByteMatchDistProbs[4];     // 2-bit tree: distance of a 1-byte match
    /* 288 */ u16 mRepeatOrNewMatchProbs[32];    // 1-bit trees (two entries per state): repeat match or new match
    /* 2c8 */ u16 mMatchLenProbs[32][16];        // 4-bit trees: match length, by length state
    /* 6c8 */ u16 mMatchLongLenProbs[256];       // 8-bit tree: long match length
    /* 8c8 */ u16 mRepeatLenProbs[32][16];       // 4-bit trees: repeat match length, by length state
    /* cc8 */ u16 mRepeatLongLenProbs[256];      // 8-bit tree: long repeat match length
    /* ec8 */ u16 mRepeatDistIndexProbs[16][4];  // 2-bit trees: which of the four last distances, by state
    /* f48 */ u16 mLiteralOrMatchProbs[32];      // 1-bit trees (two entries per state): literal or match
    // 8-bit trees: literal bytes (plain, then matched against a 0 or a 1 bit)
    /* f88 */ u16 mLiteralProbs[3][256];
} QrkProbs;

// Header: "QRK", a byte whose top three bits are the format version (QRK_VERSION), then the unpacked size as a
// variable-length number and the range-coded data.
#define QRK_VERSION 4

// Bit probabilities have QRK_PROB_BITS bits and move by 1/16 of the distance to 0 or 1 per decoded bit.
#define QRK_PROB_BITS 11
#define QRK_PROB_ONE 2048
#define QRK_PROB_INIT 1024
#define QRK_MOVE_BITS 4
#define QRK_RANGE_TOP 0x1000000

static s32 GetQrkVersion(const u8 *src);
static u32 ReadQrkHeader(const u8 **src);

// Direct (equiprobable) bits after each distance slot, and the slot's base distance before the low six bits are
// appended.
static u8 sQrkSlotDirectBits[32] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12,
};
static u16 sQrkSlotBase[32] = {
    0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x0009, 0x000b,
    0x000f, 0x0017, 0x0027, 0x0047, 0x0087, 0x0107, 0x0207, 0x0407, 0x0807, 0x1007, 0x1807,
    0x2007, 0x2807, 0x3007, 0x3807, 0x4007, 0x4807, 0x5007, 0x5807, 0x6007, 0x6807,
};
// The low six distance bits come out of their tree reversed; this puts them back in order.
static u8 sQrkLowBitOrder[64] = {
    0x00, 0x20, 0x10, 0x30, 0x08, 0x28, 0x18, 0x38, 0x04, 0x24, 0x14, 0x34, 0x0c, 0x2c, 0x1c, 0x3c,
    0x02, 0x22, 0x12, 0x32, 0x0a, 0x2a, 0x1a, 0x3a, 0x06, 0x26, 0x16, 0x36, 0x0e, 0x2e, 0x1e, 0x3e,
    0x01, 0x21, 0x11, 0x31, 0x09, 0x29, 0x19, 0x39, 0x05, 0x25, 0x15, 0x35, 0x0d, 0x2d, 0x1d, 0x3d,
    0x03, 0x23, 0x13, 0x33, 0x0b, 0x2b, 0x1b, 0x3b, 0x07, 0x27, 0x17, 0x37, 0x0f, 0x2f, 0x1f, 0x3f,
};

// Unpacks `size` bytes of range-coded LZ data from `src` to `dst`; `p` is the probability work area.
// Matching: plainer forms of the following give other registers or another instruction order.
// - The locals are declared in this order, code and range last.
// - code is built up through bound.
// - The reps loop counts with i.
// - The two plain literal decoders sit in blocks with their own locals.
// - The literal/match bit is decoded with len as the symbol, i as the one-pass counter and pp as the table.
// - The matched-literal loop sets k, sym and matching before a `for (; k >= 0; k--)`.
// - nbits counts the 4-bit length tree and the 5-bit slot tree (whose table is pp), and is read through `sym & 0x1f`
//   before slot is set.
// - The 6-bit tree takes a fresh pp = &p->mDistLowBitProbs[sym] each step. That puts the moves of 1 and 0 ahead of
//   the table address add before that loop.
static void UnpackQrkData(QrkProbs *p, u8 *dst, const u8 *src, u32 size) {
    u8 *end;
    u32 matchByte;
    u32 lenState;
    u32 litState;
    u32 state;
    u32 reps[4];
    u32 dist;
    u32 bound;
    s32 i;
    s32 len;
    u8 slot;
    u32 tmp; // the literal/match bit, then the matched byte's bit, then a probability step
    s32 nbits;
    u32 uprob;
    u32 idx;
    u16 *pp;
    u32 direct;
    u8 matching;
    s32 prob;
    u16 *probs;
    s32 k;
    u32 sym;
    u32 bit;
    u32 code;
    u32 range;

    end = dst + size;
    for (i = 0; i < sizeof(p->mDistSlotProbs) / 4; i++) {
        ((u32 *) p->mDistSlotProbs)[i] = (QRK_PROB_INIT << 16) | QRK_PROB_INIT;
    }
    for (i = 0; i < sizeof(p->mDistLowBitProbs) / 4; i++) {
        ((u32 *) p->mDistLowBitProbs)[i] = (QRK_PROB_INIT << 16) | QRK_PROB_INIT;
    }
    for (i = 0; i < sizeof(p->mOneByteMatchDistProbs) / 4; i++) {
        ((u32 *) p->mOneByteMatchDistProbs)[i] = (QRK_PROB_INIT << 16) | QRK_PROB_INIT;
    }
    for (i = 0; i < sizeof(p->mRepeatOrNewMatchProbs) / 4; i++) {
        ((u32 *) p->mRepeatOrNewMatchProbs)[i] = (QRK_PROB_INIT << 16) | QRK_PROB_INIT;
    }
    for (i = 0; i < sizeof(p->mMatchLenProbs) / 4; i++) {
        ((u32 *) p->mMatchLenProbs)[i] = (QRK_PROB_INIT << 16) | QRK_PROB_INIT;
    }
    for (i = 0; i < sizeof(p->mMatchLongLenProbs) / 4; i++) {
        ((u32 *) p->mMatchLongLenProbs)[i] = (QRK_PROB_INIT << 16) | QRK_PROB_INIT;
    }
    for (i = 0; i < sizeof(p->mRepeatLenProbs) / 4; i++) {
        ((u32 *) p->mRepeatLenProbs)[i] = (QRK_PROB_INIT << 16) | QRK_PROB_INIT;
    }
    for (i = 0; i < sizeof(p->mRepeatLongLenProbs) / 4; i++) {
        ((u32 *) p->mRepeatLongLenProbs)[i] = (QRK_PROB_INIT << 16) | QRK_PROB_INIT;
    }
    for (i = 0; i < sizeof(p->mRepeatDistIndexProbs) / 4; i++) {
        ((u32 *) p->mRepeatDistIndexProbs)[i] = (QRK_PROB_INIT << 16) | QRK_PROB_INIT;
    }
    for (i = 0; i < sizeof(p->mLiteralOrMatchProbs) / 4; i++) {
        ((u32 *) p->mLiteralOrMatchProbs)[i] = (QRK_PROB_INIT << 16) | QRK_PROB_INIT;
    }
    for (i = 0; i < sizeof(p->mLiteralProbs) / 4; i++) {
        ((u32 *) p->mLiteralProbs)[i] = (QRK_PROB_INIT << 16) | QRK_PROB_INIT;
    }

    bound = src[0];
    bound = (bound << 8) + src[1];
    bound = (bound << 8) + src[2];
    code = (bound << 8) + src[3];
    range = 0xffffffff;
    state = 0xf;
    lenState = 0x1f;
    for (i = 0; i < 4; i++) {
        reps[i] = 1;
    }
    src += 4;

    // The first byte is a plain literal.
    {
        s32 lk;
        u32 lsym;
        u32 lprob;
        u32 lbit;

        lsym = 1;
        for (lk = 7; lk >= 0; lk--) {
            pp = &p->mLiteralProbs[0][lsym];
            lprob = *pp;
            bound = (range >> QRK_PROB_BITS) * lprob;
            if (code < bound) {
                range = bound;
                lbit = 0;
            } else {
                lbit = 1;
                range -= bound;
                code -= bound;
            }
            while (range < QRK_RANGE_TOP) {
                code = (code << 8) + *src++;
                range <<= 8;
            }
            lsym <<= 1;
            if (!lbit) {
                *pp += (u16) ((QRK_PROB_ONE - lprob) >> QRK_MOVE_BITS);
            } else {
                lsym++;
                *pp -= lprob >> QRK_MOVE_BITS;
            }
        }
        matchByte = (u8) lsym;
    }
    *dst++ = matchByte;
    if (dst >= end) {
        return;
    }

    do {
        len = 1;
        pp = &p->mLiteralOrMatchProbs[state * 2];
        for (i = 0; i >= 0; i--) {
            prob = pp[len];
            bound = (range >> QRK_PROB_BITS) * prob;
            if (code < bound) {
                pp[len] = prob + ((QRK_PROB_ONE - prob) >> QRK_MOVE_BITS);
                range = bound;
                len = len << 1;
            } else {
                pp[len] = prob - (prob >> QRK_MOVE_BITS);
                len = (len << 1) + 1;
                range -= bound;
                code -= bound;
            }
            while (range < QRK_RANGE_TOP) {
                code = (code << 8) + *src++;
                range <<= 8;
            }
        }
        tmp = len & 1;
        if (tmp) {
            // A literal: right after a match (no literal since) its bits are coded against the byte that would
            // have continued the match, until the first bit that differs.
            litState = state & 3;
            if (litState == 0) {
                k = 7;
                sym = 1;
                matching = 1;
                for (; k >= 0; k--) {
                    if (matching) {
                        tmp = (matchByte >> k) & 1;
                        idx = tmp + 1;
                    } else {
                        idx = 0;
                    }
                    pp = &p->mLiteralProbs[0][(idx << 8) + sym];
                    uprob = *pp;
                    bound = (range >> QRK_PROB_BITS) * uprob;
                    if (code < bound) {
                        range = bound;
                        bit = 0;
                    } else {
                        bit = 1;
                        range -= bound;
                        code -= bound;
                    }
                    while (range < QRK_RANGE_TOP) {
                        code = (code << 8) + *src++;
                        range <<= 8;
                    }
                    matching = matching && tmp == bit;
                    sym <<= 1;
                    if (!bit) {
                        tmp = (QRK_PROB_ONE - uprob) >> QRK_MOVE_BITS;
                        *pp += (u16) tmp;
                    } else {
                        sym++;
                        tmp = uprob >> QRK_MOVE_BITS;
                        *pp -= (u16) tmp;
                    }
                }
                matchByte = (u8) sym;
            } else {
                {
                    s32 lk;
                    u32 lsym;
                    u32 lbit;

                    lsym = 1;
                    for (lk = 7; lk >= 0; lk--) {
                        pp = &p->mLiteralProbs[0][lsym];
                        uprob = *pp;
                        bound = (range >> QRK_PROB_BITS) * uprob;
                        if (code < bound) {
                            range = bound;
                            lbit = 0;
                        } else {
                            lbit = 1;
                            range -= bound;
                            code -= bound;
                        }
                        while (range < QRK_RANGE_TOP) {
                            code = (code << 8) + *src++;
                            range <<= 8;
                        }
                        lsym <<= 1;
                        if (!lbit) {
                            *pp += (u16) ((QRK_PROB_ONE - uprob) >> QRK_MOVE_BITS);
                        } else {
                            lsym++;
                            *pp -= uprob >> QRK_MOVE_BITS;
                        }
                    }
                    matchByte = (u8) lsym;
                }
            }
            *dst++ = matchByte;
            if (dst >= end) {
                return;
            }
            if (litState != 3) {
                state++;
            }
            if (lenState != 0x1f) {
                lenState++;
            }
            continue;
        }
        sym = 1;
        probs = &p->mRepeatOrNewMatchProbs[state * 2];
        for (k = 0; k >= 0; k--) {
            prob = probs[sym];
            bound = (range >> QRK_PROB_BITS) * prob;
            if (code < bound) {
                probs[sym] = prob + ((QRK_PROB_ONE - prob) >> QRK_MOVE_BITS);
                range = bound;
                sym = sym << 1;
            } else {
                probs[sym] = prob - (prob >> QRK_MOVE_BITS);
                sym = (sym << 1) + 1;
                range -= bound;
                code -= bound;
            }
            while (range < QRK_RANGE_TOP) {
                code = (code << 8) + *src++;
                range <<= 8;
            }
        }
        if (!(sym & 1)) {
            // A new match: its length, then its distance.
            sym = 1;
            probs = p->mMatchLenProbs[lenState];
            for (nbits = 3; nbits >= 0; nbits--) {
                prob = probs[sym];
                bound = (range >> QRK_PROB_BITS) * prob;
                if (code < bound) {
                    probs[sym] = prob + ((QRK_PROB_ONE - prob) >> QRK_MOVE_BITS);
                    range = bound;
                    sym = sym << 1;
                } else {
                    probs[sym] = prob - (prob >> QRK_MOVE_BITS);
                    sym = (sym << 1) + 1;
                    range -= bound;
                    code -= bound;
                }
                while (range < QRK_RANGE_TOP) {
                    code = (code << 8) + *src++;
                    range <<= 8;
                }
            }
            len = sym & 0xf;
            if (len++ == 15) {
                sym = 1;
                probs = p->mMatchLongLenProbs;
                for (k = 7; k >= 0; k--) {
                    prob = probs[sym];
                    bound = (range >> QRK_PROB_BITS) * prob;
                    if (code < bound) {
                        probs[sym] = prob + ((QRK_PROB_ONE - prob) >> QRK_MOVE_BITS);
                        range = bound;
                        sym = sym << 1;
                    } else {
                        probs[sym] = prob - (prob >> QRK_MOVE_BITS);
                        sym = (sym << 1) + 1;
                        range -= bound;
                        code -= bound;
                    }
                    while (range < QRK_RANGE_TOP) {
                        code = (code << 8) + *src++;
                        range <<= 8;
                    }
                }
                len += sym & 0xff;
            }
            if (len == 1) {
                sym = 1;
                probs = p->mOneByteMatchDistProbs;
                for (k = 1; k >= 0; k--) {
                    prob = probs[sym];
                    bound = (range >> QRK_PROB_BITS) * prob;
                    if (code < bound) {
                        probs[sym] = prob + ((QRK_PROB_ONE - prob) >> QRK_MOVE_BITS);
                        range = bound;
                        sym = sym << 1;
                    } else {
                        probs[sym] = prob - (prob >> QRK_MOVE_BITS);
                        sym = (sym << 1) + 1;
                        range -= bound;
                        code -= bound;
                    }
                    while (range < QRK_RANGE_TOP) {
                        code = (code << 8) + *src++;
                        range <<= 8;
                    }
                }
                dist = sym & 3;
            } else {
                k = len - 2;
                if (k > 7) {
                    k = 7;
                }
                sym = 1;
                pp = p->mDistSlotProbs[k];
                for (nbits = 4; nbits >= 0; nbits--) {
                    prob = pp[sym];
                    bound = (range >> QRK_PROB_BITS) * prob;
                    if (code < bound) {
                        pp[sym] = prob + ((QRK_PROB_ONE - prob) >> QRK_MOVE_BITS);
                        range = bound;
                        sym = sym << 1;
                    } else {
                        pp[sym] = prob - (prob >> QRK_MOVE_BITS);
                        sym = (sym << 1) + 1;
                        range -= bound;
                        code -= bound;
                    }
                    while (range < QRK_RANGE_TOP) {
                        code = (code << 8) + *src++;
                        range <<= 8;
                    }
                }
                nbits = sQrkSlotDirectBits[sym & 0x1f];
                slot = sym & 0x1f;
                dist = sQrkSlotBase[slot];
                if (nbits != 0) {
                    direct = 0;
                    do {
                        range >>= 1;
                        direct <<= 1;
                        if (code >= range) {
                            direct += 1;
                            code -= range;
                        }
                        while (range < QRK_RANGE_TOP) {
                            code = (code << 8) + *src++;
                            range <<= 8;
                        }
                    } while (--nbits != 0);
                    dist += direct;
                }
                sym = 1;
                for (nbits = 0; nbits < 6; nbits++) {
                    pp = &p->mDistLowBitProbs[sym];
                    prob = *pp;
                    bound = (range >> QRK_PROB_BITS) * prob;
                    if (code < bound) {
                        range = bound;
                        *pp = prob + ((QRK_PROB_ONE - prob) >> QRK_MOVE_BITS);
                        sym = sym << 1;
                    } else {
                        *pp = prob - (prob >> QRK_MOVE_BITS);
                        range -= bound;
                        code -= bound;
                        sym = (sym << 1) + 1;
                    }
                    while (range < QRK_RANGE_TOP) {
                        code = (code << 8) + *src++;
                        range <<= 8;
                    }
                }
                dist <<= 6;
                dist += sQrkLowBitOrder[sym - 64];
            }
            dist++;
            for (k = 3; k > 0; k--) {
                reps[k] = reps[k - 1];
            }
            reps[0] = dist;
            if ((state & 0xc) != 0xc) {
                state += 4;
            }
            lenState += len;
            state &= ~3;
            if (lenState >= 0x20) {
                lenState = 0x1f;
            }
        } else {
            // A repeated match: its length, then which of the last four distances.
            sym = 1;
            probs = p->mRepeatLenProbs[lenState];
            for (k = 3; k >= 0; k--) {
                prob = probs[sym];
                bound = (range >> QRK_PROB_BITS) * prob;
                if (code < bound) {
                    probs[sym] = prob + ((QRK_PROB_ONE - prob) >> QRK_MOVE_BITS);
                    range = bound;
                    sym = sym << 1;
                } else {
                    probs[sym] = prob - (prob >> QRK_MOVE_BITS);
                    sym = (sym << 1) + 1;
                    range -= bound;
                    code -= bound;
                }
                while (range < QRK_RANGE_TOP) {
                    code = (code << 8) + *src++;
                    range <<= 8;
                }
            }
            len = sym & 0xf;
            if (len++ == 15) {
                sym = 1;
                probs = p->mRepeatLongLenProbs;
                for (k = 7; k >= 0; k--) {
                    prob = probs[sym];
                    bound = (range >> QRK_PROB_BITS) * prob;
                    if (code < bound) {
                        probs[sym] = prob + ((QRK_PROB_ONE - prob) >> QRK_MOVE_BITS);
                        range = bound;
                        sym = sym << 1;
                    } else {
                        probs[sym] = prob - (prob >> QRK_MOVE_BITS);
                        sym = (sym << 1) + 1;
                        range -= bound;
                        code -= bound;
                    }
                    while (range < QRK_RANGE_TOP) {
                        code = (code << 8) + *src++;
                        range <<= 8;
                    }
                }
                len += sym & 0xff;
            }
            sym = 1;
            probs = p->mRepeatDistIndexProbs[state];
            for (k = 1; k >= 0; k--) {
                prob = probs[sym];
                bound = (range >> QRK_PROB_BITS) * prob;
                if (code < bound) {
                    probs[sym] = prob + ((QRK_PROB_ONE - prob) >> QRK_MOVE_BITS);
                    range = bound;
                    sym = sym << 1;
                } else {
                    probs[sym] = prob - (prob >> QRK_MOVE_BITS);
                    sym = (sym << 1) + 1;
                    range -= bound;
                    code -= bound;
                }
                while (range < QRK_RANGE_TOP) {
                    code = (code << 8) + *src++;
                    range <<= 8;
                }
            }
            k = sym & 3;
            dist = reps[k];
            for (; k > 0; k--) {
                reps[k] = reps[k - 1];
            }
            lenState = len - 1;
            if (len - 1 >= 0x20) {
                lenState = 0x1f;
            }
            reps[0] = dist;
            state = 0;
        }
        do {
            *dst = *(dst - dist);
            dst++;
        } while (--len > 0);
        matchByte = *(dst - dist);
    } while (dst < end);
}

// Unpacks the "QRK" data at `src` to `dst` with the work area `work` (GetQrkWorkSize() bytes).
void UnpackQrk(const u8 *src, u8 *dst, void *work) {
    u32 size = ReadQrkHeader(&src);
    if (size == 0) {
        return;
    }
    UnpackQrkData(work, dst, src, size);
}

// Returns the unpacked size of the "QRK" data at `src` (0 if it isn't QRK_VERSION data).
u32 GetQrkUnpackedSize(const u8 *src) {
    return ReadQrkHeader(&src);
}

u32 GetQrkWorkSize(void) {
    return sizeof(QrkProbs);
}

// Returns the format version of the "QRK" data at `src`, or -1 when the tag is missing.
static s32 GetQrkVersion(const u8 *src) {
    if (src[0] != 'Q' || src[1] != 'R' || src[2] != 'K') {
        return -1;
    }
    return src[3] >> 5;
}

// Reads the header at `*src`: returns the unpacked size (0 for other data) and moves `*src` to the packed data.
// The size has as many extra bytes as the first byte has leading 1 bits; each extra byte also adds 1 << (7 * n).
static u32 ReadQrkHeader(const u8 **src) {
    u32 value = 0;
    u32 extra = 0;
    s32 n = 1;
    u32 mask = 0x80;
    u32 b;
    u32 c;
    if (GetQrkVersion(*src) != QRK_VERSION) {
        return 0;
    }
    *src += 4;
    b = **src;
    (*src)++;
    if (b & 0x80) {
        do {
            extra += 1 << (n * 8 - n);
            b &= ~mask;
            mask >>= 1;
            n++;
            c = **src;
            (*src)++;
            value = (value << 8) + c;
        } while (b & mask);
    }
    value |= b << ((n - 1) * 8);
    return value + (extra + 1);
}

#pragma thumb on
