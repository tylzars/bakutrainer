#include <nitro/os.h>
#include <nitro/gx.h>
#include <Main/Unknown/functions.h>
#include <Main/Unknown/map_entries.h>
#include <Main/Unknown/camera.h>
#include <Main/Unknown/random_angle.h>
#include <Main/Unknown/engine_asm.h>

// The stage data file starts with MAP_FILE_MAGIC and MAP_FILE_VERSION, followed by chunks of 16 * n bytes whose
// first halfword is the chunk key (high byte MAP_CHUNK_TAG) and second halfword n; the first chunk follows a
// 16-byte header.
#define MAP_FILE_MAGIC 0x4d4d
#define MAP_FILE_VERSION 0x3730
#define MAP_CHUNK_TAG 0x55

static inline void PutTrans(fx32 x, fx32 y, fx32 z) {
    reg_G3X_MTX_TRANS = x;
    reg_G3X_MTX_TRANS = y;
    reg_G3X_MTX_TRANS = z;
}

static inline void PutScale(fx32 x, fx32 y, fx32 z) {
    reg_G3X_MTX_SCALE = x;
    reg_G3X_MTX_SCALE = y;
    reg_G3X_MTX_SCALE = z;
}

// Chunk 0x5513: the number of entries and the entries, sorted by group.
typedef struct MapEntryChunk {
    u8 mUnk_00[8];
    u16 mUnk_08;
    MapEntry mUnk_0a[1];
} MapEntryChunk;

typedef struct MapPoint {
    int x;
    int z;
} MapPoint;

// The map object's .bss block: two bit sets, indexed by cell number (mUnk_24) and by position in a rectangle
// (mUnk_28).
typedef struct MapState {
    u8 mUnk_00[0x24];
    u32 *mUnk_24;
    u32 *mUnk_28;
} MapState;

extern MapState data_020c3e44;

static int func_02031cc8(int x, int z);
static void func_02031fec(u16 group, u16 key, VecFx32 *out);
static MapEntry *func_02032074(u8 group, u8 key, u8 *count);
static MapEntry *func_0203219c(void);

// Returns the lowest address among the nodes of the ring of chunk 0x5540 that p belongs to (each node's halfword at
// 4 is the offset of the next node).
void *func_02031c70(void *p) {
    u16 *cur = p;
    u16 *base = func_02031eb8(0x5540);
    u16 *start = cur;
    u16 *min = cur;

    do {
        if (min > cur) {
            min = cur;
        }
        cur = base + cur[2];
    } while (cur != start);
    return min;
}

// Walks the same ring from entry, replacing each node's first halfword with (value & mask) | bits; returns the
// number of nodes.
int func_02031c98(u16 *entry, u16 mask, u16 bits) {
    u16 *base = func_02031eb8(0x5540);
    u16 *start = entry;
    int count = 0;

    do {
        count++;
        entry[0] = (entry[0] & mask) | bits;
        entry = base + entry[2];
    } while (entry != start);
    return count;
}

// 1 when (x, z) is within the view radius of the camera position.
static int func_02031cc8(int x, int z) {
    int dx = (data_020c5e44.mUnk_000.mUnk_80.x - x) / 0x1000;
    int dz = (data_020c5e44.mUnk_000.mUnk_80.z - z) / 0x1000;
    u32 r = (u32) data_020c5e44.mUnk_300.mUnk_44 >> 12;

    if ((u32) (dx * dx + dz * dz) <= r * r) {
        return 1;
    }
    return 0;
}

