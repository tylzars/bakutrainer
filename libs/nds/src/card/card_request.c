#include <nitro/card.h>
#include <nitro/os.h>
#include <nitro/pxi.h>

void func_02045e40(int tag, u32 data, BOOL err) {
    CardCommon *c;
    if (tag != CARD_PXI_TAG) {
        return;
    }
    if (err == 0) {
        return;
    }
    c = (CardCommon *) data_020cc880;
    c->flags &= ~CF_PXIWAIT;
    OS_WakeupThreadDirect(c->curThread);
}

void func_02045e74(void *arg) {
    CardCommon *c = (CardCommon *) data_020cc880;
    for (;;) {
        u32 e = OS_DisableInterrupts();
        while (!(c->flags & CF_TASK)) {
            OS_SleepThread(NULL);
        }
        OS_RestoreInterrupts(e);
        c->task(c);
    }
}

int func_02045ec4(CardCommon *c, u32 cmd, int tries) {
    u32 e;
    if (!(c->flags & CF_PXI)) {
        c->flags |= CF_PXI;
        if (!PXI_IsCallbackReady(CARD_PXI_TAG, 1)) {
            do {
                OS_SpinWait(100);
            } while (!PXI_IsCallbackReady(CARD_PXI_TAG, 1));
        }
        func_02045ec4(c, 0, 1);
    }
    DC_FlushRange(c->arg, 0x60);
    DC_WaitWriteBufferEmpty();
    for (;;) {
        c->mUnk_04 = cmd;
        c->flags |= CF_PXIWAIT;
        while (PXI_SendWordByFifo(CARD_PXI_TAG, cmd, 1) < 0) {
        }
        if (cmd == 0) {
            CardArg *a = c->arg;
            while (PXI_SendWordByFifo(CARD_PXI_TAG, (u32) a, 1) < 0) {
            }
        }
        e = OS_DisableInterrupts();
        while (c->flags & CF_PXIWAIT) {
            OS_SleepThread(NULL);
        }
        OS_RestoreInterrupts(e);
        DC_InvalidateRange(c->arg, 0x60);
        if (c->arg->result != 4) {
            break;
        }
        if (--tries <= 0) {
            break;
        }
    }
    return c->arg->result == 0;
}
