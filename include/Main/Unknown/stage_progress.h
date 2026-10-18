#pragma once

#include "types.h"
#include <Main/Unknown/stage_data.h>

#ifdef __cplusplus
extern "C" {
#endif

void func_020145bc(void);
int func_020149b0(void);
void func_020149e0(void);
void func_02014ac8(u32 stage, int mode);
void func_02014afc(int stage, int flags);
void func_02014bf0(void);
void func_02014c38(StageInfo *info);
void func_02014ccc(StageInfo *info);
void func_02014cdc(StageInfo *info);
void func_02014cf8(StageInfo *info);
int func_02014d2c(StageInfo *info);
void func_02014d8c(int a);
int func_02014de4(int kind, int index);
int func_02014f14(int kind, int index);
// Takes two arguments (it adds r1 to mInfo.mUnk_04), but the two calls in result_screen.c pass only the record:
// r1 still holds the stage index there. The declaration carries no prototype.
int func_02014f98();
int func_02014ffc(StageRec *rec, int amount, int a, int b);
int func_020150a0(StageRec *rec, int delta);
int func_020150c0(StageInfo *info);
void func_020150ec(int a);
int func_02015114(int a, int b);
StageRec *func_02015190(int index);
int func_020151cc(int stage);
void func_020151f8(void);
int func_02015260(void);
int func_02015294(int stage);
int func_020152d4(void);
int func_0201530c(int stage);

extern const u8 data_020500d4[3];

#ifdef __cplusplus
}
#endif