// Box test of a box around pos, scaled by a, b and c, against the view volume; 1 when any part is visible.
int func_02031d0c(VecFx32 *pos, int a, int b, int c) {
    u32 result;
    s16 box[6];

    if (func_02031cc8(pos->x, pos->z) == 0) {
        return 0;
    }
    if (data_020c5e44.mUnk_300.mUnk_4a != 0) {
        func_020334f8(0);
    }
    reg_G3X_MTX_MODE = 2;
    G3_LoadMtx43(&data_020c3e80);
    box[0] = -256;
    box[1] = 0;
    box[2] = -256;
    box[3] = 0x200;
    box[4] = 0x200;
    box[5] = 0x200;
    PutTrans(pos->x, pos->y, pos->z);
    PutScale(a << 2, b << 2, c << 2);
    reg_G3X_BEGIN_VTXS = 0;
    reg_G3X_END_VTXS = 0;
    reg_G3X_BOX_TEST = *(u32 *) &box[0];
    reg_G3X_BOX_TEST = *(u32 *) &box[2];
    reg_G3X_BOX_TEST = *(u32 *) &box[4];
    while (G3X_GetBoxTestResult(&result) != 0) {
    }
    if (result != 0) {
        return 1;
    }
    return 0;
}

// 1 when the map cell under (x, z) is marked in the first bit set, or lies in a rectangle of chunk 0x5530 whose
// bit in the second set is marked.
int func_02031dc4(s32 x, s32 z) {
    u32 grid = data_020c5e44.mUnk_000.mUnk_64;
    s32 cx = (u32) x / grid;
    s32 cz = (u32) z / grid;
    u16 *cells;
    u16 v;
    u8 *chunk;
    u8 *p;
    int n;
    int count;

    if (func_02031cc8(x, z) == 0) {
        return 0;
    }
    cells = func_02032898();
    v = cells[cx + cz * data_020c5e44.mUnk_000.mUnk_78] & 0x7fff;
    if (v != 0) {
        if (data_020c3e44.mUnk_24[v >> 5] & (1 << (v & 0x1f))) {
            return 1;
        }
    }
    chunk = (u8 *) func_02031eb8(0x5530);
    if (chunk == 0) {
        return 0;
    }
    count = chunk[0];
    p = chunk + 2;
    n = count - 1;
    if (count != 0) {
        do {
            int w = p[0];
            int h = p[1];
            int x0 = *(u16 *) (p + 2);
            int y0 = *(u16 *) (p + 4);

            if (cx >= x0 && cx < x0 + w && cz >= y0 && cz < y0 + h) {
                u32 idx;
                u32 sh;

                cz -= y0;
                cx -= x0;
                sh = 32 - func_02028250(w);
                idx = cx + (cz << sh);
                if (data_020c3e44.mUnk_28[idx >> 5] & (1 << (idx & 0x1f))) {
                    return 1;
                }
                return 0;
            }
            p += w * h * 2 + 6;
        } while (n-- != 0);
    }
    return 0;
}

// Returns the data of the chunk with key id, or 0 when the file has none. Loads the file when it is not loaded.
#pragma thumb off
u16 *func_02031eb8(u32 id) {
    u16 *p = data_020c5e44.mUnk_000.mUnk_6c;
    int key;

    if (p == 0 || p[0] != MAP_FILE_MAGIC) {
        p = func_0202ded8(data_020c5e44.mUnk_000.mUnk_74);
        data_020c5e44.mUnk_000.mUnk_6c = p;
        data_020c5e44.mUnk_300.mUnk_50 = func_02031eb8(0x5511);
    }
    if (data_020c5e44.mUnk_000.mUnk_70 != data_020c5e44.mUnk_000.mUnk_6c) {
        data_020c5e44.mUnk_000.mUnk_70 = data_020c5e44.mUnk_000.mUnk_6c;
        func_02032b88(0);
    }
    if (p[0] != MAP_FILE_MAGIC) {
        OS_Terminate();
    }
    if (p[1] != MAP_FILE_VERSION) {
        OS_Terminate();
    }
    p += 8;
    key = *p;
    while (key != id) {
        if ((key >> 8) != MAP_CHUNK_TAG) {
            return 0;
        }
        p += p[1] * 8;
        key = *p;
    }
    return p + 2;
}
#pragma thumb on

s32 func_02031f98(MapEntry *entry) {
    return entry->mUnk_04[0] + (entry->mUnk_04[1] << 16);
}

