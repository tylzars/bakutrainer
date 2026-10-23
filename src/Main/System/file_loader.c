#include <Main/System/system_init.h>
#include <Main/System/file_stream.h>
#include <Main/System/engine_asm.h>
#include <Main/Graphics/vram_cache.h>
#include <Main/System/file_loader.h>
#include <Main/System/qrk_decode.h>
#include <Main/System/align_util.h>
#include <Main/Link/comm_link.h>
#include <Main/Link/arm_system.h>
#include <Main/Link/wireless_helper.h>
#include <nitro/mi.h>
#include <nitro/os.h>

#pragma thumb off

#define FILE_CACHE_NODE_COUNT 0x166
#define FILE_CACHE_LIMIT 0x280000
#define FILE_CACHE_HASH_BITS 9

// Tags at the start of a stored file: "EZ10" (unpacked in place), "EZ00" and "EZ01" (unpacked into a new buffer),
// "QRK" (low 3 bytes) and "DIFF" followed by "08" (delta coded).
#define EZ10_MAGIC 0x30315a45
#define EZ00_MAGIC 0x30305a45
#define QRK_MAGIC 0x004b5251
#define DIFF_MAGIC 0x46464944
#define DIFF_VERSION 0x3830

// Bit 9 of the system flags: set when the start-up code did not read the file table from bank.bin. Headers of
// stored files are then read up to FILE_HEADER_MAX bytes instead of FILE_HEADER_MIN.
#define SYSTEM_FLAG_NO_BANK 0x200
#define FILE_HEADER_MIN 0x20
#define FILE_HEADER_MAX 0x1f7

// The cache of unpacked files: a hash table of slots keyed by file id and a list of nodes that hold the data.
FileCache gFileCache;

void InitFileCache(void) {
    InitCache(&gFileCache, LoadCacheEntry, FILE_CACHE_LIMIT, FILE_CACHE_HASH_BITS, FILE_CACHE_NODE_COUNT);
}

void RemoveCachedFile(int id) {
    RemoveCacheEntry(id, &gFileCache);
}

void *FindOrLoadCachedFile(int id) {
    return FindOrLoadCacheEntry(id, id, GetFileOffset(id), GetStoredFileSize(id), &gFileCache);
}

// Nodes are chained in order from node 0 to the last one.
void InitCache(FileCache *cache, FileCacheLoad load, u32 limit, u32 bits, u32 count) {
    int n = 1 << bits;
    FileCacheSlot *slots;
    int i;
    u32 j;
    FileCacheNode *nodes;

    cache->mSlots = Alloc(n * 8);
    cache->mNodes = Alloc(count * 16);
    slots = cache->mSlots;
    nodes = cache->mNodes;
    for (i = 0; i < n; i++) {
        slots[i].mKey = -1;
        slots[i].mPathCount = 0;
    }
    for (j = 0; j < count; j++) {
        nodes[j].mPrev = j - 1;
        nodes[j].mNext = j + 1;
        nodes[j].mData = 0;
        nodes[j].mSize = 0;
        nodes[j].mSlotIndex = 0;
        nodes[j].mUnk_e = 0;
    }
    nodes[0].mPrev = count - 1;
    (nodes + count)[-1].mNext = 0xffff;
    cache->mEntryCount = 0;
    cache->mByteCount = 0;
    cache->mLoad = load;
    cache->mHashBits = bits;
    cache->mByteLimit = limit;
    cache->mEntryLimit = count - 3;
}

void MoveCacheNodeToFront(FileCacheNode *nodes, u32 index) {
    FileCacheNode *e = &nodes[index];

    nodes[e->mPrev].mNext = e->mNext;
    nodes[e->mNext].mPrev = e->mPrev;
    e->mPrev = 0;
    e->mNext = nodes[0].mNext;
    nodes[0].mNext = (u16) index;
    nodes[e->mNext].mPrev = (u16) index;
}

void MoveCacheNodeBeforeLast(FileCacheNode *nodes, u32 index) {
    u32 tail = nodes[0].mPrev;
    FileCacheNode *e = &nodes[index];

    nodes[e->mPrev].mNext = e->mNext;
    nodes[e->mNext].mPrev = e->mPrev;
    e->mPrev = nodes[tail].mPrev;
    e->mNext = tail;
    nodes[tail].mPrev = (u16) index;
    nodes[e->mPrev].mNext = (u16) index;
}

