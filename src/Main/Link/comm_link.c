#include <nitro/os.h>
#include <nitro/fs.h>
#include <nitro/mi.h>
#include <nitro/wm.h>
#include <Main/clib.h>
#include <Main/Link/comm_link.h>
#include <Main/Link/arm_system.h>
#include <Main/System/bg_image.h>
#include <Main/System/screen_fade.h>
#include <Main/Save/save_blocks.h>
#include <Main/Graphics/texture_blocks.h>
#include <Main/Sound/tracker_player.h>
#include <Main/Sound/wave_player.h>
#include <Main/System/engine_asm.h>
#include <Main/System/system_init.h>
#include <Main/System/file_loader.h>
#include <Main/Link/wireless_helper.h>
#include <Main/Link/link_indicator.h>
#include <Main/Graphics/gx_pool.h>

Unk_020c6adc data_020c6adc;

// The file link over the wireless helper. It does not run in this build: its per-frame steps (UpdateCommChild here,
// UpdateCommParent in arm_system.c) are called only while gSystemState.mFlags has 0x200 or 0x400 set, and nothing
// sets either flag; the error state that HaltOnCommError waits for is entered only under 0x200 as well.

// The link keeps COMM_SLOTS received records of COMM_RECORD_SIZE bytes, indexed by slot.
#define COMM_SLOTS 16
#define COMM_RECORD_SIZE 254

// First halfword of a record: the kind of message waiting in it.
#define COMM_REQUEST 0x50
#define COMM_RANGE 0x51
// First byte of a message: the reply tag, and the state byte of a queued transfer that is waiting to be sent.
#define COMM_REPLY 0x52
#define COMM_PENDING 0x53
// Bytes of file data in one packet.
#define COMM_CHUNK 0x1f7
// Bit 9 of data_020c9e04: a packet is out.
#define COMM_BUSY 0x200
// Bit 23 of gSystemState.mUnk_2c8. Where the code tests it, the result goes nowhere (the test survives but whatever
// it guarded does not): written below as a conditional expression whose value is unused.
#define COMM_TRACE 0x800000

// A received record: the kind of message, then the message (a request carries a file number at 4; a range
// message carries its start at 6 and its length at 2, both unaligned).
typedef struct Unk_CommRecord {
    u16 mUnk_0;
    u16 mUnk_2;
    u16 mUnk_4;
    u8 mUnk_6[COMM_RECORD_SIZE - 6];
} Unk_CommRecord;

#pragma thumb off

// With the link in WH_SYSSTATE_DATASHARING and this frame's exchange gone through, reads each of the 16 slots: a slot
// that holds data has its record stored in gCommRecords and its flag set (gCommRecordReceived, and a bit in the mask at
// gSystemState.mLinkSlotMask); then CountUnchangedSlotMask follows the mask.
void ExchangeCommRecords(void) {
    u16 i;

    if (WhGetSysState() != WH_SYSSTATE_DATASHARING) {
        return;
    }
    if (WhUpdateDataSharing(gCommSendData) == 0) {
        return;
    }
    *data_020c66d4->mUnk_4 = 0;
    gSystemState.mLinkSlotMask = 0;
    for (i = 0; i < COMM_SLOTS; i++) {
        void *src = WhGetSharedData(i);
        if (src != NULL) {
            MI_CpuCopy8(src, gCommRecords + i * COMM_RECORD_SIZE, COMM_RECORD_SIZE);
            gCommRecordReceived[i] = 1;
            gSystemState.mLinkSlotMask |= 1 << i;
        } else {
            gCommRecordReceived[i] = 0;
        }
    }
    CountUnchangedSlotMask();
}

// Counts (up to 0x800) the calls for which the slot mask has not changed.
void CountUnchangedSlotMask(void) {
    if (gSystemState.mLinkSlotMask != gSystemState.mPrevLinkSlotMask) {
        gSystemState.mPrevLinkSlotMask = gSystemState.mLinkSlotMask;
        gSystemState.mUnchangedLinkSlotMaskCount = 0;
    } else if (gSystemState.mUnchangedLinkSlotMaskCount < 0x800) {
        (gSystemState.mUnchangedLinkSlotMaskCount)++;
    }
}

void ClearCommSendData(void) {
    memset(gCommSendData, 0, 0xfe);
}

