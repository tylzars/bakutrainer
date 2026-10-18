#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Pos2 {
    s32 mX;
    s32 mY;
} Pos2;

// Extent of the playing field in 8 x 8 cells.
typedef struct FieldInfo {
    u8 mUnk_00[0x1c];
    s32 mUnk_1c;
    s32 mUnk_20;
    s32 mUnk_24;
    s32 mUnk_28;
    s32 mUnk_2c;
    s32 mUnk_30;
} FieldInfo;

// One object of the field; the field has 4 x 8 x 8 of them in a grid plus a list of 128.
typedef struct FieldObject {
    u8 mType;
    u8 mKind;
    u8 mUnk_02[2];
    s32 mUnk_04;
    s32 mUnk_08;
    s32 mUnk_0c;
    u8 mUnk_10[0x20];
    s32 mUnk_30;
    s32 mUnk_34;
    s32 mUnk_38;
    u8 mUnk_3c[0xc];
    u8 mUnk_48[0x178];
    s8 mUnk_1c0;
    u8 mUnk_1c1;
    s16 mUnk_1c2;
} FieldObject;

typedef struct FieldObjects {
    u8 mUnk_000[4];
    u32 mUnk_004;
    u8 mUnk_008[4];
    s32 mUnk_00c;
    u8 mUnk_010[0x230];
    Pos2 mUnk_240;
    s32 mUnk_248;
    u8 mUnk_24c[4];
    Pos2 mUnk_250;
    s32 mUnk_258;
    u32 mUnk_25c;
    u8 mUnk_260;
    u8 mUnk_261[3];
    s32 mUnk_264;
    s32 mUnk_268;
    s32 mUnk_26c;
    u8 mUnk_270[0x10];
    FieldInfo mInfo;
    FieldObject mGrid[4][8][8];
    FieldObject mList[128];
    u8 mUnk_2a8b4[0x1000];
    u8 mUnk_2b8b4[0x2c];
} FieldObjects;

extern FieldObjects data_0206d460;

s32 func_02005c10(int a, int b);
s32 func_02005c24(int a, int b);
s32 func_02005c3c(int a, int b);
void func_02005c54(s32 x, s32 y, s32 *outCol, s32 *outSub, s32 *outRow);
void func_02005cb4(void);
void func_02005d94(u8 group);
void func_02005de0(u8 group, u8 table);
void func_02005f70(void);
void func_020061ac(void);
int func_020062e8(void);
int func_020062ec(void);
void func_0200630c(s32 value, u32 time);
void func_02006350(s32 value, u32 time);
void func_02006398(FieldObject *obj);

#ifdef __cplusplus
}
#endif
