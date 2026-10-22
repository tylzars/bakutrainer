#include <Main/clib.h>
#include <Main/System/system_init.h>
#include <Main/System/bg_image.h>
#include <Main/Graphics/vram_cache.h>
#include <Main/Graphics/model.h>
#include <nitro/fx.h>
#include <nitro/mi.h>
#include <nitro/os.h>
#include <Main/System/engine_asm.h>
#include <Main/System/screen_fade.h>
#include <Main/Graphics/texture_blocks.h>
#include <Main/System/file_loader.h>
#include <Main/Link/wireless_helper.h>
#include <Main/Link/link_indicator.h>

// Block counts by texture size code, used by GetSizeCodeBytes.
const u16 data_02050978[16] = {
    1, 4, 0x10, 0x40, 2, 4, 8, 0x20, 2, 4, 8, 0x20, 0x40, 0x80, 0x80, 0x100,
};

const u8 data_02050998[16][2] = {
    {8, 8},  {16, 16}, {32, 32}, {64, 64}, {16, 8},   {32, 8},   {32, 16},  {64, 32},
    {8, 16}, {8, 32},  {16, 32}, {32, 64}, {128, 32}, {64, 128}, {128, 64}, {128, 128},
};

#pragma thumb off

void func_0202ad34(void) {}

void func_0202ad38(void) {}

void func_0202ad3c(void) {}

int func_0202ad40(void) {
    return 0;
}

void func_0202ad48(int a) {}

void func_0202ad4c(void) {}

void func_0202ad50(void) {}

void func_0202ad54(void) {}

u16 GetPaletteFile(void) {
    return gTextureBlocks.mPaletteFile;
}

u16 GetBlockFile(void) {
    return gTextureBlocks.mBlockFile;
}

// The unit shift (mUnitShift) of layer 1's tables; any other layer selects mLayers[1].
u8 GetUnitShift(int layer) {
    return gTextureBlocks.mLayers[layer != 1].mUnitShift;
}

int func_0202ad9c(void) {
    return gTextureBlocks.mUnk_b28;
}

s8 func_0202adac(void) {
    return gTextureBlocks.mUnk_b2c;
}

// Header of block `index` in the packed file's block table: the table holds each block's offset from its start.
u32 *GetBlockHeader(u32 index) {
    u8 *table = (u8 *) gTextureBlocks.mBlockTable;
    return (u32 *) (table + *(u32 *) (table + ((u16) index << 2) + 0x10));
}

u16 GetBlockCount(void) {
    return gTextureBlocks.mBlockCount;
}

// Number of OAM entries to fill with 0x200 for a layer: 128, or fewer on the screen that GetIndicatorLayer reports when
// the flag byte or bit 9 of the system flags is set.
u32 GetOamFillCount(int layer) {
    if (layer == GetIndicatorLayer()) {
        if (gSystemState.mUnk_250.mUnk_28[0] != 0) {
            return 0x7e;
        }
        if (gSystemState.mFlags & 0x200) {
            return 0x7f;
        }
    }
    return 0x80;
}

// Hides every sprite of a layer (OAM filled with 0x200) and empties the layer's queue of OAM entries (mOamCount).
void ResetOam(int layer) {
    int sub = layer & 1;
    u32 oam = sub ? 0x7000000 : 0x7000400;

    MIi_CpuClear32(0x200, (void *) oam, GetOamFillCount(layer) * 8);
    gTextureBlocks.mLayers[sub == 0].mOamCount = 0;
}

// Fills `out` from texture `sub` of block `index`; an empty block gives a zeroed record with mUnk_12 = -1.
// The block header word holds four offsets/counts; the records follow the header's tables.
void GetTexInfo(u32 index, TexInfo *out, u32 sub) {
    u32 *hdr = GetBlockHeader(index);
    u32 w = hdr[0];
    u32 b0 = w & 0xff;
    u32 b1 = (w >> 8) & 0xff;
    int b2 = (w >> 16) & 0xff;
    u32 count = (w >> 24) & 0xff;
    TexRec *e;

    if (count == 0) {
        memset(out, 0, 0x18);
        out->mUnk_12 = -1;
        return;
    }
    if (!(sub < count)) {
        OS_Terminate();
    }
    e = (TexRec *) (hdr + (b0 + 1 + b1 * 2 + b2 / 2));
    out->mUnk_04 = (u32) ((u32 *) &e[sub] - b2 / 2);
    out->mRecordCount = count;
    out->mX = e[sub].mUnk_00_0;
    out->mY = e[sub].mUnk_00_9;
    out->mUnk_12 = e[sub].mUnk_00_18;
    out->mSizeCode = e[sub].mUnk_00_21;
    out->mUnk_13 = e[sub].mUnk_00_25;
    out->mUnk_00 = e[sub].mUnk_04 & 0xffffff;
    out->mUnk_14 = (e[sub].mUnk_04 << 4) >> 28;
    out->mUnk_15 = (e[sub].mUnk_04 >> 28) + 1;
    out->mWidth = data_02050998[out->mSizeCode][0];
    out->mHeight = data_02050998[out->mSizeCode][1];
}

// Frees the run of VRAM units that starts at unit `index` of a layer's block heap: merges it with a free neighbour
// on each side, links it into the free list, which is ordered by size, and returns the merged run's length in units.
u16 FreeLayerBlock(u32 index, int layer) {
    TextureLayer *textureLayer = &gTextureBlocks.mLayers[layer == 2];
    Unk_PoolRange *n = textureLayer->mBlocks;
    u32 next;
    u32 prev;
    u32 size;

    n[index].mUnk_0 &= ~POOL_BLOCK_USED;
    next = index + n[index].mUnk_0;
    if (next < textureLayer->mUnitCount && (n[next].mUnk_0 & POOL_BLOCK_USED) == 0) {
        n[index].mUnk_0 += n[next].mUnk_0;
        n[index + n[index].mUnk_0].mUnk_6 = index;
        n[n[next].mNext].mPrev = n[next].mPrev;
        n[n[next].mPrev].mNext = n[next].mNext;
    }
    prev = n[index].mUnk_6;
    if (prev < textureLayer->mUnitCount && (n[prev].mUnk_0 & POOL_BLOCK_USED) == 0) {
        n[prev].mUnk_0 += n[index].mUnk_0;
        index = prev;
        n[index + n[index].mUnk_0].mUnk_6 = index;
        n[n[prev].mNext].mPrev = n[prev].mPrev;
        n[n[prev].mPrev].mNext = n[prev].mNext;
    }
    size = n[index].mUnk_0;
    if (size < 4) {
        prev = textureLayer->mUnitCount + 1;
        next = n[prev].mPrev;
        while (size > n[next].mUnk_0) {
            prev = next;
            next = n[next].mPrev;
        }
        n[index].mPrev = next;
        n[index].mNext = prev;
        n[prev].mPrev = index;
        n[next].mNext = index;
    } else {
        u32 q = textureLayer->mUnitCount;
        u32 p = n[q].mNext;
        while (size < n[p].mUnk_0) {
            q = p;
            p = n[p].mNext;
        }
        n[index].mNext = p;
        n[index].mPrev = q;
        n[q].mNext = index;
        n[p].mPrev = index;
    }
    return n[index].mUnk_0;
}