// The search starts at the key's low bits and steps by gHashSteps[next 7 bits of the key]. Each slot passed loses
// one path; a removed slot (-2) left with no path becomes free (-1). The key's own slot becomes free if no path goes
// through it, removed otherwise. Its node moves in front of the last node.
void RemoveCacheEntry(u32 key, FileCache *cache) {
    u32 n;
    u32 idx;
    FileCacheSlot *slots;
    FileCacheNode *nodes;

    n = cache->mHashBits;
    idx = (key << (32 - n)) >> (32 - n);
    n = gHashSteps[(key >> n) & 0x7f];
    slots = cache->mSlots;
    nodes = cache->mNodes;
    for (;;) {
        if (key == slots[idx].mKey) {
            u32 node = slots[idx].mNodeIndex;

            slots[idx].mKey = slots[idx].mPathCount == 0 ? -1 : -2;
            cache->mEntryCount--;
            cache->mByteCount -= nodes[node].mSize;
            Free(&nodes[node]);
            MoveCacheNodeBeforeLast(nodes, node);
            return;
        }
        if (slots[idx].mKey == -1) {
            return;
        }
        if (slots[idx].mPathCount == 0) {
            OS_Terminate();
        }
        slots[idx].mPathCount--;
        if (slots[idx].mPathCount == 0 && slots[idx].mKey == -2) {
            slots[idx].mKey = -1;
        }
        idx += n;
        idx = (idx << (32 - cache->mHashBits)) >> (32 - cache->mHashBits);
    }
}

// The search is the one RemoveCacheEntry uses. A hit found past a removed slot moves into that slot, and the slots
// in between lose one path. A hit's node moves to the front of the list.
// A miss takes the first removed slot seen (else the free slot that ended the search) and the least recently used
// node, adds one path to each slot passed, and calls the load callback. A miss with id, offset and size all -1
// returns -1 and loads nothing.
void *FindOrLoadCacheEntry(u32 key, u32 id, u32 offset, u32 size, FileCache *cache) {
    u32 bits = cache->mHashBits;
    FileCacheSlot *slots = cache->mSlots;
    FileCacheNode *nodes = cache->mNodes;
    u32 step, home, idx;
    u32 firstFree = -1;

    home = (key << (32 - bits)) >> (32 - bits);
    step = gHashSteps[(key >> bits) & 0x7f];
    idx = home;

    for (;;) {
        s32 k = slots[idx].mKey;

        if (key == k) {
            if (firstFree != -1) {
                u32 p = idx;

                if (idx != firstFree) {
                    do {
                        p = ((p - step) << (32 - cache->mHashBits)) >> (32 - cache->mHashBits);
                        slots[p].mPathCount--;
                        if (slots[p].mPathCount == 0 && slots[p].mKey == -2) {
                            slots[p].mKey = -1;
                        }
                    } while (p != firstFree);
                }
                nodes[slots[idx].mNodeIndex].mSlotIndex = firstFree;
                slots[firstFree].mKey = slots[idx].mKey;
                slots[firstFree].mNodeIndex = slots[idx].mNodeIndex;
                slots[idx].mKey = slots[idx].mPathCount == 0 ? -1 : -2;
                idx = firstFree;
            }
            if (nodes[slots[idx].mNodeIndex].mUnk_e == 0) {
                MoveCacheNodeToFront(nodes, slots[idx].mNodeIndex);
            }
            gSystemState.mCacheHitSlotIndex = idx;
            return nodes[slots[idx].mNodeIndex].mData;
        }
        if (k == -1) {
            u32 n;

            if ((id & offset & size) == -1) {
                return (void *) -1;
            }
            if (firstFree != -1) {
                idx = firstFree;
            }
            slots[idx].mKey = key;
            MoveCacheNodeToFront(nodes, nodes[nodes[0].mPrev].mPrev);
            n = nodes[0].mNext;
            slots[idx].mNodeIndex = n;
            nodes[n].mSlotIndex = idx;
            while (idx != home) {
                idx = ((idx - step) << (32 - cache->mHashBits)) >> (32 - cache->mHashBits);
                slots[idx].mPathCount++;
            }
            cache->mLoad(id, &nodes[n], size, offset);
            return nodes[n].mData;
        }
        if (k == -2 && firstFree == -1) {
            firstFree = idx;
        }
        idx = ((idx + step) << (32 - cache->mHashBits)) >> (32 - cache->mHashBits);
    }
}

