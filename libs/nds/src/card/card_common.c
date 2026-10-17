#include <nitro/card.h>
#include <nitro/mi.h>
#include <nitro/os.h>
#include <nitro/pxi.h>
#include <nitro/init.h>

#define CARD_ROMCNT (*(vu32 *) 0x40001a4)
#define CARD_ROMCNT_BUSY 0x80000000
#define CARD_ROMCNT_DATA_READY 0x800000
#define CARD_ROMDATA_ADDR 0x4100010
#define CARD_ROMDATA (*(vu32 *) CARD_ROMDATA_ADDR)

u32 data_020cc800 __attribute__((aligned(32)));
CardArg data_020cc820 __attribute__((aligned(32)));
// Bytes of the CardCommon state, accessed through CARD_COMMON.
u8 data_020cc880[sizeof(CardCommon)];
// Stack of the card thread.
static u8 data_020ccaa0[0x400];
// 32 bytes holding the offset added to every ROM read address (CARD_ROM_BASE); the request follows it.
u8 data_020ccea0[0x20];
// Bytes of the ROM read request, 32-byte aligned (the ROM read code keeps its address in a register across calls).
u8 data_020ccec0[sizeof(CardRomReq)] __attribute__((aligned(32)));
CardHeader *data_020644c8 = (CardHeader *) 0x27ffe00;

#define CARD_ROM_BASE (*(u32 *) data_020ccea0)
#define CARD_ROM_REQ ((CardRomReq *) data_020ccec0)
#define CARD_COMMON ((CardCommon *) data_020cc880)

void func_02044c20(void (*task)(CardCommon *)) {
    CardCommon *c = CARD_COMMON;
    OS_SetThreadPriority(&c->thread, c->priority);
    c->curThread = &c->thread;
    c->task = task;
    c->flags |= CF_TASK;
    OS_WakeupThreadDirect(&c->thread);
}

#pragma optimize_for_size on
#pragma dont_inline on
void func_02044c5c(u32 id, u32 type) {
    CardCommon *c = CARD_COMMON;
    u32 e = OS_DisableInterrupts();
    if (c->owner == id) {
        if (c->lockTarget != type) {
            OS_Terminate();
        }
    } else {
        while (c->owner != CARD_OWNER_NONE) {
            OS_SleepThread(&c->lockQueue);
        }
        c->owner = id;
        c->lockTarget = type;
    }
    c->lockCount++;
    c->arg->result = 0;
    OS_RestoreInterrupts(e);
}
#pragma dont_inline reset
#pragma optimize_for_size reset

void func_02044ce0(u32 id, u32 type) {
    CardCommon *c = CARD_COMMON;
    u32 e = OS_DisableInterrupts();
    u32 n;
    if (c->owner != id || c->lockCount == 0) {
        OS_Terminate();
    } else {
        if (c->lockTarget != type) {
            OS_Terminate();
        }
        n = c->lockCount - 1;
        c->lockCount = n;
        if (n == 0) {
            c->owner = CARD_OWNER_NONE;
            c->lockTarget = 0;
            OS_WakeupThread(&c->lockQueue);
        }
    }
    c->arg->result = 0;
    OS_RestoreInterrupts(e);
}

static inline BOOL IsBootType2(void) {
    return *(vu16 *) 0x27ffc40 == 2;
}

void func_02044d6c(void) {
    CardCommon *c = CARD_COMMON;
    c->owner = CARD_OWNER_NONE;
    c->lockCount = 0;
    c->lockTarget = 0;
    c->arg = &data_020cc820;
    MI_CpuClearFast(&data_020cc820, 0x60);
    DC_FlushRange(&data_020cc820, 0x60);
    c->mUnk_118 = -1;
    c->mUnk_11c = -1;
    if (!IsBootType2()) {
        MI_CpuCopy8((void *) 0x27ffe00, (void *) 0x27ffa80, 0x160);
    }
    c->priority = 4;
    c->lockQueue.tail = 0;
    c->lockQueue.head = 0;
    c->queue.tail = 0;
    c->queue.head = 0;
    OS_CreateThread(&c->thread, func_02045e74, 0, &data_020ccaa0[0x400], 0x400, c->priority);
    OS_WakeupThreadDirect(&c->thread);
    PXI_SetFifoRecvCallback(CARD_PXI_TAG, func_02045e40);
    if (IsBootType2()) {
        return;
    }
    func_02044ea4(1);
}

