// Side header of system_init.c: display layers, JPEG decoding, VRAM bank mapping and palette lookup.
#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Display layers: background control values, tile copies and image loading.
u32 func_0201db58(int layer);
int GetLayerWidth(int layer);
int GetLayerRowSize(int layer);
void CopyStripRows(u16 *src, u32 dst, u32 width, u32 stride, u8 *lut);
void CopyStripToLayers(u16 *src, u32 layers, u32 row, u32 width, u32 total, u8 *lut);
void CopyImageToLayer(u8 *src, u32 dst, u32 width, u32 rows);
void LoadImage(int id, int layers);
void LoadImageEx(int id, int layers, int paletteFile, u16 flags);
void LoadImageData(u8 *data, int layers, int paletteFile, u16 flags);

// JPEG decoding: the marker scan, the inverse DCT of one block and the decoder of one image.
u8 *func_0201e178(void);
void func_0201e19c(s16 *in, s16 *quant, u8 *out, u8 *clamp);
void func_0201e518(u8 *data, u32 *width, u32 *height);
void func_0201e56c(u8 *data, int layers, void *lut);

void ResetVramBanks(void);
void FreeVramBanks(int type);
void ReallocVramBanks(int type, int size, int addr);
void AllocVramBanks(int type, u32 size, u32 addr);

// Entry of the colour fill queue: a 5-bit colour (three components) and the palette index that reached it.
typedef struct Unk_ColorQueue {
    u8 mUnk_0;
    u8 mUnk_1;
    u8 mUnk_2;
    u8 mUnk_3;
} Unk_ColorQueue;

// Nearest-palette lookup: CreateNearestColorTable uploads the 256 colours of file `id` to the palettes of the layers
// in mask `layers` and returns a 0x8000-byte table indexed by a 15-bit colour (r + (g << 5) + (b << 10)) that holds
// the nearest of those 256 entries. Only LoadImageData calls it, for a nonzero paletteFile; every LoadImageEx call
// passes 0, so it never runs in this build.
u16 ColorDistanceSquared(u8 (*pal)[3], int c0, int c1, int c2, int index);
void *CreateNearestColorTable(int id, int layers);

#ifdef __cplusplus
}
#endif
