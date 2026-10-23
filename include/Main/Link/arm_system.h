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
    // Buffer of the packet ServeCommFiles sends; never assigned in this build (only freed), so it stays NULL.
    u8 *mPacket;
} StateC;
// Declared without a size. ServeCommFiles loads mPacket ahead of a store to data_020c9e04; the compiler only allows
// that when the declared object is not a complete type of 16 bytes or less.
extern StateC data_020c66d4[];
extern u8 gCommSendData[] __attribute__((aligned(32)));
extern u8 gCommRecords[] __attribute__((aligned(32)));
extern u32 gCommRecordReceived[] __attribute__((aligned(32)));
extern u8 gCommConnectData[] __attribute__((aligned(32)));

void ReleaseComm(void);
BOOL func_020010b4(u32 timeout);
void ShutDownComm(void);
void UpdateCommParent(void);

#ifdef __cplusplus
}
#endif
