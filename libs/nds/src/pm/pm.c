#include <nitro/os.h>
#include <nitro/mi.h>
#include <nitro/pxi.h>
#include <nitro/svc.h>
#include <nitro/pm.h>
#include <nitro/ctrdg.h>

#define PM_PXI_TAG 8

#define REG_POWCNT (*(vu16 *) 0x04000304)
#define REG_DISPCNT_A ((vu32 *) 0x04000000)
#define REG_DISPCNT_B ((vu32 *) 0x04001000)

// Words in shared memory: a frame counter the other processor advances, and a status word.
#define PM_SHARED_FRAME_COUNT ((vu32 *) 0x027ffc3c)
#define PM_SHARED_WORD_C40 ((vu16 *) 0x027ffc40)

#define PM_IRQ_ALL 0x3fffff
#define PM_IRQ_VCOUNT 0x8
#define PM_IRQ_FIFO_RECV 0x40000
#define PM_IRQ_BIT20 0x100000

// Spin count used between retries of a request.
#define PM_RETRY_WAIT 0xa3a47

typedef struct {
    /* 00 */ u16 init;
    /* 04 */ u32 mUnk_04;
    /* 08 */ u32 mUnk_08;
    /* 0c */ u32 mUnk_0c;
    /* 10 */ u32 mUnk_10;
    /* 14 */ PMCallbackInfo *postList;
    /* 18 */ PMCallbackInfo *preList;
    /* 1c */ u32 lock;
    /* 20 */ void (*cb)(u32 result, void *arg);
    /* 24 */ void *cbArg;
    /* 28 */ u16 *resultPtr;
} PMWork;

extern PMWork pmWork;
void (*pmWaitFunc)(void) = func_02043700;

// Takes the request lock; returns 0 if a request is already pending.
BOOL func_020436e0(void) {
    if (pmWork.lock != 0) {
        return 0;
    }
    pmWork.lock = 1;
    return 1;
}

// Waits for the pending request to finish, servicing the FIFO by hand while in IRQ mode.
void func_02043700(void) {
    vu32 *l = (vu32 *) &pmWork.lock;
    if (pmWork.lock == 0) {
        return;
    }
    do {
        if (OS_GetProcMode() == 0x12) {
            PXIi_HandlerRecvFifoNotEmpty();
        }
    } while (*l != 0);
}

// Waits for the pending request to finish, servicing the FIFO by hand while interrupts are masked.
void func_02043740(void) {
    vu32 *l = (vu32 *) &pmWork.lock;
    if (pmWork.lock == 0) {
        return;
    }
    do {
        if (OS_GetCpsrIrqBits() == 0x80 || REG_IME == 0) {
            PXIi_HandlerRecvFifoNotEmpty();
        }
    } while (*l != 0);
}

// Waits for the pending request to finish, servicing the FIFO by hand in either case.
void func_02043790(void) {
    vu32 *l = (vu32 *) &pmWork.lock;
    if (pmWork.lock == 0) {
        return;
    }
    do {
        if (OS_GetCpsrIrqBits() == 0x80 || REG_IME == 0 || OS_GetProcMode() == 0x12) {
            PXIi_HandlerRecvFifoNotEmpty();
        }
    } while (*l != 0);
}

// Request callback that stores the result.
void func_020437ec(u32 v, u32 *p) {
    if (p != NULL) {
        *p = v;
    }
}

// Releases the request lock and passes the result to the pending callback.
void func_020437f8(u32 result) {
    void (*cb)(u32, void *) = pmWork.cb;
    void *arg = pmWork.cbArg;
    pmWork.lock = 0;
    if (cb == NULL) {
        return;
    }
    pmWork.cb = NULL;
    cb(result, arg);
}

void func_02043828(void) {
    if (pmWork.init) {
        return;
    }
    pmWork.init = 1;
    pmWork.lock = 0;
    pmWork.cb = NULL;
    PXI_Init();
    if (!PXI_IsCallbackReady(PM_PXI_TAG, 1)) {
        while (1) {
            WaitByLoop(100);
            if (PXI_IsCallbackReady(PM_PXI_TAG, 1)) {
                break;
            }
        }
    }
    PXI_SetFifoRecvCallback(PM_PXI_TAG, func_020438c0);
    pmWork.mUnk_04 = pmWork.mUnk_08 = *PM_SHARED_FRAME_COUNT;
}

