#include <Main/Unknown/map_entries.h>
#include <Main/Unknown/model.h>
#include <Main/Unknown/random_angle.h>
#include <Main/Unknown/screen_fade.h>
#include <Main/Unknown/system_init.h>
#include <Main/Unknown/text_draw.h>
#include <Main/Unknown/texture_blocks.h>
#include <nitro/mi.h>
#include <nitro/os.h>
#include <Main/Unknown/bg_image.h>
#include <Main/Unknown/game_math.h>
#include <Main/clib.h>
#include <Main/Unknown/script_records.h>
#include <Main/Unknown/wave_player.h>
#include <Main/Unknown/tracker_player.h>
#include <Main/Unknown/touch_buttons.h>

extern const u16 data_0204e208[];
extern const u16 data_0204e20c[];
extern const u16 data_0204e210[];
extern const u32 data_0204e214[];
extern const u16 data_0204e2c8[];

// Finds the entry of the message block whose normalised text equals name and selects it.
void func_02028e5c(const char *name) {
    char first[0x20];
    char second[0x20];
    int i = 2;
    func_0202938c(first, name);
    for (;; i++) {
        if (func_02021e08(data_020c61ac.mUnk_0e, i) == 4) {
            const u8 *s = func_02021e18(data_020c61ac.mUnk_0e, i);
            if (s[0] != 0) {
                func_0202938c(second, s);
                if (func_020290ac(first, second) == 1) {
                    func_02028eb4(i);
                    return;
                }
            }
        }
    }
}

// Resets the record state and marks every slot as unused.
void func_02028eb4(int index) {
    u32 i;
    func_0202a194();
    data_020c61ac.mUnk_10 = index;
    data_020c61ac.mUnk_16 = 0;
    data_020c61ac.mUnk_280.mUnk_38 = -1;
    data_020c61ac.mUnk_280.mUnk_34 = -1;
    data_020c61ac.mUnk_280.mUnk_3c = 0;
    data_020c61ac.mUnk_1e = 0;
    data_020c61ac.mUnk_2a = 0;
    data_020c61ac.mUnk_1b = 0;
    data_020c61ac.mUnk_1a = 0;
    data_020c61ac.mUnk_1c_3 = 0;
    data_020c61ac.mUnk_1c_2 = 0;
    data_020c61ac.mUnk_280.mUnk_14[0] = data_020c61ac.mUnk_280.mUnk_14[1] = 0;
    data_020c61ac.mUnk_280.mUnk_1c = 0;
    data_020c61ac.mUnk_18 = 0;
    for (i = 0; i < 0x20; i++) {
        data_020c61ac.mUnk_5c[i].mUnk_12 = -1;
    }
}

// Sets up the record store: the ids and buffers, or releases the old buffers when the ids change.
void func_02028f0c(int a, int b, int c, int d, int e, int f, void (*g)(u32 id, int x, int y, int w, int mode),
                   u8 (*h)(void), u32 (*i)(u32)) {
    data_020c61ac.mUnk_280.mUnk_28 = h;
    data_020c61ac.mUnk_04 = i;
    if ((a != data_020c61ac.mUnk_08 && a != 0) || (b != data_020c61ac.mUnk_0a && b != 0)) {
        func_02028fe8();
    }
    if (data_020c61ac.mUnk_08 == 0) {
        if (a == 0) {
            OS_Terminate();
        }
        if (b == 0) {
            OS_Terminate();
        }
        data_020c61ac.mUnk_08 = a;
        data_020c61ac.mUnk_0a = b;
        data_020c61ac.mUnk_0c = c;
        data_020c61ac.mUnk_0e = d;
        data_020c61ac.mUnk_32 = 0;
        data_020c61ac.mUnk_2c = func_0201c430(0x800);
        data_020c61ac.mUnk_58 = func_0201c430(0x200);
        data_020c61ac.mUnk_5c = func_0201c430(0x2c0);
        data_020c61ac.mUnk_60 = func_0201c430(0x480);
        data_020c61ac.mUnk_64 = func_0201c430(0x120);
        data_020c61ac.mUnk_68 = func_0201c430(0x48);
        data_020c61ac.mUnk_6c = func_0201c430(0x48);
        data_020c61ac.mUnk_28 = f;
    }
    data_020c61ac.mUnk_280.mUnk_2c = data_020c61ac.mUnk_280.mUnk_2e = data_020c61ac.mUnk_280.mUnk_30 = 0;
    data_020c61ac.mUnk_14 = 2;
    data_020c61ac.mUnk_24 = 0;
    data_020c61ac.mUnk_26 = 0;
    data_020c61ac.mUnk_00 = g;
    func_02028eb4(e);
    func_0202a1ec(0);
}

// Releases the record store buffers.
void func_02028fe8(void) {
    data_020c61ac.mUnk_08 = 0;
    func_0201c4f0(&data_020c61ac.mUnk_2c);
    func_0201c4f0(&data_020c61ac.mUnk_5c);
    func_0201c4f0(&data_020c61ac.mUnk_58);
    func_0201c4f0(&data_020c61ac.mUnk_60);
    func_0201c4f0(&data_020c61ac.mUnk_64);
    func_0201c4f0(&data_020c61ac.mUnk_68);
    func_0201c4f0(&data_020c61ac.mUnk_6c);
}

// Upper-cases a-z.
u8 func_0202903c(u8 c) {
    if (c >= 'a' && c <= 'z') {
        c &= 0x5f;
    }
    return c;
}

// Letter or underscore.
int func_02029050(u8 c) {
    c = func_0202903c(c);
    if ((c >= 'A' && c <= 'Z') || c == '_') {
        return 1;
    }
    return 0;
}

// Digit or sign.
int func_0202906c(u8 c) {
    if ((c >= '0' && c <= '9') || c == '-' || c == '+') {
        return 1;
    }
    return 0;
}

// Sign character.
int func_02029084(u8 c) {
    if (c == '-' || c == '+') {
        return 1;
    }
    return 0;
}

// Neither a sign nor a space.
int func_02029094(u8 c) {
    if (func_02029084(c) == 0 && c != ' ') {
        return 1;
    }
    return 0;
}

// Equality of two NUL-terminated strings.
int func_020290ac(const char *x, const char *y) {
    const u8 *a = (const u8 *) x;
    const u8 *b = (const u8 *) y;
    while (*a == *b) {
        if (*a == 0) {
            return 1;
        }
        a++;
        b++;
    }
    return 0;
}

// Copies the next identifier of *src, upper-cased, to *dst and advances both.
void func_020290cc(u8 **src, u8 **dst) {
    u8 *p = *src;
    u8 *q = *dst;
    while (func_02029050(*p) == 0) {
        p++;
    }
    while (func_02029050(*p) == 1) {
        *q++ = func_0202903c(*p++);
    }
    *q = 0;
    *dst = q;
    *src = p;
}

// Stores a 16-bit value little-endian at *out and advances *out.
void func_02029118(s16 value, u8 **out) {
    u8 *p = *out;
    p[0] = value;
    p[1] = value >> 8;
    *out = p + 2;
}

