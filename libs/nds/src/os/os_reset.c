#include <nitro/os.h>
#include <nitro/pxi.h>
#include <nitro/svc.h>

static struct {
    u16 state;
    u16 initialized;
} sResetState;

void func_0203f6a4(void) {
    WaitByLoop(1);
    OS_WaitIrq(1, 1);
}

void OS_InitReset(void) {
    if (sResetState.initialized != 0) {
        return;
    }
    sResetState.initialized = 1;
    PXI_Init();
    while (!PXI_IsCallbackReady(12, 1)) {
    }
    PXI_SetFifoRecvCallback(12, ResetFifoReceive);
}

void ResetFifoReceive(int tag, u32 data, BOOL err) {
    if ((((data & 0x7f00) << 8) >> 16) == 0x10) {
        sResetState.state = 1;
    } else {
        OS_Terminate();
    }
}
