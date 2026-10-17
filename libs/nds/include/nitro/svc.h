#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// BIOS call: busy-waits for count loop iterations.
void WaitByLoop(u32 count);

#ifdef __cplusplus
}
#endif
