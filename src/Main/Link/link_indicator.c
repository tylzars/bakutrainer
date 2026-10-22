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
#include <Main/Link/link_indicator.h>

// Of the code in this file only GetIndicatorLayer does any work in this build: the link conditions the rest needs
// (gSystemState.mFlags 0x200 or 0x400, mUnk_2a6_0, mUnk_2a6_1, mUnk_2a4[1] above 1) never hold, so UpdateIndicator
// returns at once and the other functions are never called.

void func_0203982c(u16 code) {}

// Link indicator and shutdown. The work area of the code below: the bit set of finished steps and the buffers of
// the link status exchange. Its zero initialiser makes it take part in the compiler's size ordering of this file's
// data where it is defined, which puts the local initialisers of the functions below in the game's order.
LinkBuffers gLinkBuffers __attribute__((aligned(32))) = {0};

// Plane bits of the display control registers (the four backgrounds, then the objects).
#define DISPCNT_BGS 0xf00
#define DISPCNT_OBJ 0x1000

// Steps whose end the link waits for in WaitForLinkStep (bits of LinkBuffers.mFlags). LINK_STEP_FIRST_SHUT_DOWN and
// LINK_STEP_SECOND_SHUT_DOWN are set by the callbacks of ShutDownLink's first and second step, which both run
// whatever the link's role; nothing sets LINK_STEP_STATUS.
#define LINK_STEP_STATUS 0x10
#define LINK_STEP_FIRST_SHUT_DOWN 0x20
#define LINK_STEP_SECOND_SHUT_DOWN 0x40
#define LINK_PACKET_OUT 0x200

// One entry of object attribute memory (attributes 0-2, then one of the four affine parameters of the entry's group).
typedef struct {
    vu16 mUnk_0;
    vu16 mUnk_2;
    vu16 mUnk_4;
    vu16 mUnk_6;
} OamEntry;

// Sets up one object of layer `layer`: calls DrawSprite for the picture `id`, then writes object attribute entry
// `index` (attr0, attribute 1 from `attr1`, and attribute 2 from a tile offset and the palette) and, when `rotate` is
// set, the rotation entry of the layer's group from the sine/cosine table at the frame counter's phase.
void WriteIndicatorOam(int id, u32 shift, u32 index, u32 attr1, u32 attr0, int rotate, int layer) {
    OamEntry *oam = layer == 1 ? (OamEntry *) 0x07000000 : (OamEntry *) 0x07000400;
    s16 tile;

    DrawSprite(id + 0x200000, 0x80, 0x60, 0, layer);
    tile = GetSpriteTile();
    if (tile < 0) {
        return;
    }
    tile += (shift * 4 >> GetUnitShift(layer));
    if (rotate == 0) {
        oam[index].mUnk_0 = attr0;
        oam[index].mUnk_2 = attr1 + 0x4000;
    } else {
        int angle = (gSystemState.mElapsedFrames & 0xff) << 8;
        u32 group = GetMatCount(layer);
        int k = (angle >> 4) * 2;

        *(vu16 *) ((u8 *) oam + 6 + group * 32) = FX_SinCosTable_[k + 1] >> 4;
        ((vu16 *) ((u8 *) oam + group * 32))[15] = *(vu16 *) ((u8 *) oam + 6 + group * 32);
        *(vu16 *) ((u8 *) oam + 14 + group * 32) = FX_SinCosTable_[k] >> 4;
        ((vu16 *) ((u8 *) oam + group * 32))[11] = -*(vu16 *) ((u8 *) oam + 14 + group * 32);
        oam[index].mUnk_0 = 0x300 | attr0;
        oam[index].mUnk_2 = attr1 + 0x8000 + group * 512;
    }
    oam[index].mUnk_4 = tile + (GetSpritePalette() << 12);
}

