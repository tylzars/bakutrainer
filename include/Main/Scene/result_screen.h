#pragma once

#include "types.h"
#include <Main/Minigame/falling_blocks.h>

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
    Sprite mUnk_68;
    Sprite mUnk_16c;
} ResultState;

// Fractions of points carried between frames.
typedef struct ResultRemainder {
    u32 mUnk_00;
    u32 mUnk_04;
    u32 mUnk_08;
    s32 mUnk_0c;
    s32 mUnk_10;
} ResultRemainder;

extern ResultState data_0206d1d4;
extern ResultRemainder data_0206d1c0;

void func_0200494c(void);
void func_02004964(void);
void func_02004aa8(void);
void func_02005194(void);
void func_0200548c(void);

#ifdef __cplusplus
}
#endif
