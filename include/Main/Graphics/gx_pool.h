#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

void InitGxPool(int texFile, int plttFile, int c, int d);
void func_020220c8(int a);
void ResetTexCache(void);
void ResetPlttCache(void);
void ReleaseGxPool(void);

#ifdef __cplusplus
}
#endif
