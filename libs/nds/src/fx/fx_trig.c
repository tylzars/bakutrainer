#include <nitro/fx.h>

// Angle index (0x2000 = 45 degrees) for each 1/128 step of the ratio of the smaller to the larger coordinate.
extern const s16 FX_AtanIdxTable_[];

// Angle of the vector (x, y) as a 16-bit index, 0x10000 = full turn.
u16 FX_Atan2Idx(fx32 y, fx32 x) {
    fx32 a;
    fx32 b;
    int base;
    int add;
    if (y > 0) {
        if (x > 0) {
            if (x > y) {
                a = y;
                b = x;
                base = 0;
                add = 1;
            } else if (x < y) {
                a = x;
                b = y;
                base = 0x4000;
                add = 0;
            } else {
                return 0x2000;
            }
        } else if (x < 0) {
            x = -x;
            if (x < y) {
                a = x;
                b = y;
                base = 0x4000;
                add = 1;
            } else if (x > y) {
                a = y;
                b = x;
                base = 0x8000;
                add = 0;
            } else {
                return 0x6000;
            }
        } else {
            return 0x4000;
        }
    } else if (y < 0) {
        y = -y;
        if (x < 0) {
            x = -x;
            if (x > y) {
                a = y;
                b = x;
                base = -0x8000;
                add = 1;
            } else if (x < y) {
                a = x;
                b = y;
                base = -0x4000;
                add = 0;
            } else {
                return 0xa000;
            }
        } else if (x > 0) {
            if (x < y) {
                a = x;
                b = y;
                base = -0x4000;
                add = 1;
            } else if (x > y) {
                a = y;
                b = x;
                base = 0;
                add = 0;
            } else {
                return 0xe000;
            }
        } else {
            return 0xc000;
        }
    } else {
        if (x >= 0) {
            return 0;
        }
        return 0x8000;
    }
    if (b == 0) {
        return 0;
    }
    if (add) {
        return (u16) (base + FX_AtanIdxTable_[FX_Div(a, b) >> 5]);
    }
    return (u16) (base - FX_AtanIdxTable_[FX_Div(a, b) >> 5]);
}