// Reads the next signed decimal number of *text; 0x55aa when there is none.
s16 func_02029128(u8 **text) {
    u8 *p = *text;
    s16 sign = 1;
    s16 value = 0;
    int found = 0;
    while (func_0202906c(*p) == 0) {
        if (*p == 0) {
            return 0x55aa;
        }
        p++;
    }
    while (func_0202906c(*p) == 1) {
        int c = *p;
        if (c == '+') {
        } else if (c == '-') {
            sign = -sign;
        } else {
            found = 1;
            value = c - '0' + value * 10;
        }
        p++;
    }
    *text = p;
    if (found == 1) {
        return sign * value;
    }
    return 0x55aa;
}

// Looks up the record named name in the record buffer.
const u8 *func_020291a4(const u8 *name) {
    const u8 *rec = data_020c61ac.mUnk_2c;
    const u8 *p;
    const u8 *q;
    while (*rec != 0) {
        for (p = rec + 1, q = name;
             *q == *p && func_02029050(*q) != 0 && func_02029050(*q) == 1 && func_02029050(*p) == 1; p++, q++) {
        }
        if (func_02029050(*q) == 0 && func_02029050(*p) == 0) {
            return rec;
        }
        rec += *rec;
    }
    return NULL;
}

// Reads the values of a record: a count byte, then that many little-endian 16-bit values.
void func_0202920c(const u8 *p, u16 *values, u32 *count) {
    u8 n;
    p++;
    while (func_02029050(*p) == 1) {
        p++;
    }
    n = *p++;
    *count = n;
    if (n != 0) {
        do {
            *values++ = p[0] + (p[1] << 8);
            p += 2;
            n--;
        } while (n != 0);
    }
}

// Names of the script commands, in command order, padded to 6 characters.
static const u8 data_020508e0[25][6] = {
    "LET   ", "SPR   ", "SPRX  ", "SPRSCR", "SHPSCR", "SHAPE ", "TXT   ", "EOF   ", "TXTSPD",
    "ESC   ", "SYNC  ", "GOTO  ", "SCREEN", "IF    ", "EQ\t  ", "NE    ", "GT\t  ", "LT\t  ",
    "MUSIC ", "SOUND ", "BRA   ", "FADE  ", "SETEYE", "SETLS ", "SETLSC",
};

// Entry kind for each value of the fifth field of a list entry.
static const u8 data_020508dc[4] = {0, 2, 1, 0};

// Adds (or, for '-', subtracts) count 16-bit values of src to dst.
void func_02029250(const s16 *src, s16 *dst, int count, int op) {
    while (count-- != 0) {
        switch (op) {
            case '+':
                *dst++ += *src++;
                break;
            case '-':
                *dst++ -= *src++;
                break;
        }
    }
}

// Finds the command whose name starts the text, advances the text past it and returns its index.
int func_02029298(u8 **text) {
    u32 k;
    u8 *start = *text;
    const u8 *name = (const u8 *) data_020508e0;
    u32 i = 0;
    do {
        u8 c;
        k = 0;
        c = func_0202903c(start[0]);
        if (c == name[0]) {
            for (;;) {
                u8 d;
                u8 *p;
                k++;
                p = start + k;
                d = func_0202903c(*p);
                if (d == 0 || d == ' ') {
                    *text = p;
                    return i;
                }
                if (d != name[k]) {
                    break;
                }
            }
        }
        name += 6;
        i++;
    } while (i < 25);
    if (i >= 25) {
        OS_Terminate();
    }
    return 25;
}

// Stores the record for the text: its name, the count of values and the values themselves.
void func_020292f4(const char *text) {
    u8 name[0x28];
    u8 *first;
    u8 *rest;
    u8 *dst;
    u8 *values;
    u8 *rec;
    u32 oldLen;
    u32 count = 0;
    rest = (u8 *) text;
    first = (u8 *) text;
    dst = name;
    func_020290cc(&first, &dst);
    rec = (u8 *) func_020291a4(name);
    if (rec == 0) {
        oldLen = 0;
        rec = data_020c61ac.mUnk_2c;
        while (*rec != 0) {
            rec += *rec;
        }
    } else {
        oldLen = *rec;
    }
    dst = rec + 1;
    func_020290cc(&rest, &dst);
    values = dst + 1;
    for (;;) {
        s16 v = func_02029128(&rest);
        if (v == 0x55aa) {
            break;
        }
        func_02029118(v, &values);
        count++;
    }
    if ((u32) (values - data_020c61ac.mUnk_2c) >= 0x800) {
        OS_Terminate();
    }
    *rec = values - rec;
    *dst = count;
    if (oldLen != 0 && *rec != oldLen) {
        OS_Terminate();
    }
}

// Copies the first word of src, upper-cased, to dst; returns the end of the copy.
char *func_0202938c(char *dst, const char *src) {
    u8 c;
    do {
        c = *src++;
        if (c == 0) {
            OS_Terminate();
        }
    } while (c == ' ');
    while (c != ' ' && c != 0) {
        *dst++ = func_0202903c(c);
        c = *src++;
    }
    *dst = 0;
    return dst;
}

// Clears the output and parses the text into it; returns the number of values.
int func_020293c4(s16 *out, u8 *text, u32 size) {
    MIi_CpuClear16(0, out, size);
    return func_020293dc(out, text);
}

// Evaluates the text: names of records and numbers, joined by signs, into out; returns the number of values.
int func_020293dc(s16 *out, u8 *text) {
    s16 buf[0x10];
    u8 name[0x10];
    u32 count;
    u8 *dst;
    const u8 *rec;
    int n = 0;
    int have = 0;
    int off;
    u8 op = '+';
    MIi_CpuClear16(0, buf, 0x20);
    while (*text != 0) {
        if (func_02029094(*text) == 1) {
            have = 0;
            op = '+';
        }
        if (func_02029050(*text) == 1) {
            dst = name;
            func_020290cc(&text, &dst);
            rec = func_020291a4(name);
            // rec is compared with 0 twice (two compares in the code).
            if (rec == 0 && rec == 0) {
                OS_Terminate();
            }
            off = n;
            func_0202920c(rec, buf + off, &count);
            func_02029250(buf + off, out + off, count, op);
            have = 1;
            n += count;
        } else if (func_02029084(*text) == 1 && have == 1) {
            op = *text++;
            n -= count;
        } else if (func_0202906c(*text) == 1) {
            off = n;
            buf[off] = func_02029128(&text);
            func_02029250(buf + off, out + off, 1, op);
            count = 1;
            n++;
            have = 1;
        } else {
            text++;
        }
    }
    return n;
}

