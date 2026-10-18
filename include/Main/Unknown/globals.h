#pragma once

#include "types.h"
#include <Main/Unknown/stage_data.h>
#include <Main/Unknown/move_object.h>
#include <nitro/fx.h>
#include <Main/Unknown/falling_blocks.h>
#include <Main/Unknown/stage_camera.h>
#include <Main/Unknown/move_object.h>

#ifdef __cplusplus
extern "C" {
#endif

// Global state blocks. Only the members the code touches are named.
// Block cleared together at start-up: 0x24 bytes and the object at member 0x24.
typedef struct Unk_020c1b0c {
    u8 mUnk_00[0x24];
    u8 mUnk_24[8];
} Unk_020c1b0c;

typedef struct SystemState {
    u8 mUnk_000[0x10];
    u16 mUnk_10;
    u16 mUnk_12;
    u8 mUnk_014[0x250 - 0x14];
    Unk_020c1b0c mUnk_250;
    u8 mUnk_27c[0x292 - 0x27c];
    u16 mUnk_292;
    u16 mUnk_294;
    u16 mUnk_296;
    u8 mUnk_298[0x2a2 - 0x298];
    u8 mUnk_2a2;
    u8 mUnk_2a3;
    u8 mUnk_2a4[2];
    u8 mUnk_2a6;
    u8 mUnk_2a7;
    s32 mUnk_2a8;
    u8 mUnk_2ac_0 : 1;
    u8 mUnk_2ac_1 : 1;
    u8 mUnk_2ac_2 : 1;
    u8 mUnk_2ac_3 : 1;
    u8 mUnk_2ac_4 : 1;
    u8 mUnk_2ac_5 : 1;
    u8 mUnk_2ac_6 : 2;
    u8 mUnk_2ad;
    u8 mUnk_2ae;
    u8 mUnk_2af;
    u8 mUnk_2b0;
    u8 mUnk_2b1[0x2d4 - 0x2b1];
    s32 mUnk_2d4;
    s32 mUnk_2d8;
    s32 mUnk_2dc;
    s32 mUnk_2e0;
    u8 mUnk_2e4[0x2f0 - 0x2e4];
    u8 mUnk_2f0;
    u8 mUnk_2f1[0x2fa - 0x2f1];
    u8 mUnk_2fa;
    u8 mUnk_2fb[0x308 - 0x2fb];
    u16 mUnk_308;
    u8 mUnk_30a[6];
    u8 mUnk_310;
    u8 mUnk_311[5];
    u8 mUnk_316;
    u8 mUnk_317[5];
    void (*mUnk_31c)(int);
    u8 mUnk_320[4];
    s32 mUnk_324;
    u8 mUnk_328[4];
    u16 mUnk_32c;
    u8 mUnk_32e;
    u8 mUnk_32f[0x450 - 0x32f];
    u32 mUnk_450;
    u8 mUnk_454[3];
    u8 mUnk_457;
    u8 mUnk_458[4];
    vu32 mUnk_45c;
    u8 mUnk_460[0x46c - 0x460];
    u32 mUnk_46c;
    u8 mUnk_470[0x492 - 0x470];
    u8 mUnk_492;
    u8 mUnk_493;
    u8 mUnk_494;
    u8 mUnk_495[3];
    s32 mUnk_498;
    u16 mUnk_49c[2];
} SystemState;

typedef struct StateC {
    u16 mUnk_0;
    u16 mUnk_2;
    s32 mUnk_4;
} StateC;

extern SystemState data_020c18bc;
extern StateC data_020c66d4;
extern u8 data_020c66dc[];
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

#define STATE_COUNT 25

typedef void (*StateFunc)(void);

extern GameFlow data_020b1194;
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
    s32 mUnk_14[5];
} Unk_020b10fc;
extern Unk_020b10fc data_020b10fc;

typedef struct Unk_020c2660 {
    u8 mUnk_00[4];
    u32 mUnk_04;
    u8 mUnk_08[8];
    u16 mUnk_10;
} Unk_020c2660;
extern Unk_020c2660 data_020c2660;

