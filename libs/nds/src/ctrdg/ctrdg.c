#include <nitro/os.h>
#include <nitro/mi.h>
#include <nitro/pxi.h>
#include <nitro/svc.h>
#include <nitro/ctrdg.h>

extern u32 data_020cdbe4;
extern CTRDGState data_020cdbe8;
extern CTRDGWork data_020cdc00;
extern u32 data_020cdc20[] __attribute__((aligned(32)));
extern u8 data_020cdc24[];
extern u32 data_020cdce0;
extern u32 data_020cddcc[];
extern CTRDGGlobal data_020cddc8;
extern u8 data_020ce1f0[];
extern vu16 data_04000300;

// The cartridge header as read into the buffer, and the block of its fields written to main memory.
typedef struct {
    /* 00 */ u8 mUnk_00[0xac];
    /* ac */ u32 mUnk_ac;
    /* b0 */ u16 mUnk_b0;
    /* b2 */ u8 mUnk_b2[3];
    /* b5 */ u8 mUnk_b5[3];
    /* b8 */ u8 mUnk_b8[6];
    /* be */ u16 mUnk_be;
} CartHeader;

typedef struct {
    /* 00 */ u16 mUnk_00;
    /* 02 */ u8 mUnk_02[3];
    /* 05 */ u8 mUnk_05;
    /* 06 */ u16 mUnk_06;
    /* 08 */ u32 mUnk_08;
} CartInfo;

void func_020480c0(CTRDGTiming *t) {
    t->rom1st = (REG_EXMEMCNT & 0xc) >> 2;
    t->rom2nd = (REG_EXMEMCNT & 0x10) >> 4;
    REG_EXMEMCNT = (REG_EXMEMCNT & ~0xc) | 0xc;
    REG_EXMEMCNT = REG_EXMEMCNT & ~0x10;
}

void func_02048108(CTRDGTiming *t) {
    REG_EXMEMCNT = (t->rom1st << 2) | (REG_EXMEMCNT & ~0xc);
    REG_EXMEMCNT = (t->rom2nd << 4) | (REG_EXMEMCNT & ~0x10);
}

void func_0204813c(u16 id, CTRDGLock *l) {
    for (;;) {
        l->irq = OS_DisableInterrupts();
        l->locked = OS_ReadOwnerOfLockWord((void *) 0x027fffe8) & 0x40;
        if (l->locked) {
            return;
        }
        if (OS_TryLockCartridge(id) == 0) {
            return;
        }
        OS_RestoreInterrupts(l->irq);
        WaitByLoop(1);
    }
}

void func_02048198(u16 id, CTRDGLock *l) {
    if (l->locked == 0) {
        OS_UnLockCartridge(id);
    }
    OS_RestoreInterrupts(l->irq);
}

void func_020481bc(u32 data) {
    while (PXI_SendWordByFifo(0xd, data, 0) != 0) {
        WaitByLoop(1);
    }
}

void func_0204820c(u32 v) {
    u32 e = OS_DisableInterrupts();
    data_020cdbe4 = v;
    if (func_02047f74() == 0) {
        func_0203edb0(0xf000, v ? 0x1000 : 0x5000);
    }
    OS_RestoreInterrupts(e);
}

void func_02048254(void) {
    if (data_020cdc00.init != 0) {
        return;
    }
    data_020cdc00.init = 1;
    func_02047f3c();
    data_020cdc00.mUnk_0c = 0;
    PXI_Init();
    while (!PXI_IsCallbackReady(0xd, 1)) {
    }
    PXI_SetFifoRecvCallback(0xd, func_020484fc);
    func_02048308();
    PXI_SetFifoRecvCallback(0xd, 0);
    PXI_SetFifoRecvCallback(0xd, func_02048528);
    data_020cdc00.cb = 0;
    func_020485ac(&data_020cdce0);
    PXI_SetFifoRecvCallback(0x11, func_02048598);
    func_0204820c(0);
}

