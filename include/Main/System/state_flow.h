#pragma once

#include "types.h"
#include <nitro/fx.h>
#include <Main/Scene/stage_data.h>
#include <Main/Unknown/move_object.h>
#include <Main/Minigame/falling_blocks.h>
#include <Main/Scene/stage_camera.h>

#ifdef __cplusplus
extern "C" {
#endif

// Main loop state: frame counter, current/previous/requested state numbers, the queue of requested states (PushState
// terminates at 25 entries) and a table of values the states pass on through SetFlowValue and GetFlowValue (a prompt
// number, a selection, a state to push); none of them is called.
#define QUEUE_SIZE 0x1c
#define HANDLER_COUNT 24
typedef struct GameFlow {
    u32 mFrame;
    u32 mState;
    u32 mPrevState;
    u32 mNextState;
    // 1 makes the main loop leave and enter the state again even when it does not change. Only PopStateEx writes it
    // from a parameter, and its only caller passes 0, so it is never 1 in this build.
    s32 mReenterRequested;
    u32 mQueueCount;
    u8 mQueue[QUEUE_SIZE];
    u32 mHandlers[HANDLER_COUNT];
    s32 mStateFlag;
} GameFlow;

#define STATE_COUNT 25

typedef void (*StateFunc)(void);

extern GameFlow gGameFlow;

extern const u8 gStatePromptScreenEnabled[STATE_COUNT];
extern const StateFunc gStateEnterFuncs[STATE_COUNT];
extern const StateFunc gStateUpdateFuncs[STATE_COUNT];
extern const StateFunc gStateLeaveFuncs[STATE_COUNT];

typedef struct Unk_020b10fc {
    s32 mUnk_00;
    s32 mUnk_04;
    s32 mUnk_08;
    s32 mUnk_0c;
    u32 mUnk_10;
    s32 mUnk_14[5];
} Unk_020b10fc;
extern Unk_020b10fc data_020b10fc;

// Language and three more settings words (the first is saved under the key "LANGUAGE").
typedef struct Unk_020b1084 {
    u32 mLanguage;
    u32 mUnk_04;
    u32 mUnk_08;
    s32 mUnk_0c;
} Unk_020b1084;
extern Unk_020b1084 data_020b1084;

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

u16 LookupLanguageFileId(int index);
const char *GetText(int id);
const char *GetLanguageText(int id, int index);
void InitGameFlow(void);
void ClearStateQueueAndFlag(void);
void ReenterState(void);
void PushState(int state);
void PopState(void);
void PopStateEx(u32 flag);
u32 GetState(void);
u32 GetNextState(void);
void SetFlowValue(u32 index, u32 value);
u32 GetFlowValue(u32 index);
void SetStateFlag(s32 value);
s32 GetStateFlag(void);

#ifdef __cplusplus
}
#endif
