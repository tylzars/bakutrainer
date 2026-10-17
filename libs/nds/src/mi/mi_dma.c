#include <nitro/os.h>
#include <nitro/mi.h>

#define MI_DMA_BASE 0x4000000
#define MI_DMA_CNT(n) (*(vu32 *) (MI_DMA_BASE + ((n) * 3 + 2) * 4 + 0xb0))
#define MI_DMA_FILL(n) (*(vu32 *) (MI_DMA_BASE + (n) * 4 + 0xe0))

#define MI_DMA_ENABLE 0x80000000
#define MI_DMA_IF_ENABLE 0x40000000
#define MI_DMA_32BIT 0x04000000
#define MI_DMA_SRC_FIXED 0x01000000

// Control word of the 1-unit transfer written to DMA 0 after a stop.
#define MI_DMA0_DUMMY_CNT 0x81400001

void MI_DmaFill32(u32 dmaNo, void *dest, u32 data, u32 size) {
    vu32 *dmaCntp;
    if (size == 0) {
        return;
    }
    dmaCntp = &MI_DMA_CNT(dmaNo);
    while (*dmaCntp & MI_DMA_ENABLE) {
    }
    {
        OSIntrMode e = OS_DisableInterrupts();
        MI_DMA_FILL(dmaNo) = data;
        func_01ff82b8(dmaNo, (u32) &MI_DMA_FILL(dmaNo), (u32) dest,
                      (size / 4) | (MI_DMA_ENABLE | MI_DMA_32BIT | MI_DMA_SRC_FIXED));
        OS_RestoreInterrupts(e);
    }
    while (*dmaCntp & MI_DMA_ENABLE) {
    }
}

void MI_DmaCopy32(u32 dmaNo, const void *src, void *dest, u32 size) {
    vu32 *dmaCntp;
    MIi_CheckDma0SourceAddress(dmaNo, (u32) src, size, 0);
    if (size == 0) {
        return;
    }
    dmaCntp = &MI_DMA_CNT(dmaNo);
    while (*dmaCntp & MI_DMA_ENABLE) {
    }
    func_01ff8224(dmaNo, (u32) src, (u32) dest, (size / 4) | (MI_DMA_ENABLE | MI_DMA_32BIT));
    while (*dmaCntp & MI_DMA_ENABLE) {
    }
}

void MI_DmaCopy16(u32 dmaNo, const void *src, void *dest, u32 size) {
    vu32 *dmaCntp;
    if (size == 0) {
        return;
    }
    MIi_CheckDma0SourceAddress(dmaNo, (u32) src, size, 0);
    dmaCntp = &MI_DMA_CNT(dmaNo);
    while (*dmaCntp & MI_DMA_ENABLE) {
    }
    func_01ff8224(dmaNo, (u32) src, (u32) dest, (size / 2) | MI_DMA_ENABLE);
    while (*dmaCntp & MI_DMA_ENABLE) {
    }
}

void MI_DmaFill32Async(u32 dmaNo, void *dest, u32 data, u32 size, void (*callback)(void *), void *arg) {
    if (size == 0) {
        if (callback != NULL) {
            callback(arg);
        }
        return;
    }
    func_0203fa68(dmaNo);
    if (callback != NULL) {
        OSIntrMode e;
        OSi_EnterDmaCallback(dmaNo, callback, arg);
        e = OS_DisableInterrupts();
        MI_DMA_FILL(dmaNo) = data;
        func_01ff8294(dmaNo, (u32) &MI_DMA_FILL(dmaNo), (u32) dest,
                      (size / 4) | (MI_DMA_ENABLE | MI_DMA_IF_ENABLE | MI_DMA_32BIT | MI_DMA_SRC_FIXED));
        OS_RestoreInterrupts(e);
    } else {
        OSIntrMode e = OS_DisableInterrupts();
        MI_DMA_FILL(dmaNo) = data;
        func_01ff8294(dmaNo, (u32) &MI_DMA_FILL(dmaNo), (u32) dest,
                      (size / 4) | (MI_DMA_ENABLE | MI_DMA_32BIT | MI_DMA_SRC_FIXED));
        OS_RestoreInterrupts(e);
    }
}

// Waits for the channel to finish; for channel 0 also writes the dummy transfer registers.
void func_0203fa68(u32 dmaNo) {
    OSIntrMode e = OS_DisableInterrupts();
    vu32 *dmaCntp = &MI_DMA_CNT(dmaNo);
    while (*dmaCntp & MI_DMA_ENABLE) {
    }
    if (dmaNo == 0) {
        vu32 *dma = (vu32 *) (MI_DMA_BASE + dmaNo * 12 + 0xb0);
        dma[0] = 0;
        dma[1] = 0;
        dma[2] = MI_DMA0_DUMMY_CNT;
    }
    OS_RestoreInterrupts(e);
}

void MI_StopDma(u32 dmaNo) {
    OSIntrMode e = OS_DisableInterrupts();
    vu16 *cntH = (vu16 *) (MI_DMA_BASE + (dmaNo * 6 + 5) * 2 + 0xb0);
    *cntH &= ~0x3a00;
    *cntH &= ~0x8000;
    (void) *cntH;
    (void) *cntH;
    if (dmaNo == 0) {
        vu32 *dma = (vu32 *) (MI_DMA_BASE + dmaNo * 12 + 0xb0);
        dma[0] = 0;
        dma[1] = 0;
        dma[2] = MI_DMA0_DUMMY_CNT;
    }
    OS_RestoreInterrupts(e);
}

void MI_StopAllDma(void) {
    MI_StopDma(0);
    MI_StopDma(1);
    MI_StopDma(2);
    MI_StopDma(3);
}

void MIi_CheckAnotherAutoDMA(u32 dmaNo, u32 mode) {
    int i;
    vu32 *cntp = (vu32 *) 0x40000b8;
    for (i = 0; i < 3; i++, cntp += 3) {
        u32 cnt;
        if (i == dmaNo) {
            continue;
        }
        cnt = *cntp;
        if (!(cnt & MI_DMA_ENABLE)) {
            continue;
        }
        cnt &= 0x38000000;
        if (cnt == mode) {
            continue;
        }
        if (cnt == 0x8000000 && mode == 0x10000000) {
            continue;
        }
        if (cnt == 0x10000000 && mode == 0x8000000) {
            continue;
        }
        if (cnt == 0x18000000 || cnt == 0x20000000 || cnt == 0x28000000 || cnt == 0x30000000
            || cnt == 0x38000000 || cnt == 0x8000000 || cnt == 0x10000000) {
            OS_Terminate();
        }
    }
}

void MIi_CheckDma0SourceAddress(u32 dmaNo, u32 src, u32 size, u32 dir) {
    u32 srcHi;
    u32 endHi;
    if (dmaNo != 0) {
        return;
    }
    srcHi = src & 0xff000000;
    if (dir != 0) {
        if (dir == 0x800000) {
            src -= size;
        }
    } else {
        src += size;
    }
    endHi = src & 0xff000000;
    if (srcHi == 0x4000000 || srcHi >= 0x8000000 || endHi == 0x4000000 || endHi >= 0x8000000) {
        OS_Terminate();
    }
}
