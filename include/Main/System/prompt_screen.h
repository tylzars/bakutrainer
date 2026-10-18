#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// State of the two-button prompt screen: the saved display setup restored when the prompt closes, and the
// position of the buttons and the prompt text while they slide in and out.
typedef struct Unk_020b1124 {
    u16 mUnk_00[2];
    u8 mUnk_04[0x0c - 0x04];
    s32 mUnk_0c;
    s32 mUnk_10;
    s32 mUnk_14;
    s32 mUnk_18;
    s32 mUnk_1c;
    s32 mUnk_20;
    u16 mUnk_24;
    u16 mUnk_26;
    s32 mUnk_28;
    s32 mUnk_2c;
    s32 mUnk_30;
    s32 mUnk_34;
    s32 mUnk_38;
    s32 mUnk_3c;
    s32 mUnk_40;
    s32 mUnk_44;
    s32 mUnk_48;
    s32 mUnk_4c;
    u8 mUnk_50[0x64 - 0x50];
    s32 mUnk_64;
    s32 mUnk_68;
    u16 mUnk_6c;
    u16 mUnk_6e;
} Unk_020b1124;
extern Unk_020b1124 data_020b1124;

int func_0200fb68(int *a, int *b);
void func_0200fd90(void);
void func_0200feb8(void);
void func_0201004c(void);
void func_020101d0(void);
void func_02010294(int a, int b, int c);
void func_020102f8(void);
void func_0201033c(int a, int b);
void func_02010364(int a, int b);

#ifdef __cplusplus
}
#endif
