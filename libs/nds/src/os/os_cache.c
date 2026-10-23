// Data and instruction cache maintenance, hand-written assembly.

#include <nitro/os.h>

// Cleans (writes back without invalidating) every data cache line by set/way: four segments (top two bits of the
// index, step 0x40000000) times 32 lines (step 0x20 up to 0x400).
// Not compiled: the clean is a CP15 mcr instruction, which C has no expression for.
// clang-format off
asm void func_0203e4a4(void) {
    mov r1, #0
L1:                             // outer: next segment
    mov r0, #0
L2:                             // inner: next line
    orr r2, r1, r0
    mcr p15, 0, r2, c7, c10, 2
    add r0, r0, #0x20
    cmp r0, #0x400
    blt L2
    add r1, r1, #0x40000000
    cmp r1, #0
    bne L1
    bx lr
}
// clang-format on

// For every data cache line by set/way (same walk as above), drains the write buffer, then cleans and invalidates
// the line.
// Not compiled: the drain and the clean and invalidate are CP15 mcr instructions, which C has no expression
// for.
// clang-format off
asm void DC_FlushAll(void) {
    mov ip, #0
    mov r1, #0
L1:
    mov r0, #0
L2:
    orr r2, r1, r0
    mcr p15, 0, ip, c7, c10, 4  // drain write buffer
    mcr p15, 0, r2, c7, c14, 2  // clean + invalidate line by set/way
    add r0, r0, #0x20
    cmp r0, #0x400
    blt L2
    add r1, r1, #0x40000000
    cmp r1, #0
    bne L1
    bx lr
}
// clang-format on

// Invalidates the data cache lines covering [addr, addr+size), starting from addr rounded down to a 32-byte line.
// Not compiled: the invalidate is a CP15 mcr instruction, which C has no expression for.
// clang-format off
asm void DC_InvalidateRange(void *addr, u32 size) {
    add r1, r1, r0              // end address
    bic r0, r0, #0x1f
L1:
    mcr p15, 0, r0, c7, c6, 1
    add r0, r0, #0x20
    cmp r0, r1
    blt L1
    bx lr
}
// clang-format on

// Cleans (writes back) the data cache lines covering [addr, addr+size).
// Not compiled: the clean is a CP15 mcr instruction, which C has no expression for.
// clang-format off
asm void DC_StoreRange(void *addr, u32 size) {
    add r1, r1, r0
    bic r0, r0, #0x1f
L1:
    mcr p15, 0, r0, c7, c10, 1
    add r0, r0, #0x20
    cmp r0, r1
    blt L1
    bx lr
}
// clang-format on

// For each data cache line covering [addr, addr+size), drains the write buffer, then cleans and invalidates the
// line.
// Not compiled: the drain and the clean and invalidate are CP15 mcr instructions, which C has no expression
// for.
// clang-format off
asm void DC_FlushRange(void *addr, u32 size) {
    mov ip, #0
    add r1, r1, r0              // end address
    bic r0, r0, #0x1f
L1:
    mcr p15, 0, ip, c7, c10, 4  // drain write buffer
    mcr p15, 0, r0, c7, c14, 1  // clean + invalidate line
    add r0, r0, #0x20
    cmp r0, r1
    blt L1
    bx lr
}
// clang-format on

// Drains the write buffer.
// Not compiled: the drain is a CP15 mcr instruction, which C has no expression for.
// clang-format off
asm void DC_WaitWriteBufferEmpty(void) {
    mov r0, #0
    mcr p15, 0, r0, c7, c10, 4
    bx lr
}
// clang-format on

// Invalidates the whole instruction cache.
// Not compiled: the invalidate is a CP15 mcr instruction, which C has no expression for.
// clang-format off
asm void IC_InvalidateAll(void) {
    mov r0, #0
    mcr p15, 0, r0, c7, c5, 0
    bx lr
}
// clang-format on

// Invalidates the instruction cache lines covering [addr, addr+size).
// Not compiled: the invalidate is a CP15 mcr instruction, which C has no expression for.
// clang-format off
asm void IC_InvalidateRange(void *addr, u32 size) {
    add r1, r1, r0
    bic r0, r0, #0x1f
L1:
    mcr p15, 0, r0, c7, c5, 1
    add r0, r0, #0x20
    cmp r0, r1
    blt L1
    bx lr
}
// clang-format on
