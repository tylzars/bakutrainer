#include <nitro/os.h>

static struct {
    u16 initialized;
    int needReset;
    volatile OSTick counter;
} sTickState;

void OS_InitTick(void) {
    if (sTickState.initialized) {
        return;
    }
    sTickState.initialized = TRUE;
    OSi_SetTimerReserved(0);
    sTickState.counter = 0;
    *(vu16 *) 0x4000102 = 0;
    *(vu16 *) 0x4000100 = 0;
    *(vu16 *) 0x4000102 = 0xc1;
    OS_SetIrqFunction(8, OSi_CountUpTick);
    OS_EnableIrqMask(8);
    sTickState.needReset = FALSE;
}

BOOL OS_IsTickAvailable(void) {
    return sTickState.initialized;
}

void OSi_CountUpTick(void) {
    sTickState.counter++;
    if (sTickState.needReset) {
        *(vu16 *) 0x4000102 = 0;
        *(vu16 *) 0x4000100 = 0;
        *(vu16 *) 0x4000102 = 0xc1;
        sTickState.needReset = FALSE;
    }
    OSi_EnterTimerCallback(0, (void (*)(void *)) OSi_CountUpTick, NULL);
}

OSTick OS_GetTick(void) {
    OSIntrMode e = OS_DisableInterrupts();
    volatile u16 countL = *(vu16 *) 0x4000100;
    volatile u64 countH = sTickState.counter & 0x0000ffffffffffffULL;
    if ((*(vu32 *) 0x4000214 & 8) && !(countL & 0x8000)) {
        countH++;
    }
    OS_RestoreInterrupts(e);
    return (countH << 16) | countL;
}
