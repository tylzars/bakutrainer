#include <nitro/card.h>
#include <nitro/mi.h>
#include <nitro/os.h>
#include <nitro/init.h>

#define CARD_ROMCNT (*(vu32 *) 0x40001a4)
#define CARD_ROMCNT_BUSY 0x80000000
#define CARD_ROMCNT_DATA_READY 0x800000
#define CARD_ROMDATA_ADDR 0x4100010
#define CARD_ROMDATA (*(vu32 *) CARD_ROMDATA_ADDR)

// 32 bytes holding the offset added to every ROM read address (CARD_ROM_BASE); the request follows it.
u8 data_020ccea0[0x20];
// Bytes of the ROM read request, 32-byte aligned (the ROM read code keeps its address in a register across calls).
u8 data_020ccec0[sizeof(CardRomReq)] __attribute__((aligned(32)));
CardHeader *data_020644c8 = (CardHeader *) 0x27ffe00;

#define CARD_ROM_BASE (*(u32 *) data_020ccea0)
#define CARD_ROM_REQ ((CardRomReq *) data_020ccec0)

int func_0204574c(CardRomReq *r) {
    CardCommon *c = (CardCommon *) data_020cc880;
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
    CardCommon *c = (CardCommon *) data_020cc880;
    CardRomReq *r = CARD_ROM_REQ;
    func_02040040(c->dma, (const void *) CARD_ROMDATA_ADDR, (void *) c->dst, 0x200);
    func_020457d8((c->src >> 8) | 0xb7000000, c->src << 24);
    CARD_ROMCNT = r->ctrl;
}

void func_0204588c(void) {
    CardCommon *c;
    CardCommon *p;
    u32 more;
    MI_StopDma(((CardCommon *) data_020cc880)->dma);
    p = (CardCommon *) data_020cc880;
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
        c = (CardCommon *) data_020cc880;
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
    CardCommon *c = (CardCommon *) data_020cc880;
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
    CardCommon *c = (CardCommon *) data_020cc880;
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
            t = ((CardCommon *) data_020cc880)->src + 0x200;
            ((CardCommon *) data_020cc880)->src = t;
            t = ((CardCommon *) data_020cc880)->dst + 0x200;
            ((CardCommon *) data_020cc880)->dst = t;
            t = ((CardCommon *) data_020cc880)->len - 0x200;
            ((CardCommon *) data_020cc880)->len = t;
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
    c = (CardCommon *) data_020cc880;
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
    CardCommon *c = (CardCommon *) data_020cc880;
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
    CardCommon *c = (CardCommon *) data_020cc880;
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
