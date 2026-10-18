// CPSR interrupt-mask, processor-mode and busy-wait helpers, hand-written assembly.

#include <nitro/os.h>

// OS_EnableInterrupts: clears the IRQ-disable bit (0x80) in CPSR; returns the
// previous value of that bit.
// Not compiled: reads and writes CPSR with mrs/msr, which C has no expression for.
// clang-format off
asm OSIntrMode OS_EnableInterrupts(void) {
    mrs r0, cpsr
    bic r1, r0, #0x80
    msr cpsr_c, r1
    and r0, r0, #0x80
    bx lr
}
// clang-format on

// OS_DisableInterrupts: sets the IRQ-disable bit; returns its previous value.
// Not compiled: reads and writes CPSR with mrs/msr, which C has no expression for.
// clang-format off
asm OSIntrMode OS_DisableInterrupts(void) {
    mrs r0, cpsr
    orr r1, r0, #0x80
    msr cpsr_c, r1
    and r0, r0, #0x80
    bx lr
}
// clang-format on

// OS_RestoreInterrupts(old): sets the IRQ-disable bit to 'old' (0 or 0x80);
// returns the bit's previous value.
// Not compiled: reads and writes CPSR with mrs/msr, which C has no expression for.
// clang-format off
asm OSIntrMode OS_RestoreInterrupts(OSIntrMode old) {
    mrs r1, cpsr
    bic r2, r1, #0x80
    orr r2, r2, r0
    msr cpsr_c, r2
    and r0, r1, #0x80
    bx lr
}
// clang-format on

// OS_DisableInterrupts_IrqAndFiq: sets both disable bits (0xc0); returns their
// previous value.
// Not compiled: reads and writes CPSR with mrs/msr, which C has no expression for.
// clang-format off
asm OSIntrMode OS_DisableInterrupts_IrqAndFiq(void) {
    mrs r0, cpsr
    orr r1, r0, #0xc0
    msr cpsr_c, r1
    and r0, r0, #0xc0
    bx lr
}
// clang-format on

// OS_RestoreInterrupts_IrqAndFiq(old): sets both bits (0xc0 field) to 'old';
// returns the previous value.
// Not compiled: reads and writes CPSR with mrs/msr, which C has no expression for.
// clang-format off
asm OSIntrMode OS_RestoreInterrupts_IrqAndFiq(OSIntrMode old) {
    mrs r1, cpsr
    bic r2, r1, #0xc0
    orr r2, r2, r0
    msr cpsr_c, r2
    and r0, r1, #0xc0
    bx lr
}
// clang-format on

// OS_GetCpsrIrqBits: returns CPSR & 0x80.
// Not compiled: reads CPSR with mrs, which C has no expression for.
// clang-format off
asm u32 OS_GetCpsrIrqBits(void) {
    mrs r0, cpsr
    and r0, r0, #0x80
    bx lr
}
// clang-format on

// OS_GetProcMode: returns the mode field of CPSR (CPSR & 0x1f).
// Not compiled: reads CPSR with mrs, which C has no expression for.
// clang-format off
asm u32 OS_GetProcMode(void) {
    mrs r0, cpsr
    and r0, r0, #0x1f
    bx lr
}
// clang-format on

// OS_SpinWait(n): busy loop; subtracts 4 from n until the subtraction borrows
// (about n/4 iterations).
// Not compiled: the loop is `subs r0, r0, #4; bhs L1`, a branch on the subtract's carry.
// The compiler makes a flag-setting subtract only by folding a following `cmp rd, #0` into it, which leaves
// the branch testing eq, ne, mi or pl (ge and lt become pl and mi, hs becomes always; lo, gt and le are not
// folded), or inside 64-bit subtract, compare and shift sequences, where the next sbc, sbcs or conditional orr
// reads the flags.
// clang-format off
asm void OS_SpinWait(u32 n) {
L1:
    subs r0, r0, #4
    bhs L1
    bx lr
}
// clang-format on