// Reads a command from the text and queues it in the next command slot.
void func_020294c4(u8 *text, int mode) {
    s16 buf[12];
    TexInfo info;
    s32 rect[4];
    Unk_020c61ac_Cmd *cmd;
    u32 index;
    int n = func_020293c4(buf, text, 0x18);
    u32 tableValue;
    u32 shiftX;
    u32 shiftY;
    if (n < 4) {
        OS_Terminate();
    }
    if (n > 12) {
        OS_Terminate();
    }
    if (data_020c61ac.mUnk_16 >= 0x20) {
        OS_Terminate();
    }
    func_0202a0dc(data_020c61ac.mUnk_16, buf[0]);
    index = data_020c61ac.mUnk_16++;
    cmd = data_020c61ac.mUnk_60 + index;
    cmd->mUnk_00 = buf[0];
    if (mode == 0) {
        cmd->mUnk_04 = data_0204e2c8[buf[0]];
        if (buf[4] & 8) {
            cmd->mUnk_22 = 1;
        } else {
            cmd->mUnk_22 = 2;
        }
        func_0202ae84(cmd->mUnk_04, &info, 0);
        // Both arms clear bit 4 of mUnk_21: one through the bit-field, one as a byte operation (the code keeps two
        // separate stores).
        if (info.mUnk_12 > 1) {
            cmd->mUnk_21_4 = 0;
        } else {
            cmd->mUnk_21 &= ~0x10;
        }
    } else {
        tableValue = data_0204e214[buf[0]];
        cmd->mUnk_04 = 0x80000000 | (u16) tableValue;
        shiftX = 1 << (tableValue >> 24);
        shiftY = 1 << (u8) (tableValue >> 16);
        if (cmd->mUnk_1a >= 0x100) {
            OS_Terminate();
        }
        cmd->mUnk_21_4 = 0;
        cmd->mUnk_22 = 1;
    }
    cmd->mUnk_08 = buf[1];
    cmd->mUnk_0a = buf[2];
    cmd->mUnk_1a = buf[3];
    cmd->mUnk_16 = buf[5];
    cmd->mUnk_18 = buf[6];
    cmd->mUnk_1d = buf[7];
    cmd->mUnk_1c = cmd->mUnk_1d;
    cmd->mUnk_1e = buf[8];
    cmd->mUnk_1f = 0;
    cmd->mUnk_0c = buf[9] * 100;
    cmd->mUnk_0e = buf[10] * 100;
    cmd->mUnk_10 = buf[11];
    if (buf[4] & 0x10) {
        cmd->mUnk_21_1 = 1;
    } else {
        cmd->mUnk_21_1 = 0;
    }
    if (cmd->mUnk_10 != 0) {
        cmd->mUnk_08 = cmd->mUnk_08 * 100;
        cmd->mUnk_0a = cmd->mUnk_0a * 100;
        cmd->mUnk_12 = cmd->mUnk_08;
        cmd->mUnk_14 = cmd->mUnk_0a;
        cmd->mUnk_21_0 = 1;
    }
    if (buf[4] & 0x20) {
        cmd->mUnk_21_2 = 1;
    } else {
        cmd->mUnk_21_2 = 0;
    }
    if (buf[4] & 0x40) {
        cmd->mUnk_21_3 = 1;
    } else {
        cmd->mUnk_21_3 = 0;
    }
    if (!(buf[4] & 4) && cmd->mUnk_10 == 0) {
        cmd->mUnk_20 = 0x80;
        cmd->mUnk_12 = cmd->mUnk_08;
        cmd->mUnk_14 = cmd->mUnk_0a;
        cmd->mUnk_21_0 = 0;
        return;
    }
    if (mode == 0) {
        func_0202d828(cmd->mUnk_04, 0, rect);
        if (buf[4] & 1) {
            cmd->mUnk_14 = cmd->mUnk_0a;
            if (buf[4] & 2) {
                s32 v;
                if (cmd->mUnk_04 & 0x10000) {
                    v = -rect[0];
                } else {
                    v = rect[2];
                }
                cmd->mUnk_12 = -v;
            } else {
                s32 v;
                if (cmd->mUnk_04 & 0x10000) {
                    v = rect[2];
                } else {
                    v = -rect[0];
                }
                cmd->mUnk_12 = v + 0x100;
            }
        } else {
            cmd->mUnk_12 = cmd->mUnk_08;
            if (buf[4] & 2) {
                cmd->mUnk_14 = 0xc0 - rect[1];
            } else {
                cmd->mUnk_14 = -rect[3];
            }
        }
    } else {
        if (buf[4] & 1) {
            cmd->mUnk_14 = cmd->mUnk_0a;
            if (buf[4] & 2) {
                cmd->mUnk_12 = -shiftX;
            } else {
                cmd->mUnk_12 = 0x100;
            }
        } else {
            cmd->mUnk_12 = cmd->mUnk_08;
            if (buf[4] & 2) {
                cmd->mUnk_14 = 0xc0;
            } else {
                cmd->mUnk_14 = -shiftY;
            }
        }
    }
    cmd->mUnk_20 = 0;
}

// Reads an entry of the lists from the text and appends it to the list the text selects.
void func_020297c8(u8 *text) {
    s16 buf[8];
    Unk_020c61ac_Entry *entry;
    u32 n = func_020293c4(buf, text, 0x10);
    if (n - 6 > 2) {
        OS_Terminate();
    }
    if (buf[6] == 0) {
        entry = data_020c61ac.mUnk_64 + data_020c61ac.mUnk_1e++;
        if (data_020c61ac.mUnk_1e >= 0x10) {
            data_020c61ac.mUnk_1e = 0xf;
        }
    } else {
        entry = data_020c61ac.mUnk_68 + data_020c61ac.mUnk_24++;
        if (data_020c61ac.mUnk_24 >= 4) {
            data_020c61ac.mUnk_24 = 3;
        }
    }
    entry->mUnk_00 = data_020c61ac.mUnk_12++;
    entry->mUnk_02 = buf[0];
    entry->mUnk_04 = buf[1];
    entry->mUnk_06 = buf[2];
    entry->mUnk_08 = buf[3];
    entry->mUnk_0a = data_020508dc[buf[4]];
    entry->mUnk_0e = buf[5];
    if (buf[7] == 0) {
        entry->mUnk_11 = 2;
    } else {
        entry->mUnk_11 = 1;
    }
    entry->mUnk_0c =
        func_02021f0c(data_020c61ac.mUnk_0a, entry->mUnk_00, (data_020c61ac.mUnk_280.mUnk_28 != 0 ? 2 : 0) + 1);
    entry->mUnk_10 = data_020c61ac.mUnk_20;
}

// Reads one value from the text into mUnk_20.
void func_02029898(u8 *text) {
    s16 value[1] = {3};
    if (func_020293dc(value, text) > 1) {
        OS_Terminate();
    }
    data_020c61ac.mUnk_20 = value[0];
}

// Starts the listed ids (through the mUnk_04 callback when one is set) and empties the list.
void func_020298c8(void) {
    u32 i;
    data_020c61ac.mUnk_1b = 3;
    if (data_020c61ac.mUnk_32 > 0) {
        func_02035108(data_020c61ac.mUnk_32, 0, data_020c61ac.mUnk_34, 0, 1, 0);
        data_020c61ac.mUnk_30 = data_020c61ac.mUnk_34;
        data_020c61ac.mUnk_32 = 0;
    }
    for (i = 0; i < data_020c61ac.mUnk_56; i++) {
        u32 id;
        if (data_020c61ac.mUnk_04 == 0) {
            id = data_020c61ac.mUnk_36[i];
        } else {
            id = data_020c61ac.mUnk_04(data_020c61ac.mUnk_36[i]);
        }
        if (data_020c61ac.mUnk_4e[i] == 2) {
            func_0202edd0(id);
        } else if (!data_020c61ac.mUnk_1c_2) {
            func_0202eb64(id, data_020c61ac.mUnk_46[i], data_020c61ac.mUnk_4e[i], 0x40, 0xffff, 0);
        }
    }
    data_020c61ac.mUnk_56 = 0;
}