// Holds a countdown at 20 while IsCommReading reports 1 and counts it down otherwise. When the screens are flagged,
// saves the plane bits of each engine's display control (hiding all but the objects on the engine that is not the
// indicator's), or puts them back, then draws the two objects.
void UpdateCommIndicator(u32 attr1, u32 attr0, int screen) {
    int layer;
    int other;
    int i;
    u32 mask;

    if (IsCommReading() == 1) {
        gSystemState.mUnk_250.mUnk_28[0]++;
        if (gSystemState.mUnk_250.mUnk_28[0] != 0) {
            gSystemState.mUnk_250.mUnk_28[0] = 20;
        }
    } else if (gSystemState.mUnk_250.mUnk_28[0] != 0) {
        gSystemState.mUnk_250.mUnk_28[0]--;
    }
    if (!(gSystemState.mFlags & 3)) {
        return;
    }
    {
        u16 *reg;
        u16 *regs[2] = {(u16 *) 0x04000000, (u16 *) 0x04001000};

        if (screen == 1) {
            layer = 1;
            if (!(gSystemState.mFlags & 1)) {
                layer = 2;
            }
        } else {
            layer = 2;
            if (!(gSystemState.mFlags & 2)) {
                layer = 1;
            }
        }
        other = layer == 1 ? 0 : 1;
        for (i = 0; i < 2; i++) {
            if (gSystemState.mUnk_27c[2] == 1) {
                mask = DISPCNT_BGS;
                if (i == other) {
                    SetFadeClear(1 << i);
                } else {
                    mask |= DISPCNT_OBJ;
                }
                reg = regs[i];
                gSystemState.mUnk_27c[i] |= (*reg >> 8) & 0x1f;
                *reg &= ~mask;
            } else if (gSystemState.mUnk_27c[i] != 0) {
                *regs[i] |= gSystemState.mUnk_27c[i] << 8;
                gSystemState.mUnk_27c[i] = 0;
            }
        }
        if ((gSystemState.mFlags & 0xc0000) && gSystemState.mUnk_250.mUnk_28[0] != 0) {
            WriteIndicatorOam(gSystemState.mUnk_290 + 9, 0, 0x7e, 0x61, 0x41, 1, layer);
        }
        WriteIndicatorOam(gSystemState.mUnk_290 + 10, 3 - func_0204672c(), 0x7f, attr1, attr0, 0, layer);
    }
}

// The screen (layer 1 or 2) that carries the indicator, by bit 2 of mUnk_2a4[0] and the screen flags.
u32 GetIndicatorLayer(void) {
    if (gSystemState.mUnk_2a4[0] & 4) {
        return (gSystemState.mFlags & 2) ? 2 : 1;
    }
    return (gSystemState.mFlags & 1) ? 1 : 2;
}

// While the link is flagged, draws an object (through UpdateCommIndicator when the 0x200 flag is set) and calls the
// hook; otherwise copies a 16 by 8 halfword picture, chosen by the link level, into the background memory.
void UpdateIndicator(void) {
    u32 x;
    u32 y;
    u32 layer;
    u32 level;
    u16 *src;
    u16 *dst;
    int col;
    int row;

    if (!gSystemState.mUnk_2a6_0 && !gSystemState.mUnk_2a6_1) {
        return;
    }
    if (func_02046380() != 0) {
        return;
    }
    {
        u8 positions[8] = {0, 0, 0xf0, 0, 0xf0, 0xb0, 0, 0xb0};

        x = positions[(gSystemState.mUnk_2a4[0] & 3) * 2];
        y = positions[(gSystemState.mUnk_2a4[0] & 3) * 2 + 1];
    }
    layer = GetIndicatorLayer();
    level = 3 - func_0204672c();
    if (gSystemState.mFlags & 3) {
        if (gSystemState.mFlags & 0x200) {
            UpdateCommIndicator(x, y, layer);
        } else {
            DrawSprite(gSystemState.mUnk_290 + level, x, y, 0, layer);
        }
        if (gSystemState.mUnk_240 != 0) {
            gSystemState.mUnk_240();
        }
        Free(&gSystemState.mUnk_284);
    } else {
        src = gSystemState.mUnk_284;
        if (src == 0) {
            return;
        }
        dst = (u16 *) 0x06000000;
        src = (u16 *) ((u8 *) src + ((level << 8) & ~1));
        for (row = 0; row < 16; row++) {
            for (col = 0; col < 8; col++) {
                *dst++ = *src++;
            }
            dst = (u16 *) ((u8 *) dst + 0xf0);
        }
    }
}

// Restarts the countdown and, when the status call succeeds with state 3, waits for the status step (its bit is never
// set, so the wait runs to its 0x78-frame timeout). The spinner characters are unused: they only reproduce the 4-byte
// rodata object that sits before the other rodata of the file.
void func_02039b70(void) {
    char spin[4] = {'-', '\\', '|', '/'};
    u16 buf[1000];

    gSystemState.mUnk_2a3 = 60;
    if (func_020465b0(buf) == 0 && buf[0] == 3) {
        WaitForLinkStep(LINK_STEP_STATUS, 0x78);
    }
}

