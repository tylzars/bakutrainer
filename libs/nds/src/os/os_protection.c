// Protection-unit and CP15 control helpers, hand-written assembly.

#include <nitro/os.h>

// Returns the data TCM region register (c9,c1,0) with the low 12 bits cleared (mask 0xfffff000, kept in a literal
// word).
// Not compiled: reads the register with a CP15 mrc instruction, which C has no expression for.
// clang-format off
asm u32 func_0203ed7c(void) {
    mrc p15, 0, r0, c9, c1, 0
    ldr r1, =0xfffff000
    and r0, r0, r1
    bx lr
}
// clang-format on

// Sets bit 0 of the control register (c1,c0,0).
// Not compiled: CP15 mrc/mcr instructions, which C has no expression for.
// clang-format off
asm void func_0203ed90(void) {
    mrc p15, 0, r0, c1, c0, 0
    orr r0, r0, #1
    mcr p15, 0, r0, c1, c0, 0
    bx lr
}
// clang-format on

// Clears bit 0 of the control register (c1,c0,0).
// Not compiled: CP15 mrc/mcr instructions, which C has no expression for.
// clang-format off
asm void func_0203eda0(void) {
    mrc p15, 0, r0, c1, c0, 0
    bic r0, r0, #1
    mcr p15, 0, r0, c1, c0, 0
    bx lr
}
// clang-format on

// Sets register c5,c0,2 to (value & ~clear) | set.
// Not compiled: CP15 mrc/mcr instructions, which C has no expression for.
// clang-format off
asm void func_0203edb0(u32 clear, u32 set) {
    mrc p15, 0, r2, c5, c0, 2
    bic r2, r2, r0
    orr r2, r2, r1
    mcr p15, 0, r2, c5, c0, 2
    bx lr
}
// clang-format on

// Writes value to region register c6,c1,0.
// Not compiled: a CP15 mcr instruction, which C has no expression for.
// clang-format off
asm void OS_SetProtectionRegion1(u32 value) {
    mcr p15, 0, r0, c6, c1, 0
    bx lr
}
// clang-format on

// Writes value to region register c6,c2,0.
// Not compiled: a CP15 mcr instruction, which C has no expression for.
// clang-format off
asm void OS_SetProtectionRegion2(u32 value) {
    mcr p15, 0, r0, c6, c2, 0
    bx lr
}
// clang-format on
