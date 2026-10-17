// Each function turns the interrupt master enable off around the register
// update and returns the previous register value.

#include <nitro/os.h>

OSIrqMask OS_SetIrqMask(OSIrqMask mask) {
    u16 prev = REG_IME;
    u32 old;
    REG_IME = 0;
    old = REG_IE;
    REG_IE = mask;
    (void) REG_IME;
    REG_IME = prev;
    return old;
}

OSIrqMask OS_EnableIrqMask(OSIrqMask mask) {
    u16 prev = REG_IME;
    u32 old;
    REG_IME = 0;
    old = REG_IE;
    REG_IE = old | mask;
    (void) REG_IME;
    REG_IME = prev;
    return old;
}

OSIrqMask OS_DisableIrqMask(OSIrqMask mask) {
    u16 prev = REG_IME;
    u32 old;
    REG_IME = 0;
    old = REG_IE;
    REG_IE = old & ~mask;
    (void) REG_IME;
    REG_IME = prev;
    return old;
}

OSIrqMask OS_ResetRequestIrqMask(OSIrqMask mask) {
    u16 prev = REG_IME;
    u32 old;
    REG_IME = 0;
    old = REG_IF;
    REG_IF = mask;
    (void) REG_IME;
    REG_IME = prev;
    return old;
}

// Writes the stack-overflow marker words: one just below the interrupt stack's
// top, one at its bottom.
void func_0203d520(void) {
    u32 top = HW_DTCM + 0x3f80;
    *(u32 *) (HW_DTCM + 0x3f7c) = 0xfddb597d;
    *(u32 *) (top - (u32) SDK_IRQ_STACKSIZE) = 0x7bf9dd5b;
}