void LoadCacheEntry(u32 id, FileCacheNode *node, u32 size, u32 offset) {
    FileCacheSlot *slots = gFileCache.mSlots;
    FileCacheNode *nodes = gFileCache.mNodes;
    u32 i;

    node->mData = AllocWithFlags(size, 3);
    node->mData = ReadAndUnpackFile(id, node->mData, size, 0);
    gFileCache.mEntryCount++;
    gFileCache.mByteCount += gSystemState.mLoadedFileSize;
    node->mSize = gSystemState.mLoadedFileSize;
    i = nodes[nodes[0].mPrev].mPrev;
    while (gFileCache.mByteCount > gFileCache.mByteLimit || gFileCache.mEntryCount > gFileCache.mEntryLimit) {
        FileCacheNode *n;

        while (nodes[i].mData == 0) {
            i = nodes[i].mPrev;
        }
        n = &nodes[i];
        i = n->mPrev;
        RemoveCacheEntry(slots[n->mSlotIndex].mKey, &gFileCache);
    }
}

void RemoveLeastRecentCacheEntry(void) {
    FileCacheSlot *slots = gFileCache.mSlots;
    FileCacheNode *nodes = gFileCache.mNodes;
    u32 i;

    if (gFileCache.mEntryCount == 0) {
        OS_Terminate();
    }
    i = nodes[nodes[0].mPrev].mPrev;
    while (nodes[i].mData == 0) {
        i = nodes[i].mPrev;
    }
    RemoveCacheEntry(slots[nodes[i].mSlotIndex].mKey, &gFileCache);
}

void ReadFileBytes(u32 file, void *dst, u32 size, u32 offset) {
    u8 *data = FindOrLoadCacheEntry(file, -1, -1, -1, &gFileCache);

    if (data != (u8 *) -1) {
        MI_CpuCopy8(data + offset, dst, size);
    } else {
        u32 pos = GetFileOffset(file);

        SeekBank(pos + offset);
        ReadBank(dst, size);
    }
}

// The first bytes are read to see the tag. "EZ10" data is unpacked in place: it is read to the end of a new buffer
// big enough for the unpacked size (and at least the margin word of the header) and unpacked to its start.
// "EZ00", "EZ01" and "QRK" data are read into `dst`, unpacked into a new buffer, and `dst` is freed.
// Any other data stays in `dst`.
void *ReadAndUnpackFile(u32 file, void *dst, u32 size, u32 offset) {
    u32 hdr[FILE_HEADER_MAX];
    void *work;
    u32 n;
    u32 pad;
    u32 pos;
    u8 *tmp;

    if (size < ((gSystemState.mFlags & SYSTEM_FLAG_NO_BANK) != 0 ? FILE_HEADER_MAX : FILE_HEADER_MIN)) {
        n = size;
    } else {
        n = (gSystemState.mFlags & SYSTEM_FLAG_NO_BANK) != 0 ? FILE_HEADER_MAX : FILE_HEADER_MIN;
    }
    pos = GetFileOffset(file);
    SeekBank(pos + offset);
    ReadBank(hdr, n);
    if (hdr[0] == EZ10_MAGIC) {
        u32 need = hdr[1] - size;

        Free(&dst);
        if (-hdr[2] > need) {
            need = -hdr[2];
        }
        pad = (u32) AlignPtrUp4((void *) need);
        dst = AllocWithFlags(pad + size, 3);
        tmp = (u8 *) dst + pad;
        MI_CpuCopy8(hdr, tmp, n);
        if (size > n) {
            ReadBank(tmp + n, size - n);
        }
        UnpackEz10(tmp, dst);
        pad += size;
        size = pad;
    } else {
        MI_CpuCopy8(hdr, dst, n);
        if (size > n) {
            ReadBank((u8 *) dst + n, size - n);
        }
        if (hdr[0] == EZ00_MAGIC || hdr[0] == EZ00_MAGIC + 0x01000000) {
            void *out;

            size = hdr[1];
            out = AllocWithFlags(size, 3);
            UnpackEz(dst, out);
            Free(&dst);
            dst = out;
        } else if ((hdr[0] & 0xffffff) == QRK_MAGIC) {
            void *out;

            size = GetQrkUnpackedSize(dst);
            out = AllocWithFlags(size, 3);
            work = AllocWithFlags(GetQrkWorkSize(), 3);
            UnpackQrk(dst, out, work);
            Free(&dst);
            Free(&work);
            dst = out;
        }
    }
    gSystemState.mLoadedFileSize = size;
    DecodeDiff(dst, size);
    return dst;
}