void func_02048308(void) {
    CTRDGLock lock;
    CTRDGTiming timing;
    u32 irq;
    u16 ime;
    s32 cart;
    vu16 *reg;
    u16 imeold;
    if (data_020cdc00.mUnk_00 != 0) {
        return;
    }
    reg = &data_04000300;
    data_020cdc00.mUnk_00 = 1;
    if (!(*reg & 1)) {
        return;
    }
    irq = OS_SetIrqMask(0x40000);
    ime = *(vu16 *) 0x04000208;
    *(vu16 *) 0x04000208 = 1;
    func_0204813c(data_020cdbe8.id, &lock);
    cart = (REG_EXMEMCNT & 0x8000) >> 15;
    func_020480c0(&timing);
    REG_EXMEMCNT &= ~0x8000;
    DC_InvalidateRange((u8 *) data_020cdc20 + 0x80, 0x40);
    MI_DmaCopy16(1, (void *) 0x8000080, (u8 *) data_020cdc20 + 0x80, 0x40);
    REG_EXMEMCNT = (REG_EXMEMCNT & ~0x8000) | (cart << 15);
    func_02048108(&timing);
    func_02048198(data_020cdbe8.id, &lock);
    if (*(u8 *) 0x027fff9b == 0 && *(u8 *) 0x027fff9a != 0) {
    } else {
        int i;
        CartHeader *hdr = (CartHeader *) data_020cdc20;
        CartInfo *dst = (CartInfo *) 0x027ffc30;
        dst->mUnk_00 = hdr->mUnk_be;
        i = 0;
        while (i < 3) {
            dst->mUnk_02[i] = hdr->mUnk_b5[i];
            i++;
        }
        dst->mUnk_06 = hdr->mUnk_b0;
        dst->mUnk_08 = hdr->mUnk_ac;
        *(u8 *) 0x027fff9b = func_02047fb4() != 0;
        *(u8 *) 0x027fff9a = 1;
    }
    MIi_CpuCopy32((void *) 0xffff0020, data_020cdc24, 0x9c);
    DC_FlushAll();
    func_020481bc((((u32) data_020cdc20 - 0x02000000) >> 5 << 6) | 1);
    while (data_020cdbe8.flag != 1) {
        WaitByLoop(1);
    }
    imeold = *(vu16 *) 0x04000208;
    *(vu16 *) 0x04000208 = ime;
    OS_SetIrqMask(irq);
}

void func_020484fc(int tag, u32 data, BOOL err) {
    if ((data & 0x3f) == 1) {
        data_020cdbe8.flag = 1;
        return;
    }
    OS_Terminate();
}

void func_02048528(int tag, u32 data, BOOL err) {
    if ((data & 0x3f) == 0x11) {
        u32 r;
        if (data_020cdc00.mUnk_0c != 0) {
            return;
        }
        r = 0;
        if (data_020cdc00.cb) {
            r = data_020cdc00.cb();
        }
        if (r) {
            func_02048584();
        }
        data_020cdc00.mUnk_0c = 1;
        return;
    }
    OS_Terminate();
}

void func_02048584(void) {
    func_020481bc(2);
    OS_Terminate();
}

void func_02048598(int tag, u32 data, BOOL err) {
    data_020cdc00.mUnk_04 = 0;
}

void func_020485ac(u32 *p) {
    u32 e = OS_DisableInterrupts();
    if (data_020cddc8.work == 0) {
        data_020cddc8.work = p;
        func_02048638(p + 0x31);
        func_02048638(data_020cddcc);
        p[0x30] = 0;
        OS_CreateThread((OSThread *) p, (void *) func_0204864c, p, data_020ce1f0, 0x400, 0x14);
        OS_WakeupThreadDirect((OSThread *) p);
    }
    OS_RestoreInterrupts(e);
}

void func_02048638(void *p) {
    MI_CpuFill8(p, 0, 0x24);
}

void func_0204864c(u32 *w) {
    CTRDGTask t;
    u32 e;
    void (*cb)(CTRDGTask *);
    for (;;) {
        MI_CpuFill8(&t, 0, 0x24);
        e = OS_DisableInterrupts();
        while (((vu32 *) w)[0x30] == 0) {
            OS_SleepThread(0);
        }
        t = *(CTRDGTask *) w[0x30];
        OS_RestoreInterrupts(e);
        if (t.func) {
            t.result = ((u32 (*)(CTRDGTask *)) t.func)(&t);
        }
        e = OS_DisableInterrupts();
        cb = t.cb;
        data_020cddc8.mUnk_26 = 0;
        if (cb) {
            cb(&t);
        }
        if (data_020cddc8.work == 0) {
            break;
        }
        w[0x30] = 0;
        OS_RestoreInterrupts(e);
    }
    OS_ExitThread();
}
