#include <Main/System/system_init.h>
#include <Main/System/screen_fade.h>
#include <nitro/math.h>
#include <nitro/os.h>
#include <Main/System/random_angle.h>
#include <Main/Graphics/vram_cache.h>
#include <Main/Graphics/model.h>
#include <Main/Graphics/texture_blocks.h>
#include <Main/System/file_loader.h>

#pragma thumb off

// Frees the run of 512-byte VRAM blocks that starts at block i of the texture cache's heap, merges it with free
// neighbours, links it into the free list ordered by size and returns the merged run's length in blocks.
u32 FreeTexBlock(u32 i) {
    Unk_PoolRange *n = gGxState.mTexBlocks;
    u32 q;
    u32 j;
    u32 k;
    u32 size;

    n[i].mUnk_0 &= ~POOL_BLOCK_USED;
    j = i + n[i].mUnk_0;
    if (j < gGxState.mTexBlockCount && !(n[j].mUnk_0 & POOL_BLOCK_USED)) {
        n[i].mUnk_0 += n[j].mUnk_0;
        n[i + n[i].mUnk_0].mUnk_6 = i;
        n[n[j].mNext].mPrev = n[j].mPrev;
        n[n[j].mPrev].mNext = n[j].mNext;
    }
    k = n[i].mUnk_6;
    if (k < gGxState.mTexBlockCount && !(n[k].mUnk_0 & POOL_BLOCK_USED)) {
        n[k].mUnk_0 += n[i].mUnk_0;
        n[k + n[k].mUnk_0].mUnk_6 = k;
        n[n[k].mNext].mPrev = n[k].mPrev;
        n[n[k].mPrev].mNext = n[k].mNext;
        i = k;
    }
    size = n[i].mUnk_0;
    if (size < 4) {
        j = gGxState.mTexBlockCount + 1;
        q = n[j].mPrev;
        while (size > n[q].mUnk_0) {
            j = q;
            q = n[q].mPrev;
        }
        n[i].mPrev = q;
        n[i].mNext = j;
        n[j].mPrev = i;
        n[q].mNext = i;
    } else {
        j = gGxState.mTexBlockCount;
        q = n[j].mNext;
        while (size < n[q].mUnk_0) {
            j = q;
            q = n[q].mNext;
        }
        n[i].mNext = q;
        n[i].mPrev = j;
        n[j].mNext = i;
        n[q].mPrev = i;
    }
    return n[i].mUnk_0;
}

// Removes the least recently used entry of the texture cache, node i, which must be node 1's mNext (as at both
// callers; any nodes between them would be unlinked too): marks its hash slot removed (-2), frees its blocks and puts
// the node on the free chain headed by node 1's mPrev. Returns the length of the merged free run in 512-byte blocks.
u32 RemoveLeastRecentTexCacheEntry(u32 i) {
    u32 result;

    gGxState.mTexEntryCount--;
    gGxState.mTexSlots[gGxState.mTexNodes[i].mSlotIndex].mKey = -2;
    result = FreeTexBlock(gGxState.mTexNodes[i].mOffset >> 9);
    gGxState.mTexNodes[1].mNext = gGxState.mTexNodes[i].mNext;
    gGxState.mTexNodes[gGxState.mTexNodes[i].mNext].mPrev = 1;
    gGxState.mTexNodes[i].mPrev = gGxState.mTexNodes[1].mPrev;
    gGxState.mTexNodes[1].mPrev = i;
    return result;
}

