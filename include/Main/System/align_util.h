#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

u32 AlignUp32(u32 x);
void *AlignPtrUp4(void *p);
void *AlignPtrUp2(void *p);

#ifdef __cplusplus
}
#endif
