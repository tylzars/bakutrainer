#include <nitro/os.h>
#include <nitro/pxi.h>
#include <nitro/mic.h>

#define MIC_PXI_TAG 9

// Shared memory words the other processor fills in: a status word and the address of the latest sample.
#define MIC_SHARED_STATUS ((vu32 *) 0x027fff90)
#define MIC_SHARED_LAST ((vu16 *) 0x027fff94)

typedef struct {
    /* 00 */ u16 init;
    /* 04 */ vu32 busy;
    /* 08 */ void (*cb)(u32 result, void *arg);
    /* 0c */ void *cbArg;
    /* 10 */ u32 result;
    /* 14 */ void (*fullCb)(u32 result, void *arg);
    /* 18 */ void *fullArg;
    /* 1c */ u16 *lastAddr;
} MICWork;

extern MICWork micWork;

void MIC_Init(void) {
    if (micWork.init) {
        return;
    }
    micWork.init = 1;
    micWork.busy = 0;
    micWork.cb = NULL;
    PXI_Init();
    while (!PXI_IsCallbackReady(MIC_PXI_TAG, 1)) {
    }
    *MIC_SHARED_STATUS = 0;
    PXI_SetFifoRecvCallback(MIC_PXI_TAG, func_02043464);
}

u32 MIC_StartAutoSamplingAsync(MICAutoParam *p, void (*cb)(u32, void *), void *arg) {
    u32 type, e;
    if ((u32) p->buffer & 0x1f) {
        return 2;
    }
    if (p->size & 0x1f) {
        return 2;
    }
    if (p->size == 0) {
        return 2;
    }
    if (p->rate < 0x400) {
        return 2;
    }
    switch (p->type) {
        case 0:
            type = 0;
            break;
        case 1:
            type = 1;
            break;
        case 2:
            type = 2;
            break;
        case 3:
            type = 3;
            break;
        case 4:
            type = 5;
            break;
        case 5:
            type = 7;
            break;
        default:
            return 2;
    }
    if (p->loop) {
        type = (u8) (type | 0x10);
    } else {
        type = (u8) type;
    }
    type = (u8) type;
    e = OS_DisableInterrupts();
    if (micWork.busy) {
        OS_RestoreInterrupts(e);
        return 1;
    }
    micWork.busy = 1;
    OS_RestoreInterrupts(e);
    micWork.cb = cb;
    micWork.cbArg = arg;
    micWork.fullCb = p->full;
    micWork.fullArg = p->fullArg;
    return func_0204358c((u32) p->buffer, p->size, p->rate, type) ? 0 : 3;
}

u32 MIC_StartAutoSampling(MICAutoParam *p) {
    micWork.result = MIC_StartAutoSamplingAsync(p, func_020436b8, NULL);
    if (micWork.result == 0) {
        func_020436c8();
    }
    return micWork.result;
}

u32 MIC_StopAutoSamplingAsync(void (*cb)(u32, void *), void *arg) {
    u32 e = OS_DisableInterrupts();
    if (micWork.busy) {
        OS_RestoreInterrupts(e);
        return 1;
    }
    micWork.busy = 1;
    OS_RestoreInterrupts(e);
    micWork.cb = cb;
    micWork.cbArg = arg;
    return func_02043690() ? 0 : 3;
}

u32 MIC_StopAutoSampling(void) {
    micWork.result = MIC_StopAutoSamplingAsync(func_020436b8, NULL);
    if (micWork.result == 0) {
        func_020436c8();
    }
    return micWork.result;
}

u32 func_02043454(void) {
    return *MIC_SHARED_STATUS;
}

// Receive handler for the microphone's FIFO channel.
void func_02043464(int tag, u32 data, BOOL err) {
    u32 res;
    u16 cmd, r;
    if (err) {
        if (micWork.busy) {
            micWork.busy = 0;
        }
        if (micWork.cb) {
            void (*f)(u32, void *) = micWork.cb;
            micWork.cb = NULL;
            f(6, micWork.cbArg);
        }
    }
    cmd = (data & 0x7f00) >> 8;
    r = data & 0xff;
    switch (r) {
        case 0:
            res = 0;
            break;
        case 1:
            res = 4;
            break;
        case 2:
            res = 2;
            break;
        case 3:
            res = 5;
            break;
        case 4:
            res = 1;
            break;
        default:
            res = 6;
            break;
    }
    if (cmd == 0x51) {
        if (micWork.fullCb) {
            micWork.fullCb(res, micWork.fullArg);
        }
        return;
    }
    if (cmd == 0x40) {
        if (micWork.lastAddr) {
            *micWork.lastAddr = *MIC_SHARED_LAST;
        }
    }
    if (micWork.busy) {
        micWork.busy = 0;
    }
    if (micWork.cb) {
        void (*f)(u32, void *) = micWork.cb;
        micWork.cb = NULL;
        f(res, micWork.cbArg);
    }
}

// Sends the start command and its parameters, one 16-bit piece per word.
BOOL func_0204358c(u32 buf, u32 size, u32 rate, u32 type) {
    if (PXI_SendWordByFifo(MIC_PXI_TAG, (type | 0x4100) | 0x2000000, 0) < 0) {
        return 0;
    }
    if (PXI_SendWordByFifo(MIC_PXI_TAG, (buf >> 16) | 0x10000, 0) < 0) {
        return 0;
    }
    if (PXI_SendWordByFifo(MIC_PXI_TAG, (u16) buf | 0x20000, 0) < 0) {
        return 0;
    }
    if (PXI_SendWordByFifo(MIC_PXI_TAG, (size >> 16) | 0x30000, 0) < 0) {
        return 0;
    }
    if (PXI_SendWordByFifo(MIC_PXI_TAG, (u16) size | 0x40000, 0) < 0) {
        return 0;
    }
    if (PXI_SendWordByFifo(MIC_PXI_TAG, (rate >> 16) | 0x50000, 0) < 0) {
        return 0;
    }
    return PXI_SendWordByFifo(MIC_PXI_TAG, 0x1060000 | (u16) rate, 0) >= 0;
}

BOOL func_02043690(void) {
    return PXI_SendWordByFifo(MIC_PXI_TAG, 0x3004200, 0) >= 0;
}

void func_020436b8(u32 result, void *arg) {
    micWork.result = result;
}

// Waits while a request is still pending (spins on micWork.busy).
void func_020436c8(void) {
    vu32 *busy = &micWork.busy;
    while (*busy == 1) {
    }
}