// Reads the flags of the next command and the values that go with them.
void func_02029970(u8 *text) {
    s16 buf[3];
    if (func_020293c4(buf, text, 6) > 3) {
        OS_Terminate();
    }
    data_020c61ac.mUnk_1a = buf[0];
    if (data_020c61ac.mUnk_1a & 8) {
        func_020298c8();
        data_020c61ac.mUnk_18 = buf[1];
    }
    data_020c18bc.mUnk_313 = 0;
    if (data_020c61ac.mUnk_1a & 0x10) {
        data_020c18bc.mUnk_314 = buf[2];
    } else {
        data_020c18bc.mUnk_314 = data_020c61ac.mUnk_14;
    }
}

// Reads up to 16 values from the text into the next entry of the event queue.
void func_020299d4(u8 *text) {
    int len = func_020293c4(&data_020c61ac.mUnk_70[data_020c61ac.mUnk_280.mUnk_10].mUnk_00, text, 0x20);
    if (len > 0x10) {
        OS_Terminate();
    }
    data_020c61ac.mUnk_70[data_020c61ac.mUnk_280.mUnk_10].mUnk_20 = len;
    data_020c61ac.mUnk_280.mUnk_10 = (data_020c61ac.mUnk_280.mUnk_10 + 1) & 0xf;
}

// Finds the entry of the message block (mUnk_0c) whose normalised text equals name and stores its index.
void func_02029a1c(const char *name) {
    char first[0x20];
    char second[0x20];
    int i;
    func_0202938c(first, name);
    for (i = 2;; i++) {
        if (func_02021e08(data_020c61ac.mUnk_0c, i) == 4) {
            const u8 *s = func_02021e18(data_020c61ac.mUnk_0c, i);
            if (s[0] != 0) {
                func_0202938c(second, s);
                if (func_020290ac(first, second) == 1) {
                    data_020c61ac.mUnk_12 = i;
                    return;
                }
            }
        }
    }
}

// Adds the two values read from the text to mUnk_10 and mUnk_12.
void func_02029a74(u8 *text) {
    s16 v[0x10];
    if (func_020293c4(v, text, 0x20) > 2) {
        OS_Terminate();
    }
    data_020c61ac.mUnk_10 += v[0];
    data_020c61ac.mUnk_12 += v[1];
}

// Reads an index and up to two more values; unless mUnk_280.mUnk_40 is set, mUnk_280.mUnk_1c becomes a table entry
// minus a top-bit marker chosen by flag, and the two values are stored after it.
void func_02029aac(u8 *text, int flag) {
    s16 v[3];
    int n;
    s32 x;
    s32 sel;
    v[1] = 0;
    v[2] = 0;
    n = func_020293c4(v, text, 6);
    if (n != 1 && n != 3) {
        OS_Terminate();
    }
    {
        Unk_020c642c *o = &data_020c61ac.mUnk_280;
        if (o->mUnk_40 == 0) {
            if (flag == 0) {
                x = data_0204e214[v[0]] & 0x7fff;
                sel = 2;
            } else {
                x = data_0204e2c8[v[0]];
                sel = 1;
            }
            o->mUnk_1c = x - (sel << 30);
            data_020c61ac.mUnk_280.mUnk_20 = v[1];
            data_020c61ac.mUnk_280.mUnk_22 = v[2];
        }
    }
}

// Reads a value and a mode; odd modes store the value in mUnk_280.mUnk_16[0] with the mode in mUnk_280.mUnk_14[0],
// even modes store it in mUnk_280.mUnk_16[1] and set mUnk_280.mUnk_14[1] to 2.
void func_02029b28(u8 *text) {
    s16 v[4];
    if (func_020293c4(v, text, 8) > 2) {
        OS_Terminate();
    }
    if (v[1] & 1) {
        data_020c61ac.mUnk_280.mUnk_16[0] = v[0];
        data_020c61ac.mUnk_280.mUnk_14[0] = v[1];
    } else {
        data_020c61ac.mUnk_280.mUnk_16[1] = v[0];
        data_020c61ac.mUnk_280.mUnk_14[1] = 2;
    }
}

// Reads the two values compared by the conditional commands.
void func_02029b6c(u8 *text) {
    s16 v[2];
    if (func_020293c4(v, text, 4) != 2) {
        OS_Terminate();
    }
    data_020c61ac.mUnk_280.mUnk_24 = v[0];
    data_020c61ac.mUnk_280.mUnk_26 = v[1];
}

// Adds the value to mUnk_10 when the two compared values are equal.
void func_02029b98(u8 *text) {
    s16 v[0x10];
    if (data_020c61ac.mUnk_280.mUnk_24 == data_020c61ac.mUnk_280.mUnk_26) {
        if (func_020293c4(v, text, 0x20) != 1) {
            OS_Terminate();
        }
        data_020c61ac.mUnk_10 += v[0];
    }
}

// Adds the value to mUnk_10 when the two compared values differ.
void func_02029bd8(u8 *text) {
    s16 v[0x10];
    if (data_020c61ac.mUnk_280.mUnk_24 != data_020c61ac.mUnk_280.mUnk_26) {
        if (func_020293c4(v, text, 0x20) != 1) {
            OS_Terminate();
        }
        data_020c61ac.mUnk_10 += v[0];
    }
}

// Adds the value to mUnk_10 when the first compared value is greater.
void func_02029c18(u8 *text) {
    s16 v[8];
    if (data_020c61ac.mUnk_280.mUnk_24 > data_020c61ac.mUnk_280.mUnk_26) {
        if (func_020293c4(v, text, 0x10) != 1) {
            OS_Terminate();
        }
        data_020c61ac.mUnk_10 += v[0];
    }
}

// Adds the value to mUnk_10 when the first compared value is smaller.
void func_02029c58(u8 *text) {
    s16 v[8];
    if (data_020c61ac.mUnk_280.mUnk_24 < data_020c61ac.mUnk_280.mUnk_26) {
        if (func_020293c4(v, text, 0x10) != 1) {
            OS_Terminate();
        }
        data_020c61ac.mUnk_10 += v[0];
    }
}

// Reads an index (and an optional second value, default 100); selects mUnk_32 through the remap table when it is
// enabled.
void func_02029c98(u8 *text) {
    s16 v[4] = {0, 0, 0, 0};
    int n = func_020293dc(v, text);
    if (n != 1 && n != 2) {
        OS_Terminate();
    }
    if (n == 1) {
        v[1] = 100;
    }
    if (data_0204e210[0] != 0) {
        data_020c61ac.mUnk_32 = data_0204e210[v[0]];
    } else {
        data_020c61ac.mUnk_32 = v[0];
    }
    data_020c61ac.mUnk_34 = v[1];
}

// Appends one entry (table value, second value, third value in 0..2) to the lists indexed by mUnk_56.
void func_02029d00(u8 *text) {
    s16 v[4] = {0, 0, 0, 0};
    u8 idx = data_020c61ac.mUnk_56++;
    int n = func_020293dc(v, text);
    int ok;
    if (n < 1 || n > 3) {
        OS_Terminate();
    }
    if (n == 1) {
        v[1] = 100;
    }
    ok = 0;
    if (v[2] >= 0 && v[2] <= 2) {
        ok = 1;
    }
    if (ok == 0) {
        OS_Terminate();
    }
    data_020c61ac.mUnk_36[idx] = data_0204e208[v[0]];
    data_020c61ac.mUnk_46[idx] = v[1];
    data_020c61ac.mUnk_4e[idx] = v[2];
}

