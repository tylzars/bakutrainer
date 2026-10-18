#pragma once

#include "types.h"
#include <Main/Unknown/stage_data.h>
#include <Main/Unknown/move_object.h>
#include <nitro/fx.h>
#include <Main/Unknown/falling_blocks.h>
#include <Main/Unknown/move_object.h>

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
    u8 mUnk_460[0x49c - 0x460];
    u16 mUnk_49c[2];
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

// Main loop state: frame counter, current/previous/requested state numbers, the queue of requested states and a
// table of per-state handler words.
#define QUEUE_SIZE 0x1c
#define HANDLER_COUNT 24
typedef struct GameFlow {
    u32 mFrame;
    u32 mState;
    u32 mPrevState;
    u32 mNextState;
    s32 mUnk_10;
    u32 mQueueCount;
    u8 mQueue[QUEUE_SIZE];
    u32 mHandlers[HANDLER_COUNT];
    s32 mUnk_94;
} GameFlow;

typedef struct StateD {
    u8 mUnk_00[0x50];
    u32 mUnk_50;
    u8 mUnk_54[8];
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
    u8 mUnk_0c_2 : 1;
    u8 mUnk_0c_3 : 1;
    u8 mUnk_0c_4 : 1;
    u8 mUnk_0c_5 : 1;
    u8 mUnk_0c_6 : 2;
    u8 mUnk_0d;
    u8 mUnk_0e;
    u8 mUnk_0f;
    u8 mUnk_10;
} Unk_020c1b5c;
extern Unk_020c1b5c data_020c1b5c;
typedef struct Unk_020c1bbc {
    u8 mUnk_00[8];
    u16 mUnk_08;
    u8 mUnk_0a[6];
    u8 mUnk_10;
    u8 mUnk_11[5];
    u8 mUnk_16;
    u8 mUnk_17[5];
    void (*mUnk_1c)(int);
    u8 mUnk_20[4];
    s32 mUnk_24;
    u8 mUnk_28[4];
    u16 mUnk_2c;
} Unk_020c1bbc;
extern Unk_020c1bbc data_020c1bbc;
typedef struct Unk_020c1d3c {
    u8 mUnk_00[0x12];
    u8 mUnk_12;
    u8 mUnk_13;
    u8 mUnk_14;
    u8 mUnk_15[3];
    s32 mUnk_18;
    u16 mUnk_1c;
    u16 mUnk_1e;
} Unk_020c1d3c;
extern Unk_020c1d3c data_020c1d3c;
extern u8 data_020c14b4[];
extern void *data_0201abec[];

extern StateFunc data_0204e840[STATE_COUNT];
extern StateFunc data_0204e8a4[STATE_COUNT];
extern StateFunc data_0204e908[STATE_COUNT];
extern u8 data_0204e824[STATE_COUNT];

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

// Language and three more settings words (the first is saved under the key "LANGUAGE").
typedef struct Unk_020b1084 {
    u32 mLanguage;
    u32 mUnk_04;
    u32 mUnk_08;
    s32 mUnk_0c;
} Unk_020b1084;
extern Unk_020b1084 data_020b1084;

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