// State of the save and load menu: the selection depth and the page toggle.
typedef struct Unk_020b1078 {
    s32 mUnk_00;
    s32 mUnk_04;
    s32 mUnk_08;
} Unk_020b1078;
extern Unk_020b1078 data_020b1078;

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

// Camera block: two position vectors the scene moves.
typedef struct Unk_020c5f04 {
    u8 mUnk_000[0x204];
    VecFx32 mUnk_204;
    u8 mUnk_210[0x228 - 0x210];
    VecFx32 mUnk_228;
    u8 mUnk_234[0x240 - 0x234];
} Unk_020c5f04;

// Camera and projection state (the ARM render code also addresses it). Member 0xc0 is the camera block the scene
// moves; 0x314/0x316/0x318 are angles, 0x330 and 0x338/0x33c are flags, 0x344 a register value.
typedef struct CameraState {
    u8 mUnk_000[0x28];
    void (*mUnk_28)(void);
    u8 mUnk_2c[0x34 - 0x2c];
    u16 *mUnk_34;
    s32 mUnk_38;
    s32 mUnk_3c;
    u8 mUnk_40[4];
    s32 mUnk_44;
    s32 mUnk_48;
    u8 mUnk_4c[4];
    s32 mUnk_50;
    s32 mUnk_54;
    u8 mUnk_58[0x5e - 0x58];
    u16 mUnk_5e;
    u8 mUnk_60[0x7f - 0x60];
    u8 mUnk_7f;
    u8 mUnk_80[0xa4 - 0x80];
    VecFx32 mUnk_a4;
    VecFx32 mUnk_b0;
    u8 mUnk_bc;
    u8 mUnk_bd[3];
    Unk_020c5f04 mUnk_0c0;
    u8 mUnk_300[0x310 - 0x300];
    u8 mUnk_310;
    u8 mUnk_311[3];
    u16 mUnk_314;
    u16 mUnk_316;
    u16 mUnk_318;
    u8 mUnk_31a[0x330 - 0x31a];
    u16 mUnk_330;
    u8 mUnk_332[0x338 - 0x332];
    s32 mUnk_338;
    s32 mUnk_33c;
    u16 mUnk_340;
    u8 mUnk_342[2];
    s32 mUnk_344;
    u8 mUnk_348[0x34c - 0x348];
    u8 mUnk_34c;
    u8 mUnk_34d[0x354 - 0x34d];
    u16 mUnk_354;
    u8 mUnk_356[0x35b - 0x356];
    u8 mUnk_35b;
} CameraState;
extern CameraState data_020c5e44;

// Tables in the code area and state objects the field and page screens read.
extern const u16 data_0204e138[];

extern const u32 data_0201ab04[];

extern const u32 data_0201aa60[];

// The light matrix and font tables in the code area.
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

extern u8 data_020ae464[];
extern Unk_020c642c data_020c642c;
extern Unk_020c61ac data_020c61ac;
extern const Unk_0204e378 data_0204e378[];

// Font blocks in the code area (as data_0201a61c).
extern const u8 data_0201a626[];
extern const u8 data_0201a662[];
extern const u8 data_0201a676[];

// Part of data_020b6ee0 the stage info panel works on through a pointer: a position, two values and two fade
// counters.
typedef struct Unk_020c1454 {
    VecFx32 mUnk_00;
    VecFx32 mUnk_0c;
    u8 mUnk_18[0x24 - 0x18];
    s32 mUnk_24;
    s32 mUnk_28;
    s32 mUnk_2c;
    s32 mUnk_30;
    s32 mUnk_34;
    u8 mUnk_38[0x3c - 0x38];
    s32 mUnk_3c;
    s32 mUnk_40;
} Unk_020c1454;