// Removes `key` from a layer's hash table of cached blocks. The search starts at the key's low 10 bits and steps by
// an odd prime chosen from the next 7 bits; every entry passed on the way loses one from its path count, and an
// entry that has no path left and was marked removed (-2) becomes free again (-1). The key's own entry becomes
// free if no path goes through it, removed otherwise.
void RemoveLayerKey(int layer, u32 key) {
    TextureLayer *textureLayer = &gTextureBlocks.mLayers[layer == 2];
    unsigned long idx = key & 0x3ff; // unsigned long, not u32: the compiler's output needs it (register choice)
    u32 step = gHashSteps[(key >> 10) & 0x7f];
    TextureSlot *e;

    for (;;) {
        e = &textureLayer->mSlots[idx];
        if (key == e->mKey) {
            if (e->mPathCount == 0) {
                e->mKey = -1;
            } else {
                e->mKey = -2;
            }
            break;
        }
        if (e->mPathCount == 0) {
            OS_Terminate();
        }
        textureLayer->mSlots[idx].mPathCount--;
        e = &textureLayer->mSlots[idx];
        if (e->mPathCount == 0 && e->mKey == -2) {
            e->mKey = -1;
        }
        idx = (idx + step) & 0x3ff;
    }
}

// Releases the least recently used entry of a layer's cache list, entry `index`, which must be entry 1's mNext (as at
// both callers): drops its hash entry, frees its run of units and moves the entry to the head of the free chain
// after the list's sentinel (entry 1). Returns the length FreeLayerBlock reports.
u16 RemoveLeastRecentLayerCacheEntry(u32 index, int layer) {
    TextureLayer *textureLayer = &gTextureBlocks.mLayers[layer == 2];
    s32 key = textureLayer->mSlots[textureLayer->mNodes[index].mUnk_0].mKey;
    u16 size;

    if (key < 0) {
        OS_Terminate();
    }
    textureLayer->mEntryCount--;
    RemoveLayerKey(layer, key);
    size = FreeLayerBlock(textureLayer->mNodes[index].mUnk_6, layer);
    textureLayer->mNodes[1].mNext = textureLayer->mNodes[index].mNext;
    textureLayer->mNodes[textureLayer->mNodes[index].mNext].mPrev = 1;
    textureLayer->mNodes[index].mPrev = textureLayer->mNodes[1].mPrev;
    textureLayer->mNodes[1].mPrev = index;
    return size;
}

// Size in bytes for size code `index`: data_02050978 entry times 32, doubled for mode 0 and modes 5 to 7.
u32 GetSizeCodeBytes(u32 index, int mode) {
    u32 v = data_02050978[index] << 5;

    if (mode == 0 || (u32) (mode - 5) <= 2) {
        v <<= 1;
    }
    return v;
}

// Finds or adds the cache entry of block `key` (with `sub` in mode-2 packed files) in a layer's hash table and returns
// the first VRAM unit of its block. A hit moves its hash entry up into the first removed entry the search passed and
// its list entry to the most recently used end of the list; its data is reloaded from the preloaded file block when
// that block is the one asked for, and in mode 0 its sixteen 4-bit palette slots are compared with the current ones
// and its tiles remapped when they differ. A miss takes the first never-used entry on the search path (or the first
// removed one once the search comes round to it), a list entry and a run of VRAM units from the block heap (dropping
// the oldest list entries until a large enough run is free), and queues the block's transfer from the packed file.
u16 FindOrLoadLayerBlock(u32 code, u32 key, u32 sub, u32 mode, u32 count, int layer) {
    unsigned long entry; // unsigned long, not u32: the compiler's output needs it (stack slot order)
    s32 stored;
    u32 val;
    TextureLayer *textureLayer = &gTextureBlocks.mLayers[layer == 2];
    u32 step;
    u32 n;
    int home;
    Unk_PoolRange *heap;
    u32 blk;
    u32 pos;
    TextureSlot *he;
    Unk_PoolRange *node;
    u32 i;
    u8 *vals;
    u32 split;
    u32 steps = 0;
    u32 before;
    u32 next;
    u32 *map;
    int loaded = 0;
    s32 freePos = -1;
    u32 k;

    if (gTextureBlocks.mUnk_000 != 0 && gTextureBlocks.mUnk_017 == 1) {
        if (gTextureBlocks.mUnk_00c == ((key << 2) | (sub << 25))) {
            gTextureBlocks.mUnk_017 = 0;
            loaded = 1;
        }
    }
    if (gTextureBlocks.mBlockFileKind == 2) {
        key = sub | (key << 7);
    } else {
        key <<= 5;
    }
    step = gHashSteps[(key >> 10) & 0x7f];
    home = key & 0x3ff;
    pos = home;
    for (;;) {
        stored = textureLayer->mSlots[pos].mKey;
        steps++;
        if (stored == key) {
            if (freePos != -1) {
                n = pos;
                while (n != freePos) {
                    n = (n - step) & 0x3ff;
                    textureLayer->mSlots[n].mPathCount--;
                    he = &textureLayer->mSlots[n];
                    if (he->mPathCount == 0 && he->mKey == -2) {
                        he->mKey = -1;
                    }
                }
                textureLayer->mSlots[freePos].mUnk_0[0] = textureLayer->mSlots[pos].mUnk_0[0];
                textureLayer->mSlots[freePos].mUnk_0[1] = textureLayer->mSlots[pos].mUnk_0[1];
                textureLayer->mSlots[freePos].mKey = textureLayer->mSlots[pos].mKey;
                textureLayer->mSlots[freePos].mNodeIndex = textureLayer->mSlots[pos].mNodeIndex;
                he = &textureLayer->mSlots[pos];
                he->mKey = he->mPathCount == 0 ? -1 : -2;
                pos = freePos;
            }
            entry = textureLayer->mSlots[pos].mNodeIndex;
            textureLayer->mNodes[entry].mUnk_0 = pos;
            MoveIndexNodeToEnd((IndexNode *) textureLayer->mNodes, entry);
            if (loaded == 1) {
                u32 addr = (layer == 1 ? 0x6400000 : 0x6600000) +
                           ((textureLayer->mNodes[entry].mUnk_6 * (1 << textureLayer->mUnitShift)) << 5);

                MIi_CpuCopy32(gTextureBlocks.mUnk_004, (void *) addr, GetSizeCodeBytes(code, mode));
                textureLayer->mSlots[pos].mUnk_0[0] = 0x76543210;
                textureLayer->mSlots[pos].mUnk_0[1] = 0xfedcba98;
            }
            if (mode > 1) {
                OS_Terminate();
            }
            if (mode == 0) {
                u8 *cur;
                u32 *words;

                cur = &textureLayer->mUnk_115[textureLayer->mUnk_516 - count];
                words = textureLayer->mSlots[pos].mUnk_0;
                for (i = 0; i < count; i++) {
                    val = (words[(i * 4) >> 5] >> ((i * 4) & 0x1f)) & 0xf;
                    if (val != *cur++) {
                        u8 buf[16];
                        u32 addr = (layer == 1 ? 0x6400000 : 0x6600000) +
                                   ((textureLayer->mNodes[entry].mUnk_6 * (1 << textureLayer->mUnitShift)) << 5);
                        int size = GetSizeCodeBytes(code, mode);

                        cur = &textureLayer->mUnk_115[textureLayer->mUnk_516 - count];
                        for (i = 0; i < count; i++) {
                            u32 w = (i * 4) >> 5;
                            u32 sh = (i * 4) & 0x1f;

                            buf[(words[w] >> sh) & 0xf] = *cur;
                            words[w] = (words[w] & ~(0xf << sh)) | (*cur++ << sh);
                        }
                        RemapPaletteRows((u16 *) addr, buf, size / 2);
                        break;
                    }
                }
            }
            return textureLayer->mNodes[entry].mUnk_6;
        }
        if (stored == -2 && freePos == -1) {
            freePos = pos;
        } else if (stored == -1 || freePos == pos) {
            textureLayer->mEntryCount++;
            entry = textureLayer->mNodes[1].mPrev;
            textureLayer->mNodes[1].mPrev = textureLayer->mNodes[entry].mPrev;
            textureLayer->mNodes[entry].mPrev = textureLayer->mNodes[0].mPrev;
            textureLayer->mNodes[entry].mNext = 0;
            textureLayer->mNodes[0].mPrev = entry;
            textureLayer->mNodes[textureLayer->mNodes[entry].mPrev].mNext = entry;
            textureLayer->mSlots[pos].mKey = key;
            textureLayer->mSlots[pos].mNodeIndex = entry;
            textureLayer->mNodes[entry].mUnk_0 = pos;
            n = pos;
            while (n != home) {
                n = (n - step) & 0x3ff;
                textureLayer->mSlots[n].mPathCount++;
            }
            if (steps > 20) {
                textureLayer->mUnk_57c = 1;
            }
            if (mode > 1) {
                OS_Terminate();
            }
            if (mode == 0) {
                vals = &textureLayer->mUnk_115[textureLayer->mUnk_516 - count];
                map = textureLayer->mSlots[pos].mUnk_0;
                for (k = 0; k < count; k++) {
                    u32 w = (k * 4) >> 5;
                    u32 sh = (k * 4) & 0x1f;

                    map[w] = (map[w] & ~(0xf << sh)) | (*vals++ << sh);
                }
            }
            break;
        }
        pos = (pos + step) & 0x3ff;
    }

    n = data_02050978[code] << (mode == 0);
    n = ((n - 1) >> textureLayer->mUnitShift) + 1;
    heap = textureLayer->mBlocks;
    for (;;) {
        blk = -1;
        next = heap[textureLayer->mUnitCount].mNext;
        while (n <= heap[next].mUnk_0) {
            blk = next;
            next = heap[next].mNext;
        }
        if (textureLayer->mEntryCount > 0x2fe) {
            RemoveLeastRecentLayerCacheEntry(textureLayer->mNodes[1].mNext, layer);
        }
        if (blk != -1) {
            break;
        }
        i = 1;
        do {
            i = textureLayer->mNodes[i].mNext;
        } while (RemoveLeastRecentLayerCacheEntry(i, layer) < n);
    }
    {
        u32 rest = heap[blk].mUnk_0;

        heap[blk].mUnk_0 = n + POOL_BLOCK_USED;
        rest -= n;
        if (mode == 0) {
            textureLayer->mUnk_014[textureLayer->mUnk_114] = textureLayer->mUnk_516 - count;
            QueueTransferEx(gTextureBlocks.mBlockFile, key,
                            (layer == 1 ? 0x6400000 : 0x6600000) + ((blk * (1 << textureLayer->mUnitShift)) << 5),
                            (n * (1 << textureLayer->mUnitShift)) << 5, 0, 0,
                            (textureLayer->mUnk_114++ << 1) | (layer == 2));
        } else {
            QueueTransfer(gTextureBlocks.mBlockFile, key,
                          (layer == 1 ? 0x6400000 : 0x6600000) + ((blk * (1 << textureLayer->mUnitShift)) << 5),
                          (n * (1 << textureLayer->mUnitShift)) << 5);
        }
        textureLayer->mNodes[entry].mUnk_6 = blk;
        node = &heap[blk];
        before = node->mPrev;
        heap[node->mNext].mPrev = before;
        heap[node->mPrev].mNext = node->mNext;
        if (rest != 0) {
            split = blk + n;
            heap[split].mUnk_0 = rest;
            heap[split].mUnk_6 = blk;
            heap[split + rest].mUnk_6 = split;
            while (rest < heap[next].mUnk_0) {
                before = next;
                next = heap[next].mNext;
            }
            heap[split].mNext = next;
            heap[split].mPrev = before;
            heap[before].mNext = split;
            heap[next].mPrev = split;
        }
        return blk;
    }
}

