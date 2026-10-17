// CPSR interrupt-mask, processor-mode and busy-wait helpers, hand-written assembly.

#include <nitro/os.h>

// OS_EnableInterrupts: clears the IRQ-disable bit (0x80) in CPSR; returns the
// previous value of that bit.
asm OSIntrMode OS_EnableInterrupts(void) {
    mrs r0, cpsr
    bic r1, r0, #0x80
    msr cpsr_c, r1
    and r0, r0, #0x80
    bx lr
}

// OS_DisableInterrupts: sets the IRQ-disable bit; returns its previous value.
asm OSIntrMode OS_DisableInterrupts(void) {
    mrs r0, cpsr
    orr r1, r0, #0x80
    msr cpsr_c, r1
    and r0, r0, #0x80
    bx lr
}

// OS_RestoreInterrupts(old): sets the IRQ-disable bit to 'old' (0 or 0x80);
// returns the bit's previous value.
asm OSIntrMode OS_RestoreInterrupts(OSIntrMode old) {
    mrs r1, cpsr
    bic r2, r1, #0x80
    orr r2, r2, r0
    msr cpsr_c, r2
    and r0, r1, #0x80
    bx lr
}

// OS_DisableInterrupts_IrqAndFiq: sets both disable bits (0xc0); returns their
// previous value.
asm OSIntrMode OS_DisableInterrupts_IrqAndFiq(void) {
    mrs r0, cpsr
    orr r1, r0, #0xc0
    msr cpsr_c, r1
    and r0, r0, #0xc0
    bx lr
}

// OS_RestoreInterrupts_IrqAndFiq(old): sets both bits (0xc0 field) to 'old';
// returns the previous value.
asm OSIntrMode OS_RestoreInterrupts_IrqAndFiq(OSIntrMode old) {
    mrs r1, cpsr
    bic r2, r1, #0xc0
    orr r2, r2, r0
    msr cpsr_c, r2
    and r0, r1, #0xc0
    bx lr
}

// OS_GetCpsrIrqBits: returns CPSR & 0x80.
asm u32 OS_GetCpsrIrqBits(void) {
    mrs r0, cpsr
    and r0, r0, #0x80
    bx lr
}

// OS_GetProcMode: returns the mode field of CPSR (CPSR & 0x1f).
asm u32 OS_GetProcMode(void) {
    mrs r0, cpsr
    and r0, r0, #0x1f
    bx lr
}

// OS_SpinWait(n): busy loop; subtracts 4 from n until the subtraction borrows
// (about n/4 iterations).
asm void OS_SpinWait(u32 n) {
L1:
    subs r0, r0, #4
    bhs L1
    bx lr
}
