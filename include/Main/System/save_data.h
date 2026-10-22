#pragma once

#include "types.h"
#include <Main/Scene/stage_data.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SAVE_BLOCK_ID 0xfba
#define SAVE_SLOT_COUNT 3

// Saved progress record.
typedef struct SaveRecord {
    s32 mUnk_00[4];
    // Set to 1 only by WriteSaveSlot (the settings write, func_02010d4c, sets mUnk_00[0] instead); ReadSaveSlot
    // applies the progress only when it is 1.
    s32 mWritten;
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
    // Number of stages WriteSaveSlot wrote; nothing reads it.
    s32 mStageCount;
    // Per stage id: 1 when WriteSaveSlot wrote the stage's entries in the arrays below; ReadSaveSlot reads only those.
    s32 mStageWritten[STAGE_MAX];
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
int ReadSaveSlot(u32 slot);
void WriteSaveSlot(u32 slot);
void func_02010c94(void);
void func_02010cfc(void);
void func_02010d34(void);
void func_02010d4c(SaveRecord *rec);

#ifdef __cplusplus
}
#endif