// Link error handler (the callback in gSystemState.mCommErrorFunc, run only by HaltOnCommError, so never in this
// build): with 0 it tears down the display, the track and the waves; otherwise it opens the text screen and prints a
// message that the wireless communication failed, in the owner's language (2 French, 3 German, 4 Italian, 5 Spanish,
// English otherwise).
void ShowCommError(int a) {
    OSOwnerInfo info;

    if (a == 0) {
        SetFaded(3);
        ReleaseTextureLayers();
        ReleaseGxPool();
        SetLcdSwap(0);
        FreeVramBanks(2);
        FreeVramBanks(6);
        gSystemState.mFlags &= ~0x20000;
        SetFadeClear(2);
        SetLcdSwap(1);
        ReleaseTrack(0);
        StopAllWaves();
        return;
    }
    OS_GetOwnerInfo(&info);
    OpenTextScreen("");
    switch (info.language) {
        case 2:
            PrintTextScreenRow("    ERREUR DE COMMUNICATION", 10);
            PrintTextScreenRow(" VEUILLEZ ETEINDRE LA CONSOLE", 12);
            break;
        case 3:
            PrintTextScreenRow("        FEHLER BEI DER", 10);
            PrintTextScreenRow("       DATENUBERTRAGUNG", 12);
            PrintTextScreenRow(" BITTE SCHALTE DAS SYSTEM AUS", 14);
            break;
        case 4:
            PrintTextScreenRow("   ERRORE NELLA COMUNICAZIONE", 10);
            PrintTextScreenRow("       SPEGNI LA CONSOLE", 12);
            break;
        case 5:
            PrintTextScreenRow("    ERROR EN LA COMUNICACION", 10);
            PrintTextScreenRow("        APAGA LA CONSOLA", 12);
            break;
        default:
            PrintTextScreenRow(" WIRELESS COMMUNICATIONS ERROR", 10);
            PrintTextScreenRow("  PLEASE TURN THE POWER OFF", 12);
            break;
    }
}

// When the error state (-2) is set: runs the handler with 0, then runs it with 1 and the frame update forever. In this
// build the error state is never set, so it returns at once.
void HaltOnCommError(void) {
    if (gSystemState.mUnk_2a4[1] != -2) {
        return;
    }
    gSystemState.mCommErrorFunc(0);
    while (1) {
        gSystemState.mCommErrorFunc(1);
        UpdateSystem(0);
    }
}

void UpdateCommChild(void) {
    switch (WhGetSysState()) {
        case WH_SYSSTATE_STOP:
        case WH_SYSSTATE_SCANNING:
        case WH_SYSSTATE_BUSY:
        case WH_SYSSTATE_MEASURECHANNEL:
            break;
        case WH_SYSSTATE_CONNECT_FAIL:
            WhShutDownOrFail();
            break;
        case WH_SYSSTATE_IDLE:
            if (gSystemState.mConnectAttemptCount < 5) {
                ConnectCommAsChild();
                gSystemState.mConnectAttemptCount++;
                break;
            }
        case WH_SYSSTATE_ERROR:
            func_02034cdc();
            break;
        case WH_SYSSTATE_CONNECTED:
        case WH_SYSSTATE_DATASHARING:
        case WH_SYSSTATE_KEYSHARING:
            gSystemState.mConnectAttemptCount = 0;
            ExchangeCommRecords();
            break;
    }
}

void ConnectCommAsChild(void) {
    WhConnectAsChild(WH_CONNECT_MODE_CHILD_DATASHARING, gCommConnectData);
}

void func_02034cdc(void) {}

void WriteU32AsHalfwords(u32 value, void *dst) {
    ((u16 *) dst)[0] = value;
    ((u16 *) dst)[1] = value >> 16;
}

u32 ReadU32FromHalfwords(void *src) {
    return ((u16 *) src)[0] + (((u16 *) src)[1] << 16);
}

// Stores a 32-bit value as four bytes, low byte first.
void WriteU32Bytes(u32 value, u8 *dst) {
    u32 i;
    for (i = 0; i < 4; i++) {
        dst[i] = value;
        value >>= 8;
    }
}

