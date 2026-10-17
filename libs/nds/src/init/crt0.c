// Start-up code: the entry point, its fill, expand and autoload helpers, and the cache and protection
// set-up. All of it is hand-written assembly.

#include <nitro/os.h>
#include <nitro/init.h>
#include <runtime.h>

// Base of protection region 2 in the data TCM, with its enable and size bits (the region's register value).
extern char data_027e0021[];

#define IRQ_STACK_SIZE 0x800

// Startup entry. Masks interrupts, waits for scanline 0, sets up the memory protection/cache,
// sets the supervisor, IRQ and system-mode stacks inside the data TCM (at +0x3fc0, +0x3f7c and
// +0x377c), switches to system mode, clears the 16 KB data TCM and 1 KB of palette memory, fills
// 1 KB of OAM with the word 0x200, unpacks the compressed block described by the build table,
// copies and zero-fills the autoload areas, clears the BSS range and flushes both caches over it,
// zeroes a word in shared main memory, stores the IRQ handler address at data TCM + 0x3ffc, calls
// func_0204ded0 (empty), func_02000b98 (empty) and func_0204dee8 (runs the static-initializer
// table), and finally jumps to main with lr = 0xffff0000 and sp lowered to an 8-byte boundary.
asm void Entry(void) {
    mov ip, #0x4000000
    str ip, [ip, #0x208]            // master interrupt enable off
L_wait0:
    ldrh r0, [ip, #6]               // wait for vertical counter == 0
    cmp r0, #0
    bne L_wait0
    bl func_02000ab0                // cache / protection setup
    mov r0, #0x13
    msr cpsr_c, r0                  // supervisor mode
    ldr r0, =SDK_AUTOLOAD_DTCM_START
    add r0, r0, #0x3fc0
    mov sp, r0
    mov r0, #0x12
    msr cpsr_c, r0                  // IRQ mode
    ldr r0, =SDK_AUTOLOAD_DTCM_START
    add r0, r0, #0x3fc0
    sub r0, r0, #0x40
    sub sp, r0, #4
    tst sp, #4                      // lower by 4 if bit 2 is clear (leaves bit 2 set)
    beq L_al1
    b L_al2
L_al1:
    sub sp, sp, #4
L_al2:
    ldr r1, =IRQ_STACK_SIZE
    sub r1, r0, r1
    mov r0, #0x1f
    msr cpsr_fsxc, r0               // system mode
    sub sp, r1, #4
    mov r0, #0                      // clear the 16 KB data TCM
    ldr r1, =SDK_AUTOLOAD_DTCM_START
    mov r2, #0x4000
    bl func_02000954
    mov r0, #0                      // clear 1 KB of palette memory
    ldr r1, =0x5000000
    mov r2, #0x400
    bl func_02000954
    mov r0, #0x200                  // fill 1 KB of OAM with the word 0x200
    ldr r1, =0x7000000
    mov r2, #0x400
    bl func_02000954
    ldr r1, =BuildInfo
    ldr r0, [r1, #0x14]
    bl func_02000970                // unpack the compressed block
    bl func_02000a1c                // copy the autoload areas
    ldr r0, =BuildInfo
    ldr r1, [r0, #0xc]
    ldr r2, [r0, #0x10]
    mov r3, r1
    mov r0, #0
L_bss:
    cmp r1, r2                      // zero the BSS word range
    blo L_bss1
    b L_bss2
L_bss1:
    str r0, [r1], #4
L_bss2:
    blo L_bss
    bic r1, r3, #0x1f
L_flush:
    mcr p15, 0, r0, c7, c10, 4      // drain write buffer
    mcr p15, 0, r1, c7, c5, 1       // invalidate instruction line
    mcr p15, 0, r1, c7, c14, 1      // clean and invalidate data line
    add r1, r1, #0x20
    cmp r1, r2
    blt L_flush
    ldr r1, =0x27fff9c              // zero this word
    str r0, [r1, #0]
    ldr r1, =SDK_AUTOLOAD_DTCM_START
    add r1, r1, #0x3fc0
    add r1, r1, #0x3c
    ldr r0, =func_01ff8000          // store the IRQ handler pointer
    str r0, [r1, #0]
    bl func_0204ded0
    bl func_02000b98
    bl func_0204dee8
    ldr r1, =main
    ldr lr, =0xffff0000
    tst sp, #4                      // lower by 4 if bit 2 is set (8-byte boundary)
    bne L_ex1
    b L_ex2
L_ex1:
    sub sp, sp, #4
L_ex2:
    bx r1
}

// Fills size bytes at dst (r1, word aligned) with the word in r0, a word at a time.
asm void func_02000954(u32 val, u32 *dst, u32 size) {
    add ip, r1, r2                  // end address
L_loop:
    cmp r1, ip
    blt L_st
    b L_nx
L_st:
    stmia r1!, {r0}
L_nx:
    blt L_loop
    bx lr
}

// Expands a backwards-packed stream. r0 points just past the packed data; the word pair before
// it holds the packed length/header-size (top byte = header bytes) and the growth in size. Reads
// bytes downward, each flag byte (MSB first) selects a literal byte or a copy from earlier output
// (12-bit distance, length from the high nibble + 3); output is written downward. Stops when the
// source cursor reaches the lower bound. Does nothing if r0 is null. Afterwards, for each 32-byte
// line from the packed data's lower bound up to the output end, drains the write buffer,
// invalidates the instruction line and cleans and invalidates the data line.
asm void func_02000970(void *end) {
    cmp r0, #0
    beq L_ret
    stmfd sp!, {r4, r5, r6, r7}
    ldmdb r0, {r1, r2}
    add r2, r0, r2                  // output end
    sub r3, r0, r1, lsr #24         // source cursor, after skipping the header
    bic r1, r1, #0xff000000
    sub r1, r0, r1                  // source lower bound
    mov r4, r2
L_next:
    cmp r3, r1
    ble L_done
    ldrb r5, [r3, #-1]!             // flag byte
    mov r6, #8
L_bit:
    subs r6, r6, #1
    blt L_next
    tst r5, #0x80
    bne L_ref
    ldrb r0, [r3, #-1]!             // literal byte
    strb r0, [r2, #-1]!
    b L_chk
L_ref:
    ldrb ip, [r3, #-1]!             // copy from earlier output: two bytes
    ldrb r7, [r3, #-1]!
    orr r7, r7, ip, lsl #8
    bic r7, r7, #0xf000
    add r7, r7, #2                  // distance
    add ip, ip, #0x20
L_copy:
    ldrb r0, [r2, r7]
    strb r0, [r2, #-1]!
    subs ip, ip, #0x10
    bge L_copy
L_chk:
    cmp r3, r1
    mov r5, r5, lsl #1
    bgt L_bit
L_done:
    mov r0, #0
    bic r3, r1, #0x1f
L_cache:
    mcr p15, 0, r0, c7, c10, 4
    mcr p15, 0, r3, c7, c5, 1
    mcr p15, 0, r3, c7, c14, 1
    add r3, r3, #0x20
    cmp r3, r4
    blt L_cache
    ldmfd sp!, {r4, r5, r6, r7}
L_ret:
    bx lr
}

// Walks the table at BuildInfo (start, end, source pointer). Each entry is (dest, copy length,
// zero length): copies the words from the source stream to dest, zero-fills the following
// words, then for every 32-byte line of the whole area drains the write buffer, invalidates the
// instruction line and cleans and invalidates the data line. Ends by jumping to AutoloadCallback.
asm void func_02000a1c(void) {
    ldr r0, =BuildInfo
    ldr r1, [r0, #0]                // table cursor
    ldr r2, [r0, #4]                // table end
    ldr r3, [r0, #8]                // source stream
L_entry:
    cmp r1, r2
    beq L_fin
    ldr r5, [r1], #4                // destination
    ldr r7, [r1], #4                // copy length
    add r6, r5, r7
    mov r4, r5
L_cp:
    cmp r4, r6
    bmi L_cp1
    b L_cp2
L_cp1:
    ldr r7, [r3], #4
L_cp2:
    bmi L_cp3
    b L_cp4
L_cp3:
    str r7, [r4], #4
L_cp4:
    bmi L_cp
    ldr r7, [r1], #4                // zero length
    add r6, r4, r7
    mov r7, #0
L_zf:
    cmp r4, r6
    blo L_zf1
    b L_zf2
L_zf1:
    str r7, [r4], #4
L_zf2:
    blo L_zf
    bic r4, r5, #0x1f
L_fl:
    mcr p15, 0, r7, c7, c10, 4
    mcr p15, 0, r4, c7, c5, 1
    mcr p15, 0, r4, c7, c14, 1
    add r4, r4, #0x20
    cmp r4, r6
    blt L_fl
    b L_entry
L_fin:
    b AutoloadCallback
}

// Empty hook: returns immediately.
void AutoloadCallback(void) {
}

// Cache and protection setup: clears control-register bits 0xf9005 (among them MPU, caches, TCMs),
// invalidates both caches and drains the write buffer, programs the eight protection regions, the
// data/instruction TCM windows, the cacheability / bufferability / access-permission registers,
// then sets control-register bits 0x5707d (MPU, caches, write buffer, both TCMs, high exception
// vectors, round-robin replacement).
asm void func_02000ab0(void) {
    mrc p15, 0, r0, c1, c0, 0
    ldr r1, =0xf9005
    bic r0, r0, r1
    mcr p15, 0, r0, c1, c0, 0
    mov r0, #0
    mcr p15, 0, r0, c7, c5, 0       // invalidate instruction cache
    mcr p15, 0, r0, c7, c6, 0       // invalidate data cache
    mcr p15, 0, r0, c7, c10, 4      // drain write buffer
    ldr r0, =0x4000033
    mcr p15, 0, r0, c6, c0, 0
    ldr r0, =0x200002d
    mcr p15, 0, r0, c6, c1, 0
    ldr r0, =data_027e0021
    mcr p15, 0, r0, c6, c2, 0
    ldr r0, =0x8000035
    mcr p15, 0, r0, c6, c3, 0
    ldr r0, =SDK_AUTOLOAD_DTCM_START
    orr r0, r0, #0x1a
    orr r0, r0, #1
    mcr p15, 0, r0, c6, c4, 0
    ldr r0, =0x100002f
    mcr p15, 0, r0, c6, c5, 0
    ldr r0, =0xffff001d
    mcr p15, 0, r0, c6, c6, 0
    ldr r0, =0x27ff017
    mcr p15, 0, r0, c6, c7, 0
    mov r0, #0x20
    mcr p15, 0, r0, c9, c1, 1       // instruction TCM region
    ldr r0, =SDK_AUTOLOAD_DTCM_START
    orr r0, r0, #0xa
    mcr p15, 0, r0, c9, c1, 0       // data TCM region
    mov r0, #0x42
    mcr p15, 0, r0, c2, c0, 1       // instruction cacheability
    mov r0, #0x42
    mcr p15, 0, r0, c2, c0, 0       // data cacheability
    mov r0, #2
    mcr p15, 0, r0, c3, c0, 0       // write-buffer control
    ldr r0, =0x5100011
    mcr p15, 0, r0, c5, c0, 3       // instruction access permissions
    ldr r0, =0x15111011
    mcr p15, 0, r0, c5, c0, 2       // data access permissions
    mrc p15, 0, r0, c1, c0, 0
    ldr r1, =0x5707d
    orr r0, r0, r1
    mcr p15, 0, r0, c1, c0, 0       // enable MPU, caches and TCMs
    bx lr
}

// Empty hook: returns immediately.
void func_02000b98(void) {
}

// Empty hook: returns immediately.
void func_02000b9c(void) {
}
