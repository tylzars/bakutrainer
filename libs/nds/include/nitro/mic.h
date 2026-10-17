#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Sampling request: buffer and size must be 32-byte aligned; loop selects repeating sampling;
// full is called when the buffer fills.
typedef struct {
    /* 00 */ u32 type;
    /* 04 */ void *buffer;
    /* 08 */ u32 size;
    /* 0c */ u32 rate;
    /* 10 */ u32 loop;
    /* 14 */ void (*full)(u32 result, void *arg);
    /* 18 */ void *fullArg;
} MICAutoParam;

void MIC_Init(void);
u32 MIC_StartAutoSamplingAsync(MICAutoParam *param, void (*callback)(u32 result, void *arg), void *arg);
u32 MIC_StartAutoSampling(MICAutoParam *param);
u32 MIC_StopAutoSamplingAsync(void (*callback)(u32 result, void *arg), void *arg);
u32 MIC_StopAutoSampling(void);
u32 func_02043454(void);
void func_02043464(int tag, u32 data, BOOL err);
BOOL func_0204358c(u32 buffer, u32 size, u32 rate, u32 type);
BOOL func_02043690(void);
void func_020436b8(u32 result, void *arg);
void func_020436c8(void);

#ifdef __cplusplus
}
#endif
