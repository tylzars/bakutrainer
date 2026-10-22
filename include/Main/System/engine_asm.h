#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// The C-callable routines of the hand-written engine object (engine_asm.s) that game code calls.
void OpenTextScreen(const char *msg);
void PrintTextScreenRow(const char *msg, int line);
void UnpackRlePart(const void *src, void *dst, u32 skip, u32 size);
u32 LeadingZeroCount(u32 x);
const void *UnpackEz(const void *src, void *dst);
void UnpackEz10(const void *src, void *dst);
void RemapPaletteRows(u16 *data, const u8 *table, u32 count);
void func_02028980(s16 *block, int comp, s16 *maxcode, u8 *valptr, u16 *mincode, u8 *huffval, s16 *dc,
                   const u8 *zigzag);
void func_02028b8c(u8 *blocks, u16 *out, u8 *clamp);

#ifdef __cplusplus
}
#endif
