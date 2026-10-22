#include <nitro/os.h>
#include <nitro/mi.h>
#include <nitro/wm.h>
#include <nitro/fx.h>
#include <Main/clib.h>
#include <Main/Graphics/texture_blocks.h>
#include <Main/System/screen_fade.h>
#include <Main/Link/comm_link.h>
#include <Main/Link/arm_system.h>
#include <Main/Graphics/text_draw.h>
#include <Main/System/system_init.h>
#include <Main/Link/wireless_helper.h>
#include <Main/Link/wireless_state.h>

// Wireless helper. The link is started only by WhConnectAsParent and WhConnectAsChild, whose only callers
// (UpdateCommParent and ConnectCommAsChild) never run in this build, so none of the steps below runs either.

// Names of the indication state codes, indexed by code.
const char *gWmStateCodeNames[27] = {
    "WM_STATECODE_PARENT_START",
    "N/A",
    "WM_STATECODE_BEACON_SENT",
    "WM_STATECODE_SCAN_START",
    "WM_STATECODE_PARENT_NOT_FOUND",
    "WM_STATECODE_PARENT_FOUND",
    "WM_STATECODE_CONNECT_START",
    "WM_STATECODE_CONNECTED",
    "WM_STATECODE_BEACON_LOST",
    "WM_STATECODE_DISCONNECTED",
    "WM_STATECODE_MP_START",
    "WM_STATECODE_MPEND_IND",
    "WM_STATECODE_MP_IND",
    "WM_STATECODE_MPACK_IND",
    "WM_STATECODE_DCF_START",
    "WM_STATECODE_DCF_IND",
    "WM_STATECODE_BEACON_RECV",
    "WM_STATECODE_DISASSOCIATE",
    "WM_STATECODE_REASSOCIATE",
    "WM_STATECODE_AUTHENTICATE",
    "WM_STATECODE_PORT_SEND",
    "WM_STATECODE_PORT_RECV",
    "WM_STATECODE_FIFO_ERROR",
    "WM_STATECODE_INFORMATION",
    "WM_STATECODE_UNKNOWN",
    "WM_STATECODE_PORT_INIT",
    "WM_STATECODE_DISCONNECTED_FROM_MYSELF",
};

// The names the tables in wireless_state.c point at, defined in this order, and four layout placeholders
// (wireless_unused0..3: nothing reads them and the link drops them). The compiler orders this file's data by size
// together with the strings of gWmStateCodeNames; with these taking part, those strings come out in the game's order.
char gWhSysStateConnectFailName[] = "WH_SYSSTATE_CONNECT_FAIL";
char gWhSysStateIdleName[] = "WH_SYSSTATE_IDLE";
char gWmApiIdPortRecvName[] = "WM_APIID_PORT_RECV";
char gWmApiIdReadStatusName[] = "WM_APIID_READ_STATUS";
const u8 wireless_unused2[19] = {0};
const u8 wireless_unused0[6] = {0};
char gWhSysStateStopName[] = "WH_SYSSTATE_STOP";
const u8 wireless_unused1[100] = {0};
char gWhSysStateConnectedName[] = "WH_SYSSTATE_CONNECTED";
char gWhSysStateErrorName[] = "WH_SYSSTATE_ERROR";
char gWhSysStateScanningName[] = "WH_SYSSTATE_SCANNING";
char gWmApiIdUnknownName[] = "WM_APIID_UNKNOWN";
char gWhSysStateKeySharingName[] = "WH_SYSSTATE_KEYSHARING";
char gWhSysStateDataSharingName[] = "WH_SYSSTATE_DATASHARING";
char gWhSysStateBusyName[] = "WH_SYSSTATE_BUSY";
char gWhSysStateMeasureChannelName[] = "WH_SYSSTATE_MEASURECHANNEL";
char gWmApiIdPortSendName[] = "WM_APIID_PORT_SEND";
const u8 wireless_unused3[2] = {0};
char gWmApiIdIndicationName[] = "WM_APIID_INDICATION";

