#pragma once

#include "types.h"
#include <Main/System/system_init.h>

#ifdef __cplusplus
extern "C" {
#endif

// Header of one texture block (GetTexInfo fills it).
typedef struct TexInfo {
    u32 mUnk_00;
    u32 mUnk_04;
    s16 mX;
    s16 mY;
    s16 mWidth;
    s16 mHeight;
    // The block's record count; nothing reads it in this build.
    u8 mRecordCount;
    u8 mSizeCode;
    s8 mUnk_12;
    u8 mUnk_13;
    u8 mUnk_14;
    u8 mUnk_15;
    u8 mUnk_16[2];
} TexInfo;

// Width and height in pixels of each texture size code (index = size code, [0] = width, [1] = height).
extern const u16 data_02050978[16];
extern const u8 data_02050998[16][2];

// One texture record of a block: a packed word (two signed 9-bit values, a 3-bit and a 4-bit code, a 7-bit
// value) and a second word whose low 24 bits are an offset and whose high byte holds two 4-bit values.
typedef struct TexRec {
    s32 mUnk_00_0 : 9;
    s32 mUnk_00_9 : 9;
    u32 mUnk_00_18 : 3;
    u32 mUnk_00_21 : 4;
    u32 mUnk_00_25 : 7;
    u32 mUnk_04;
} TexRec;

void func_0202ad34(void);
void func_0202ad38(void);
void func_0202ad3c(void);
int func_0202ad40(void);
void func_0202ad48(int a);
void func_0202ad4c(void);
void func_0202ad50(void);
void func_0202ad54(void);
u16 GetPaletteFile(void);
u16 GetBlockFile(void);
u8 GetUnitShift(int layer);
int func_0202ad9c(void);
s8 func_0202adac(void);
u32 *GetBlockHeader(u32 index);
u16 GetBlockCount(void);
u32 GetOamFillCount(int layer);
void ResetOam(int layer);
void GetTexInfo(u32 index, TexInfo *out, u32 sub);
u16 FreeLayerBlock(u32 index, int layer);
void RemoveLayerKey(int layer, u32 key);
u16 RemoveLeastRecentLayerCacheEntry(u32 index, int layer);
u32 GetSizeCodeBytes(u32 index, int mode);
u16 FindOrLoadLayerBlock(u32 code, u32 key, u32 sub, u32 mode, u32 count, int layer);
void ResetTextureLayer(int layer);
void SetupTextureLayer(int a, int b, int c, u32 d, int e);
void ReleaseTextureLayers(void);
void ReleaseTextureLayersEx(int a);
void func_0202c2f0(void *dst, const void *src, u32 w, u32 h, BOOL mode);
void func_0202c3e0(Transfer *entry, const void *src, u32 w);
void MoveChainEntryToListEnd(u32 i, TextureLayer *half);
u32 FindChainEntry(u32 key, u32 val, TextureLayer *half);
u32 func_0202c5fc(u32 index, u32 key, u32 val, TextureLayer *half);
s32 func_0202c778(u16 *keys, int a, u32 n, int engine);
s32 func_0202ca80(u16 *ids, int flag, u32 count, int mode);
void func_0202cce0(u32 obj, u32 rec, u8 *out);
u8 GetMatCount(int layer);
u32 FindOrAddMat(int angle, int sx, int sy, u32 flags);
void DrawSprite(int id, int x, int y, int w, int mode);
void DrawSpriteEx(int id, int x, int y, int w, int angle, int sx, int sy, int mode);
int GetSpritePoint(u16 flags, int index, s32 *out);
int GetSpriteRect(int obj, int a, s32 *rect);
void WriteOam(int a);

#ifdef __cplusplus
}
#endif
