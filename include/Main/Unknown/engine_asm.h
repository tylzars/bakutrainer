#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// The C-callable routines of the hand-written engine object (engine_asm.s) that game code calls.
void func_02027fc2(const char *msg);
void func_02028150(const void *src, void *dst, u32 skip, u32 size);
u32 func_02028250(u32 x);
void func_0202875c(const void *src, void *dst);
void func_02028928(u16 *data, const u8 *table, u32 count);
void func_02028980(s16 *block, int comp, s16 *maxcode, u8 *valptr, u16 *mincode, u8 *huffval, s16 *dc,
                   const u8 *zigzag);
void func_02028b8c(u8 *blocks, u16 *out, u8 *clamp);

#ifdef __cplusplus
}
#endif
