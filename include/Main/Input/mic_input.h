// Side header of system_init.c: its microphone capture and loudness analysis.
#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Microphone capture and loudness analysis.
#define MIC_FLAG_ACTIVE 0x800
#define MIC_MAX_BANDS 8
#define MIC_HISTORY 16

void func_02020ff4(int size, int b);
void func_02021054(int a);
void func_02021074(int rate, int type);
void func_020210c4(void);
u32 func_020210cc(void);
void func_0202111c(u32 count);
void func_02021194(void);
void func_020211d8(const u8 *spectrum, const u16 *ranges, u32 count, u32 *totals, u32 *sums);
void func_020212b4(u8 *out, u32 nShift);
u32 func_02021364(u32 x);
u32 func_02021380(u32 index);
void func_02021390(void);
int func_020213e8(u32 a);
void func_02021458(void);

#ifdef __cplusplus
}
#endif
