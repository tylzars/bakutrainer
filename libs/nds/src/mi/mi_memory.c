#include <nitro/mi.h>

// Two reasons recur in the "Not compiled" lines below:
// - Branch pair: `blt L2; b L3; L2:` jumps over a lone `b` that nothing else reaches. With optimisation on, the
//   compiler turns every such pair into one inverted branch. The merges that run after that could only leave a
//   moved instruction in front of the first branch or at the `b` target; the routines that cite this say why
//   neither is the case there. Without optimisation no parameter stays in its incoming register unless an asm
//   statement names it, but these routines use r0-r2 as they arrive.
// - Writeback: the compiler writes back an ldm/stm base only in push/pop on sp and in struct copies and fills,
//   which always use r0-r3 (r0-r1 for a two-word fill). The pass that merges loads or stores into one ldm/stm
//   never adds writeback.

// Stores the halfword data over size bytes at dest, two bytes at a time.
// Not compiled: branch pairs; only the compare is in front of `blt L2`, and L3 starts with a branch.
// As a counted C loop, -O2 and up give `strhlt; addlt; blt` (predicated, no branch pairs), -O1 gives `bxge lr`
// and an unconditional loop (also with `#pragma peephole off`), -O0 spills r0-r2.
// clang-format off
asm void MIi_CpuClear16(u16 data, void *dest, u32 size) {
    // r3 = offset
    mov r3, #0
    // while offset < size: store the halfword, step 2; each step sits behind its own branch pair
L1:
    cmp r3, r2
    blt L2
    b L3
L2:
    strh r0, [r1, r3]
L3:
    blt L4
    b L5
L4:
    add r3, r3, #2
L5:
    blt L1
    bx lr
}
// clang-format on

// Copies size bytes from src to dest as halfwords.
// Not compiled: branch pairs; only the compare is in front of `blt L2`, and L3 starts with a branch.
// As a counted C loop, -O2 and up give `ldrhlt; strhlt; addlt; blt`, -O1 gives `bxge lr` and an unconditional
// loop.
// clang-format off
asm void MIi_CpuCopy16(const void *src, void *dest, u32 size) {
    // ip = offset
    mov ip, #0
    // while offset < size: load a halfword, store it, step 2; each step sits behind its own branch pair
L1:
    cmp ip, r2
    blt L2
    b L3
L2:
    ldrh r3, [r0, ip]
L3:
    blt L4
    b L5
L4:
    strh r3, [r1, ip]
L5:
    blt L6
    b L7
L6:
    add ip, ip, #2
L7:
    blt L1
    bx lr
}
// clang-format on

// Stores the word data over size bytes at dest.
// Not compiled: writeback, `stmia r1!, {r0}` (one register).
// clang-format off
asm void MIi_CpuClear32(u32 data, void *dest, u32 size) {
    add ip, r1, r2
L1:
    cmp r1, ip
    blt L2
    b L3
L2:
    stmia r1!, {r0}
L3:
    blt L1
    bx lr
}
// clang-format on

// Copies size bytes from src to dest as words.
// Not compiled: writeback, `ldmia r0!, {r2}` and `stmia r1!, {r2}` (one register).
// clang-format off
asm void MIi_CpuCopy32(const void *src, void *dest, u32 size) {
    // ip = end of dest
    add ip, r1, r2
    // while dest < end: copy a word; the load and the store each sit behind a branch pair
L1:
    cmp r1, ip
    blt L2
    b L3
L2:
    ldmia r0!, {r2}
L3:
    blt L4
    b L5
L4:
    stmia r1!, {r2}
L5:
    blt L1
    bx lr
}
// clang-format on

// Stores the word data over size bytes at dest: eight words per store while 32 bytes remain, then single words.
// Not compiled: writeback, `stmia r1!, {r0, r2-r8}` and `stmia r1!, {r0}`.
// clang-format off
asm void MIi_CpuClearFast(u32 data, void *dest, u32 size) {
    stmfd sp!, {r4, r5, r6, r7, r8, r9}
    // r9 = end of dest, ip = end of the whole 32-byte blocks
    add r9, r1, r2
    mov ip, r2, lsr #5
    add ip, r1, ip, lsl #5
    // copy data into r2-r8
    mov r2, r0
    mov r3, r2
    mov r4, r2
    mov r5, r2
    mov r6, r2
    mov r7, r2
    mov r8, r2
    // store eight words at a time up to the block end
L1:
    cmp r1, ip
    blt L2
    b L3
L2:
    stmia r1!, {r0, r2, r3, r4, r5, r6, r7, r8}
L3:
    blt L1
    // store single words up to the end
L4:
    cmp r1, r9
    blt L5
    b L6
L5:
    stmia r1!, {r0}
L6:
    blt L4
    ldmfd sp!, {r4, r5, r6, r7, r8, r9}
    bx lr
}
// clang-format on

