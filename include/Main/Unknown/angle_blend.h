#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

void BlendAngles(u32 t, u32 shift, u16 *cur, const u16 *target);
void BlendVec3(int t, int shift, s32 *dst, const s32 *target);

#ifdef __cplusplus
}
#endif
