#include <Main/Unknown/system_init.h>
#include <Main/Unknown/file_stream.h>
#include <Main/Unknown/engine_asm.h>
#include <Main/Unknown/vram_cache.h>
#include <Main/Unknown/file_loader.h>
#include <Main/Unknown/qrk_decode.h>
#include <Main/Unknown/align_util.h>
#include <Main/Unknown/comm_link.h>
#include <Main/Unknown/wireless_helper.h>
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
FileCache data_020c6470;

void func_0202de94(void) {
    func_0202df14(&data_020c6470, func_0202e3b4, FILE_CACHE_LIMIT, FILE_CACHE_HASH_BITS, FILE_CACHE_NODE_COUNT);
}

// Drops the entry with key `id` from the file cache.
void func_0202dec4(int id) {
    func_0202e0a4(id, &data_020c6470);
}

// Returns the data of file `id`: the cache entry for it, loaded through the cache's callback when missing.
void *func_0202ded8(int id) {
    return func_0202e19c(id, id, func_0202e794(id), func_0202e760(id), &data_020c6470);
}

// Sets a cache up: 1 << bits hash slots (all unused), `count` nodes chained in order from node 0 to the last one,
// the load callback and the byte limit.
void func_0202df14(FileCache *cache, FileCacheLoad load, u32 limit, u32 bits, u32 count) {
    int n = 1 << bits;
    FileCacheSlot *slots;
    int i;
    u32 j;
    FileCacheNode *nodes;

    cache->mUnk_00 = func_0201c430(n * 8);
    cache->mUnk_04 = func_0201c430(count * 16);
    slots = cache->mUnk_00;
    nodes = cache->mUnk_04;
    for (i = 0; i < n; i++) {
        slots[i].mUnk_0 = -1;
        slots[i].mUnk_6 = 0;
    }
    for (j = 0; j < count; j++) {
        nodes[j].mUnk_8 = j - 1;
        nodes[j].mUnk_a = j + 1;
        nodes[j].mUnk_0 = 0;
        nodes[j].mUnk_4 = 0;
        nodes[j].mUnk_c = 0;
        nodes[j].mUnk_e = 0;
    }
    nodes[0].mUnk_8 = count - 1;
    (nodes + count)[-1].mUnk_a = 0xffff;
    cache->mUnk_14 = 0;
    cache->mUnk_0c = 0;
    cache->mUnk_08 = load;
    cache->mUnk_18 = bits;
    cache->mUnk_10 = limit;
    cache->mUnk_16 = count - 3;
}

// Moves node `index` to the front of the list, after the head node.
void func_0202dff8(FileCacheNode *nodes, u32 index) {
    FileCacheNode *e = &nodes[index];

    nodes[e->mUnk_8].mUnk_a = e->mUnk_a;
    nodes[e->mUnk_a].mUnk_8 = e->mUnk_8;
    e->mUnk_8 = 0;
    e->mUnk_a = nodes[0].mUnk_a;
    nodes[0].mUnk_a = (u16) index;
    nodes[e->mUnk_a].mUnk_8 = (u16) index;
}

// Moves node `index` in front of the last node of the list.
void func_0202e04c(FileCacheNode *nodes, u32 index) {
    u32 tail = nodes[0].mUnk_8;
    FileCacheNode *e = &nodes[index];

    nodes[e->mUnk_8].mUnk_a = e->mUnk_a;
    nodes[e->mUnk_a].mUnk_8 = e->mUnk_8;
    e->mUnk_8 = nodes[tail].mUnk_8;
    e->mUnk_a = tail;
    nodes[tail].mUnk_8 = (u16) index;
    nodes[e->mUnk_8].mUnk_a = (u16) index;
}

