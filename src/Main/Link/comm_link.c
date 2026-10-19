#include <nitro/os.h>
#include <nitro/fs.h>
#include <nitro/mi.h>
#include <nitro/wm.h>
#include <Main/clib.h>
#include <Main/Link/comm_link.h>
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
#include <Main/Graphics/gx_pool.h>

Unk_020c6adc data_020c6adc;

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
// Bit 23 of data_020c18bc.mUnk_2c8. Where the code tests it, the result goes nowhere (the test survives but whatever
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

// With the link in state 5 and the state check passed, reads each of the 16 slots: a slot that holds data has its
// record stored in data_020c68e0 and its flag set (data_020c66e0, and a bit in the mask at data_020c18bc.mUnk_288);
// then func_02034a08 follows the mask.
void func_02034954(void) {
    u16 i;

    if (func_0203946c() != 5) {
        return;
    }
    if (func_020396b8(data_020c67e0) == 0) {
        return;
    }
    *data_020c66d4->mUnk_4 = 0;
    data_020c18bc.mUnk_288 = 0;
    for (i = 0; i < COMM_SLOTS; i++) {
        void *src = func_02039698(i);
        if (src != NULL) {
            MI_CpuCopy8(src, data_020c68e0 + i * COMM_RECORD_SIZE, COMM_RECORD_SIZE);
            data_020c66e0[i] = 1;
            data_020c18bc.mUnk_288 |= 1 << i;
        } else {
            data_020c66e0[i] = 0;
        }
    }
    func_02034a08();
}

// Counts (up to 0x800) the calls for which the slot mask has not changed.
void func_02034a08(void) {
    if (data_020c18bc.mUnk_288 != data_020c18bc.mUnk_28a) {
        data_020c18bc.mUnk_28a = data_020c18bc.mUnk_288;
        data_020c18bc.mUnk_28c = 0;
    } else if (data_020c18bc.mUnk_28c < 0x800) {
        (data_020c18bc.mUnk_28c)++;
    }
}

// Clears the command buffer.
void func_02034a44(void) {
    memset(data_020c67e0, 0, 0xfe);
}

// Link error handler (the callback in data_020c18bc.mUnk_24c): with 0 it restores the display set-up; otherwise it
// shows a message that the wireless communication failed, in the owner's language (2 French, 3 German, 4 Italian,
// 5 Spanish, English otherwise).
void func_02034a60(int a) {
    OSOwnerInfo info;

    if (a == 0) {
        func_0201f880(3);
        func_0202c1c4();
        func_02022370();
        func_02020854(0);
        func_0201eb74(2);
        func_0201eb74(6);
        data_020c18bc.mUnk_46c &= ~0x20000;
        func_0201f8d0(2);
        func_02020854(1);
        func_020359d0(0);
        func_0202eeec();
        return;
    }
    OS_GetOwnerInfo(&info);
    func_02027fc2("");
    switch (info.language) {
        case 2:
            func_02028036("    ERREUR DE COMMUNICATION", 10);
            func_02028036(" VEUILLEZ ETEINDRE LA CONSOLE", 12);
            break;
        case 3:
            func_02028036("        FEHLER BEI DER", 10);
            func_02028036("       DATENUBERTRAGUNG", 12);
            func_02028036(" BITTE SCHALTE DAS SYSTEM AUS", 14);
            break;
        case 4:
            func_02028036("   ERRORE NELLA COMUNICAZIONE", 10);
            func_02028036("       SPEGNI LA CONSOLE", 12);
            break;
        case 5:
            func_02028036("    ERROR EN LA COMUNICACION", 10);
            func_02028036("        APAGA LA CONSOLA", 12);
            break;
        default:
            func_02028036(" WIRELESS COMMUNICATIONS ERROR", 10);
            func_02028036("  PLEASE TURN THE POWER OFF", 12);
            break;
    }
}

// When the error state (-2) is set: runs the handler with 0, then runs it with 1 and the frame update forever.
void func_02034be4(void) {
    if (data_020c18bc.mUnk_2a4[1] != -2) {
        return;
    }
    data_020c18bc.mUnk_24c(0);
    while (1) {
        data_020c18bc.mUnk_24c(1);
        func_0201bb40(0);
    }
}

// Per-frame step of the link by its state.
void func_02034c38(void) {
    switch (func_0203946c()) {
        case 0:
        case 2:
        case 3:
        case 7:
            break;
        case 8:
            func_0203970c();
            break;
        case 1:
            if (data_020c18bc.mUnk_2a2 < 5) {
                func_02034cc4();
                data_020c18bc.mUnk_2a2++;
                break;
            }
        case 9:
            func_02034cdc();
            break;
        case 4:
        case 5:
        case 6:
            data_020c18bc.mUnk_2a2 = 0;
            func_02034954();
            break;
    }
}

void func_02034cc4(void) {
    func_02039598(5, data_020c6720);
}

