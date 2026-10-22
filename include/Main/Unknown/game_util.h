#pragma once

#include "types.h"
#include <nitro/fx.h>

#ifdef __cplusplus
extern "C" {
#endif

// Parameter block for creating a sprite or a model effect.
typedef struct DrawParams {
    VecFx32 mPos;
    u16 mUnk_0c[8];
    s8 mUnk_1c[8];
    u8 mUnk_24;
    u8 mUnk_25[3];
    s32 mUnk_28;
    s32 mUnk_2c;
    s32 mUnk_30;
    s32 mUnk_34;
    u16 mUnk_38;
    u16 mUnk_3a;
    u16 mUnk_3c;
    u8 mUnk_3e;
    u8 mUnk_3f;
    u8 mUnk_40;
    u8 mUnk_41;
    u8 mUnk_42;
    u8 mUnk_43 : 3;
    u8 mUnk_43_3 : 1;
    u8 mUnk_43_4 : 1;
    u8 mUnk_43_5 : 3;
} DrawParams;

void func_0200b898(int x, int y, const char *format, ...);
void func_0200b8dc(int x, int y, const char *format, ...);
void func_0200b920(int line, const char *format, ...);
int EaseToward(s32 *p, s32 target, int steps);
int EaseVecToward(VecFx32 *p, VecFx32 target, int steps);
int EaseAngleToward(s32 *p, s32 target, int steps);
int MoveTowardZero(s32 *p, int delta);
void func_0200ba08(u32 a, const VecFx32 *pos, u32 b, u32 c, int d, int e);
void func_0200ba68(u32 a, int index, int b, int c, int d, int e);
void func_0200ba94(const VecFx32 *src, int dy, int d);
u32 MaxAbs3(int x, int y, int z);
u32 MaxAbsDeltaXz(const VecFx32 *a, const VecFx32 *b);
void func_0200bb14(int a, int b, int c, int d, int e, int f, int g, int h);
void func_0200bb58(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j, int k, int l, int m, int n,
                   int o);
void func_0200bbd0(int a, int b, int c, int d, int e, int f, int g, const char *h, const void *i, int j, int k, int l,
                   int m, int n, int o, int p, const u8 *q);
void func_0200bc50(int a, int b, int c, int d, int e, int f, int g, const char *h, const void *i, int j, int k, int l,
                   int m, int n, int o, int p);
int TestButton(u32 a, u32 b);
int TestButtonRange(u32 a, u32 b, u32 c);
void func_0200bdcc(int a, int b);
u16 func_0200be10(u32 r, u32 g, u32 b);
void SetLightFromAngles(u32 num, int a, int b, u16 color);
void SetLight(u32 num, s8 x, s8 y, s8 z, u32 color);
void VecFromAngles(VecFx32 *out, int a, int b, int scale);
void func_0200bf38(u32 value);
u32 func_0200bf54(int n);
int TakeRandomClearBit(u32 *p);
int MapRangeClamped(int a, int b, int c, int d, int e);
void DrawDecimal(int x, int y, int value, const void *font, int align, int mode);

#ifdef __cplusplus
}
#endif