// Removes `key` from a cache's hash table and frees its data. The search starts at the key's low bits and steps by an
// entry of data_02050378 (1, then odd primes) chosen by the next 7 bits; every slot passed on the way loses one from
// its path count, and a slot that has no path left and was marked removed (-2) becomes free again (-1). The key's own
// slot becomes free if no path goes through it, removed otherwise. Its node moves to the end of the list.
void func_0202e0a4(u32 key, FileCache *cache) {
    u32 bits;
    u32 step;
    u32 idx;
    FileCacheSlot *slots;
    FileCacheNode *nodes;

    bits = cache->mUnk_18;
    idx = (key << (32 - bits)) >> (32 - bits);
    step = data_02050378[(key >> bits) & 0x7f];
    slots = cache->mUnk_00;
    nodes = cache->mUnk_04;
    for (;;) {
        if (key == slots[idx].mUnk_0) {
            u32 node = slots[idx].mUnk_4;

            slots[idx].mUnk_0 = slots[idx].mUnk_6 == 0 ? -1 : -2;
            cache->mUnk_14--;
            cache->mUnk_0c -= nodes[node].mUnk_4;
            func_0201c4f0(&nodes[node]);
            func_0202e04c(nodes, node);
            return;
        }
        if (slots[idx].mUnk_0 == -1) {
            return;
        }
        if (slots[idx].mUnk_6 == 0) {
            OS_Terminate();
        }
        slots[idx].mUnk_6--;
        if (slots[idx].mUnk_6 == 0 && slots[idx].mUnk_0 == -2) {
            slots[idx].mUnk_0 = -1;
        }
        idx += step;
        idx = (idx << (32 - cache->mUnk_18)) >> (32 - cache->mUnk_18);
    }
}

// Looks `key` up in the hash table. The search starts at the key's low bits and steps by an entry of data_02050378
// (1, then odd primes) chosen by the next 7 bits. A hit that was found past a removed slot moves into that slot (the
// path counts of the slots passed in between drop by one), and its node moves to the front of the list. A miss takes
// the first removed slot seen, or the free slot that ended the search, claims the least recently used node, raises
// the path counts along the way and calls the cache's load callback with `id`, the node, `size` and `offset`; a miss
// with id, offset and size all -1 only reports -1 and loads nothing.
void *func_0202e19c(u32 key, u32 id, u32 offset, u32 size, FileCache *cache) {
    u32 bits = cache->mUnk_18;
    FileCacheSlot *slots = cache->mUnk_00;
    FileCacheNode *nodes = cache->mUnk_04;
    u32 step, home, idx;
    u32 firstFree = -1;

    home = (key << (32 - bits)) >> (32 - bits);
    step = data_02050378[(key >> bits) & 0x7f];
    idx = home;

    for (;;) {
        s32 k = slots[idx].mUnk_0;

        if (key == k) {
            if (firstFree != -1) {
                u32 p = idx;

                if (idx != firstFree) {
                    do {
                        p = ((p - step) << (32 - cache->mUnk_18)) >> (32 - cache->mUnk_18);
                        slots[p].mUnk_6--;
                        if (slots[p].mUnk_6 == 0 && slots[p].mUnk_0 == -2) {
                            slots[p].mUnk_0 = -1;
                        }
                    } while (p != firstFree);
                }
                nodes[slots[idx].mUnk_4].mUnk_c = firstFree;
                slots[firstFree].mUnk_0 = slots[idx].mUnk_0;
                slots[firstFree].mUnk_4 = slots[idx].mUnk_4;
                slots[idx].mUnk_0 = slots[idx].mUnk_6 == 0 ? -1 : -2;
                idx = firstFree;
            }
            if (nodes[slots[idx].mUnk_4].mUnk_e == 0) {
                func_0202dff8(nodes, slots[idx].mUnk_4);
            }
            data_020c18bc.mUnk_33c = idx;
            return nodes[slots[idx].mUnk_4].mUnk_0;
        }
        if (k == -1) {
            u32 n;

            if ((id & offset & size) == -1) {
                return (void *) -1;
            }
            if (firstFree != -1) {
                idx = firstFree;
            }
            slots[idx].mUnk_0 = key;
            func_0202dff8(nodes, nodes[nodes[0].mUnk_8].mUnk_8);
            n = nodes[0].mUnk_a;
            slots[idx].mUnk_4 = n;
            nodes[n].mUnk_c = idx;
            while (idx != home) {
                idx = ((idx - step) << (32 - cache->mUnk_18)) >> (32 - cache->mUnk_18);
                slots[idx].mUnk_6++;
            }
            cache->mUnk_08(id, &nodes[n], size, offset);
            return nodes[n].mUnk_0;
        }
        if (k == -2 && firstFree == -1) {
            firstFree = idx;
        }
        idx = ((idx + step) << (32 - cache->mUnk_18)) >> (32 - cache->mUnk_18);
    }
}

