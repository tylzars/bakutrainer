#pragma once

#include "types.h"
#include <Main/Unknown/stage_data.h>

#ifdef __cplusplus
extern "C" {
#endif

// Stage selection screen state: member 3 is set once the shown stage was changed with a button, member 4 is the stage
// shown in the large panel (-1 for none) and member 8 the highlighted entry of the row of five (-1 for none).
typedef struct Unk_020b13b0 {
    u8 mUnk_00[2];
    u8 mUnk_02;
    u8 mUnk_03;
    s32 mUnk_04;
    s32 mUnk_08;
} Unk_020b13b0;
extern Unk_020b13b0 data_020b13b0;
// The three stage numbers of the list as they were when the screen was entered; a cancel puts them back.
extern s32 data_020b13bc[3];

void func_0201353c(void);
void func_0201364c(void);
void func_02013a2c(void);
void func_02013a34(void);
void func_02013b48(StageInfo *info, int slot, int *a, int *b);
void func_02013c24(StageInfo *info, int slot, int *a, int *b, int *c, int *d, int *e, int *f);
void func_02013d48(StageInfo *info, s16 slot, u8 flag, int a, int b, int c, int d, int e);
void func_02013dc8(StageInfo *info, s16 slot, int a, int b);
void func_02013f74(int stage, int *cursor, int x, int y, int mode, int flags);
void func_020140d4(void);

#ifdef __cplusplus
}
#endif
