// Side header of system_init.c: random numbers, bit reads, angle helpers and the save store's block sizes.
#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

u32 func_0201fd68(void);
u32 func_0201fd84(u32 lo, u32 hi);
void func_0201fda4(void);
u32 func_0201fdbc(const void *data, u32 lo, u32 hi);
u32 func_0201fe14(u32 a, u32 b);
int func_0201fe34(u32 a, u32 b);
int func_0201fe6c(u16 a, u16 b);
u32 func_0201fe8c(u16 a, u16 b);
u32 func_0201feb0(s32 x, s32 y);
int func_0201febc(int ax, int ay, int bx, int by, int cx, int cy);
void func_0201fed4(int a);
int func_0201ff80(u32 id);
int func_0201ffa0(void);
void *func_0201ffbc(void);
void func_0201ffcc(void *p);

#ifdef __cplusplus
}
#endif
