#include <nitro/os.h>
#include <nitro/gx.h>
#include <Main/Unknown/functions.h>
#include <Main/Unknown/map_entries.h>
#include <Main/Unknown/camera.h>
#include <Main/Unknown/random_angle.h>
#include <Main/Unknown/engine_asm.h>
#include <Main/Unknown/game_math.h>
#include <Main/Unknown/system_init.h>

// The stage data file starts with MAP_FILE_MAGIC and MAP_FILE_VERSION, followed by chunks of 16 * n bytes whose
// first halfword is the chunk key (high byte MAP_CHUNK_TAG) and second halfword n; the first chunk follows a
// 16-byte header.
#define MAP_FILE_MAGIC 0x4d4d
#define MAP_FILE_VERSION 0x3730
#define MAP_CHUNK_TAG 0x55

// func_02032368: the top two bits of the flags select how many limits apply; the result is the low ten bits plus
// the class of the farthest limit the rectangle lies beyond.
#define MAP_LOD_MASK 0xc000
#define MAP_LOD_SHIFT 14
#define MAP_LOD_KEEP 0x3ff
#define MAP_LOD_UNIT 10000
#define MAP_LOD_1 0x8001
#define MAP_LOD_2 0x8002
#define MAP_LOD_3 0x8003

#define MAP_VIEWPORT_FULL 0xbfff0000

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

// One polygon of chunk 0x5516: the index of its first vertex in the vertex list (chunk 0x5517), its group and key.
typedef struct MapPoly {
    u16 mUnk_00;
    u8 mUnk_02;
    u8 mUnk_03;
} MapPoly;

// An entry of the sorted table at MapState.mUnk_1c: a key and the value it maps to.
typedef struct MapPair {
    u16 mUnk_0;
    u16 mUnk_2;
} MapPair;

