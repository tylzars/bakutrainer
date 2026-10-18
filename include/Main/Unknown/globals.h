#pragma once

#include "types.h"
#include <nitro/mic.h>
#include <nitro/fs.h>
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
    s32 mUnk_00;
    u32 mUnk_04;
    s32 mUnk_08;
    void *mUnk_0c;
    u8 mUnk_10[0x24 - 0x10];
    u8 *mUnk_24;
    u8 mUnk_28[4];
} Unk_020c1b0c;

// One queued transfer (0x14 bytes): the source (a file offset, packed with a block index for some files, or an
// address when the file is 0), the destination, the stored and the unpacked size, the file and two format bytes.
typedef struct Unk_LoadEntry {
    u32 mUnk_00;
    u32 mUnk_04;
    u32 mUnk_08;
    u32 mUnk_0c;
    u16 mUnk_10;
    u8 mUnk_12;
    u8 mUnk_13;
} Unk_LoadEntry;

// VRAM banks mapped for one use: each bank's control register, its size in KB, its LCDC address and the control
// value to restore, then the number of banks.
typedef struct Unk_VramGroup {
    vu8 *mUnk_00[4];
    u16 mUnk_10[4];
    u32 mUnk_18[4];
    u8 mUnk_28[6];
    u8 mUnk_2e;
    u8 mUnk_2f;
} Unk_VramGroup;

// One VRAM bank: its control register, its LCDC address and the control value to restore.
typedef struct Unk_VramBank {
    vu8 *mUnk_0;
    u32 mUnk_4;
    u8 mUnk_8;
    u8 mUnk_9[3];
} Unk_VramBank;

