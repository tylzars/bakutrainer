#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// DMA (mi_dma.c)
void MI_DmaFill32(u32 dmaNo, void *dest, u32 data, u32 size);
void MI_DmaCopy32(u32 dmaNo, const void *src, void *dest, u32 size);
void MI_DmaCopy16(u32 dmaNo, const void *src, void *dest, u32 size);
void MI_DmaFill32Async(u32 dmaNo, void *dest, u32 data, u32 size, void (*callback)(void *), void *arg);
void func_0203fa68(u32 dmaNo);
void MI_StopDma(u32 dmaNo);
void MI_StopAllDma(void);
void MIi_CheckAnotherAutoDMA(u32 dmaNo, u32 mode);
void MIi_CheckDma0SourceAddress(u32 dmaNo, u32 src, u32 size, u32 dir);

// DMA register set-up and the geometry FIFO sender (mi_dma_itcm.c, in ITCM)
void func_01ff81e4(u32 dmaNo, u32 src, u32 dest, u32 ctrl);
void func_01ff8224(u32 dmaNo, u32 src, u32 dest, u32 ctrl);
void func_01ff8294(u32 dmaNo, u32 src, u32 dest, u32 ctrl);
void func_01ff82b8(u32 dmaNo, u32 src, u32 dest, u32 ctrl);
void func_01ff831c(int dmaNo, const void *src, u32 commandLength);

// CPU copies and fills (mi_memory.c)
void MIi_CpuClear16(u16 data, void *dest, u32 size);
void MIi_CpuCopy16(const void *src, void *dest, u32 size);
void MIi_CpuClear32(u32 data, void *dest, u32 size);
void MIi_CpuCopy32(const void *src, void *dest, u32 size);
void MIi_CpuClearFast(u32 data, void *dest, u32 size);

static inline void MI_CpuClearFast(void *dest, u32 size) {
    MIi_CpuClearFast(0, dest, size);
}
void MIi_CpuCopyFast(const void *src, void *dest, u32 size);
void func_0203fda0(const void *src, void *dest);
void MI_Copy36B(const void *src, void *dest);
void func_0203fdd8(const void *src, void *dest);
void MI_CpuFill8(void *dest, u8 data, u32 size);
void MI_CpuCopy8(const void *src, void *dest, u32 size);

static inline void MI_CpuClear32(void *dest, u32 size) {
    MIi_CpuClear32(0, dest, size);
}

// Swap (mi_swap.c)
u32 MI_SwapWord(u32 setData, vu32 *destp);

// Card-slot DMA (mi_dma_card.c)
void func_02040040(u32 dmaNo, const void *src, void *dest, u32 size);

// Start-up of the MI state (mi_init.c)
void func_020400b4(void);

// Work RAM (mi_wram.c)
void MI_SetWramBank(u8 bank);

#ifdef __cplusplus
}
#endif
