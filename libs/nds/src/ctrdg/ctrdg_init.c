#include <nitro/os.h>
#include <nitro/pxi.h>
#include <nitro/svc.h>
#include <nitro/ctrdg.h>

// Defined here: this file's code reaches them through its .bss base, the other cartridge code through their symbols.
CTRDGState data_020cdbe8;
u32 data_020cdbe4;

// No code in the ROM uses it; the link keeps it (force_active.txt).
u8 data_020cdbec[0x14];

void func_02047f3c(void) {
    u32 zero = 0;
    CpuSet(&zero, &data_020cdbe8, 0x5000001);
    data_020cdbe8.id = OS_GetLockID();
}

int func_02047f74(void) {
    if (func_02047fb4() != 0) {
        if (func_02047f9c() == 0) {
            return 1;
        }
    }
    return 0;
}

int func_02047f9c(void) {
    CartInfo *info = (CartInfo *) 0x27ffc30;
    return info->b.f0;
}

int func_02047fb4(void) {
    CartInfo *info = (CartInfo *) 0x27ffc30;
    CTRDGLock lock;
    CTRDGTiming timing;
    u8 *rom = (u8 *) 0x8000000;
    int ok = 1;
    u8 tag;
    if (info->mUnk_00 == 0xffff) {
        return 0;
    }
    if (info->b.f1 == 1) {
        return 0;
    }
    func_0204813c(data_020cdbe8.id, &lock);
    func_020480c0(&timing);
    tag = rom[0xb2];
    if ((tag == 0x96 && info->mUnk_00 != *(u16 *) (rom + 0xbe)) ||
        (tag != 0x96 && info->mUnk_00 != *(u16 *) 0x801fffe) ||
        (info->mUnk_08 != *(u32 *) (rom + 0xac) && info->b.f0)) {
        ok = 0;
        info->b.f1 = 1;
    }
    func_02048108(&timing);
    func_02048198(data_020cdbe8.id, &lock);
    return ok;
}

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
