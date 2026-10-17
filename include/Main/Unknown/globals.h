#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Global state blocks. Only the members the code touches are named.
typedef struct SystemState {
    u8 mUnk_000[0x12];
    u16 mUnk_12;
    u8 mUnk_014[0x2a2 - 0x14];
    u8 mUnk_2a2;
    u8 mUnk_2a3;
    u8 mUnk_2a4[2];
    u8 mUnk_2a6;
    u8 mUnk_2a7;
    s32 mUnk_2a8;
    u8 mUnk_2ac[0x45c - 0x2ac];
    vu32 mUnk_45c;
} SystemState;

typedef struct StateB {
    u8 mUnk_00[0x92];
    u16 mUnk_92;
    u16 mUnk_94;
    u16 mUnk_96;
} StateB;

typedef struct StateC {
    u16 mUnk_0;
    u16 mUnk_2;
    s32 mUnk_4;
} StateC;

extern SystemState data_020c18bc;
extern StateB data_020c1abc;
extern StateC data_020c66d4;
extern u8 data_020c66dc[];
extern u8 data_020c1b30[];
extern u8 data_020c1b0c[];
extern u8 data_020c67e0[];
extern u8 data_020c68e0[];
extern u8 data_020c66e0[];
extern u8 data_020c6720[];

// Main loop state: frame counter, current/previous/requested state numbers.
typedef struct GameFlow {
    u32 mFrame;
    u32 mState;
    u32 mPrevState;
    u32 mNextState;
    s32 mUnk_10;
    u8 mUnk_14[0x94 - 0x14];
    s32 mUnk_94;
} GameFlow;

typedef struct StateD {
    u8 mUnk_00[0x5c];
    u32 mUnk_5c;
    u8 mUnk_60[0x6c - 0x60];
    u32 mUnk_6c;
} StateD;

#define STATE_COUNT 25

typedef void (*StateFunc)(void);

extern GameFlow data_020b1194;
extern StateD data_020c1cbc;
extern u8 data_020c1b5c[];
extern u8 data_020c1bbc[];
extern u8 data_020c1d3c[];
extern u8 data_020c14b4[];
typedef struct StateE {
    u8 mUnk_00[0x1a];
    u8 mUnk_1a;
    u8 mUnk_1b;
} StateE;
extern StateE data_020c17e0;
extern void *data_0201abec[];

extern StateFunc data_0204e840[STATE_COUNT];
extern StateFunc data_0204e8a4[STATE_COUNT];
extern StateFunc data_0204e908[STATE_COUNT];
extern u8 data_0204e824[STATE_COUNT];

#ifdef __cplusplus
}
#endif