// Receive handler for the power-management FIFO channel.
void func_020438c0(int tag, u32 data, BOOL err) {
    u16 cmd = (data & 0x7f00) >> 8;
    u16 arg = data & 0xff;
    if (err) {
        switch (cmd) {
            case 0x61:
            case 0x62:
                arg = 1;
                break;
            default:
                arg = 2;
                break;
        }
        func_020437f8(arg);
        return;
    }
    switch (cmd) {
        case 0x61:
            if (pmWork.resultPtr) {
                *pmWork.resultPtr = arg;
            }
            arg = 0;
            break;
        case 0x60:
            arg = 0;
            break;
        case 0x62:
            break;
        case 0x63:
            pmWork.mUnk_0c = 1;
            break;
    }
    func_020437f8(arg);
}

// Sends n words as one request and repeats the request until the reply reports success.
void func_02043954(u32 *cmd, int n) {
    vu32 res;
    int i;
    u32 e;
    while (1) {
        e = OS_DisableInterrupts();
        if (!func_020436e0()) {
            OS_RestoreInterrupts(e);
            continue;
        }
        res = 0xffff0000;
        pmWork.cb = (void (*)(u32, void *)) func_020437ec;
        pmWork.cbArg = (void *) &res;
        for (i = 0; i < n; i++) {
            func_02043e5c(cmd[i]);
        }
        OS_RestoreInterrupts(e);
        while (res == 0xffff0000) {
            OS_SpinWait(PM_RETRY_WAIT);
        }
        if (res == 0) {
            return;
        }
        OS_SpinWait(PM_RETRY_WAIT);
    }
}

int func_02043a14(u32 a, u32 b) {
    u32 cmd[2];
    cmd[0] = 0x3006000;
    func_02043954(cmd, 1);
    while (func_02044194(0, 2, 0, 1) != 1) {
    }
    cmd[0] = (a & 0xff) | 0x6200 | 0x2000000;
    cmd[1] = 0x1010000 | (u16) b;
    func_02043954(cmd, 2);
    return 0;
}

// Starts a request (command byte and 16-bit data) on the power-management channel; the reply byte goes to res and the
// result to cb.
int func_02043aa0(u32 cmd, u32 data, u16 *res, void (*cb)(u32, u32 *), u32 *arg) {
    u32 e = OS_DisableInterrupts();
    if (!func_020436e0()) {
        OS_RestoreInterrupts(e);
        return 1;
    }
    pmWork.cb = (void (*)(u32, void *)) cb;
    pmWork.cbArg = arg;
    pmWork.resultPtr = res;
    func_02043e5c((cmd & 0xff) | 0x6100 | 0x2000000);
    func_02043e5c(0x1010000 | (u16) data);
    OS_RestoreInterrupts(e);
    return 0;
}

// Same request, waiting for the reply.
int func_02043b24(u32 cmd, u32 data, u16 *res) {
    u32 r;
    int ret = func_02043aa0(cmd, data, res, func_020437ec, &r);
    if (ret != 0) {
        return ret;
    }
    pmWaitFunc();
    return r;
}

int func_02043b68(u32 a, void (*cb)(u32, u32 *), u32 *arg) {
    u32 cmd;
    switch (a) {
        case 1:
            cmd = 1;
            break;
        case 3:
            cmd = 2;
            break;
        case 2:
            cmd = 3;
            break;
        default:
            cmd = 0;
            break;
    }
    if (cmd == 0) {
        return 0xffff;
    }
    return func_02043aa0(cmd, 0, 0, cb, arg);
}

int func_02043bcc(u32 a) {
    u32 r;
    int ret = func_02043b68(a, func_020437ec, &r);
    if (ret != 0) {
        return ret;
    }
    pmWaitFunc();
    return r;
}

int func_02043c00(u32 a, u32 b, void (*cb)(u32, u32 *), u32 *arg) {
    u32 cmd = 0;
    if (a == 0) {
        if (b == 1) {
            cmd = 6;
        }
        if (b == 0) {
            cmd = 7;
        }
    } else if (a == 1) {
        if (b == 1) {
            cmd = 4;
        }
        if (b == 0) {
            cmd = 5;
        }
    } else if (a == 2) {
        if (b == 1) {
            cmd = 8;
        }
        if (b == 0) {
            cmd = 9;
        }
    }
    if (cmd == 0) {
        return 0xffff;
    }
    return func_02043aa0(cmd, 0, 0, cb, arg);
}

