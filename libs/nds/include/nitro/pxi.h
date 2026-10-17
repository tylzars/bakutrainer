#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

void PXI_Init(void);
void PXI_SetFifoRecvCallback(int fifoTag, void (*callback)(int tag, u32 data, BOOL err));
BOOL PXI_IsCallbackReady(int fifoTag, int proc);

#ifdef __cplusplus
}
#endif
