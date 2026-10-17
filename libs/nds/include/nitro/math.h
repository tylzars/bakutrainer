#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline u32 CountLeadingZeros(u32 x) {
    u32 result;
    asm { clz result, x }
    return result;
}

u32 func_0204873c(u32 x);
void func_02048780(u16 *table, u16 poly);
void func_020487c4(const u16 *table, u16 *currentCrc, const void *data, u32 dataLength);
u16 func_02048804(const u16 *table, const void *data, u32 dataLength);
void func_0204882c(fx16 *sinTable, u32 nShift);
void func_02048880(fx32 *data, u32 nShift, const fx16 *sinTable);
void func_02048af8(fx32 *data, u32 nShift, const fx16 *sinTable);

#ifdef __cplusplus
}
#endif
