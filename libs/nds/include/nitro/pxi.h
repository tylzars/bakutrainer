#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Receive callback for one FIFO tag: the tag, the 26-bit data word and the error bit.
typedef void (*PXIFifoCallback)(int tag, u32 data, BOOL err);

void PXI_Init(void);
void PxiFifoSetup(void);
void PXI_SetFifoRecvCallback(int fifoTag, PXIFifoCallback callback);
BOOL PXI_IsCallbackReady(int fifoTag, int proc);
int PXI_SendWordByFifo(int fifoTag, u32 data, BOOL err);
void PXIi_HandlerRecvFifoNotEmpty(void);

#ifdef __cplusplus
}
#endif
