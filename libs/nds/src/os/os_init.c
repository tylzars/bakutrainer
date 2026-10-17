#include <nitro/os.h>
#include <nitro/card.h>
#include <nitro/ctrdg.h>
#include <nitro/mi.h>
#include <nitro/pm.h>
#include <nitro/pxi.h>

// Waits for the display's vertical counter to read 0, with the interrupt
// master enable (IME) cleared meanwhile and restored afterwards.
asm void OSi_WaitVCount0(void) {
    mov ip, #0x4000000
    ldr r1, [ip, #0x208]
    str ip, [ip, #0x208]
L1:
    ldrh r0, [ip, #6]
    cmp r0, #0
    bne L1
    str r1, [ip, #0x208]
    bx lr
}

void OS_Init(void) {
    OS_InitArena();
    PXI_Init();
    OS_InitLock();
    OS_InitArenaEx();
    func_0203d338();
    func_0203d520();
    OS_InitException();
    func_020400b4();
    OS_InitVAlarm();
    func_0203f7e8();
    OS_InitThread();
    OS_InitReset();
    func_02048254();
    func_02045dc4();
    func_02043828();
    OSi_WaitVCount0();
}
