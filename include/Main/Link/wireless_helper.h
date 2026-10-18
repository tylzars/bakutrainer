#pragma once

#include "types.h"
#include <nitro/wm.h>

#ifdef __cplusplus
extern "C" {
#endif

// States of the link (Unk_WirelessWork.mUnk_38); the names are the ones the game prints.
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

// Message block that the wireless callbacks receive.
typedef struct Unk_WirelessMsg {
    u16 mUnk_00;
    u16 mUnk_02;
    u16 mUnk_04;
    u16 mUnk_06;
    u16 mUnk_08;
    u16 mUnk_0a;
    u8 mUnk_0c[4];
    u16 mUnk_10;
} Unk_WirelessMsg;

// State of the wireless link (0x60 bytes).
typedef struct Unk_WirelessWork {
    u16 mUnk_00;
    // Bit per child (by its id) that is connected.
    u16 mUnk_02;
    u16 mUnk_04;
    u8 mUnk_06[0x16];
    // Fills the key block (first argument) for the parent / the child (second argument is the parent parameter
    // block / the connection record) and returns the key mode.
    u32 (*mUnk_1c)(void *key, void *param);
    u32 mUnk_20;
    int mUnk_24;
    u32 (*mUnk_28)(void *key, void *param);
    u8 mUnk_2c[4];
    // Debug print, called when set.
    void (*mUnk_30)(const char *fmt, ...);
    int mUnk_34;
    int mUnk_38;
    u32 mUnk_3c;
    // Asked when a child connects: nonzero accepts it.
    int (*mUnk_40)(Unk_WirelessMsg *msg);
    u8 mUnk_44[0x1c];
} Unk_WirelessWork;

extern Unk_WirelessWork data_020c8380;
extern u8 data_020c83e0[0x40];
extern WMParentParam data_020c8420;
extern u16 data_020c8460[0x60];
extern u16 data_020c8520[0x100];
extern u8 data_020c8720[0x220];
extern u8 data_020c8940[0x480];
extern WMDataSharing data_020c8dc0;
extern WMDataSharing data_020c95e0;

const char *func_020386a4(u32 code);
void func_020386c0(int state);
void func_02038724(int error);
BOOL func_02038740(void);
void func_02038780(Unk_WirelessMsg *msg);
BOOL func_020387e4(void);
void func_02038844(Unk_WirelessMsg *msg);
BOOL func_0203887c(void);
void func_020388d4(Unk_WirelessMsg *msg);
BOOL func_02038a78(void);
void func_02038b0c(Unk_WirelessMsg *msg);
BOOL func_02038c48(void);
BOOL func_02038c7c(void);
BOOL func_02038ce0(void);
void func_02038d10(Unk_WirelessMsg *msg);
BOOL func_02038d60(void);
void func_02038d88(Unk_WirelessMsg *msg);
BOOL func_02038dac(void);
void func_02038e0c(Unk_WirelessMsg *msg);
BOOL func_02038e64(void);
void func_02038f04(Unk_WirelessMsg *msg);
BOOL func_020390d4(void);
void func_02039140(Unk_WirelessMsg *msg);
BOOL func_02039298(void);
BOOL func_020392f0(void);
BOOL func_02039348(void);
void func_02039378(Unk_WirelessMsg *msg);
BOOL func_020393ac(void);
void func_020393e4(Unk_WirelessMsg *msg);
BOOL func_02039408(void);
void func_02039438(Unk_WirelessMsg *msg);
int func_0203946c(void);
int func_0203947c(int mode, u16 b, u16 c);
int func_02039598(int mode, const void *data);
u8 *func_02039698(u32 n);
BOOL func_020396b8(void *buf);
void func_0203970c(void);
void func_02039728(void);
u16 func_0203981c(void);
void func_0203982c(u16 code);

// Work area of the link status code: +4 is the bit set of finished steps (see func_02039c88); +0x10 and +0x18 are the
// two buffers of the outgoing message; +0x8, +0xc, +0x10, +0x14, +0x18 and +0x1c are released when the link shuts down.
// The Thumb code of wireless_helper.c reaches the members through data_020c9e00 (pool word 0x020c9e00, offset in the
// load), while the ARM code here and system_init.c / comm_link.c load the exact addresses 0x020c9e04, 0x020c9e10,
// 0x020c9e14 and 0x020c9e1c, so they keep those as separate objects (system_init.h).
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

extern Unk_LinkBuffers data_020c9e00;

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