// Copies size bytes from src to dest: eight words per step while 32 bytes remain, then single words.
// Not compiled: saves seven registers (r4-r10) and has no stack frame. A compiled function adds r3 to its save
// list whenever the saved registers and its frame come to an odd number of words, so it would save {r3-r10}.
// clang-format off
asm void MIi_CpuCopyFast(const void *src, void *dest, u32 size) {
    stmfd sp!, {r4, r5, r6, r7, r8, r9, r10}
    // r10 = end of dest, ip = end of the whole 32-byte blocks
    add r10, r1, r2
    mov ip, r2, lsr #5
    add ip, r1, ip, lsl #5
    // copy eight words at a time up to the block end
L1:
    cmp r1, ip
    blt L2
    b L3
L2:
    ldmia r0!, {r2, r3, r4, r5, r6, r7, r8, r9}
L3:
    blt L4
    b L5
L4:
    stmia r1!, {r2, r3, r4, r5, r6, r7, r8, r9}
L5:
    blt L1
    // copy single words up to the end
L6:
    cmp r1, r10
    blt L7
    b L8
L7:
    ldmia r0!, {r2}
L8:
    blt L9
    b L10
L9:
    stmia r1!, {r2}
L10:
    blt L6
    ldmfd sp!, {r4, r5, r6, r7, r8, r9, r10}
    bx lr
}
// clang-format on

// Copies 32 bytes (eight words) from src to dest.
// Not compiled: writeback, `ldmia r0!, {r2, r3, ip}` and `stmia r1!, {r2, r3, ip}`.
// clang-format off
asm void func_0203fda0(const void *src, void *dest) {
    ldmia r0!, {r2, r3, ip}
    stmia r1!, {r2, r3, ip}
    ldmia r0!, {r2, r3, ip}
    stmia r1!, {r2, r3, ip}
    ldmia r0!, {r2, r3}
    stmia r1!, {r2, r3}
    bx lr
}
// clang-format on

// Copies 36 bytes (nine words) from src to dest.
// Not compiled: writeback, `ldmia r0!, {r2, r3, ip}` and `stmia r1!, {r2, r3, ip}`.
// clang-format off
asm void MI_Copy36B(const void *src, void *dest) {
    ldmia r0!, {r2, r3, ip}
    stmia r1!, {r2, r3, ip}
    ldmia r0!, {r2, r3, ip}
    stmia r1!, {r2, r3, ip}
    ldmia r0!, {r2, r3, ip}
    stmia r1!, {r2, r3, ip}
    bx lr
}
// clang-format on

// Copies 48 bytes (twelve words) from src to dest.
// Not compiled: writeback, `ldmia r0!, {r2, r3, ip}` and `stmia r1!, {r2, r3, ip}`.
// clang-format off
asm void func_0203fdd8(const void *src, void *dest) {
    // four steps of three words
    ldmia r0!, {r2, r3, ip}
    stmia r1!, {r2, r3, ip}
    ldmia r0!, {r2, r3, ip}
    stmia r1!, {r2, r3, ip}
    ldmia r0!, {r2, r3, ip}
    stmia r1!, {r2, r3, ip}
    ldmia r0!, {r2, r3, ip}
    stmia r1!, {r2, r3, ip}
    bx lr
}
// clang-format on

