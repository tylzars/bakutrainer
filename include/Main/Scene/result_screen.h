#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// State of the result screen.
typedef struct ResultState {
    u16 mUnk_00;
    u8 mPad_02[2];
    s32 mUnk_04;
    u32 mUnk_08;
    s32 mUnk_0c;
    s32 mUnk_10;
    s32 mUnk_14;
    s32 mUnk_18;
    s32 mUnk_1c;
    s32 mUnk_20;
    s32 mUnk_24;
    s32 mUnk_28;
    s32 mUnk_2c;
    s32 mUnk_30;
    s32 mUnk_34;
    s32 mUnk_38;
    s32 mUnk_3c;
    s32 mUnk_40;
    s32 mUnk_44;
    s32 mUnk_48[8];
    s32 mUnk_68;
    s32 mUnk_6c;
} ResultState;

// Fractions of points carried between frames.
typedef struct ResultRemainder {
    u32 mUnk_00;
    u32 mUnk_04;
    u32 mUnk_08;
    s32 mUnk_0c;
    s32 mUnk_10;
} ResultRemainder;

typedef struct ResultObjA {
    u8 mUnk_00[6];
    u8 mUnk_06;
} ResultObjA;

typedef struct ResultObjB {
    u8 mUnk_00[10];
    u8 mUnk_0a;
} ResultObjB;

typedef struct ResultObjC {
    u8 mUnk_00[0x70];
    s32 mUnk_70;
} ResultObjC;

extern ResultState data_0206d1d4;
extern ResultRemainder data_0206d1c0;
extern ResultObjA data_0206d314;
extern ResultObjB data_0206d414;
extern ResultObjC data_0206d2d4;
extern u8 data_0206d210[];
extern u8 data_0206d23c[];
extern u8 data_0206d340[];

void func_0200494c(void);
void func_02004964(void);
void func_02004aa8(void);
void func_02005194(void);
void func_0200548c(void);

#ifdef __cplusplus
}
#endif
