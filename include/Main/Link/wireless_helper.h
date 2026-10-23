#pragma once

#include "types.h"
#include <nitro/wm.h>

#ifdef __cplusplus
extern "C" {
#endif

// States of the link (WirelessWork.mSysState); the names are the ones the game prints.
#define WH_SYSSTATE_STOP 0
#define WH_SYSSTATE_IDLE 1
#define WH_SYSSTATE_SCANNING 2
#define WH_SYSSTATE_BUSY 3
#define WH_SYSSTATE_CONNECTED 4
#define WH_SYSSTATE_DATASHARING 5
#define WH_SYSSTATE_KEYSHARING 6
#define WH_SYSSTATE_MEASURECHANNEL 7
#define WH_SYSSTATE_CONNECT_FAIL 8
#define WH_SYSSTATE_ERROR 9
// Entered when shutting the link down fails; the game has no name for it.
#define WH_SYSSTATE_FAILED 10

// State codes of the link indications (the message field after the error code); the names are the ones the game
// prints.
#define WM_STATECODE_PARENT_START 0
#define WM_STATECODE_BEACON_SENT 2
#define WM_STATECODE_CONNECT_START 6
#define WM_STATECODE_CONNECTED 7
#define WM_STATECODE_BEACON_LOST 8
#define WM_STATECODE_DISCONNECTED 9
#define WM_STATECODE_MP_START 10
#define WM_STATECODE_MPEND_IND 11
#define WM_STATECODE_MP_IND 12
#define WM_STATECODE_MPACK_IND 13
#define WM_STATECODE_DISCONNECTED_FROM_MYSELF 26

// Connect modes: the mode of WhConnectAsParent (parent modes) and WhConnectAsChild (child modes), kept as the wireless
// work's connect mode. The key-sharing and data-sharing modes start that sharing when the MP exchange starts. Only
// the two data-sharing modes are passed in this build, by UpdateCommParent and ConnectCommAsChild, which never run.
#define WH_CONNECT_MODE_PARENT 0
#define WH_CONNECT_MODE_CHILD 1
#define WH_CONNECT_MODE_PARENT_KEYSHARING 2
#define WH_CONNECT_MODE_CHILD_KEYSHARING 3
#define WH_CONNECT_MODE_PARENT_DATASHARING 4
#define WH_CONNECT_MODE_CHILD_DATASHARING 5

// Message block that the wireless callbacks receive.
typedef struct WirelessMsg {
    u16 mUnk_00;
    u16 mError;
    u16 mMpStateCode;
    u16 mUnk_06;
    u16 mStateCode;
    u16 mAssignedAid;
    u8 mUnk_0c[4];
    u16 mChildAid;
} WirelessMsg;

// State of the wireless link (0x60 bytes).
typedef struct WirelessWork {
    u16 mUnk_00;
    // Bit per aid. Bit 0 (this unit, aid 0) is set when hosting starts. A child's bit is set when it connects and
    // cleared when it disconnects. Nothing reads it in this build.
    u16 mAidBits;
    u16 mAid;
    u8 mUnk_06[0x16];
    // Fills the key block (first argument) for the parent / the child and returns the key mode. The second argument is
    // the parent parameter block / the child connect data. Neither this nor mChildWepKeyFunc is set in this build.
    u32 (*mParentWepKeyFunc)(void *key, void *param);
    u32 mSendBufferSize;
    // Last error code recorded by WhSetError; nothing reads it in this build.
    int mError;
    u32 (*mChildWepKeyFunc)(void *key, void *param);
    u8 mUnk_2c[4];
    // Print function, called with a format and its values when set; never set in this build.
    void (*mPrintFunc)(const char *fmt, ...);
    int mConnectMode;
    int mSysState;
    u32 mRecvBufferSize;
    // Asked when a child connects: nonzero accepts it. Never set in this build, so every child is accepted.
    int (*mChildConnectedFunc)(WirelessMsg *msg);
    u8 mUnk_44[0x1c];
} WirelessWork;

// Names of the api ids (WM_APIID_UNKNOWN .. WM_APIID_READ_STATUS), then of the link states (WH_SYSSTATE_STOP ..
// WH_SYSSTATE_ERROR); the tables in wireless_state.c point at them.
extern char gWmApiIdUnknownName[];
extern char gWmApiIdIndicationName[];
extern char gWmApiIdPortSendName[];
extern char gWmApiIdPortRecvName[];
extern char gWmApiIdReadStatusName[];
extern char gWhSysStateStopName[];
extern char gWhSysStateIdleName[];
extern char gWhSysStateScanningName[];
extern char gWhSysStateBusyName[];
extern char gWhSysStateConnectedName[];
extern char gWhSysStateDataSharingName[];
extern char gWhSysStateKeySharingName[];
extern char gWhSysStateMeasureChannelName[];
extern char gWhSysStateConnectFailName[];
extern char gWhSysStateErrorName[];

// Key block mParentWepKeyFunc / mChildWepKeyFunc fill for WhSetParentWepKey / WhSetChildWepKey.
extern u8 gWhWepKey[0x30];
// The parent parameter block, the child connect data and the data-sharing block handed to the wireless library are
// 32-byte aligned. The child connect data is flushed from the data cache by range; the game code passes it on
// without reading it.
extern WMParentParam gWhParentParam __attribute__((aligned(32)));
extern u16 gWhChildConnectData[0x60] __attribute__((aligned(32)));
extern u16 gWhDataSharingRecv[0x100];
extern u8 gWhSendBuffer[0x220];
extern u8 gWhRecvBuffer[0x480];
extern WMDataSharing gWhKeySharing;
extern WMDataSharing gWhDataSharing __attribute__((aligned(32)));

const char *WhGetStateCodeName(u32 code);
void WhSetSysState(int state);
void WhSetError(int error);
BOOL WhSetParentParam(void);
void WhSetParentParamCallback(WirelessMsg *msg);
BOOL WhSetParentWepKey(void);
void WhSetParentWepKeyCallback(WirelessMsg *msg);
BOOL WhStartParent(void);
void WhStartParentCallback(WirelessMsg *msg);
BOOL WhStartParentMp(void);
void WhStartParentMpCallback(WirelessMsg *msg);
BOOL WhStartParentKeyShare(void);
BOOL WhEndParentKeyShareAndMp(void);
BOOL WhEndParentMp(void);
void WhEndParentMpCallback(WirelessMsg *msg);
BOOL WhEndParent(void);
void WhEndParentCallback(WirelessMsg *msg);
BOOL WhSetChildWepKey(void);
void WhSetChildWepKeyCallback(WirelessMsg *msg);
BOOL WhStartChild(void);
void WhStartChildCallback(WirelessMsg *msg);
BOOL WhStartChildMp(void);
void WhStartChildMpCallback(WirelessMsg *msg);
BOOL WhStartChildKeyShare(void);
BOOL WhEndChildKeyShareAndMp(void);
BOOL WhEndChildMp(void);
void WhEndChildMpCallback(WirelessMsg *msg);
BOOL WhEndChild(void);
void WhEndChildCallback(WirelessMsg *msg);
BOOL WhStartShutDown(void);
void WhShutDownCallback(WirelessMsg *msg);
int WhGetSysState(void);
int WhConnectAsParent(int mode, u16 b, u16 c);
int WhConnectAsChild(int mode, const void *data);
u8 *WhGetSharedData(u32 n);
BOOL WhUpdateDataSharing(void *buf);
void WhShutDownOrFail(void);
void WhFinalize(void);
u16 WhGetAid(void);

#ifdef __cplusplus
}
#endif
