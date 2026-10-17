#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Allocates size bytes from the game heap (NULL when it is full), and frees such a block.
void *func_0202f500(u32 size);
void func_0202f50c(void *p);

#ifdef __cplusplus
}
#endif
