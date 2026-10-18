#include <nitro/os.h>
#include <nitro/mi.h>
#include <nitro/wm.h>
#include <nitro/fx.h>
#include <Main/clib.h>
#include <Main/Unknown/texture_blocks.h>
#include <Main/Unknown/screen_fade.h>
#include <Main/Unknown/comm_link.h>
#include <Main/Unknown/text_draw.h>
#include <Main/Unknown/system_init.h>
#include <Main/Unknown/wireless_helper.h>

// Names of the api ids.
const char *const data_02063cc8[5] = {
    "WM_APIID_UNKNOWN", "WM_APIID_INDICATION", "WM_APIID_PORT_SEND", "WM_APIID_PORT_RECV", "WM_APIID_READ_STATUS",
};

// Names of the link states, indexed by state.
const char *const data_02063cdc[10] = {
    "WH_SYSSTATE_STOP",         "WH_SYSSTATE_IDLE",        "WH_SYSSTATE_SCANNING",   "WH_SYSSTATE_BUSY",
    "WH_SYSSTATE_CONNECTED",    "WH_SYSSTATE_DATASHARING", "WH_SYSSTATE_KEYSHARING", "WH_SYSSTATE_MEASURECHANNEL",
    "WH_SYSSTATE_CONNECT_FAIL", "WH_SYSSTATE_ERROR",
};

// Names of the indication state codes, indexed by code.
const char *const data_02064108[27] = {
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
                data_020c8380.mUnk_30("StartParent - new child (aid 0x%x) connected\n");
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
                data_020c8380.mUnk_30("StartParent - child (aid 0x%x) disconnected\n");
            }
            if (data_020c18bc.mUnk_230 != 0) {
                data_020c18bc.mUnk_230(msg->mUnk_10);
            }
            data_020c8380.mUnk_02 &= ~bit;
            return;
        case WM_STATECODE_DISCONNECTED_FROM_MYSELF:
            if (data_020c8380.mUnk_30 != 0) {
                data_020c8380.mUnk_30("StartParent - child (aid 0x%x) disconnected from myself\n");
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
                int r = func_02046fc8(&data_020c95e0, 13, 3, 84, 1);

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
                // The state code (mUnk_04) is read again here through a halfword view of the message, as the original
                // reloads it instead of reusing the switch value.
                data_020c8380.mUnk_30("unknown indicate, state = %d\n", ((u16 *) msg)[2]);
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
                int r = func_02046fc8(&data_020c95e0, 13, 3, 84, 1);

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
                // The state code (mUnk_04) is read again here through a halfword view of the message, as the original
                // reloads it instead of reusing the switch value.
                data_020c8380.mUnk_30("unknown indicate, state = %d\n", ((u16 *) msg)[2]);
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
        return FALSE;
    }
    ok = func_02039348();
    return ok != 0;
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
    if (data_020c8380.mUnk_38 != WH_SYSSTATE_IDLE) {
        OS_Terminate();
    }
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
    if (data_020c8380.mUnk_38 != WH_SYSSTATE_IDLE) {
        OS_Terminate();
    }
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

#pragma thumb on

void func_0203982c(u16 code) {}

// Link indicator and shutdown. The work area of the code below: the bit set of finished steps and the buffers of
// the link status exchange.
Unk_LinkBuffers data_020c9e00;

// Plane bits of the display control registers (the four backgrounds, then the objects).
#define DISPCNT_BGS 0xf00
#define DISPCNT_OBJ 0x1000

// Steps whose end the link waits for in func_02039c88 (bits of Unk_LinkBuffers.mUnk_04).
#define LINK_STEP_STATUS 0x10
#define LINK_STEP_PARENT 0x20
#define LINK_STEP_CHILD 0x40
#define LINK_PACKET_OUT 0x200