// Resets a layer's tables: hides its sprites (ResetOam), then resets the cache list (entries 0 and 1 are the ends,
// the rest chained as free), the hash table (all keys unused), the block heap (reallocated, one free run of
// mUnitCount units between two sentinels), the palette slot list, the palette position lists and, when the layer has
// a chain table (mUnk_57b == 1), that table's links. Also clears the long-search flag and
// gSystemState.mUnk_250.mUnk_28[0].
void ResetTextureLayer(int layer) {
    TextureLayer *textureLayer = &gTextureBlocks.mLayers[layer == 2];
    int i;
    Unk_PoolRange *n;
    u32 m;

    ResetOam(layer);
    textureLayer->mNodes[0].mUnk_0 = 0x55aa;
    textureLayer->mNodes[0].mUnk_6 = 0x55aa;
    textureLayer->mNodes[1].mUnk_0 = 0x55aa;
    textureLayer->mNodes[1].mUnk_6 = 0x55aa;
    textureLayer->mNodes[0].mPrev = 1;
    textureLayer->mNodes[1].mNext = 0;
    textureLayer->mNodes[1].mPrev = 2;
    textureLayer->mEntryCount = 0;
    for (i = textureLayer->mNodes[1].mPrev; i < 0x302; i++) {
        textureLayer->mNodes[i].mPrev = i + 1;
        textureLayer->mNodes[i].mUnk_0 = 0xffff;
    }
    for (i = 0; i < 0x400; i++) {
        textureLayer->mSlots[i].mKey = -1;
        textureLayer->mSlots[i].mPathCount = 0;
    }
    if (gSystemState.mFlags & layer) {
        Free(&textureLayer->mBlocks);
    }
    textureLayer->mBlocks = Alloc((textureLayer->mUnitCount + 2) << 3);
    m = textureLayer->mUnitCount;
    n = textureLayer->mBlocks;
    n[0].mUnk_0 = m;
    textureLayer->mBlocks[0].mUnk_6 = 0xffff;
    textureLayer->mBlocks[0].mPrev = textureLayer->mUnitCount;
    textureLayer->mBlocks[0].mNext = textureLayer->mUnitCount + 1;
    textureLayer->mBlocks[textureLayer->mUnitCount].mUnk_0 = 0xffff;
    textureLayer->mBlocks[textureLayer->mUnitCount].mNext = 0;
    textureLayer->mBlocks[textureLayer->mUnitCount].mPrev = 0xffff;
    textureLayer->mBlocks[textureLayer->mUnitCount + 1].mUnk_0 = 0;
    textureLayer->mBlocks[textureLayer->mUnitCount + 1].mNext = 0xffff;
    textureLayer->mBlocks[textureLayer->mUnitCount + 1].mPrev = 0;
    for (i = 0; i < 0x10; i++) {
        textureLayer->mUnk_534[i].mKey = 0xffff;
        textureLayer->mUnk_534[i].mPaletteIndex = i;
    }
    textureLayer->mUnk_114 = 1;
    textureLayer->mUnk_516 = 1;
    gSystemState.mUnk_250.mUnk_28[0] = 0;
    textureLayer->mUnk_57c = 0;
    if (textureLayer->mUnk_57b == 1) {
        for (i = 0; i < 0x100; i++) {
            textureLayer->mUnk_518[i].mKey = 0xfffe;
            textureLayer->mUnk_518[i].mUnk_6 = i;
            textureLayer->mUnk_518[i].mChainPrev = 0xff;
            textureLayer->mUnk_518[i].mChainNext = 0xff;
            textureLayer->mUnk_518[i].mPrev = i - 1;
            textureLayer->mUnk_518[i].mNext = i + 1;
        }
        for (i = 0; i < 0x10; i++) {
            textureLayer->mUnk_51c[i] = 0x10;
        }
        textureLayer->mUnk_51c[15] = 0xf;
    }
}