void func_02034cdc(void) {}

// Stores a 32-bit value as two halfwords.
void func_02034ce0(u32 value, void *dst) {
    ((u16 *) dst)[0] = value;
    ((u16 *) dst)[1] = value >> 16;
}

// Reads a 32-bit value stored as two halfwords.
u32 func_02034cf0(void *src) {
    return ((u16 *) src)[0] + (((u16 *) src)[1] << 16);
}

// Stores a 32-bit value as four bytes, low byte first.
void func_02034d00(u32 value, u8 *dst) {
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
void func_02034d1c(void) {
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

    (void) data_020c18bc.mUnk_45c;
    n = 0;
    for (i = 0; i < 1; i++) {
        n++;
        order[i] = n;
        if (n >= 1) {
            n = 0;
        }
    }
    c = &data_020c18bc.mUnk_250;
    for (i = 0; i < 1; i++) {
        idxA = order[i];
        if (data_020c18bc.mUnk_250.mUnk_10[idxA] == COMM_PENDING && (data_020c9e04 & COMM_BUSY) == 0) {
            buf = data_020c66d4->mUnk_8;
            len = c->mUnk_1c[idxA];

            if (len >= COMM_CHUNK) {
                len = COMM_CHUNK;
            }
            buf[0] = COMM_PENDING;
            func_02034d00(c->mUnk_14[idxA], buf + 1);
            func_020425f4(&data_020c1874, c->mUnk_14[idxA], 0);
            func_020425e4(&data_020c1874, buf + 5, len);
            data_020c9e04 |= COMM_BUSY;
            buf = data_020c66d4->mUnk_8;
            if (func_02046e28(func_02039d74, 0, buf, len + 5, 3, 10, 0) == 2) {
                c->mUnk_14[idxA] += len;
                c->mUnk_1c[idxA] -= len;
                if (c->mUnk_1c[idxA] == 0) {
                    data_020c18bc.mUnk_250.mUnk_10[idxA] = 0;
                    (void) ((data_020c18bc.mUnk_2c8 & COMM_TRACE) ? 0 : 0);
                }
            }
        }
    }
    for (i = 0; i < 1; i++) {
        idxB = order[i];
        rec = (Unk_CommRecord *) (data_020c68e0 + idxB * COMM_RECORD_SIZE);
        if (data_020c66e0[idxB] == 0) {
            continue;
        }
        switch (rec->mUnk_0) {
            case COMM_REQUEST: {
                data_020c18bc.mUnk_296 = 0;
                buf2 = func_02039cd4(0x12);
                rec->mUnk_0 = 0;
                buf2[0] = COMM_REPLY;
                buf2[2] = idxB;
                *(u16 *) (buf2 + 4) = rec->mUnk_4;
                func_02034ce0(((Unk_FileEntry *) data_020c18bc.mUnk_000)[rec->mUnk_4].mUnk_0, buf2 + 6);
                func_02034ce0(((Unk_FileEntry *) data_020c18bc.mUnk_000)[rec->mUnk_4].mUnk_4, buf2 + 10);
                func_02034ce0(data_020c18bc.mUnk_334, buf2 + 14);
                (void) ((data_020c18bc.mUnk_2c8 & COMM_TRACE) ? 0 : 0);
                break;
            }
            case COMM_RANGE:
                a = func_02034cf0((u8 *) rec + 6);
                b = func_02034cf0((u8 *) rec + 2);
                rec->mUnk_0 = 0;
                state = COMM_PENDING;
                for (j = 1; j < 2; j++) {
                    if (idxB == j) {
                        continue;
                    }
                    if (data_020c18bc.mUnk_250.mUnk_10[j] != COMM_PENDING) {
                        continue;
                    }
                    if ((a <= data_020c18bc.mUnk_250.mUnk_14[j] && a + b > data_020c18bc.mUnk_250.mUnk_14[j]) ||
                        (data_020c18bc.mUnk_250.mUnk_14[j] <= a &&
                         data_020c18bc.mUnk_250.mUnk_14[j] + data_020c18bc.mUnk_250.mUnk_1c[j] > a)) {
                        state = 0;
                    }
                }
                if (state != 0) {
                    data_020c18bc.mUnk_250.mUnk_10[idxB] = state;
                    data_020c18bc.mUnk_250.mUnk_14[idxB] = a;
                    data_020c18bc.mUnk_250.mUnk_1c[idxB] = b;
                } else {
                    (void) ((data_020c18bc.mUnk_2c8 & COMM_TRACE) ? 0 : 0);
                }
                break;
        }
    }
}

// 1 when the file access goes through the link: bit 9 of the system flags is set and either data is still to be
// received or the command buffer's high nibble is 5.
int func_02035058(void) {
    if (data_020c18bc.mUnk_46c & 0x200) {
        if (data_020c18bc.mUnk_250.mUnk_08 != 0 || (data_020c67e0[0] >> 4) == 5) {
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
