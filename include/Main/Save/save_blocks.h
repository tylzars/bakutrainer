// Side header of system_init.c: its save blocks (tags, checksums, packing).
#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Store kinds accepted by InitSaveStore: a store of 0x200 bytes and one of 0x2000 bytes, split evenly into the set's
// blocks.
#define SAVE_STORE_SMALL 0x901
#define SAVE_STORE_LARGE 0xd01

u16 GetSaveBlockTag(int id, u32 index);
void InitSaveStore(u32 a, int b, u16 c, int d, void (*fn)(u32), int e);
u32 ReadSaveBlock(u32 index, void *buf, int c);
void WriteZeroSaveBlock(u32 index, int c);
void WriteSaveBlock(u32 index, void *buf, int c);
void ClearBackupTimer(void);
void SortIndicesByKey(u32 *order, const u32 *key, int count);
int PackSaveFields(const u32 *values, u32 *out, u32 id);
void UnpackSaveFields(u32 *out, u32 *packed, u32 id);
void SetLcdSwap(int a);

#ifdef __cplusplus
}
#endif