// Sets up a layer: checks the layer `e` (1 = main engine, 2 = sub engine) and the mode `d` (low 6 bits; bit 7 selects
// the chain tables), programs the engine's display control register and VRAM banks, allocates the layer's tables
// unless the layer is already set up, reads the header of packed file `b` and loads the block table from file `a`
// unless files `b`/`c` are already loaded ("PACK" files keep their offsets in memory, "EZ10" files are read on
// demand), then resets the layer.
void SetupTextureLayer(int a, int b, int c, u32 d, int e) {
    u32 sub = d >> 7;
    TextureLayer *textureLayer = &gTextureBlocks.mLayers[e == 2];
    u32 mask = e == 1 ? 1 : 2;
    vu32 *reg = e == 1 ? (vu32 *) 0x4000000 : (vu32 *) 0x4001000;
    int size;
    u32 v;
    u32 hdr[2];

    d &= 0x3f;
    if (!(e == 1 || d == 0 || d == 2)) {
        OS_Terminate();
    }
    if (!(e == 2 || d <= 3)) {
        OS_Terminate();
    }
    if (!(e == 1 || e == 2)) {
        OS_Terminate();
    }
    gSystemState.mFlags &= ~0xc0000;
    textureLayer->mUnitCount = (e == 2 && d == 0) ? 0x200 : 0x400;
    if (e & 1) {
        v = *(vu32 *) 0x4000000;
        v = (v & 0x7f1fefcf) | (0x1030 | (sub << 31));
        *(vu32 *) 0x4000000 = v;
        textureLayer->mUnk_57b = sub;
    }
    if (e & 2) {
        v = *(vu32 *) 0x4001000;
        v = (v & 0x7f3fefef) | (0x1010 | (sub << 31));
        *(vu32 *) 0x4001000 = v;
        textureLayer->mUnk_57b = sub;
    }
    if (!(gSystemState.mFlags & mask)) {
        textureLayer->mUnitShift = d;
        size = 0x20 << d;
        v = *reg;
        v = (v & ~0x300000) | (d << 20);
        *reg = v;
        if (sub == 1) {
            ReallocVramBanks(e == 1 ? 5 : 9, 0x10, -1);
        }
        if (textureLayer->mUnitCount < 0x400) {
            size >>= 1;
        }
        ReallocVramBanks(e == 1 ? 3 : 7, size, -1);
        Free(textureLayer);
        Free(&textureLayer->mSortKeys);
        Free(&textureLayer->mMats);
        Free(&textureLayer->mSlots);
        Free(&textureLayer->mNodes);
        textureLayer->mOam = Alloc(0x400);
        textureLayer->mSortKeys = Alloc(0x80);
        textureLayer->mOamCount = 0;
        textureLayer->mMats = Alloc(0x100);
        textureLayer->mMatCount = 0;
        textureLayer->mSlots = Alloc(0x4000);
        textureLayer->mNodes = Alloc(0x1810);
    }
    if (!(gTextureBlocks.mBlockFile == b && gTextureBlocks.mPaletteFile == c && gTextureBlocks.mBlockTable != 0)) {
        if (!(gTextureBlocks.mBlockFile == b && gTextureBlocks.mPaletteFile == c)) {
            gTextureBlocks.mPaletteCount = GetStoredFileSize(c) >> 5;
            if (gTextureBlocks.mUnk_000 != 0) {
                Free(&gTextureBlocks.mUnk_004);
                gTextureBlocks.mUnk_000 = 0;
            }
            gTextureBlocks.mUnk_016 = 0;
            gTextureBlocks.mUnk_012 = gTextureBlocks.mPaletteCount;
        }
        ReadFileBytes(b, hdr, 8, 0);
        if (hdr[0] == 0x4b434150) {
            gTextureBlocks.mBlockFileKind = 1;
            gTextureBlocks.mPackOffsets = Alloc(hdr[1] - 4);
            ReadFileBytes(b, gTextureBlocks.mPackOffsets, hdr[1] - 4, 4);
        } else if (hdr[0] == 0x30315a45) {
            gTextureBlocks.mBlockFileKind = 2;
            gTextureBlocks.mPackOffsets = 0;
        } else {
            gTextureBlocks.mBlockFileKind = 0;
            gTextureBlocks.mPackOffsets = 0;
        }
        Free(&gTextureBlocks.mBlockTable);
        gTextureBlocks.mBlockTable = (u32 *) LoadFile(a);
        gTextureBlocks.mPaletteFile = c;
        gTextureBlocks.mBlockFile = b;
        gTextureBlocks.mBlockCount = (gTextureBlocks.mBlockTable[4] - 0x10) >> 2;
    }
    Free(&textureLayer->mUnk_52c);
    Free(&textureLayer->mUnk_518);
    if (sub == 1) {
        textureLayer->mUnk_518 = Alloc(0x800);
        textureLayer->mUnk_52c = Alloc(gTextureBlocks.mPaletteCount + 0x200);
        memset(textureLayer->mUnk_52c, 0xff, gTextureBlocks.mPaletteCount + 0x200);
    }
    gSystemState.mFlags |= mask;
    ResetTextureLayer(e);
}

// Frees the tables of all set-up layers, leaving the loaded file's table.
void ReleaseTextureLayers(void) {
    ReleaseTextureLayersEx(0);
}

// Frees the tables of the layers in use, their VRAM banks and the PACK offsets (but not mUnk_52c); with a == 1 also
// frees the loaded file's table.
void ReleaseTextureLayersEx(int a) {
    u32 i;

    if (!(gSystemState.mFlags & 3)) {
        return;
    }
    for (i = 0; i < 2; i++) {
        if (gSystemState.mFlags & (i == 0 ? 1 : 2)) {
            Free(&gTextureBlocks.mPackOffsets);
            Free(&gTextureBlocks.mLayers[i].mOam);
            Free(&gTextureBlocks.mLayers[i].mSortKeys);
            Free(&gTextureBlocks.mLayers[i].mSlots);
            Free(&gTextureBlocks.mLayers[i].mNodes);
            Free(&gTextureBlocks.mLayers[i].mBlocks);
            Free(&gTextureBlocks.mLayers[i].mMats);
            Free(&gTextureBlocks.mLayers[i].mUnk_518);
            FreeVramBanks(i == 0 ? 3 : 7);
        }
    }
    if (a == 1) {
        Free(&gTextureBlocks.mBlockTable);
    }
    gSystemState.mFlags &= ~3;
}

