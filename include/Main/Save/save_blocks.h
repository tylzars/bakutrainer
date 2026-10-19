// Side header of system_init.c: its save blocks (tags, checksums, packing).
#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

u16 func_0201ffd4(int id, u32 index);
void func_0202002c(u32 a, int b, u16 c, int d, void (*fn)(u32), int e);
u32 func_02020140(u32 index, void *buf, int c);
void func_02020224(u32 index, int c);
void func_02020258(u32 index, void *buf, int c);
void func_02020348(void);
void func_02020354(u32 *order, const u32 *key, int count);
int func_0202043c(const u32 *values, u32 *out, u32 id);
void func_0202062c(u32 *out, u32 *packed, u32 id);
void func_02020854(int a);

#ifdef __cplusplus
}
#endif
