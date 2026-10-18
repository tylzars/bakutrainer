#pragma once

#include "types.h"
#include <nitro/fx.h>
#include <Main/System/system_init.h>

#ifdef __cplusplus
extern "C" {
#endif

// Hash steps of the graphics pools: the odd primes from 1 to 719.
extern const u16 data_02050378[128];

u32 func_020225dc(u32 i);
u32 func_020227b4(u32 i);
u32 func_0202283c(Unk_PoolRange *n, u32 size, u32 slot);
u32 func_020229b4(u32 kind, u32 key, u32 size, u32 srcSize);
u32 func_02022cdc(u32 i);
u32 func_02022eb4(u32 i);
u32 func_02022f40(Unk_PoolRange *n, u32 size, u32 slot);
u32 func_020230b0(u32 kind, u32 key, u32 size);
void func_020232f0(u32 map, u16 *out, s32 *pos, s32 *vec, u32 d, int *done, const Unk_PoseNode *node);
void func_02023478(u16 a, u16 *rot, VecFx32 *pos, VecFx32 *vec, int e, int f, int g, int h);

#ifdef __cplusplus
}
#endif
