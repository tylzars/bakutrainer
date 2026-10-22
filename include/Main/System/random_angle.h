// Side header of system_init.c: random numbers, bit reads, angle helpers and the save store's block sizes.
#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

u32 NextRandom(void);
u32 RandomRange(u32 lo, u32 hi);
void MixRandomSeed(void);
u32 ReadBits(const void *data, u32 lo, u32 hi);
u32 AngleDistance(u32 a, u32 b);
int AngleDelta(u32 a, u32 b);
int IsAngleIncreasing(u16 to, u16 from);
u32 AngleHalfway(u16 a, u16 b);
u32 AngleOfVector(s32 x, s32 y);
int Cross2d(int ax, int ay, int bx, int by, int cx, int cy);
void func_0201fed4(int a);
int GetSaveBlockSize(u32 id);
int GetSaveFieldCount(void);
void *AllocSaveRecord(void);
void FreeSaveRecord(void *p);

#ifdef __cplusplus
}
#endif