// Rearranges `w` x `h` tiles from sequential order into rows of a bitmap `w` tiles wide: for each tile, `mode` 1
// copies eight words (4 bits per pixel), any other mode eight word pairs (8 bits per pixel). Terminates when the
// source and destination are the same.
void func_0202c2f0(void *dst, const void *src, u32 w, u32 h, BOOL mode) {
    u32 *d = dst;
    u32(*d8)[2] = dst;
    const u32 *s = src;
    u32 n = 0;
    u32 y;
    u32 x;
    u32 k;

    if (dst == src) {
        OS_Terminate();
    }
    for (y = 0; y < h; y++) {
        for (x = 0; x < w; x++) {
            if (mode == 1) {
                u32 *p = d + (y * w * 8 + x);
                for (k = 0; k < 8; k++) {
                    *p = s[n++];
                    p += w;
                }
            } else {
                u32(*p)[2] = d8 + (y * w * 8 + x);
                for (k = 0; k < 8; k++) {
                    p[0][0] = s[n];
                    p[0][1] = s[n + 1];
                    n += 2;
                    p += w;
                }
            }
        }
    }
}

// Expands the tiles of `src` (`w` tiles wide, 4 or 8 bits per pixel by the entry's byte 13) into 16-bit colours
// at the entry's destination: every non-zero pixel is looked up in the palette memory of the layer the
// destination belongs to and written with bit 15 set; the rows of the destination are GetLayerWidth(layer)
// halfwords apart. Sets the entry's palette byte to 0xff. In the 8-bit branch the test uses the low four bits of
// the pixel but the palette index is the whole byte.
void func_0202c3e0(Transfer *entry, const void *src, u32 w) {
    u32 dst = entry->mDst;
    u32 size = entry->mSize;
    int layer = (dst >> 20) == 0x60 ? 1 : 2;
    int stride = GetLayerWidth(layer);
    const u32 *s = src;
    u32 j;
    int k;

    if (entry->mUnk_13 == 0) {
        const u16 *pal = (const u16 *) ((layer == 1 ? 0x5000200 : 0x5000600) + (entry->mUnk_12 >> 4) * 32);
        if (size != 0) {
            do {
                u32 row = dst;
                for (j = 0; j < w; j++) {
                    u32 word = *s++;
                    for (k = 0; k < 8; k++) {
                        if ((word & 0xf) != 0) {
                            *(u16 *) dst = pal[word & 0xf] + 0x8000;
                        }
                        dst += 2;
                        word >>= 4;
                    }
                }
                dst = row + stride * 2;
                size -= w * 4;
            } while (size != 0);
        }
    } else {
        const u16 *pal = (const u16 *) (layer == 1 ? 0x5000200 : 0x5000600);
        if (size != 0) {
            do {
                u32 row = dst;
                for (j = 0; j < w * 2; j++) {
                    const u8 *p = (const u8 *) s++;
                    for (k = 0; k < 4; k++) {
                        u32 b = *p++;
                        if ((b & 0xf) != 0) {
                            *(u16 *) dst = pal[b] + 0x8000;
                        }
                        dst += 2;
                    }
                }
                dst = row + stride * 2;
                size -= w * 8;
            } while (size != 0);
        }
    }
    entry->mUnk_12 = 0xff;
}

// Moves entry `i` of a layer's chain table to the end of the list threaded through mPrev and mNext; entry 255 is the
// list's anchor.
void MoveChainEntryToListEnd(u32 i, TextureLayer *textureLayer) {
    textureLayer->mUnk_518[textureLayer->mUnk_518[i].mPrev].mNext = textureLayer->mUnk_518[i].mNext;
    textureLayer->mUnk_518[textureLayer->mUnk_518[i].mNext].mPrev = textureLayer->mUnk_518[i].mPrev;
    textureLayer->mUnk_518[i].mPrev = textureLayer->mUnk_518[255].mPrev;
    textureLayer->mUnk_518[i].mNext = 0xff;
    textureLayer->mUnk_518[255].mPrev = (u8) i;
    textureLayer->mUnk_518[textureLayer->mUnk_518[i].mPrev].mNext = (u8) i;
}

// Looks for the entry of block `key` whose slot (high nibble of byte 6) is `val`: the chain starts at
// textureLayer->mUnk_52c[key] and continues through each entry's mChainNext. Returns 0xff when there is none.
u32 FindChainEntry(u32 key, u32 val, TextureLayer *textureLayer) {
    u32 n = textureLayer->mUnk_52c[key];

    if (n == 0xff) {
        return 0xff;
    }
    do {
        if (val == textureLayer->mUnk_518[n].mUnk_6 >> 4) {
            return n;
        }
        n = textureLayer->mUnk_518[n].mChainNext;
    } while (n != 0xff);
    return 0xff;
}

// Takes the oldest entry of slot `val` from the list (MoveChainEntryToListEnd), loads block `key` into the 32 bytes
// that entry names (from the file when `key` is below the file's block count, otherwise from the preloaded block at
// gTextureBlocks.mUnk_008 + index * 32), chains the entry at the end of key `key`'s chain and returns its number.
u32 func_0202c5fc(u32 index, u32 key, u32 val, TextureLayer *textureLayer) {
    u32 n = textureLayer->mUnk_518[255].mNext;
    u32 sel;
    PaletteChainEntry *t;
    u32 prev;
    u32 next;

    while (val != textureLayer->mUnk_518[n].mUnk_6 >> 4) {
        n = textureLayer->mUnk_518[n].mNext;
    }
    textureLayer->mUnk_51c[val]--;
    MoveChainEntryToListEnd(n, textureLayer);
    sel = (&gTextureBlocks.mLayers[1] == textureLayer) ? 9 : 5;
    if (key < gTextureBlocks.mPaletteCount) {
        QueueTransfer(gTextureBlocks.mPaletteFile, key << 5, (textureLayer->mUnk_518[n].mUnk_6 << 5) + (sel << 28),
                      0x20);
    } else {
        QueueTransfer(0, (u32) (gTextureBlocks.mUnk_008 + (index << 5)),
                      (textureLayer->mUnk_518[n].mUnk_6 << 5) + (sel << 28), 0x20);
    }
    prev = textureLayer->mUnk_52c[key];
    if (prev == 0xff) {
        textureLayer->mUnk_52c[key] = n;
        textureLayer->mUnk_518[n].mKey = key;
        textureLayer->mUnk_518[n].mChainPrev = 0xff;
        textureLayer->mUnk_518[n].mChainNext = 0xff;
    } else {
        t = textureLayer->mUnk_518;
        next = t[prev].mChainNext;
        while (next != 0xff) {
            prev = next;
            next = t[next].mChainNext;
        }
        t[prev].mChainNext = n;
        textureLayer->mUnk_518[n].mChainPrev = prev;
        textureLayer->mUnk_518[n].mKey = 0xffff;
    }
    return textureLayer->mUnk_518[n].mUnk_6;
}

