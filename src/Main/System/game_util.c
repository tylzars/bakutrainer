#include <Main/Input/touch_buttons.h>
#include <Main/System/random_angle.h>
#include <Main/System/game_math.h>
#include <Main/System/state_flow.h>
#include <Main/System/game_util.h>
#include <Main/Graphics/text_draw.h>
#include <Main/Graphics/model.h>
#include <Main/Sound/wave_player.h>
#include <Main/Graphics/sprite_script.h>
#include <Main/Map/map_entries.h>
#include <Main/Graphics/script_data.h>
#include <Main/System/system_init.h>
#include <Main/clib.h>
#include <nitro/fx.h>
#include <nitro/gx.h>
#include <nitro/os.h>
#include <stdarg.h>
#include <stdio.h>

// DrawFormattedTextOnLayer2 and DrawFormattedTextOnLayer1 draw with font data_0201a6d0, width 240.
void DrawFormattedTextOnLayer2(int x, int y, const char *format, ...) {
    char buf[256];
    va_list args;
    va_start(args, format);
    vsprintf(buf, format, args);
    DrawText(x, y, data_0201a6d0, buf, 0, 240, 16, 2);
}

void DrawFormattedTextOnLayer1(int x, int y, const char *format, ...) {
    char buf[256];
    va_list args;
    va_start(args, format);
    vsprintf(buf, format, args);
    DrawText(x, y, data_0201a6d0, buf, 0, 240, 16, 1);
}

// Debug print that does nothing in this build.
void func_0200b920(int line, const char *format, ...) {}