WMDataSharing gWhDataSharing __attribute__((aligned(32)));
u8 gWhSendBuffer[0x220];
WMParentParam gWhParentParam __attribute__((aligned(32)));
u8 gWhRecvBuffer[0x480];
u16 data_020c8460[0x60] __attribute__((aligned(32)));
WMDataSharing gWhKeySharing;
u8 data_020c83e0[0x30];
u16 gWhDataSharingRecv[0x100];

#pragma thumb off

// Name of a state code, "N/A" outside the table.
const char *WhGetStateCodeName(u32 code) {
    return code < 27 ? gWmStateCodeNames[code] : "N/A";
}

// Moves the link to a new state, printing the old and the new name when the print function is set.
void WhSetSysState(int state) {
    if (gWirelessWork.mPrintFunc != 0) {
        gWirelessWork.mPrintFunc("%s -> ", gWhSysStateNames[gWirelessWork.mSysState]);
    }
    gWirelessWork.mSysState = state;
    if (gWirelessWork.mPrintFunc != 0) {
        gWirelessWork.mPrintFunc("%s\n", gWhSysStateNames[state]);
    }
}

// Records an error code unless the link is in the error or failed state; the state itself is not changed.
void WhSetError(int error) {
    if ((u32) (gWirelessWork.mSysState - WH_SYSSTATE_ERROR) > 1) {
        gWirelessWork.mError = error;
    }
}

// Starts the link's initialisation.
BOOL func_02038740(void) {
    int r;

    WhSetSysState(WH_SYSSTATE_BUSY);
    r = func_020468c4(func_02038780, &gWhParentParam);
    if (r == 2) {
        return TRUE;
    }
    WhSetError(r);
    WhSetSysState(WH_SYSSTATE_ERROR);
    return FALSE;
}

// Callback of the initialisation: on success sets the parent's key when mParentWepKeyFunc is set (never in this
// build), otherwise starts hosting.
void func_02038780(WirelessMsg *msg) {
    if (msg->mError != 0) {
        WhSetError(msg->mError);
        WhSetSysState(WH_SYSSTATE_ERROR);
        return;
    }
    if (gWirelessWork.mParentWepKeyFunc != 0) {
        if (WhSetParentWepKey() == 0) {
            WhSetSysState(WH_SYSSTATE_ERROR);
        }
    } else {
        if (WhStartParent() == 0) {
            WhSetSysState(WH_SYSSTATE_ERROR);
        }
    }
}

// Generates the parent's key through mParentWepKeyFunc and sets it. Never called in this build: the function pointer
// is never set.
BOOL WhSetParentWepKey(void) {
    int r;

    WhSetSysState(WH_SYSSTATE_BUSY);
    r = func_02047b70(WhSetParentWepKeyCallback, gWirelessWork.mParentWepKeyFunc(data_020c83e0, &gWhParentParam),
                      data_020c83e0);
    if (r == 2) {
        return TRUE;
    }
    WhSetError(r);
    WhSetSysState(WH_SYSSTATE_ERROR);
    return FALSE;
}

void WhSetParentWepKeyCallback(WirelessMsg *msg) {
    if (msg->mError != 0) {
        WhSetError(msg->mError);
        WhSetSysState(WH_SYSSTATE_ERROR);
        return;
    }
    if (WhStartParent() == 0) {
        WhSetSysState(WH_SYSSTATE_ERROR);
    }
}

// Starts hosting unless the link is already connected.
BOOL WhStartParent(void) {
    int r;

    if ((u32) (gWirelessWork.mSysState - WH_SYSSTATE_CONNECTED) <= 2) {
        return TRUE;
    }
    r = func_02046a4c(WhStartParentCallback);
    if (r != 2) {
        WhSetError(r);
        return FALSE;
    }
    gWirelessWork.mAid = 0;
    gWirelessWork.mAidBits = 1;
    return TRUE;
}

