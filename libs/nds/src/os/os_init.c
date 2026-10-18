#include <nitro/os.h>
#include <nitro/card.h>
#include <nitro/ctrdg.h>
#include <nitro/mi.h>
#include <nitro/pm.h>
#include <nitro/pxi.h>

// Waits for the display's vertical counter to read 0, with the interrupt master enable (IME) cleared meanwhile and
// restored afterwards; the base address is written to IME, which clears its enable bit.
void OSi_WaitVCount0(void) {
    vu8 *io = (vu8 *) 0x04000000;
    u32 ime = *(vu32 *) (io + 0x208);
    *(vu32 *) (io + 0x208) = (u32) io;
    while (*(vu16 *) (io + 6) != 0) {
    }
    *(vu32 *) (io + 0x208) = ime;
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
