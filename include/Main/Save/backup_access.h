// Side header of system_init.c: its backup memory reads and writes.
#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

void InitBackup(u32 a);
int WriteBackup(const void *buf, u32 address, u32 size);

int ReadBackup(void *buf, u32 address, u32 size);
#ifdef __cplusplus
}
#endif