// Sets up one object of a screen: calls func_0202cf08 for the picture `id`, then writes object attribute entry `index`
// (attr0, and attribute 2 from `attr2`, a tile offset and the palette) and, when `rotate` is set, the rotation entry of
// the screen's group from the sine/cosine table at the frame counter's phase.
void func_02039830(int id, u32 shift, u32 index, u32 attr2, u32 attr0, int rotate, int screen) {
    u16 *oam = screen == 1 ? (u16 *) 0x07000000 : (u16 *) 0x07000400;
    s16 tile;
    u32 off;

    func_0202cf08(id + 0x200000, 0x80, 0x60, 0, screen);
    tile = func_0202ad9c();
    if (tile < 0) {
        return;
    }
    tile = tile + (shift * 4 >> func_0202ad78(screen));
    if (rotate == 0) {
        off = index * 8;
        *(u16 *) ((u8 *) oam + off) = attr0;
        ((u16 *) ((u8 *) oam + off))[1] = attr2 + 0x4000;
    } else {
        int angle = (data_020c18bc.mUnk_45c & 0xff) << 8;
        u32 group = func_0202cdac(screen);
        int k = (angle >> 4) * 2;
        u16 *rot = (u16 *) ((u8 *) oam + group * 32);

        *(u16 *) ((u8 *) oam + 6 + group * 32) = FX_SinCosTable_[k + 1] >> 4;
        rot[15] = *(u16 *) ((u8 *) oam + 6 + group * 32);
        *(u16 *) ((u8 *) oam + 14 + group * 32) = FX_SinCosTable_[k] >> 4;
        rot[11] = -*(u16 *) ((u8 *) oam + 14 + group * 32);
        off = index * 8;
        *(u16 *) ((u8 *) oam + off) = 0x300 | attr0;
        ((u16 *) ((u8 *) oam + off))[1] = attr2 + 0x8000 + group * 512;
    }
    ((u16 *) ((u8 *) oam + off))[2] = tile + (func_0202adac() << 12);
}

// Holds a countdown at 20 while func_02035058 reports 1 and counts it down otherwise. When the screens are flagged,
// saves the plane bits of each engine's display control (hiding all but the objects on the engine that is not the
// indicator's), or puts them back, then draws the two objects.
void func_02039908(u32 attr2, u32 attr0, int screen) {
    int layer;
    int other;
    int i;
    u32 mask;

    if (func_02035058() == 1) {
        data_020c18bc.mUnk_250.mUnk_28[0]++;
        if (data_020c18bc.mUnk_250.mUnk_28[0] != 0) {
            data_020c18bc.mUnk_250.mUnk_28[0] = 20;
        }
    } else if (data_020c18bc.mUnk_250.mUnk_28[0] != 0) {
        data_020c18bc.mUnk_250.mUnk_28[0]--;
    }
    if (!(data_020c18bc.mUnk_46c & 3)) {
        return;
    }
    {
        u16 *reg;
        u16 *regs[2] = {(u16 *) 0x04000000, (u16 *) 0x04001000};

        if (screen == 1) {
            layer = 1;
            if (!(data_020c18bc.mUnk_46c & 1)) {
                layer = 2;
            }
        } else {
            layer = 2;
            if (!(data_020c18bc.mUnk_46c & 2)) {
                layer = 1;
            }
        }
        other = layer == 1 ? 0 : 1;
        for (i = 0; i < 2; i++) {
            if (data_020c18bc.mUnk_27c[2] == 1) {
                mask = DISPCNT_BGS;
                if (i == other) {
                    func_0201f8d0(1 << i);
                } else {
                    mask |= DISPCNT_OBJ;
                }
                reg = regs[i];
                data_020c18bc.mUnk_27c[i] |= (*reg >> 8) & 0x1f;
                *reg &= ~mask;
            } else if (data_020c18bc.mUnk_27c[i] != 0) {
                *regs[i] |= data_020c18bc.mUnk_27c[i] << 8;
                data_020c18bc.mUnk_27c[i] = 0;
            }
        }
        if ((data_020c18bc.mUnk_46c & 0xc0000) && data_020c18bc.mUnk_250.mUnk_28[0] != 0) {
            func_02039830(data_020c18bc.mUnk_290 + 9, 0, 0x7e, 0x61, 0x41, 1, layer);
        }
        func_02039830(data_020c18bc.mUnk_290 + 10, 3 - func_0204672c(), 0x7f, attr2, attr0, 0, layer);
    }
}

