#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// The link's message state and its 32-byte aligned buffers.
typedef struct StateC {
    u16 mUnk_0;
    u16 mUnk_2;
    u16 *mUnk_4;
    u8 *mUnk_8;
} StateC;
// Declared without a size: func_02034d1c loads mUnk_8 ahead of a store to data_020c9e04, which the compiler only
// allows when the declared object is not a complete type of 16 bytes or less.
extern StateC data_020c66d4[];
extern u8 data_020c67e0[] __attribute__((aligned(32)));
extern u8 data_020c68e0[] __attribute__((aligned(32)));
extern u32 data_020c66e0[] __attribute__((aligned(32)));
extern u8 data_020c6720[] __attribute__((aligned(32)));

void func_02001000(void);
BOOL func_020010b4(u32 timeout);
void func_02001120(void);
void func_02001170(void);

#ifdef __cplusplus
}
#endif
