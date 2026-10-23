#include <nitro/mi.h>

#define MI_DMA_ENABLE 0x80000000
#define MI_DMA_START_CARD 0x28000000
#define MI_DMA_REPEAT 0x02000000
#define MI_DMA_32BIT 0x04000000
#define MI_DMA_SRC_FIXED 0x01000000
#define MI_DMA_AUTO_MODES_ALL 0xffffffff

// Card-slot DMA: starts channel dmaNo with a one-word, repeating, source-fixed transfer paced by the cartridge
// slot. size only feeds the channel 0 source-address check.
void func_02040040(u32 dmaNo, const void *src, void *dest, u32 size) {
    vu32 *dmaCntp;
    MIi_CheckAnotherAutoDMA(dmaNo, MI_DMA_AUTO_MODES_ALL);
    MIi_CheckDma0SourceAddress(dmaNo, (u32) src, size, MI_DMA_SRC_FIXED);
    if (size == 0) {
        return;
    }
    dmaCntp = (vu32 *) (0x4000000 + (dmaNo * 3 + 2) * 4 + 0xb0);
    while (*dmaCntp & MI_DMA_ENABLE) {
    }
    func_01ff81e4(dmaNo, (u32) src, (u32) dest,
                  1 | (MI_DMA_ENABLE | MI_DMA_START_CARD | MI_DMA_REPEAT | MI_DMA_32BIT | MI_DMA_SRC_FIXED));
}