// Fills size bytes at dest with the low byte of data using halfword and word stores. An odd first or last byte
// is merged into its halfword by read-modify-write. Does nothing when size is 0.
// Not compiled: branch pairs (`beq L1; b L2; L1: bx lr`); only the compare is in front of `beq L1`, and L1
// returns instead of falling into L2. In C at -O4,p each skipped instruction comes out predicated (`bxeq lr`,
// `strhne r3, [r0], #2`).
// clang-format off
asm void MI_CpuFill8(void *dest, u8 data, u32 size) {
    // size 0: return
    cmp r2, #0
    beq L1
    b L2
L1:
    bx lr
    // odd dest: put the byte in the high half of the halfword at dest - 1; step 1, return if nothing is left
L2:
    tst r0, #1
    beq L4
    ldrh ip, [r0, #-1]
    and ip, ip, #0xff
    orr r3, ip, r1, lsl #8
    strh r3, [r0, #-1]
    add r0, r0, #1
    subs r2, r2, #1
    beq L3
    b L4
L3:
    bx lr
    // under 2 bytes left: go to the last byte. Otherwise repeat the byte into a halfword, and store one
    // halfword if dest is not word aligned; return if nothing is left
L4:
    cmp r2, #2
    bcc L10
    orr r1, r1, r1, lsl #8
    tst r0, #2
    beq L6
    strh r1, [r0], #2
    subs r2, r2, #2
    beq L5
    b L6
L5:
    bx lr
    // repeat the halfword into a word and store words for the whole words of size
L6:
    orr r1, r1, r1, lsl #16
    bics r3, r2, #3
    beq L8
    sub r2, r2, r3
    add ip, r3, r0
L7:
    str r1, [r0], #4
    cmp r0, ip
    bcc L7
    // a halfword left: store it
L8:
    tst r2, #2
    bne L9
    b L10
L9:
    strh r1, [r0], #2
    // a byte left: put it in the low half of the halfword at dest, keeping the high half
L10:
    tst r2, #1
    beq L11
    b L12
L11:
    bx lr
L12:
    ldrh r3, [r0]
    and r3, r3, #0xff00
    and r1, r1, #0xff
    orr r1, r1, r3
    strh r1, [r0]
    bx lr
}
// clang-format on

// Copies size bytes from src to dest using halfword and word accesses only, for any alignment of src and dest.
// An odd first or last byte is merged into its halfword by read-modify-write; when src and dest differ in parity
// the source halfwords are shifted by a byte on the way.
// Not compiled: `subs r2, r2, #2` is followed by `bcc` and `bcs`, branches on the subtract's carry. The compiler
// makes a flag-setting subtract only by folding a following `cmp rd, #0` into it, which leaves the branch testing
// eq, ne, mi or pl (ge and lt become pl and mi, hs becomes always; lo, gt and le are not folded), or inside 64-bit
// subtract, compare and shift sequences, where the next sbc, sbcs or conditional orr reads the flags.
// clang-format off
asm void MI_CpuCopy8(const void *src, void *dest, u32 size) {
    // size 0: return
    cmp r2, #0
    beq L1
    b L2
L1:
    bx lr
    // odd dest: ip = the byte below dest; r3 = the src byte (high half of the halfword at src - 1 for an odd
    // src, low half of the halfword at src for an even one)
L2:
    tst r1, #1
    beq L10
    ldrh ip, [r1, #-1]
    and ip, ip, #0xff
    tst r0, #1
    bne L3
    b L4
L3:
    ldrh r3, [r0, #-1]
L4:
    bne L5
    b L6
L5:
    mov r3, r3, lsr #8
L6:
    beq L7
    b L8
L7:
    ldrh r3, [r0]
    // store the two bytes as the halfword at dest - 1; step 1, return if nothing is left
L8:
    orr r3, ip, r3, lsl #8
    strh r3, [r1, #-1]
    add r0, r0, #1
    add r1, r1, #1
    subs r2, r2, #1
    beq L9
    b L10
L9:
    bx lr
    // dest is even here. src odd: align src down, r3 = its first byte
L10:
    eor ip, r1, r0
    tst ip, #1
    beq L15
    bic r0, r0, #1
    ldrh ip, [r0], #2
    mov r3, ip, lsr #8
    subs r2, r2, #2
    bcc L12
    // per halfword: next src halfword, store the carried byte with its low byte above it, carry its high byte
L11:
    ldrh ip, [r0], #2
    orr ip, r3, ip, lsl #8
    strh ip, [r1], #2
    mov r3, ip, lsr #16
    subs r2, r2, #2
    bcs L11
    // a byte left: put the carried byte in the low half of the halfword at dest
L12:
    tst r2, #1
    beq L13
    b L14
L13:
    bx lr
L14:
    ldrh ip, [r1]
    and ip, ip, #0xff00
    orr ip, ip, r3
    strh ip, [r1]
    bx lr
    // same parity but not the same word alignment: copy halfwords, then the last byte
L15:
    tst ip, #2
    beq L17
    bics r3, r2, #1
    beq L25
    sub r2, r2, r3
    add ip, r3, r1
L16:
    ldrh r3, [r0], #2
    strh r3, [r1], #2
    cmp r1, ip
    bcc L16
    b L25
    // same word alignment: under 2 bytes left, go to the last byte; copy a halfword if dest is not word
    // aligned, return if nothing is left
L17:
    cmp r2, #2
    bcc L25
    tst r1, #2
    beq L19
    ldrh r3, [r0], #2
    strh r3, [r1], #2
    subs r2, r2, #2
    beq L18
    b L19
L18:
    bx lr
    // copy words for the whole words of size
L19:
    bics r3, r2, #3
    beq L21
    sub r2, r2, r3
    add ip, r3, r1
L20:
    ldr r3, [r0], #4
    str r3, [r1], #4
    cmp r1, ip
    bcc L20
    // a halfword left: copy it
L21:
    tst r2, #2
    bne L22
    b L23
L22:
    ldrh r3, [r0], #2
L23:
    bne L24
    b L25
L24:
    strh r3, [r1], #2
    // a byte left: put the src byte in the low half of the halfword at dest
L25:
    tst r2, #1
    beq L26
    b L27
L26:
    bx lr
L27:
    ldrh r2, [r1]
    ldrh r0, [r0]
    and r2, r2, #0xff00
    and r0, r0, #0xff
    orr r0, r2, r0
    strh r0, [r1]
    bx lr
}
// clang-format on
