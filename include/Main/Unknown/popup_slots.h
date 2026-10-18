#pragma once

#include "types.h"

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

// The work block that data_02063940 points to; only the members the code touches are named.
typedef struct PopupWork {
    u8 mUnk_000[0x364];
    u32 mUnk_364;
    u8 mUnk_368[0x3b0 - 0x368];
    s32 mUnk_3b0;
    u8 mUnk_3b4[0x5b0 - 0x3b4];
    s32 mUnk_5b0;
    u8 mUnk_5b4[0x610 - 0x5b4];
    u32 mUnk_610;
    s32 mUnk_614;
    s32 mUnk_618;
    s32 mUnk_61c;
    s32 mUnk_620;
    s32 mUnk_624;
    s32 mUnk_628;
    s32 mUnk_62c;
    s32 mUnk_630;
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
} PopupWork;

extern PopupWork *data_02063940;

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
