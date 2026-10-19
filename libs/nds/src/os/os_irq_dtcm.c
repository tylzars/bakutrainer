#include <nitro/os.h>

// Interrupt handler table at the start of data TCM, one entry per interrupt bit (0-21); the dispatcher in ITCM
// jumps through it and OS_SetIrqFunction fills it. The timer bits (3-6) start on the timer callback slots, the DMA
// bits (8-11) on the DMA callback slots, every other bit on the empty handler.
void (*data_027e0000[22])(void) = {
    func_0203d22c, func_0203d22c, func_0203d22c, func_0203d2f8, func_0203d308, func_0203d318,
    func_0203d328, func_0203d22c, func_0203d2b8, func_0203d2c8, func_0203d2d8, func_0203d2e8,
    func_0203d22c, func_0203d22c, func_0203d22c, func_0203d22c, func_0203d22c, func_0203d22c,
    func_0203d22c, func_0203d22c, func_0203d22c, func_0203d22c,
};

// Thread queue OS_WaitIrq sleeps on; the interrupt return path in ITCM wakes it.
OSThreadQueue data_027e0060;