// Makes the `n` consecutive blocks starting at keys[0] (or the `n` keys listed there when keys[0] is below the
// file's block count) resident in one slot: a slot already holding all of them is used if there is one, otherwise
// the slot that needs the fewest loads and has room, evicting the oldest entries until one has. Each key is then
// looked up or loaded (FindChainEntry, func_0202c5fc) and its entry's low nibble appended to the layer's index list
// (mUnk_115, count in mUnk_516). Returns the slot. `a` is not used.
s32 func_0202c778(u16 *keys, int a, u32 n, int layer) {
    TextureLayer *textureLayer = &gTextureBlocks.mLayers[layer == 2];
    u32 node2;
    u8 count[16];
    int min;
    u16 list[16];
    u32 mask = 0xffff;
    int j;
    u32 idx;
    s32 slot;
    u32 node;
    u32 i;
    u32 avail;
    u32 key;

    if (keys[0] < gTextureBlocks.mPaletteCount) {
        memcpy(list, keys, n * 2);
    } else {
        for (j = 0; j < n; j++) {
            list[j] = keys[0] + j;
        }
    }
    for (i = 0; i < n; i++) {
        key = list[i];
        node = textureLayer->mUnk_52c[key];
        avail = 0;
        while (node != 0xff) {
            avail |= 1 << (textureLayer->mUnk_518[node].mUnk_6 >> 4);
            node = textureLayer->mUnk_518[node].mChainNext;
        }
        mask &= avail;
    }
    if (mask != 0) {
        slot = 31 - LeadingZeroCount(mask);
    } else {
        MI_CpuClear32(count, 16);
        for (i = 0; i < n; i++) {
            node = textureLayer->mUnk_52c[list[i]];
            while (node != 0xff) {
                count[textureLayer->mUnk_518[node].mUnk_6 >> 4]++;
                node = textureLayer->mUnk_518[node].mChainNext;
            }
        }
        for (;;) {
            slot = -1;
            min = 32;
            for (j = 0; j < 16; j++) {
                int need = n - count[j];
                if (need <= textureLayer->mUnk_51c[j]) {
                    if (need < min) {
                        min = need;
                        slot = j;
                    }
                }
            }
            if (slot != -1) {
                break;
            }
            node = textureLayer->mUnk_518[255].mNext;
            while (textureLayer->mUnk_518[node].mKey == 0xfffe) {
                node = textureLayer->mUnk_518[node].mNext;
            }
            if (textureLayer->mUnk_518[node].mKey != 0xffff) {
                textureLayer->mUnk_52c[textureLayer->mUnk_518[node].mKey] = textureLayer->mUnk_518[node].mChainNext;
            }
            textureLayer->mUnk_518[node].mChainPrev = 0xff;
            textureLayer->mUnk_518[node].mChainNext = 0xff;
            textureLayer->mUnk_518[node].mKey = 0xfffe;
            textureLayer->mUnk_51c[textureLayer->mUnk_518[node].mUnk_6 >> 4]++;
        }
    }
    for (i = 0; i < n; i++) {
        node2 = FindChainEntry(list[i], slot, textureLayer);
        if (node2 == 0xff) {
            node2 = func_0202c5fc(i, list[i], slot, textureLayer);
        }
        if (textureLayer->mUnk_516 >= 0x400) {
            OS_Terminate();
        }
        idx = textureLayer->mUnk_516;
        textureLayer->mUnk_516++;
        textureLayer->mUnk_115[idx] = textureLayer->mUnk_518[node2].mUnk_6 & 0xf;
    }
    return slot;
}

#define VRAM_PLTT_BG 0x5000200

// Hardware OAM entry (8 bytes), and a rotation/scale matrix as stored in a layer's matrix list.
typedef struct Oam {
    u16 mUnk_0;
    u16 mUnk_2;
    u16 mUnk_4;
    u16 mUnk_6;
} Oam;

typedef struct Mat4 {
    s16 mUnk_0;
    s16 mUnk_2;
    s16 mUnk_4;
    s16 mUnk_6;
} Mat4;

// Slot list of a layer: the keys of the palettes in VRAM, most recently used first. For count keys (a key below
// the preloaded block count comes from the list at ids, which advances; any other key is the first key plus its
// position and is read from the block data) it finds or loads each palette and returns the last slot. With flag
// 0 the slots are also appended to the layer's record, and a record that repeats the run before it is removed again.
s32 func_0202ca80(u16 *ids, int flag, u32 count, int layer) {
    TextureLayer *textureLayer = &gTextureBlocks.mLayers[layer == 2];
    int base = textureLayer->mUnk_516;
    int s;
    u32 i;
    s32 ret;

    if (flag == 0 && textureLayer->mUnk_57b == 1) {
        ret = func_0202c778(ids, flag, count, layer);
    } else {
        for (i = 0; i < count; i++) {
            u32 key;
            u32 miss;

            key = *ids;
            if (key < gTextureBlocks.mPaletteCount) {
                ids++;
            } else {
                key = (u16) (key + i);
            }
            miss = 1;
            for (s = 0; s < 16; s++) {
                u32 k = textureLayer->mUnk_534[s].mKey;
                if (key == k) {
                    miss = 0;
                    break;
                }
                if (k == 0xffff) {
                    break;
                }
            }
            if (s >= count) {
                u32 keep;
                u32 *p;
                if (s >= 15) {
                    s = 15;
                }
                keep = textureLayer->mUnk_534[s].mPaletteIndex & 0xff;
                for (p = (u32 *) &textureLayer->mUnk_534[s]; s > 0; s--) {
                    *p = p[-1];
                    p--;
                }
                textureLayer->mUnk_534[0].mPaletteIndex = keep;
                s = 0;
            }
            textureLayer->mUnk_534[s].mKey = key;
            if (miss == 1) {
                u32 pal = (layer == 2) ? 0x400 : 0;
                if (key < gTextureBlocks.mPaletteCount) {
                    QueueTransfer(gTextureBlocks.mPaletteFile, key * 32,
                                  VRAM_PLTT_BG + pal + textureLayer->mUnk_534[s].mPaletteIndex * 32, 0x20);
                } else {
                    QueueTransfer(0, (u32) gTextureBlocks.mUnk_008 + i * 32,
                                  VRAM_PLTT_BG + pal + textureLayer->mUnk_534[s].mPaletteIndex * 32, 0x20);
                }
            }
            ret = textureLayer->mUnk_534[s].mPaletteIndex;
            if (flag == 0) {
                if (textureLayer->mUnk_516 >= 0x400) {
                    OS_Terminate();
                }
                textureLayer->mUnk_115[textureLayer->mUnk_516++] = ret;
            }
        }
    }
    if (flag == 0) {
        int d = textureLayer->mUnk_516 - base;
        if (base >= d) {
            if (memcmp(&textureLayer->mUnk_115[base - d], &textureLayer->mUnk_115[base], d) == 0) {
                textureLayer->mUnk_516 = base;
            }
        }
    }
    return ret;
}

// Fills out (8 bytes) for a sprite record: the point GetSpritePoint gives for type 3 (or 0, 0), the record's two
// 9-bit values, the width and height of its rect and of its size code.
void func_0202cce0(u32 obj, u32 rec, u8 *out) {
    s32 pt[2];
    s32 rect[4];

    GetSpriteRect(obj, 0, rect);
    if (GetSpritePoint((u16) obj, 3, pt) == 1) {
        out[0] = pt[0];
        out[1] = pt[1];
    } else {
        out[1] = 0;
        out[0] = 0;
    }
    out[6] = rect[2] - rect[0] + 1;
    out[7] = rect[3] - rect[1] + 1;
    out[2] = rec & 0x1ff;
    out[3] = (rec << 14) >> 23;
    out[4] = data_02050998[(rec << 7) >> 28][0];
    out[5] = data_02050998[(rec << 7) >> 28][1];
}

// The matrix count (mMatCount) of a layer; layer 3 is invalid.
u8 GetMatCount(int layer) {
    if (layer == 3) {
        OS_Terminate();
    }
    return gTextureBlocks.mLayers[(layer & 1) == 0].mMatCount;
}

