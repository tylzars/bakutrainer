#include <nitro/gx.h>

// Copies 12 words from src, three at a time, to the single address dst
// (nothing advances dst).
asm void func_0203d178(const void *src, volatile void *dst) {
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

// Copies 16 words from src, eight at a time, to the single address dst
// (nothing advances dst).
// Not compiled: saves five registers (r4-r8) and has no stack frame. A compiled function adds r3 to its
// save list whenever the saved registers and its frame come to an odd number of words, so it would save
// {r3-r8}.
asm void func_0203d19c(const void *src, volatile void *dst) {
    stmfd sp!, {r4, r5, r6, r7, r8}
    ldmia r0!, {r2, r3, r4, r5, r6, r7, r8, ip}
    stmia r1, {r2, r3, r4, r5, r6, r7, r8, ip}
    ldmia r0!, {r2, r3, r4, r5, r6, r7, r8, ip}
    stmia r1, {r2, r3, r4, r5, r6, r7, r8, ip}
    ldmfd sp!, {r4, r5, r6, r7, r8}
    bx lr
}
