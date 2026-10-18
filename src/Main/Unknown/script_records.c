#include <Main/Unknown/functions.h>
#include <Main/Unknown/globals.h>
#include <Main/Unknown/system_init.h>
#include <Main/Unknown/text_draw.h>
#include <nitro/os.h>
#include <Main/Unknown/script_records.h>

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
    data_020c61ac.mUnk_280.mUnk_14 = data_020c61ac.mUnk_280.mUnk_15 = 0;
    data_020c61ac.mUnk_280.mUnk_1c = 0;
    data_020c61ac.mUnk_18 = 0;
    for (i = 0; i < 0x20; i++) {
        data_020c61ac.mUnk_5c[i].mUnk_12 = -1;
    }
}

// Sets up the record store: the ids and buffers, or releases the old buffers when the ids change.
void func_02028f0c(int a, int b, int c, int d, int e, int f, int g, u8 (*h)(void), int i) {
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
