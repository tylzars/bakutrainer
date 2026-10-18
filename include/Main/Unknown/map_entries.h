#pragma once

#include "types.h"
#include <nitro/fx.h>

#ifdef __cplusplus
extern "C" {
#endif

// One 16-byte record of the stage data file's position chunk: the key, flags, group and a position kept as three
// 32-bit values split into halfwords.
typedef struct MapEntry {
    u16 mUnk_00;
    u8 mUnk_02;
    u8 mUnk_03;
    u16 mUnk_04[2];
    u16 mUnk_08[2];
    u16 mUnk_0c[2];
} MapEntry;

// The chunk of the stage data file whose key is id; 0 when the file has no such chunk.
u16 *func_02031eb8(u32 id);

void *func_02031c70(void *p);
int func_02031c98(u16 *entry, u16 mask, u16 bits);
int func_02031d0c(VecFx32 *pos, int a, int b, int c);
int func_02031dc4(s32 x, s32 z);
s32 func_02031f98(MapEntry *entry);
s32 func_02031fa4(MapEntry *entry);
s32 func_02031fb0(MapEntry *entry);
u16 func_02031fbc(MapEntry *entry);
void func_02031fc4(u32 id, VecFx32 *out);
void func_02032018(u32 index, VecFx32 *pos, u8 *flags);
MapEntry *func_0203204c(u32 a, u8 *count);
int func_020320c0(int a, int b, VecFx32 *pos);

// Defined further on in this file.
// The cell table of chunk 0x5510 (terminates when the chunk is missing).
u16 *func_02032898(void);
// The vertex index list of one polygon of chunk 0x5516; *count receives the vertex count.
u16 *func_020321ac(int a, int b, u32 *count);
// Stores up to `max` polygon keys matching (a, b) in out; returns how many.
u32 func_02032244(int a, int b, u16 *out, u32 max);
// Clears the 20 records of data_020c3e44 (and the camera's halfwords at 0x24/0x26 when a == 1).
void func_02032b88(int a);

#ifdef __cplusplus
}
#endif