int func_02043c88(u32 a, u32 b) {
    u32 r;
    int ret = func_02043c00(a, b, func_020437ec, &r);
    if (ret != 0) {
        return ret;
    }
    pmWaitFunc();
    return r;
}

int func_02043cbc(void (*cb)(u32, u32 *), u32 *arg) {
    void (*old)(void) = pmWaitFunc;
    pmWaitFunc = func_02043790;
    func_02044474();
    pmWaitFunc = old;
    return func_02043aa0(0xe, 0, 0, cb, arg);
}

int func_02043d10(void) {
    u32 r;
    int ret = func_02043cbc(func_020437ec, &r);
    if (ret != 0) {
        return ret;
    }
    pmWaitFunc = func_02043740;
    pmWaitFunc();
    pmWaitFunc = func_02043700;
    return r;
}

int func_02043d5c(void) {
    if (func_02043d10() != 0) {
        do {
            OS_SpinWait(PM_RETRY_WAIT);
        } while (func_02043d10() != 0);
    }
    OS_DisableInterrupts();
    MI_StopAllDma();
    while (1) {
        OS_Halt();
    }
}

int func_02043d98(u32 v) {
    pmWork.mUnk_10 = v;
    return func_02043db0(v);
}

int func_02043db0(u32 v) {
    if (!func_020443cc()) {
        return 0;
    }
    return func_02043b24(0x10, (u16) v, 0);
}

int func_02043de0(u32 v) {
    return func_02043b24(0x11, (u16) v, 0);
}

int func_02043dfc(u32 *a, u32 *b) {
    u16 st;
    int ret = func_02043b24(0xf, 1, &st);
    if (ret != 0) {
        return ret;
    }
    if (a) {
        *a = (st & 8) != 0;
    }
    if (b) {
        *b = (st & 4) != 0;
    }
    return ret;
}

// Sends one word, retrying until the FIFO accepts it.
void func_02043e5c(u32 word) {
    while (PXI_SendWordByFifo(PM_PXI_TAG, word, 0) != 0) {
    }
}

void func_02043e88(u32 flags, u32 a, u32 b) {
    int ok = 0;
    u32 dispA;
    u32 led0;
    u32 e;
    u32 mask;
    u32 led1;
    u32 dispB;
    u32 lcd;
    vu32 tick;
    u32 cur;
    u16 ime;
    u32 t;
    func_02044434(pmWork.preList);
    ime = REG_IME;
    REG_IME = 0;
    e = OS_DisableInterrupts();
    mask = OS_DisableIrqMask(PM_IRQ_ALL);
    OS_SetIrqMask((OS_IsTickAvailable() ? PM_IRQ_VCOUNT : 0) | PM_IRQ_FIFO_RECV);
    OS_RestoreInterrupts(e);
    t = REG_IME;
    REG_IME = 1;
    if (flags & 8) {
        if (*PM_SHARED_WORD_C40 == 2) {
            flags &= ~8;
        }
    }
    if (flags & 0x10) {
        if (func_02047fb4() == 0) {
            flags &= ~0x10;
        }
    }
    dispA = *REG_DISPCNT_A;
    dispB = *REG_DISPCNT_B;
    lcd = func_020443cc();
    while (func_02043dfc(&led0, &led1)) {
        OS_SpinWait(PM_RETRY_WAIT);
    }
    while (func_02043c88(2, 0)) {
        OS_SpinWait(PM_RETRY_WAIT);
    }
    tick = *PM_SHARED_FRAME_COUNT;
    do {
        cur = *PM_SHARED_FRAME_COUNT;
    } while (tick == cur);
    tick = *PM_SHARED_FRAME_COUNT;
    *REG_DISPCNT_A &= ~0x30000;
    *REG_DISPCNT_B &= ~0x10000;
    do {
        cur = *PM_SHARED_FRAME_COUNT;
    } while (tick == cur);
    tick = *PM_SHARED_FRAME_COUNT;
    do {
        cur = *PM_SHARED_FRAME_COUNT;
    } while (tick == cur);
    pmWork.mUnk_0c = 0;
    func_02043a14((u16) (flags | (led0 ? 0x40 : 0) | (led1 ? 0x80 : 0)), (u16) (a | b));
    while (pmWork.mUnk_0c == 0) {
        OS_Halt();
    }
    if (flags & 8) {
        if (REG_IF & PM_IRQ_BIT20) {
            ok = 1;
        }
    }
    if (!ok) {
        if (lcd == 1) {
            while (func_02044194(1, 1, 1, 1) != 1) {
            }
        } else {
            if (func_02043bcc(1)) {
                do {
                    OS_SpinWait(PM_RETRY_WAIT);
                } while (func_02043bcc(1));
            }
        }
        *REG_DISPCNT_A = dispA;
        *REG_DISPCNT_B = dispB;
    }
    OS_SpinWait(0x360000);
    OS_DisableInterrupts();
    OS_SetIrqMask(mask);
    OS_RestoreInterrupts(e);
    t = REG_IME;
    REG_IME = ime;
    if (ok) {
        func_02043d5c();
    }
    func_02044434(pmWork.postList);
}