// Callback of hosting: follows children connecting and disconnecting.
void WhStartParentCallback(WirelessMsg *msg) {
    u16 bit = 1 << msg->mChildAid;

    if (msg->mError != 0) {
        WhSetError(msg->mError);
        WhSetSysState(WH_SYSSTATE_ERROR);
        return;
    }
    switch (msg->mStateCode) {
        case WM_STATECODE_CONNECTED: {
            int r;

            if (gWirelessWork.mPrintFunc != 0) {
                gWirelessWork.mPrintFunc("StartParent - new child (aid 0x%x) connected\n", msg->mChildAid);
            }
            if (gWirelessWork.mChildConnectedFunc != 0 && gWirelessWork.mChildConnectedFunc(msg) == 0) {
                r = func_02046b68(0, msg->mChildAid);
                if (r != 2) {
                    WhSetError(r);
                    WhSetSysState(WH_SYSSTATE_ERROR);
                }
                return;
            }
            gWirelessWork.mAidBits |= bit;
            return;
        }
        case WM_STATECODE_DISCONNECTED:
            if (gWirelessWork.mPrintFunc != 0) {
                gWirelessWork.mPrintFunc("StartParent - child (aid 0x%x) disconnected\n", msg->mChildAid);
            }
            if (gSystemState.mUnk_230 != 0) {
                gSystemState.mUnk_230(msg->mChildAid);
            }
            gWirelessWork.mAidBits &= ~bit;
            return;
        case WM_STATECODE_DISCONNECTED_FROM_MYSELF:
            if (gWirelessWork.mPrintFunc != 0) {
                gWirelessWork.mPrintFunc("StartParent - child (aid 0x%x) disconnected from myself\n", msg->mChildAid);
            }
            return;
        case WM_STATECODE_PARENT_START:
            if (WhStartParentMp() == 0) {
                WhSetSysState(WH_SYSSTATE_ERROR);
            }
            return;
        case WM_STATECODE_BEACON_SENT:
            return;
        default:
            if (gWirelessWork.mPrintFunc != 0) {
                gWirelessWork.mPrintFunc("unknown indicate, state = %d\n", msg->mStateCode);
            }
            return;
    }
}

// Starts the data exchange as the host unless it is already running.
BOOL WhStartParentMp(void) {
    int r;

    if (!(gWirelessWork.mSysState != WH_SYSSTATE_CONNECTED && gWirelessWork.mSysState != WH_SYSSTATE_KEYSHARING &&
          gWirelessWork.mSysState != WH_SYSSTATE_DATASHARING)) {
        return TRUE;
    }
    WhSetSysState(WH_SYSSTATE_CONNECTED);
    r = func_02046dc0(WhStartParentMpCallback, (u32) gWhRecvBuffer, (u16) gWirelessWork.mRecvBufferSize,
                      (u32) gWhSendBuffer, (u16) gWirelessWork.mSendBufferSize, 1);
    if (r == 2) {
        return TRUE;
    }
    WhSetError(r);
    return FALSE;
}

// Callback of the host's data exchange.
void WhStartParentMpCallback(WirelessMsg *msg) {
    u16 err = msg->mError;

    if (err != 0) {
        WhSetError(err);
        WhSetSysState(WH_SYSSTATE_ERROR);
        return;
    }
    switch (msg->mMpStateCode) {
        case WM_STATECODE_MP_START:
            if (gWirelessWork.mConnectMode == WH_CONNECT_MODE_PARENT_KEYSHARING) {
                if (gWirelessWork.mSysState == WH_SYSSTATE_CONNECTED) {
                    if (WhStartParentKeyShare() == 0) {
                        if (gWirelessWork.mPrintFunc != 0) {
                            gWirelessWork.mPrintFunc("WH_StateInStartParentKeyShare failed\n");
                        }
                        WhSetSysState(WH_SYSSTATE_ERROR);
                    }
                    return;
                }
                if (gWirelessWork.mSysState == WH_SYSSTATE_KEYSHARING) {
                    return;
                }
            } else if (gWirelessWork.mConnectMode == WH_CONNECT_MODE_PARENT_DATASHARING) {
                // Shares data between aid 0 (the parent) and aid 1.
                u16 mask = 3;
                int r = func_02046fc8(&gWhDataSharing, 13, mask, 84, 1);

                if (r != 0) {
                    WhSetError(r);
                    WhSetSysState(WH_SYSSTATE_ERROR);
                    return;
                }
                WhSetSysState(WH_SYSSTATE_DATASHARING);
                return;
            }
            WhSetSysState(WH_SYSSTATE_CONNECTED);
            return;
        case WM_STATECODE_MPEND_IND:
            data_020c9e04 |= 0x800;
            return;
        case WM_STATECODE_MP_IND:
        case WM_STATECODE_MPACK_IND:
        default:
            if (gWirelessWork.mPrintFunc != 0) {
                gWirelessWork.mPrintFunc("unknown indicate, state = %d\n", msg->mMpStateCode);
            }
            return;
    }
}

