#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// BIOS call: busy-waits for count loop iterations.
void WaitByLoop(u32 count);

// BIOS call: copies or fills (bit 24 of control) count units of 16 or 32 bits (bit 26) from src to dest.
void CpuSet(const void *src, void *dest, u32 control);

#ifdef __cplusplus
}
#endif