u32 func_02044e7c(void) {
    return data_020cc800;
}

void func_02044e8c(void) {
    if (func_02044e7c() == 0) {
        OS_Terminate();
    }
}

void func_02044ea4(u32 v) {
    data_020cc800 = v;
}

#pragma optimize_for_size on
#pragma dont_inline on
int func_02044eb4(void) {
    CardCommon *c = CARD_COMMON;
    u32 e = OS_DisableInterrupts();
    while (c->flags & CF_BUSY) {
        OS_SleepThread(&c->queue);
    }
    OS_RestoreInterrupts(e);
    return c->arg->result == 0;
}
#pragma dont_inline reset
#pragma optimize_for_size reset

int func_02044f00(void) {
    return (CARD_COMMON->flags & CF_BUSY) == 0;
}

void func_02044f1c(u16 id) {
    func_02044c5c(id, 1);
    OS_LockCard(id);
}

void func_02044f38(u16 id) {
    OS_UnlockCard(id);
    func_02044ce0(id, 1);
}

void func_02044f54(u32 id) {
    func_02044c5c(id, 2);
}

void func_02044f64(u32 id) {
    if (!func_02045740()) {
        func_02045734();
    }
    func_02044ce0(id, 2);
}

void func_02044f8c(s32 id) {
    CardArg *a = CARD_COMMON->arg;
    u32 size;
    u32 type;
    u32 variant;
    MI_CpuFill8(&a->mUnk_18, 0, 0x48);
    a->id = id;
    a->mUnk_58 = 0x203f;
    if (id == 0) {
        return;
    }
    size = 1 << ((id >> 8) & 0xff);
    type = id & 0xff;
    variant = (id >> 16) & 0xff;
    a->mUnk_18 = size;
    a->mUnk_54 = 0xff;
    if (type == 1) {
        switch (size) {
            case 0x200:
                a->mUnk_24 = 0x10;
                a->mUnk_28 = 1;
                a->mUnk_2c = 5;
                a->mUnk_54 = 0xf0;
                break;
            case 0x2000:
                a->mUnk_24 = 0x20;
                a->mUnk_28 = 2;
                a->mUnk_2c = 5;
                a->mUnk_54 = 0;
                break;
            case 0x10000:
                a->mUnk_24 = 0x80;
                a->mUnk_28 = 2;
                a->mUnk_2c = 10;
                a->mUnk_54 = 0;
                break;
            case 0x20000:
                a->mUnk_24 = 0x100;
                a->mUnk_28 = 3;
                a->mUnk_2c = 5;
                a->mUnk_54 = 0;
                break;
            default:
                goto fail;
        }
        a->mUnk_1c = a->mUnk_24;
        a->mUnk_58 |= 0x40;
        a->mUnk_58 |= 0x4300;
        return;
    } else if (type == 2) {
        switch (size) {
            case 0x40000:
            case 0x80000:
            case 0x100000:
                a->mUnk_30 = 0x19;
                a->mUnk_34 = 0x12c;
                a->mUnk_50 = 0x12c;
                a->mUnk_40 = 0x1388;
                a->mUnk_58 |= 0x480;
                break;
            case 0x200000:
                a->mUnk_30 = 0x17;
                a->mUnk_34 = 0x12c;
                a->mUnk_40 = 0x1f4;
                a->mUnk_44 = 0x1388;
                a->mUnk_38 = 10000;
                a->mUnk_3c = 0xea60;
                a->mUnk_54 = 0;
                a->mUnk_58 |= 0x80;
                a->mUnk_58 |= 0x5400;
                break;
            case 0x400000:
                a->mUnk_40 = 0x258;
                a->mUnk_44 = 0xbb8;
                a->mUnk_48 = 0x46;
                a->mUnk_4c = 0x96;
                a->mUnk_38 = 0x59d8;
                a->mUnk_3c = 0xc3500;
                a->mUnk_54 = 0;
                a->mUnk_20 = 0x1000;
                a->mUnk_58 |= 0x1000;
                a->mUnk_58 |= 0xc000;
                break;
            case 0x800000:
                if (variant == 0) {
                    a->mUnk_40 = 0x3e8;
                    a->mUnk_44 = 0xbb8;
                    a->mUnk_38 = 0x109a0;
                    a->mUnk_3c = 0x27100;
                    a->mUnk_54 = 0;
                    a->mUnk_58 |= 0x1000;
                    a->mUnk_58 |= 0x4000;
                } else if (variant == 1) {
                    a->mUnk_40 = 0x3e8;
                    a->mUnk_44 = 0xbb8;
                    a->mUnk_38 = 0x109a0;
                    a->mUnk_3c = 0x27100;
                    a->mUnk_54 = 0x84;
                    a->mUnk_58 |= 0x1000;
                    a->mUnk_58 |= 0x4000;
                }
                break;
            default:
                goto fail;
        }
        a->mUnk_1c = 0x10000;
        a->mUnk_24 = 0x100;
        a->mUnk_28 = 3;
        a->mUnk_2c = 5;
        a->mUnk_58 |= 0xb40;
        return;
    } else if (type == 3) {
        if (size == 0x2000 || size == 0x8000) {
            a->mUnk_24 = size;
            a->mUnk_1c = size;
            a->mUnk_28 = 2;
            a->mUnk_54 = 0;
            a->mUnk_58 |= 0x40;
            a->mUnk_58 |= 0x4300;
            return;
        }
    }
fail:
    a->mUnk_18 = a->id = 0;
    CARD_COMMON->arg->result = 3;
}

