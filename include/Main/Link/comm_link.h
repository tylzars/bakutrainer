#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// State block of the link; the halfword at 0x0c is written by func_020350e8 and returned by func_020350f8.
typedef struct Unk_020c6adc {
    u8 mUnk_00[0xc];
    u16 mUnk_0c;
    u8 mUnk_0e[0x38 - 0xe];
} Unk_020c6adc;

extern Unk_020c6adc data_020c6adc;

void ExchangeCommRecords(void);
void CountUnchangedSlotMask(void);
void ClearCommSendData(void);
void ShowCommError(int a);
void HaltOnCommError(void);
void UpdateCommChild(void);
void ConnectCommAsChild(void);
void func_02034cdc(void);
void WriteU32AsHalfwords(u32 value, void *dst);
u32 ReadU32FromHalfwords(void *src);
void WriteU32Bytes(u32 value, u8 *dst);
void ServeCommFiles(void);
int IsCommReading(void);
void func_020350a0(void);
void func_020350e8(u16 value);
int func_020350f8(void);

#ifdef __cplusplus
}
#endif
