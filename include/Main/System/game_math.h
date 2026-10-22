// Side header of system_init.c: its math helpers (interpolation, square roots, division, number formatting).
#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

int InterpolateQuadratic(int a, int b, int c, int t);
u32 LengthSquared2d(int x, int y);
u32 Length2d(int x, int y);
s64 LengthSquared2d64(int x, int y);
void EaseLevel(u16 *value, int speed);
void AddLevel(u16 *p, int add);
int MapRangeFromZero(int a, int b, int c, int d);
int MapRange(int v, int a, int b, int c, int d);
char *FormatDecimal(int value, char *buf);
char *FormatDecimalGrouped(int value, char *buf, char separator);
int DivideRounded(int a, int b);
u32 SqrtU32(u32 value);
u32 SqrtU64(u64 value);
u32 DivideWithRemainder(u32 numerator, u32 denominator, u32 *remainder);
s32 Divide(s32 numerator, s32 denominator);

#ifdef __cplusplus
}
#endif