// Shuts the link down: prints, waits for the two end steps when the link is up, then clears the indicator state and
// releases its buffers. The link level is tested once without effect before the flags: the code compares it with
// 1 and drops the result (the compiler does not optimise a function that uses the 32-byte aligned gLinkBuffers, so
// the test stays).
void ShutDownLink(void) {
    gSystemState.mUnk_2a3 = 60;
    func_02021fa8("WIR Shut Down\n");
    (void) ((gSystemState.mUnk_2a4[1] >= 1) ? 0 : 0);
    if ((gSystemState.mFlags & 0x600) || gSystemState.mUnk_2a4[1] >= 1) {
        if (func_0204684c(FirstShutDownStepCallback) == 2) {
            WaitForLinkStep(LINK_STEP_FIRST_SHUT_DOWN, 0x2d0);
        }
        if (func_02046884(SecondShutDownStepCallback) == 2) {
            WaitForLinkStep(LINK_STEP_SECOND_SHUT_DOWN, 0x3c);
        }
    }
    gSystemState.mUnk_2a4[1] = -1;
    gSystemState.mUnk_2a6_0 = 0;
    gSystemState.mUnk_2a6_1 = 0;
    gSystemState.mUnk_250.mUnk_08 = 0;
    Free(&gLinkBuffers.mUnk_08);
    Free(&gLinkBuffers.mUnk_0c);
    Free(&gLinkBuffers.mRecvBuffer);
    Free(&gLinkBuffers.mRecvCopy);
    Free(&gLinkBuffers.mSendBuffer);
    Free(&gLinkBuffers.mOutMessage);
    gLinkBuffers.mFlags = 0;
    gSystemState.mFlags &= ~0x400;
}

// Waits until a step in `mask` has finished (a bit set by the callbacks below), calling the system frame function
// whenever the frame counter changes; gives up and returns FALSE after `timeout` frames. Clears the bits on success.
BOOL WaitForLinkStep(u32 mask, u32 timeout) {
    u32 start = gSystemState.mElapsedFrames;
    u32 last = gSystemState.mElapsedFrames;

    if (!(gLinkBuffers.mFlags & mask)) {
        do {
            if (gSystemState.mElapsedFrames != last) {
                UpdateSystem(3);
                last = gSystemState.mElapsedFrames;
            }
            if (gSystemState.mElapsedFrames - start > timeout) {
                return FALSE;
            }
        } while (!(gLinkBuffers.mFlags & mask));
    }
    gLinkBuffers.mFlags &= ~mask;
    return TRUE;
}

// Reserves `size` bytes of the outgoing message buffer and returns its start (byte 1 holds the size), or 0 when the
// link is not up or the buffer (0x54 bytes) is full.
u8 *ReserveLinkMessage(u32 size) {
    u32 pos;
    u8 *p;

    if (gSystemState.mUnk_2a4[1] > 1) {
        pos = gSystemState.mUnk_296;
        if (pos + size <= 0x54) {
            if (gSystemState.mFlags & 0x600) {
                p = gCommSendData;
            } else {
                p = gLinkBuffers.mOutMessage;
            }
            p += pos;
            p[1] = size;
            gSystemState.mUnk_296 += size;
            return p;
        }
    }
    return 0;
}

// Ends the outgoing message with a terminator record and, unless the 0x600 flags are set, copies the buffer on.
void EndLinkMessage(void) {
    u8 *p;

    if (gSystemState.mUnk_2a4[1] > 1) {
        p = ReserveLinkMessage(2);
        if (p != 0) {
            p[0] = 0xff;
            p[1] = 0;
        }
        gSystemState.mUnk_296 = 0;
        if (!(gSystemState.mFlags & 0x600)) {
            MIi_CpuCopyFast(gLinkBuffers.mOutMessage, gLinkBuffers.mSendBuffer, 0x200);
        }
    }
}

// Callback of a packet send (func_02046e28): on success clears the packet-out bit, otherwise passes the code on.
void PacketSentCallback(WirelessMsg *msg) {
    if (msg->mError != 0) {
        func_0203982c(msg->mUnk_00);
    } else {
        gLinkBuffers.mFlags &= ~LINK_PACKET_OUT;
    }
}

void FirstShutDownStepCallback(WirelessMsg *msg) {
    if (msg->mError != 0) {
        func_0203982c(msg->mUnk_00);
    } else {
        gLinkBuffers.mFlags |= LINK_STEP_FIRST_SHUT_DOWN;
    }
}

void SecondShutDownStepCallback(WirelessMsg *msg) {
    if (msg->mError != 0) {
        func_0203982c(msg->mUnk_00);
    } else {
        gLinkBuffers.mFlags |= LINK_STEP_SECOND_SHUT_DOWN;
    }
}