// Selects the entry named by the text, then steps mUnk_10 back by one.
void func_02029d8c(const char *name) {
    func_02028e5c(name);
    data_020c61ac.mUnk_10--;
}

// Reads flags; bit 0 and bit 1 each trigger func_0201f880 with that bit.
void func_02029da0(u8 *text) {
    s16 v[1] = {0};
    if (func_020293dc(v, text) != 1) {
        OS_Terminate();
    }
    if (v[0] & 1) {
        func_0201f880(1);
    }
    if (v[0] & 2) {
        func_0201f880(2);
    }
}

// Parses a text command (slot number, two ids, a value and up to five timing values) and fills that slot,
// or marks it unused when only the slot number is given.
void func_02029dd8(u8 *text) {
    s16 v[9] = {0};
    Unk_020c61ac_Slot *slot;
    int n = func_020293dc(v, text);
    BOOL ok;
    if (n != 1 && (n < 4 || n >= 10)) {
        OS_Terminate();
    }
    ok = 0;
    if (v[0] >= 0 && v[0] < 0x20) {
        ok = 1;
    }
    if (ok == 0) {
        OS_Terminate();
    }
    slot = &data_020c61ac.mUnk_5c[v[0]];
    if (n > 1) {
        slot->mUnk_00 = 1;
        slot->mUnk_02 = v[1];
        slot->mUnk_04 = v[2];
        if (slot->mUnk_02 < 0 && slot->mUnk_04 < 0) {
            OS_Terminate();
        }
        if (slot->mUnk_02 < -1 || slot->mUnk_04 < -1) {
            OS_Terminate();
        }
        slot->mUnk_0c = v[3];
        if (slot->mUnk_0c < 0) {
            OS_Terminate();
        }
        slot->mUnk_06 = 0x3c;
        slot->mUnk_08 = 0x78;
        slot->mUnk_0a = 0x1e;
        slot->mUnk_0e = 0;
        slot->mUnk_10 = 0;
        if (n > 4) {
            slot->mUnk_06 = v[4];
            if (n > 5) {
                slot->mUnk_08 = v[5];
                if (n > 6) {
                    slot->mUnk_0a = v[6];
                    if (n > 7) {
                        slot->mUnk_0e = v[7];
                        if (n > 8) {
                            slot->mUnk_10 = v[8];
                        }
                    }
                }
            }
        }
    } else {
        slot->mUnk_00 = 0;
    }
}

// Looks up the active slot whose value equals id, returns its three values and steps its countdown; when the
// countdown is unset it draws a new one between the slot's two bounds.
void func_02029ed8(int id, int *a, int *b, int *c) {
    Unk_020c61ac_Slot *slot;
    u32 i;
    *a = -1;
    for (i = 0, slot = data_020c61ac.mUnk_5c; i < 0x20; i++, slot++) {
        if (slot->mUnk_00 == 1 && slot->mUnk_0c == id) {
            *b = slot->mUnk_0e;
            *c = slot->mUnk_10;
            *a = slot->mUnk_02;
            if (slot->mUnk_12 == -1) {
                u16 lo;
                slot->mUnk_14 = 0;
                lo = slot->mUnk_06;
                slot->mUnk_12 = lo + func_0201fd68() % (slot->mUnk_08 - lo);
                return;
            }
            if (slot->mUnk_14 == 1) {
                *a = slot->mUnk_04;
            }
            slot->mUnk_12--;
            if (slot->mUnk_12 == 0) {
                if (slot->mUnk_14 == 0) {
                    slot->mUnk_12 = slot->mUnk_0a;
                    slot->mUnk_14 = 1;
                    return;
                }
                slot->mUnk_12 = -1;
                return;
            }
            return;
        }
    }
}

// Parses a text command (slot number, a value and up to five more) and fills that small record,
// or marks it unused when only the slot number is given.
void func_02029f6c(u8 *text) {
    s16 v[8] = {0};
    Unk_020c61ac_Rec10 *rec;
    int n = func_020293dc(v, text);
    BOOL ok;
    if (n != 1 && (n < 3 || n >= 9)) {
        OS_Terminate();
    }
    ok = 0;
    if (v[0] >= 0 && v[0] < 0x20) {
        ok = 1;
    }
    if (ok == 0) {
        OS_Terminate();
    }
    rec = &data_020c61ac.mUnk_58[v[0]];
    if (n > 1) {
        rec->mUnk_00 = 1;
        rec->mUnk_02 = v[1];
        if (rec->mUnk_02 < -1) {
            OS_Terminate();
        }
        rec->mUnk_04 = v[2];
        rec->mUnk_06 = 2;
        rec->mUnk_08 = 2;
        rec->mUnk_0a = 0;
        rec->mUnk_0c = 0;
        rec->mUnk_0e = -1;
        if (n > 3) {
            rec->mUnk_06 = v[3];
            if (n > 4) {
                rec->mUnk_08 = v[4];
                if (n > 5) {
                    rec->mUnk_0a = v[5];
                    if (n > 6) {
                        rec->mUnk_0c = v[6];
                        if (n > 7) {
                            rec->mUnk_0e = v[7];
                        }
                    }
                }
            }
        }
    } else {
        rec->mUnk_00 = 0;
    }
}

// Parses a text command with no values or two (a record index and a second index) and selects them.
void func_0202a034(u8 *text) {
    s16 v[2] = {0};
    int n = func_020293dc(v, text);
    if (n != 0 && n != 2) {
        OS_Terminate();
    }
    if (n == 2) {
        func_0202a078(v[0], v[1]);
    } else {
        func_0202a078(-1, -1);
    }
}

// Stores the two selected indexes (each -1 or a slot number) and checks that the first one is in use.
void func_0202a078(int a, int b) {
    BOOL ok;
    data_020c61ac.mUnk_280.mUnk_34 = a;
    data_020c61ac.mUnk_280.mUnk_38 = b;
    ok = 0;
    if (data_020c61ac.mUnk_280.mUnk_34 >= -1 && data_020c61ac.mUnk_280.mUnk_34 < 0x20) {
        ok = 1;
    }
    if (ok == 0) {
        OS_Terminate();
    }
    ok = 0;
    if (data_020c61ac.mUnk_280.mUnk_38 >= -1 && data_020c61ac.mUnk_280.mUnk_38 < 0x20) {
        ok = 1;
    }
    if (ok == 0) {
        OS_Terminate();
    }
    if (data_020c61ac.mUnk_280.mUnk_34 != -1 && data_020c61ac.mUnk_280.mUnk_38 != -1) {
        if (data_020c61ac.mUnk_58[data_020c61ac.mUnk_280.mUnk_34].mUnk_00 != 1) {
            OS_Terminate();
        }
    }
}

// Selects the record whose value equals id together with the given index.
void func_0202a0dc(int index, int id) {
    Unk_020c61ac_Rec10 *rec;
    u32 i;
    for (i = 0, rec = data_020c61ac.mUnk_58; i < 0x20; i++, rec++) {
        if (rec->mUnk_00 == 1 && rec->mUnk_0e == id) {
            func_0202a078(i, index);
            return;
        }
    }
}