// Moves *p toward target by 1/steps of the distance (at least one unit); returns 1 while it is still moving.
int EaseToward(s32 *p, s32 target, int steps) {
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

// EaseToward for each component; returns how many components are still moving.
int EaseVecToward(VecFx32 *p, VecFx32 target, int steps) {
    int moving = 0;
    moving += EaseToward(&p->x, target.x, steps);
    moving += EaseToward(&p->y, target.y, steps);
    moving += EaseToward(&p->z, target.z, steps);
    return moving;
}

// Moves *p toward target by 1/steps of the distance. When they are more than 0x8000 apart it heads for target + 0xffff
// instead and wraps back below 0x10000, which is the shorter way only when target is below *p.
int EaseAngleToward(s32 *p, s32 target, int steps) {
    int r;
    s32 d = target - *p;
    s32 w = target + 0xffff;
    if (d < 0) {
        d = -d;
    }
    if (d > 0x8000) {
        r = EaseToward(p, w, steps);
        if (*p > 0xffff) {
            *p -= 0xffff;
        }
    } else {
        r = EaseToward(p, target, steps);
    }
    return r;
}

// Moves *p toward zero by delta and stops at zero.
int MoveTowardZero(s32 *p, int delta) {
    s8 s = *p != 0 ? (*p >> 31) | 1 : 0;
    *p = *p + -s * delta;
    if (s != (*p != 0 ? (*p >> 31) | 1 : 0)) {
        *p = 0;
    }
    return *p;
}

void DrawModelFileAtPos(u32 a, const VecFx32 *pos, u32 b, u32 c, int d, int e) {
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
    DrawModelFile(a, &p, e);
}

// `index` is a map entry id (group in the high byte, key in the low byte).
void DrawModelFileAtMapEntry(u32 a, int index, int b, int c, int d, int e) {
    VecFx32 v;
    func_02031fc4(index, &v);
    DrawModelFileAtPos(a, &v, b, c, d, e);
}

void func_0200ba94(const VecFx32 *src, int dy, int d) {
    VecFx32 v = *src;
    v.y -= dy;
    DrawModelFileAtPos(0x1c5, &v, 0, 0, d, 0x38000006);
}

u32 MaxAbs3(int x, int y, int z) {
    u32 a = x < 0 ? -x : x;
    u32 b = y < 0 ? -y : y;
    u32 c = z < 0 ? -z : z;
    return (a < c ? c : a) < b ? b : (a < c ? c : a);
}

u32 MaxAbsDeltaXz(const VecFx32 *a, const VecFx32 *b) {
    return MaxAbs3(b->x - a->x, 0, b->z - a->z);
}

void DrawButton(int a, int b, int c, int d, int e, int f, int g, int h) {
    DrawButtonEx(a, b, 0, 0, 140, c, d, GetText(e), 0, 0, f, g, h, 0, 0, 0);
}

void func_0200bb58(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j, int k, int l, int m, int n,
                   int o) {
    if (f == g) {
        DrawButtonEx(a, b, c, d, e, f, h + 1, (const char *) i, 0, 0, j, k, l, m, 1, o);
    } else {
        DrawButtonEx(a, b, c, d, e, f, h, (const char *) i, 0, 0, j, k, l, m, n, o);
    }
}

void func_0200bbd0(int a, int b, int c, int d, int e, int f, int g, const char *h, const void *i, int j, int k, int l,
                   int m, int n, int o, int p, const u8 *q) {
    if (*q != 0) {
        if (m == 1) {
            g += (*q == 2 && ((gSystemState.mElapsedFrames / 10) & 1) == 0) ? 2 : 0;
        } else {
            g += 4;
        }
        DrawButtonEx(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p);
    }
}

// Draws button f for this frame at (a, b), on layer 2 while the LCDs are swapped, else layer 1.
// The sprite is g, or g + 1 while button f is held (only with o == 0, p == 0 and data_020b6ee0.mButtonsEnabled set).
// It is drawn only when n == 0, and with p == 1 only while button f is held.
// Text h, if given, is drawn at (a, b) with font i (or data_0201a6c6), alignment j and width 200.
// With m == 1 it also adds touch button f, with key mask k, and plays sound l when button f is released. The touch
// area is the rectangle a - c..a + c, b - d..b + d when c is not 0, else the area AddTouchButton takes from sprite g.
void DrawButtonEx(int a, int b, int c, int d, int e, int f, int g, const char *h, const void *i, int j, int k, int l,
                  int m, int n, int o, int p) {
    int layer = IsLcdSwapped() ? 2 : 1;
    int g2 = g;
    if (o == 0 && data_020b6ee0.mButtonsEnabled != 0 && p == 0) {
        g2 = g + (gTouchInput.mTrail.mTouchState.mButtonState == f + 0x40000000);
    }
    if (n == 0 && (p != 1 || gTouchInput.mTrail.mTouchState.mButtonState == f + 0x40000000)) {
        func_0202de80(g2, a, b, e, -1, layer);
    }
    if (h != 0) {
        const void *font = i;
        if (font == 0) {
            font = data_0201a6c6;
        }
        DrawText(a, b, font, h, j, 200, 16, layer);
    }
    if (m == 1) {
        if (c != 0) {
            AddTouchButtonEx(f, a - c, b - d, a + c, b + d, -1, 0, 0, (u16) k);
        } else {
            AddTouchButtonEx(f, a, b, 0, 0, g, 0, 0, (u16) k);
        }
        if (l != 0 && TestButton(0x20000000, f)) {
            PlayWaveKeepAlive(l, 100, 0, 64, 0xffff, 0);
        }
    }
}

int TestButton(u32 mask, u32 id) {
    if (data_020b6ee0.mButtonsEnabled == 0) {
        return 0;
    }
    if ((mask & gTouchInput.mTrail.mTouchState.mButtonState) &&
        (u16) gTouchInput.mTrail.mTouchState.mButtonState == id) {
        return 1;
    }
    return 0;
}

// 1 when the button state has a flag of mask and its id is in lo..hi. Unlike TestButton, it does not check
// mButtonsEnabled.
int TestButtonRange(u32 mask, u32 lo, u32 hi) {
    if ((mask & gTouchInput.mTrail.mTouchState.mButtonState) &&
        (u16) gTouchInput.mTrail.mTouchState.mButtonState >= lo &&
        (u16) gTouchInput.mTrail.mTouchState.mButtonState <= hi) {
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

u16 PackColor(u32 r, u32 g, u32 b) {
    return r | (g << 5) | (b << 10);
}

void SetLightFromAngles(u32 num, int a, int b, u16 color) {
    VecFx32 v;
    VecFromAngles(&v, a, b, 127);
    SetLight(num, v.x, v.y, v.z, color);
}

static inline void G3_LightVector(u32 num, fx16 x, fx16 y, fx16 z) {
    *(vu32 *) 0x040004c8 = ((x >> 3) & 0x3ff) | (((y >> 3) & 0x3ff) << 10) | (((z >> 3) & 0x3ff) << 20) | (num << 30);
}

static inline void G3_LightColor(u32 num, u16 color) {
    *(vu32 *) 0x040004cc = (num << 30) | color;
}

// Sets the direction and color of light num; the light mask is replaced by num's bit, so only that light stays on.
void SetLight(u32 num, s8 x, s8 y, s8 z, u32 color) {
    G3_LoadMtx43(&gMapState.mViewMtx);
    reg_G3X_MTX_MODE = 2;
    gMapState.mCamera.mUnk_2dc.mLightMask = 0;
    G3_LightVector(num, x << 5, y << 5, z << 5);
    G3_LightColor(num, color);
    gMapState.mCamera.mUnk_2dc.mLightMask |= 1 << num;
}

void VecFromAngles(VecFx32 *out, int a, int b, int scale) {
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
    gMapState.mCamera.mUnk_2dc.mUnk_14 = (value << 16) / 360;
    gMapState.mCamera.mUnk_2dc.mUnk_16 = gMapState.mCamera.mUnk_2dc.mUnk_14;
    gMapState.mCamera.mUnk_2dc.mUnk_18 = gMapState.mCamera.mUnk_2dc.mUnk_16;
}

u32 func_0200bf54(int n) {
    if (n == 32) {
        return -1;
    }
    return ~((1 << n) - 1);
}

int TakeRandomClearBit(u32 *p) {
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
        u32 pick = bits[RandomRange(0, n - 1)];
        *p |= 1 << pick;
        return pick;
    }
    return -1;
}

int MapRangeClamped(int a, int b, int c, int d, int e) {
    int r = MapRange(a, b, c, d, e);
    int lo = d < e ? d : e;
    int hi = d < e ? e : d;
    return (r < hi ? r : hi) < lo ? lo : (r < hi ? r : hi);
}

void DrawDecimal(int x, int y, int value, const void *font, int align, int layer) {
    char buf[16];
    *FormatDecimal(value, buf) = 0;
    DrawText(x, y, font, buf, align, 100, 10, layer);
}