void func_02045310(CardCommon *c) {
    u32 mode;
    u32 chunk;
    u32 buf;
    u32 cmd;
    u32 len;
    u32 retry;
    u32 e;
    void *cbarg;
    void (*fn)(void *);
    cmd = c->cmd;
    mode = c->mode;
    retry = c->retry;
    chunk = 0x100;
    func_02000b9c((void *) 0x2000bec);
    if (cmd == 0xb) {
        chunk = func_020455e8();
    } else if (cmd == 0xf) {
        chunk = CARD_COMMON->arg->mUnk_20;
    }
    buf = (u32) c->buf;
    do {
        len = c->len;
        if (chunk < len) {
            len = chunk;
        }
        c->arg->mUnk_14 = len;
        if (c->flags & CF_CANCEL) {
            c->flags &= ~CF_CANCEL;
            c->arg->result = 7;
            break;
        }
        switch (mode) {
            case 0:
                DC_InvalidateRange((void *) buf, len);
                c->arg->mUnk_0c = c->src;
                c->arg->mUnk_10 = buf;
                break;
            case 1:
            case 2:
                MI_CpuCopy8((void *) c->src, (void *) buf, len);
                DC_FlushRange((void *) buf, len);
                DC_WaitWriteBufferEmpty();
                c->arg->mUnk_0c = buf;
                c->arg->mUnk_10 = c->dst;
                break;
            case 3:
                c->arg->mUnk_0c = c->src;
                c->arg->mUnk_10 = c->dst;
                break;
        }
        if (!func_02045ec4(c, cmd, retry)) {
            break;
        }
        if (mode == 2) {
            if (!func_02045ec4(c, 9, 1)) {
                break;
            }
        } else if (mode == 0) {
            MI_CpuCopy8((void *) buf, (void *) c->dst, len);
        }
        c->src += len;
        c->dst += len;
        c->len -= len;
    } while (c->len != 0);
    fn = c->callback;
    cbarg = c->cbArg;
    e = OS_DisableInterrupts();
    c->flags &= ~(CF_BUSY | CF_TASK | CF_CANCEL);
    OS_WakeupThread(&c->queue);
    if (c->flags & CF_TASKWAIT) {
        OS_WakeupThreadDirect(&c->thread);
    }
    OS_RestoreInterrupts(e);
    if (fn) {
        fn(cbarg);
    }
}

