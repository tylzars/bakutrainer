#include <Main/Unknown/functions.h>
#include <Main/Unknown/game_math.h>
#include <Main/Unknown/random_angle.h>
#include <Main/Unknown/system_init.h>
#include <nitro/fx.h>
#include <nitro/os.h>
#include <Main/Unknown/screen_fade.h>

#define ANGLE_HALF 0x8000
#define ANGLE_FULL 0x10000

// Advances the generator state: add a constant, rotate right by 3.
u32 func_0201fd68(void) {
    u32 s = data_020c18bc.mUnk_478 + 0x5cfae23d;
    s = (s >> 3) + (s << 29);
    data_020c18bc.mUnk_478 = s;
    return s;
}

// Random value between lo and hi. When lo == hi nothing is computed and lo is still in r0; adding `return lo;`
// changes the code.
u32 func_0201fd84(u32 lo, u32 hi) {
    if (lo != hi) {
        return func_0201fb74((u16) func_0201fd68(), ANGLE_FULL, lo, hi);
    }
}

// Mixes the current scanline into the generator state.
void func_0201fda4(void) {
    data_020c18bc.mUnk_478 += *(vu16 *) 0x04000006;
}

// Reads the bits lo..hi (inclusive, bit 0 = lowest bit of the first byte) of a byte array.
u32 func_0201fdbc(const void *data, u32 lo, u32 hi) {
    const u8 *base = (const u8 *) data;
    u32 result = 0;
    u32 shift = 0;
    const u8 *last = base + hi / 8;
    u32 pos = lo;
    while (pos <= hi) {
        const u8 *p = base + pos / 8;
        u32 n;
        if (p != last) {
            n = 8 - (pos & 7);
        } else {
            n = hi - pos + 1;
        }
        result |= (((int) *p >> (pos & 7)) & (u8) ((1 << n) - 1)) << shift;
        pos += n;
        shift += n;
    }
    return result;
}

// Shortest distance between two angles.
u32 func_0201fe14(u32 a, u32 b) {
    int d;
    if (a < b) {
        u32 t = a;
        a = b;
        b = t;
    }
    d = a - b;
    if (d > ANGLE_HALF) {
        b += ANGLE_FULL;
        d = b - a;
    }
    return d;
}

// Shortest signed distance from b to a.
int func_0201fe34(u32 a, u32 b) {
    int d;
    int sign;
    if (a < b) {
        u32 t = a;
        a = b;
        b = t;
        sign = 1;
    } else {
        sign = -1;
    }
    d = a - b;
    if (d > ANGLE_HALF) {
        b += ANGLE_FULL;
        d = b - a;
        sign = -sign;
    }
    if (sign > 0) {
        return d;
    }
    return -d;
}

// 1 when the short way from b to a runs upward.
int func_0201fe6c(u16 a, u16 b) {
    u32 d;
    int up;
    if (b < a) {
        d = a - b;
        up = 1;
    } else {
        d = b - a;
        up = 0;
    }
    if (d > ANGLE_FULL - d) {
        up ^= 1;
    }
    return up;
}

// Moves b toward a by half the shortest distance.
u32 func_0201fe8c(u16 a, u16 b) {
    u32 d = func_0201fe14(a, b);
    int up = func_0201fe6c(a, b);
    return (u16) (b + (up * 2 - 1) * (d >> 1));
}

u32 func_0201feb0(s32 x, s32 y) {
    return FX_Atan2Idx(x, -y);
}

// Cross product of the vectors b->a and b->c.
int func_0201febc(int ax, int ay, int bx, int by, int cx, int cy) {
    return (ax - bx) * (cy - by) - (cx - bx) * (ay - by);
}

void func_0201fed4(int a) {
    u32 elapsed;
    u32 flag;
    u32 limit;
    if (data_020c18bc.mUnk_32e == 0) {
        return;
    }
    if (data_020c18bc.mUnk_32e == 4 && (a & 1) == 0) {
        return;
    }
    elapsed = data_020c18bc.mUnk_45c - data_020c18bc.mUnk_324;
    flag = 0;
    limit = data_020c18bc.mUnk_457;
    if (limit == 0) {
        limit = 180;
    }
    while (func_0201f910(&elapsed, &flag, 60, limit, 3) == 0) {
        func_0201bb30();
    }
    data_020c18bc.mUnk_450 |= 4;
    while (data_020c18bc.mUnk_32e == 3 || data_020c18bc.mUnk_32e == 10 || data_020c18bc.mUnk_32e == 4) {
        func_0201bb30();
    }
    data_020c18bc.mUnk_32f = data_020c18bc.mUnk_32e;
    data_020c18bc.mUnk_32e = 0;
    data_020c18bc.mUnk_46c &= 0xfff7ffff;
}

int func_0201ff80(u32 id) {
    return data_020c18bc.mUnk_328 / func_02021e44(id, data_020c18bc.mUnk_32c + 1);
}

int func_0201ffa0(void) {
    int d = data_020c18bc.mUnk_32c - data_020c18bc.mUnk_32a;
    if (!(data_020c18bc.mUnk_32a < data_020c18bc.mUnk_32c)) {
        OS_Terminate();
    }
    return d + 1;
}

void *func_0201ffbc(void) {
    return func_0201c430(func_0201ffa0() * 4);
}

void func_0201ffcc(void *p) {
    func_0201c4f0(p);
}
