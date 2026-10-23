#pragma once

#include "types.h"
#include <Main/Scene/move_object.h>
#include <Main/Scene/stage_data.h>

#ifdef __cplusplus
extern "C" {
#endif

// Progress of the current exchange.
typedef struct BattleState {
    u32 mTableEntryIndex;
    u16 mKindValueLevel;
    u16 mPickSide;
    s16 mSubtractedTarget;
    u8 mUnk_0a;
    u8 mUnk_0b;
    u8 mUnk_0c;
    u8 mKindValueRow;
    s8 mPickIndices[2];
    u8 mTotalFell[2];
    s8 mEntryCursor;
    // ApplyTableEntry's done value, cleared before each apply; nothing reads it. 0 for kind 2, for kind 5 while
    // mSubtracted <= 0, for kind 15 while the other side's kind is not the table entry's column 5, and for kinds
    // without a case; 1 otherwise.
    u8 mUnk_13;
    // 0 or 1, toggled after each compare. While 0 the stage pick takes touch and keys (otherwise
    // ComparePickIndicesWithBestPair decides). The entry picks start with this side. The 2 set by SetupBattleScreen
    // is replaced by 0 before the stage pick runs.
    u8 mFirstSide;
    // Turns taken, a skipped turn included; table entry kind 3 sets it to 4, which ends the picks.
    u8 mEntryPickCount;
    // Side whose record stayed in at the last compare. On a tie side 1 is kept; when both totals are 0 both records go
    // out but it is still 1.
    s8 mKeptSide;
    // 60 while the side's pick is awaited. 59 after it, then counted down while gBattleView.mModel[1] does not play
    // (mId 0); at 0 the next turn starts.
    u8 mEntryTimer;
    // Frames since the side's total changed, counted up to 18 while the change sprite shows; 0xff parks it past that.
    u8 mChangeFrames[2];
    // Side whose mSubtracted eases toward mSubtractedTarget (0 or 1), with bit 0x80 once SetSidesSubtracted has run;
    // -1: none.
    s8 mSubtractedSide;
    s8 mUnk_1b;
} BattleState;

// The battle screen: two models, both fighters' stage data and the exchange's bookkeeping.
typedef struct BattleView {
    Model mModel[2];
    StageInfo mSide[2];
    s32 mApplySide;
    BattleState mState;
} BattleView;

extern BattleView gBattleView;
extern StageRec gBattleSlotStages[3];

void func_020071a8(void);
void func_020071ac(void);
void DrawConfirmButton(void);
void SetupStageModels(int a);
void DrawBattleModels(void);
void SetCameraFromMapEntries0And1(void);
void SetCameraFromMapEntries0And2(void);
void SetCameraFromModel(void);
void SetupBattleScreen(void);
void UpdateBattleScreen(void);
void LeaveBattleScreen(void);
int IsStageInfosOutClear(void);
void LoadKindValues(void);
int GetPercentOfBattleTotal(StageInfo *info);
void SetupBattleSides(int slotIndex, int stageInfoIndex);
int func_020078f8(void);
void SetSidesSubtracted(void);
int GetBattleTotal(const StageInfo *info);
int GetValuePlusKindValue(const StageInfo *info);
int FindTableEntryIndexUnbounded(int id);
void StartBattleViewModel(int a);
void StopBattleViewModel(void);
void UpdateBattleViewModel(void);
int GetTableEntryTotalDiff(int side, int row);
void ApplyTableEntry(void);
int ComparePickIndicesWithBestPair(void);
int FindBestSide1Entry(void);
void SubtractHalfFromList(int which, int flag);
void func_02007ffc(u16 *a, u16 *b);
void DrawBattleStageInfo(StageInfo *info);
void SetStateFlagTo1(void);
void UpdateBattleCompare(void);
int IsEntryPickDone(StageInfo *info, int mode);
int GetEntryStartSharedModelIndex(int order);
int GetEntryStartWaveId(int order);
void UpdateBattleEntryPick(int side);
void UpdateBattleStagePick(void);
void UpdateBattleKindValues(void);

#ifdef __cplusplus
}
#endif
