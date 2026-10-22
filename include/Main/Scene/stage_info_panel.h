#pragma once

#include "types.h"
#include <Main/Scene/stage_data.h>

#ifdef __cplusplus
extern "C" {
#endif

void LoadStageOrTextImage(int stage, int layer);
void FormatFrameTime(int frames, char *buf);
void FormatSignedSeconds(int frames, char *buf);
void DrawModeScoreAndTime(int stage, u32 score, int scoreThreshold, int frames, int layer);
int PercentToX(int percent);
void DrawPercentGauge(int stage, int layer);
void DrawStageInfo(int level, int layer);
void func_02011a94(int stage, int layer);
void func_02011b70(int stage, int layer);
void SetTextAndImage(int a, int b, int c);
void DrawTextPanel(void);
void UpdateTextAndStageInfo(void);
void func_02011ebc(StageRec *rec, u8 index, u32 flags);
int MapRangeToIndex(int value, int low, int high, int steps);
void AddAmountObjSlots(int kind, int x, int y, int delta);
void DrawGauge(int a, int b, int c, u8 d, u8 e, int x, int y, int z, u8 *state, s32 *position);
void StartGauge(u8 type);
void StopGauge(int a);
void UpdateGauge(u8 a, int b);
void UpdatePanel(int stage);
void SetupSlotScreen(void);
void UpdateSlotScreen(void);
void LeaveSlotScreen(void);
void UpdateSlotScreenInput(void);
void GetTableEntrySpriteIds(StageInfo *info, int tableEntryId, int *a, int *b);
void GetTableEntryInfoIds(StageInfo *info, int tableEntryId, int *a, int *b, int *c, int *d, int *e, int *f);
void DrawTableEntryButton(StageInfo *info, s16 tableEntryId, u8 flag, int a, int b, int c, int d, int e);
void DrawTableEntryInfo(StageInfo *info, s16 tableEntryId, int a, int b);
void DrawTableEntryRowAndHandleButtons(int stage, int *cursor, int x, int y, int layer, int flags);
void SetupSharedModelViewer(void);
void UpdateSharedModelViewer(void);
void LeaveSharedModelViewer(void);
void func_0201418c(void);
void SetupBattleModelViewer(void);
void UpdateBattleModelViewer(void);
void LeaveBattleModelViewer(void);
void ResetProgress(int a);
void ResetAllProgress(void);
void ResetAndReadSaveSlot(void);
void RunSaveScreen(int a);
int FindTableEntryIndex(int id);
int GetTableEntryValue(int row, int column);
int GetTableEntryValueById(int id, int column);
int GetNodeValue(int node, int column);
int GetSelectedNodeStageId(int a);

#ifdef __cplusplus
}
#endif