// Camera and projection state (the ARM render code also addresses it).
typedef struct CameraState {
    u8 mUnk_000[0x28];
    void (*mUnk_28)(void);
    u8 mUnk_2c[0x7f - 0x2c];
    u8 mUnk_7f;
    u8 mUnk_80[0xa4 - 0x80];
    VecFx32 mUnk_a4;
    VecFx32 mUnk_b0;
    u8 mUnk_bc;
    u8 mUnk_bd[0x2c4 - 0xbd];
    VecFx32 mUnk_2c4;
    u8 mUnk_2d0[0x2e8 - 0x2d0];
    VecFx32 mUnk_2e8;
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

// State objects of the 3D view and input, the light matrix, and a font table (some may be members of a larger
// object).
// Input state read by the button helpers: member 0x10 is a flag byte.
typedef struct Unk_020c14a0 {
    u8 mUnk_00[0x10];
    u8 mUnk_10;
} Unk_020c14a0;
extern Unk_020c14a0 data_020c14a0;

// Screen state: members 0x14/0x16/0x18 are angles, 0x30 and 0x38/0x3c are flags, 0x44 a register value.
typedef struct Unk_020c6144 {
    u8 mUnk_00[0x10];
    u8 mUnk_10;
    u8 mUnk_11[3];
    u16 mUnk_14;
    u16 mUnk_16;
    u16 mUnk_18;
    u8 mUnk_1a[0x30 - 0x1a];
    u16 mUnk_30;
    u8 mUnk_32[2];
    s32 mUnk_34;
    s32 mUnk_38;
    s32 mUnk_3c;
    u8 mUnk_40[4];
    u32 mUnk_44;
} Unk_020c6144;
extern Unk_020c6144 data_020c6144;

typedef struct Unk_020c6184 {
    u16 mUnk_00;
    u8 mUnk_02[0x14 - 2];
    u16 mUnk_14;
    u8 mUnk_16[0x1b - 0x16];
    u8 mUnk_1b;
} Unk_020c6184;
extern Unk_020c6184 data_020c6184;

typedef struct Unk_020c5ea4 {
    u8 mUnk_00[0x1f];
    u8 mUnk_1f;
} Unk_020c5ea4;
extern Unk_020c5ea4 data_020c5ea4;

// Camera block: two position vectors the scene moves.
typedef struct Unk_020c5f04 {
    u8 mUnk_000[0x204];
    VecFx32 mUnk_204;
    u8 mUnk_210[0x228 - 0x210];
    VecFx32 mUnk_228;
    u8 mUnk_234[0x240 - 0x234];
} Unk_020c5f04;
extern Unk_020c5f04 data_020c5f04;

extern MtxFx43 data_020c3e80;

extern const u8 data_0201a6c6[];
extern const u8 data_0201a6bc[];
extern const u8 data_0201a6d0[];

// Objects and tables of the slide transition and title code.
typedef struct Unk_RecordBuf {
    u8 mUnk_00[0x10];
    s32 mUnk_10;
    s32 mUnk_14;
    s32 mUnk_18;
} Unk_RecordBuf;

typedef struct Unk_020ae44c {
    s32 mUnk_00;
    s32 mUnk_04;
    s32 mUnk_08;
    u32 mUnk_0c;
    u32 mUnk_10;
    u32 mUnk_14;
} Unk_020ae44c;

typedef struct Unk_0204e378 {
    s32 mUnk_00;
    s32 mUnk_04;
    s32 mUnk_08;
    s32 mUnk_0c;
} Unk_0204e378;

typedef struct Unk_020c642c {
    u8 mUnk_00[0x40];
    s32 mUnk_40;
} Unk_020c642c;

typedef struct Unk_EventRec {
    s16 mUnk_00;
    s16 mUnk_02;
    u8 mUnk_04[0x22 - 4];
} Unk_EventRec;

typedef struct Unk_020c61ac {
    u8 mUnk_00[0x1a];
    u8 mUnk_1a;
    u8 mUnk_1b;
    u8 mUnk_1c_0 : 2;
    u8 mUnk_1c_2 : 1;
    u8 mUnk_1c_3 : 5;
    u8 mUnk_1d[3];
    u16 mUnk_20;
    u8 mUnk_22[8];
    s16 mUnk_2a;
} Unk_020c61ac;

extern Unk_020ae44c data_020ae44c;
extern u8 data_020ae464[];
extern Unk_020c642c data_020c642c;
extern Unk_020c61ac data_020c61ac;
extern const Unk_0204e378 data_0204e378[];

// Font blocks in the code area (as data_0201a61c).
extern const u8 data_0201a626[];
extern const u8 data_0201a662[];
extern const u8 data_0201a676[];

// Objects of the sound and save code. The large state block data_020b6ee0 also contains data_020c0fe0,
// data_020c13e0 and data_020c1440 (at +0xa100, +0xa500 and +0xa560), declared as separate objects.
// Part of the large game state block that holds the progress values.
// Large block of game state shared by several screens; only the members the code touches are named.
typedef struct Unk_020b6ee0 {
    u8 mUnk_0000[0xc60];
    s32 mUnk_c60[3];
    s32 mUnk_c6c[3];
    u8 mUnk_c78[0xc94 - 0xc78];
    s32 mUnk_c94;
    s32 mUnk_c98;
    s32 mUnk_c9c;
    u8 mUnk_ca0[6];
    u16 mUnk_ca6;
    u16 mUnk_ca8;
    u8 mUnk_caa[2];
    Model mModel[2];
    VecFx32 mUnk_f9c;
    u8 mUnk_fa8[0xfc8 - 0xfa8];
    VecFx32 mUnk_fc8;
    u8 mUnk_fd4[0xfdf - 0xfd4];
    u8 mUnk_fdf;
    u8 mUnk_fe0;
    u8 mUnk_fe1;
    u16 mUnk_fe2;
    u16 mUnk_fe4;
    u16 mUnk_fe6[2];
    u16 mUnk_fea;
    u16 mUnk_fec;
    u16 mUnk_fee;
    u16 mUnk_ff0;
    u16 mUnk_ff2;
    u16 mUnk_ff4;
    u16 mUnk_ff6;
    u16 mUnk_ff8;
    u16 mUnk_ffa;
    u16 mUnk_ffc;
    u16 mUnk_ffe;
    u8 mUnk_1000[0xc];
    s16 mUnk_100c;
    u8 mUnk_100e[0xa0fc - 0x100e];
    s32 mUnk_a0fc;
    s32 mUnk_a100;
    s32 mUnk_a104;
    s32 mUnk_a108;
    s32 mUnk_a10c;
    s32 mUnk_a110;
    s32 mUnk_a114;
    s32 mUnk_a118;
    s32 mUnk_a11c;
    s32 mUnk_a120;
    s32 mUnk_a124;
    s32 mUnk_a128;
    s32 mUnk_a12c;
    s32 mUnk_a130;
    s32 mUnk_a134;
    s32 mUnk_a138;
    u8 mUnk_a13c;
    u8 mUnk_a13d;
    u8 mUnk_a13e;
    u8 mUnk_a13f;
    u8 mUnk_a140;
    u8 mUnk_a141;
    u8 mUnk_a142;
    u8 mUnk_a143;
    u8 mUnk_a144;
    s8 mUnk_a145;
    u8 mUnk_a146;
    u8 mUnk_a147;
    u8 mUnk_a148;
    u8 mUnk_a149[2];
    u8 mUnk_a14b;
    u8 mUnk_a14c;
    u8 mUnk_a14d;
    u8 mUnk_a14e[0xa530 - 0xa14e];
    s32 mUnk_a530;
    s32 mUnk_a534;
    s32 mUnk_a538;
    u32 mUnk_a53c;
    s32 mUnk_a540[8];
    s32 mUnk_a560;
    s32 mUnk_a564;
    u8 mUnk_a568[0xa56c - 0xa568];
    u8 mUnk_a56c;
    u8 mUnk_a56d;
    u8 mUnk_a56e;
    u8 mUnk_a56f;
    u8 mUnk_a570[0xa5d4 - 0xa570];
    Sprite mUnk_a5d4;
    Sprite mUnk_a6d8[2];
    u8 mUnk_a8e0[4];
    s32 mUnk_a8e4;
    u8 mUnk_a8e8[0xa8ee - 0xa8e8];
    u8 mUnk_a8ee;
    u8 mUnk_a8ef;
    u8 mUnk_a8f0[0xa8f3 - 0xa8f0];
    u8 mUnk_a8f3[39];
    u8 mUnk_a91a;
    u8 mUnk_a91b;
} Unk_020b6ee0;
extern Unk_020b6ee0 data_020b6ee0;

typedef struct Unk_020c1bdc {
    u8 mUnk_00[0xe];
    u8 mUnk_0e;
} Unk_020c1bdc;
extern Unk_020c1bdc data_020c1bdc;

typedef struct Unk_020c1cfc {
    u8 mUnk_00[0x17];
    u8 mUnk_17;
} Unk_020c1cfc;
extern Unk_020c1cfc data_020c1cfc;

typedef struct Unk_020c17c0 {
    u8 mUnk_00[0xe];
    u8 mUnk_0e;
    u8 mUnk_0f;
    u8 mUnk_10[2];
    u8 mUnk_12;
} Unk_020c17c0;
extern Unk_020c17c0 data_020c17c0;

typedef struct Unk_020c1440 {
    u8 mUnk_00[0xc];
    u8 mUnk_0c;
    u8 mUnk_0d;
    u8 mUnk_0e;
    u8 mUnk_0f;
} Unk_020c1440;
extern Unk_020c1440 data_020c1440;

typedef struct Unk_020c0fe0 {
    u8 mUnk_00[0x10];
    s32 mUnk_10;
    u8 mUnk_14[0x20 - 0x14];
    s32 mUnk_20;
    s32 mUnk_24;
    s32 mUnk_28;
    s32 mUnk_2c;
    s32 mUnk_30;
    s32 mUnk_34;
    s32 mUnk_38;
    u8 mUnk_3c;
    u8 mUnk_3d;
    u8 mUnk_3e[0x4b - 0x3e];
    u8 mUnk_4b;
} Unk_020c0fe0;
extern Unk_020c0fe0 data_020c0fe0;

typedef struct Unk_020b10f8 {
    s32 mUnk_00;
    s32 mUnk_04[5];
    s32 mUnk_18;
    s32 mUnk_1c;
    s32 mUnk_20;
    s32 mUnk_24;
    s32 mUnk_28;
} Unk_020b10f8;
extern Unk_020b10f8 data_020b10f8;

typedef struct Unk_020c8254 {
    u8 mUnk_00[8];
    u8 mUnk_08;
} Unk_020c8254;
extern Unk_020c8254 data_020c8254;

typedef struct Unk_020c8214 {
    u8 mUnk_00[0x10];
    u32 mUnk_10;
    u8 mUnk_14[0x24 - 0x14];
    s32 mUnk_24;
} Unk_020c8214;
extern Unk_020c8214 data_020c8214;

// Objects of the prompt screen code.
typedef struct Unk_020c1b3c {
    u8 mUnk_00[0x54];
    s32 mUnk_54;
    s32 mUnk_58;
    s32 mUnk_5c;
    s32 mUnk_60;
} Unk_020c1b3c;
extern Unk_020c1b3c data_020c1b3c;

typedef struct Unk_020c1b9c {
    u8 mUnk_00[0x10];
    u8 mUnk_10;
    u8 mUnk_11[0x1a - 0x11];
    u8 mUnk_1a;
} Unk_020c1b9c;
extern Unk_020c1b9c data_020c1b9c;

// Rows of 16 bytes (0x49 of them); the first word is passed on by func_0201033c.
typedef struct Unk_0204e96c {
    s32 mUnk_00;
    s32 mUnk_04;
    s32 mUnk_08;
    s32 mUnk_0c;
} Unk_0204e96c;
extern const Unk_0204e96c data_0204e96c[];

// Objects of the debug save menu and model viewer.
typedef struct Unk_020b10d8 {
    u8 mUnk_00[0xd];
    u8 mUnk_0d;
} Unk_020b10d8;
extern Unk_020b10d8 data_020b10d8;
extern StageInfo data_020b10d4;
extern s32 data_020b1090[];
extern s32 data_020b1094[];
// The model viewer's state: the model, the index of the model shown, its rotation and a toggle.
typedef struct Unk_020b122c {
    Model mModel;
    s32 mUnk_178;
    u16 mUnk_17c;
    u8 mUnk_17e[2];
    s32 mUnk_180;
} Unk_020b122c;
extern Unk_020b122c data_020b122c;

// Objects of the stage info panel.
typedef struct Unk_020c17a0 {
    u8 mUnk_00[0x2c];
    u16 mUnk_2c;
} Unk_020c17a0;
extern Unk_020c17a0 data_020c17a0;


typedef struct Unk_020c1454 {
    u8 mUnk_00[0x40];
    s32 mUnk_40;
} Unk_020c1454;
extern Unk_020c1454 data_020c1454;

extern const u8 data_0201a66c[];

extern const u8 data_0201a612[];
extern const u8 data_0201a68a[];
extern const u8 data_0201a694[];

#ifdef __cplusplus
}
#endif
