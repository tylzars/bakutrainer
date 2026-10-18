#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// State shared by the mode selection screen and the screen transition.
typedef struct ModeSelectState {
    s32 mUnk_00;
    s32 mUnk_04;
    s32 mUnk_08;
    u8 mUnk_0c;
    u8 mPad_0d[3];
    s32 mUnk_10;
    s32 mUnk_14;
    s32 mUnk_18;
} ModeSelectState;

extern ModeSelectState data_0206d444;
// The word after it, which the board screen reads through its own symbol.
extern s32 data_0206d448;
extern u32 data_0206391c;

void func_020054a4(int mode);
void func_02005548(void);
void func_020055a8(void);
void func_02005898(void);
void func_020059ac(void);
void func_020059b4(int a, u32 b);
void func_020059cc(u32 value);
void func_020059d0(u32 *value, u32 add, int shift);
void func_020059e8(u32 *value, u32 add);
int func_02005a00(u32 value);
void func_02005a8c(int a, int b);
int func_02005b1c(int mode);
u8 func_02005bf8(void);
s32 func_02005c04(void);

#ifdef __cplusplus
}
#endif