// Swaps the two value pairs of a record once, unless it is flagged as already swapped.
void func_0202a10c(Unk_020c61ac_Cmd *rec) {
    if (!rec->mUnk_21_0) {
        s16 a = rec->mUnk_08;
        rec->mUnk_08 = rec->mUnk_12;
        rec->mUnk_12 = a;
        a = rec->mUnk_0a;
        rec->mUnk_0a = rec->mUnk_14;
        rec->mUnk_14 = a;
        rec->mUnk_20 = 0;
    }
}

// Swaps the value pairs of every record in use.
void func_0202a138(void) {
    u32 i;
    for (i = 0; i < data_020c61ac.mUnk_16; i++) {
        func_0202a10c(&data_020c61ac.mUnk_60[i]);
    }
}

// Sets the state to 2 and, depending on the flags, resets the counter and swaps the record values.
void func_0202a160(void) {
    data_020c61ac.mUnk_1b = 2;
    if (!(data_020c61ac.mUnk_1a & 2)) {
        data_020c61ac.mUnk_1e = 0;
    }
    if (!(data_020c61ac.mUnk_1a & 1)) {
        func_0202a138();
    }
}

// Returns the state.
u8 func_0202a188(void) {
    return data_020c61ac.mUnk_1b;
}

// Empties the event queue.
void func_0202a194(void) {
    data_020c61ac.mUnk_280.mUnk_11 = 0;
    data_020c61ac.mUnk_280.mUnk_10 = 0;
}

// Takes the next event off the queue; returns 0 when it is empty.
Unk_EventRec *func_0202a1a4(void) {
    u8 read = data_020c61ac.mUnk_280.mUnk_11;
    if (read != data_020c61ac.mUnk_280.mUnk_10) {
        data_020c61ac.mUnk_280.mUnk_11 = (read + 1) & 0xf;
        return &data_020c61ac.mUnk_70[read];
    }
    return NULL;
}

// Resets one camera byte and steps another through 0..0x1f.
void func_0202a1cc(void) {
    func_020240ec();
    data_020c3e44.mUnk_2024.mUnk_2dc.mUnk_4b = 0;
    data_020c3e44.mUnk_2024.mUnk_2dc.mUnk_4c++;
    data_020c3e44.mUnk_2024.mUnk_2dc.mUnk_4c &= 0x1f;
}