int func_02045500(u32 src, void *dst, u32 len, void (*cb)(void *), void *cbarg, int async, u32 cmd, u32 retry,
                  u32 mode) {
    CardCommon *c = CARD_COMMON;
    u32 e;
    func_02000b9c((void *) 0x2000bec);
    e = OS_DisableInterrupts();
    while (c->flags & CF_BUSY) {
        OS_SleepThread(&c->queue);
    }
    c->flags |= CF_BUSY;
    c->callback = cb;
    c->cbArg = cbarg;
    OS_RestoreInterrupts(e);
    c->src = src;
    c->dst = (u32) dst;
    c->len = len;
    c->cmd = cmd;
    c->retry = retry;
    c->mode = mode;
    if (async) {
        func_02044c20(func_02045310);
        return 1;
    }
    CARD_COMMON->curThread = data_020ca730.current;
    func_02045310(c);
    return c->arg->result == 0;
}

u32 func_020455e8(void) {
    return CARD_COMMON->arg->mUnk_1c;
}

int func_020455fc(u32 id) {
    CardCommon *c = CARD_COMMON;
    u32 e;
    u32 e2;
    void *arg;
    void (*fn)(void *);
    func_02000b9c((void *) 0x2000bec);
    if (id == 0) {
        OS_Terminate();
    }
    func_02044e8c();
    e = OS_DisableInterrupts();
    while (c->flags & CF_BUSY) {
        OS_SleepThread(&c->queue);
    }
    c->flags |= CF_BUSY;
    c->callback = NULL;
    c->cbArg = NULL;
    OS_RestoreInterrupts(e);
    func_02044f8c(id);
    CARD_COMMON->curThread = data_020ca730.current;
    func_02045ec4(c, 2, 1);
    c->arg->mUnk_0c = 0;
    c->arg->mUnk_10 = (u32) c->buf;
    c->arg->mUnk_14 = 1;
    func_02045ec4(c, 6, 1);
    fn = c->callback;
    arg = c->cbArg;
    e2 = OS_DisableInterrupts();
    c->flags &= ~(CF_BUSY | CF_TASK | CF_CANCEL);
    OS_WakeupThread(&c->queue);
    if (c->flags & CF_TASKWAIT) {
        OS_WakeupThreadDirect(&c->thread);
    }
    OS_RestoreInterrupts(e2);
    if (fn) {
        fn(arg);
    }
    return c->arg->result == 0;
}

int func_02045734(void) {
    return func_02044eb4();
}

int func_02045740(void) {
    return func_02044f00();
}

int func_0204574c(CardRomReq *r) {
    CardCommon *c = CARD_COMMON;
    u32 page = *(vu32 *) &c->src & ~0x1ff;
    if (page == r->page) {
        u32 off = c->src - page;
        u32 n = 0x200 - off;
        if (n > c->len) {
            n = c->len;
        }
        MI_CpuCopy8(r->cache + off, (void *) c->dst, n);
        c->src += n;
        c->dst += n;
        c->len -= n;
    }
    return c->len != 0;
}

void func_020457d8(u32 a, u32 b) {
    volatile u8 *p;
    while (CARD_ROMCNT & CARD_ROMCNT_BUSY) {
    }
    p = (volatile u8 *) 0x40001a1;
    p[0] = 0xc0;
    p[7] = a >> 24;
    p[8] = a >> 16;
    p[9] = a >> 8;
    p[10] = a;
    p[11] = b >> 24;
    p[12] = b >> 16;
    p[13] = b >> 8;
    p[14] = b;
}

void func_02045838(void) {
    CardCommon *c = CARD_COMMON;
    CardRomReq *r = CARD_ROM_REQ;
    func_02040040(c->dma, (const void *) CARD_ROMDATA_ADDR, (void *) c->dst, 0x200);
    func_020457d8((c->src >> 8) | 0xb7000000, c->src << 24);
    CARD_ROMCNT = r->ctrl;
}