void func_0202e3b4(u32 id, FileCacheNode *node, u32 size, u32 offset) {
    FileCacheSlot *slots = data_020c6470.mUnk_00;
    FileCacheNode *nodes = data_020c6470.mUnk_04;
    u32 i;

    node->mUnk_0 = func_0201c43c(size, 3);
    node->mUnk_0 = func_0202e56c(id, node->mUnk_0, size, 0);
    data_020c6470.mUnk_14++;
    data_020c6470.mUnk_0c += data_020c18bc.mUnk_338;
    node->mUnk_4 = data_020c18bc.mUnk_338;
    i = nodes[nodes[0].mUnk_8].mUnk_8;
    while (data_020c6470.mUnk_0c > data_020c6470.mUnk_10 || data_020c6470.mUnk_14 > data_020c6470.mUnk_16) {
        FileCacheNode *n;

        while (nodes[i].mUnk_0 == 0) {
            i = nodes[i].mUnk_8;
        }
        n = &nodes[i];
        i = n->mUnk_8;
        func_0202e0a4(slots[n->mUnk_c].mUnk_0, &data_020c6470);
    }
}

void func_0202e494(void) {
    FileCacheSlot *slots = data_020c6470.mUnk_00;
    FileCacheNode *nodes = data_020c6470.mUnk_04;
    u32 i;

    if (data_020c6470.mUnk_14 == 0) {
        OS_Terminate();
    }
    i = nodes[nodes[0].mUnk_8].mUnk_8;
    while (nodes[i].mUnk_0 == 0) {
        i = nodes[i].mUnk_8;
    }
    func_0202e0a4(slots[nodes[i].mUnk_c].mUnk_0, &data_020c6470);
}

void func_0202e4fc(u32 file, void *dst, u32 size, u32 offset) {
    u8 *data = func_0202e19c(file, -1, -1, -1, &data_020c6470);

    if (data != (u8 *) -1) {
        MI_CpuCopy8(data + offset, dst, size);
    } else {
        u32 pos = func_0202e794(file);

        func_0201d9a8(pos + offset);
        func_0201d9d4(dst, size);
    }
}

// The first bytes are read to see the tag. "EZ10" data is unpacked in place: it is read to the end of a new buffer
// that leaves room for the unpacked size (the buffer is at least as large as the margin word of the header asks for)
// and then unpacked to the start of it. "EZ00" and "EZ01" data and "QRK" data are read into `dst` and unpacked into a
// new buffer; `dst` is freed. Any other data stays in `dst`.
void *func_0202e56c(u32 file, void *dst, u32 size, u32 offset) {
    u32 hdr[FILE_HEADER_MAX];
    void *work;
    u32 n;
    u32 pad;
    u32 pos;
    u8 *tmp;

    if (size < ((data_020c18bc.mUnk_46c & SYSTEM_FLAG_NO_BANK) != 0 ? FILE_HEADER_MAX : FILE_HEADER_MIN)) {
        n = size;
    } else {
        n = (data_020c18bc.mUnk_46c & SYSTEM_FLAG_NO_BANK) != 0 ? FILE_HEADER_MAX : FILE_HEADER_MIN;
    }
    pos = func_0202e794(file);
    func_0201d9a8(pos + offset);
    func_0201d9d4(hdr, n);
    if (hdr[0] == EZ10_MAGIC) {
        u32 need = hdr[1] - size;

        func_0201c4f0(&dst);
        if (-hdr[2] > need) {
            need = -hdr[2];
        }
        pad = (u32) func_0203492c((void *) need);
        dst = func_0201c43c(pad + size, 3);
        tmp = (u8 *) dst + pad;
        MI_CpuCopy8(hdr, tmp, n);
        if (size > n) {
            func_0201d9d4(tmp + n, size - n);
        }
        func_0202875c(tmp, dst);
        pad += size;
        size = pad;
    } else {
        MI_CpuCopy8(hdr, dst, n);
        if (size > n) {
            func_0201d9d4((u8 *) dst + n, size - n);
        }
        if (hdr[0] == EZ00_MAGIC || hdr[0] == EZ00_MAGIC + 0x01000000) {
            void *out;

            size = hdr[1];
            out = func_0201c43c(size, 3);
            func_020282ec(dst, out);
            func_0201c4f0(&dst);
            dst = out;
        } else if ((hdr[0] & 0xffffff) == QRK_MAGIC) {
            void *out;

            size = func_02030744(dst);
            out = func_0201c43c(size, 3);
            work = func_0201c43c(func_02030760(), 3);
            func_02030700(dst, out, work);
            func_0201c4f0(&dst);
            func_0201c4f0(&work);
            dst = out;
        }
    }
    data_020c18bc.mUnk_338 = size;
    func_0202e924(dst, size);
    return dst;
}