// Runs one frame of the script engine: applies queued screen changes, moves and draws the command sprites, builds
// and draws the message text, then reads script commands until one waits; repeats while mUnk_1c_2 is set.
void func_0202a1ec(int (*callback)(void)) {
    char text[0x200];
    u8 name[0x80];
    TexInfo info;
    int dx;
    int dy;
    int id;
    u8 *line;
    BOOL flag;
    BOOL done;
    int i;
    u32 start;
    s32 flip;
    u8 saved;

    if (data_020c61ac.mUnk_280.mUnk_28 != 0) {
        data_020c18bc.mUnk_304 = (u32(*)(u32)) data_020c61ac.mUnk_280.mUnk_28;
    }
    do {
        done = 0;
        flag = 0;
        if (data_020c61ac.mUnk_08 == 0) {
            OS_Terminate();
        }
        data_020c61ac.mUnk_1c_0 = 0;
        for (i = 0; i < 2; i++) {
            if (data_020c61ac.mUnk_280.mUnk_14[i] != 0) {
                u16 image = data_0204e20c[data_020c61ac.mUnk_280.mUnk_16[i]];
                func_0201f880(data_020c61ac.mUnk_280.mUnk_14[i]);
                func_0201dde0(image, data_020c61ac.mUnk_280.mUnk_14[i]);
                if (data_020c61ac.mUnk_280.mUnk_14[i] == 1) {
                    data_020c61ac.mUnk_280.mUnk_1c = 0;
                    data_020c61ac.mUnk_280.mUnk_2c = image;
                    flag = 1;
                } else if (data_020c61ac.mUnk_280.mUnk_14[i] == 2) {
                    data_020c61ac.mUnk_280.mUnk_2e = image;
                } else if (data_020c61ac.mUnk_280.mUnk_14[i] == 3) {
                    flag = 1;
                    data_020c61ac.mUnk_280.mUnk_2c = data_020c61ac.mUnk_280.mUnk_2e = image;
                }
                data_020c61ac.mUnk_280.mUnk_14[i] = 0;
            }
            if (flag == 1 && (data_020c18bc.mUnk_46c & 0x80)) {
                func_02024208();
            }
        }

        if (data_020c61ac.mUnk_280.mUnk_1c != 0) {
            if (data_020c61ac.mUnk_280.mUnk_1c & 0x80000000) {
                data_020c61ac.mUnk_280.mUnk_1c &= 0x7fffffff;
                func_0201f880(1);
                data_020c61ac.mUnk_280.mUnk_30 = data_020c61ac.mUnk_280.mUnk_1c;
            }
            if (!(data_020c61ac.mUnk_280.mUnk_1c & 0x40000000)) {
                func_020242e8(data_020c61ac.mUnk_280.mUnk_1c, data_020c61ac.mUnk_280.mUnk_20,
                              data_020c61ac.mUnk_280.mUnk_22, 0x4000, 0x1000, 0x8000, 0xff, 0);
            } else {
                u32 tex = data_020c61ac.mUnk_280.mUnk_1c & 0x3fffffff;
                func_0202ae84(tex, &info, 0);
                if (info.mUnk_12 <= 1) {
                    OS_Terminate();
                }
                func_0202a1cc();
                func_0202cf08(tex + 0x800000, data_020c61ac.mUnk_280.mUnk_20, data_020c61ac.mUnk_280.mUnk_22, 0xff, 1);
            }
        }

        for (i = 0; i < data_020c61ac.mUnk_16; i++) {
            Unk_020c61ac_Cmd *cmd = &data_020c61ac.mUnk_60[i];
            int x;
            int y;
            flip = 0;
            if (cmd->mUnk_16 != 0) {
                data_020c61ac.mUnk_1c_0 = 1;
                cmd->mUnk_16--;
                continue;
            }
            if (cmd->mUnk_18 != 0) {
                data_020c61ac.mUnk_1c_0 = 1;
                cmd->mUnk_18--;
                if (cmd->mUnk_18 == 0) {
                    if (cmd->mUnk_12 == cmd->mUnk_08 && cmd->mUnk_14 == cmd->mUnk_0a) {
                        cmd->mUnk_04 = -1;
                    } else {
                        func_0202a10c(cmd);
                        cmd->mUnk_21_0 = 1;
                    }
                }
            }
            if (cmd->mUnk_1e != 0 && cmd->mUnk_04 != -1) {
                data_020c61ac.mUnk_1c_0 = 1;
                cmd->mUnk_1f++;
                if (cmd->mUnk_1f == cmd->mUnk_1e) {
                    cmd->mUnk_04++;
                    cmd->mUnk_1f = 0;
                    cmd->mUnk_1d--;
                    if (cmd->mUnk_1d == 0) {
                        if (cmd->mUnk_21_1 == 0) {
                            cmd->mUnk_04--;
                            cmd->mUnk_1e = 0;
                        } else {
                            data_020c61ac.mUnk_1c_0 = 0;
                            cmd->mUnk_04 -= cmd->mUnk_1c;
                            cmd->mUnk_1d = cmd->mUnk_1c;
                        }
                    }
                }
            }
            if (cmd->mUnk_10 != 0) {
                s16 to = cmd->mUnk_10 > 0 ? cmd->mUnk_0c : cmd->mUnk_12;
                if (cmd->mUnk_08 < to) {
                    cmd->mUnk_08 += abs(cmd->mUnk_10);
                    if (cmd->mUnk_08 >= to) {
                        cmd->mUnk_08 = to;
                        if (cmd->mUnk_21_2 == 1) {
                            cmd->mUnk_10 = -cmd->mUnk_10;
                        }
                    }
                } else if (cmd->mUnk_08 > to) {
                    cmd->mUnk_08 -= abs(cmd->mUnk_10);
                    if (cmd->mUnk_08 <= to) {
                        cmd->mUnk_08 = to;
                        if (cmd->mUnk_21_2 == 1) {
                            cmd->mUnk_10 = -cmd->mUnk_10;
                        }
                    }
                }
                to = cmd->mUnk_10 > 0 ? cmd->mUnk_0e : cmd->mUnk_14;
                if (cmd->mUnk_0a < to) {
                    cmd->mUnk_0a += abs(cmd->mUnk_10);
                    if (cmd->mUnk_0a >= to) {
                        cmd->mUnk_0a = to;
                        if (cmd->mUnk_21_2 == 1) {
                            cmd->mUnk_10 = -cmd->mUnk_10;
                        }
                    }
                } else if (cmd->mUnk_0a > to) {
                    cmd->mUnk_0a -= abs(cmd->mUnk_10);
                    if (cmd->mUnk_0a <= to) {
                        cmd->mUnk_0a = to;
                        if (cmd->mUnk_21_2 == 1) {
                            cmd->mUnk_10 = -cmd->mUnk_10;
                        }
                    }
                }
                if (cmd->mUnk_10 < 0 && cmd->mUnk_21_3 == 1) {
                    flip = 0x10000;
                }
                x = cmd->mUnk_08 / 100;
                y = cmd->mUnk_0a / 100;
            } else if (cmd->mUnk_20 < 0x80) {
                if (cmd->mUnk_21_4 == 0) {
                    cmd->mUnk_20 += 20;
                } else if (cmd->mUnk_21_4 > 1) {
                    cmd->mUnk_21_4--;
                } else if (data_020c18bc.mUnk_480 == 0) {
                    cmd->mUnk_21_4 = 0;
                }
                data_020c61ac.mUnk_1c_0 = 1;
                x = func_0201fb74(cmd->mUnk_20 < 0x80 ? cmd->mUnk_20 : 0x80, 0x80, cmd->mUnk_12, cmd->mUnk_08);
                y = func_0201fb74(cmd->mUnk_20 < 0x80 ? cmd->mUnk_20 : 0x80, 0x80, cmd->mUnk_14, cmd->mUnk_0a);
                if (x == cmd->mUnk_08 && y == cmd->mUnk_0a) {
                    cmd->mUnk_20 = 0x80;
                }
            } else {
                x = cmd->mUnk_08;
                y = cmd->mUnk_0a;
            }

            if (data_020c61ac.mUnk_1c_2 == 0) {
                if (!(cmd->mUnk_04 & 0x80000000)) {
                    if (data_020c61ac.mUnk_00 != 0) {
                        data_020c61ac.mUnk_00(cmd->mUnk_04 ^ flip, x, y, cmd->mUnk_1a, cmd->mUnk_22);
                    } else {
                        func_0202ae84(cmd->mUnk_04, &info, 0);
                        if (info.mUnk_12 > 1) {
                            func_0202a1cc();
                            func_0202cf08(cmd->mUnk_04 ^ (flip + 0x800000), x, y, cmd->mUnk_1a, cmd->mUnk_22);
                        } else {
                            func_0202cf08(flip ^ cmd->mUnk_04, x, y, cmd->mUnk_1a, cmd->mUnk_22);
                        }
                    }
                } else {
                    func_020242e8(cmd->mUnk_04 & 0x7fffffff, x, y, cmd->mUnk_1a << 6, 0x1000, 0x8000, 0xff, 0);
                }
            }

            func_02029ed8(cmd->mUnk_00, &id, &dx, &dy);
            if (id != -1) {
                id = data_0204e2c8[id];
                func_0202ae84(id, &info, 0);
                if (info.mUnk_12 > 1) {
                    func_0202a1cc();
                    func_0202cf08(id + flip + 0x800000, x + dx, y + dy, cmd->mUnk_1a - 2, cmd->mUnk_22);
                } else {
                    func_0202cf08(id + flip, x + dx, y + dy, cmd->mUnk_1a - 2, cmd->mUnk_22);
                }
            }

            if (data_020c61ac.mUnk_280.mUnk_34 != -1 && data_020c61ac.mUnk_280.mUnk_38 == i) {
                Unk_020c61ac_Rec10 *rec = &data_020c61ac.mUnk_58[data_020c61ac.mUnk_280.mUnk_34];
                id = -1;
                if (data_020c61ac.mUnk_280.mUnk_3c != 0) {
                    data_020c61ac.mUnk_280.mUnk_3c--;
                    // Always true: mUnk_04 is 16-bit.
                    if (rec->mUnk_04 != -1) {
                        id = data_0204e2c8[rec->mUnk_04] + (data_020c18bc.mUnk_45c / rec->mUnk_08) % rec->mUnk_06;
                    }
                } else if (rec->mUnk_02 != -1) {
                    id = data_0204e2c8[rec->mUnk_02];
                }
                if (id != -1) {
                    func_0202ae84(id, &info, 0);
                    if (info.mUnk_12 > 1) {
                        func_0202a1cc();
                        func_0202cf08(id + flip + 0x800000, x + rec->mUnk_0a, y + rec->mUnk_0c, cmd->mUnk_1a - 2,
                                      cmd->mUnk_22);
                    } else {
                        func_0202cf08(id + flip, x + rec->mUnk_0a, y + rec->mUnk_0c, cmd->mUnk_1a - 2, cmd->mUnk_22);
                    }
                }
            }
        }

        {
            u32 len;
            start = 0;
            len = 0;
            i = 0;
            for (; i < data_020c61ac.mUnk_1e; i++) {
                Unk_020c61ac_Entry *entry = &data_020c61ac.mUnk_64[i];
                const u8 *s;
                u16 pos;
                if (data_020c61ac.mUnk_280.mUnk_28 != 0) {
                    s = name;
                    func_02021e94(data_020c61ac.mUnk_0a, entry->mUnk_00, name, 1);
                } else {
                    s = func_02021e18(data_020c61ac.mUnk_0a, entry->mUnk_00);
                }
                pos = data_020c18bc.mUnk_30c;
                if (pos >= start && pos <= start + entry->mUnk_0c) {
                    data_020c61ac.mUnk_22++;
                    if (data_020c61ac.mUnk_22 > entry->mUnk_10 && data_020c61ac.mUnk_1c_3 == 1) {
                        data_020c61ac.mUnk_280.mUnk_3c = entry->mUnk_10 + 1;
                        data_020c18bc.mUnk_30c = pos + 1;
                        if (data_020c61ac.mUnk_28 != 0) {
                            func_0202eb64(data_020c61ac.mUnk_28, 100, 0, 0x40, 0xffff, 0);
                        }
                        data_020c61ac.mUnk_22 = 0;
                    }
                }
                start += entry->mUnk_0c;
                while (*s != 0) {
                    text[len++] = *s++;
                }
                if (len >= 0x200) {
                    OS_Terminate();
                }
            }
            text[len] = 0;

            if (data_020c61ac.mUnk_1c_2 == 0 && len != 0) {
                Unk_020c61ac_Entry *entry = data_020c61ac.mUnk_64;
                u8 value = 0;
                u32 extra = 0;
                if (callback != 0) {
                    value = callback();
                }
                func_0202ae84(data_02063988[entry->mUnk_0e]->mGlyph, &info, 0);
                if (info.mUnk_12 > 1) {
                    extra = TEXT_ROTATE;
                    saved = data_020c18bc.mUnk_30a[0];
                    data_020c18bc.mUnk_30a[0] = value;
                    func_0202a1cc();
                }
                func_02021624(entry->mUnk_02, entry->mUnk_04 + data_020c61ac.mUnk_2a, data_02063988,
                              (u8) entry->mUnk_0e, (const u8 *) text, 0,
                              extra + (entry->mUnk_0a + (TEXT_LINE_WINDOW | TEXT_LIMIT | TEXT_HILITE) +
                                       (data_020c61ac.mUnk_280.mUnk_28 != 0 ? TEXT_ESCAPES : 0)),
                              entry->mUnk_06, entry->mUnk_08, entry->mUnk_11);
                if (extra & TEXT_ROTATE) {
                    data_020c18bc.mUnk_30a[0] = saved;
                }
                data_020c61ac.mUnk_1c_3 = data_020c18bc.mUnk_317_0;
            }
        }

        if (data_020c61ac.mUnk_1b == 3) {
            return;
        }
        if (data_020c61ac.mUnk_1b == 1) {
            if (data_020c61ac.mUnk_1c_2 == 1 ||
                (data_020c61ac.mUnk_16 == 0 && data_020c61ac.mUnk_1e == 0 && data_020c61ac.mUnk_18 == 0)) {
                func_0202a160();
            } else {
                if (data_020c61ac.mUnk_1c_1 == 1 && data_020c61ac.mUnk_1c_3 == 1) {
                    data_020c18bc.mUnk_30c += 2;
                }
                if (data_020c61ac.mUnk_18 != 0 && --data_020c61ac.mUnk_18 == 0) {
                    func_0202a160();
                } else if (!(data_020c61ac.mUnk_1a & 8) &&
                           ((data_020c18bc.mUnk_10 & 1) ||
                            (data_020c2560.mUnk_100.mUnk_04 & 0x4000ffff) == 0x40000400)) {
                    if (data_020c61ac.mUnk_1c_3 == 1) {
                        if ((data_020c18bc.mUnk_12 & 1) ||
                            (data_020c2560.mUnk_100.mUnk_04 & 0xc000ffff) == 0xc0000400) {
                            data_020c61ac.mUnk_1c |= 2;
                        }
                    } else if ((data_020c18bc.mUnk_12 & 1) ||
                               (data_020c2560.mUnk_100.mUnk_04 & 0xc000ffff) == 0xc0000400) {
                        if (data_020c18bc.mUnk_313 + data_020c18bc.mUnk_314 < data_020c18bc.mUnk_315) {
                            data_020c18bc.mUnk_313 += data_020c18bc.mUnk_314;
                        } else {
                            func_0202a160();
                        }
                    }
                }
            }
        } else if (data_020c61ac.mUnk_1b == 2) {
            if (data_020c61ac.mUnk_1c_2 == 1 || data_020c61ac.mUnk_1c_0 == 0) {
                if (((data_020c3e44.mUnk_2024.mUnk_2dc.mUnk_0c != 0 && data_020c3e44.mUnk_2024.mUnk_2dc.mUnk_33 == 8 &&
                      (data_020c18bc.mUnk_46c & 0x80)) ||
                     (data_020c3e44.mUnk_2024.mUnk_2dc.mUnk_0e != 0 && (data_020c18bc.mUnk_46c & 0x80))) &&
                    !(data_020c18bc.mUnk_46c & 0x08000000)) {
                    continue;
                }
                if (!(data_020c61ac.mUnk_1a & 1)) {
                    data_020c61ac.mUnk_16 = 0;
                    data_020c61ac.mUnk_280.mUnk_34 = data_020c61ac.mUnk_280.mUnk_38 = -1;
                }
                if (data_020c61ac.mUnk_1a & 4) {
                    data_020c61ac.mUnk_1b = 3;
                    func_020298c8();
                } else {
                    data_020c61ac.mUnk_1b = 0;
                }
            }
        } else {
            data_020c61ac.mUnk_1c_1 = 0;
            do {
                int kind = func_02021e08(data_020c61ac.mUnk_08, data_020c61ac.mUnk_10);
                // kind is compared with 0 before the test for 4 (both compares are in the code).
                if (kind != 0 && kind == 4) {
                    line = (u8 *) func_02021e18(data_020c61ac.mUnk_08, data_020c61ac.mUnk_10);
                    switch (func_02029298(&line)) {
                        case 0:
                            func_020292f4((const char *) line);
                            break;
                        case 1:
                            func_020294c4(line, 0);
                            break;
                        case 2:
                            func_020294c4(line, 0);
                            data_020c61ac.mUnk_60[data_020c61ac.mUnk_16 - 1].mUnk_04 |= 0x10000;
                            break;
                        case 3:
                            func_02029aac(line, 1);
                            break;
                        case 4:
                            func_02029aac(line, 0);
                            break;
                        case 5:
                            func_020294c4(line, 1);
                            break;
                        case 6:
                            func_020297c8(line);
                            data_020c18bc.mUnk_30c = 0;
                            break;
                        case 8:
                            func_02029898(line);
                            break;
                        case 7:
                            func_02029970(line);
                            data_020c61ac.mUnk_1b = 1;
                            done = 1;
                            break;
                        case 9:
                            func_020299d4(line);
                            break;
                        case 10:
                            func_02029a1c((const char *) line);
                            break;
                        case 11:
                            func_02029a74(line);
                            break;
                        case 12:
                            func_02029b28(line);
                            break;
                        case 13:
                            func_02029b6c(line);
                            break;
                        case 14:
                            func_02029b98(line);
                            break;
                        case 15:
                            func_02029bd8(line);
                            break;
                        case 16:
                            func_02029c18(line);
                            break;
                        case 17:
                            func_02029c58(line);
                            break;
                        case 18:
                            func_02029c98(line);
                            break;
                        case 19:
                            func_02029d00(line);
                            break;
                        case 20:
                            func_02029d8c((const char *) line);
                            break;
                        case 21:
                            func_02029da0(line);
                            break;
                        case 22:
                            func_02029dd8(line);
                            break;
                        case 23:
                            func_02029f6c(line);
                            break;
                        case 24:
                            func_0202a034(line);
                            break;
                    }
                }
                data_020c61ac.mUnk_10++;
            } while (done == 0);
        }
    } while (data_020c61ac.mUnk_1c_2);
}