// Allocates a run of 512-byte VRAM blocks for `size` bytes from the texture cache's heap: takes the smallest free run
// that fits (removing least recently used entries until one does), marks it used, stores its byte offset in node
// `node`, gives the rest of the run back to the free list and returns the offset.
u32 AllocTexBlock(Unk_PoolRange *n, u32 size, u32 node) {
    unsigned long q;
    int found;
    u32 p;
    u32 spare;
    u32 next;
    u32 addr;
    u32 victim;

    size = (size >> 9) + ((size & 0x1ff) != 0);
    for (;;) {
        q = n[(gSystemState.mTexVramGroup.mSize16Kb << 14) / 512].mNext;
        found = -1;
        while (size <= n[q].mUnk_0) {
            found = q;
            q = n[q].mNext;
        }
        if (found != -1) {
            break;
        }
        victim = 1;
        do {
            victim = gGxState.mTexNodes[victim].mNext;
        } while (RemoveLeastRecentTexCacheEntry(victim) < size);
    }
    spare = n[found].mUnk_0;
    n[found].mUnk_0 = size + POOL_BLOCK_USED;
    addr = found << 9;
    gGxState.mTexNodes[node].mOffset = addr;
    p = n[found].mPrev;
    n[n[found].mNext].mPrev = p;
    n[n[found].mPrev].mNext = n[found].mNext;
    if (spare - size == 0) {
        return addr;
    }
    next = found + size;
    n[next].mUnk_0 = spare - size;
    n[next].mUnk_6 = found;
    n[next + (spare - size)].mUnk_6 = next;
    while (spare - size < n[q].mUnk_0) {
        p = q;
        q = n[q].mNext;
    }
    n[next].mNext = q;
    n[next].mPrev = p;
    n[p].mNext = next;
    n[q].mPrev = next;
    return addr;
}

// Returns the VRAM byte offset of texture `key`, looked up in the texture cache's hash table; a hit makes the entry the
// most recently used. On a miss it takes a free node (removing the least recently used entry once there are more
// than 0x2bc), allocates blocks for `size` bytes and queues the transfer, so the offset is returned before the data
// arrives. `kind` picks the source: 1 a texture of the block file, 2 the file gSystemState.mUnk_42c, any other value
// the data's offset in mTexFile (`srcSize` is the stored size for kinds other than 1; 0 means `size`). Counts calls
// and probes in mTexLookupCount and mTexProbeCount.
u32 FindOrLoadTex(u32 kind, u32 key, u32 size, u32 srcSize) {
    u32 idx;
    int first = -1;
    u32 node;
    u32 step;
    u32 off;
    TexInfo info;

    step = gHashSteps[(key >> 10) & 0x7f];
    idx = key & 0x3ff;
    gGxState.mTexLookupCount++;
    for (;;) {
        gGxState.mTexProbeCount++;
        if (key == gGxState.mTexSlots[idx].mKey) {
            node = gGxState.mTexSlots[idx].mNodeIndex;
            MoveIndexNode16ToEnd((IndexNode16 *) gGxState.mTexNodes, node);
            return gGxState.mTexNodes[node].mOffset;
        }
        if (gGxState.mTexSlots[idx].mKey == -2 && first == -1) {
            first = idx;
        } else if (gGxState.mTexSlots[idx].mKey == -1 || (gGxState.mTexSlots[idx].mKey == -2 && idx == first)) {
            u16 count = gGxState.mTexEntryCount + 1;

            gGxState.mTexEntryCount = count;
            if (count > 0x2bc) {
                RemoveLeastRecentTexCacheEntry(gGxState.mTexNodes[1].mNext);
            }
            if (srcSize == 0) {
                srcSize = size;
            }
            node = gGxState.mTexNodes[1].mPrev;
            gGxState.mTexNodes[1].mPrev = gGxState.mTexNodes[node].mPrev;
            gGxState.mTexNodes[node].mPrev = gGxState.mTexNodes[0].mPrev;
            gGxState.mTexNodes[node].mNext = 0;
            gGxState.mTexNodes[0].mPrev = node;
            gGxState.mTexNodes[gGxState.mTexNodes[node].mPrev].mNext = node;
            if (first != -1) {
                idx = first;
            }
            gGxState.mTexSlots[idx].mKey = key;
            gGxState.mTexSlots[idx].mNodeIndex = node;
            gGxState.mTexNodes[node].mSlotIndex = idx;
            gGxState.mTexNodes[node].mOffset = AllocTexBlock(gGxState.mTexBlocks, size, node);
            if (kind == 1) {
                u32 shift;
                u8 flag;
                s16 bits;

                off = key - GetTexFileKeyCount();
                GetTexInfo(off >> 6, &info, off & 0x3f);
                flag = (info.mUnk_12 != 1 && info.mUnk_12 != 4) ? 0xff : 0;
                if (gTextureBlocks.mBlockFileKind == 2) {
                    kind = (info.mDataOffset << 7) + info.mUnk_13;
                } else {
                    kind = info.mDataOffset << 5;
                }
                bits = info.mWidth;
                shift = 32 - CountLeadingZeros(~bits & (bits - 1));
                QueueTransferEx(GetBlockFile(), kind, gGxState.mTexNodes[node].mOffset | 0xa0000000, size, 0, shift,
                                flag);
            } else if (kind == 2) {
                off = key - func_0201c550();
                QueueTransferEx(gSystemState.mUnk_42c, off, gGxState.mTexNodes[node].mOffset | 0xa0000000, srcSize,
                                size, 0, 0);
            } else {
                QueueTransferEx(gGxState.mTexFile, kind, gGxState.mTexNodes[node].mOffset | 0xa0000000, srcSize, size,
                                0, 0);
            }
            gSystemState.mUnk_2bc += size;
            gSystemState.mUnk_2b8 += size;
            return gGxState.mTexNodes[node].mOffset;
        }
        idx = (idx + step) & 0x3ff;
    }
}

