#include <nitro/gx.h>

// Copies 12 words from src, three at a time, to dst (dst, dst + 4, dst + 8 each time; dst is not advanced, so the
// same three words of a port window are written four times).
// Not compiled: loads with `ldmia r0!, {r2, r3, ip}` (base written back). The compiler writes back an ldm/stm base
// only in push/pop on sp and in struct copies and fills, whose registers are always r0-r3 (r0-r1 for a two-word
// fill); the pass that merges loads or stores into one ldm/stm never adds writeback.
// clang-format off
asm void func_0203d178(const void *src, volatile void *dst) {
    // four times: three words from src (advancing) to dst (fixed)
    ldmia r0!, {r2, r3, ip}
    stmia r1, {r2, r3, ip}
    ldmia r0!, {r2, r3, ip}
    stmia r1, {r2, r3, ip}
    ldmia r0!, {r2, r3, ip}
    stmia r1, {r2, r3, ip}
    ldmia r0!, {r2, r3, ip}
    stmia r1, {r2, r3, ip}
    bx lr
}
// clang-format on

// Copies 16 words from src, eight at a time, to dst (dst to dst + 0x1c each time; dst is not advanced, so the same
// eight words of a port window are written twice).
// Not compiled: saves five registers (r4-r8) and has no stack frame. A compiled function adds r3 to its save list
// whenever the saved registers and its frame come to an odd number of words, so it would save {r3-r8}.
// clang-format off
asm void func_0203d19c(const void *src, volatile void *dst) {
    stmfd sp!, {r4, r5, r6, r7, r8}
    ldmia r0!, {r2, r3, r4, r5, r6, r7, r8, ip}
    stmia r1, {r2, r3, r4, r5, r6, r7, r8, ip}
    ldmia r0!, {r2, r3, r4, r5, r6, r7, r8, ip}
    stmia r1, {r2, r3, r4, r5, r6, r7, r8, ip}
    ldmfd sp!, {r4, r5, r6, r7, r8}
    bx lr
}
// clang-format on