// Finds or appends the matrix for an angle and two scales (16.12 fixed point, as four halfwords) in a layer's matrix
// list (layer 2 when flags bit 1 is set) and returns its index.
u32 FindOrAddMat(int angle, int sx, int sy, u32 flags) {
    TextureLayer *textureLayer = &gTextureBlocks.mLayers[(flags & 2) != 0];
    int ix = 0x10000 / sx;
    int iy = ix;
    s32 idx;
    s16 m[4];
    u32 *p;
    u32 *w;
    u32 j;

    if (sx != sy) {
        iy = 0x10000 / sy;
    }
    idx = ((angle << 8) >> 4) << 1;
    m[0] = (ix * FX_SinCosTable_[idx + 1]) >> 12;
    m[1] = (ix * FX_SinCosTable_[idx]) >> 12;
    m[2] = (iy * -FX_SinCosTable_[idx]) >> 12;
    m[3] = (iy * FX_SinCosTable_[idx + 1]) >> 12;
    w = (u32 *) m;
    p = (u32 *) textureLayer->mMats;
    for (j = 0; j < textureLayer->mMatCount; j++) {
        if (p[0] == w[0] && p[1] == w[1]) {
            return j;
        }
        p += 2;
    }
    p[0] = w[0];
    p[1] = w[1];
    return ++textureLayer->mMatCount - 1;
}

// Draws sprite id at x, y on a layer, unrotated and at full scale (0x100); w gives the priority (w >> 8) and the sort
// key (its low byte), as in DrawSpriteEx.
void DrawSprite(int id, int x, int y, int w, int layer) {
    DrawSpriteEx(id, x, y, w, 0, 0x100, 0x100, layer);
}

// Draws sprite `id` of a layer (layer 1 or 2) at x, y. The low 16 bits of id select the block; bits 16 and 17 flip
// it horizontally and vertically, bit 18 sets the semi-transparent mode, bit 19 the double size of a rotated
// sprite, bit 20 copies the tiles straight into the layer's background VRAM instead, bit 21 draws nothing and keeps
// the tile and palette of the first record it reaches in mUnk_b28 and mUnk_b2c, bit 22 sets attribute 2 bit 11,
// and bit 23 hands the sprite to func_020243c0 with the alpha in bits 27-31 (0 = 31). w >> 8 is the priority and
// its low byte the sort key of each entry. angle and the scales sx, sy (0x100 = 1) give the sprite a rotation/scale
// matrix. The checks that stop the system are written as `(void) (cond || (OS_Terminate(), 0))` because the
// compiler's output needs it: the expression form computes each condition's value and loads the 0, which an `if`
// drops.
void DrawSpriteEx(int id, int x, int y, int w, int angle, int sx, int sy, int layer) {
    u32 bit;
    u32 flags;
    u32 aff;
    u32 w0;
    u16 *ids;
    u32 b0;
    u32 b1;
    int b2;
    u8 count;
    u32 *p;
    u16 key;
    int dx;
    int dy;
    int dbl;
    u32 n;
    int prio;
    s8 rect[8];
    TextureLayer *textureLayer;

    key = id;
    flags = id & 0xffff0000;
    bit = layer == 1 ? 1 : 2;
    textureLayer = &gTextureBlocks.mLayers[layer == 2];
    aff = -1;
    (void) ((gSystemState.mFlags & bit) || (id & 0x800000) || (OS_Terminate(), 0));
    if (id == -1 || key == 0xffff) {
        return;
    }
    gTextureBlocks.mUnk_b28 = -1;
    gTextureBlocks.mUnk_b2c = -1;
    if (id & 0x800000) {
        u32 alpha = (u32) id >> 27;

        (void) (layer == 1 || (OS_Terminate(), 0));
        if (alpha == 0) {
            alpha = 0x1f;
        }
        func_020243c0(id | flags, x, y, -w << 6, -angle << 8, sx << 4, sy << 4, alpha);
        return;
    }
    p = GetBlockHeader(key);
    w0 = *p;
    b0 = w0 & 0xff;
    b1 = (w0 >> 8) & 0xff;
    b2 = (w0 >> 16) & 0xff;
    count = (w0 >> 24) & 0xff;
    (void) (b1 != 0 || (OS_Terminate(), 0));
    p += b0 + 1 + b1 * 2 + b2 / 2;
    ids = (u16 *) (p - b2 / 2);
    if (gTextureBlocks.mUnk_000 == 1 && gTextureBlocks.mUnk_010 == key) {
        ids = &gTextureBlocks.mUnk_012;
    }
    if (sx != 0x100 || sy != 0x100 || angle != 0) {
        int sxv;
        int syv;
        int cx;
        int cy;

        sxv = sx;
        syv = sy;
        (void) ((sx >= 0 && sy >= 0) || (OS_Terminate(), 0));
        if (flags & 0x10000) {
            angle = (u8) -angle;
            sxv = -sx;
            flags &= ~0x10000;
        }
        if (flags & 0x20000) {
            angle = (u8) -angle;
            syv = -sy;
            flags &= ~0x20000;
        }
        func_0202cce0(key, *p, (u8 *) rect);
        dbl = ((flags & 0x80000) && (angle != 0 || sx > 0x100 || sy > 0x100)) ? 1 : 0;
        aff = FindOrAddMat(angle, sxv, syv, layer);
        // Offset from the first record's centre to the object's point, scaled and turned by the matrix.
        cx = (-(rect[2] + (rect[4] / 2 - rect[0])) * ((s16) sx * (s16) sx)) >> 8;
        cy = (-(rect[3] + (rect[5] / 2 - rect[1])) * ((s16) sy * (s16) sy)) >> 8;
        cx = sxv < 0 ? -cx : cx;
        cy = syv < 0 ? -cy : cy;
        dx = (cx * -((Mat4 *) textureLayer->mMats)[aff].mUnk_0 + cy * ((Mat4 *) textureLayer->mMats)[aff].mUnk_2) >> 16;
        dy = (cx * ((Mat4 *) textureLayer->mMats)[aff].mUnk_4 - cy * ((Mat4 *) textureLayer->mMats)[aff].mUnk_6) >> 16;
        dx = sxv < 0 ? -dx : dx;
        dy = syv < 0 ? -dy : dy;
        dx = dx - (rect[4] >> (1 - dbl)) + rect[0] * sxv / 256 - rect[2];
        dy = dy - (rect[5] >> (1 - dbl)) + rect[1] * syv / 256 - rect[3];
    }
    (void) (w < 0x400 || (OS_Terminate(), 0));
    n = textureLayer->mOamCount;
    prio = w >> 8;
    while (count-- != 0 && n < 0x80) {
        int px = ((TexRec *) p)->mUnk_00_0;
        int py = ((TexRec *) p)->mUnk_00_9;
        u32 shape = ((TexRec *) p)->mUnk_00_18;
        u32 size = ((TexRec *) p)->mUnk_00_21;
        u32 c = ((TexRec *) p)->mUnk_00_25;
        u32 w1 = ((TexRec *) p)->mUnk_04;
        u32 off = (w1 << 8) >> 8;
        u32 pi = (w1 << 4) >> 28;
        int cnt = shape == 1 ? 1 : (w1 >> 28) + 1;
        int wd;
        int ht;
        u32 pal;
        u16 tile;
        u16 a0;
        u16 a1;

        wd = data_02050998[size][0];
        ht = data_02050998[size][1];
        p += 2;
        if (flags & 0x10000) {
            px = -px - wd;
        }
        px += x;
        if (flags & 0x20000) {
            py = -py - ht;
        }
        py += y;
        if (aff == -1 && (px + wd < 0 || px >= 0x100 || py + ht < 0 || py >= 0xc0)) {
            continue;
        }
        pal = func_0202ca80(&ids[pi], shape, cnt, layer);
        if (flags & 0x100000) {
            u32 base;
            u32 dst;
            u32 src;
            u32 shift;
            u32 sz;
            u32 fl;
            u32 bits;

            (void) (func_0201db58(layer) == 0x8000 || (OS_Terminate(), 0));
            (void) (shape == 1 || textureLayer->mUnk_57b == 0 || (OS_Terminate(), 0));
            (void) ((flags & 0x30000) == 0 || (OS_Terminate(), 0));
            (void) (aff == -1 || (OS_Terminate(), 0));
            base = layer == 1 ? 0x6000000 : 0x6200000;
            dst = base + (py * GetLayerWidth(layer) + px) * 2;
            if (gTextureBlocks.mBlockFileKind == 2) {
                src = c + (off << 7);
            } else {
                (void) (gTextureBlocks.mBlockFileKind == 1 || (OS_Terminate(), 0));
                src = off << 5;
            }
            // shift = log2 of the width: 32 minus the leading zeros of the bits below its lowest set bit. An asm
            // statement: clz has no C expression.
            bits = data_02050998[size][0];
            bits = ~bits & (bits - 1);
            asm { clz bits, bits }
            shift = 32 - bits;
            sz = GetSizeCodeBytes(size, shape);
            if (shape == 1) {
                fl = 0;
            } else {
                fl = ((textureLayer->mUnk_516 - cnt) << 1) | (layer == 2);
            }
            QueueTransferEx(gTextureBlocks.mBlockFile, src, dst, sz, 0, shift + (pal << 4), fl);
            continue;
        }
        tile = FindOrLoadLayerBlock(size, off, c, shape, cnt, layer);
        if (flags & 0x200000) {
            gTextureBlocks.mUnk_b28 = tile;
            gTextureBlocks.mUnk_b2c = pal;
            return;
        }
        if (aff == -1) {
            a0 = (u8) py | ((size >> 2) << 14) | ((shape == 0) << 13) | ((flags & 0x40000) >> 8);
            a1 = (px & 0x1ff) | ((size & 3) << 14) | ((flags & 0x30000) >> 4);
        } else {
            a0 = (u8) (py + dy) | ((size >> 2) << 14) | ((shape == 0) << 13) | 0x100 | (dbl << 9) |
                 ((flags & 0x40000) >> 8);
            a1 = ((px + dx) & 0x1ff) | ((size & 3) << 14) | (aff << 9);
        }
        ((Oam *) textureLayer->mOam)[n].mUnk_0 = a0;
        ((Oam *) textureLayer->mOam)[n].mUnk_2 = a1;
        ((Oam *) textureLayer->mOam)[n].mUnk_4 = tile | (pal << 12) | (prio << 10) | ((flags & 0x400000) >> 11);
        ((u8 *) textureLayer->mSortKeys)[n] = w;
        n++;
    }
    textureLayer->mOamCount = n;
}