u32 FreePlttBlock(u32 i) {
    Unk_PoolRange *n = gGxState.mPlttBlocks;
    u32 q;
    u32 j;
    u32 k;
    u32 size;

    n[i].mUnk_0 &= ~POOL_BLOCK_USED;
    j = i + n[i].mUnk_0;
    if (j < gGxState.mPlttBlockCount && !(n[j].mUnk_0 & POOL_BLOCK_USED)) {
        n[i].mUnk_0 += n[j].mUnk_0;
        n[i + n[i].mUnk_0].mUnk_6 = i;
        n[n[j].mNext].mPrev = n[j].mPrev;
        n[n[j].mPrev].mNext = n[j].mNext;
    }
    k = n[i].mUnk_6;
    if (k < gGxState.mPlttBlockCount && !(n[k].mUnk_0 & POOL_BLOCK_USED)) {
        n[k].mUnk_0 += n[i].mUnk_0;
        n[k + n[k].mUnk_0].mUnk_6 = k;
        n[n[k].mNext].mPrev = n[k].mPrev;
        n[n[k].mPrev].mNext = n[k].mNext;
        i = k;
    }
    size = n[i].mUnk_0;
    if (size < 4) {
        j = gGxState.mPlttBlockCount + 1;
        q = n[j].mPrev;
        while (size > n[q].mUnk_0) {
            j = q;
            q = n[q].mPrev;
        }
        n[i].mPrev = q;
        n[i].mNext = j;
        n[j].mPrev = i;
        n[q].mNext = i;
    } else {
        j = gGxState.mPlttBlockCount;
        q = n[j].mNext;
        while (size < n[q].mUnk_0) {
            j = q;
            q = n[q].mNext;
        }
        n[i].mNext = q;
        n[i].mPrev = j;
        n[j].mNext = i;
        n[q].mPrev = i;
    }
    return n[i].mUnk_0;
}

u32 RemoveLeastRecentPlttCacheEntry(u32 i) {
    u32 result;

    gGxState.mPlttEntryCount--;
    gGxState.mPlttSlots[gGxState.mPlttNodes[i].mSlotIndex].mKey = -2;
    result = FreePlttBlock(gGxState.mPlttNodes[i].mOffset >> 5);
    gGxState.mPlttNodes[1].mNext = gGxState.mPlttNodes[i].mNext;
    gGxState.mPlttNodes[gGxState.mPlttNodes[i].mNext].mPrev = 1;
    gGxState.mPlttNodes[i].mPrev = gGxState.mPlttNodes[1].mPrev;
    gGxState.mPlttNodes[1].mPrev = i;
    return result;
}

