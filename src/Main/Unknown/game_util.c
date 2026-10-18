#include <Main/Unknown/functions.h>
#include <Main/Unknown/touch_buttons.h>
#include <Main/Unknown/random_angle.h>
#include <Main/Unknown/game_math.h>
#include <Main/Unknown/state_flow.h>
#include <Main/Unknown/game_util.h>
#include <Main/Unknown/globals.h>
#include <Main/clib.h>
#include <nitro/fx.h>
#include <nitro/gx.h>
#include <nitro/os.h>
#include <stdarg.h>
#include <stdio.h>

// Prints a formatted line with the debug font on the given screen layer.
void func_0200b898(int x, int y, const char *format, ...) {
    char buf[256];
    va_list args;
    va_start(args, format);
    vsprintf(buf, format, args);
    func_020215fc(x, y, data_0201a6d0, buf, 0, 240, 16, 2);
}

void func_0200b8dc(int x, int y, const char *format, ...) {
    char buf[256];
    va_list args;
    va_start(args, format);
    vsprintf(buf, format, args);
    func_020215fc(x, y, data_0201a6d0, buf, 0, 240, 16, 1);
}

// Debug print that does nothing in this build.
void func_0200b920(int line, const char *format, ...) {}

// Moves *p toward target by 1/steps of the distance (at least one unit); returns 1 while it is still moving.
int func_0200b928(s32 *p, s32 target, int steps) {
    if (steps <= 1) {
        *p = target;
    } else if (*p != target) {
        s32 diff = target - *p;
        s32 sign = diff != 0 ? (diff >> 31) | 1 : 0;
        s32 step = diff / steps;
        *p += step + sign;
        return 1;
    }
    return 0;
}

// func_0200b928 for each component; returns how many components are still moving.
int func_0200b960(VecFx32 *p, VecFx32 target, int steps) {
    int moving = 0;
    moving += func_0200b928(&p->x, target.x, steps);
    moving += func_0200b928(&p->y, target.y, steps);
    moving += func_0200b928(&p->z, target.z, steps);
    return moving;
}

// Moves *p toward target by 1/steps of the distance, across the wrap-around at 0xffff when that is the shorter way.
int func_0200b994(s32 *p, s32 target, int steps) {
    int r;
    s32 d = target - *p;
    s32 w = target + 0xffff;
    if (d < 0) {
        d = -d;
    }
    if (d > 0x8000) {
        r = func_0200b928(p, w, steps);
        if (*p > 0xffff) {
            *p -= 0xffff;
        }
    } else {
        r = func_0200b928(p, target, steps);
    }
    return r;
}

// Moves *p toward zero by delta and stops at zero.
int func_0200b9cc(s32 *p, int delta) {
    s8 s = *p != 0 ? (*p >> 31) | 1 : 0;
    *p = *p + -s * delta;
    if (s != (*p != 0 ? (*p >> 31) | 1 : 0)) {
        *p = 0;
    }
    return *p;
}

void func_0200ba08(u32 a, const VecFx32 *pos, u32 b, u32 c, int d, int e) {
    DrawParams p;
    memset(&p, 0, sizeof(p));
    p.mPos.x = pos->x;
    p.mPos.y = pos->y;
    p.mPos.z = pos->z;
    p.mUnk_38 = c;
    if (d != 0) {
        p.mUnk_43 = 2;
        p.mUnk_28 = d;
    }
    if (b != 0) {
        p.mUnk_34 = -1;
        p.mUnk_30 = -1;
        p.mUnk_2c = -1;
        p.mUnk_3e = b - 1;
    }
    func_020248e8(a, &p, e);
}

void func_0200ba68(u32 a, int index, int b, int c, int d, int e) {
    VecFx32 v;
    func_02031fc4(index, &v);
    func_0200ba08(a, &v, b, c, d, e);
}

void func_0200ba94(const VecFx32 *src, int dy, int d) {
    VecFx32 v = *src;
    v.y -= dy;
    func_0200ba08(0x1c5, &v, 0, 0, d, 0x38000006);
}

// Largest of the three absolute values.
u32 func_0200bacc(int x, int y, int z) {
    u32 a = x < 0 ? -x : x;
    u32 b = y < 0 ? -y : y;
    u32 c = z < 0 ? -z : z;
    return (a < c ? c : a) < b ? b : (a < c ? c : a);
}

u32 func_0200baf8(const VecFx32 *a, const VecFx32 *b) {
    return func_0200bacc(b->x - a->x, 0, b->z - a->z);
}

void func_0200bb14(int a, int b, int c, int d, int e, int f, int g, int h) {
    func_0200bc50(a, b, 0, 0, 140, c, d, func_0200f9f0(e), 0, 0, f, g, h, 0, 0, 0);
}

void func_0200bb58(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j, int k, int l, int m, int n,
                   int o) {
    if (f == g) {
        func_0200bc50(a, b, c, d, e, f, h + 1, (const char *) i, 0, 0, j, k, l, m, 1, o);
    } else {
        func_0200bc50(a, b, c, d, e, f, h, (const char *) i, 0, 0, j, k, l, m, n, o);
    }
}

void func_0200bbd0(int a, int b, int c, int d, int e, int f, int g, const char *h, const void *i, int j, int k, int l,
                   int m, int n, int o, int p, const u8 *q) {
    if (*q != 0) {
        if (m == 1) {
            g += (*q == 2 && ((data_020c18bc.mUnk_45c / 10) & 1) == 0) ? 2 : 0;
        } else {
            g += 4;
        }
        func_0200bc50(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p);
    }
}

