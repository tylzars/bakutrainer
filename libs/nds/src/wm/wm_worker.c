#include <nitro/os.h>
#include <nitro/mi.h>
#include <nitro/pxi.h>
#include <nitro/math.h>
#include <nitro/wm.h>

typedef struct {
    /* 000 */ u8 mUnk_000[0x508];
    /* 508 */ WMCallback mUnk_508;
    /* 50c */ u8 mUnk_50c[1];
    /* 50d */ u8 mUnk_50d;
    /* 50e */ u8 mUnk_50e[0xe];
    /* 51c */ void (*mUnk_51c)(u32 code, void *arg);
    /* 520 */ u8 mUnk_520[6];
    /* 526 */ u16 mUnk_526;
    /* 528 */ u8 mUnk_528[0xbc];
    /* 5e4 */ u32 mUnk_5e4;
} WMWorkerState;

typedef struct {
    /* 0000 */ u8 mUnk_0000[0x1316];
    /* 1316 */ u16 mUnk_1316;
    /* 1318 */ u8 mUnk_1318[8];
    /* 1320 */ u32 mUnk_1320;
} WMWorkerShared;

typedef struct {
    /* 00 */ u32 mUnk_00;
    /* 04 */ u32 mUnk_04;
    /* 08 */ WMWorkerState *state;
    /* 0c */ WMWorkerShared *shared;
} WMWorkerLink;

extern WMWorkerLink data_020cdba0;
extern WMTaskThread *data_020cdbe0;

void func_02047be4(void *arg) {
    data_020cdba0.state->mUnk_50d = 0;
    data_020cdba0.shared->mUnk_1316 = 0;
    if (data_020cdba0.state->mUnk_51c != 0) {
        data_020cdba0.state->mUnk_51c(0x11, arg);
    }
}

int func_02047c28(void) {
    int r = func_0204684c(data_020cdba0.state->mUnk_508);
    func_02047d3c(1, r);
    if (r == 2) {
        r = 0;
    }
    return r;
}

int func_02047c60(void) {
    return func_02047c28();
}

int func_02047c6c(void) {
    int r = 1;
    u32 e = OS_DisableInterrupts();
    if (data_020cdba0.state->mUnk_50d == 0) {
        func_02047be4(0);
    } else if (data_020cdba0.state->mUnk_526 == 0) {
        data_020cdba0.state->mUnk_5e4 = 0;
        data_020cdba0.state->mUnk_526 = r;
        if (func_02047d74()) {
            func_02047ef4(func_02047c60);
            r = 0;
        } else {
            r = func_02047c28();
        }
    }
    OS_RestoreInterrupts(e);
    return r;
}

void func_02047d00(void) {
    u32 e = OS_DisableInterrupts();
    if (data_020cdba0.shared->mUnk_1320 == 0) {
        OS_Terminate();
    }
    func_02047c6c();
    OS_RestoreInterrupts(e);
}

void func_02047d3c(u32 code, u32 status) {
    struct {
        u16 code;
        u16 status;
    } report;
    if (status == 2 || status == 0) {
        return;
    }
    report.code = code;
    report.status = status;
    data_020cdba0.state->mUnk_51c(0xff, &report);
}

BOOL func_02047d74(void) {
    return data_020cdbe0 != 0;
}

void func_02047d90(WMTask *task, u32 a, void *b, u32 prio) {
    WMTaskThread *t = data_020cdbe0;
    u32 e;
    WMTask *p;
    WMTask *q;
    if (!func_02047d74()) {
        return;
    }
    if (task->busy) {
        return;
    }
    if (prio > 31) {
        u32 cur = func_0203e098(&t->thread);
        if (prio == 32) {
            prio = cur != 0 ? cur - 1 : 0;
        } else if (prio == 33) {
            prio = cur < 31 ? cur + 1 : 31;
        } else if (prio == 34) {
            prio = cur;
        } else {
            prio = 31;
        }
    }
    e = OS_DisableInterrupts();
    task->busy = 1;
    task->prio = prio;
    task->mUnk_08 = a;
    task->mUnk_0c = b;
    if (t->queue == 0) {
        if (task == &t->exitTask) {
            data_020cdbe0 = 0;
        }
        t->queue = task;
        OS_WakeupThreadDirect(&t->thread);
    } else {
        p = t->queue;
        if (task == &t->exitTask) {
            while (p->next != 0) {
                p = p->next;
            }
            p->next = task;
            data_020cdbe0 = 0;
        } else if (prio < p->prio) {
            t->queue = task;
            task->next = p;
        } else {
            while ((q = p->next) != 0 && prio >= q->prio) {
                p = q;
            }
            task->next = q;
            p->next = task;
        }
    }
    OS_RestoreInterrupts(e);
}

void func_02047ef4(void *callback) {
    u32 e = OS_DisableInterrupts();
    if (func_02047d74()) {
        func_02047d90(&data_020cdbe0->exitTask, 0, callback, 0);
    }
    OS_RestoreInterrupts(e);
}
