#pragma once

#include "types.h"
#include <Main/Scene/stage_data.h>

#ifdef __cplusplus
extern "C" {
#endif

void LoadStageOrTextImage(int stage, int layer);
void FormatFrameTime(int frames, char *buf);
void FormatSignedSeconds(int frames, char *buf);
void func_0201168c(int stage, u32 score, int b, int frames, int d);
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
void func_02013b48(StageInfo *info, int slot, int *a, int *b);
void func_02013c24(StageInfo *info, int slot, int *a, int *b, int *c, int *d, int *e, int *f);
void func_02013d48(StageInfo *info, s16 slot, u8 flag, int a, int b, int c, int d, int e);
void func_02013dc8(StageInfo *info, s16 slot, int a, int b);
void func_02013f74(int stage, int *cursor, int x, int y, int layer, int flags);
void func_020140d4(void);
void func_02014124(void);
void func_02014184(void);
void func_0201418c(void);
void func_02014190(void);
void func_02014260(void);
void func_0201438c(void);
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