BOOL WhStartParentKeyShare(void) {
    int r;

    WhSetSysState(WH_SYSSTATE_KEYSHARING);
    r = func_02047b48(&gWhKeySharing, 13);
    if (r == 0) {
        return TRUE;
    }
    WhSetError(r);
    return FALSE;
}

// Ends the key sharing of the host, then the host's data exchange.
BOOL WhEndParentKeyShareAndMp(void) {
    int r = func_02047b64(&gWhKeySharing);

    if (r != 0) {
        WhSetError(r);
        return FALSE;
    }
    if (WhEndParentMp() == 0) {
        if (gWirelessWork.mPrintFunc != 0) {
            gWirelessWork.mPrintFunc("WH_StateInEndParentMP failed\n");
        }
        WhShutDownOrFail();
        return FALSE;
    }
    return TRUE;
}

// Ends the host's data exchange.
BOOL WhEndParentMp(void) {
    int r;

    WhSetSysState(WH_SYSSTATE_BUSY);
    r = func_02046f58(WhEndParentMpCallback);
    if (r == 2) {
        return TRUE;
    }
    WhSetError(r);
    return FALSE;
}

void WhEndParentMpCallback(WirelessMsg *msg) {
    if (msg->mError != 0) {
        WhSetError(msg->mError);
        WhShutDownOrFail();
        return;
    }
    if (WhEndParent() == 0) {
        if (gWirelessWork.mPrintFunc != 0) {
            gWirelessWork.mPrintFunc("WH_StateInEndParent failed\n");
        }
        WhShutDownOrFail();
    }
}

// The step after WhEndParentMp; its callback leaves the link idle.
BOOL WhEndParent(void) {
    int r = func_02046a5c(WhEndParentCallback);

    if (r == 2) {
        return TRUE;
    }
    WhSetError(r);
    return FALSE;
}

// Callback of WhEndParent: the link is idle again.
void WhEndParentCallback(WirelessMsg *msg) {
    if (msg->mError != 0) {
        WhSetError(msg->mError);
        return;
    }
    WhSetSysState(WH_SYSSTATE_IDLE);
}

// Layout placeholder: prints the two buffer sizes; nothing calls it and the link drops it. The compiler creates
// the two strings where they are first used, and the game has them at this point of the file's string order, well
// before their users WhConnectAsParent and WhConnectAsChild.
void func_unused_sizes(void) {
    gWirelessWork.mPrintFunc("recv buffer size = %d\n", gWirelessWork.mRecvBufferSize);
    gWirelessWork.mPrintFunc("send buffer size = %d\n", gWirelessWork.mSendBufferSize);
}

// Generates the child's key through mChildWepKeyFunc and sets it. Never called in this build: the function pointer
// is never set.
BOOL WhSetChildWepKey(void) {
    int r;

    WhSetSysState(WH_SYSSTATE_BUSY);
    r = func_02047b70(WhSetChildWepKeyCallback, gWirelessWork.mChildWepKeyFunc(data_020c83e0, data_020c8460),
                      data_020c83e0);
    if (r == 2) {
        return TRUE;
    }
    WhSetError(r);
    WhSetSysState(WH_SYSSTATE_ERROR);
    return FALSE;
}