void func_0200bc50(int a, int b, int c, int d, int e, int f, int g, const char *h, const void *i, int j, int k, int l,
                   int m, int n, int o, int p) {
    int mode = func_02020878() ? 2 : 1;
    int g2 = g;
    if (o == 0 && data_020b6ee0.mUnk_a5d0 != 0 && p == 0) {
        g2 = g + (data_020c2560.mUnk_100.mUnk_04 == f + 0x40000000);
    }
    if (n == 0 && (p != 1 || data_020c2560.mUnk_100.mUnk_04 == f + 0x40000000)) {
        func_0202de80(g2, a, b, e, -1, mode);
    }
    if (h != 0) {
        const void *font = i;
        if (font == 0) {
            font = data_0201a6c6;
        }
        func_020215fc(a, b, font, h, j, 200, 16, mode);
    }
    if (m == 1) {
        if (c != 0) {
            func_02020914(f, a - c, b - d, a + c, b + d, -1, 0, 0, (u16) k);
        } else {
            func_02020914(f, a, b, 0, 0, g, 0, 0, (u16) k);
        }
        if (l != 0 && func_0200bd80(0x20000000, f)) {
            func_0202f110(l, 100, 0, 64, 0xffff, 0);
        }
    }
}

int func_0200bd80(u32 mask, u32 id) {
    if (data_020b6ee0.mUnk_a5d0 == 0) {
        return 0;
    }
    if ((mask & data_020c2560.mUnk_100.mUnk_04) && (u16) data_020c2560.mUnk_100.mUnk_04 == id) {
        return 1;
    }
    return 0;
}

int func_0200bdac(u32 mask, u32 lo, u32 hi) {
    if ((mask & data_020c2560.mUnk_100.mUnk_04) && (u16) data_020c2560.mUnk_100.mUnk_04 >= lo &&
        (u16) data_020c2560.mUnk_100.mUnk_04 <= hi) {
        return 1;
    }
    return 0;
}

void func_0200bdcc(int a, int b) {
    if ((u32) b >= 16) {
        b = 16;
    }
    if (a & 1) {
        *(vu16 *) 0x04000050 = 0x2840;
        *(vu16 *) 0x04000052 = (b << 8) + (16 - b);
    }
    if (a & 2) {
        *(vu16 *) 0x04001050 = 0x2840;
        *(vu16 *) 0x04001052 = (b << 8) + (16 - b);
    }
}

u16 func_0200be10(u32 r, u32 g, u32 b) {
    return r | (g << 5) | (b << 10);
}

void func_0200be20(u32 num, int a, int b, u16 color) {
    VecFx32 v;
    func_0200bec0(&v, a, b, 127);
    func_0200be50(num, v.x, v.y, v.z, color);
}

static inline void G3_LightVector(u32 num, fx16 x, fx16 y, fx16 z) {
    *(vu32 *) 0x040004c8 = ((x >> 3) & 0x3ff) | (((y >> 3) & 0x3ff) << 10) | (((z >> 3) & 0x3ff) << 20) | (num << 30);
}

static inline void G3_LightColor(u32 num, u16 color) {
    *(vu32 *) 0x040004cc = (num << 30) | color;
}

void func_0200be50(u32 num, s8 x, s8 y, s8 z, u16 color) {
    G3_LoadMtx43(&data_020c3e80);
    reg_G3X_MTX_MODE = 2;
    data_020c6144.mUnk_5b = 0;
    G3_LightVector(num, x << 5, y << 5, z << 5);
    G3_LightColor(num, color);
    data_020c6144.mUnk_5b |= 1 << num;
}

void func_0200bec0(VecFx32 *out, int a, int b, int scale) {
    int j = (b >> 4) * 2;
    int i = (a >> 4) * 2;
    out->x = (FX_SinCosTable_[i + 1] * FX_SinCosTable_[j + 1]) / 4096;
    out->y = FX_SinCosTable_[i];
    out->z = (FX_SinCosTable_[i + 1] * FX_SinCosTable_[j]) / 4096;
    out->x = (scale * out->x) / 4096;
    out->y = (scale * out->y) / 4096;
    out->z = (scale * out->z) / 4096;
}

void func_0200bf38(u32 value) {
    data_020c6144.mUnk_14 = (value << 16) / 360;
    data_020c6144.mUnk_16 = data_020c6144.mUnk_14;
    data_020c6144.mUnk_18 = data_020c6144.mUnk_16;
}

u32 func_0200bf54(int n) {
    if (n == 32) {
        return -1;
    }
    return ~((1 << n) - 1);
}

int func_0200bf68(u32 *p) {
    u8 bits[32];
    u32 free = ~*p;
    u8 n = 0;
    int i = 0;
    if (free == 0) {
        return n - 1;
    }
    while (free != 0) {
        if (free & 1) {
            bits[n] = i;
            n++;
        }
        free >>= 1;
        i++;
    }
    if (n != 0) {
        u32 pick = bits[func_0201fd84(0, n - 1)];
        *p |= 1 << pick;
        return pick;
    }
    return -1;
}

int func_0200bfc4(int a, int b, int c, int d, int e) {
    int r = func_0201fbe4(a, b, c, d, e);
    int lo = d < e ? d : e;
    int hi = d < e ? e : d;
    return (r < hi ? r : hi) < lo ? lo : (r < hi ? r : hi);
}

void func_0200bffc(int x, int y, int value, const void *font, int align, int mode) {
    char buf[16];
    *func_0201fc08(value, buf) = 0;
    func_020215fc(x, y, font, buf, align, 100, 10, mode);
}
