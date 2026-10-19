#pragma once

#include "types.h"
#include <Main/Scene/stage_data.h>

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
void func_02011fec(int a, int b, int c, u8 d, u8 e, int x, int y, int z, u8 *state, s32 *position);
void func_020122c0(u8 type);
void func_020123a4(int a);
void func_02012490(u8 a, int b);
void func_020126a0(int stage);
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
void func_02014124(void);
void func_02014184(void);
void func_0201418c(void);
void func_02014190(void);
void func_02014260(void);
void func_0201438c(void);
void func_02014390(int a);
void func_020143f4(void);
void func_0201440c(void);
void func_02014430(int a);
int func_020144a4(int a);
int func_020144e0(int a, int b);
int func_02014518(int stage, int field);
int func_02014538(int a, int b);
int func_0201456c(int a);

#ifdef __cplusplus
}
#endif
