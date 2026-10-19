#include <nitro/cp.h>

// Saves the divider and square-root unit state to the 0x1c-byte buffer at cpContext: the four words from
// 0x04000290 (numerator and denominator), the two words from 0x040002b8 (square-root parameter), then the
// halfword at 0x04000280 masked to its two mode bits and the one at 0x040002b0 masked to its mode bit.
// Not compiled: the prologue saves r4 alone (`stmfd sp!, {r4}`, one word, no locals). The compiler pads every
// frame of a non-interrupt function to a multiple of 8 bytes by also saving r3, so it never emits a lone {r4}
// save; its struct copies also load and store only through r0-r3, never {r2, r3, r4, ip}.
// clang-format off
asm void func_02042898(void *cpContext) {
    ldr r1, =0x04000290
    stmfd sp!, {r4}
    ldmia r1, {r2, r3, r4, ip}      // numerator and denominator
    stmia r0!, {r2, r3, r4, ip}
    ldrh ip, [r1, #-0x10]           // divider control (0x04000280)
    add r1, r1, #0x28               // 0x040002b8
    ldmia r1, {r2, r3}              // square-root parameter
    stmia r0!, {r2, r3}
    and ip, ip, #3
    ldrh r2, [r1, #-8]              // square-root control (0x040002b0)
    strh ip, [r0, #0]
    and r2, r2, #1
    strh r2, [r0, #2]
    ldmfd sp!, {r4}
    bx lr
}
// clang-format on

// Writes a state saved by func_02042898 back: the four divider words to 0x04000290, the two mode halfwords to
// 0x04000280 and 0x040002b0, then the two square-root words to 0x040002b8.
// Not compiled: saves r4 alone (`stmfd sp!, {r4}`, one word, no locals), which the compiler's 8-byte frame
// padding with r3 rules out, and copies through {r2, r3, r4, ip} where its struct copies use only r0-r3.
// clang-format off
asm void func_020428d8(const void *cpContext) {
    stmfd sp!, {r4}
    ldr r1, =0x04000290
    ldmia r0, {r2, r3, r4, ip}      // numerator and denominator
    stmia r1, {r2, r3, r4, ip}
    ldrh r2, [r0, #0x18]
    ldrh r3, [r0, #0x1a]
    strh r2, [r1, #-0x10]           // divider control (0x04000280)
    strh r3, [r1, #0x20]            // square-root control (0x040002b0)
    add r0, r0, #0x10
    add r1, r1, #0x28               // 0x040002b8
    ldmia r0, {r2, r3}              // square-root parameter
    stmia r1, {r2, r3}
    ldmfd sp!, {r4}
    bx lr
}
// clang-format on
