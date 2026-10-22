#include <nitro/os.h>
#include <nitro/mi.h>
#include <nitro/wm.h>
#include <nitro/fx.h>
#include <Main/clib.h>
#include <Main/Graphics/texture_blocks.h>
#include <Main/System/screen_fade.h>
#include <Main/Link/comm_link.h>
#include <Main/Unknown/arm_system.h>
#include <Main/Graphics/text_draw.h>
#include <Main/System/system_init.h>
#include <Main/Link/wireless_helper.h>
#include <Main/Link/wireless_state.h>

// Names of the indication state codes, indexed by code.
const char *data_02064108[27] = {
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
// together with the strings of data_02064108; with these taking part, those strings come out in the game's order.
char data_02063f54[] = "WH_SYSSTATE_CONNECT_FAIL";
char data_02063d08[] = "WH_SYSSTATE_IDLE";
char data_02063d80[] = "WM_APIID_PORT_RECV";
char data_02063dec[] = "WM_APIID_READ_STATUS";
const u8 wireless_unused2[19] = {0};
const u8 wireless_unused0[6] = {0};
char data_02063d30[] = "WH_SYSSTATE_STOP";
const u8 wireless_unused1[100] = {0};
char data_02063e34[] = "WH_SYSSTATE_CONNECTED";
char data_02063d58[] = "WH_SYSSTATE_ERROR";
char data_02063e04[] = "WH_SYSSTATE_SCANNING";
char data_02063d1c[] = "WM_APIID_UNKNOWN";
char data_02063e4c[] = "WH_SYSSTATE_KEYSHARING";
char data_02063f0c[] = "WH_SYSSTATE_DATASHARING";
char data_02063d44[] = "WH_SYSSTATE_BUSY";
char data_02064088[] = "WH_SYSSTATE_MEASURECHANNEL";
char data_02063d6c[] = "WM_APIID_PORT_SEND";
const u8 wireless_unused3[2] = {0};
char data_02063da8[] = "WM_APIID_INDICATION";

WMDataSharing data_020c95e0 __attribute__((aligned(32)));
u8 data_020c8720[0x220];
WMParentParam data_020c8420 __attribute__((aligned(32)));
u8 data_020c8940[0x480];
u16 data_020c8460[0x60] __attribute__((aligned(32)));
WMDataSharing data_020c8dc0;
u8 data_020c83e0[0x30];
u16 data_020c8520[0x100];

#pragma thumb off

// Name of a state code, "N/A" outside the table.
const char *func_020386a4(u32 code) {
    return code < 27 ? data_02064108[code] : "N/A";
}

// Moves the link to a new state, printing the old and the new name when the debug print is set.
void func_020386c0(int state) {
    if (data_020c8380.mUnk_30 != 0) {
        data_020c8380.mUnk_30("%s -> ", data_02063cdc[data_020c8380.mUnk_38]);
    }
    data_020c8380.mUnk_38 = state;
    if (data_020c8380.mUnk_30 != 0) {
        data_020c8380.mUnk_30("%s\n", data_02063cdc[state]);
    }
}

// Records an error code unless the link is already in the error state.
void func_02038724(int error) {
    if ((u32) (data_020c8380.mUnk_38 - WH_SYSSTATE_ERROR) > 1) {
        data_020c8380.mUnk_24 = error;
    }
}

// Starts the link's initialisation.
BOOL func_02038740(void) {
    int r;

    func_020386c0(WH_SYSSTATE_BUSY);
    r = func_020468c4(func_02038780, &data_020c8420);
    if (r == 2) {
        return TRUE;
    }
    func_02038724(r);
    func_020386c0(WH_SYSSTATE_ERROR);
    return FALSE;
}

// Callback of the initialisation: on success sets the parent's or the child's key.
void func_02038780(Unk_WirelessMsg *msg) {
    if (msg->mUnk_02 != 0) {
        func_02038724(msg->mUnk_02);
        func_020386c0(WH_SYSSTATE_ERROR);
        return;
    }
    if (data_020c8380.mUnk_1c != 0) {
        if (func_020387e4() == 0) {
            func_020386c0(WH_SYSSTATE_ERROR);
        }
    } else {
        if (func_0203887c() == 0) {
            func_020386c0(WH_SYSSTATE_ERROR);
        }
    }
}

// Generates the parent's key and sets it.
BOOL func_020387e4(void) {
    int r;

    func_020386c0(WH_SYSSTATE_BUSY);
    r = func_02047b70(func_02038844, data_020c8380.mUnk_1c(data_020c83e0, &data_020c8420), data_020c83e0);
    if (r == 2) {
        return TRUE;
    }
    func_02038724(r);
    func_020386c0(WH_SYSSTATE_ERROR);
    return FALSE;
}

// Callback of the key set by func_020387e4.
void func_02038844(Unk_WirelessMsg *msg) {
    if (msg->mUnk_02 != 0) {
        func_02038724(msg->mUnk_02);
        func_020386c0(WH_SYSSTATE_ERROR);
        return;
    }
    if (func_0203887c() == 0) {
        func_020386c0(WH_SYSSTATE_ERROR);
    }
}

// Starts hosting unless the link is already connected.
BOOL func_0203887c(void) {
    int r;

    if ((u32) (data_020c8380.mUnk_38 - WH_SYSSTATE_CONNECTED) <= 2) {
        return TRUE;
    }
    r = func_02046a4c(func_020388d4);
    if (r != 2) {
        func_02038724(r);
        return FALSE;
    }
    data_020c8380.mUnk_04 = 0;
    data_020c8380.mUnk_02 = 1;
    return TRUE;
}

// Callback of hosting: follows children connecting and disconnecting.
void func_020388d4(Unk_WirelessMsg *msg) {
    u16 bit = 1 << msg->mUnk_10;

    if (msg->mUnk_02 != 0) {
        func_02038724(msg->mUnk_02);
        func_020386c0(WH_SYSSTATE_ERROR);
        return;
    }
    switch (msg->mUnk_08) {
        case WM_STATECODE_CONNECTED: {
            int r;

            if (data_020c8380.mUnk_30 != 0) {
                data_020c8380.mUnk_30("StartParent - new child (aid 0x%x) connected\n", msg->mUnk_10);
            }
            if (data_020c8380.mUnk_40 != 0 && data_020c8380.mUnk_40(msg) == 0) {
                r = func_02046b68(0, msg->mUnk_10);
                if (r != 2) {
                    func_02038724(r);
                    func_020386c0(WH_SYSSTATE_ERROR);
                }
                return;
            }
            data_020c8380.mUnk_02 |= bit;
            return;
        }
        case WM_STATECODE_DISCONNECTED:
            if (data_020c8380.mUnk_30 != 0) {
                data_020c8380.mUnk_30("StartParent - child (aid 0x%x) disconnected\n", msg->mUnk_10);
            }
            if (data_020c18bc.mUnk_230 != 0) {
                data_020c18bc.mUnk_230(msg->mUnk_10);
            }
            data_020c8380.mUnk_02 &= ~bit;
            return;
        case WM_STATECODE_DISCONNECTED_FROM_MYSELF:
            if (data_020c8380.mUnk_30 != 0) {
                data_020c8380.mUnk_30("StartParent - child (aid 0x%x) disconnected from myself\n", msg->mUnk_10);
            }
            return;
        case WM_STATECODE_PARENT_START:
            if (func_02038a78() == 0) {
                func_020386c0(WH_SYSSTATE_ERROR);
            }
            return;
        case WM_STATECODE_BEACON_SENT:
            return;
        default:
            if (data_020c8380.mUnk_30 != 0) {
                data_020c8380.mUnk_30("unknown indicate, state = %d\n", msg->mUnk_08);
            }
            return;
    }
}

// Starts the data exchange as the host unless it is already running.
BOOL func_02038a78(void) {
    int r;

    if (!(data_020c8380.mUnk_38 != WH_SYSSTATE_CONNECTED && data_020c8380.mUnk_38 != WH_SYSSTATE_KEYSHARING &&
          data_020c8380.mUnk_38 != WH_SYSSTATE_DATASHARING)) {
        return TRUE;
    }
    func_020386c0(WH_SYSSTATE_CONNECTED);
    r = func_02046dc0(func_02038b0c, (u32) data_020c8940, (u16) data_020c8380.mUnk_3c, (u32) data_020c8720,
                      (u16) data_020c8380.mUnk_20, 1);
    if (r == 2) {
        return TRUE;
    }
    func_02038724(r);
    return FALSE;
}

// Callback of the host's data exchange.
void func_02038b0c(Unk_WirelessMsg *msg) {
    u16 err = msg->mUnk_02;

    if (err != 0) {
        func_02038724(err);
        func_020386c0(WH_SYSSTATE_ERROR);
        return;
    }
    switch (msg->mUnk_04) {
        case WM_STATECODE_MP_START:
            if (data_020c8380.mUnk_34 == 2) {
                if (data_020c8380.mUnk_38 == WH_SYSSTATE_CONNECTED) {
                    if (func_02038c48() == 0) {
                        if (data_020c8380.mUnk_30 != 0) {
                            data_020c8380.mUnk_30("WH_StateInStartParentKeyShare failed\n");
                        }
                        func_020386c0(WH_SYSSTATE_ERROR);
                    }
                    return;
                }
                if (data_020c8380.mUnk_38 == WH_SYSSTATE_KEYSHARING) {
                    return;
                }
            } else if (data_020c8380.mUnk_34 == 4) {
                // Shares data between aid 0 (the parent) and aid 1.
                u16 mask = 3;
                int r = func_02046fc8(&data_020c95e0, 13, mask, 84, 1);

                if (r != 0) {
                    func_02038724(r);
                    func_020386c0(WH_SYSSTATE_ERROR);
                    return;
                }
                func_020386c0(WH_SYSSTATE_DATASHARING);
                return;
            }
            func_020386c0(WH_SYSSTATE_CONNECTED);
            return;
        case WM_STATECODE_MPEND_IND:
            data_020c9e04 |= 0x800;
            return;
        case WM_STATECODE_MP_IND:
        case WM_STATECODE_MPACK_IND:
        default:
            if (data_020c8380.mUnk_30 != 0) {
                data_020c8380.mUnk_30("unknown indicate, state = %d\n", msg->mUnk_04);
            }
            return;
    }
}

// Starts sharing keys on the host.
BOOL func_02038c48(void) {
    int r;

    func_020386c0(WH_SYSSTATE_KEYSHARING);
    r = func_02047b48(&data_020c8dc0, 13);
    if (r == 0) {
        return TRUE;
    }
    func_02038724(r);
    return FALSE;
}

// Ends the key sharing of the host, then the hosting.
BOOL func_02038c7c(void) {
    int r = func_02047b64(&data_020c8dc0);

    if (r != 0) {
        func_02038724(r);
        return FALSE;
    }
    if (func_02038ce0() == 0) {
        if (data_020c8380.mUnk_30 != 0) {
            data_020c8380.mUnk_30("WH_StateInEndParentMP failed\n");
        }
        func_0203970c();
        return FALSE;
    }
    return TRUE;
}

// Ends the hosting.
BOOL func_02038ce0(void) {
    int r;

    func_020386c0(WH_SYSSTATE_BUSY);
    r = func_02046f58(func_02038d10);
    if (r == 2) {
        return TRUE;
    }
    func_02038724(r);
    return FALSE;
}

// Callback of func_02038ce0.
void func_02038d10(Unk_WirelessMsg *msg) {
    if (msg->mUnk_02 != 0) {
        func_02038724(msg->mUnk_02);
        func_0203970c();
        return;
    }
    if (func_02038d60() == 0) {
        if (data_020c8380.mUnk_30 != 0) {
            data_020c8380.mUnk_30("WH_StateInEndParent failed\n");
        }
        func_0203970c();
    }
}

// Resets the link.
BOOL func_02038d60(void) {
    int r = func_02046a5c(func_02038d88);

    if (r == 2) {
        return TRUE;
    }
    func_02038724(r);
    return FALSE;
}

// Callback of func_02038d60: the link is idle again.
void func_02038d88(Unk_WirelessMsg *msg) {
    if (msg->mUnk_02 != 0) {
        func_02038724(msg->mUnk_02);
        return;
    }
    func_020386c0(WH_SYSSTATE_IDLE);
}

// Layout placeholder: prints the two buffer sizes; nothing calls it and the link drops it. The compiler creates
// the two strings where they are first used, and the game has them at this point of the file's string order, well
// before their users func_0203947c and func_02039598.
void func_unused_sizes(void) {
    data_020c8380.mUnk_30("recv buffer size = %d\n", data_020c8380.mUnk_3c);
    data_020c8380.mUnk_30("send buffer size = %d\n", data_020c8380.mUnk_20);
}

// Generates the child's key and sets it.
BOOL func_02038dac(void) {
    int r;

    func_020386c0(WH_SYSSTATE_BUSY);
    r = func_02047b70(func_02038e0c, data_020c8380.mUnk_28(data_020c83e0, data_020c8460), data_020c83e0);
    if (r == 2) {
        return TRUE;
    }
    func_02038724(r);
    func_020386c0(WH_SYSSTATE_ERROR);
    return FALSE;
}

// Callback of the key set by func_02038dac.
void func_02038e0c(Unk_WirelessMsg *msg) {
    if (msg->mUnk_02 != 0) {
        func_02038724(msg->mUnk_02);
        func_020386c0(WH_SYSSTATE_ERROR);
        return;
    }
    if (func_02038e64() == 0) {
        if (data_020c8380.mUnk_30 != 0) {
            data_020c8380.mUnk_30("WH_StateOutSetChildWEPKey : startchild failed\n");
        }
        func_020386c0(WH_SYSSTATE_ERROR);
    }
}

// Connects to the parent whose record is in data_020c8460.
BOOL func_02038e64(void) {
    int r;

    if (!(data_020c8380.mUnk_38 != WH_SYSSTATE_CONNECTED && data_020c8380.mUnk_38 != WH_SYSSTATE_KEYSHARING &&
          data_020c8380.mUnk_38 != WH_SYSSTATE_DATASHARING)) {
        if (data_020c8380.mUnk_30 != 0) {
            data_020c8380.mUnk_30("WH_StateInStartChild : already connected?\n");
        }
        return TRUE;
    }
    func_020386c0(WH_SYSSTATE_BUSY);
    r = func_02046a9c(func_02038f04, data_020c8460, 0, 1, data_020c8380.mUnk_28 != 0 ? 1 : 0);
    if (r == 2) {
        return TRUE;
    }
    func_02038724(r);
    return FALSE;
}

// Callback of the connection to the parent.
void func_02038f04(Unk_WirelessMsg *msg) {
    if (msg->mUnk_02 != 0) {
        func_02038724(msg->mUnk_02);
        if (msg->mUnk_02 == 12) {
            data_020c18bc.mUnk_46c |= 0x100;
            func_020386c0(WH_SYSSTATE_ERROR);
            return;
        }
        if (msg->mUnk_02 == 11) {
            data_020c18bc.mUnk_46c |= 0x100;
            func_020386c0(WH_SYSSTATE_ERROR);
            return;
        }
        if (msg->mUnk_02 == 1) {
            data_020c18bc.mUnk_46c |= 0x100;
            func_020386c0(WH_SYSSTATE_CONNECT_FAIL);
            return;
        }
        data_020c18bc.mUnk_46c |= 0x100;
        func_020386c0(WH_SYSSTATE_ERROR);
        return;
    }
    if (msg->mUnk_08 == WM_STATECODE_BEACON_LOST) {
        return;
    }
    if (msg->mUnk_08 == WM_STATECODE_CONNECTED) {
        if (data_020c8380.mUnk_30 != 0) {
            data_020c8380.mUnk_30("Connect to Parent\n");
        }
        func_020386c0(WH_SYSSTATE_CONNECTED);
        if (func_020390d4() == 0) {
            if (data_020c8380.mUnk_30 != 0) {
                data_020c8380.mUnk_30("WH_StateInStartChildMP failed\n");
            }
            func_020386c0(WH_SYSSTATE_BUSY);
            return;
        }
        data_020c8380.mUnk_04 = msg->mUnk_0a;
        return;
    }
    if (msg->mUnk_08 == WM_STATECODE_CONNECT_START) {
        return;
    }
    if (msg->mUnk_08 == WM_STATECODE_DISCONNECTED) {
        if (data_020c8380.mUnk_30 != 0) {
            data_020c8380.mUnk_30("Disconnected from Parent\n");
        }
        func_02038724(20);
        func_020386c0(WH_SYSSTATE_ERROR);
        data_020c18bc.mUnk_46c |= 0x100;
        return;
    }
    if (msg->mUnk_08 == WM_STATECODE_DISCONNECTED_FROM_MYSELF) {
        data_020c18bc.mUnk_46c |= 0x100;
        return;
    }
    if (data_020c8380.mUnk_30 != 0) {
        data_020c8380.mUnk_30("unknown state %d, %s\n", msg->mUnk_08, func_020386a4(msg->mUnk_08));
    }
    func_020386c0(WH_SYSSTATE_ERROR);
}

// Starts the data exchange as the child.
BOOL func_020390d4(void) {
    int r = func_02046dc0(func_02039140, (u32) data_020c8940, (u16) data_020c8380.mUnk_3c, (u32) data_020c8720,
                          (u16) data_020c8380.mUnk_20, 1);

    if (r == 2) {
        return TRUE;
    }
    func_02038724(r);
    return FALSE;
}

// Callback of the child's data exchange.
void func_02039140(Unk_WirelessMsg *msg) {
    if (msg->mUnk_02 != 0) {
        if (msg->mUnk_02 == 15 || msg->mUnk_02 == 9 || msg->mUnk_02 == 13) {
            return;
        }
        func_02038724(msg->mUnk_02);
        func_0203970c();
        return;
    }
    switch (msg->mUnk_04) {
        case WM_STATECODE_MP_START:
            if (data_020c8380.mUnk_34 == 3) {
                if (data_020c8380.mUnk_38 == WH_SYSSTATE_KEYSHARING) {
                    return;
                }
                if (data_020c8380.mUnk_38 == WH_SYSSTATE_CONNECTED) {
                    if (func_02039298() == 0) {
                        if (data_020c8380.mUnk_30 != 0) {
                            data_020c8380.mUnk_30("WH_StateInStartChildKeyShare failed\n");
                        }
                        func_02039728();
                    }
                    return;
                }
            } else if (data_020c8380.mUnk_34 == 5) {
                // Shares data between aid 0 (the parent) and aid 1.
                u16 mask = 3;
                int r = func_02046fc8(&data_020c95e0, 13, mask, 84, 1);

                if (r != 0) {
                    func_02038724(r);
                    func_02039728();
                    return;
                }
                if (data_020c8380.mUnk_30 != 0) {
                    data_020c8380.mUnk_30("WH_StateOutStartChildMP : WM_StartDataSharing OK\n");
                }
                func_020386c0(WH_SYSSTATE_DATASHARING);
                return;
            }
            func_020386c0(WH_SYSSTATE_CONNECTED);
            return;
        case WM_STATECODE_MP_IND:
            data_020c9e04 |= 0x800;
            return;
        case WM_STATECODE_MPACK_IND:
            break;
        case WM_STATECODE_MPEND_IND:
        default:
            if (data_020c8380.mUnk_30 != 0) {
                data_020c8380.mUnk_30("unknown indicate, state = %d\n", msg->mUnk_04);
            }
            return;
    }
}

// Starts sharing keys as the child.
BOOL func_02039298(void) {
    int r;

    if (data_020c8380.mUnk_38 == WH_SYSSTATE_KEYSHARING) {
        return TRUE;
    }
    if (data_020c8380.mUnk_38 != WH_SYSSTATE_CONNECTED) {
        return FALSE;
    }
    func_020386c0(WH_SYSSTATE_KEYSHARING);
    r = func_02047b48(&data_020c8dc0, 13);
    if (r == 0) {
        return TRUE;
    }
    func_02038724(r);
    return FALSE;
}

// Ends the key sharing of the child, then the connection.
BOOL func_020392f0(void) {
    int r;
    BOOL ok;

    if (data_020c8380.mUnk_38 != WH_SYSSTATE_KEYSHARING) {
        return FALSE;
    }
    func_020386c0(WH_SYSSTATE_BUSY);
    r = func_02047b64(&data_020c8dc0);
    if (r != 0) {
        func_02038724(r);
        ok = FALSE;
    } else if (func_02039348() == 0) {
        ok = FALSE;
    } else {
        ok = TRUE;
    }
    return ok;
}

// Ends the connection to the host.
BOOL func_02039348(void) {
    int r;

    func_020386c0(WH_SYSSTATE_BUSY);
    r = func_02046f58(func_02039378);
    if (r == 2) {
        return TRUE;
    }
    func_02038724(r);
    return FALSE;
}

// Callback of func_02039348.
void func_02039378(Unk_WirelessMsg *msg) {
    if (msg->mUnk_02 != 0) {
        func_02038724(msg->mUnk_02);
        func_02039728();
        return;
    }
    if (func_020393ac() == 0) {
        func_020386c0(WH_SYSSTATE_ERROR);
    }
}

// Resets the link.
BOOL func_020393ac(void) {
    int r;

    func_020386c0(WH_SYSSTATE_BUSY);
    r = func_02046b68(func_020393e4, 0);
    if (r == 2) {
        return TRUE;
    }
    func_02038724(r);
    func_0203970c();
    return FALSE;
}

// Callback of func_020393ac: the link is idle again.
void func_020393e4(Unk_WirelessMsg *msg) {
    if (msg->mUnk_02 != 0) {
        func_02038724(msg->mUnk_02);
        return;
    }
    func_020386c0(WH_SYSSTATE_IDLE);
}

// Shuts the link down.
BOOL func_02039408(void) {
    int r;

    func_020386c0(WH_SYSSTATE_BUSY);
    r = func_0204684c(func_02039438);
    if (r == 2) {
        return TRUE;
    }
    func_02038724(r);
    return FALSE;
}

// Callback of func_02039408.
void func_02039438(Unk_WirelessMsg *msg) {
    if (msg->mUnk_02 != 0) {
        func_020386c0(WH_SYSSTATE_ERROR);
        func_02038724(msg->mUnk_02);
        return;
    }
    func_020386c0(WH_SYSSTATE_IDLE);
}

// The state of the link.
int func_0203946c(void) {
    return data_020c8380.mUnk_38;
}

// Sets up the host with the parameters (the game id, the channel) and starts the link in the given mode (even
// modes host, odd modes join); returns whether it started.
int func_0203947c(int mode, u16 b, u16 c) {
    (void) (data_020c8380.mUnk_38 == WH_SYSSTATE_IDLE || (OS_Terminate(), 0));
    data_020c8380.mUnk_3c = 0x240;
    data_020c8380.mUnk_20 = 0x220;
    if (data_020c8380.mUnk_30 != 0) {
        data_020c8380.mUnk_30("recv buffer size = %d\n", data_020c8380.mUnk_3c);
    }
    if (data_020c8380.mUnk_30 != 0) {
        data_020c8380.mUnk_30("send buffer size = %d\n", data_020c8380.mUnk_20);
    }
    data_020c8380.mUnk_34 = mode;
    func_020386c0(WH_SYSSTATE_BUSY);
    data_020c8420.mUnk_0c = b;
    data_020c8420.mUnk_32 = c;
    data_020c8420.mUnk_18 = func_020467bc();
    data_020c8420.mUnk_34 = 0x200;
    data_020c8420.mUnk_36 = 0xfe;
    data_020c8420.mUnk_10 = 1;
    data_020c8420.mUnk_16 = 0;
    data_020c8420.mUnk_12 = 0;
    data_020c8420.mUnk_0e = 1;
    data_020c8420.mUnk_14 = mode == 2;
    if (mode == 0 || mode == 2 || mode == 4) {
        return func_02038740();
    }
    if (data_020c8380.mUnk_30 != 0) {
        data_020c8380.mUnk_30("unknown connect mode %d\n", mode);
    }
    return 0;
}

// Starts the link as a child in the given mode, to the parent whose record is data.
int func_02039598(int mode, const void *data) {
    (void) (data_020c8380.mUnk_38 == WH_SYSSTATE_IDLE || (OS_Terminate(), 0));
    data_020c8380.mUnk_3c = 0x480;
    data_020c8380.mUnk_20 = 0x100;
    if (data_020c8380.mUnk_30 != 0) {
        data_020c8380.mUnk_30("recv buffer size = %d\n", data_020c8380.mUnk_3c);
    }
    if (data_020c8380.mUnk_30 != 0) {
        data_020c8380.mUnk_30("send buffer size = %d\n", data_020c8380.mUnk_20);
    }
    data_020c8380.mUnk_34 = mode;
    func_020386c0(WH_SYSSTATE_BUSY);
    if (mode == 1 || mode == 3 || mode == 5) {
        MI_CpuCopy8(data, data_020c8460, 0xc0);
        DC_FlushRange(data_020c8460, 0xc0);
        DC_WaitWriteBufferEmpty();
        if (data_020c8380.mUnk_28 != 0) {
            return func_02038dac();
        }
        return func_02038e64();
    }
    if (data_020c8380.mUnk_30 != 0) {
        data_020c8380.mUnk_30("unknown connect mode %d\n", mode);
    }
    return 0;
}

// The data received from child n, or NULL.
u8 *func_02039698(u32 n) {
    return func_02047ac4(&data_020c95e0, data_020c8520, n);
}

// Runs one step of the data exchange, passing the data to send; returns whether it went through.
BOOL func_020396b8(void *buf) {
    int r = func_02047264(&data_020c95e0, buf, data_020c8520);

    if (r == 7) {
        return TRUE;
    }
    if (r == 5) {
        func_02038724(r);
        return FALSE;
    }
    if (r == 0) {
        return TRUE;
    }
    func_02038724(r);
    return FALSE;
}

// Shuts the link down; if that cannot be started the link enters the failed state.
void func_0203970c(void) {
    if (func_02039408() == 0) {
        func_020386c0(WH_SYSSTATE_FAILED);
    }
}

// Ends the current connection according to the mode, then shuts the link down on failure.
void func_02039728(void) {
    if (data_020c8380.mUnk_38 == WH_SYSSTATE_IDLE) {
        if (data_020c8380.mUnk_30 != 0) {
            data_020c8380.mUnk_30("already WH_SYSSTATE_IDLE\n");
        }
        return;
    }
    if (data_020c8380.mUnk_30 != 0) {
        data_020c8380.mUnk_30("WH_Finalize, state = %d\n", data_020c8380.mUnk_38);
    }
    if (data_020c8380.mUnk_38 != WH_SYSSTATE_KEYSHARING && data_020c8380.mUnk_38 != WH_SYSSTATE_DATASHARING &&
        data_020c8380.mUnk_38 != WH_SYSSTATE_CONNECTED) {
        func_020386c0(WH_SYSSTATE_BUSY);
        func_0203970c();
        return;
    }
    func_020386c0(WH_SYSSTATE_BUSY);
    switch (data_020c8380.mUnk_34) {
        case 3:
            if (func_020392f0() == 0) {
                func_0203970c();
            }
            return;
        case 1:
        case 5:
            if (func_02039348() == 0) {
                func_0203970c();
            }
            return;
        case 2:
            if (func_02038c7c() == 0) {
                func_0203970c();
            }
            return;
        case 0:
        case 4:
            if (func_02038ce0() == 0) {
                func_0203970c();
            }
            return;
        default:
            return;
    }
}

// The id the link assigned to this unit.
u16 func_0203981c(void) {
    return data_020c8380.mUnk_04;
}