typedef struct SystemState {
    void *mUnk_000;
    u8 mUnk_004[0xc];
    u16 mUnk_10;
    u16 mUnk_12;
    u16 mUnk_014;
    u16 mUnk_016;
    u16 mUnk_018;
    u16 mUnk_01a;
    u16 mUnk_01c;
    u8 mUnk_01e[16];
    u8 mUnk_02e;
    u8 mUnk_02f;
    u8 *mUnk_030;
    s32 mUnk_034;
    s32 mUnk_038;
    Unk_LoadEntry *mUnk_03c;
    Unk_VramGroup mUnk_040;
    Unk_VramGroup mUnk_070;
    Unk_VramBank mUnk_0a0;
    Unk_VramBank mUnk_0ac;
    u8 mUnk_0b8[9];
    u8 mUnk_0c1;
    u8 mUnk_0c2[2];
    void *mUnk_0c4;
    u8 *mUnk_0c8;
    u16 mUnk_0cc;
    u8 mUnk_0ce[2];
    MICAutoParam mUnk_0d0;
    u16 mUnk_0ec[16][8];
    u32 mUnk_1ec[8];
    u32 mUnk_20c[8];
    u8 mUnk_22c;
    u8 mUnk_22d[3];
    s32 mUnk_230;
    s32 mUnk_234;
    s32 mUnk_238;
    s32 mUnk_23c;
    s32 mUnk_240;
    s32 mUnk_244;
    s32 mUnk_248;
    void (*mUnk_24c)(int);
    Unk_020c1b0c mUnk_250;
    u8 mUnk_27c[4];
    s32 mUnk_280;
    u8 mUnk_284[4];
    u16 mUnk_288;
    u8 mUnk_28a[0x292 - 0x28a];
    u16 mUnk_292;
    u16 mUnk_294;
    u16 mUnk_296;
    u8 mUnk_298[0x2a2 - 0x298];
    u8 mUnk_2a2;
    u8 mUnk_2a3;
    s8 mUnk_2a4[2];
    u8 mUnk_2a6_0 : 1;
    u8 mUnk_2a6_1 : 1;
    u8 mUnk_2a6_2 : 6;
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
    u8 mUnk_2b1;
    u8 mUnk_2b2;
    u8 mUnk_2b3;
    u8 mUnk_2b4;
    u8 mUnk_2b5[0x2c8 - 0x2b5];
    s32 mUnk_2c8;
    u16 mUnk_2cc;
    u16 mUnk_2ce;
    u16 mUnk_2d0;
    u8 mUnk_2d2[2];
    s32 mUnk_2d4;
    s32 mUnk_2d8;
    s32 mUnk_2dc;
    s32 mUnk_2e0;
    u16 mUnk_2e4;
    u16 mUnk_2e6;
    u16 mUnk_2e8;
    u16 mUnk_2ea;
    u16 mUnk_2ec;
    u16 mUnk_2ee;
    u8 mUnk_2f0;
    u8 mUnk_2f1;
    u16 mUnk_2f2;
    u16 mUnk_2f4;
    u16 mUnk_2f6;
    u16 mUnk_2f8;
    u8 mUnk_2fa;
    u8 mUnk_2fb[0x308 - 0x2fb];
    u16 mUnk_308;
    u8 mUnk_30a[4];
    u16 mUnk_30e;
    u8 mUnk_310;
    u8 mUnk_311[5];
    u8 mUnk_316;
    u8 mUnk_317[5];
    void (*mUnk_31c)(int);
    u8 mUnk_320[4];
    s32 mUnk_324;
    u16 mUnk_328;
    u16 mUnk_32a;
    u16 mUnk_32c;
    u8 mUnk_32e;
    u8 mUnk_32f;
    u16 mUnk_330;
    u8 mUnk_332[2];
    s32 mUnk_334;
    u8 mUnk_338[0x340 - 0x338];
    u16 mUnk_340[4];
    u8 mUnk_348[4];
    u8 *mUnk_34c;
    u32 mUnk_350;
    u16 mUnk_354;
    u8 mUnk_356[0x42c - 0x356];
    s32 mUnk_42c;
    u8 mUnk_430[0x450 - 0x430];
    u32 mUnk_450;
    u16 mUnk_454;
    u8 mUnk_456;
    u8 mUnk_457;
    u8 mUnk_458[4];
    vu32 mUnk_45c;
    u32 mUnk_460;
    void (*mUnk_464)(void *data, u32 size);
    void (*mUnk_468)(void);
    u32 mUnk_46c;
    s32 mUnk_470;
    u8 mUnk_474[4];
    u32 mUnk_478;
    u16 mUnk_47c;
    u16 mUnk_47e;
    u16 mUnk_480;
    u8 mUnk_482[2];
    s32 mUnk_484;
    u16 mUnk_488;
    u16 mUnk_48a;
    s8 mUnk_48c;
    s8 mUnk_48d;
    u8 mUnk_48e;
    u8 mUnk_48f;
    u8 mUnk_490[2];
    u8 mUnk_492;
    u8 mUnk_493;
    u8 mUnk_494;
    u8 mUnk_495;
    u8 mUnk_496[2];
    s32 mUnk_498;
    u16 mUnk_49c[2];
    s32 mUnk_4a0;
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

// Camera and projection state (the ARM render code also addresses it), declared 0x120 bytes: it ends with the
// eight vectors at 0xc0, and the scene code reaches the two camera positions further on as mUnk_0c0[43] and
// mUnk_0c0[46], past the declared end. The declared size is what orders the stores to mUnk_a4/mUnk_b0 before
// later loads from other objects.
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
    u8 mUnk_60[0x6c - 0x60];
    s32 mUnk_6c;
    u8 mUnk_70[4];
    u16 mUnk_74;
    u8 mUnk_76[0x7f - 0x76];
    u8 mUnk_7f;
    VecFx32 mUnk_80;
    u8 mUnk_8c[0x98 - 0x8c];
    VecFx32 mUnk_98;
    VecFx32 mUnk_a4;
    VecFx32 mUnk_b0;
    u8 mUnk_bc;
    u8 mUnk_bd[3];
    // The game code's view of the camera module's object ends at 0x120: the vectors further in (0x2c4 and 0x2e8)
    // are reached as mUnk_0c0[43] and mUnk_0c0[46], past this array's end. The bytes show that view size (the
    // accesses anchor like members of this object, and a larger declared size changes four functions).
    VecFx32 mUnk_0c0[8];
} CameraState;
extern CameraState data_020c5e44;

