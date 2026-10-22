#pragma once

#include "types.h"
#include <Main/Scene/stage_data.h>

#ifdef __cplusplus
extern "C" {
#endif

void UpdateProgress(void);
int IsReachedNodeSelected(void);
void UpdateNodeState(void);
void SetupNodeStageInfos(u32 node, int mode);
void ApplyBattleSlotFlagsAndSetNodeStages(int node, int flags);
void func_02014bf0(void);
void LoadStageInfo(StageInfo *info);
void func_02014ccc(StageInfo *info);
void func_02014cdc(StageInfo *info);
void PickTableEntries(StageInfo *info);
int PickWeightedTableEntry(StageInfo *info);
void ResetStages(int a);
int AddStage(int list, int id);
int FindStage(int list, int id);
int AddValue(StageRec *rec, int delta);
int AddPercent(StageRec *rec, int amount, int a, int b);
int AddGrade(StageRec *rec, int delta);
int IsValueAtMax(StageInfo *info);
void ResetSlots(int a);
int SetSlotStage(int slot, int stage);
StageRec *GetSlotStage(int slot);
int FindStageSlot(int stage);
void FillEmptySlots(void);
int FindFirstUnslottedStage(void);
int FindNextUnslottedStage(int stage);
int FindLastUnslottedStage(void);
int FindPrevUnslottedStage(int stage);

extern const u8 data_020500d4[3];

#ifdef __cplusplus
}
#endif
