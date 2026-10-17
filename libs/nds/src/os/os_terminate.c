#include <nitro/os.h>

void OS_Terminate(void) {
    while (1) {
        OS_DisableInterrupts();
        OS_Halt();
    }
}

// Writes 0 to the CP15 wait-for-interrupt register (c7, c0, 4), which stops
// the CPU until the next interrupt, then returns.
asm void OS_Halt(void) {
    mov r0, #0
    mcr p15, 0, r0, c7, c0, 4
    bx lr
}