u32 GetStoredFileSize(u32 index) {
    if (gSystemState.mFileTable == NULL) {
        LoadFileTable();
    }
    return ((FileEntry *) gSystemState.mFileTable)[index].mStoredSize;
}

u32 GetFileOffset(u32 index) {
    if (gSystemState.mFileTable == NULL) {
        LoadFileTable();
    }
    return ((FileEntry *) gSystemState.mFileTable)[index].mOffset;
}

// Sends a request packet (tag 0x50) until a reply arrives with tag 0x52, the expected sequence byte and the value 1.
// The reply carries the table's size; the table is then read from the start of the file system.
void LoadFileTable(void) {
    u8 *reply = gCommRecords;
    u8 *cmd = gCommSendData;

    for (;;) {
        cmd[0] = 0x50;
        cmd[2] = 0;
        *(u16 *) (cmd + 4) = 1;
        UpdateSystemAndWait();
        if (gCommRecordReceived[0] == 0) {
            continue;
        }
        if (reply[0] != 0x52) {
            continue;
        }
        if (reply[2] != WhGetAid() || *(u16 *) (reply + 4) != 1) {
            continue;
        }
        break;
    }
    gSystemState.mFileTableSize = ReadU32FromHalfwords(reply + 14);
    gSystemState.mFileTable = AllocWithFlags(gSystemState.mFileTableSize, 3);
    SeekBank(0);
    ReadBank(gSystemState.mFileTable, gSystemState.mFileTableSize);
}

u8 *LoadFile(int id) {
    int size = FindCachedFileSize(id);
    u8 *buf;

    gSystemState.mLoadedFileSize = size >= 0 ? size : -size;
    buf = AllocWithFlags(size >= 0 ? size : -size, 3);
    if (size > 0) {
        MI_CpuCopy8((void *) FindCachedFile(id), buf, size);
    } else {
        buf = ReadAndUnpackFile(id, buf, -size, 0);
    }
    return buf;
}

void func_0202e8e4(void) {
    FileCacheNode *nodes = gFileCache.mNodes;
    u32 i;

    (void) gSystemState.mElapsedFrames;
    i = nodes[0].mNext;
    while (nodes[i].mData != 0) {
        i = nodes[i].mNext;
    }
}

void DecodeDiff(u8 *data, u32 size) {
    if (*(u32 *) data != DIFF_MAGIC) {
        return;
    }
    if (*(u16 *) (data + 4) == DIFF_VERSION) {
        u32 n = size - 7;
        s8 *src = (s8 *) data + 8;
        u8 *dst = (u8 *) src - 8;
        u32 acc = 0;

        while (--n != 0) {
            u32 next = *src++ + acc;

            *dst++ = next;
            acc = next;
        }
    } else {
        OS_Terminate();
    }
}

int FindCachedFile(int id) {
    void *data = FindOrLoadCacheEntry(id, -1, -1, -1, &gFileCache);

    if (data != (void *) -1) {
        return (int) data;
    }
    return -GetFileOffset(id);
}

int FindCachedFileSize(int id) {
    void *data = FindOrLoadCacheEntry(id, -1, -1, -1, &gFileCache);

    if (data == (void *) -1) {
        return -GetStoredFileSize(id);
    }
    return gFileCache.mNodes[gFileCache.mSlots[gSystemState.mCacheHitSlotIndex].mNodeIndex].mSize;
}

#pragma thumb on
