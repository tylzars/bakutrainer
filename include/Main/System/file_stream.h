#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

void SeekBank(int offset);
void ReadBank(void *dst, u32 size);

#ifdef __cplusplus
}
#endif
