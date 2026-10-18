#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Global state blocks. Only the members the code touches are named.
typedef struct SystemState {
    u8 mUnk_000[0x10];
    u16 mUnk_10;
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
typedef struct Unk_020c1b5c {
    u8 mUnk_00[0xc];
    u8 mUnk_0c_0 : 1;
} Unk_020c1b5c;
extern Unk_020c1b5c data_020c1b5c;
typedef struct Unk_020c1bbc {
    u8 mUnk_00[8];
    u16 mUnk_08;
    u8 mUnk_0a[6];
    u8 mUnk_10;
} Unk_020c1bbc;
extern Unk_020c1bbc data_020c1bbc;
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

typedef struct Unk_020c1760 {
    u8 mUnk_00[0x64];
    s32 mUnk_64;
} Unk_020c1760;
extern Unk_020c1760 data_020c1760;

typedef struct Unk_020b10fc {
    s32 mUnk_00;
    s32 mUnk_04;
    s32 mUnk_08;
    s32 mUnk_0c;
    u32 mUnk_10;
} Unk_020b10fc;
extern Unk_020b10fc data_020b10fc;
extern u32 data_020b1110[];

typedef struct Unk_020c2660 {
    u8 mUnk_00[4];
    u32 mUnk_04;
} Unk_020c2660;
extern Unk_020c2660 data_020c2660;

typedef struct Unk_020c60c4 {
    u8 mUnk_00[0x70];
    s32 mUnk_70;
} Unk_020c60c4;
extern Unk_020c60c4 data_020c60c4;

typedef struct Unk_02098d40 {
    u8 mUnk_00[0x14];
    u8 mUnk_14;
} Unk_02098d40;
extern Unk_02098d40 data_02098d40;

typedef struct Unk_02098d60 {
    u8 mUnk_00[0xc];
    u32 mUnk_0c;
    s32 mUnk_10;
} Unk_02098d60;
extern Unk_02098d60 data_02098d60;

extern u32 data_020b1084;

// Word tables and constant blocks in the code area.
extern const u32 data_0201abbc[];
extern const u8 data_0201a61c[];
extern const u8 data_0201a680[];
extern const u8 data_0201a64e[];
extern const u32 data_0204e0cc[];
extern const u32 data_0204e0d4[];
extern const s32 data_0204e0fc[];
extern const s32 data_0204e110[];
extern const s32 data_0204e124[];
extern const u32 data_0204e0f0[];
extern const u32 data_0204e0e8[];
extern const u32 data_0204e144[];

#ifdef __cplusplus
}
#endif
