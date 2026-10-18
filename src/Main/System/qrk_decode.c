#include <Main/System/qrk_decode.h>

#pragma thumb off

// Work area of the "QRK" unpacker: 11-bit bit probabilities (0x400 = even) for its range decoder, filled with
// 0x04000400 words before each file. 0x1588 bytes (func_02030760).
typedef struct QrkProbs {
    /* 000 */ u16 mUnk_000[8][32];   // 5-bit trees: distance slot, by match length
    /* 200 */ u16 mUnk_200[64];      // 6-bit tree: low distance bits
    /* 280 */ u16 mUnk_280[4];       // 2-bit tree: distance of a 1-byte match
    /* 288 */ u16 mUnk_288[32];      // 1-bit trees (two entries per state): repeat match or new match
    /* 2c8 */ u16 mUnk_2c8[32][16];  // 4-bit trees: match length, by length state
    /* 6c8 */ u16 mUnk_6c8[256];     // 8-bit tree: long match length
    /* 8c8 */ u16 mUnk_8c8[32][16];  // 4-bit trees: repeat match length, by length state
    /* cc8 */ u16 mUnk_cc8[256];     // 8-bit tree: long repeat match length
    /* ec8 */ u16 mUnk_ec8[16][4];   // 2-bit trees: which of the four last distances, by state
    /* f48 */ u16 mUnk_f48[32];      // 1-bit trees (two entries per state): literal or match
    /* f88 */ u16 mUnk_f88[3][256];  // 8-bit trees: literal bytes (plain, then matched against a 0 or a 1 bit)
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

static s32 func_0203076c(const u8 *src);
static u32 func_02030794(const u8 **src);

// Direct (equiprobable) bits after each distance slot, and the slot's base distance.
static u8 data_02063ab8[32] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12,
};
static u16 data_02063ad8[32] = {
    0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x0009, 0x000b,
    0x000f, 0x0017, 0x0027, 0x0047, 0x0087, 0x0107, 0x0207, 0x0407, 0x0807, 0x1007, 0x1807,
    0x2007, 0x2807, 0x3007, 0x3807, 0x4007, 0x4807, 0x5007, 0x5807, 0x6007, 0x6807,
};
// The low six distance bits come out of their tree reversed; this puts them back in order.
static u8 data_02063b18[64] = {
    0x00, 0x20, 0x10, 0x30, 0x08, 0x28, 0x18, 0x38, 0x04, 0x24, 0x14, 0x34, 0x0c, 0x2c, 0x1c, 0x3c,
    0x02, 0x22, 0x12, 0x32, 0x0a, 0x2a, 0x1a, 0x3a, 0x06, 0x26, 0x16, 0x36, 0x0e, 0x2e, 0x1e, 0x3e,
    0x01, 0x21, 0x11, 0x31, 0x09, 0x29, 0x19, 0x39, 0x05, 0x25, 0x15, 0x35, 0x0d, 0x2d, 0x1d, 0x3d,
    0x03, 0x23, 0x13, 0x33, 0x0b, 0x2b, 0x1b, 0x3b, 0x07, 0x27, 0x17, 0x37, 0x0f, 0x2f, 0x1f, 0x3f,
};

