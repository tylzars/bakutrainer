#include <nitro/os.h>
#include <nitro/pxi.h>

#define REG_IPCSYNC (*(vu16 *) 0x4000180)
#define REG_IPCFIFOCNT (*(vu16 *) 0x4000184)
#define REG_IPCFIFOSEND (*(vu32 *) 0x4000188)
#define REG_IPCFIFORECV (*(vu32 *) 0x4100000)

#define IPCFIFOCNT_SEND_FULL 0x0002
#define IPCFIFOCNT_SEND_CLEAR 0x0008
#define IPCFIFOCNT_RECV_EMPTY 0x0100
#define IPCFIFOCNT_RECV_NOT_EMPTY_IRQ 0x0400
#define IPCFIFOCNT_ERROR 0x4000
#define IPCFIFOCNT_ENABLE 0x8000

#define IRQ_MASK_IPC_RECV_FIFO_NOT_EMPTY 0x40000

#define PXI_TAG_COUNT 32

// Shared main-memory block; flags[proc] has bit n set while processor proc has a receive callback for tag n.
typedef struct {
    /* 000 */ u8 pad[0x388];
    /* 388 */ u32 flags[2];
} PXIWork;

#define PXI_SHARED_WORK ((PXIWork *) 0x27ffc00)

typedef union {
    u32 raw;
    struct {
        u32 tag : 5;
        u32 err : 1;
        u32 data : 26;
    } bits;
} PXIFifoMessage;

static vu16 pxiInitialized;
static PXIFifoCallback pxiCallback[PXI_TAG_COUNT];

void PXI_Init(void) {
    PxiFifoSetup();
}

void PxiFifoSetup(void) {
    OSIntrMode e = OS_DisableInterrupts();
    int timeout;
    u32 remote;
    int count;
    PXIWork *w = PXI_SHARED_WORK;
    if (pxiInitialized == 0) {
        int i;
        pxiInitialized = 1;
        w->flags[0] = 0;
        for (i = 0; i < PXI_TAG_COUNT; i++) {
            pxiCallback[i] = NULL;
        }
        REG_IPCFIFOCNT = IPCFIFOCNT_ENABLE | IPCFIFOCNT_ERROR | IPCFIFOCNT_RECV_NOT_EMPTY_IRQ | IPCFIFOCNT_SEND_CLEAR;
        OS_ResetRequestIrqMask(IRQ_MASK_IPC_RECV_FIFO_NOT_EMPTY);
        OS_SetIrqFunction(IRQ_MASK_IPC_RECV_FIFO_NOT_EMPTY, PXIi_HandlerRecvFifoNotEmpty);
        OS_EnableIrqMask(IRQ_MASK_IPC_RECV_FIFO_NOT_EMPTY);
        // Handshake with the other processor: echo its sync value until it reads 0 after a few rounds.
        count = 0;
        while (1) {
            remote = REG_IPCSYNC & 0xf;
            REG_IPCSYNC = remote << 8;
            if (remote == 0 && count > 4) {
                break;
            }
            timeout = 1000;
            if ((REG_IPCSYNC & 0xf) == remote) {
                while (1) {
                    if (timeout <= 0) {
                        count = 0;
                        break;
                    }
                    timeout--;
                    if ((REG_IPCSYNC & 0xf) != remote) {
                        break;
                    }
                }
            }
            count++;
        }
    }
    OS_RestoreInterrupts(e);
}

void PXI_SetFifoRecvCallback(int fifoTag, PXIFifoCallback callback) {
    OSIntrMode e = OS_DisableInterrupts();
    PXIWork *w = PXI_SHARED_WORK;
    pxiCallback[fifoTag] = callback;
    if (callback != NULL) {
        w->flags[0] |= 1 << fifoTag;
    } else {
        w->flags[0] &= ~(1 << fifoTag);
    }
    OS_RestoreInterrupts(e);
}

BOOL PXI_IsCallbackReady(int fifoTag, int proc) {
    PXIWork *w = PXI_SHARED_WORK;
    BOOL ready = TRUE;
    if (!(w->flags[proc] & (1 << fifoTag))) {
        ready = FALSE;
    }
    return ready;
}

int PXI_SendWordByFifo(int fifoTag, u32 data, BOOL err) {
    PXIFifoMessage msg;
    OSIntrMode e;
    msg.bits.tag = fifoTag;
    msg.bits.err = err;
    msg.bits.data = data;
    if (REG_IPCFIFOCNT & IPCFIFOCNT_ERROR) {
        REG_IPCFIFOCNT |= IPCFIFOCNT_ENABLE | IPCFIFOCNT_ERROR;
        return -1;
    }
    e = OS_DisableInterrupts();
    if (REG_IPCFIFOCNT & IPCFIFOCNT_SEND_FULL) {
        OS_RestoreInterrupts(e);
        return -2;
    }
    REG_IPCFIFOSEND = msg.raw;
    OS_RestoreInterrupts(e);
    return 0;
}

void PXIi_HandlerRecvFifoNotEmpty(void) {
    PXIFifoMessage msg;
    int result;
    OSIntrMode e;
    PXIFifoCallback cb;
    while (1) {
        if (REG_IPCFIFOCNT & IPCFIFOCNT_ERROR) {
            REG_IPCFIFOCNT |= IPCFIFOCNT_ENABLE | IPCFIFOCNT_ERROR;
            result = -3;
        } else {
            e = OS_DisableInterrupts();
            if (REG_IPCFIFOCNT & IPCFIFOCNT_RECV_EMPTY) {
                OS_RestoreInterrupts(e);
                result = -4;
            } else {
                msg.raw = REG_IPCFIFORECV;
                OS_RestoreInterrupts(e);
                result = 0;
            }
        }
        if (result == -4) {
            return;
        }
        if (result == -3) {
            continue;
        }
        if (msg.bits.tag == 0) {
            continue;
        }
        cb = pxiCallback[msg.bits.tag];
        if (cb != NULL) {
            cb(msg.bits.tag, msg.bits.data, msg.bits.err);
            continue;
        }
        if (msg.bits.err) {
            continue;
        }
        msg.bits.err = 1;
        if (REG_IPCFIFOCNT & IPCFIFOCNT_ERROR) {
            REG_IPCFIFOCNT |= IPCFIFOCNT_ENABLE | IPCFIFOCNT_ERROR;
            continue;
        }
        e = OS_DisableInterrupts();
        if (REG_IPCFIFOCNT & IPCFIFOCNT_SEND_FULL) {
            OS_RestoreInterrupts(e);
            continue;
        }
        REG_IPCFIFOSEND = msg.raw;
        OS_RestoreInterrupts(e);
    }
}