void WhSetChildWepKeyCallback(WirelessMsg *msg) {
    if (msg->mError != 0) {
        WhSetError(msg->mError);
        WhSetSysState(WH_SYSSTATE_ERROR);
        return;
    }
    if (WhStartChild() == 0) {
        if (gWirelessWork.mPrintFunc != 0) {
            gWirelessWork.mPrintFunc("WH_StateOutSetChildWEPKey : startchild failed\n");
        }
        WhSetSysState(WH_SYSSTATE_ERROR);
    }
}

// Connects to the parent whose record is in data_020c8460.
BOOL WhStartChild(void) {
    int r;

    if (!(gWirelessWork.mSysState != WH_SYSSTATE_CONNECTED && gWirelessWork.mSysState != WH_SYSSTATE_KEYSHARING &&
          gWirelessWork.mSysState != WH_SYSSTATE_DATASHARING)) {
        if (gWirelessWork.mPrintFunc != 0) {
            gWirelessWork.mPrintFunc("WH_StateInStartChild : already connected?\n");
        }
        return TRUE;
    }
    WhSetSysState(WH_SYSSTATE_BUSY);
    r = func_02046a9c(WhStartChildCallback, data_020c8460, 0, 1, gWirelessWork.mChildWepKeyFunc != 0 ? 1 : 0);
    if (r == 2) {
        return TRUE;
    }
    WhSetError(r);
    return FALSE;
}

void WhStartChildCallback(WirelessMsg *msg) {
    if (msg->mError != 0) {
        WhSetError(msg->mError);
        if (msg->mError == 12) {
            gSystemState.mFlags |= 0x100;
            WhSetSysState(WH_SYSSTATE_ERROR);
            return;
        }
        if (msg->mError == 11) {
            gSystemState.mFlags |= 0x100;
            WhSetSysState(WH_SYSSTATE_ERROR);
            return;
        }
        if (msg->mError == 1) {
            gSystemState.mFlags |= 0x100;
            WhSetSysState(WH_SYSSTATE_CONNECT_FAIL);
            return;
        }
        gSystemState.mFlags |= 0x100;
        WhSetSysState(WH_SYSSTATE_ERROR);
        return;
    }
    if (msg->mStateCode == WM_STATECODE_BEACON_LOST) {
        return;
    }
    if (msg->mStateCode == WM_STATECODE_CONNECTED) {
        if (gWirelessWork.mPrintFunc != 0) {
            gWirelessWork.mPrintFunc("Connect to Parent\n");
        }
        WhSetSysState(WH_SYSSTATE_CONNECTED);
        if (WhStartChildMp() == 0) {
            if (gWirelessWork.mPrintFunc != 0) {
                gWirelessWork.mPrintFunc("WH_StateInStartChildMP failed\n");
            }
            WhSetSysState(WH_SYSSTATE_BUSY);
            return;
        }
        gWirelessWork.mAid = msg->mAssignedAid;
        return;
    }
    if (msg->mStateCode == WM_STATECODE_CONNECT_START) {
        return;
    }
    if (msg->mStateCode == WM_STATECODE_DISCONNECTED) {
        if (gWirelessWork.mPrintFunc != 0) {
            gWirelessWork.mPrintFunc("Disconnected from Parent\n");
        }
        WhSetError(20);
        WhSetSysState(WH_SYSSTATE_ERROR);
        gSystemState.mFlags |= 0x100;
        return;
    }
    if (msg->mStateCode == WM_STATECODE_DISCONNECTED_FROM_MYSELF) {
        gSystemState.mFlags |= 0x100;
        return;
    }
    if (gWirelessWork.mPrintFunc != 0) {
        gWirelessWork.mPrintFunc("unknown state %d, %s\n", msg->mStateCode, WhGetStateCodeName(msg->mStateCode));
    }
    WhSetSysState(WH_SYSSTATE_ERROR);
}

// Starts the data exchange as the child.
BOOL WhStartChildMp(void) {
    int r = func_02046dc0(WhStartChildMpCallback, (u32) gWhRecvBuffer, (u16) gWirelessWork.mRecvBufferSize,
                          (u32) gWhSendBuffer, (u16) gWirelessWork.mSendBufferSize, 1);

    if (r == 2) {
        return TRUE;
    }
    WhSetError(r);
    return FALSE;
}

