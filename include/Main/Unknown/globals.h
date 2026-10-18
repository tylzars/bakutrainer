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
    u8 mUnk_0c_1 : 1;
    u8 mUnk_0c_2 : 6;
    u8 mUnk_0d;
    u8 mUnk_0e;
    u8 mUnk_0f;
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

// A position or vector of three fixed-point coordinates.
typedef struct Pos3 {
    s32 mX;
    s32 mY;
    s32 mZ;
} Pos3;

// Camera and projection state (the ARM render code also addresses it).
typedef struct CameraState {
    u8 mUnk_000[0x28];
    void (*mUnk_28)(void);
    u8 mUnk_2c[0x7f - 0x2c];
    u8 mUnk_7f;
    u8 mUnk_80[0xa4 - 0x80];
    Pos3 mUnk_a4;
    Pos3 mUnk_b0;
    u8 mUnk_bc;
    u8 mUnk_bd[0x2c4 - 0xbd];
    Pos3 mUnk_2c4;
    u8 mUnk_2d0[0x2e8 - 0x2d0];
    Pos3 mUnk_2e8;
    u8 mUnk_2f4[0x310 - 0x2f4];
    u8 mUnk_310;
    u8 mUnk_311[3];
    u16 mUnk_314;
    u8 mUnk_316[2];
    u16 mUnk_318;
    u8 mUnk_31a[0x330 - 0x31a];
    u16 mUnk_330;
    u8 mUnk_332[0x338 - 0x332];
    s32 mUnk_338;
    s32 mUnk_33c;
    u16 mUnk_340;
    u8 mUnk_342[2];
    s32 mUnk_344;
    u8 mUnk_348[0x354 - 0x348];
    u16 mUnk_354;
} CameraState;
extern CameraState data_020c5e44;

// Tables in the code area and state objects the field and page screens read.
extern const u16 data_0204e138[];

extern const u32 data_0201ab04[];

extern const u32 data_0201aa60[];

typedef struct Unk_020c1020 {
    u8 mUnk_00[0xc];
    u8 mUnk_0c;
} Unk_020c1020;
extern Unk_020c1020 data_020c1020;

typedef struct Unk_020c13e0 {
    u8 mUnk_00[0x34];
    s32 mUnk_34;
} Unk_020c13e0;
extern Unk_020c13e0 data_020c13e0;

typedef struct Unk_020c1680 {
    u8 mUnk_00[0x16];
    u8 mUnk_16;
} Unk_020c1680;
extern Unk_020c1680 data_020c1680;

typedef struct Unk_020c1780 {
    u8 mUnk_00[0x1a];
    u8 mUnk_1a;
} Unk_020c1780;
extern Unk_020c1780 data_020c1780;

#ifdef __cplusplus
}
#endif
