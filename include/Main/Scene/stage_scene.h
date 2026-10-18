#pragma once

#include "types.h"
#include <nitro/fx.h>
#include <Main/Unknown/game_util.h>
#include <Main/Unknown/move_object.h>
#include <Main/Scene/stage_data.h>

#ifdef __cplusplus
extern "C" {
#endif

// One timed slot of the popup table; the table has 9 of them.
typedef struct PopupSlot {
    s32 mUnk_00;
    s32 mUnk_04;
    s32 mUnk_08;
    s32 mUnk_0c;
    s32 mUnk_10;
    s32 mUnk_14;
    s32 mUnk_18;
    s32 mUnk_1c;
    s32 mUnk_20;
    s32 mUnk_24;
} PopupSlot;

#define POPUP_SLOT_COUNT 9

typedef struct PopupPos {
    s32 x;
    s32 y;
} PopupPos;

// The stage scene object (data_02063940 points to it): the stage settings, two models, the draw parameters, the
// scene's vectors and counters, and the timed popup slots.
typedef struct Scene {
    u8 mUnk_000[0x178];
    StageInfo mStage;
    u8 mUnk_19e[0x230 - 0x19e];
    Model mUnk_230;
    DrawParams mUnk_3a8;
    Model mUnk_3ec;
    u8 mUnk_564[0x5a8 - 0x564];
    VecFx32 mUnk_5a8;
    VecFx32 mUnk_5b4;
    VecFx32 mUnk_5c0;
    VecFx32 mUnk_5cc;
    VecFx32 mUnk_5d8;
    VecFx32 mUnk_5e4;
    VecFx32 mUnk_5f0;
    VecFx32 mUnk_5fc;
    u32 mUnk_608;
    s32 mUnk_60c;
    u32 mUnk_610;
    s32 mUnk_614;
    s32 mUnk_618;
    s32 mUnk_61c;
    s32 mUnk_620;
    s32 mUnk_624;
    s32 mUnk_628;
    s32 mUnk_62c;
    u32 mUnk_630;
    s32 mUnk_634;
    s32 mUnk_638;
    s32 mUnk_63c;
    s32 mUnk_640;
    s32 mUnk_644;
    s32 mUnk_648;
    s32 mUnk_64c;
    s32 mUnk_650;
    s32 mUnk_654;
    s32 mUnk_658;
    s32 mUnk_65c;
    s32 mUnk_660;
    s32 mUnk_664;
    PopupSlot mSlots[POPUP_SLOT_COUNT];
    s32 mUnk_7d0;
    u16 mUnk_7d4;
    u16 mUnk_7d6;
    PopupPos mPos;
} Scene;

extern Scene *data_02063940;
extern const u16 data_0204e1c0[];
extern const s32 data_0204e1cc[][2];

void func_0200c65c(int index, VecFx32 *out);
void func_0200c688(int n);
void func_0200c6ac(void);
void func_0200c7a4(void);
void func_0200ca14(void);
void func_0200cbe8(void);
void func_0200cd1c(void);
void func_0200cd2c(void);
void func_0200cdc4(void);
void func_0200ce24(void);
void func_0200ceac(void);
void func_0200d014(int slot, int type, int index);
void func_0200d098(void);
void func_0200d2a4(PopupSlot *slot);
void func_0200d434(void);
void func_0200d5f0(void);
void func_0200d60c(int value, const PopupPos *pos);
void func_0200d638(void);
int func_0200d6b0(u8 stage);

#ifdef __cplusplus
}
#endif