// Callback of the child's data exchange.
void WhStartChildMpCallback(WirelessMsg *msg) {
    if (msg->mError != 0) {
        if (msg->mError == 15 || msg->mError == 9 || msg->mError == 13) {
            return;
        }
        WhSetError(msg->mError);
        WhShutDownOrFail();
        return;
    }
    switch (msg->mMpStateCode) {
        case WM_STATECODE_MP_START:
            if (gWirelessWork.mConnectMode == WH_CONNECT_MODE_CHILD_KEYSHARING) {
                if (gWirelessWork.mSysState == WH_SYSSTATE_KEYSHARING) {
                    return;
                }
                if (gWirelessWork.mSysState == WH_SYSSTATE_CONNECTED) {
                    if (WhStartChildKeyShare() == 0) {
                        if (gWirelessWork.mPrintFunc != 0) {
                            gWirelessWork.mPrintFunc("WH_StateInStartChildKeyShare failed\n");
                        }
                        WhFinalize();
                    }
                    return;
                }
            } else if (gWirelessWork.mConnectMode == WH_CONNECT_MODE_CHILD_DATASHARING) {
                // Shares data between aid 0 (the parent) and aid 1.
                u16 mask = 3;
                int r = func_02046fc8(&gWhDataSharing, 13, mask, 84, 1);

                if (r != 0) {
                    WhSetError(r);
                    WhFinalize();
                    return;
                }
                if (gWirelessWork.mPrintFunc != 0) {
                    gWirelessWork.mPrintFunc("WH_StateOutStartChildMP : WM_StartDataSharing OK\n");
                }
                WhSetSysState(WH_SYSSTATE_DATASHARING);
                return;
            }
            WhSetSysState(WH_SYSSTATE_CONNECTED);
            return;
        case WM_STATECODE_MP_IND:
            data_020c9e04 |= 0x800;
            return;
        case WM_STATECODE_MPACK_IND:
            break;
        case WM_STATECODE_MPEND_IND:
        default:
            if (gWirelessWork.mPrintFunc != 0) {
                gWirelessWork.mPrintFunc("unknown indicate, state = %d\n", msg->mMpStateCode);
            }
            return;
    }
}

BOOL WhStartChildKeyShare(void) {
    int r;

    if (gWirelessWork.mSysState == WH_SYSSTATE_KEYSHARING) {
        return TRUE;
    }
    if (gWirelessWork.mSysState != WH_SYSSTATE_CONNECTED) {
        return FALSE;
    }
    WhSetSysState(WH_SYSSTATE_KEYSHARING);
    r = func_02047b48(&gWhKeySharing, 13);
    if (r == 0) {
        return TRUE;
    }
    WhSetError(r);
    return FALSE;
}

// Ends the key sharing of the child, then the child's data exchange.
BOOL WhEndChildKeyShareAndMp(void) {
    int r;
    BOOL ok;

    if (gWirelessWork.mSysState != WH_SYSSTATE_KEYSHARING) {
        return FALSE;
    }
    WhSetSysState(WH_SYSSTATE_BUSY);
    r = func_02047b64(&gWhKeySharing);
    if (r != 0) {
        WhSetError(r);
        ok = FALSE;
    } else if (WhEndChildMp() == 0) {
        ok = FALSE;
    } else {
        ok = TRUE;
    }
    return ok;
}

// Ends the child's data exchange.
BOOL WhEndChildMp(void) {
    int r;

    WhSetSysState(WH_SYSSTATE_BUSY);
    r = func_02046f58(WhEndChildMpCallback);
    if (r == 2) {
        return TRUE;
    }
    WhSetError(r);
    return FALSE;
}

void WhEndChildMpCallback(WirelessMsg *msg) {
    if (msg->mError != 0) {
        WhSetError(msg->mError);
        WhFinalize();
        return;
    }
    if (WhEndChild() == 0) {
        WhSetSysState(WH_SYSSTATE_ERROR);
    }
}