// The screen (1 or 2) that carries the indicator, by bit 2 of mUnk_2a4[0] and the screen flags.
u32 func_02039a64(void) {
    if (data_020c18bc.mUnk_2a4[0] & 4) {
        return (data_020c18bc.mUnk_46c & 2) ? 2 : 1;
    }
    return (data_020c18bc.mUnk_46c & 1) ? 1 : 2;
}

// While the link is flagged, draws an object (through func_02039908 when the 0x200 flag is set) and calls the hook;
// otherwise copies a 16 by 8 halfword picture, chosen by the link level, into the background memory.
void func_02039a94(void) {
    u32 x;
    u32 y;
    u32 screen;
    u32 level;
    u16 *base;
    u16 *src;
    u16 *dst;
    int row;
    int col;

    if (!data_020c18bc.mUnk_2a6_0 && !data_020c18bc.mUnk_2a6_1) {
        return;
    }
    if (func_02046380() != 0) {
        return;
    }
    {
        u8 colors[8] = {0, 0, 0xf0, 0, 0xf0, 0xb0, 0, 0xb0};

        x = colors[(data_020c18bc.mUnk_2a4[0] & 3) * 2];
        y = colors[(data_020c18bc.mUnk_2a4[0] & 3) * 2 + 1];
    }
    screen = func_02039a64();
    level = 3 - func_0204672c();
    if (data_020c18bc.mUnk_46c & 3) {
        if (data_020c18bc.mUnk_46c & 0x200) {
            func_02039908(x, y, screen);
        } else {
            func_0202cf08(data_020c18bc.mUnk_290 + level, x, y, 0, screen);
        }
        if (data_020c18bc.mUnk_240 != 0) {
            data_020c18bc.mUnk_240();
        }
        func_0201c4f0(&data_020c18bc.mUnk_284);
    } else {
        base = data_020c18bc.mUnk_284;
        if (base == 0) {
            return;
        }
        src = (u16 *) ((u8 *) base + ((level << 8) & ~1));
        dst = (u16 *) 0x06000000;
        for (row = 0; row < 16; row++) {
            for (col = 0; col < 8; col++) {
                *dst++ = *src++;
            }
            dst = (u16 *) ((u8 *) dst + 0xf0);
        }
    }
}

// Restarts the countdown and, when the status call succeeds with state 3, waits for the status step.
void func_02039b70(void) {
    u16 buf[1000];

    data_020c18bc.mUnk_2a3 = 60;
    if (func_020465b0(buf) == 0 && buf[0] == 3) {
        func_02039c88(LINK_STEP_STATUS, 0x78);
    }
}

// Shuts the link down: prints, waits for the two end steps when the link is up, then clears the indicator state and
// releases its buffers. The array is unused: it only reproduces the 4-byte rodata object that sits before the other
// rodata of the file.
void func_02039ba8(void) {
    char spin[4] = {'-', '\\', '|', '/'};

    data_020c18bc.mUnk_2a3 = 60;
    func_02021fa8("WIR Shut Down\n");
    if ((data_020c18bc.mUnk_46c & 0x600) || data_020c18bc.mUnk_2a4[1] >= 1) {
        if (func_0204684c(func_02039d98) == 2) {
            func_02039c88(LINK_STEP_PARENT, 0x2d0);
        }
        if (func_02046884(func_02039db8) == 2) {
            func_02039c88(LINK_STEP_CHILD, 0x3c);
        }
    }
    data_020c18bc.mUnk_2a4[1] = -1;
    data_020c18bc.mUnk_2a6_0 = 0;
    data_020c18bc.mUnk_2a6_1 = 0;
    data_020c18bc.mUnk_250.mUnk_08 = 0;
    func_0201c4f0(&data_020c9e00.mUnk_08);
    func_0201c4f0(&data_020c9e00.mUnk_0c);
    func_0201c4f0(&data_020c9e00.mUnk_14);
    func_0201c4f0(&data_020c9e00.mUnk_1c);
    func_0201c4f0(&data_020c9e00.mUnk_10);
    func_0201c4f0(&data_020c9e00.mUnk_18);
    data_020c9e00.mUnk_04 = 0;
    data_020c18bc.mUnk_46c &= ~0x400;
}

