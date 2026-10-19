#include <nitro/os.h>
#include <nitro/mi.h>
#include <nitro/ctrdg.h>

CTRDGGlobal data_020cddc8;
// Stack of the cartridge thread.
u8 data_020cddf0[0x400];

void func_020485ac(u32 *p) {
    u32 e = OS_DisableInterrupts();
    if (data_020cddc8.work == 0) {
        data_020cddc8.work = p;
        func_02048638(p + 0x31);
        func_02048638(data_020cddc8.mUnk_04);
        p[0x30] = 0;
        OS_CreateThread((OSThread *) p, (void *) func_0204864c, p, data_020cddf0 + 0x400, 0x400, 0x14);
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
            t.result = ((u32(*)(CTRDGTask *)) t.func)(&t);
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