// Looks up the point of type index in the object's record and flips it for the flip flags in bits 16 and 17 of
// flags; returns whether it exists.
int GetSpritePoint(u16 flags, int index, s32 *out) {
    u32 *hdr = GetBlockHeader(flags);
    u32 i;
    u32 count = hdr[0] & 0xff;
    u32 *rec = hdr + 1;

    for (i = 0; i < count; i++) {
        s32 w = *rec++;
        if (index == (w >> 18)) {
            out[0] = (w << 23) >> 23;
            out[1] = (w << 14) >> 23;
            if (flags & 0x10000) {
                out[0] = -out[0];
            }
            if (flags & 0x20000) {
                out[1] = -out[1];
            }
            return 1;
        }
    }
    return 0;
}

// Looks up the rect of type a in the object's record, flipped for bits 16 and 17 of obj; returns whether it exists.
int GetSpriteRect(int obj, int a, s32 *rect) {
    u32 *hdr = GetBlockHeader((u16) obj);
    u32 i = 0;
    u32 w = hdr[0];
    u32 b0 = w & 0xff;
    u32 *rec = hdr + (b0 + 1);
    u32 count = (w >> 8) & 0xff;

    for (; i < count; i++) {
        s32 lo = rec[0];
        s32 hi = rec[1];
        rec += 2;
        if (((lo >> 18) & 0x1f) == a) {
            s32 t;
            rect[0] = (lo << 23) >> 23;
            rect[2] = (hi << 23) >> 23;
            if (obj & 0x10000) {
                t = rect[0];
                rect[0] = -rect[2];
                rect[2] = -t;
            }
            rect[1] = (lo << 14) >> 23;
            rect[3] = (hi << 14) >> 23;
            if (obj & 0x20000) {
                t = rect[1];
                rect[1] = -rect[3];
                rect[3] = -t;
            }
            return 1;
        }
    }
    return 0;
}

// Writes a layer's queued sprites into OAM ordered by their sort key, hides the rest, copies the matrices to the
// OAM parameter slots and empties the queues.
void WriteOam(int layer) {
    int sub = (layer == 2);
    u32 bit = (layer == 1) ? 1 : 2;
    u32 other = (layer == 1) ? 0x40000 : 0x80000;
    TextureLayer *textureLayer = &gTextureBlocks.mLayers[sub];
    Oam *oam;
    u16 *affine;
    int n;
    int i;
    u8 hist[256];

    if (bit != (gSystemState.mFlags & (bit | other))) {
        return;
    }
    n = 0;
    oam = (layer == 1) ? (Oam *) 0x7000000 : (Oam *) 0x7000400;
    if (textureLayer->mOamCount != 0) {
        u16 *src = (u16 *) textureLayer->mOam;

        MI_CpuClearFast(hist, 0x100);
        n = textureLayer->mOamCount;
        for (i = 0; i < n; i++) {
            hist[((u8 *) textureLayer->mSortKeys)[i]]++;
        }
        n = hist[0];
        for (i = 1; i < 256; i++) {
            n += hist[i];
            hist[i] = n;
        }
        for (i = 0; i < textureLayer->mOamCount; i++) {
            u8 key = ((u8 *) textureLayer->mSortKeys)[i];
            u8 v = --hist[key];

            ((u16 *) oam)[v * 4] = src[0];
            ((u16 *) oam)[v * 4 + 1] = src[1];
            ((u16 *) oam)[v * 4 + 2] = src[2];
            src += 4;
        }
    }
    i = GetOamFillCount(layer);
    for (; n < i; n++) {
        oam[n].mUnk_0 = 0x200;
    }
    affine = (layer == 1) ? (u16 *) 0x7000006 : (u16 *) 0x7000406;
    for (i = 0; i < textureLayer->mMatCount; i++) {
        affine[0] = ((Mat4 *) textureLayer->mMats)[i].mUnk_0;
        affine[4] = ((Mat4 *) textureLayer->mMats)[i].mUnk_2;
        affine[8] = ((Mat4 *) textureLayer->mMats)[i].mUnk_4;
        affine[12] = ((Mat4 *) textureLayer->mMats)[i].mUnk_6;
        affine += 16;
    }
    textureLayer->mOamCount = 0;
    textureLayer->mMatCount = 0;
    textureLayer->mUnk_114 = 1;
    textureLayer->mUnk_516 = 1;
}

#pragma thumb on
