#pragma once

#include "types.h"
#include <Main/Unknown/stage_data.h>

#ifdef __cplusplus
extern "C" {
#endif

#define STAGE_MAX 0x27
#define SAVE_BLOCK_ID 0xfba

// Saved progress record. Member 0x58 counts the stages whose entry in the per-stage arrays is valid.
typedef struct SaveRecord {
    s32 mUnk_00[4];
    s32 mUnk_10;
    s32 mUnk_14;
    s32 mUnk_18;
    s32 mUnk_1c[3];
    s32 mUnk_28;
    s32 mUnk_2c;
    s32 mUnk_30;
    s32 mUnk_34;
    s32 mUnk_38;
    s32 mUnk_3c;
    s32 mUnk_40;
    s32 mUnk_44;
    s32 mUnk_48;
    s32 mUnk_4c;
    s32 mUnk_50;
    s32 mUnk_54;
    s32 mUnk_58;
    s32 mUnk_5c[STAGE_MAX];
    s32 mUnk_f8[STAGE_MAX];
    s32 mUnk_194[STAGE_MAX];
    s32 mUnk_230[STAGE_MAX];
    s32 mUnk_2cc[STAGE_MAX];
    s32 mUnk_368[STAGE_MAX];
    s32 mUnk_404[STAGE_MAX];
    s32 mUnk_4a0[STAGE_MAX];
    s32 mUnk_53c[STAGE_MAX];
    s32 mUnk_5d8[STAGE_MAX];
    s32 mUnk_674[STAGE_MAX];
} SaveRecord;

void func_02010648(u32 a);
int func_0201079c(u32 slot);
void func_020109f4(u32 slot);

#ifdef __cplusplus
}
#endif
