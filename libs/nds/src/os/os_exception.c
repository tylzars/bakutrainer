#include <nitro/os.h>

static struct {
    u32 handler;
    u32 callbackArg;
    u32 callback;
} sExceptionState;

void OS_InitException(void) {
    u32 handler = *(vu32 *) 0x27ffd9c;
    if (handler >= 0x2600000 && handler < 0x2800000) {
        sExceptionState.handler = handler;
    } else {
        sExceptionState.handler = 0;
    }
    if (sExceptionState.handler == 0) {
        vu8 *dtcm = (vu8 *) 0x27e3000;
        *(vu32 *) 0x27ffd9c = (u32) OSi_ExceptionHandler;
        *(vu32 *) (dtcm + 0xfdc) = (u32) OSi_ExceptionHandler;
    }
    sExceptionState.callback = 0;
}

__declspec(align(32)) static char sExceptionContext[0x80];

// OSi_ExceptionHandler: entry for processor exceptions (abort or undefined
// instruction). If the first word of sExceptionState is non-zero (the
// shared-memory word OS_InitException read), it is called first (lr set
// to the following branch so it returns here). Then r0-r3, the entry sp and lr
// are pushed onto a small stack growing down from the base of main RAM, r0 is set to the
// entry sp's low bit, and sp is moved to that stack. The mode bits of CPSR are
// tested: for modes 0x17 and 0x1b (abort/undefined) the context capture is run
// (func_0203eec8). Afterwards the code loops forever whether or not the first
// word is non-zero; the trailing restore of r0-r3, sp (through ip) and lr and
// the return are never reached.
// Not compiled: reads the processor mode from CPSR with mrs, which C has no expression for.
asm void OSi_ExceptionHandler(void) {
    ldr ip, =sExceptionState
    ldr ip, [ip]
    cmp ip, #0
    bne Lcall
    b Lskip
Lcall:
    mov lr, pc
Lskip:
    bne Ljump
    b Lnojump
Ljump:
    bx ip
Lnojump:
    ldr ip, =0x2000000
    stmdb ip!, {r0, r1, r2, r3, sp, lr}
    and r0, sp, #1
    mov sp, ip
    mrs r1, cpsr
    and r1, r1, #0x1f
    teq r1, #0x17               // abort mode
    bne Ltry2
    bl func_0203eec8
    b Ldone
Ltry2:
    teq r1, #0x1b               // undefined-instruction mode
    bne Ldone
    bl func_0203eec8
Ldone:
    ldr ip, =sExceptionState
    ldr ip, [ip]
    cmp ip, #0
Lspin:
    beq Lspin                   // first word zero: halt here
Lnop:
    mov r0, r0
    b Lnop                      // non-zero: loop forever as well
    // unreachable: both paths above loop forever
    ldmfd sp!, {r0, r1, r2, r3, ip, lr}
    mov sp, ip
    bx lr
}

// func_0203eec8: saves r0 and lr, runs the register capture (func_0203eedc)
// and the hook launcher (func_0203ef6c), then returns.
// Not compiled: saves r0 beside lr and returns with `ldmfd sp!, {r0, lr}; bx lr`. A compiled function
// saves r0 only as an interrupt function, which saves every register among r0-r7 it writes (the calls
// here write r1-r3 too), and a compiled function that saved lr returns by loading it into pc. It also
// passes the handler's r0 and ip on to func_0203eedc, which no C call does.
asm void func_0203eec8(void) {
    stmfd sp!, {r0, lr}
    bl func_0203eedc
    bl func_0203ef6c
    ldmfd sp!, {r0, lr}
    bx lr
}

// func_0203eedc: captures the faulting context into the block at
// sExceptionContext. r0 is the entry sp's low bit as set by the handler, ip
// points at the pushed exception frame { r0, r1, r2, r3, sp, lr }. Stores CPSR (+0x74), r0 (+0x6c),
// the pushed r0-r3 (+4..+0x10), r4-r11 (+0x14), the frame address (+0x70), then
// reads four words from the address in the frame's fifth word with bit 0
// cleared (+0x64, +0, +0x34, +0x40). It then briefly writes the second of
// those words to CPSR (IRQ bit set, Thumb bit cleared), entering that mode, to
// grab that mode's sp (+0x38), lr (+0x3c) and SPSR (+0x7c), and restores the
// original CPSR.
// Not compiled: reads CPSR and SPSR with mrs and changes mode with msr, which C has no expression for.
asm void func_0203eedc(void) {
    ldr r1, =sExceptionContext
    mrs r2, cpsr
    str r2, [r1, #0x74]
    str r0, [r1, #0x6c]
    ldr r0, [ip, #0]
    str r0, [r1, #4]
    ldr r0, [ip, #4]
    str r0, [r1, #8]
    ldr r0, [ip, #8]
    str r0, [r1, #0xc]
    ldr r0, [ip, #0xc]
    str r0, [r1, #0x10]
    ldr r2, [ip, #0x10]
    bic r2, r2, #1
    add r0, r1, #0x14
    stmia r0, {r4, r5, r6, r7, r8, r9, r10, r11}
    str ip, [r1, #0x70]
    ldr r0, [r2, #0]
    str r0, [r1, #0x64]
    ldr r3, [r2, #4]
    str r3, [r1, #0]
    ldr r0, [r2, #8]
    str r0, [r1, #0x34]
    ldr r0, [r2, #0xc]
    str r0, [r1, #0x40]
    mrs r0, cpsr
    orr r3, r3, #0x80
    bic r3, r3, #0x20
    msr cpsr_fsxc, r3           // enter the captured mode
    str sp, [r1, #0x38]
    str lr, [r1, #0x3c]
    mrs r2, spsr
    str r2, [r1, #0x7c]
    msr cpsr_fsxc, r0           // back to the handler's mode
    bx lr
}

// func_0203ef6c: if the third word of sExceptionState is non-zero, calls it
// in system mode with IRQ masked (CPSR 0x9f): switches mode but keeps the
// caller's stack, pushes system mode's own sp and the caller's CPSR there,
// calls func_0203ed90, then jumps to the function in the third word with r0 =
// sExceptionContext and r1 = the second word, lr preset to the following
// 'bl func_0203eda0' instruction (its address held as a constant).
// After that function returns, calls func_0203eda0, restores system mode's sp
// and the caller's CPSR and returns.
// Not compiled: switches to system mode and back with mrs/msr, which C has no expression for.
asm void func_0203ef6c(void) {
    stmfd sp!, {r3, lr}
    ldr r0, =sExceptionState
    ldr r0, [r0, #8]
    cmp r0, #0
    ldmeqfd sp!, {r3, pc}
    mrs r2, cpsr
    mov r0, sp
    ldr r1, =0x9f
    msr cpsr_fsxc, r1
    mov r1, sp
    mov sp, r0
    stmfd sp!, {r1, r2}
    bl func_0203ed90
    ldr r0, =sExceptionContext
    ldr r1, =sExceptionState+4
    ldr r1, [r1, #0]
    ldr ip, =sExceptionState+8
    ldr ip, [ip, #0]
    ldr lr, =Lret
    bx ip
Lret:
    bl func_0203eda0
    ldmfd sp!, {r1, r2}
    mov sp, r1
    msr cpsr_fsxc, r2
    ldmfd sp!, {r3, pc}
}
