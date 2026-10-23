#include <Main/Link/wireless_state.h>

// Names of the api ids (nothing reads this table).
const char *gWmApiIdNames[5] = {
    gWmApiIdUnknownName, gWmApiIdIndicationName, gWmApiIdPortSendName, gWmApiIdPortRecvName, gWmApiIdReadStatusName,
};

// Names of the link states, indexed by state. WH_SYSSTATE_FAILED (10) is past its end. Only the print function reads
// it.
const char *gWhSysStateNames[10] = {
    gWhSysStateStopName,        gWhSysStateIdleName,        gWhSysStateScanningName,   gWhSysStateBusyName,
    gWhSysStateConnectedName,   gWhSysStateDataSharingName, gWhSysStateKeySharingName, gWhSysStateMeasureChannelName,
    gWhSysStateConnectFailName, gWhSysStateErrorName,
};

WirelessWork gWirelessWork;
