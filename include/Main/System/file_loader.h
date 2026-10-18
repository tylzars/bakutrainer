#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Slot of the cache's hash table (8 bytes): the key (-1 = never used, -2 = removed), the node that holds the entry's
// data and the number of search paths that pass through the slot.
typedef struct FileCacheSlot {
    s32 mUnk_0;
    u16 mUnk_4;
    u16 mUnk_6;
} FileCacheSlot;

// Node of the cache (16 bytes): the data block, its size, and the links of a circular list of nodes (node 0 is the
// head, its mUnk_8 is the last node).
typedef struct FileCacheNode {
    void *mUnk_0;
    u32 mUnk_4;
    u16 mUnk_8;
    u16 mUnk_a;
    u16 mUnk_c;
    u8 mUnk_e;
    u8 mUnk_f;
} FileCacheNode;

typedef struct FileCache FileCache;
typedef void (*FileCacheLoad)(u32 id, FileCacheNode *node, u32 size, u32 offset);

// A cache of loaded file data: a hash table of 1 << mUnk_18 slots, a list of nodes, the callback that loads an entry
// and the number of bytes held against a limit.
struct FileCache {
    FileCacheSlot *mUnk_00;
    FileCacheNode *mUnk_04;
    FileCacheLoad mUnk_08;
    u32 mUnk_0c;
    u32 mUnk_10;
    u16 mUnk_14;
    u16 mUnk_16;
    u16 mUnk_18;
    u8 mUnk_1a[2];
};

// Entry of the file table (8 bytes): the file's position and its stored size.
typedef struct Unk_FileEntry {
    u32 mUnk_0;
    u32 mUnk_4;
} Unk_FileEntry;

// Sets the file cache up.
void func_0202de94(void);

// Drops the entry with key `id` from the file cache.
void func_0202dec4(int id);

// Returns the data of file `id`: the cache entry for it, loaded through the cache's callback when missing.
void *func_0202ded8(int id);

// Sets a cache up: 1 << bits hash slots (all unused), `count` nodes chained in order, the load callback and the byte
// limit.
void func_0202df14(FileCache *cache, FileCacheLoad load, u32 limit, u32 bits, u32 count);

// Moves node `index` to the front of the list, after the head node.
void func_0202dff8(FileCacheNode *nodes, u32 index);

// Moves node `index` in front of the last node of the list.
void func_0202e04c(FileCacheNode *nodes, u32 index);

// Removes `key` from a cache's hash table and frees its data.
void func_0202e0a4(u32 key, FileCache *cache);

// Looks `key` up in the cache and loads it through the cache's callback when missing; returns the data, or -1 when
// the entry is missing and id, offset and size are all -1.
void *func_0202e19c(u32 key, u32 id, u32 offset, u32 size, FileCache *cache);

// The cache's load callback: reads file `id` (size bytes), unpacks it into node, then drops least recently used
// entries while the cache is over its byte or entry limit.
void func_0202e3b4(u32 id, FileCacheNode *node, u32 size, u32 offset);

// Drops the least recently used entry that holds data.
void func_0202e494(void);

// Copies `size` bytes at `offset` of file `file` to `dst`, from the cache when the file is cached.
void func_0202e4fc(u32 file, void *dst, u32 size, u32 offset);

// Reads `size` stored bytes at `offset` of file `file` and unpacks them according to the file's header; returns the
// buffer with the data (dst, or a new buffer when the data was packed) and leaves the unpacked size in the system
// state.
void *func_0202e56c(u32 file, void *dst, u32 size, u32 offset);

// Stored size of file `index` (second word of its file table entry).
u32 func_0202e760(u32 index);

// Position of file `index` (first word of its file table entry).
u32 func_0202e794(u32 index);

// Loads the file table.
void func_0202e7c4(void);

// Returns a new buffer with the unpacked data of file `id`, taken from the cache when it is cached.
u8 *func_0202e86c(int id);

// Walks the cache's node list from its head to the first node without data.
void func_0202e8e4(void);

// Undoes the delta coding of a buffer in place (the buffer starts with the "DIFF" header).
void func_0202e924(u8 *data, u32 size);

// Cached data of file `id`, or the negated position of the file when it is not cached.
int func_0202e988(int id);

// Size of the cached data of file `id`, or the negated stored size when it is not cached.
int func_0202e9d4(int id);

#ifdef __cplusplus
}
#endif
