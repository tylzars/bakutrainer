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
#ifdef __cplusplus
}
#endif