void func_0204588c(void) {
    CardCommon *c;
    CardCommon *p;
    u32 more;
    MI_StopDma(CARD_COMMON->dma);
    p = CARD_COMMON;
    p->src += 0x200;
    p->dst += 0x200;
    p->len -= 0x200;
    more = p->len != 0;
    if (!more) {
        void (*fn)(void *);
        void *arg;
        u32 e;
        OS_DisableIrqMask(CARD_IRQ_MASK);
        OS_ResetRequestIrqMask(CARD_IRQ_MASK);
        c = CARD_COMMON;
        func_02046134(func_02045bd8());
        c->arg->result = 0;
        fn = c->callback;
        arg = c->cbArg;
        e = OS_DisableInterrupts();
        c->flags &= ~(CF_BUSY | CF_TASK | CF_CANCEL);
        OS_WakeupThread(&c->queue);
        if (c->flags & CF_TASKWAIT) {
            OS_WakeupThreadDirect(&c->thread);
        }
        OS_RestoreInterrupts(e);
        if (fn) {
            fn(arg);
        }
    } else {
        func_02045838();
    }
}

int func_0204595c(CardRomReq *r) {
    CardCommon *c = CARD_COMMON;
    u32 len = c->len;
    int ok = 0;
    int pageAligned = 0;
    int noTcm = 0;
    u32 dst = c->dst;
    u32 mis = dst & 0x1f;
    u32 dmaOk = 0;
    if (mis == 0) {
        dmaOk = c->dma <= 3;
    }
    if (dmaOk) {
        u32 dtcm = func_0203ed7c();
        u32 tcm = 1;
        u32 inItcm = (dst + len > 0x1ff8000 && dst < 0x2000000);
        u32 end = dst + len;
        if (inItcm == 0) {
            if (dtcm >= end || dtcm + 0x4000 <= dst) {
                tcm = 0;
            }
        }
        if (tcm == 0) {
            noTcm = 1;
        }
    }
    if (noTcm) {
        if (((c->src | len) & 0x1ff) == 0) {
            pageAligned = 1;
        }
    }
    if (pageAligned && len != 0) {
        ok = 1;
    }
    r->ctrl = (data_020644c8->romctrl & ~0x7000000) | 0xa1000000;
    if (ok) {
        u32 e;
        e = OS_DisableInterrupts();
        if (len < c->mUnk_118) {
            IC_InvalidateRange((void *) dst, len);
        } else {
            IC_InvalidateAll();
        }
        if (len < c->mUnk_11c) {
            if (mis != 0) {
                dst -= mis;
                DC_StoreRange((void *) dst, 0x20);
                DC_StoreRange((void *) (dst + len), 0x20);
                len += 0x20;
            }
            DC_InvalidateRange((void *) dst, len);
            DC_WaitWriteBufferEmpty();
        } else {
            DC_FlushAll();
        }
        OS_SetIrqFunction(CARD_IRQ_MASK, func_0204588c);
        OS_ResetRequestIrqMask(CARD_IRQ_MASK);
        OS_EnableIrqMask(CARD_IRQ_MASK);
        OS_RestoreInterrupts(e);
        func_02045838();
    }
    return ok;
}

void func_02045ae8(CardRomReq *r) {
    CardCommon *c = CARD_COMMON;
    u32 t;
    for (;;) {
        u32 page = c->src & ~0x1ff;
        u32 *dst;
        u32 st;
        u32 i;
        u32 n;
        if (page != c->src || ((u32) (dst = (u32 *) c->dst) & 3) || c->len < 0x200) {
            r->page = page;
            dst = (u32 *) r->cache;
        }
        func_020457d8((page >> 8) | 0xb7000000, page << 24);
        i = 0;
        CARD_ROMCNT = r->ctrl;
        n = 0;
        do {
            st = CARD_ROMCNT;
            if (st & CARD_ROMCNT_DATA_READY) {
                u32 w = CARD_ROMDATA;
                if (n < 0x200) {
                    dst[i] = w;
                    n += 4;
                    i++;
                }
            }
        } while (st & CARD_ROMCNT_BUSY);
        if ((u32) dst == c->dst) {
            t = CARD_COMMON->src + 0x200;
            CARD_COMMON->src = t;
            t = CARD_COMMON->dst + 0x200;
            CARD_COMMON->dst = t;
            t = CARD_COMMON->len - 0x200;
            CARD_COMMON->len = t;
            if (t == 0) {
                break;
            }
        } else {
            if (!func_0204574c(r)) {
                return;
            }
        }
    }
}

