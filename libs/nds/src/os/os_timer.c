#include <nitro/os.h>

static u16 sTimerReserved;

void OSi_SetTimerReserved(int timerNum) {
    sTimerReserved |= 1 << timerNum;
}

// Timer registers are 4 bytes apart from 0x04000100: counter/reload first, control second.
// Loads the reload value (count up from ~count) and starts the timer with the given control
// bits plus enable (0x80) and IRQ (0x40).
void func_0203f004(int timerNum, u32 count, u32 control) {
    vu16 *timer = (vu16 *) (0x4000000 + timerNum * 4);
    timer[0x80] = ~count;
    timer[0x81] = control | 0xc0;
}

// Stops the timer.
void func_0203f024(int timerNum) {
    vu16 *timer = (vu16 *) (0x4000000 + timerNum * 4);
    timer[0x81] = 0;
}