// Unpacks `size` bytes of range-coded LZ data from `src` to `dst`; `p` is the probability work area.
static void func_0202fcc4(QrkProbs *p, u8 *dst, const u8 *src, u32 size) {
    u8 *end;
    u32 matchByte;
    u32 lenState;
    u32 litState;
    u32 state;
    u32 reps[4];
    u32 dist;
    u32 bound;
    u32 i;
    s32 len;
    s32 k;
    u8 slot;
    u32 sym;
    u32 code;
    u32 tmp; // the literal/match bit, then the matched byte's bit, then a probability step
    s32 nbits;
    u32 uprob;
    u32 idx;
    u32 bit;
    u16 *pp;
    u32 direct;
    u8 matching;
    u32 range;
    s32 prob;
    u16 *probs;

    end = dst + size;
    for (i = 0; i < sizeof(p->mUnk_000) / 4; i++) {
        ((u32 *) p->mUnk_000)[i] = (QRK_PROB_INIT << 16) | QRK_PROB_INIT;
    }
    for (i = 0; i < sizeof(p->mUnk_200) / 4; i++) {
        ((u32 *) p->mUnk_200)[i] = (QRK_PROB_INIT << 16) | QRK_PROB_INIT;
    }
    for (i = 0; i < sizeof(p->mUnk_280) / 4; i++) {
        ((u32 *) p->mUnk_280)[i] = (QRK_PROB_INIT << 16) | QRK_PROB_INIT;
    }
    for (i = 0; i < sizeof(p->mUnk_288) / 4; i++) {
        ((u32 *) p->mUnk_288)[i] = (QRK_PROB_INIT << 16) | QRK_PROB_INIT;
    }
    for (i = 0; i < sizeof(p->mUnk_2c8) / 4; i++) {
        ((u32 *) p->mUnk_2c8)[i] = (QRK_PROB_INIT << 16) | QRK_PROB_INIT;
    }
    for (i = 0; i < sizeof(p->mUnk_6c8) / 4; i++) {
        ((u32 *) p->mUnk_6c8)[i] = (QRK_PROB_INIT << 16) | QRK_PROB_INIT;
    }
    for (i = 0; i < sizeof(p->mUnk_8c8) / 4; i++) {
        ((u32 *) p->mUnk_8c8)[i] = (QRK_PROB_INIT << 16) | QRK_PROB_INIT;
    }
    for (i = 0; i < sizeof(p->mUnk_cc8) / 4; i++) {
        ((u32 *) p->mUnk_cc8)[i] = (QRK_PROB_INIT << 16) | QRK_PROB_INIT;
    }
    for (i = 0; i < sizeof(p->mUnk_ec8) / 4; i++) {
        ((u32 *) p->mUnk_ec8)[i] = (QRK_PROB_INIT << 16) | QRK_PROB_INIT;
    }
    for (i = 0; i < sizeof(p->mUnk_f48) / 4; i++) {
        ((u32 *) p->mUnk_f48)[i] = (QRK_PROB_INIT << 16) | QRK_PROB_INIT;
    }
    for (i = 0; i < sizeof(p->mUnk_f88) / 4; i++) {
        ((u32 *) p->mUnk_f88)[i] = (QRK_PROB_INIT << 16) | QRK_PROB_INIT;
    }

    code = (((((src[0] << 8) + src[1]) << 8) + src[2]) << 8) + src[3];
    range = 0xffffffff;
    state = 0xf;
    lenState = 0x1f;
    for (k = 0; k < 4; k++) {
        reps[k] = 1;
    }
    src += 4;

    // The first byte is a plain literal.
    sym = 1;
    for (k = 7; k >= 0; k--) {
        pp = &p->mUnk_f88[0][sym];
        uprob = *pp;
        bound = (range >> QRK_PROB_BITS) * uprob;
        if (code < bound) {
            range = bound;
            bit = 0;
        } else {
            range -= bound;
            bit = 1;
            code -= bound;
        }
        while (range < QRK_RANGE_TOP) {
            code = (code << 8) + *src++;
            range <<= 8;
        }
        sym <<= 1;
        if (!bit) {
            *pp += (u16) ((QRK_PROB_ONE - uprob) >> QRK_MOVE_BITS);
        } else {
            sym++;
            *pp -= uprob >> QRK_MOVE_BITS;
        }
    }
    matchByte = (u8) sym;
    *dst++ = matchByte;
    if (dst >= end) {
        return;
    }

    do {
        probs = &p->mUnk_f48[state * 2];
        sym = 1;
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
        tmp = sym & 1;
        if (tmp) {
            // A literal: right after a match (no literal since) its bits are coded against the byte that would
            // have continued the match, until the first bit that differs.
            litState = state & 3;
            if (litState == 0) {
                matching = 1;
                sym = 1;
                for (k = 7; k >= 0; k--) {
                    if (matching) {
                        tmp = (matchByte >> k) & 1;
                        idx = tmp + 1;
                    } else {
                        idx = 0;
                    }
                    pp = &p->mUnk_f88[0][(idx << 8) + sym];
                    uprob = *pp;
                    bound = (range >> QRK_PROB_BITS) * uprob;
                    if (code < bound) {
                        range = bound;
                        bit = 0;
                    } else {
                        range -= bound;
                        bit = 1;
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
                sym = 1;
                for (k = 7; k >= 0; k--) {
                    pp = &p->mUnk_f88[0][sym];
                    uprob = *pp;
                    bound = (range >> QRK_PROB_BITS) * uprob;
                    if (code < bound) {
                        range = bound;
                        bit = 0;
                    } else {
                        range -= bound;
                        bit = 1;
                        code -= bound;
                    }
                    while (range < QRK_RANGE_TOP) {
                        code = (code << 8) + *src++;
                        range <<= 8;
                    }
                    sym <<= 1;
                    if (!bit) {
                        *pp += (u16) ((QRK_PROB_ONE - uprob) >> QRK_MOVE_BITS);
                    } else {
                        sym++;
                        *pp -= uprob >> QRK_MOVE_BITS;
                    }
                }
                matchByte = (u8) sym;
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
        probs = &p->mUnk_288[state * 2];
        sym = 1;
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
            probs = p->mUnk_2c8[lenState];
            sym = 1;
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
                probs = p->mUnk_6c8;
                sym = 1;
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
                probs = p->mUnk_280;
                sym = 1;
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
                probs = p->mUnk_000[k];
                sym = 1;
                for (k = 4; k >= 0; k--) {
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
                slot = sym & 0x1f;
                nbits = data_02063ab8[slot];
                dist = data_02063ad8[slot];
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
                probs = p->mUnk_200;
                sym = 1;
                for (k = 0; k < 6; k++) {
                    prob = probs[sym];
                    bound = (range >> QRK_PROB_BITS) * prob;
                    if (code < bound) {
                        range = bound;
                        probs[sym] = prob + ((QRK_PROB_ONE - prob) >> QRK_MOVE_BITS);
                        sym = sym << 1;
                    } else {
                        probs[sym] = prob - (prob >> QRK_MOVE_BITS);
                        range -= bound;
                        code -= bound;
                        sym = (sym << 1) + 1;
                    }
                    while (range < QRK_RANGE_TOP) {
                        code = (code << 8) + *src++;
                        range <<= 8;
                    }
                }
                dist = data_02063b18[sym - 64] + (dist << 6);
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
            probs = p->mUnk_8c8[lenState];
            sym = 1;
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
                probs = p->mUnk_cc8;
                sym = 1;
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
            probs = p->mUnk_ec8[state];
            sym = 1;
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

// Unpacks the "QRK" data at `src` to `dst` with the work area `work` (func_02030760() bytes).
void func_02030700(const u8 *src, u8 *dst, void *work) {
    u32 size = func_02030794(&src);
    if (size == 0) {
        return;
    }
    func_0202fcc4(work, dst, src, size);
}

// Returns the unpacked size of the "QRK" data at `src` (0 if it isn't QRK_VERSION data).
u32 func_02030744(const u8 *src) {
    return func_02030794(&src);
}

u32 func_02030760(void) {
    return sizeof(QrkProbs);
}

// Returns the format version of the "QRK" data at `src`, or -1 when the tag is missing.
static s32 func_0203076c(const u8 *src) {
    if (src[0] != 'Q' || src[1] != 'R' || src[2] != 'K') {
        return -1;
    }
    return src[3] >> 5;
}

// Reads the header at `*src`: returns the unpacked size (0 for other data) and moves `*src` to the packed data.
// The size has as many extra bytes as the first byte has leading 1 bits; each extra byte also adds 1 << (7 * n).
static u32 func_02030794(const u8 **src) {
    u32 value = 0;
    u32 extra = 0;
    s32 n = 1;
    u32 mask = 0x80;
    u32 b;
    u32 c;
    if (func_0203076c(*src) != QRK_VERSION) {
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
