#include <nitro/card.h>
#include <nitro/mi.h>
#include <nitro/os.h>
#include <nitro/pxi.h>
#include <nitro/svc.h>
#include <nitro/pm.h>

CardPullout data_020cd0e0;

// No code in the ROM uses it; the link keeps it (force_active.txt).
u8 data_020cd0e8[0x18];

void func_02046018(void) {
    PXI_Init();
    PXI_SetFifoRecvCallback(CARD_PULLOUT_PXI_TAG, func_02046044);
    data_020cd0e0.callback = NULL;
}

void func_02046044(int tag, u32 data, BOOL err) {
    if ((data & 0x3f) == CARD_PULLOUT_MESSAGE) {
        BOOL r;
        if (data_020cd0e0.pulledOut != 0) {
            return;
        }
        data_020cd0e0.pulledOut = 1;
        r = 1;
        if (data_020cd0e0.callback != NULL) {
            r = data_020cd0e0.callback();
        }
        if (r == 0) {
            return;
        }
        func_020460a8();
    } else {
        OS_Terminate();
    }
}

BOOL func_02046098(void) {
    return data_020cd0e0.pulledOut;
}

void func_020460a8(void) {
    int doit = 1;
    MI_StopDma(0);
    MI_StopDma(1);
    MI_StopDma(2);
    MI_StopDma(3);
    if (((s32) (*(vu16 *) 0x27fffa8 & 0x8000)) >> 15) {
        int r = func_02043d5c();
        if (r == 4) {
            do {
                OS_SpinWait(0xa3a47);
                r = func_02043d5c();
            } while (r == 4);
        }
        if (r == 0) {
            doit = 0;
        }
    }
    if (doit) {
        func_02046194(1, 1);
    }
    OS_Terminate();
}

void func_02046134(u32 id) {
    vu32 cur;
    u32 e;
    u32 *p;
    if (*(vu16 *) 0x27ffc10 == 0) {
        p = (u32 *) 0x27ff800;
    } else {
        p = (u32 *) 0x27ffc00;
    }
    cur = *p;
    if (id == cur) {
        return;
    }
    e = OS_DisableInterrupts();
    func_02046044(CARD_PULLOUT_PXI_TAG, CARD_PULLOUT_MESSAGE, 0);
    OS_RestoreInterrupts(e);
}

void func_02046194(u32 a, u32 b) {
    if (PXI_SendWordByFifo(CARD_PULLOUT_PXI_TAG, a, 0) == 0) {
        return;
    }
    do {
        WaitByLoop(b);
    } while (PXI_SendWordByFifo(CARD_PULLOUT_PXI_TAG, a, 0) != 0);
}
