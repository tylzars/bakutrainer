#pragma once

#include "types.h"
#include <Main/Link/wireless_helper.h>

#ifdef __cplusplus
extern "C" {
#endif

// Work area of the link status code: +4 is the bit set of finished steps (see func_02039c88); +0x10 and +0x18 are the
// two buffers of the outgoing message; +0x8, +0xc, +0x10, +0x14, +0x18 and +0x1c are released when the link shuts down.
// The code of link_indicator.c reaches the members through this object's base (offset in the load), while
// wireless_helper.c and system_init.c / comm_link.c name the flags and buffers as their own objects (data_020c9e04,
// data_020c9e10, data_020c9e14 and data_020c9e1c in system_init.h).
// The block is 32-byte aligned, and the compiler does not optimise the functions that use it (func_02039ba8 keeps an
// unused test and func_02039c88's loop is not rotated).
typedef struct Unk_LinkBuffers {
    u32 mUnk_00;
    vu16 mUnk_04;
    void *mUnk_08;
    void *mUnk_0c;
    void *mUnk_10;
    u16 *mUnk_14;
    u8 *mUnk_18;
    u8 *mUnk_1c;
} Unk_LinkBuffers;

extern Unk_LinkBuffers data_020c9e00 __attribute__((aligned(32)));

void func_0203982c(u16 code);
void func_02039830(int id, u32 shift, u32 index, u32 attr2, u32 attr0, int rotate, int screen);
void func_02039908(u32 attr2, u32 attr0, int screen);
u32 func_02039a64(void);
void func_02039a94(void);
void func_02039b70(void);
void func_02039ba8(void);
BOOL func_02039c88(u32 mask, u32 timeout);
u8 *func_02039cd4(u32 size);
void func_02039d24(void);
void func_02039d74(Unk_WirelessMsg *msg);
void func_02039d98(Unk_WirelessMsg *msg);
void func_02039db8(Unk_WirelessMsg *msg);

#ifdef __cplusplus
}
#endif