// Large block of game state shared by several screens; only the members the code touches are named.
typedef struct Unk_020b6ee0 {
    u16 mUnk_0000;
    u16 mUnk_0002;
    u8 mUnk_0004;
    u8 mUnk_0005[3];
    Unk_020b6ee8 mUnk_0008[8];
    u8 mUnk_0c48;
    u8 mUnk_0c49[3];
    u16 *mUnk_0c4c;
    s32 mUnk_0c50;
    s32 mUnk_0c54;
    u8 mUnk_0c58;
    u8 mUnk_0c59;
    u16 mUnk_0c5a;
    u8 mUnk_0c5c;
    u8 mUnk_0c5d;
    u8 mUnk_0c5e[2];
    s32 mUnk_c60[3];
    s32 mUnk_c6c[3];
    u8 mUnk_c78;
    u8 mUnk_c79;
    u8 mUnk_c7a[0xc94 - 0xc7a];
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
    s32 mUnk_1000[3];
    s16 mUnk_100c;
    s16 mUnk_100e[3];
    Unk_020b7ef4 mUnk_1014[48];
    u8 mUnk_5a54;
    u8 mUnk_5a55[3];
    Unk_020bc938 mUnk_5a58[32];
    u8 mUnk_8ad8;
    u8 mUnk_8ad9;
    u8 mUnk_8ada[2];
    Unk_020bf9bc mUnk_8adc[32];
    u8 mUnk_8e5c;
    u8 mUnk_8e5d[0x8f94 - 0x8e5d];
    u32 mUnk_8f94;
    u8 mUnk_8f98[0xa0fc - 0x8f98];
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
    u8 mUnk_a149;
    u8 mUnk_a14a;
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
    s32 mUnk_a568;
    u8 mUnk_a56c;
    u8 mUnk_a56d;
    u8 mUnk_a56e;
    u8 mUnk_a56f;
    u8 mUnk_a570;
    u8 mUnk_a571[3];
    Unk_020c1454 mUnk_a574;
    u8 mUnk_a5b8[0xa5d0 - 0xa5b8];
    u8 mUnk_a5d0;
    u8 mUnk_a5d1[3];
    Sprite mUnk_a5d4;
    Sprite mUnk_a6d8[2];
    s32 mUnk_a8e0;
    s32 mUnk_a8e4;
    s32 mUnk_a8e8;
    u16 mUnk_a8ec;
    u8 mUnk_a8ee;
    u8 mUnk_a8ef;
    u8 mUnk_a8f0;
    u8 mUnk_a8f1;
    u8 mUnk_a8f2;
    u8 mUnk_a8f3[39];
    u8 mUnk_a91a;
    u8 mUnk_a91b;
} Unk_020b6ee0;
extern Unk_020b6ee0 data_020b6ee0;

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

// Rows of 16 bytes (0x49 of them); the first word is passed on by func_0201033c.
typedef struct Unk_0204e96c {
    s32 mUnk_00;
    s32 mUnk_04;
    s32 mUnk_08;
    s32 mUnk_0c;
} Unk_0204e96c;
extern const Unk_0204e96c data_0204e96c[];

// Objects of the save menu: the stage settings of its card test and the selection of each depth.
extern StageInfo data_020b10d4;
extern s32 data_020b1094[8];
// The model viewer's state: the model, the index of the model shown, its rotation and a toggle.
typedef struct Unk_020b122c {
    Model mModel;
    s32 mUnk_178;
    u16 mUnk_17c;
    u8 mUnk_17e[2];
    s32 mUnk_180;
} Unk_020b122c;
extern Unk_020b122c data_020b122c;

extern const u8 data_0201a66c[];
extern const u8 data_0201a63a[];
extern const u8 data_0201a644[];

extern const u8 data_0201a612[];
extern const u8 data_0201a68a[];
extern const u8 data_0201a694[];

// Objects of the stage camera code.
// Touch trail at 0x800: the points (two bytes each) func_02020e3c appends; their count is data_020c2660.mUnk_10
// (it wraps at 0x80).
typedef struct Unk_TouchPoint {
    u8 mUnk_0;
    u8 mUnk_1;
} Unk_TouchPoint;

typedef struct Unk_020c1d60 {
    u8 mUnk_000[0x800];
    Unk_TouchPoint mUnk_800[0x80];
} Unk_020c1d60;
extern Unk_020c1d60 data_020c1d60;

typedef struct Unk_020c0cbc {
    s32 mUnk_00;
    s32 mUnk_04;
    s32 mUnk_08;
} Unk_020c0cbc;
extern Unk_020c0cbc data_020c0cbc;
extern u32 data_0201aba0[7];

#ifdef __cplusplus
}
#endif
