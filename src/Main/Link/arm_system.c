#include <Main/clib.h>
#include <Main/System/system_init.h>
#include <Main/Link/arm_system.h>
#include <Main/Link/comm_link.h>
#include <Main/Link/wireless_helper.h>
#include <Main/Link/link_indicator.h>

StateC data_020c66d4[1];
u32 gCommRecordReceived[16] __attribute__((aligned(32)));
u8 gCommConnectData[0xc0] __attribute__((aligned(32)));
u8 gCommSendData[0x100] __attribute__((aligned(32)));
u8 gCommRecords[0x1fc] __attribute__((aligned(32)));

#pragma thumb off

// The host side of the file link in comm_link.c. Like the rest of that link it does not run in this build: UpdateSystem
// calls UpdateCommParent only while gSystemState.mFlags has 0x400 set, which nothing sets.

void ReleaseComm(void) {
    Free(&data_020c66d4->mPacket);
    Free(&gSystemState.mUnk_250.mUnk_24);
    memset(&gSystemState.mUnk_250, 0, 0x2c);
    gSystemState.mConnectAttemptCount = 0;
    gSystemState.mUnk_2a8 = 0;
    gSystemState.mUnk_296 = 0;
    gSystemState.mUnk_292 = 0;
    memset(gCommSendData, 0, 0xfe);
    memset(gCommRecords, 0, 0x1fc);
    memset(gCommRecordReceived, 0, 0x40);
    memset(gCommConnectData, 0, 0xc0);
    data_020c66d4->mUnk_4 = 0;
}

BOOL func_020010b4(u32 timeout) {
    u32 start = gSystemState.mElapsedFrames;
    u32 last = gSystemState.mElapsedFrames;

    if (func_020350f8() != 0) {
        do {
            if (gSystemState.mElapsedFrames != last) {
                UpdateSystem(3);
                last = gSystemState.mElapsedFrames;
            }
            if (gSystemState.mElapsedFrames - start > timeout) {
                return FALSE;
            }
        } while (func_020350f8() != 0);
    }
    return TRUE;
}

// Shuts the file link down. Unless the link state (func_020350f8) is 0 or 5, it first runs func_020350a0 and waits up
// to 0x3c frames for the state to clear. Then releases the link's buffers and shuts the link down. In this build the
// state stays 0, so the wait never runs; and the function is never reached, as its caller needs mUnk_2a6_1, which is
// never set.
void ShutDownComm(void) {
    gSystemState.mUnk_2a6_1 = 0;
    gSystemState.mUnk_2a3 = 0x10;
    if (func_020350f8() != 5 && func_020350f8() != 0) {
        func_020350a0();
        func_020010b4(0x3c);
    }
    ReleaseComm();
    ShutDownLink();
}

void UpdateCommParent(void) {
    switch (WhGetSysState()) {
        case WH_SYSSTATE_IDLE:
            WhConnectAsParent(WH_CONNECT_MODE_PARENT_DATASHARING, gSystemState.mUnk_294 + 1, data_020c66d4->mUnk_0);
            break;
        case WH_SYSSTATE_CONNECTED:
        case WH_SYSSTATE_DATASHARING:
        case WH_SYSSTATE_KEYSHARING:
            ServeCommFiles();
            ExchangeCommRecords();
            break;
    }
}

#pragma thumb on