// Camera view settings after the camera state: 0x14/0x16/0x18 are angles, 0x30 and 0x38/0x3c are flags, 0x44 a
// register value.
typedef struct Unk_020c6144 {
    u8 mUnk_00[0x10];
    u8 mUnk_10;
    u8 mUnk_11[3];
    u16 mUnk_14;
    u16 mUnk_16;
    u16 mUnk_18;
    u8 mUnk_1a[0x30 - 0x1a];
    u16 mUnk_30;
    u8 mUnk_32[0x38 - 0x32];
    s32 mUnk_38;
    s32 mUnk_3c;
    u16 mUnk_40;
    u8 mUnk_42[2];
    s32 mUnk_44;
    u8 mUnk_48[2];
    u8 mUnk_4a;
    u8 mUnk_4b;
    u8 mUnk_4c;
    u8 mUnk_4d[0x54 - 0x4d];
    u16 mUnk_54;
    u8 mUnk_56[0x5b - 0x56];
    u8 mUnk_5b;
    u8 mUnk_5c[0x68 - 0x5c];
} Unk_020c6144;
extern Unk_020c6144 data_020c6144;

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
typedef struct Unk_020bff04 {
    Model mModel;
    VecFx32 mUnk_178;
    s32 mUnk_184;
} Unk_020bff04;

typedef struct Unk_020c1454 {
    VecFx32 mUnk_00;
    VecFx32 mUnk_0c;
    VecFx32 mUnk_18;
    s32 mUnk_24;
    s32 mUnk_28;
    s32 mUnk_2c;
    s32 mUnk_30;
    s32 mUnk_34;
    s32 mUnk_38;
    s32 mUnk_3c;
    s32 mUnk_40;
} Unk_020c1454;

// The stage's character: its model, the draw parameters and the countdown (time left, then the count).
typedef struct Unk_020bfd40 {
    Model mModel;
    DrawParams mDraw;
    s32 mUnk_1bc;
    u32 mUnk_1c0;
} Unk_020bfd40;

// Camera follow state: draw parameters, the drag velocity and the zoom (current, target) with a countdown.
typedef struct Unk_020c1030 {
    DrawParams mUnk_00;
    s32 mUnk_44;
    u8 mUnk_48[4];
    s32 mUnk_4c;
    Model mUnk_50;
    Sprite mUnk_1c8;
    Sprite mUnk_2cc;
    s32 mUnk_3d0;
    s32 mUnk_3d4;
    s32 mUnk_3d8;
    u8 mUnk_3dc;
    u8 mUnk_3dd;
    u8 mUnk_3de[2];
} Unk_020c1030;

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
    u8 mUnk_c7a[2];
    VecFx32 mUnk_c7c;
    VecFx32 mUnk_c88;
    s32 mUnk_c94;
    s32 mUnk_c98;
    s32 mUnk_c9c;
    u16 mUnk_ca0[3];
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
    u8 mUnk_8e5d[3];
    Unk_020bfd40 mUnk_8e60;
    Unk_020bff04 mUnk_9024[8];
    Model mUnk_9c64;
    s32 mUnk_9ddc[3];
    MoveObj mUnk_9de8;
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
    u8 mUnk_a14e[2];
    Unk_020c1030 mUnk_a150;
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
extern const u8 data_0201a630[];
extern const u8 data_0201a6b2[];
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

extern u32 data_0201aba0[7];

// Objects of the system start-up code.
typedef struct Unk_020c1818 {
    u32 mUnk_00;
    void *mUnk_04;
    u8 mUnk_08[8];
    void (*mUnk_10)(void);
    s32 mUnk_14;
} Unk_020c1818;
extern Unk_020c1818 data_020c1818;
extern FSFile data_020c1874;

#ifdef __cplusplus
}
#endif
