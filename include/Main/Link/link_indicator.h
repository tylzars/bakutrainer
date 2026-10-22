#pragma once

#include "types.h"
#include <Main/Link/wireless_helper.h>

#ifdef __cplusplus
extern "C" {
#endif

// Work area of the link status code: mFlags is the bit set of finished steps (see WaitForLinkStep); mSendBuffer and
// mOutMessage are the two buffers of the outgoing message; the six pointers are released when the link shuts down.
// The code of link_indicator.c reaches the members through this object's base (offset in the load), while
// wireless_helper.c and system_init.c / comm_link.c name the flags and buffers as their own objects (data_020c9e04,
// data_020c9e10, data_020c9e14 and data_020c9e1c in system_init.h).
// The block is 32-byte aligned, and the compiler does not optimise the functions that use it (ShutDownLink keeps an
// unused test and WaitForLinkStep's loop is not rotated).
typedef struct LinkBuffers {
    u32 mUnk_00;
    vu16 mFlags;
    void *mUnk_08;
    void *mUnk_0c;
    // The data-sharing send and receive buffers (not gWhSendBuffer / gWhRecvBuffer).
    void *mSendBuffer;
    u16 *mRecvBuffer;
    // Records handed out by ReserveLinkMessage (gCommSendData instead under the 0x600 flags), copied into mSendBuffer
    // by EndLinkMessage.
    u8 *mOutMessage;
    // Copies of the received entries, 0x54 bytes per aid.
    u8 *mRecvCopy;
} LinkBuffers;

extern LinkBuffers gLinkBuffers __attribute__((aligned(32)));

void func_0203982c(u16 code);
void WriteIndicatorOam(int id, u32 shift, u32 index, u32 attr2, u32 attr0, int rotate, int screen);
void UpdateCommIndicator(u32 attr2, u32 attr0, int screen);
u32 GetIndicatorLayer(void);
void UpdateIndicator(void);
void func_02039b70(void);
void ShutDownLink(void);
BOOL WaitForLinkStep(u32 mask, u32 timeout);
u8 *ReserveLinkMessage(u32 size);
void EndLinkMessage(void);
void PacketSentCallback(WirelessMsg *msg);
void FirstShutDownStepCallback(WirelessMsg *msg);
void SecondShutDownStepCallback(WirelessMsg *msg);

#ifdef __cplusplus
}
#endif
