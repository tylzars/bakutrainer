#pragma once

#include "types.h"

// Per-stage settings; the table of records has 0x230 bytes per stage.
typedef struct StageInfo {
    u8 mUnk_00[0x10];
    u8 mUnk_10;
    u8 mUnk_11;
    u8 mUnk_12;
    u8 mUnk_13;
    u8 mUnk_14[8];
    s16 mUnk_1c[5];
} StageInfo;

typedef struct StageRec {
    u8 mUnk_000[0x178];
    StageInfo mInfo;
    u8 mUnk_19e[0x92];
} StageRec;

extern StageRec data_020b1990[];
