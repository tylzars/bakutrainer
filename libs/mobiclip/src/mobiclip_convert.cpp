#include <mobiclip.h>
#include <nitro/mi.h>

// YUV-to-RGB555 conversion (hand-written assembly): args points at {Y, UV, dst, stride, width, height, table}.
// Y rows are 0x100 bytes apart; each U/V row (0x100 bytes, U in the first 0x80, V in the second) serves two Y
// rows, one U/V pair per 2x2 pixels. Output pixels are 16-bit with bit 15 set. The loops take 16 pixels and two
// rows per pass.
// Not compiled: saves ip (r12) with r4-r11 and lr. A compiled function saves r12 only as an interrupt function,
// which saves every register among r0-r7 it writes; this one writes r0-r3 and does not save them.
// clang-format off
asm void func_0203af68(void *args) {
    // save the registers; unpack the argument block
    stmfd sp!, {r4, r5, r6, r7, r8, r9, r10, r11, ip, lr}
    ldr r8, [r0, #0xc]              // stride: bytes per output row
    ldr r9, [r0, #0x10]             // width in pixels
    ldr r10, [r0, #0x14]            // height in pixels
    ldr r3, [r0, #0x18]             // lookup table
    mov r4, #0x100
    rsb r5, r9, r4, lsl #0x1        // Y step to the next row pair: 0x200 - width
    sub r6, r4, r9, lsr #0x1        // U/V step to the next row: 0x100 - width / 2
    sub r7, r8, r9
    mov r7, r7, lsl #0x1            // output step over the second row: 2 * (stride - width)
    stmfd sp!, {r5, r6, r7, r8, r9, r10} // kept on the stack for the whole call
    ldr r1, [r0, #0x4]              // r1 = U/V
    ldr r2, [r0, #0x8]              // r2 = output, first row of the pair
    ldr r0, [r0, #0x0]              // r0 = Y
    add r3, r3, #0x100              // r3 = table + 0x100, indexed with signed offsets
    add lr, r2, r8                  // lr = output, second row of the pair
    // one pass per pair of rows
row_pair:
    str r10, [sp, #-4]!             // save the rows left
    ldr r10, [sp, #0x14]            // r10 = pixels left in this row pair
pixel_block:
    // 16 pixels per pass, as eight 2x2 blocks. Each block shares one U and one V byte: three table bases
    // are formed from them, each indexed by a Y byte, and the three results are packed into one 16-bit
    // pixel. Two of the four pixels use Y - 4.
    ldrb r5, [r1], #0x80            // U
    ldrb r6, [r1], #-0x7f           // V, 0x80 bytes after U; r1 moves on to the next U
    ldrb r4, [r0], #0x1             // Y, first row, left pixel
    sub r5, r5, #0x80               // u = U - 0x80
    sub r6, r6, #0x80               // v = V - 0x80
    add r7, r3, r6                  // r7 = table + v
    sub r6, r5, r6
    add r6, r6, r3                  // r6 = table + u - v
    sub r5, r6, r5, lsl #0x1        // r5 = table - u - v
    ldrb r8, [r7, r4]               // each base indexed by Y
    ldrb r11, [r6, r4]
    ldrb ip, [r5, r4]
    ldrb r4, [r0], #0xff            // Y, first row, right pixel; r0 moves to the second row
    add r8, r11, r8, lsl #0x5       // r6 lookup in bits 0-4, r7 in bits 5-9
    add r8, r8, ip, lsl #0xa        // r5 in bits 10-14
    orr r8, r8, #0x8000             // and bit 15
    sub r4, r4, #0x4                // this pixel uses Y - 4
    ldrb r9, [r7, r4]
    ldrb r11, [r6, r4]
    ldrb ip, [r5, r4]
    ldrb r4, [r0], #0x1             // Y, second row, left pixel
    add r8, r8, r9, lsl #0x15       // same packing for the upper halfword
    add r8, r8, r11, lsl #0x10
    add r8, r8, ip, lsl #0x1a
    orr r8, r8, #0x80000000         // bit 31
    str r8, [r2], #0x4              // two pixels to the first row
    sub r4, r4, #0x4                // this pixel uses Y - 4 too
    ldrb r8, [r7, r4]
    ldrb r11, [r6, r4]
    ldrb ip, [r5, r4]
    ldrb r4, [r0], #-0xff           // Y, second row, right pixel; r0 back to the first row, next pair of columns
    add r8, r11, r8, lsl #0x5
    add r8, r8, ip, lsl #0xa
    orr r8, r8, #0x8000
    ldrb r9, [r7, r4]
    ldrb r11, [r6, r4]
    ldrb ip, [r5, r4]
    add r8, r8, r9, lsl #0x15
    add r8, r8, r11, lsl #0x10
    add r8, r8, ip, lsl #0x1a
    orr r8, r8, #0x80000000
    str r8, [lr], #0x4              // two pixels to the second row
    // block 2: the same, two columns on
    ldrb r5, [r1], #0x80
    ldrb r6, [r1], #-0x7f
    ldrb r4, [r0], #0x1
    sub r5, r5, #0x80
    sub r6, r6, #0x80
    add r7, r3, r6
    sub r6, r5, r6
    add r6, r6, r3
    sub r5, r6, r5, lsl #0x1
    ldrb r8, [r7, r4]
    ldrb r11, [r6, r4]
    ldrb ip, [r5, r4]
    ldrb r4, [r0], #0xff
    add r8, r11, r8, lsl #0x5
    add r8, r8, ip, lsl #0xa
    orr r8, r8, #0x8000
    sub r4, r4, #0x4
    ldrb r9, [r7, r4]
    ldrb r11, [r6, r4]
    ldrb ip, [r5, r4]
    ldrb r4, [r0], #0x1
    add r8, r8, r9, lsl #0x15
    add r8, r8, r11, lsl #0x10
    add r8, r8, ip, lsl #0x1a
    orr r8, r8, #0x80000000
    str r8, [r2], #0x4
    sub r4, r4, #0x4
    ldrb r8, [r7, r4]
    ldrb r11, [r6, r4]
    ldrb ip, [r5, r4]
    ldrb r4, [r0], #-0xff
    add r8, r11, r8, lsl #0x5
    add r8, r8, ip, lsl #0xa
    orr r8, r8, #0x8000
    ldrb r9, [r7, r4]
    ldrb r11, [r6, r4]
    ldrb ip, [r5, r4]
    add r8, r8, r9, lsl #0x15
    add r8, r8, r11, lsl #0x10
    add r8, r8, ip, lsl #0x1a
    orr r8, r8, #0x80000000
    str r8, [lr], #0x4
    // block 3: the same, two columns on
    ldrb r5, [r1], #0x80
    ldrb r6, [r1], #-0x7f
    ldrb r4, [r0], #0x1
    sub r5, r5, #0x80
    sub r6, r6, #0x80
    add r7, r3, r6
    sub r6, r5, r6
    add r6, r6, r3
    sub r5, r6, r5, lsl #0x1
    ldrb r8, [r7, r4]
    ldrb r11, [r6, r4]
    ldrb ip, [r5, r4]
    ldrb r4, [r0], #0xff
    add r8, r11, r8, lsl #0x5
    add r8, r8, ip, lsl #0xa
    orr r8, r8, #0x8000
    sub r4, r4, #0x4
    ldrb r9, [r7, r4]
    ldrb r11, [r6, r4]
    ldrb ip, [r5, r4]
    ldrb r4, [r0], #0x1
    add r8, r8, r9, lsl #0x15
    add r8, r8, r11, lsl #0x10
    add r8, r8, ip, lsl #0x1a
    orr r8, r8, #0x80000000
    str r8, [r2], #0x4
    sub r4, r4, #0x4
    ldrb r8, [r7, r4]
    ldrb r11, [r6, r4]
    ldrb ip, [r5, r4]
    ldrb r4, [r0], #-0xff
    add r8, r11, r8, lsl #0x5
    add r8, r8, ip, lsl #0xa
    orr r8, r8, #0x8000
    ldrb r9, [r7, r4]
    ldrb r11, [r6, r4]
    ldrb ip, [r5, r4]
    add r8, r8, r9, lsl #0x15
    add r8, r8, r11, lsl #0x10
    add r8, r8, ip, lsl #0x1a
    orr r8, r8, #0x80000000
    str r8, [lr], #0x4
    // block 4: the same, two columns on
    ldrb r5, [r1], #0x80
    ldrb r6, [r1], #-0x7f
    ldrb r4, [r0], #0x1
    sub r5, r5, #0x80
    sub r6, r6, #0x80
    add r7, r3, r6
    sub r6, r5, r6
    add r6, r6, r3
    sub r5, r6, r5, lsl #0x1
    ldrb r8, [r7, r4]
    ldrb r11, [r6, r4]
    ldrb ip, [r5, r4]
    ldrb r4, [r0], #0xff
    add r8, r11, r8, lsl #0x5
    add r8, r8, ip, lsl #0xa
    orr r8, r8, #0x8000
    sub r4, r4, #0x4
    ldrb r9, [r7, r4]
    ldrb r11, [r6, r4]
    ldrb ip, [r5, r4]
    ldrb r4, [r0], #0x1
    add r8, r8, r9, lsl #0x15
    add r8, r8, r11, lsl #0x10
    add r8, r8, ip, lsl #0x1a
    orr r8, r8, #0x80000000
    str r8, [r2], #0x4
    sub r4, r4, #0x4
    ldrb r8, [r7, r4]
    ldrb r11, [r6, r4]
    ldrb ip, [r5, r4]
    ldrb r4, [r0], #-0xff
    add r8, r11, r8, lsl #0x5
    add r8, r8, ip, lsl #0xa
    orr r8, r8, #0x8000
    ldrb r9, [r7, r4]
    ldrb r11, [r6, r4]
    ldrb ip, [r5, r4]
    add r8, r8, r9, lsl #0x15
    add r8, r8, r11, lsl #0x10
    add r8, r8, ip, lsl #0x1a
    orr r8, r8, #0x80000000
    str r8, [lr], #0x4
    // block 5: the same, two columns on
    ldrb r5, [r1], #0x80
    ldrb r6, [r1], #-0x7f
    ldrb r4, [r0], #0x1
    sub r5, r5, #0x80
    sub r6, r6, #0x80
    add r7, r3, r6
    sub r6, r5, r6
    add r6, r6, r3
    sub r5, r6, r5, lsl #0x1
    ldrb r8, [r7, r4]
    ldrb r11, [r6, r4]
    ldrb ip, [r5, r4]
    ldrb r4, [r0], #0xff
    add r8, r11, r8, lsl #0x5
    add r8, r8, ip, lsl #0xa
    orr r8, r8, #0x8000
    sub r4, r4, #0x4
    ldrb r9, [r7, r4]
    ldrb r11, [r6, r4]
    ldrb ip, [r5, r4]
    ldrb r4, [r0], #0x1
    add r8, r8, r9, lsl #0x15
    add r8, r8, r11, lsl #0x10
    add r8, r8, ip, lsl #0x1a
    orr r8, r8, #0x80000000
    str r8, [r2], #0x4
    sub r4, r4, #0x4
    ldrb r8, [r7, r4]
    ldrb r11, [r6, r4]
    ldrb ip, [r5, r4]
    ldrb r4, [r0], #-0xff
    add r8, r11, r8, lsl #0x5
    add r8, r8, ip, lsl #0xa
    orr r8, r8, #0x8000
    ldrb r9, [r7, r4]
    ldrb r11, [r6, r4]
    ldrb ip, [r5, r4]
    add r8, r8, r9, lsl #0x15
    add r8, r8, r11, lsl #0x10
    add r8, r8, ip, lsl #0x1a
    orr r8, r8, #0x80000000
    str r8, [lr], #0x4
    // block 6: the same, two columns on
    ldrb r5, [r1], #0x80
    ldrb r6, [r1], #-0x7f
    ldrb r4, [r0], #0x1
    sub r5, r5, #0x80
    sub r6, r6, #0x80
    add r7, r3, r6
    sub r6, r5, r6
    add r6, r6, r3
    sub r5, r6, r5, lsl #0x1
    ldrb r8, [r7, r4]
    ldrb r11, [r6, r4]
    ldrb ip, [r5, r4]
    ldrb r4, [r0], #0xff
    add r8, r11, r8, lsl #0x5
    add r8, r8, ip, lsl #0xa
    orr r8, r8, #0x8000
    sub r4, r4, #0x4
    ldrb r9, [r7, r4]
    ldrb r11, [r6, r4]
    ldrb ip, [r5, r4]
    ldrb r4, [r0], #0x1
    add r8, r8, r9, lsl #0x15
    add r8, r8, r11, lsl #0x10
    add r8, r8, ip, lsl #0x1a
    orr r8, r8, #0x80000000
    str r8, [r2], #0x4
    sub r4, r4, #0x4
    ldrb r8, [r7, r4]
    ldrb r11, [r6, r4]
    ldrb ip, [r5, r4]
    ldrb r4, [r0], #-0xff
    add r8, r11, r8, lsl #0x5
    add r8, r8, ip, lsl #0xa
    orr r8, r8, #0x8000
    ldrb r9, [r7, r4]
    ldrb r11, [r6, r4]
    ldrb ip, [r5, r4]
    add r8, r8, r9, lsl #0x15
    add r8, r8, r11, lsl #0x10
    add r8, r8, ip, lsl #0x1a
    orr r8, r8, #0x80000000
    str r8, [lr], #0x4
    // block 7: the same, two columns on
    ldrb r5, [r1], #0x80
    ldrb r6, [r1], #-0x7f
    ldrb r4, [r0], #0x1
    sub r5, r5, #0x80
    sub r6, r6, #0x80
    add r7, r3, r6
    sub r6, r5, r6
    add r6, r6, r3
    sub r5, r6, r5, lsl #0x1
    ldrb r8, [r7, r4]
    ldrb r11, [r6, r4]
    ldrb ip, [r5, r4]
    ldrb r4, [r0], #0xff
    add r8, r11, r8, lsl #0x5
    add r8, r8, ip, lsl #0xa
    orr r8, r8, #0x8000
    sub r4, r4, #0x4
    ldrb r9, [r7, r4]
    ldrb r11, [r6, r4]
    ldrb ip, [r5, r4]
    ldrb r4, [r0], #0x1
    add r8, r8, r9, lsl #0x15
    add r8, r8, r11, lsl #0x10
    add r8, r8, ip, lsl #0x1a
    orr r8, r8, #0x80000000
    str r8, [r2], #0x4
    sub r4, r4, #0x4
    ldrb r8, [r7, r4]
    ldrb r11, [r6, r4]
    ldrb ip, [r5, r4]
    ldrb r4, [r0], #-0xff
    add r8, r11, r8, lsl #0x5
    add r8, r8, ip, lsl #0xa
    orr r8, r8, #0x8000
    ldrb r9, [r7, r4]
    ldrb r11, [r6, r4]
    ldrb ip, [r5, r4]
    add r8, r8, r9, lsl #0x15
    add r8, r8, r11, lsl #0x10
    add r8, r8, ip, lsl #0x1a
    orr r8, r8, #0x80000000
    str r8, [lr], #0x4
    // block 8: the same, two columns on
    ldrb r5, [r1], #0x80
    ldrb r6, [r1], #-0x7f
    ldrb r4, [r0], #0x1
    sub r5, r5, #0x80
    sub r6, r6, #0x80
    add r7, r3, r6
    sub r6, r5, r6
    add r6, r6, r3
    sub r5, r6, r5, lsl #0x1
    ldrb r8, [r7, r4]
    ldrb r11, [r6, r4]
    ldrb ip, [r5, r4]
    ldrb r4, [r0], #0xff
    add r8, r11, r8, lsl #0x5
    add r8, r8, ip, lsl #0xa
    orr r8, r8, #0x8000
    sub r4, r4, #0x4
    ldrb r9, [r7, r4]
    ldrb r11, [r6, r4]
    ldrb ip, [r5, r4]
    ldrb r4, [r0], #0x1
    add r8, r8, r9, lsl #0x15
    add r8, r8, r11, lsl #0x10
    add r8, r8, ip, lsl #0x1a
    orr r8, r8, #0x80000000
    str r8, [r2], #0x4
    sub r4, r4, #0x4
    ldrb r8, [r7, r4]
    ldrb r11, [r6, r4]
    ldrb ip, [r5, r4]
    ldrb r4, [r0], #-0xff
    add r8, r11, r8, lsl #0x5
    add r8, r8, ip, lsl #0xa
    orr r8, r8, #0x8000
    ldrb r9, [r7, r4]
    ldrb r11, [r6, r4]
    ldrb ip, [r5, r4]
    add r8, r8, r9, lsl #0x15
    add r8, r8, r11, lsl #0x10
    add r8, r8, ip, lsl #0x1a
    orr r8, r8, #0x80000000
    str r8, [lr], #0x4
    subs r10, r10, #0x10            // 16 pixels done
    bgt pixel_block
    ldr r10, [sp], #4               // rows left
    // next row pair: advance Y, U/V and both output rows by the saved steps
    ldr r5, [sp, #0x0]
    ldr r6, [sp, #0x4]
    ldr r7, [sp, #0x8]
    add r0, r0, r5
    add r1, r1, r6
    add r2, r2, r7
    add lr, lr, r7
    subs r10, r10, #0x2             // two rows done
    bgt row_pair
    add sp, sp, #0x18               // drop the six saved values
    ldmfd sp!, {r4, r5, r6, r7, r8, r9, r10, r11, ip, pc}
}
// clang-format on

// Run-time copies of the decoder's tables and the buffers that hold them. A table is duplicated once, on first
// use, into the buffer given by the setters below; when the buffer is too small the table in place is used.
struct {
    u32 mUnk_0;
    u32 mUnk_4;
    u32 mUnk_8;
    u32 mUnk_c;
    u8 *mUnk_10;
    u32 mUnk_14;
    u8 *mUnk_18;
    u32 mUnk_1c;
    u32 mUnk_20;
} data_020ca680;

// Size of the buffer func_0203b53c takes.
u32 func_0203b528() {
    return 0x659c;
}

// Size of the buffer func_0203b55c takes: room for the three tables it holds.
u32 func_0203b534() {
    return 0x2580;
}

void func_0203b53c(u8 *p, u32 n) {
    data_020ca680.mUnk_18 = p;
    data_020ca680.mUnk_14 = n & ~3;
    data_020ca680.mUnk_8 = 0;
}

void func_0203b55c(u8 *p, u32 n) {
    data_020ca680.mUnk_10 = p;
    data_020ca680.mUnk_c = n & ~3;
    data_020ca680.mUnk_4 = 0;
    data_020ca680.mUnk_0 = 0;
    data_020ca680.mUnk_20 = 0;
}

void *func_0203b584() {
    if (data_020ca680.mUnk_8 == 0) {
        if (data_020ca680.mUnk_14 >= 0x659c) {
            data_020ca680.mUnk_8 = (u32) data_020ca680.mUnk_18;
            MIi_CpuCopyFast((void *) func_020583d4, data_020ca680.mUnk_18, 0x659c);
            data_020ca680.mUnk_18 += 0x659c;
            data_020ca680.mUnk_14 -= 0x659c;
        } else {
            return (void *) func_020583d4;
        }
    }
    return (void *) data_020ca680.mUnk_8;
}

void *func_0203b600() {
    if (data_020ca680.mUnk_4 == 0) {
        if (data_020ca680.mUnk_c >= 0x2100) {
            data_020ca680.mUnk_4 = (u32) data_020ca680.mUnk_10;
            MIi_CpuCopyFast(data_020562d4, data_020ca680.mUnk_10, 0x2100);
            data_020ca680.mUnk_10 += 0x2100;
            data_020ca680.mUnk_c -= 0x2100;
        } else {
            return data_020562d4;
        }
    }
    return (void *) data_020ca680.mUnk_4;
}

void *func_0203b670() {
    if (data_020ca680.mUnk_0 == 0) {
        if (data_020ca680.mUnk_c >= 0x300) {
            data_020ca680.mUnk_0 = (u32) data_020ca680.mUnk_10;
            MIi_CpuCopyFast(data_0205f03c, data_020ca680.mUnk_10, 0x300);
            data_020ca680.mUnk_10 += 0x300;
            data_020ca680.mUnk_c -= 0x300;
        } else {
            return data_0205f03c;
        }
    }
    return (void *) data_020ca680.mUnk_0;
}

void *func_0203b6e0() {
    if (data_020ca680.mUnk_20 == 0) {
        if (data_020ca680.mUnk_c >= 0x180) {
            data_020ca680.mUnk_20 = (u32) data_020ca680.mUnk_10;
            MIi_CpuCopyFast(data_0205eebc, data_020ca680.mUnk_10, 0x180);
            data_020ca680.mUnk_10 += 0x180;
            data_020ca680.mUnk_c -= 0x180;
        } else {
            return data_0205eebc;
        }
    }
    return (void *) data_020ca680.mUnk_20;
}

// Reads the predictor and step index at the start of a block of 4-bit audio samples.
void func_0203b750(AdSt *st, u8 *p) {
    st->idx = p[0];
    st->pred = *(s16 *) (p + 2);
}

// Applies one 4-bit sample n to the channel state.
#define DECODE_NIBBLE(st, n)                                                                                           \
    {                                                                                                                  \
        s16 step = data_0205f34c[(st)->idx];                                                                           \
        (st)->idx += data_0205f33c[(n) & 7];                                                                           \
        if ((st)->idx < 0)                                                                                             \
            (st)->idx = 0;                                                                                             \
        if ((st)->idx > 0x58)                                                                                          \
            (st)->idx = 0x58;                                                                                          \
        int diff = step >> 3;                                                                                          \
        if ((n) & 4)                                                                                                   \
            diff += step;                                                                                              \
        if ((n) & 2)                                                                                                   \
            diff += step >> 1;                                                                                         \
        if ((n) & 1)                                                                                                   \
            diff += step >> 2;                                                                                         \
        if ((n) & 8)                                                                                                   \
            (st)->pred -= diff;                                                                                        \
        else                                                                                                           \
            (st)->pred += diff;                                                                                        \
        if ((st)->pred < -0x8000)                                                                                      \
            (st)->pred = -0x8000;                                                                                      \
        if ((st)->pred > 0x7fff)                                                                                       \
            (st)->pred = 0x7fff;                                                                                       \
    }

// Decodes n bytes of 4-bit samples into n words of two 16-bit samples each.
void func_0203b764(AdSt *st, u8 *in, int n, u32 *out) {
    for (int i = 0; i < n; i++) {
        int b = in[i];
        DECODE_NIBBLE(st, b)
        int s1 = st->pred;
        b >>= 4;
        DECODE_NIBBLE(st, b)
        *out++ = (u16) s1 | (st->pred << 16);
    }
}
