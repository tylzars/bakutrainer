#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Save and restore the divider and square-root unit registers (7 words).
void func_02042898(void *cpContext);
void func_020428d8(const void *cpContext);

#ifdef __cplusplus
}
#endif