// Disconnects from aid 0 (the parent): the call the parent uses to drop a child; its callback leaves the link idle.
BOOL WhEndChild(void) {
    int r;

    WhSetSysState(WH_SYSSTATE_BUSY);
    r = func_02046b68(WhEndChildCallback, 0);
    if (r == 2) {
        return TRUE;
    }
    WhSetError(r);
    WhShutDownOrFail();
    return FALSE;
}

// Callback of WhEndChild: the link is idle again.
void WhEndChildCallback(WirelessMsg *msg) {
    if (msg->mError != 0) {
        WhSetError(msg->mError);
        return;
    }
    WhSetSysState(WH_SYSSTATE_IDLE);
}

// Shuts the link down.
BOOL WhStartShutDown(void) {
    int r;

    WhSetSysState(WH_SYSSTATE_BUSY);
    r = func_0204684c(WhShutDownCallback);
    if (r == 2) {
        return TRUE;
    }
    WhSetError(r);
    return FALSE;
}

// Callback of WhStartShutDown. An error code is not recorded: the state is set to the error state first, in which
// WhSetError does not store.
void WhShutDownCallback(WirelessMsg *msg) {
    if (msg->mError != 0) {
        WhSetSysState(WH_SYSSTATE_ERROR);
        WhSetError(msg->mError);
        return;
    }
    WhSetSysState(WH_SYSSTATE_IDLE);
}

int WhGetSysState(void) {
    return gWirelessWork.mSysState;
}

// Sets up the host (b and c go into the parent parameter block) and starts the link in the given parent mode (the
// even modes); returns whether it started, 0 for any other mode. Stops the system unless the link is idle; that
// check is an expression rather than an `if` because the compiler's output needs it (it loads the 0 an `if` drops).
int WhConnectAsParent(int mode, u16 b, u16 c) {
    (void) (gWirelessWork.mSysState == WH_SYSSTATE_IDLE || (OS_Terminate(), 0));
    gWirelessWork.mRecvBufferSize = 0x240;
    gWirelessWork.mSendBufferSize = 0x220;
    if (gWirelessWork.mPrintFunc != 0) {
        gWirelessWork.mPrintFunc("recv buffer size = %d\n", gWirelessWork.mRecvBufferSize);
    }
    if (gWirelessWork.mPrintFunc != 0) {
        gWirelessWork.mPrintFunc("send buffer size = %d\n", gWirelessWork.mSendBufferSize);
    }
    gWirelessWork.mConnectMode = mode;
    WhSetSysState(WH_SYSSTATE_BUSY);
    gWhParentParam.mUnk_0c = b;
    gWhParentParam.mUnk_32 = c;
    gWhParentParam.mUnk_18 = func_020467bc();
    gWhParentParam.mUnk_34 = 0x200;
    gWhParentParam.mUnk_36 = 0xfe;
    gWhParentParam.mUnk_10 = 1;
    gWhParentParam.mUnk_16 = 0;
    gWhParentParam.mUnk_12 = 0;
    gWhParentParam.mUnk_0e = 1;
    gWhParentParam.mUnk_14 = mode == WH_CONNECT_MODE_PARENT_KEYSHARING;
    if (mode == WH_CONNECT_MODE_PARENT || mode == WH_CONNECT_MODE_PARENT_KEYSHARING ||
        mode == WH_CONNECT_MODE_PARENT_DATASHARING) {
        return func_02038740();
    }
    if (gWirelessWork.mPrintFunc != 0) {
        gWirelessWork.mPrintFunc("unknown connect mode %d\n", mode);
    }
    return 0;
}

