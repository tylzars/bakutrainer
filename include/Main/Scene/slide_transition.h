#pragma once

#include "types.h"
#include <Main/Unknown/move_object.h>

#ifdef __cplusplus
extern "C" {
#endif

// Three words read from a record (func_0200e4c4).
typedef struct Unk_020ae504_Rec {
    s32 mUnk_00;
    u32 mUnk_04;
    s32 mUnk_08;
} Unk_020ae504_Rec;

// State block; only the members the code touches are named.
typedef struct Unk_020ae504 {
    Unk_020ae504_Rec mUnk_00[3];
    u32 mUnk_24;
    s32 mUnk_28;
    s32 mUnk_2c;
    s32 mUnk_30;
    s32 mUnk_34;
    s8 mUnk_38;
    u8 mUnk_39[3];
    Model mUnk_3c;
    u32 mUnk_1b4;
    u8 mUnk_1b8;
    u8 mUnk_1b9[3];
    s32 mUnk_1bc[8];
    s32 mUnk_1dc;
    s32 mUnk_1e0;
    s32 mUnk_1e4;
} Unk_020ae504;

extern Unk_020ae504 data_020ae504;

int func_0200de68(void);
void func_0200e3d0(int a, int b);
void func_0200e3ec(void);
void func_0200e424(int a);
void func_0200e474(void);
void func_0200e484(void);
void func_0200e4c4(void);
void func_0200e5b0(void);
void func_0200f360(void);

#ifdef __cplusplus
}
#endif
