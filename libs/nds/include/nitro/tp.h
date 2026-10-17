#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// A touch sample: coordinates, whether the pen is down, and a validity code.
typedef struct {
    /* 00 */ u16 x;
    /* 02 */ u16 y;
    /* 04 */ u16 touch;
    /* 06 */ u16 validity;
} TPData;

// Linear map from raw touch coordinates to screen pixels.
typedef struct {
    /* 00 */ s16 x0;
    /* 02 */ s16 y0;
    /* 04 */ s16 xDotSize;
    /* 06 */ s16 yDotSize;
} TPCalibrateParam;

void func_02042914(int tag, u32 data, BOOL err);
void TP_Init(void);
int TP_GetUserInfo(TPCalibrateParam *param);
void TP_SetCalibrateParam(const TPCalibrateParam *param);
void func_02042dc8(void);
int func_02042e5c(TPData *out);
int func_02042ea8(TPData *out);
int TP_CalcCalibrateParam(TPCalibrateParam *param, u16 x1, u16 y1, u16 px1, u16 py1, u16 x2, u16 y2, u16 px2, u16 py2);
void TP_GetCalibratedPoint(TPData *out, const TPData *raw);
void func_020431d4(u32 mask);

#ifdef __cplusplus
}
#endif
