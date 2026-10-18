#pragma once

#include "types.h"

// Per-stage settings; the table of records has 0x230 bytes per stage.
typedef struct StageInfo {
    u8 mUnk_00[4];
    s16 mUnk_04;
    s16 mUnk_06;
    s16 mUnk_08;
    s16 mUnk_0a;
    s16 mUnk_0c;
    s16 mUnk_0e;
    u8 mUnk_10;
    u8 mUnk_11;
    u8 mUnk_12;
    u8 mUnk_13;
    u8 mUnk_14;
    u8 mUnk_15;
    u8 mUnk_16;
    u8 mUnk_17;
    u8 mUnk_18;
    u8 mUnk_19;
    u8 mUnk_1a;
    s8 mUnk_1b;
    s16 mUnk_1c[5];
} StageInfo;

typedef struct StageRec {
    u8 mUnk_000[0x178];
    StageInfo mInfo;
    u8 mUnk_19e[0x1e4 - 0x19e];
    s32 mUnk_1e4;
    s32 mUnk_1e8;
    s32 mUnk_1ec;
    u8 mUnk_1f0[0x40];
} StageRec;

#define STAGE_MAX 0x27

extern StageRec data_020b1990[];
extern StageInfo data_020b1524[3];
