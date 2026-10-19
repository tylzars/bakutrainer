#include <nitro/os.h>
#include <nitro/card.h>
#include <nitro/ctrdg.h>
#include <nitro/mi.h>
#include <nitro/pm.h>
#include <nitro/pxi.h>

// Waits for the display's vertical counter to read 0, with the interrupt master enable (IME) cleared meanwhile and
// restored afterwards; the base address is written to IME, which clears its enable bit.
// Not compiled: the 0x04000000 base is set once and kept in ip, with r2 and r3 unused. The compiler gives a value
// the lowest-numbered free register of r0-r3 and ip, so ip only when values live at the same time hold all of
// r0-r3; here only the saved IME word (r1) and the loaded counter (r0) overlap the base. Compiled C also sets the
// constant again before the loop, which becomes `mov r1, r0` after colouring.
// clang-format off
asm void OSi_WaitVCount0(void) {
    mov ip, #0x4000000
    ldr r1, [ip, #0x208]    // save IME
    str ip, [ip, #0x208]    // bit 0 clear: interrupts off
L1:
    ldrh r0, [ip, #6]       // VCOUNT
    cmp r0, #0
    bne L1
    str r1, [ip, #0x208]    // restore IME
    bx lr
}
// clang-format on

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