// Starts the link as a child in the given child mode (the odd modes), to the parent whose record is data; returns
// whether it started, 0 for any other mode. Stops the system unless the link is idle (an expression, as in
// WhConnectAsParent).
int WhConnectAsChild(int mode, const void *data) {
    (void) (gWirelessWork.mSysState == WH_SYSSTATE_IDLE || (OS_Terminate(), 0));
    gWirelessWork.mRecvBufferSize = 0x480;
    gWirelessWork.mSendBufferSize = 0x100;
    if (gWirelessWork.mPrintFunc != 0) {
        gWirelessWork.mPrintFunc("recv buffer size = %d\n", gWirelessWork.mRecvBufferSize);
    }
    if (gWirelessWork.mPrintFunc != 0) {
        gWirelessWork.mPrintFunc("send buffer size = %d\n", gWirelessWork.mSendBufferSize);
    }
    gWirelessWork.mConnectMode = mode;
    WhSetSysState(WH_SYSSTATE_BUSY);
    if (mode == WH_CONNECT_MODE_CHILD || mode == WH_CONNECT_MODE_CHILD_KEYSHARING ||
        mode == WH_CONNECT_MODE_CHILD_DATASHARING) {
        MI_CpuCopy8(data, data_020c8460, 0xc0);
        DC_FlushRange(data_020c8460, 0xc0);
        DC_WaitWriteBufferEmpty();
        if (gWirelessWork.mChildWepKeyFunc != 0) {
            return WhSetChildWepKey();
        }
        return WhStartChild();
    }
    if (gWirelessWork.mPrintFunc != 0) {
        gWirelessWork.mPrintFunc("unknown connect mode %d\n", mode);
    }
    return 0;
}

// The data received from child n, or NULL.
u8 *WhGetSharedData(u32 n) {
    return func_02047ac4(&gWhDataSharing, gWhDataSharingRecv, n);
}

// Runs one step of the data exchange, passing the data to send; returns whether this frame's exchange went through
// (result 0 or 7), not whether the sharing is done. Other results are recorded with WhSetError.
BOOL WhUpdateDataSharing(void *buf) {
    int r = func_02047264(&gWhDataSharing, buf, gWhDataSharingRecv);

    if (r == 7) {
        return TRUE;
    }
    if (r == 5) {
        WhSetError(r);
        return FALSE;
    }
    if (r == 0) {
        return TRUE;
    }
    WhSetError(r);
    return FALSE;
}

// Shuts the link down; if that cannot be started the link enters the failed state.
void WhShutDownOrFail(void) {
    if (WhStartShutDown() == 0) {
        WhSetSysState(WH_SYSSTATE_FAILED);
    }
}

// Ends the current connection according to the mode, then shuts the link down on failure.
void WhFinalize(void) {
    if (gWirelessWork.mSysState == WH_SYSSTATE_IDLE) {
        if (gWirelessWork.mPrintFunc != 0) {
            gWirelessWork.mPrintFunc("already WH_SYSSTATE_IDLE\n");
        }
        return;
    }
    if (gWirelessWork.mPrintFunc != 0) {
        gWirelessWork.mPrintFunc("WH_Finalize, state = %d\n", gWirelessWork.mSysState);
    }
    if (gWirelessWork.mSysState != WH_SYSSTATE_KEYSHARING && gWirelessWork.mSysState != WH_SYSSTATE_DATASHARING &&
        gWirelessWork.mSysState != WH_SYSSTATE_CONNECTED) {
        WhSetSysState(WH_SYSSTATE_BUSY);
        WhShutDownOrFail();
        return;
    }
    WhSetSysState(WH_SYSSTATE_BUSY);
    switch (gWirelessWork.mConnectMode) {
        case WH_CONNECT_MODE_CHILD_KEYSHARING:
            if (WhEndChildKeyShareAndMp() == 0) {
                WhShutDownOrFail();
            }
            return;
        case WH_CONNECT_MODE_CHILD:
        case WH_CONNECT_MODE_CHILD_DATASHARING:
            if (WhEndChildMp() == 0) {
                WhShutDownOrFail();
            }
            return;
        case WH_CONNECT_MODE_PARENT_KEYSHARING:
            if (WhEndParentKeyShareAndMp() == 0) {
                WhShutDownOrFail();
            }
            return;
        case WH_CONNECT_MODE_PARENT:
        case WH_CONNECT_MODE_PARENT_DATASHARING:
            if (WhEndParentMp() == 0) {
                WhShutDownOrFail();
            }
            return;
        default:
            return;
    }
}

// The id the link assigned to this unit.
u16 WhGetAid(void) {
    return gWirelessWork.mAid;
}