s32 func_02031fa4(MapEntry *entry) {
    return entry->mUnk_08[0] + (entry->mUnk_08[1] << 16);
}

s32 func_02031fb0(MapEntry *entry) {
    return entry->mUnk_0c[0] + (entry->mUnk_0c[1] << 16);
}

u16 func_02031fbc(MapEntry *entry) {
    return entry->mUnk_02 << 8;
}

// Position of the entry with id (group in the high byte, key in the low byte) in chunk 0x5513.
void func_02031fc4(u32 id, VecFx32 *out) {
    if (id & 0xffff0000) {
        OS_Terminate();
    }
    func_02031fec(id >> 8, id & 0xff, out);
}

static void func_02031fec(u16 group, u16 key, VecFx32 *out) {
    MapEntry *entry = func_02032074(group, key, 0);

    out->x = func_02031f98(entry);
    out->y = func_02031fa4(entry);
    out->z = func_02031fb0(entry);
}

// Position (and flags, when asked for) of the index'th entry of chunk 0x5513.
void func_02032018(u32 index, VecFx32 *pos, u8 *flags) {
    MapEntry *entry = func_0203219c() + index;

    pos->x = func_02031f98(entry);
    pos->y = func_02031fa4(entry);
    pos->z = func_02031fb0(entry);
    if (flags != 0) {
        *flags = entry->mUnk_02;
    }
}

// The entry of chunk 0x5513 with id (group in the high byte, key in the low byte); *count receives the number of
// entries when given.
MapEntry *func_0203204c(u32 a, u8 *count) {
    if (a & 0xffff0000) {
        OS_Terminate();
    }
    return func_02032074((u8) (a >> 8), (u8) a, count);
}

static MapEntry *func_02032074(u8 group, u8 key, u8 *count) {
    MapEntryChunk *chunk = (MapEntryChunk *) func_02031eb8(0x5513);
    MapEntry *entry;
    int i;
    int n;

    if (chunk == 0) {
        OS_Terminate();
    }
    entry = chunk->mUnk_0a;
    if (count != 0) {
        *count = chunk->mUnk_08;
    }
    n = chunk->mUnk_08;
    for (i = 0; i < n; i++) {
        if (entry->mUnk_03 > group) {
            break;
        }
        if (entry->mUnk_03 == group && entry->mUnk_00 == key) {
            return entry;
        }
        entry++;
    }
    return 0;
}

// 1 when pos lies on the inner side of every edge of one of the polygons that func_02032244 selects for (a, b).
int func_020320c0(int a, int b, VecFx32 *pos) {
    MapEntry *entries;
    u16 *verts;
    u32 j;
    u16 keys[48];
    u32 nverts;
    MapPoint p1;
    MapPoint p0;
    u32 count;
    u32 i;
    int px;
    int pz;

    entries = func_0203219c();
    count = func_02032244(a, b, keys, 0x30);
    px = pos->x >> 12;
    pz = pos->z >> 12;
    for (i = 0; i < count; i++) {
        verts = func_020321ac(keys[i] >> 8, keys[i] & 0xff, &nverts);
        if (nverts < 3) {
            continue;
        }
        p1.x = func_02031f98(entries + verts[0]) >> 12;
        p1.z = func_02031fb0(entries + verts[0]) >> 12;
        verts++;
        for (j = 1; j < nverts; verts++, j++) {
            p0 = p1;
            p1.x = func_02031f98(entries + verts[0]) >> 12;
            p1.z = func_02031fb0(entries + verts[0]) >> 12;
            if (func_0201febc(p0.x, p0.z, p1.x, p1.z, px, pz) > 0) {
                break;
            }
        }
        if (j == nverts) {
            return 1;
        }
    }
    return 0;
}

static MapEntry *func_0203219c(void) {
    MapEntryChunk *chunk = (MapEntryChunk *) func_02031eb8(0x5513);

    return chunk->mUnk_0a;
}
