#include <nitro/os.h>

// One callback slot: function, "stay enabled" flag, argument.
typedef struct {
    void (*fn)(void *);
    u32 keep;
    void *arg;
} IrqSlot;

// DMA slots 0-3 followed by timer slots 4-7.
IrqSlot data_020ca6a8[8];

// The timer slots start after the four DMA slots.
#define TIMER_SLOTS (&data_020ca6a8[4])

// Interrupt bit number of each slot.
u16 data_020644a8[8] = {8, 9, 10, 11, 3, 4, 5, 6};

#define IRQ_CHECK (*(vu32 *) (HW_DTCM + 0x3ff8))

// Optionally clears irqFlags in the interrupt-check word, then sleeps on a
// thread queue until any of those bits is set there.
void OS_WaitIrq(BOOL clear, OSIrqMask irqFlags) {
    OSIntrMode e = OS_DisableInterrupts();
    if (clear) {
        IRQ_CHECK &= ~irqFlags;
    }
    OS_RestoreInterrupts(e);
    while ((irqFlags & IRQ_CHECK) == 0) {
        OS_SleepThread(&data_027e0060);
    }
}

void func_0203d22c(void) {
}

// Common interrupt entry for slot n. Takes the slot's callback and clears
// it, marks the slot's interrupt bit in the check word, calls the callback
// with its argument, and masks the interrupt again unless the slot is flagged
// to stay enabled.
void func_0203d230(int n) {
    u32 bit = 1 << data_020644a8[n];
    void (*fn)(void *) = data_020ca6a8[n].fn;
    data_020ca6a8[n].fn = 0;
    if (fn != 0) {
        fn(data_020ca6a8[n].arg);
    }
    IRQ_CHECK |= bit;
    if (data_020ca6a8[n].keep == 0) {
        OS_DisableIrqMask(bit);
    }
}

void func_0203d2b8(void) { func_0203d230(0); }
void func_0203d2c8(void) { func_0203d230(1); }
void func_0203d2d8(void) { func_0203d230(2); }
void func_0203d2e8(void) { func_0203d230(3); }
void func_0203d2f8(void) { func_0203d230(4); }
void func_0203d308(void) { func_0203d230(5); }
void func_0203d318(void) { func_0203d230(6); }
void func_0203d328(void) { func_0203d230(7); }

// Zeroes the two words of the interrupt-wait thread queue.
void func_0203d338(void) {
    data_027e0060.tail = 0;
    data_027e0060.head = 0;
}

// For each interrupt bit set in mask (0..21), installs fn: bits 3-6 and 8-11
// go to the callback slots (with keep=1, arg=0), every other bit goes into
// the handler table at the start of fast data memory.
void OS_SetIrqFunction(OSIrqMask mask, void (*function)(void)) {
    int i;
    IrqSlot *s;
    for (i = 0; i < 22; i++, mask >>= 1) {
        if (mask & 1) {
            s = 0;
            if (i >= 8 && i <= 11) {
                s = &data_020ca6a8[i - 8];
            } else if (i >= 3 && i <= 6) {
                s = &data_020ca6a8[i + 1];
            } else {
                ((void (**)(void)) SDK_AUTOLOAD_DTCM_START)[i] = function;
            }
            if (s != 0) {
                s->fn = (void (*)(void *)) function;
                s->arg = 0;
                s->keep = 1;
            }
        }
    }
}

// Fills DMA slot dmaNo, enables the DMA interrupt, and records in keep
// whether it was already enabled.
void OSi_EnterDmaCallback(int dmaNo, void (*callback)(void *), void *arg) {
    u32 bit = 1 << (dmaNo + 8);
    data_020ca6a8[dmaNo].fn = callback;
    data_020ca6a8[dmaNo].arg = arg;
    data_020ca6a8[dmaNo].keep = OS_EnableIrqMask(bit) & bit;
}

// Fills timer slot timerNo, enables the timer interrupt, and marks the slot
// to stay enabled.
void OSi_EnterTimerCallback(int timerNo, void (*callback)(void *), void *arg) {
    TIMER_SLOTS[timerNo].fn = callback;
    TIMER_SLOTS[timerNo].arg = arg;
    OS_EnableIrqMask(1 << (timerNo + 3));
    TIMER_SLOTS[timerNo].keep = 1;
}
