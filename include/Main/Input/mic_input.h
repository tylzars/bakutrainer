// Side header of system_init.c: its microphone capture and loudness analysis.
#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Microphone capture and loudness analysis.
#define MIC_FLAG_ACTIVE 0x800
#define MIC_MAX_BANDS 8
#define MIC_HISTORY 16

void SetupMic(int size, int b);
void func_02021054(int a);
void StartMicSampling(int rate, int type);
void StopMicSampling(void);
u32 GetMicSampleAddr(void);
void CopyMicSamples(u32 count);
void ClearMicBands(void);
void SumMicBands(const u8 *spectrum, const u16 *ranges, u32 count, u32 *totals, u32 *sums);
void ComputeMicSpectrum(u8 *out, u32 nShift);
u32 GetMicSpectrumIndex(u32 x);
u32 GetMicBandAverage(u32 index);
void UpdateMicBands(void);
int func_020213e8(u32 a);
void ReleaseMic(void);

#ifdef __cplusplus
}
#endif
