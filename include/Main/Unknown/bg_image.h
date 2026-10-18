#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Display layers: background control values, tile copies and image loading.
u32 func_0201db58(int layer);
int func_0201db94(int layer);
int func_0201dbd4(int layer);
void func_0201dbf4(u16 *src, u32 dst, u32 width, u32 stride, u8 *lut);
void func_0201dcc0(u16 *src, u32 layers, u32 row, u32 width, u32 total, u8 *lut);
void func_0201dd74(u8 *src, u32 dst, u32 width, u32 rows);
void func_0201dde0(int id, int target);
void func_0201ddec(int id, int target, int x, int flags);
void func_0201de48(u8 *data, int target, int x, int flags);

// Sizes by layer mode (read by func_0201db94).
extern const u16 data_02050200[4];

// JPEG decoding: the marker scan, the inverse DCT of one block and the decoder of one image.
u8 *func_0201e178(void);
void func_0201e19c(s16 *in, s16 *quant, u8 *out, u8 *clamp);
void func_0201e518(u8 *data, u32 *width, u32 *height);
void func_0201e56c(u8 *data, int target, void *lut);

// VRAM bank mapping: banks in use, freeing the banks of one use and mapping banks for one.
void func_0201eb20(void);
void func_0201eb74(int type);
void func_0201ebf4(int layer, int a, int b);
void func_0201ec2c(int type, u32 size, u32 addr);

// Entry of the colour fill queue: a 5-bit colour (three components) and the palette index that reached it.
typedef struct Unk_ColorQueue {
    u8 mUnk_0;
    u8 mUnk_1;
    u8 mUnk_2;
    u8 mUnk_3;
} Unk_ColorQueue;

// Nearest-palette lookup over the 15-bit colour cube.
u16 func_0201eea4(u8 (*pal)[3], int c0, int c1, int c2, int index);
void *func_0201eee4(int x, int target);

#ifdef __cplusplus
}
#endif