// Counted in 32-byte VRAM blocks.
u32 AllocPlttBlock(Unk_PoolRange *n, u32 size, u32 node) {
    unsigned long q;
    int found;
    u32 p;
    u32 spare;
    u32 next;
    u32 addr;
    u32 victim;

    size = (size >> 5) + ((size & 0x1f) != 0);
    for (;;) {
        q = n[(gSystemState.mPlttVramGroup.mSize16Kb << 14) / 32].mNext;
        found = -1;
        while (size <= n[q].mUnk_0) {
            found = q;
            q = n[q].mNext;
        }
        if (found != -1) {
            break;
        }
        victim = 1;
        do {
            victim = gGxState.mPlttNodes[victim].mNext;
        } while (RemoveLeastRecentPlttCacheEntry(victim) < size);
    }
    spare = n[found].mUnk_0;
    n[found].mUnk_0 = size + POOL_BLOCK_USED;
    addr = found << 5;
    gGxState.mPlttNodes[node].mOffset = addr;
    p = n[found].mPrev;
    n[n[found].mNext].mPrev = p;
    n[n[found].mPrev].mNext = n[found].mNext;
    if (spare - size == 0) {
        return addr;
    }
    next = found + size;
    n[next].mUnk_0 = spare - size;
    n[next].mUnk_6 = found;
    n[next + (spare - size)].mUnk_6 = next;
    while (spare - size < n[q].mUnk_0) {
        p = q;
        q = n[q].mNext;
    }
    n[next].mNext = q;
    n[next].mPrev = p;
    n[p].mNext = next;
    n[q].mPrev = next;
    return addr;
}

// Returns the VRAM byte offset of palette `key`, cached like FindOrLoadTex's textures, with these differences: the hash
// starts at the key's low 8 bits and wraps its probe with 0x3ff although the table has 0x100 slots; a miss stops only
// at a never-used slot; more than 200 entries remove the least recently used one; nothing is counted; the node records
// `size`; and the key range picks the source: below GetPlttFileKeyCount() offset `kind` of mPlttFile, below
// GetPaletteFileKeyEnd() palette `kind` of the palette file, else entry key - GetPaletteFileKeyEnd() of
// gSystemState.mUnk_390.
u32 FindOrLoadPltt(u32 kind, u32 key, u32 size) {
    u32 idx = key & 0xff;
    int first = -1;
    u32 step = gHashSteps[(key >> 8) & 0x7f];
    u32 node;

    for (;;) {
        if (key == gGxState.mPlttSlots[idx].mKey) {
            node = gGxState.mPlttSlots[idx].mNodeIndex;
            MoveIndexNode16ToEnd((IndexNode16 *) gGxState.mPlttNodes, node);
            return gGxState.mPlttNodes[node].mOffset;
        }
        if (gGxState.mPlttSlots[idx].mKey == -2 && first == -1) {
            first = idx;
        } else if (gGxState.mPlttSlots[idx].mKey == -1) {
            gGxState.mPlttEntryCount++;
            if (gGxState.mPlttEntryCount > 200) {
                RemoveLeastRecentPlttCacheEntry(gGxState.mPlttNodes[1].mNext);
            }
            node = gGxState.mPlttNodes[1].mPrev;
            gGxState.mPlttNodes[1].mPrev = gGxState.mPlttNodes[node].mPrev;
            gGxState.mPlttNodes[node].mPrev = gGxState.mPlttNodes[0].mPrev;
            gGxState.mPlttNodes[node].mNext = 0;
            gGxState.mPlttNodes[0].mPrev = node;
            gGxState.mPlttNodes[gGxState.mPlttNodes[node].mPrev].mNext = node;
            if (first != -1) {
                idx = first;
            }
            gGxState.mPlttSlots[idx].mKey = key;
            gGxState.mPlttSlots[idx].mNodeIndex = node;
            gGxState.mPlttNodes[node].mSlotIndex = idx;
            gGxState.mPlttNodes[node].mOffset = AllocPlttBlock(gGxState.mPlttBlocks, size, node);
            gGxState.mPlttNodes[node].mSize = size;
            if (key < GetPlttFileKeyCount()) {
                QueueTransfer(gGxState.mPlttFile, kind, gGxState.mPlttNodes[node].mOffset | 0xb0000000, size);
            } else if (key < GetPaletteFileKeyEnd()) {
                QueueTransfer(GetPaletteFile(), kind << 5, gGxState.mPlttNodes[node].mOffset | 0xb0000000, size);
            } else {
                QueueTransfer(0, gSystemState.mUnk_390[key - GetPaletteFileKeyEnd()] + 4,
                              gGxState.mPlttNodes[node].mOffset | 0xb0000000, size);
            }
            gSystemState.mUnk_2c4 += size;
            gSystemState.mUnk_2c0 += size;
            return gGxState.mPlttNodes[node].mOffset;
        }
        idx = (idx + step) & 0x3ff;
    }
}

