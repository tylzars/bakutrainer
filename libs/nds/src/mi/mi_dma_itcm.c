// DMA register set-up helpers and the geometry FIFO command sender (ITCM).

#include <nitro/os.h>
#include <nitro/mi.h>

#define DMA_BASE 0x040000b0
#define DMA0_SRC (*(vu32 *) DMA_BASE)
#define GXFIFO_ADDR 0x04000400

// Programs the source, destination and control words of DMA channel n with interrupts disabled.
void func_01ff81e4(u32 n, u32 src, u32 dst, u32 cnt) {
    u32 old = OS_DisableInterrupts();
    vu32 *d = (vu32 *) (DMA_BASE + n * 12);
    d[0] = src;
    d[1] = dst;
    d[2] = cnt;
    OS_RestoreInterrupts(old);
}

// As func_01ff81e4, then (for channel 0) reads the DMA 0 source register twice and re-arms the
// channel with a zero source and destination and control word 0x81400001.
void func_01ff8224(u32 n, u32 src, u32 dst, u32 cnt) {
    u32 old = OS_DisableInterrupts();
    vu32 *d = (vu32 *) (DMA_BASE + n * 12);
    d[0] = src;
    d[1] = dst;
    d[2] = cnt;
    (void) DMA0_SRC;
    (void) DMA0_SRC;
    if (n == 0) {
        d[0] = 0;
        d[1] = 0;
        d[2] = 0x81400001;
    }
    OS_RestoreInterrupts(old);
}

// Programs the three words of DMA channel n; interrupts are left alone.
void func_01ff8294(u32 n, u32 src, u32 dst, u32 cnt) {
    vu32 *d = (vu32 *) (DMA_BASE + n * 12);
    d[0] = src;
    d[1] = dst;
    d[2] = cnt;
}

// As func_01ff8294, then the channel 0 read and re-arm sequence of func_01ff8224, with two more
// reads of the DMA 0 source register afterwards.
void func_01ff82b8(u32 n, u32 src, u32 dst, u32 cnt) {
    vu32 *d = (vu32 *) (DMA_BASE + n * 12);
    d[0] = src;
    d[1] = dst;
    d[2] = cnt;
    (void) DMA0_SRC;
    (void) DMA0_SRC;
    if (n == 0) {
        d[0] = 0;
        d[1] = 0;
        d[2] = 0x81400001;
    }
    (void) DMA0_SRC;
    (void) DMA0_SRC;
}

// Sends commandLength bytes at src to the geometry FIFO through DMA channel dmaNo, at most 0x1d8
// bytes per transfer, and waits for the channel to finish.
void func_01ff831c(int dmaNo, const void *src, u32 commandLength) {
    vu32 *cnt;
    u32 chunk;
    if (commandLength == 0) {
        return;
    }
    MIi_CheckDma0SourceAddress(dmaNo, (u32) src, commandLength, 0);
    cnt = &((vu32 *) DMA_BASE)[dmaNo * 3 + 2];
    while (*cnt & 0x80000000) {
    }
    while (commandLength != 0) {
        if (commandLength > 0x1d8) {
            chunk = 0x1d8;
        } else {
            chunk = commandLength;
        }
        func_01ff81e4(dmaNo, (u32) src, GXFIFO_ADDR, 0x84400000 | (chunk >> 2));
        commandLength -= chunk;
        src = (const u8 *) src + chunk;
    }
    while (*cnt & 0x80000000) {
    }
}