// The map object's .bss block: two bit sets, indexed by cell number (mUnk_24) and by position in a rectangle
// (mUnk_28), the squared distance limits, the sorted pair table with its search step and the buffers the map code
// allocates.
typedef struct MapState {
    s32 mUnk_00;
    u32 mUnk_04[3];
    u8 mUnk_10;
    u8 mUnk_11[3];
    u16 mUnk_14;
    u16 mUnk_16;
    u16 mUnk_18;
    u8 mUnk_1a[2];
    MapPair *mUnk_1c;
    u8 mUnk_20;
    u8 mUnk_21;
    u8 mUnk_22[2];
    u32 *mUnk_24;
    u32 *mUnk_28;
    void *mUnk_2c;
    void *mUnk_30;
    u16 mUnk_34;
    u8 mUnk_36[2];
    u16 mUnk_38;
    u8 mUnk_3a[2];
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

// The vertex index list of the polygon of chunk 0x5516 with group a and key b (-1 matches any); *count receives its
// vertex count and the matched group and key are kept in the camera object.
u16 *func_020321ac(int a, int b, u32 *count) {
    MapPoly *p = (MapPoly *) func_02031eb8(0x5516);
    u16 n;

    if (p != NULL) {
        for (n = (((u16 *) p)[-1] * 16 - 4) >> 2; n != 0; n--, p++) {
            if (a != -1 && p->mUnk_02 > a) {
                if (count != NULL) {
                    *count = 0;
                }
                return NULL;
            }
            if ((a == -1 || p->mUnk_02 == a) && (b == -1 || p->mUnk_03 == b)) {
                data_020c5e44.mUnk_000.mUnk_60 = p->mUnk_02;
                data_020c5e44.mUnk_000.mUnk_61 = p->mUnk_03;
                if (count != NULL) {
                    *count = p[1].mUnk_00 - p->mUnk_00;
                }
                return func_02031eb8(0x5517) + p->mUnk_00;
            }
        }
    }
    return NULL;
}

// Stores the group and key of up to max polygons matching (a, b) (-1 matches any) in out; returns how many.
u32 func_02032244(int a, int b, u16 *out, u32 max) {
    MapPoly *p = (MapPoly *) func_02031eb8(0x5516);
    u32 n;
    u32 found = 0;

    if (p != NULL) {
        for (n = (u32) (((u16 *) p)[-1] * 16 - 4) >> 2; n != 0 && (a == -1 || p->mUnk_02 <= a); p++, n--) {
            if ((a == -1 || p->mUnk_02 == a) && (b == -1 || p->mUnk_03 == b)) {
                *out = (p->mUnk_02 << 8) | p->mUnk_03;
                found++;
                out++;
                if (found >= max) {
                    break;
                }
            }
        }
    }
    return found;
}

// Replaces the pair table with an empty one of count entries and marks it unsorted.
void func_020322b8(u16 count) {
    if (count != 0) {
        if (data_020c3e44.mUnk_1c != NULL) {
            func_0201c4f0(&data_020c3e44.mUnk_1c);
        }
        data_020c3e44.mUnk_1c = func_0201c430(count * 4);
        data_020c3e44.mUnk_14 = 0;
        data_020c3e44.mUnk_18 = count;
        data_020c3e44.mUnk_20 = 1;
        data_020c3e44.mUnk_21 = 0;
    }
}

// The distance in cells from the camera's map position to the rectangle (x, z) of w by h cells, 0 when it lies inside.
u32 func_020322f4(int x, int z, int w, int h) {
    u32 k = data_020c5e44.mUnk_000.mUnk_64;
    int cw = k * w;
    int ch = k * h;
    int cx = data_020c5e44.mUnk_000.mUnk_80.x;

    if (cx < x) {
        int cz = data_020c5e44.mUnk_000.mUnk_80.z;
        if (cz >= z) {
            z += ch;
            if (cz < z) {
                z = cz;
            }
        }
    } else if (cx < x + cw) {
        int cz = data_020c5e44.mUnk_000.mUnk_80.z;
        if (cz < z) {
            x = cx;
        } else {
            z += ch;
            if (cz < z) {
                return 0;
            }
            x = cx;
        }
    } else {
        int cz = data_020c5e44.mUnk_000.mUnk_80.z;
        x += cw;
        if (cz >= z) {
            z += ch;
            if (cz < z) {
                z = cz;
            }
        }
    }
    return func_0201fae4((x - cx) / 512, (z - data_020c5e44.mUnk_000.mUnk_80.z) / 512);
}

// Classifies the rectangle's distance from the camera against the squared limits (rebuilt when the camera's value at
// 0x344 changed): the low ten bits of flags plus a class, or just the low bits when the rectangle is inside.
u32 func_02032368(u32 flags, int x, int z, int w, int h) {
    if (flags & MAP_LOD_MASK) {
        u32 d;
        u32 code;
        int i;
        int base;
        int t;
        int k;

        if (data_020c3e44.mUnk_00 != data_020c5e44.mUnk_300.mUnk_44) {
            base = (data_020c3e44.mUnk_10 * 5 * 256) / 256 - 256;
            k = 0;
            t = 0;
            for (i = 0; i < 3; i++) {
                int v;
                int r;
                t += 256;
                v = func_0201fab0(0, 0x400, base, t);
                r = ((u32) data_020c5e44.mUnk_300.mUnk_44 >> 6) * 41 >> 15;
                v = v * r / 1024;
                // Stored through the object's address as words (the same words as mUnk_04[k]): with a member store the
                // compiler keeps data_020c5e44.mUnk_300.mUnk_44 in a register; the code reloads it.
                ((u32 *) &data_020c3e44)[1 + k++] = v * v;
            }
            data_020c3e44.mUnk_00 = data_020c5e44.mUnk_300.mUnk_44;
        }
        d = func_020322f4(x, z, w, h);
        if (d == 0) {
            return flags & MAP_LOD_KEEP;
        }
        code = 0;
        switch (flags >> MAP_LOD_SHIFT) {
            case 1:
                if (d > MAP_LOD_UNIT * data_020c3e44.mUnk_04[0]) {
                    code = MAP_LOD_1;
                }
                break;
            case 2:
                if (d > MAP_LOD_UNIT * data_020c3e44.mUnk_04[1]) {
                    code = MAP_LOD_2;
                } else if (d > MAP_LOD_UNIT * data_020c3e44.mUnk_04[0]) {
                    code = MAP_LOD_1;
                }
                break;
            case 3:
                if (d > MAP_LOD_UNIT * data_020c3e44.mUnk_04[2]) {
                    code = MAP_LOD_3;
                } else if (d > MAP_LOD_UNIT * data_020c3e44.mUnk_04[1]) {
                    code = MAP_LOD_2;
                } else if (d > MAP_LOD_UNIT * data_020c3e44.mUnk_04[0]) {
                    code = MAP_LOD_1;
                }
                break;
        }
        return (flags & MAP_LOD_KEEP) + code;
    }
    return flags;
}

// Maps key through the sorted pair table (sorting it on first use); key itself when it has no entry.
u32 func_0203249c(u32 key) {
    if (data_020c3e44.mUnk_21 == 0 && data_020c3e44.mUnk_14 != 0) {
        if (data_020c3e44.mUnk_20 == 0) {
            MapPair *e;
            int gap;

            e = data_020c3e44.mUnk_1c;
            gap = 1 << (31 - func_02028250(data_020c3e44.mUnk_14));
            do {
                int i;
                for (i = 0; i < data_020c3e44.mUnk_14 - gap; i++) {
                    int j;
                    for (j = i; j >= 0 && e[j].mUnk_0 > e[j + gap].mUnk_0; j -= gap) {
                        MapPair tmp = e[j];
                        e[j] = e[j + gap];
                        e[j + gap] = tmp;
                    }
                }
                gap >>= 1;
            } while (gap > 0);
            while (data_020c3e44.mUnk_14 != 0 && e[data_020c3e44.mUnk_14 - 1].mUnk_0 == 0xffff) {
                data_020c3e44.mUnk_14--;
            }
            data_020c3e44.mUnk_16 = 1 << (31 - func_02028250(data_020c3e44.mUnk_14));
            data_020c3e44.mUnk_20 = 1;
        }
        {
            int n = data_020c3e44.mUnk_14;
            MapPair *e = data_020c3e44.mUnk_1c;
            int step = data_020c3e44.mUnk_16;
            int pos = 0;
            do {
                while (pos + step < n && e[pos + step].mUnk_0 <= key) {
                    pos += step;
                }
                step >>= 1;
            } while (step != 0);
            if (key == e[pos].mUnk_0) {
                return e[pos].mUnk_2;
            }
        }
    }
    return key;
}

// Starts the map: initialises with a, b, c and the pair table size d and stores the camera's two halfwords at 0x24.
void func_020325b0(u16 a, u16 b, u16 c, u16 d, u16 e, u16 f) {
    func_020325d0(a, b, c, d);
    data_020c5e44.mUnk_000.mUnk_24 = e;
    data_020c5e44.mUnk_000.mUnk_26 = f;
}

// Resets the map object and the camera for a new stage, sets up the 3D display and loads the buffers and the two
// camera positions from the stage data.
void func_020325d0(u16 a, u16 b, u16 c, u16 count) {
    u16 *hdr;
    u16 *verts;
    MapEntry *entry;

    func_02032850();
    data_020c5e44.mUnk_000.mUnk_6c = NULL;
    data_020c5e44.mUnk_000.mUnk_70 = NULL;
    data_020c5e44.mUnk_000.mUnk_74 = a;
    data_020c5e44.mUnk_000.mUnk_76 = b;
    data_020c5e44.mUnk_000.mUnk_7c = c;
    *(vu32 *) 0x04000060 &= ~0x20;
    reg_GX_DISP3DCNT &= ~0x3002;
    *(vu16 *) 0x04000050 = 0;
    reg_GX_DISP3DCNT = (u16) ((reg_GX_DISP3DCNT & ~0x3000) | 0x10);
    *(vu32 *) 0x04000540 = 0;
    G3X_SetClearColor(0, 0x1f, 0x7fff, 0, 0);
    reg_G3X_VIEWPORT = MAP_VIEWPORT_FULL;
    GX_SetGraphicsMode(1, 4, 1);
    reg_GX_DISPCNT = (reg_GX_DISPCNT & ~0x1f00) | 0x1100;
    reg_G2_BG0CNT = (reg_G2_BG0CNT & ~3) | 1;
    hdr = func_02031eb8(0x5513);
    if (hdr == NULL) {
        OS_Terminate();
    }
    data_020c5e44.mUnk_000.mUnk_78 = hdr[0];
    data_020c5e44.mUnk_000.mUnk_7a = hdr[1];
    data_020c5e44.mUnk_000.mUnk_64 = (hdr[3] << 16) + hdr[2];
    verts = func_02031eb8(0x5511);
    data_020c5e44.mUnk_300.mUnk_50 = verts;
    data_020c3e44.mUnk_34 = (((verts[-1] * 8) / 8 + 16) & ~3);
    data_020c3e44.mUnk_24 = func_0201c430(data_020c3e44.mUnk_34);
    data_020c3e44.mUnk_28 = func_0201c430(0x400);
    data_020c3e44.mUnk_2c = func_0201c430(0x1200);
    data_020c3e44.mUnk_30 = func_0201c430(0x9c40);
    data_020c5e44.mUnk_300.mUnk_56 = 0;
    entry = func_0203204c(0, NULL);
    if (entry != NULL) {
        data_020c5e44.mUnk_0c0.mUnk_204.x = func_02031f98(entry);
        data_020c5e44.mUnk_0c0.mUnk_204.y = func_02031fa4(entry);
        data_020c5e44.mUnk_0c0.mUnk_204.z = func_02031fb0(entry);
    } else {
        data_020c5e44.mUnk_0c0.mUnk_204.x = 0x32000;
        data_020c5e44.mUnk_0c0.mUnk_204.y = 0x32000 >> 2;
        data_020c5e44.mUnk_0c0.mUnk_204.z = 0x32000;
    }
    data_020c5e44.mUnk_300.mUnk_32 = 1;
    data_020c5e44.mUnk_0c0.mUnk_21c = data_020c5e44.mUnk_0c0.mUnk_204;
    entry = func_0203204c(1, NULL);
    if (entry != NULL) {
        data_020c5e44.mUnk_0c0.mUnk_228.x = func_02031f98(entry);
        data_020c5e44.mUnk_0c0.mUnk_228.y = func_02031fa4(entry);
        data_020c5e44.mUnk_0c0.mUnk_228.z = func_02031fb0(entry);
    } else {
        data_020c5e44.mUnk_0c0.mUnk_228.x = 0x4b000;
        data_020c5e44.mUnk_0c0.mUnk_228.y = 0x19000;
        data_020c5e44.mUnk_0c0.mUnk_228.z = 0x4b000;
    }
    data_020c5e44.mUnk_300.mUnk_33 = 8;
    *(VecFx32 *) &data_020c5e44.mUnk_300 = data_020c5e44.mUnk_0c0.mUnk_228;
    data_020c3e44.mUnk_38 = 0;
    data_020c5e44.mUnk_300.mUnk_54 = 0x190;
    data_020c5e44.mUnk_300.mUnk_30 = 0x3c00;
    data_020c5e44.mUnk_300.mUnk_0f = 1;
    data_020c5e44.mUnk_300.mUnk_11[0] = 4;
    data_020c5e44.mUnk_300.mUnk_0e = 0;
    data_020c5e44.mUnk_300.mUnk_10 = 0;
    data_020c5e44.mUnk_300.mUnk_11[1] = 0;
    data_020c5e44.mUnk_300.mUnk_14 = 0xe38;
    data_020c5e44.mUnk_300.mUnk_16 = data_020c5e44.mUnk_300.mUnk_14;
    data_020c5e44.mUnk_300.mUnk_18 = data_020c5e44.mUnk_300.mUnk_14;
    data_020c5e44.mUnk_000.mUnk_8c = 1;
    data_020c5e44.mUnk_000.mUnk_90 = 0x1000;
    data_020c5e44.mUnk_000.mUnk_94 = 0;
    data_020c5e44.mUnk_000.mUnk_80.x = data_020c5e44.mUnk_0c0.mUnk_204.x;
    data_020c5e44.mUnk_000.mUnk_80.y = data_020c5e44.mUnk_0c0.mUnk_204.y;
    data_020c5e44.mUnk_000.mUnk_80.z = data_020c5e44.mUnk_0c0.mUnk_204.z;
    data_020c5e44.mUnk_000.mUnk_98.x = data_020c5e44.mUnk_0c0.mUnk_228.x;
    data_020c5e44.mUnk_000.mUnk_98.y = data_020c5e44.mUnk_0c0.mUnk_228.y;
    data_020c5e44.mUnk_000.mUnk_98.z = data_020c5e44.mUnk_0c0.mUnk_228.z;
    data_020c5e44.mUnk_300.mUnk_40 = 0;
    data_020c5e44.mUnk_300.mUnk_42[0] = 0;
    data_020c5e44.mUnk_300.mUnk_44 = 0x1f4000;
    data_020c5e44.mUnk_300.mUnk_64 = 0;
    data_020c3e44.mUnk_00 = -1;
    data_020c3e44.mUnk_10 = 0x80;
    data_020c5e44.mUnk_000.mUnk_28 = NULL;
    func_020322b8(count);
    data_020c5e44.mUnk_300.mUnk_5a = 0;
    data_020c5e44.mUnk_300.mUnk_5c = 0;
    data_020c5e44.mUnk_000.mUnk_7f &= ~2;
    data_020c18bc.mUnk_46c &= ~0x10000;
}

// Frees the map buffers and clears the records.
void func_02032850(void) {
    data_020c3e44.mUnk_14 = 0;
    func_02032b88(1);
    func_0201c4f0(&data_020c3e44.mUnk_1c);
    func_0201c4f0(&data_020c3e44.mUnk_24);
    func_0201c4f0(&data_020c3e44.mUnk_28);
    func_0201c4f0(&data_020c3e44.mUnk_2c);
    func_0201c4f0(&data_020c3e44.mUnk_30);
}

// The cell table of chunk 0x5510; terminates when the chunk is missing.
u16 *func_02032898(void) {
    u16 *cells = func_02031eb8(0x5510);

    if (cells == NULL) {
        OS_Terminate();
    }
    return cells;
}