// Reads node `nodeIndex` of track `track` into out/pos/vec and sets *done; nothing is written when the track has no
// data. With *done already set it would instead combine the angles with out by the track's flags (1 adds the
// difference from the node's own angles, 2 takes the read angles when one is more than 0x100 from the node's, else
// terminates) and drop the read position and vector; both callers clear *done first, so that path does not run in
// this build.
void ApplyAnimTrack(u32 track, u16 *out, s32 *pos, s32 *vec, u32 nodeIndex, int *done, const PoseNode *node) {
    u16 r[3];
    s32 p[3];
    s32 v[3];
    u32 kind;

    if (gGxState.mAnimTracks[track].mData == NULL) {
        return;
    }
    if (*done == 0) {
        ReadAnimTrackNode(out, pos, vec, nodeIndex, track);
        *done = 1;
        return;
    }
    ReadAnimTrackNode(r, p, v, nodeIndex, track);
    kind = gGxState.mAnimTracks[track].mFlags & 3;
    if (kind == 1) {
        out[0] += AngleDelta((u16) (node->mAngleX << 8), r[0]);
        out[1] += AngleDelta((u16) (node->mAngleY << 8), r[1]);
        out[2] += AngleDelta((u16) (node->mAngleZ << 8), r[2]);
    } else if (kind == 2) {
        if (AngleDistance((u16) (node->mAngleX << 8), r[0]) > 0x100 ||
            AngleDistance((u16) (node->mAngleY << 8), r[1]) > 0x100 ||
            AngleDistance((u16) (node->mAngleZ << 8), r[2]) > 0x100) {
            out[0] = r[0];
            out[1] = r[1];
            out[2] = r[2];
        }
    } else {
        OS_Terminate();
    }
}

// Loads file `file` into track 0 (it stays there), sets the track's frames and weight, reads node `node` into
// rot/pos/vec and shifts pos right by 3.
void ReadAnimFileNode(u16 file, u16 *rot, VecFx32 *pos, VecFx32 *vec, int frame, int nextFrame, int weight, int node) {
    gGxState.mAnimTracks[0].mNextWeight = weight;
    gGxState.mAnimTracks[0].mNextFrameIndex = nextFrame;
    gGxState.mAnimTracks[0].mFrameIndex = frame;
    gGxState.mAnimTracks[0].mData = FindOrLoadCachedFile(file);
    ReadAnimTrackNode(rot, (s32 *) pos, (s32 *) vec, node, 0);
    if (pos != NULL) {
        pos->x >>= 3;
        pos->y >>= 3;
        pos->z >>= 3;
    }
}

#pragma thumb on
