#pragma once

#include "types.h"
#include <Main/Unknown/stage_data.h>

#ifdef __cplusplus
extern "C" {
#endif

void func_02011518(int stage, int mode);
void func_020115ac(int frames, char *buf);
void func_02011624(int frames, char *buf);
void func_0201168c(int level, u32 a, int b, int c, int d);
int func_020117e0(int percent);
void func_0201180c(int stage, int mode);
void func_02011890(int level, int mode);
void func_02011a94(int stage, int mode);
void func_02011b70(int stage, int mode);
void func_02011bcc(int a, int b, int c);
void func_02011c64(void);
void func_02011d1c(void);
void func_02011ebc(StageRec *rec, u8 index, u32 flags);
int func_02011f44(int value, int low, int high, int steps);
void func_02011f70(int kind, int x, int y, int delta);
void func_02011fec(int a, int b, int c, int d, u8 e, int x, int y, int z, u8 *state, s32 *position);
void func_020122c0(u8 type);
void func_020123a4(int a);
void func_02012490(u8 a, int b);

#ifdef __cplusplus
}
#endif