int func_02044194(int a, int b, int c, int d) {
    switch (a) {
        case 1:
            if (c == 0) {
                if (*PM_SHARED_FRAME_COUNT - pmWork.mUnk_04 <= 7) {
                    return 0;
                }
            }
            if (b) {
                if (d) {
                    if (func_02043bcc(b)) {
                        do {
                            OS_SpinWait(PM_RETRY_WAIT);
                        } while (func_02043bcc(b));
                    }
                } else {
                    if (func_02043b68(b, 0, 0)) {
                        do {
                            OS_SpinWait(PM_RETRY_WAIT);
                        } while (func_02043b68(b, 0, 0));
                    }
                }
            }
            REG_POWCNT |= 1;
            if (func_02043db0(pmWork.mUnk_10)) {
                do {
                    OS_SpinWait(PM_RETRY_WAIT);
                } while (func_02043db0(pmWork.mUnk_10));
            }
            break;
        case 0:
            if (func_02043db0(0)) {
                do {
                    OS_SpinWait(PM_RETRY_WAIT);
                } while (func_02043db0(0));
            }
            if (*PM_SHARED_FRAME_COUNT - pmWork.mUnk_08 <= 2) {
                func_0203f6a4();
                func_0203f6a4();
            }
            REG_POWCNT &= ~1;
            pmWork.mUnk_04 = *PM_SHARED_FRAME_COUNT;
            if (b) {
                if (d) {
                    if (func_02043bcc(b)) {
                        do {
                            OS_SpinWait(PM_RETRY_WAIT);
                        } while (func_02043bcc(b));
                    }
                } else {
                    if (func_02043b68(b, 0, 0)) {
                        do {
                            OS_SpinWait(PM_RETRY_WAIT);
                        } while (func_02043b68(b, 0, 0));
                    }
                }
            }
            break;
    }
    return 1;
}

int func_020443ac(int a) {
    return func_02044194(a == 1 ? a : 0, 0, 0, 1);
}

BOOL func_020443cc(void) {
    return (REG_POWCNT & 1) != 0;
}

// Appends node to the end of the list.
void func_020443e8(PMCallbackInfo **head, PMCallbackInfo *node) {
    PMCallbackInfo *cur, *nx;
    if (head == NULL) {
        return;
    }
    cur = *head;
    if (cur == NULL) {
        node->next = NULL;
        *head = node;
        return;
    }
    nx = cur->next;
    while (nx != NULL) {
        cur = nx;
        nx = nx->next;
    }
    node->next = nx;
    cur->next = node;
}

// Calls every callback in the list.
void func_02044434(PMCallbackInfo *list) {
    if (list == NULL) {
        return;
    }
    do {
        list->func(list->arg);
        list = list->next;
    } while (list != NULL);
}

// Adds node to the callbacks that run after sleep.
void func_0204445c(PMCallbackInfo *node) {
    func_020443e8(&pmWork.postList, node);
}

void func_02044474(void) {
    OS_SpinWait(0x360000);
    if (func_020443cc() == 1) {
        return;
    }
    while (func_02043c88(2, 0)) {
        OS_SpinWait(PM_RETRY_WAIT);
    }
    while (!func_020443ac(1)) {
        OS_SpinWait(10);
    }
}