// Waits until a step in `mask` has finished (a bit set by the callbacks below), calling the system frame function
// whenever the frame counter changes; gives up and returns FALSE after `timeout` frames. Clears the bits on success.
BOOL func_02039c88(u32 mask, u32 timeout) {
    u32 start = data_020c18bc.mUnk_45c;
    u32 last = data_020c18bc.mUnk_45c;

    while (!(data_020c9e00.mUnk_04 & mask)) {
        if (data_020c18bc.mUnk_45c != last) {
            func_0201bb40(3);
            last = data_020c18bc.mUnk_45c;
        }
        if (data_020c18bc.mUnk_45c - start > timeout) {
            return FALSE;
        }
    }
    data_020c9e00.mUnk_04 &= ~mask;
    return TRUE;
}

// Reserves `size` bytes of the outgoing message buffer and returns its start (byte 1 holds the size), or 0 when the
// link is not up or the buffer (0x54 bytes) is full.
u8 *func_02039cd4(u32 size) {
    u32 pos;
    u8 *p;

    if (data_020c18bc.mUnk_2a4[1] > 1) {
        pos = data_020c18bc.mUnk_296;
        if (pos + size <= 0x54) {
            if (data_020c18bc.mUnk_46c & 0x600) {
                p = data_020c67e0;
            } else {
                p = data_020c9e00.mUnk_18;
            }
            p += pos;
            p[1] = size;
            data_020c18bc.mUnk_296 += size;
            return p;
        }
    }
    return 0;
}

// Ends the outgoing message with a terminator record and, unless the 0x600 flags are set, copies the buffer on.
void func_02039d24(void) {
    u8 *p;

    if (data_020c18bc.mUnk_2a4[1] > 1) {
        p = func_02039cd4(2);
        if (p != 0) {
            p[0] = 0xff;
            p[1] = 0;
        }
        data_020c18bc.mUnk_296 = 0;
        if (!(data_020c18bc.mUnk_46c & 0x600)) {
            MIi_CpuCopyFast(data_020c9e00.mUnk_18, data_020c9e00.mUnk_10, 0x200);
        }
    }
}

// Callback of a packet send (func_02046e28): on success clears the packet-out bit, otherwise passes the code on.
void func_02039d74(Unk_WirelessMsg *msg) {
    if (msg->mUnk_02 != 0) {
        func_0203982c(msg->mUnk_00);
    } else {
        data_020c9e00.mUnk_04 &= ~LINK_PACKET_OUT;
    }
}

// Callback of the first shutdown step.
void func_02039d98(Unk_WirelessMsg *msg) {
    if (msg->mUnk_02 != 0) {
        func_0203982c(msg->mUnk_00);
    } else {
        data_020c9e00.mUnk_04 |= LINK_STEP_PARENT;
    }
}

// Callback of the second shutdown step.
void func_02039db8(Unk_WirelessMsg *msg) {
    if (msg->mUnk_02 != 0) {
        func_0203982c(msg->mUnk_00);
    } else {
        data_020c9e00.mUnk_04 |= LINK_STEP_CHILD;
    }
}
