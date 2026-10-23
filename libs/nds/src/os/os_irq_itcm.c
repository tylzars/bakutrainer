// Interrupt dispatch and the thread-switch return path (ITCM), hand-written assembly.

#include <nitro/os.h>
#include <nitro/cp.h>

// Interrupt dispatcher. Returns at once if the master enable (0x04000208) is zero or no enabled request is pending
// (enable 0x04000210 AND request 0x04000214). Otherwise picks the lowest-numbered pending bit (clearing the
// highest set bit until one remains, by leading-zero count), acknowledges it by writing that bit alone to the
// request register, and jumps to the handler at index = bit number (31 - leading zeros) of the word table at
// SDK_AUTOLOAD_DTCM_START. lr is preset to func_01ff8068, so the handler returns into the thread-switch code, which
// pops the lr pushed here. Two literal words follow the routine (table base, return address).
// Not compiled: clz has no C expression, and the lr it pushes is popped by func_01ff8068 once a handler has run.
// clang-format off
asm void func_01ff8000(void) {
    stmfd sp!, {lr}
    mov ip, #0x4000000
    add ip, ip, #0x210
    ldr r1, [ip, #-8]           // master enable
    cmp r1, #0
    beq Lret1
    b Lgo1
Lret1:
    ldmfd sp!, {pc}
Lgo1:
    ldmia ip, {r1, r2}          // r1 = enable, r2 = request
    ands r1, r1, r2             // pending and enabled
    beq Lret2
    b Lgo2
Lret2:
    ldmfd sp!, {pc}
Lgo2:
    mov r3, #0x80000000
Lfind:
    clz r0, r1                  // highest set bit
    bics r1, r1, r3, lsr r0     // clear it in the working copy
    bne Lfind                   // loop to the lowest set bit
    mov r1, r3, lsr r0
    str r1, [ip, #4]            // acknowledge that request bit
    rsbs r0, r0, #31
    ldr r1, =SDK_AUTOLOAD_DTCM_START
    ldr r0, [r1, r0, lsl #2]    // handler address from the table
    ldr lr, =func_01ff8068
    bx r0
}
// clang-format on

// Runs after an interrupt handler returns.
// - If the word at data_027e0060 is non-zero, walks a chain of records from it: for each it stores 1 at +0x64 and
//   zeroes +0x78, +0x7c and +0x80 (the old +0x80 is the next link). Then it zeroes both words at data_027e0060 and
//   sets the halfword flag at data_020ca730 to 1.
// - If that flag is zero it returns, popping the lr pushed by func_01ff8000. Otherwise it clears the flag, enters
//   IRQ mode with IRQ and FIQ masked (0xd2) and finds the first thread in the list (head word at data_020ca730 + 8,
//   linked through +0x68) whose halfword at +0x64 is 1. If there is none, or it is the current thread
//   (data_020ca730 + 4), it sets the CPSR control byte to 0x92 (IRQ mode, only IRQ masked) and returns.
// - Otherwise it calls the function at data_020ca730 + 0xc if non-zero (r0 = current thread, r1 = new one), makes
//   the new thread current, and saves into the old thread's context record: SPSR (+0), the interrupted r0-r3 and
//   ip from the IRQ stack frame, r4-r11 and the user-bank sp and lr (+4..+0x3c), the IRQ lr (+0x40), the
//   supervisor sp (+0x44) and, through func_02042898, the divider and square-root registers (+0x48).
// - It then restores the new thread's divider and square-root registers (func_020428d8), supervisor sp, SPSR and
//   user-bank registers from its record, writes r0-r3, ip and the resume address (+0x40) over the IRQ stack frame,
//   and returns through the word popped from the stack. Four literal words follow the routine.
// Not compiled: changes mode with msr and reads SPSR with mrs, which C has no expression for, and it returns
// through the lr that func_01ff8000 pushed.
// clang-format off
asm void func_01ff8068(void) {
    // walk the chain from data_027e0060, then clear both words and set the flag
    ldr ip, =data_027e0060
    mov r3, #0
    ldr ip, [ip]
    mov r2, #1
    cmp ip, #0
    beq Lnochain
Lchain:
    str r2, [ip, #0x64]
    str r3, [ip, #0x78]
    str r3, [ip, #0x7c]
    ldr r0, [ip, #0x80]
    str r3, [ip, #0x80]
    mov ip, r0
    cmp ip, #0
    bne Lchain
    ldr ip, =data_027e0060
    str r3, [ip]
    str r3, [ip, #4]
    ldr ip, =data_020ca730
    mov r1, #1
    strh r1, [ip]
    // flag clear: return
Lnochain:
    ldr ip, =data_020ca730
    ldrh r1, [ip]
    cmp r1, #0
    beq Lskip1
    b Lgo
Lskip1:
    ldr pc, [sp], #4
    // clear the flag
Lgo:
    mov r1, #0
    strh r1, [ip]
    mov r3, #0xd2               // IRQ mode, IRQ/FIQ masked
    msr cpsr_c, r3
    // r1 = first thread whose +0x64 halfword is 1; each step sits behind a branch pair
    add r2, ip, #8
    ldr r1, [r2]
Lscan:
    cmp r1, #0
    bne Lload
    b Lnoload
Lload:
    ldrh r0, [r1, #0x64]
Lnoload:
    bne Lcmp
    b Lnocmp
Lcmp:
    cmp r0, #1
Lnocmp:
    bne Lnext
    b Lnonext
Lnext:
    ldr r1, [r1, #0x68]
Lnonext:
    bne Lscan
    cmp r1, #0
    bne Lfound
    // none, or the current thread: IRQ mode with only IRQ masked, return
Lreturn:
    mov r3, #0x92
    msr cpsr_c, r3
    ldr pc, [sp], #4
Lfound:
    ldr r0, [ip, #4]
    cmp r1, r0
    beq Lreturn
    // call the switch function if set
    ldr r3, [ip, #0xc]
    cmp r3, #0
    beq Lnohook
    stmfd sp!, {r0, r1, ip}
    mov lr, pc
    bx r3
    ldmfd sp!, {r0, r1, ip}
    // new thread current; save SPSR, then the divider and square-root registers, into the old record
Lnohook:
    str r1, [ip, #4]
    mrs r2, spsr
    str r2, [r0, #0]!
    stmfd sp!, {r0, r1}
    add r0, r0, #0
    add r0, r0, #0x48
    ldr r1, =func_02042898
    blx r1
    ldmfd sp!, {r0, r1}
    // interrupted r0-r3 and ip, r4-r11, user-bank sp and lr, the IRQ lr; then the supervisor sp
    ldmib sp!, {r2, r3}
    stmib r0!, {r2, r3}
    ldmib sp!, {r2, r3, ip, lr}
    stmib r0!, {r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, ip, sp, lr}^
    stmib r0!, {lr}
    mov r3, #0xd3               // supervisor mode, IRQ/FIQ masked
    msr cpsr_c, r3
    stmib r0!, {sp}
    // new thread: divider and square-root registers, supervisor sp
    stmfd sp!, {r1}
    add r0, r1, #0
    add r0, r0, #0x48
    ldr r1, =func_020428d8
    blx r1
    ldmfd sp!, {r1}
    ldr sp, [r1, #0x44]
    // back in IRQ mode: SPSR, resume address and user-bank registers from its record
    mov r3, #0xd2
    msr cpsr_c, r3
    ldr r2, [r1, #0]!
    msr spsr_fc, r2
    ldr lr, [r1, #0x40]
    ldmib r1, {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, ip, sp, lr}^
    mov r0, r0
    // r0-r3, ip and the resume address over the IRQ frame; return through the popped word
    stmda sp!, {r0, r1, r2, r3, ip, lr}
    ldmfd sp!, {pc}
}
// clang-format on