// Sends file data to the peer. For each queued transfer whose state byte is COMM_PENDING and no packet is out, it
// reads up to COMM_CHUNK bytes at the transfer's position and sends them behind a COMM_PENDING tag and the position;
// when the send is accepted the position advances, and the transfer ends when nothing is left. For each received
// record it answers a request (COMM_REQUEST) with a reply carrying two values of the file table and the table's
// size, and queues a range message (COMM_RANGE) unless it overlaps another queued transfer.
void ServeCommFiles(void) {
    u8 order[4];
    u32 i;
    u32 n;
    u32 idxA;
    u32 idxB;
    u8 *buf;
    Unk_020c1b0c *c;
    Unk_CommRecord *rec;
    u32 a;
    u32 b;
    u32 j;
    u32 state;
    u32 len;
    u8 *buf2;

    (void) gSystemState.mElapsedFrames;
    n = 0;
    for (i = 0; i < 1; i++) {
        n++;
        order[i] = n;
        if (n >= 1) {
            n = 0;
        }
    }
    c = &gSystemState.mUnk_250;
    for (i = 0; i < 1; i++) {
        idxA = order[i];
        if (gSystemState.mUnk_250.mUnk_10[idxA] == COMM_PENDING && (data_020c9e04 & COMM_BUSY) == 0) {
            buf = data_020c66d4->mPacket;
            len = c->mUnk_1c[idxA];

            if (len >= COMM_CHUNK) {
                len = COMM_CHUNK;
            }
            buf[0] = COMM_PENDING;
            WriteU32Bytes(c->mUnk_14[idxA], buf + 1);
            func_020425f4(&gBankFile, c->mUnk_14[idxA], 0);
            func_020425e4(&gBankFile, buf + 5, len);
            data_020c9e04 |= COMM_BUSY;
            buf = data_020c66d4->mPacket;
            if (func_02046e28(PacketSentCallback, 0, buf, len + 5, 3, 10, 0) == 2) {
                c->mUnk_14[idxA] += len;
                c->mUnk_1c[idxA] -= len;
                if (c->mUnk_1c[idxA] == 0) {
                    gSystemState.mUnk_250.mUnk_10[idxA] = 0;
                    (void) ((gSystemState.mUnk_2c8 & COMM_TRACE) ? 0 : 0);
                }
            }
        }
    }
    for (i = 0; i < 1; i++) {
        idxB = order[i];
        rec = (Unk_CommRecord *) (gCommRecords + idxB * COMM_RECORD_SIZE);
        if (gCommRecordReceived[idxB] == 0) {
            continue;
        }
        switch (rec->mUnk_0) {
            case COMM_REQUEST: {
                gSystemState.mUnk_296 = 0;
                buf2 = ReserveLinkMessage(0x12);
                rec->mUnk_0 = 0;
                buf2[0] = COMM_REPLY;
                buf2[2] = idxB;
                *(u16 *) (buf2 + 4) = rec->mUnk_4;
                WriteU32AsHalfwords(((FileEntry *) gSystemState.mFileTable)[rec->mUnk_4].mOffset, buf2 + 6);
                WriteU32AsHalfwords(((FileEntry *) gSystemState.mFileTable)[rec->mUnk_4].mStoredSize, buf2 + 10);
                WriteU32AsHalfwords(gSystemState.mFileTableSize, buf2 + 14);
                (void) ((gSystemState.mUnk_2c8 & COMM_TRACE) ? 0 : 0);
                break;
            }
            case COMM_RANGE:
                a = ReadU32FromHalfwords((u8 *) rec + 6);
                b = ReadU32FromHalfwords((u8 *) rec + 2);
                rec->mUnk_0 = 0;
                state = COMM_PENDING;
                for (j = 1; j < 2; j++) {
                    if (idxB == j) {
                        continue;
                    }
                    if (gSystemState.mUnk_250.mUnk_10[j] != COMM_PENDING) {
                        continue;
                    }
                    if ((a <= gSystemState.mUnk_250.mUnk_14[j] && a + b > gSystemState.mUnk_250.mUnk_14[j]) ||
                        (gSystemState.mUnk_250.mUnk_14[j] <= a &&
                         gSystemState.mUnk_250.mUnk_14[j] + gSystemState.mUnk_250.mUnk_1c[j] > a)) {
                        state = 0;
                    }
                }
                if (state != 0) {
                    gSystemState.mUnk_250.mUnk_10[idxB] = state;
                    gSystemState.mUnk_250.mUnk_14[idxB] = a;
                    gSystemState.mUnk_250.mUnk_1c[idxB] = b;
                } else {
                    (void) ((gSystemState.mUnk_2c8 & COMM_TRACE) ? 0 : 0);
                }
                break;
        }
    }
}

// 1 when the file access goes through the link: bit 9 of the system flags is set and either data is still to be
// received or the send buffer's high nibble is 5. In this build the flag is never set, so it returns 0.
int IsCommReading(void) {
    if (gSystemState.mFlags & 0x200) {
        if (gSystemState.mUnk_250.mUnk_08 != 0 || (gCommSendData[0] >> 4) == 5) {
            return 1;
        }
    }
    return 0;
}

void func_020350a0(void) {
    u8 buf[2000];

    func_020350e8(6);
    if (func_020465b0(buf) != 0) {
        return;
    }
    if (*(u16 *) buf == 7 || *(u16 *) buf == 9 || *(u16 *) buf == 26) {
        func_02047d00();
    }
}

void func_020350e8(u16 value) {
    data_020c6adc.mUnk_0c = value;
}

int func_020350f8(void) {
    return data_020c6adc.mUnk_0c;
}

#pragma thumb on
