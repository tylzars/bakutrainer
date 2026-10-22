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
    // Written by StoreLanguageAndMic: [0] 1 once stored, [1] the language, [2] and [3] the two mic settings words
    // (data_020b1084.mUnk_04 and mUnk_08).
    s32 mLanguageAndMic[4];
    // Set to 1 only by WriteSaveSlot (StoreLanguageAndMic sets mLanguageAndMic[0] instead); ReadSaveSlot applies the
    // progress only when it is 1.
    s32 mWritten;
    s32 mReachedNodeIndex;
    s32 mLastNodeDone;
    // Per stage slot: the stage id + 1, 0 = empty.
    s32 mSlotStageIdHandles[3];
    s32 mProgressState;
    s32 mUnk_2c;
    s32 mUnk_30;
    s32 mUnk_34;
    s32 mUnk_38;
    // 99 and 999 stand for -1. While data_020b6ee0.mTextId is -1, WriteSaveSlot writes the saved copies
    // (mSavedTextImageIndex, mSavedTextId) instead; ReadSaveSlot restores data_020b6ee0.mTextImageIndex and mTextId.
    s32 mTextImageIndex;
    s32 mTextId;
    s32 mBestScore0;
    s32 mBestScore1;
    s32 mBestScore2;
    s32 mBestScore3;
    s32 mBestScore4;
    // Number of stages WriteSaveSlot wrote; nothing reads it.
    s32 mStageCount;
    // Per stage id: 1 when WriteSaveSlot wrote the stage's entries in the arrays below; ReadSaveSlot reads only those.
    s32 mStageWritten[STAGE_MAX];
    // StageRec.mStoredOffsetX and mStoredOffsetZ / STAGE_UNIT, + 0x100 (WriteSaveSlot terminates on a quotient
    // outside -0xff..0xff).
    s32 mStoredOffsetXStageUnits[STAGE_MAX];
    s32 mStoredOffsetZStageUnits[STAGE_MAX];
    s32 mPercent[STAGE_MAX];
    s32 mValue[STAGE_MAX];
    s32 mGrade[STAGE_MAX];
    // mInfo.mTableEntryIds[0] to [4] as FindTableEntryIndex + 1: the row of table 0x1d9 + 1, 0 for id -1.
    s32 mTableEntryHandle0[STAGE_MAX];
    s32 mTableEntryHandle1[STAGE_MAX];
    s32 mTableEntryHandle2[STAGE_MAX];
    s32 mTableEntryHandle3[STAGE_MAX];
    s32 mTableEntryHandle4[STAGE_MAX];
} SaveRecord;

void func_02010648(u32 a);
int ReadSaveSlot(u32 slot);
void WriteSaveSlot(u32 slot);
void ReadLanguageAndMicOrWriteDefaults(void);
void WriteLanguageAndMic(void);
void ResetLanguageAndMic(void);
void StoreLanguageAndMic(SaveRecord *rec);

#ifdef __cplusplus
}
#endif
