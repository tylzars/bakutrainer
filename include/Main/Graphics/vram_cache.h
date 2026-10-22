#pragma once

#include "types.h"
#include <nitro/fx.h>
#include <Main/System/system_init.h>

#ifdef __cplusplus
extern "C" {
#endif

u32 FreeTexBlock(u32 i);
u32 RemoveLeastRecentTexCacheEntry(u32 i);
u32 AllocTexBlock(Unk_PoolRange *n, u32 size, u32 node);
u32 FindOrLoadTex(u32 kind, u32 key, u32 size, u32 srcSize);
u32 FreePlttBlock(u32 i);
u32 RemoveLeastRecentPlttCacheEntry(u32 i);
u32 AllocPlttBlock(Unk_PoolRange *n, u32 size, u32 node);
u32 FindOrLoadPltt(u32 kind, u32 key, u32 size);
void ApplyAnimTrack(u32 track, u16 *out, s32 *pos, s32 *vec, u32 nodeIndex, int *done, const PoseNode *node);
void ReadAnimFileNode(u16 file, u16 *rot, VecFx32 *pos, VecFx32 *vec, int frame, int nextFrame, int weight, int node);

#ifdef __cplusplus
}
#endif
