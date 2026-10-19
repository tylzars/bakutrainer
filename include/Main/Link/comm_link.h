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

void func_02034954(void);
void func_02034a08(void);
void func_02034a44(void);
void func_02034a60(int a);
void func_02034be4(void);
void func_02034c38(void);
void func_02034cc4(void);
void func_02034cdc(void);
void func_02034ce0(u32 value, void *dst);
u32 func_02034cf0(void *src);
void func_02034d00(u32 value, u8 *dst);
void func_02034d1c(void);
int func_02035058(void);
void func_020350a0(void);
void func_020350e8(u16 value);
int func_020350f8(void);

#ifdef __cplusplus
}
#endif
