#pragma once

#include "types.h"
#include <nitro/fx.h>
#include <Main/Unknown/game_util.h>

// Per-stage settings; the table of records has 0x230 bytes per stage.
typedef struct StageInfo {
    s16 mUnk_00;
    s16 mUnk_02;
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

typedef struct StageOffset {
    s32 x;
    s32 y;
} StageOffset;

typedef struct StageRec {
    u8 mUnk_000[0x134];
    u32 mUnk_134;
    u8 mUnk_138[0x142 - 0x138];
    u16 mUnk_142;
    u8 mUnk_144[0x158 - 0x144];
    VecFx32 mUnk_158;
    u8 mUnk_164[0x170 - 0x164];
    u16 mUnk_170;
    u8 mUnk_172[0x178 - 0x172];
    StageInfo mInfo;
    u8 mUnk_19e[2];
    union {
        DrawParams mDraw;
        struct {
            VecFx32 mUnk_1a0;
            u8 mUnk_1ac[0x1d8 - 0x1ac];
            u16 mUnk_1d8;
            u16 mUnk_1da;
            u16 mUnk_1dc;
            u8 mUnk_1de[0x1e4 - 0x1de];
        };
    };
    s32 mUnk_1e4;
    s32 mUnk_1e8;
    s32 mUnk_1ec;
    s32 mUnk_1f0;
    s32 mUnk_1f4;
    StageOffset mUnk_1f8[3];
    VecFx32 mUnk_210;
    float mUnk_21c;
    s32 mUnk_220;
    u16 mUnk_224;
    s8 mUnk_226;
    u8 mUnk_227[5];
    u8 mUnk_22c;
    u8 mUnk_22d;
    u8 mUnk_22e;
    u8 mUnk_22f;
} StageRec;

#define STAGE_MAX 0x27

extern StageRec data_020b1990[];
extern StageInfo data_020b1524[3];