u32 func_02045bd8(void) {
    u32 ctrl;
    func_020457d8(0xb8000000, 0);
    ctrl = (data_020644c8->romctrl & ~0x7000000) | 0xa7000000;
    CARD_ROMCNT = ctrl & ~0x1fff;
    while (!(CARD_ROMCNT & CARD_ROMCNT_DATA_READY)) {
    }
    return CARD_ROMDATA;
}

void func_02045c34(CardCommon *unused) {
    CardCommon *c;
    CardRomReq *r = CARD_ROM_REQ;
    void (*fn)(void *);
    void *arg;
    u32 e;
    if (func_0204574c(r)) {
        r->func(r);
    }
    c = CARD_COMMON;
    func_02046134(func_02045bd8());
    c->arg->result = 0;
    fn = c->callback;
    arg = c->cbArg;
    e = OS_DisableInterrupts();
    c->flags &= ~(CF_BUSY | CF_TASK | CF_CANCEL);
    OS_WakeupThread(&c->queue);
    if (c->flags & CF_TASKWAIT) {
        OS_WakeupThreadDirect(&c->thread);
    }
    OS_RestoreInterrupts(e);
    if (fn) {
        fn(arg);
    }
}

#pragma optimize_for_size on
void func_02045ccc(u32 dma, u32 src, void *dst, u32 len, void (*cb)(void *), void *cbarg, int async) {
    CardRomReq *r = CARD_ROM_REQ;
    CardCommon *c = CARD_COMMON;
    u32 e;
    func_02044e8c();
    e = OS_DisableInterrupts();
    while (c->flags & CF_BUSY) {
        OS_SleepThread(&c->queue);
    }
    c->flags |= CF_BUSY;
    c->callback = cb;
    c->cbArg = cbarg;
    OS_RestoreInterrupts(e);
    c->dma = dma;
    c->dst = (u32) dst;
    c->src = src + CARD_ROM_BASE;
    c->len = len;
    if (dma <= 3) {
        MI_StopDma(dma);
    }
    if (func_0204595c(r)) {
        if (async) {
            return;
        }
        func_02045e28();
        return;
    }
    if (async) {
        func_02044c20(func_02045c34);
        return;
    }
    c->curThread = data_020ca730.current;
    func_02045c34(c);
}

#pragma optimize_for_size reset

void func_02045dc4(void) {
    CardCommon *c = CARD_COMMON;
    if (c->flags != 0) {
        return;
    }
    c->flags = CF_READY;
    c->len = 0;
    c->dst = 0;
    c->src = 0;
    c->dma = -1;
    c->callback = NULL;
    c->cbArg = NULL;
    CARD_ROM_BASE = 0;
    func_02044d6c();
    ((CardRomReq *) (data_020ccea0 + sizeof data_020ccea0))->func = func_02045e34();
    func_02046018();
}

int func_02045e28(void) {
    return func_02044eb4();
}

void (*func_02045e34(void))(CardRomReq *) {
    return func_02045ae8;
}

void func_02045e40(int tag, u32 data, BOOL err) {
    CardCommon *c;
    if (tag != CARD_PXI_TAG) {
        return;
    }
    if (err == 0) {
        return;
    }
    c = CARD_COMMON;
    c->flags &= ~CF_PXIWAIT;
    OS_WakeupThreadDirect(c->curThread);
}

void func_02045e74(void *arg) {
    CardCommon *c = CARD_COMMON;
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