u32 func_0202e760(u32 index) {
    if (data_020c18bc.mUnk_000 == NULL) {
        func_0202e7c4();
    }
    return ((Unk_FileEntry *) data_020c18bc.mUnk_000)[index].mUnk_4;
}

u32 func_0202e794(u32 index) {
    if (data_020c18bc.mUnk_000 == NULL) {
        func_0202e7c4();
    }
    return ((Unk_FileEntry *) data_020c18bc.mUnk_000)[index].mUnk_0;
}

// Sends a request packet (tag 0x50) until a reply arrives with tag 0x52, the expected sequence byte and the value 1;
// the reply carries the table's size, the table is then read from the start of the file system.
void func_0202e7c4(void) {
    u8 *reply = data_020c68e0;
    u8 *cmd = data_020c67e0;

    for (;;) {
        cmd[0] = 0x50;
        cmd[2] = 0;
        *(u16 *) (cmd + 4) = 1;
        func_0201bb30();
        if (data_020c66e0[0] == 0) {
            continue;
        }
        if (reply[0] != 0x52) {
            continue;
        }
        if (reply[2] != func_0203981c() || *(u16 *) (reply + 4) != 1) {
            continue;
        }
        break;
    }
    data_020c18bc.mUnk_334 = func_02034cf0(reply + 14);
    data_020c18bc.mUnk_000 = func_0201c43c(data_020c18bc.mUnk_334, 3);
    func_0201d9a8(0);
    func_0201d9d4(data_020c18bc.mUnk_000, data_020c18bc.mUnk_334);
}

u8 *func_0202e86c(int id) {
    int size = func_0202e9d4(id);
    u8 *buf;

    data_020c18bc.mUnk_338 = size >= 0 ? size : -size;
    buf = func_0201c43c(size >= 0 ? size : -size, 3);
    if (size > 0) {
        MI_CpuCopy8((void *) func_0202e988(id), buf, size);
    } else {
        buf = func_0202e56c(id, buf, -size, 0);
    }
    return buf;
}

void func_0202e8e4(void) {
    FileCacheNode *nodes = data_020c6470.mUnk_04;
    u32 i;

    (void) data_020c18bc.mUnk_45c;
    i = nodes[0].mUnk_a;
    while (nodes[i].mUnk_0 != 0) {
        i = nodes[i].mUnk_a;
    }
}

void func_0202e924(u8 *data, u32 size) {
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

int func_0202e988(int id) {
    void *data = func_0202e19c(id, -1, -1, -1, &data_020c6470);

    if (data != (void *) -1) {
        return (int) data;
    }
    return -func_0202e794(id);
}

int func_0202e9d4(int id) {
    void *data = func_0202e19c(id, -1, -1, -1, &data_020c6470);

    if (data == (void *) -1) {
        return -func_0202e760(id);
    }
    return data_020c6470.mUnk_04[data_020c6470.mUnk_00[data_020c18bc.mUnk_33c].mUnk_4].mUnk_4;
}

#pragma thumb on
