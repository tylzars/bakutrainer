#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Slot of the cache's hash table (8 bytes): the key (-1 = never used, -2 = removed), the node that holds the entry's
// data and the number of search paths that pass through the slot.
typedef struct FileCacheSlot {
    s32 mKey;
    u16 mNodeIndex;
    u16 mPathCount;
} FileCacheSlot;

// Node of the cache (16 bytes): the data block, its unpacked size, and the links of a list of nodes (node 0 is the
// head, its mPrev is the last node).
typedef struct FileCacheNode {
    void *mData;
    u32 mSize;
    u16 mPrev;
    u16 mNext;
    u16 mSlotIndex;
    // Only ever written 0 in this build, so a cache hit always moves its node to the front.
    u8 mUnk_e;
    u8 mUnk_f;
} FileCacheNode;

typedef struct FileCache FileCache;
typedef void (*FileCacheLoad)(u32 id, FileCacheNode *node, u32 size, u32 offset);

// A cache of loaded file data: a hash table of 1 << mHashBits slots, a list of nodes, the callback that loads an entry
// and the number of bytes held against a limit. Both limits can be reached: LoadCacheEntry drops entries only while
// mByteCount is above mByteLimit or mEntryCount above mEntryLimit.
struct FileCache {
    FileCacheSlot *mSlots;
    FileCacheNode *mNodes;
    FileCacheLoad mLoad;
    u32 mByteCount;
    u32 mByteLimit;
    u16 mEntryCount;
    u16 mEntryLimit;
    u16 mHashBits;
    u8 mUnk_1a[2];
};

// Entry of the file table (8 bytes): the file's position and its stored size.
typedef struct FileEntry {
    u32 mOffset;
    u32 mStoredSize;
} FileEntry;

void InitFileCache(void);

void RemoveCachedFile(int id);

void *FindOrLoadCachedFile(int id);

// Sets a cache up: 1 << bits hash slots (all unused), `count` nodes chained in order, the load callback and the byte
// limit.
void InitCache(FileCache *cache, FileCacheLoad load, u32 limit, u32 bits, u32 count);

// Moves node `index` to the front of the list, after the head node.
void MoveCacheNodeToFront(FileCacheNode *nodes, u32 index);

// Moves node `index` in front of the last node of the list.
void MoveCacheNodeBeforeLast(FileCacheNode *nodes, u32 index);

// Removes `key` from a cache's hash table and frees its data.
void RemoveCacheEntry(u32 key, FileCache *cache);

// Looks `key` up in the cache and loads it through the cache's callback when missing; returns the data, or -1 when
// the entry is missing and id, offset and size are all -1.
void *FindOrLoadCacheEntry(u32 key, u32 id, u32 offset, u32 size, FileCache *cache);

// The cache's load callback: reads file `id` (size bytes), unpacks it into node, then drops least recently used
// entries while the cache is over its byte or entry limit.
void LoadCacheEntry(u32 id, FileCacheNode *node, u32 size, u32 offset);

// Drops the least recently used entry that holds data.
void RemoveLeastRecentCacheEntry(void);

// Copies `size` bytes at `offset` of file `file` to `dst`: from the unpacked cached data when the file is cached, else
// from the stored bytes.
void ReadFileBytes(u32 file, void *dst, u32 size, u32 offset);

// Reads `size` stored bytes at `offset` of file `file` and unpacks them according to the file's header; returns the
// buffer with the data (dst, or a new buffer when the data was packed) and leaves the unpacked size in the system
// state.
void *ReadAndUnpackFile(u32 file, void *dst, u32 size, u32 offset);

// Stored size of file `index` (second word of its file table entry).
u32 GetStoredFileSize(u32 index);

// Position of file `index` (first word of its file table entry).
u32 GetFileOffset(u32 index);

// Loads the file table.
void LoadFileTable(void);

// Returns a new buffer, which the caller frees, with the unpacked data of file `id`, taken from the cache when it is
// cached.
u8 *LoadFile(int id);

// Walks the cache's node list from its head to the first node without data.
void func_0202e8e4(void);

// Undoes the delta coding of a buffer in place (the buffer starts with the "DIFF" header).
void DecodeDiff(u8 *data, u32 size);

// Cached data of file `id`, or the negated position of the file when it is not cached.
int FindCachedFile(int id);

// Size of the cached data of file `id`, or the negated stored size when it is not cached.
int FindCachedFileSize(int id);

#ifdef __cplusplus
}
#endif
