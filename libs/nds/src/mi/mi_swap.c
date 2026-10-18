#include <nitro/mi.h>

// Atomically exchanges the word at destp with setData and returns the previous word.
// Not compiled: swp (word swap) has no C expression.
asm u32 MI_SwapWord(u32 setData, vu32 *destp) {
    swp r0, r0, [r1]
    bx lr
}
