#include <nitro/os.h>
#include <nitro/gx.h>
#include <nitro/mi.h>
#include <Main/Unknown/functions.h>
#include <Main/Unknown/map_entries.h>
#include <Main/Unknown/camera.h>
#include <Main/Unknown/random_angle.h>
#include <Main/Unknown/engine_asm.h>
#include <Main/Unknown/game_math.h>
#include <Main/Unknown/system_init.h>
#include <Main/Unknown/game_util.h>
#include <Main/Unknown/model.h>
#include <Main/clib.h>

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

// One cell of the grid (mUnk_30 in MapState, 4 bytes per cell): a direction to the target and, in mUnk_3, how the cell
// was reached (0 = not visited).
typedef struct MapCell {
    s8 mUnk_0;
    s8 mUnk_1;
    s8 mUnk_2;
    u8 mUnk_3;
} MapCell;

// One area of chunk 0x5530: its size in cells, the position of its first cell and w * h halfwords, one per cell
// (the low ten bits name an object, the top four bits tell which neighbours are connected).
typedef struct MapArea {
    u8 mUnk_00;
    u8 mUnk_01;
    u16 mUnk_02;
    u16 mUnk_04;
    u16 mUnk_06[1];
} MapArea;

// A visible cell found by func_02032d50: the key of its object and its position.
typedef struct MapMarker {
    u16 mUnk_00;
    u16 mUnk_02;
    s32 mUnk_04;
    s32 mUnk_08;
} MapMarker;

// The map object's .bss block: two bit sets, indexed by cell number (mUnk_24) and by position in a rectangle
// (mUnk_28), the squared distance limits, the sorted pair table with its search step, the buffers the map code
// allocates, the view matrices (0x3c, 0x6c with its translation at 0x90, and 0x9c, reached through data_020c3e80,
// data_020c3eb0 and data_020c3ee0) and the placed objects. The 20 records end at 0x2024, 0x24 bytes past the camera
// object at 0x020c5e44 (data_020c3e44 + 0x2000); no code found reaches the camera object below its 0x24.
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
    MapMarker *mUnk_2c;
    MapCell *mUnk_30;
    u16 mUnk_34;
    u16 mUnk_36;
    u16 mUnk_38;
    u8 mUnk_3a[2];
    u8 mUnk_3c[0x90 - 0x3c];
    VecFx32 mUnk_90;
    u8 mUnk_9c[0xdc - 0x9c];
    s32 mUnk_dc;
    s32 mUnk_e0;
    MapRecord mUnk_e4[MAP_RECORDS];
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
        MapState *st = &data_020c3e44;

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
                st->mUnk_04[k++] = v * v;
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
    data_020c5e44.mUnk_000.mUnk_7f_1 = 0;
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

// Runs the actions of the object's definition: each halfword (kind in the top bit, a 6-bit action and a 6-bit
// animation) may call the camera's callback and picks the animation to show; starts it when it changed.
void func_020328b0(MapRecord *rec) {
    u16 anim = 0x3f;
    int kind = 0;
    int i = 0;

    for (; i < rec->mUnk_00->mUnk_03; i++) {
        int v = rec->mUnk_00->mUnk_04[i];
        int a = v & 0x3f;
        u16 b = (v >> 6) & 0x3f;

        if (v >> 15) {
            if (data_020c5e44.mUnk_000.mUnk_28 == NULL) {
                OS_Terminate();
            }
            data_020c5e44.mUnk_000.mUnk_28(a, rec, (u16) (b + data_020c5e44.mUnk_000.mUnk_26));
        }
        if (a == 0x3f || (data_020c5e44.mUnk_000.mUnk_68 & (1 << a))) {
            anim = b;
            kind = (rec->mUnk_00->mUnk_04[i] >> 12) & 7;
        }
    }
    if (anim != 0x3f) {
        anim += data_020c5e44.mUnk_000.mUnk_26;
    } else {
        anim = 0;
    }
    if (rec->mUnk_150 != anim) {
        if (rec->mUnk_176_10 == 1 || rec->mUnk_150 == 0) {
            func_02027230(rec->mUnk_04, rec->mUnk_174, anim, 2, 0);
        }
    }
    if (rec->mUnk_176_10 == 1) {
        switch (kind) {
            case 1:
                rec->mUnk_152--;
                rec->mUnk_176_9 = 1;
                rec->mUnk_176_7 = 1;
                break;
            case 2:
                rec->mUnk_18a = 1;
                break;
            case 3:
                break;
        }
    }
}

// Handles the object definition list at objs: for each definition, finds the object already placed for it or places
// a new one from its map entry, and runs the definition's actions.
void func_020329d0(u8 *p) {
    int n = *p++;
    MapRecord *rec;
    MapRecord *fresh = NULL;
    int i;

    while (n-- != 0) {
        MapObjDef *def = (MapObjDef *) p;

        for (i = 0; i < MAP_RECORDS; i++) {
            rec = &data_020c3e44.mUnk_e4[i];
            if (rec->mUnk_00 == def) {
                break;
            }
        }
        if (i == MAP_RECORDS) {
            u8 *entries = (u8 *) func_02031eb8(0x5513) + 0xa;
            MapRecord *r;
            int off;

            r = data_020c3e44.mUnk_e4;
            for (i = 0; i < MAP_RECORDS; i++, r++) {
                if (r->mUnk_00 == NULL) {
                    fresh = r;
                    break;
                }
            }
            if (i == MAP_RECORDS) {
                return;
            }
            fresh->mUnk_00 = def;
            fresh->mUnk_174 = def->mUnk_02 + data_020c5e44.mUnk_000.mUnk_24;
            rec = fresh;
            fresh->mUnk_176_10 = 0;
            fresh->mUnk_176_4 = 0;
            fresh->mUnk_176_9 = 0;
            fresh->mUnk_176_12 = 0;
            off = fresh->mUnk_00->mUnk_00 * 16;
            fresh->mUnk_17c.x = func_02031f98((MapEntry *) (entries + off));
            fresh->mUnk_17c.y = func_02031fa4((MapEntry *) (entries + off));
            fresh->mUnk_17c.z = func_02031fb0((MapEntry *) (entries + off));
            fresh->mUnk_188 = func_02031fbc((MapEntry *) (entries + off));
            fresh->mUnk_18a = 0;
            fresh->mUnk_18c = 0;
            fresh->mUnk_18b = 0;
            fresh->mUnk_150 = 0;
        }
        if (rec->mUnk_18a == 0 && rec->mUnk_18b == 0) {
            DrawParams dp;
            int flags = 0;

            dp.mUnk_43_4 = 0;
            dp.mUnk_43_3 = dp.mUnk_43_4;
            dp.mUnk_38 = rec->mUnk_188;
            dp.mUnk_3c = 0;
            dp.mUnk_3a = dp.mUnk_3c;
            dp.mUnk_43 = 0;
            dp.mPos = rec->mUnk_17c;
            dp.mUnk_30 = -1;
            dp.mUnk_34 = -1;
            dp.mUnk_2c = -1;
            if (rec->mUnk_18c != 0) {
                flags = (rec->mUnk_18c << 27) | 2;
            }
            func_020328b0(rec);
            if (rec->mUnk_18a == 0) {
                data_020c5e44.mUnk_300.mUnk_60++;
                func_0202733c(rec->mUnk_04, &dp, flags);
            }
        }
        p += def->mUnk_03 * 2 + 4;
    }
}

// Clears the 20 records (and the camera's halfwords at 0x24 and 0x26 when a == 1).
void func_02032b88(int a) {
    int i;

    for (i = 0; i < MAP_RECORDS; i++) {
        data_020c3e44.mUnk_e4[i].mUnk_00 = NULL;
    }
    if (a == 1) {
        data_020c5e44.mUnk_000.mUnk_24 = 0;
        data_020c5e44.mUnk_000.mUnk_26 = 0;
    }
}

// Marks the cell next to (x0, z0) on the way to (x1, z1) in the grid of stride w; 1 when it did, 0 when no cell on
// the way is marked.
int func_02032bb8(int x0, int z0, int w, int x1, int z1) {
    int dx = x1 - x0;
    int dz = z1 - z0;
    int stepx;
    int stepz;
    int px;
    int nx;
    int nz;
    int pz;
    int k;
    int fx;
    int fz;
    u8 kind;
    MapCell *c;
    MapCell *cur;

    if (abs(dx) > abs(dz)) {
        stepx = dx;
        stepz = 0;
    } else if (abs(dx) < abs(dz)) {
        stepx = 0;
        stepz = dz;
    } else {
        stepz = 0;
        stepx = 0;
    }
    // The results of the next abs calls are not used; the calls stay because abs is an ordinary function.
    if (abs(dx) < abs(dz)) {
        abs(dx * 2);
        abs(dz);
    } else if (abs(dx) > abs(dz)) {
        abs(dx);
        abs(dz * 2);
    }
    k = 1;
    px = x0 + (dx != 0 ? dx >> 31 | 1 : 0);
    pz = z0 + (dz != 0 ? dz >> 31 | 1 : 0);
    c = data_020c3e44.mUnk_30 + (px + pz * w);
    kind = c->mUnk_3;
    if (kind == 0) {
        k = 2;
        fx = px;
        fz = pz;
        nx = x0 + (stepx != 0 ? stepx >> 31 | 1 : 0);
        nz = z0 + (stepz != 0 ? stepz >> 31 | 1 : 0);
        c = data_020c3e44.mUnk_30 + (nx + nz * w);
        kind = c->mUnk_3;
        if (kind == 0) {
            return 0;
        }
    }
    cur = data_020c3e44.mUnk_30 + (x0 + z0 * w);
    if (kind > 1) {
        int sign = c->mUnk_0 * (z0 - z1) - c->mUnk_1 * (x0 - x1) != 0
                       ? (c->mUnk_0 * (z0 - z1) - c->mUnk_1 * (x0 - x1)) >> 31 | 1
                       : 0;

        k = 2;
        if (sign != c->mUnk_2) {
            return 0;
        }
        cur->mUnk_0 = c->mUnk_0;
        cur->mUnk_1 = c->mUnk_1;
        cur->mUnk_2 = c->mUnk_2;
    } else if (k == 2) {
        cur->mUnk_0 = fx - x1;
        cur->mUnk_1 = fz - z1;
        cur->mUnk_2 = (z0 - z1) * cur->mUnk_0 - (x0 - x1) * cur->mUnk_1 != 0
                          ? ((z0 - z1) * cur->mUnk_0 - (x0 - x1) * cur->mUnk_1) >> 31 | 1
                          : 0;
    }
    cur->mUnk_3 = k;
    return 1;
}

// Row and column steps to the four neighbours: direction j steps by (sMapDirs[j], sMapDirs[j + 1]).
static const s8 sMapDirs[5] = {-1, 0, 1, 0, -1};

// Walks the cells of the area of chunk 0x5530 that holds the grid cell of (px, pz), outward from that cell. Each cell
// the box test finds inside the view is listed with its position; the grid of func_02032bb8 records how it was reached.
void func_02032d50(u32 px, u32 pz) {
    u16 saved = data_020c3e44.mUnk_36;
    MapArea *area;
    int left;
    int cx;
    int cz;
    u32 scale;
    int w;
    int h;
    int x0;
    int z0;
    int shift;
    int head;
    int tail;
    u32 idx0;
    u32 result;
    s16 box[6];
    u16 queue[256];
    u32 q;
    u32 row;
    u32 col;
    u16 v;
    u32 m;
    int j;
    u32 x;
    u32 z;
    u32 i;
    int n;

    if (!data_020c5e44.mUnk_000.mUnk_7f_3) {
        return;
    }
    area = (MapArea *) func_02031eb8(0x5530);
    if (area == NULL) {
        OS_Terminate();
    }
    scale = data_020c5e44.mUnk_000.mUnk_64;
    cx = px / scale;
    cz = pz / scale;
    n = *(u8 *) area;
    area = (MapArea *) ((u8 *) area + 2);
    left = n - 1;
    if (n != 0) {
        do {
            w = area->mUnk_00;
            h = area->mUnk_01;
            x0 = area->mUnk_02;
            z0 = area->mUnk_04;
            if (cx >= x0 && cx < x0 + w && cz >= z0 && cz < z0 + h) {
                cx -= x0;
                cz -= z0;
                break;
            }
            area = (MapArea *) ((u8 *) area + w * h * 2 + 6);
        } while (left-- != 0);
    }
    if (left < 0) {
        return;
    }
    head = 0;
    tail = 0;
    shift = 32 - func_02028250(w);
    if (w * h >= 10000) {
        OS_Terminate();
    }
    reg_G3X_POLYGON_ATTR = 0x10c0;
    reg_G3X_MTX_MODE = 2;
    G3_LoadMtx43(&data_020c3e80);
    reg_G3X_MTX_STORE = 0x1e;
    box[0] = 0;
    box[1] = -0x1000;
    box[2] = 0;
    box[3] = 0x1000;
    box[4] = 0x2000;
    box[5] = 0x1000;
    MIi_CpuClearFast(0, data_020c3e44.mUnk_28, 0x400);
    MIi_CpuClearFast(0, data_020c3e44.mUnk_30, 0x9c40);
    data_020c3e44.mUnk_30[cx + cz * w].mUnk_3 = 1;
    idx0 = cx + (cz << shift);
    data_020c3e44.mUnk_28[idx0 >> 5] |= 1 << (idx0 & 0x1f);
    queue[tail++] = idx0;
    while (tail != head) {
        q = queue[head];
        head = (u8) (head + 1);
        row = q >> shift;
        col = q & ((1 << shift) - 1);
        v = area->mUnk_06[col + row * w];
        x = data_020c5e44.mUnk_000.mUnk_64 * (x0 + col);
        z = data_020c5e44.mUnk_000.mUnk_64 * (z0 + row);
        if ((v & 0x3ff) == 0) {
            data_020c5e44.mUnk_000.mUnk_7f_2 = 1;
            data_020c3e44.mUnk_dc = x;
            data_020c3e44.mUnk_e0 = z;
        } else {
            data_020c3e44.mUnk_2c[data_020c3e44.mUnk_36].mUnk_04 = x;
            data_020c3e44.mUnk_2c[data_020c3e44.mUnk_36].mUnk_08 = z;
            data_020c3e44.mUnk_2c[data_020c3e44.mUnk_36].mUnk_00 =
                func_0203249c(v & 0x3ff) + data_020c5e44.mUnk_000.mUnk_7c - 1;
            data_020c3e44.mUnk_36++;
            if (data_020c3e44.mUnk_36 >= 0x180) {
                OS_Terminate();
            }
        }
        m = v & 0xf000;
        j = 0;
        while (m != 0) {
            if (m & 0x8000) {
                u32 nc = col + sMapDirs[j + 1];
                u32 nr = row + sMapDirs[j];
                u32 idx = nc + (nr << shift);

                u32 bit = 1 << (idx & 0x1f);
                u32 word = data_020c3e44.mUnk_28[idx >> 5];

                if (!(bit & word)) {
                    data_020c3e44.mUnk_28[idx >> 5] = word | bit;
                    *(vu32 *) 0x04000450 = 0x1e;
                    PutTrans(data_020c5e44.mUnk_000.mUnk_64 * (nc + x0), 0, data_020c5e44.mUnk_000.mUnk_64 * (nr + z0));
                    PutScale(data_020c5e44.mUnk_000.mUnk_64, data_020c5e44.mUnk_000.mUnk_64 << 4,
                             data_020c5e44.mUnk_000.mUnk_64);
                    reg_G3X_BEGIN_VTXS = 0;
                    reg_G3X_END_VTXS = 0;
                    reg_G3X_BOX_TEST = *(u32 *) &box[0];
                    reg_G3X_BOX_TEST = *(u32 *) &box[2];
                    reg_G3X_BOX_TEST = *(u32 *) &box[4];
                    while (G3X_GetBoxTestResult(&result) != 0) {
                    }
                    if (result != 0 && func_02032bb8(nc, nr, w, cx, cz) != 0) {
                        data_020c3e44.mUnk_30[nc + nr * w].mUnk_3 = 1;
                        queue[tail] = idx;
                        tail++;
                    }
                }
            }
            m = (m << 1) & 0xf000;
            j++;
        }
    }
    if (!data_020c5e44.mUnk_000.mUnk_7f_2) {
        if (data_020c5e44.mUnk_000.mUnk_7e == 1) {
            data_020c5e44.mUnk_000.mUnk_7f_3 = 0;
            data_020c3e44.mUnk_36 = saved;
        } else {
            data_020c5e44.mUnk_000.mUnk_7f_4 = 0;
            data_020c5e44.mUnk_000.mUnk_30 = 0;
            MIi_CpuClearFast(0, data_020c3e44.mUnk_24, data_020c3e44.mUnk_34);
            for (i = saved; i < data_020c3e44.mUnk_36; i++) {
                data_020c3e44.mUnk_2c[i - saved] = data_020c3e44.mUnk_2c[i];
            }
            data_020c3e44.mUnk_36 -= saved;
        }
    }
}

// One record of chunk 0x5512 (the offsets of the records are in the table at Unk_020c6144.mUnk_50): the key, the
// size of the cell block in cells (mUnk_02 wide, mUnk_03 deep, mUnk_04 high), its position as three 32-bit values
// split into halfwords (x, z, y) and the number of cells it lists; the list of cell numbers follows the 0x14-byte
// header at halfword 0x14.
typedef struct MapCellRec {
    u16 mUnk_00;
    u8 mUnk_02;
    u8 mUnk_03;
    u8 mUnk_04;
    u8 mUnk_05;
    u16 mUnk_06[2];
    u16 mUnk_0a[2];
    u16 mUnk_0e[2];
    u8 mUnk_12;
    u8 mUnk_13;
} MapCellRec;

// Collects the cell blocks of chunk 0x5512 that the camera sees, starting from the cell under (x, z): each block
// within the view distance is box-tested, kept in the table at MapState.mUnk_2c (with its class) when visible and its
// listed cells that are not marked yet are marked in the first bit set and queued for the same treatment.
void func_0203315c(int x, int z) {
    u32 mask = 0x7fff;
    int n;
    u8 m;
    u16 list[256];
    u32 result;
    u16 box[6];
    int cx;
    int cz;
    u16 *cells;
    u16 *chunk;
    u16 v;
    u16 cols;
    u16 rows;
    u32 b1;
    u32 b2;

    reg_G3X_POLYGON_ATTR = 0x10c0;
    n = 0;
    m = 0;
    reg_G3X_MTX_MODE = 2;
    G3_LoadMtx43(&data_020c3e80);
    reg_G3X_MTX_STORE = 0x1e;
    box[0] = 0;
    box[1] = 0;
    box[2] = 0;
    box[3] = 1;
    box[4] = 1;
    box[5] = 1;
    cx = (u32) x / data_020c5e44.mUnk_000.mUnk_64;
    cz = (u32) z / data_020c5e44.mUnk_000.mUnk_64;
    cols = data_020c5e44.mUnk_000.mUnk_78;
    rows = data_020c5e44.mUnk_000.mUnk_7a;
    if (!(cz < rows && cx < cols)) {
        return;
    }
    cells = func_02032898();
    chunk = func_02031eb8(0x5512);
    if (chunk == 0) {
        OS_Terminate();
    }
    v = cells[cx + cz * data_020c5e44.mUnk_000.mUnk_78];
    if (v & 0x8000) {
        v &= 0x7fff;
        data_020c5e44.mUnk_000.mUnk_7f_3 = 1;
    }
    if (v != 0) {
        data_020c3e44.mUnk_24[v >> 5] |= 1 << (v & 0x1f);
        list[0] = v;
        n++;
        data_020c5e44.mUnk_000.mUnk_7f_4 = 1;
    }
    if (n == 0) {
        return;
    }
    b1 = *(u32 *) &box[2];
    b2 = *(u32 *) &box[4];
    while (n != m) {
        u16 idx = list[m] & 0x7fff;
        u16 *offs;
        MapCellRec *rec;
        u16 *p;
        int px, pz, py;
        u32 r;
        u32 i;
        u32 listlen;
        u32 g;
        int sx, sy, sz;

        m++;
        offs = data_020c5e44.mUnk_300.mUnk_50;
        rec = (MapCellRec *) (chunk + offs[idx]);
        listlen =
            (u32) ((offs[idx + 1] - offs[idx]) * 2 - ((u8 *) (rec + 1) - (u8 *) rec) - (rec->mUnk_12 - 1) * 2) >> 1;
        if (chunk[offs[idx]] == 0) {
            OS_Terminate();
        }
        if (rec->mUnk_02 == 0) {
            OS_Terminate();
        }
        if (rec->mUnk_03 == 0) {
            OS_Terminate();
        }
        if (rec->mUnk_04 == 0) {
            OS_Terminate();
        }
        px = rec->mUnk_06[0] + (rec->mUnk_06[1] << 16);
        pz = rec->mUnk_0a[0] + (rec->mUnk_0a[1] << 16);
        py = rec->mUnk_0e[0] + (rec->mUnk_0e[1] << 16);
        r = (u32) data_020c5e44.mUnk_300.mUnk_44 >> 9;
        if (func_020322f4(px, pz, rec->mUnk_02, rec->mUnk_03) > r * r) {
            continue;
        }
        *(vu32 *) 0x04000450 = 0x1e;
        PutTrans(px, py, pz);
        PutScale(rec->mUnk_02 * data_020c5e44.mUnk_000.mUnk_64, rec->mUnk_04 * data_020c5e44.mUnk_000.mUnk_64,
                 rec->mUnk_03 * data_020c5e44.mUnk_000.mUnk_64);
        reg_G3X_BEGIN_VTXS = 0;
        reg_G3X_END_VTXS = 0;
        reg_G3X_BOX_TEST = *(u32 *) &box[0];
        reg_G3X_BOX_TEST = b1;
        reg_G3X_BOX_TEST = b2;
        while (G3X_GetBoxTestResult(&result) != 0) {
        }
        if (result != 0 && rec->mUnk_12 > 1) {
            data_020c5e44.mUnk_000.mUnk_2c[data_020c5e44.mUnk_000.mUnk_30++] = &rec->mUnk_13;
        }
        if (result == 0 && data_020c3e44.mUnk_36 != 0) {
            continue;
        }
        if (result != 0) {
            u32 k = func_02032368(rec->mUnk_00, px, pz, rec->mUnk_02, rec->mUnk_03);

            data_020c3e44.mUnk_2c[data_020c3e44.mUnk_36].mUnk_04 = px;
            data_020c3e44.mUnk_2c[data_020c3e44.mUnk_36].mUnk_08 = pz;
            data_020c3e44.mUnk_2c[data_020c3e44.mUnk_36].mUnk_00 =
                data_020c5e44.mUnk_000.mUnk_7c - 1 + func_0203249c(k);
            data_020c3e44.mUnk_36++;
            if (data_020c3e44.mUnk_36 >= 0x180) {
                OS_Terminate();
            }
        }
        p = (u16 *) rec + (rec->mUnk_12 - 1);
        for (i = 0; i < listlen; i++, p++) {
            u16 c = p[10] & mask;
            u32 bit;

            if (c & 0x8000) {
                continue;
            }
            if (c == 0) {
                OS_Terminate();
            }
            if (c > 0x3ff) {
                OS_Terminate();
            }
            bit = 1 << (c & 0x1f);
            if (data_020c3e44.mUnk_24[c >> 5] & bit) {
                continue;
            }
            data_020c3e44.mUnk_24[c >> 5] |= bit;
            list[n] = c;
            n = (u8) (n + 1);
            if (n == m) {
                OS_Terminate();
            }
        }
        mask = 0xffff;
    }
}

// Moves the camera's target and position a step (1 / 2^mUnk_bc of the way) towards their goals and redraws the
// projection.
void func_02033488(void) {
    u8 s = data_020c5e44.mUnk_000.mUnk_bc;

    data_020c5e44.mUnk_000.mUnk_98.x += (data_020c5e44.mUnk_000.mUnk_b0.x - data_020c5e44.mUnk_000.mUnk_98.x) >> s;
    data_020c5e44.mUnk_000.mUnk_98.y += (data_020c5e44.mUnk_000.mUnk_b0.y - data_020c5e44.mUnk_000.mUnk_98.y) >> s;
    data_020c5e44.mUnk_000.mUnk_98.z += (data_020c5e44.mUnk_000.mUnk_b0.z - data_020c5e44.mUnk_000.mUnk_98.z) >> s;
    data_020c5e44.mUnk_000.mUnk_80.x += (data_020c5e44.mUnk_000.mUnk_a4.x - data_020c5e44.mUnk_000.mUnk_80.x) >> s;
    data_020c5e44.mUnk_000.mUnk_80.y += (data_020c5e44.mUnk_000.mUnk_a4.y - data_020c5e44.mUnk_000.mUnk_80.y) >> s;
    data_020c5e44.mUnk_000.mUnk_80.z += (data_020c5e44.mUnk_000.mUnk_a4.z - data_020c5e44.mUnk_000.mUnk_80.z) >> s;
    func_02033b8c();
    func_02033520();
}

// The camera's field of view angle.
u16 func_020334ec(void) {
    return data_020c5e44.mUnk_300.mUnk_14;
}

// Sets the projection with the far distance scaled by 8 when a is 1, then restores it.
void func_020334f8(int a) {
    s32 saved = data_020c5e44.mUnk_300.mUnk_44;
    int sh;

    if (a == 1) {
        sh = 3;
    } else {
        sh = 0;
    }
    data_020c5e44.mUnk_300.mUnk_44 <<= sh;
    func_02033520();
    data_020c5e44.mUnk_300.mUnk_44 = saved;
}

// Sets up the projection (from the field of view angle and the far distance) and the view matrix from the camera
// position and target, once a map is loaded, and keeps the view matrix's rotation and the negated translation
// derived from it.
void func_02033520(void) {
    int a = func_020334ec();

    if (data_020c5e44.mUnk_000.mUnk_74 != 0) {
        int i;

        func_0203cbe0();
        if (data_020c5e44.mUnk_000.mUnk_80.x == data_020c5e44.mUnk_000.mUnk_98.x &&
            data_020c5e44.mUnk_000.mUnk_80.y == data_020c5e44.mUnk_000.mUnk_98.y &&
            data_020c5e44.mUnk_000.mUnk_80.z == data_020c5e44.mUnk_000.mUnk_98.z) {
            data_020c5e44.mUnk_000.mUnk_80.y += 0xc800;
        }
        i = (a >> 4) * 2;
        func_0203d030(FX_SinCosTable_[i], FX_SinCosTable_[i + 1], 0x1555, 0x400, data_020c5e44.mUnk_300.mUnk_44, 0x1000,
                      1, &data_020c3ee0);
        *(vu32 *) 0x040004c8 = 0x20000000;
        func_0203d0f0(&data_020c5e44.mUnk_000.mUnk_80, (VecFx32 *) &data_020c5e44.mUnk_000.mUnk_8c,
                      &data_020c5e44.mUnk_000.mUnk_98, 1, &data_020c3e80);
        func_0203b8bc((MtxFx33 *) &data_020c3e80, (MtxFx33 *) &data_020c3eb0);
        func_0203b9b0((VecFx32 *) &data_020c3e80._[9], (MtxFx33 *) &data_020c3eb0, &data_020c3e44.mUnk_90);
        data_020c3e44.mUnk_90.x = -data_020c3e44.mUnk_90.x;
        data_020c3e44.mUnk_90.y = -data_020c3e44.mUnk_90.y;
        data_020c3e44.mUnk_90.z = -data_020c3e44.mUnk_90.z;
        reg_G3X_MTX_MODE = 2;
        data_020c5e44.mUnk_300.mUnk_4a = 0;
    }
}

// Rebuilds the visible cells and objects for a camera at (x, z), then draws the markers of the stage (filtered by
// mUnk_38 when its top bit is set) and the camera's own model.
#pragma thumb off
void func_02033614(s32 x, s32 z) {
    u32 i;
    s32 a;
    s32 b;
    DrawParams marker;
    DrawParams camera;

    data_020c5e44.mUnk_000.mUnk_2c = func_0201c430(0x80);
    data_020c5e44.mUnk_000.mUnk_30 = 0;
    if (data_020c5e44.mUnk_300.mUnk_42[0] != 0) {
        a = data_020c5e44.mUnk_300.mUnk_38;
        b = data_020c5e44.mUnk_300.mUnk_3c;
    } else {
        a = data_020c5e44.mUnk_300.mUnk_38 * 0xc800;
        b = data_020c5e44.mUnk_300.mUnk_3c * 0xc800;
    }
    func_02034768(a, b, data_020c5e44.mUnk_300.mUnk_40);
    func_02034904();
    data_020c5e44.mUnk_000.mUnk_68 = 0;
    data_020c5e44.mUnk_000.mUnk_7f_4 = 0;
    data_020c5e44.mUnk_000.mUnk_7f_3 = 0;
    data_020c5e44.mUnk_000.mUnk_7f_2 = 0;
    data_020c3e44.mUnk_36 = 0;
    MIi_CpuClearFast(0, data_020c3e44.mUnk_24, data_020c3e44.mUnk_34);
    func_0203315c(x, z);
    func_02032d50(x, z);
    if (data_020c5e44.mUnk_000.mUnk_7f_2 == 1 && data_020c5e44.mUnk_000.mUnk_7f_4 == 0) {
        func_0203315c(data_020c3e44.mUnk_dc, data_020c3e44.mUnk_e0);
    }
    data_020c5e44.mUnk_000.mUnk_7e = (data_020c5e44.mUnk_000.mUnk_7f_3 << 1) + data_020c5e44.mUnk_000.mUnk_7f_4;
    data_020c5e44.mUnk_300.mUnk_60 = 0;
    if (data_020c5e44.mUnk_300.mUnk_5c == 0) {
        for (i = 0; i < data_020c5e44.mUnk_000.mUnk_30; i++) {
            func_020329d0(data_020c5e44.mUnk_000.mUnk_2c[i]);
        }
    }
    func_0201c4f0(&data_020c5e44.mUnk_000.mUnk_2c);
    marker.mUnk_43_4 = 0;
    marker.mUnk_43_3 = marker.mUnk_43_4;
    marker.mUnk_43 = 0;
    marker.mPos.y = 0;
    marker.mUnk_38 = 0;
    marker.mUnk_3a = 0;
    marker.mUnk_3c = 0;
    marker.mUnk_3e = 0xff;
    {
        s32 v = -(data_020c5e44.mUnk_300.mUnk_5a == 0);
        marker.mUnk_2c = v;
        marker.mUnk_30 = v;
        marker.mUnk_34 = v;
    }
    for (i = 0; i < data_020c3e44.mUnk_36; i++) {
        if ((data_020c3e44.mUnk_38 & 0x8000) == 0 ||
            (data_020c3e44.mUnk_38 & ~0x8000) + data_020c5e44.mUnk_000.mUnk_7c == data_020c3e44.mUnk_2c[i].mUnk_00) {
            MapMarker *m = &data_020c3e44.mUnk_2c[i];
            marker.mPos.x = m->mUnk_04;
            marker.mPos.z = m->mUnk_08;
            func_020248e8(m->mUnk_00 & 0x7fff, &marker, data_020c5e44.mUnk_000.mUnk_7f_0 == 1 ? 0x10 : 0);
            if (*(vu16 *) 0x04000604 > data_020c5e44.mUnk_300.mUnk_54) {
                break;
            }
        }
    }
    if (data_020c5e44.mUnk_000.mUnk_76 == 0) {
        return;
    }
    func_020334ec();
    func_020334f8(1);
    camera.mPos.x = data_020c5e44.mUnk_000.mUnk_80.x;
    camera.mPos.y = data_020c5e44.mUnk_000.mUnk_80.y;
    camera.mUnk_43_3 = 0;
    camera.mUnk_43_4 = 0;
    camera.mUnk_43 = 0;
    camera.mPos.z = data_020c5e44.mUnk_000.mUnk_80.z;
    camera.mUnk_38 = 0;
    camera.mUnk_3c = 0;
    camera.mUnk_3a = 0;
    camera.mUnk_28 = 0x1000;
    func_020248e8(data_020c5e44.mUnk_000.mUnk_76, &camera, data_020c5e44.mUnk_000.mUnk_7f_1 == 0);
    func_020334f8(0);
}

// Moves the view angle a quarter of the way towards the direction from the camera to the look-at point (skipped
// once when mUnk_34[0] is set).
void func_02033b04(void) {
    s32 dx;
    s32 dz;
    s32 diff;
    VecFx32 *target;

    if (data_020c5e44.mUnk_300.mUnk_34[0] != 0) {
        data_020c5e44.mUnk_300.mUnk_34[0] = 0;
        return;
    }
    target = &data_020c5e44.mUnk_000.mUnk_98;
    dx = data_020c5e44.mUnk_000.mUnk_80.x - target->x;
    dz = data_020c5e44.mUnk_000.mUnk_80.z - target->z;
    diff = func_0201fe34(data_020c5e44.mUnk_300.mUnk_56, func_0201feb0(-dx >> 6, -dz >> 6));
    data_020c5e44.mUnk_300.mUnk_56 += (u16) (diff / 4);
}

// One step of the view angle blend from mUnk_16 to mUnk_18 over mUnk_11[0] steps (mUnk_10 counts them); c points at
// the blend counters from mUnk_0c on.
void func_02033b8c(void) {
    u8 *c = &data_020c5e44.mUnk_300.mUnk_0c;
    if (c[4] == 0) {
        return;
    }
    if (c[5] - 1 > 1) {
        data_020c5e44.mUnk_300.mUnk_14 =
            func_0201fb74(c[4], c[5] - 1, data_020c5e44.mUnk_300.mUnk_16, data_020c5e44.mUnk_300.mUnk_18);
        c[4]++;
        if (c[4] == c[5]) {
            c[4] = 0;
        }
    } else {
        data_020c5e44.mUnk_300.mUnk_14 = data_020c5e44.mUnk_300.mUnk_18;
        c[4] = 0;
    }
}

// Moves the look-at point towards pos: by a fraction of the distance to its goal when a divisor is set, otherwise
// in mUnk_0d steps from the point where pos last moved far (squared distance beyond mUnk_30 squared) away.
void func_0203393c(VecFx32 *pos) {
    VecFx32 *from;
    VecFx32 *cur;
    u32 d;
    int dx;
    int dy;
    int dz;

    if (data_020c5e44.mUnk_300.mUnk_4e == 0) {
        if (data_020c5e44.mUnk_300.mUnk_0d < 2) {
            data_020c5e44.mUnk_000.mUnk_98.x = pos->x;
            data_020c5e44.mUnk_000.mUnk_98.y = pos->y;
            data_020c5e44.mUnk_000.mUnk_98.z = pos->z;
            data_020c5e44.mUnk_300.mUnk_0c = 0;
            return;
        }
        cur = &data_020c5e44.mUnk_300.mUnk_00;
        dy = (cur->y - pos->y) >> 9;
        dx = (cur->x - pos->x) >> 9;
        dz = (cur->z - pos->z) >> 9;
        d = dx * dx + dy * dy + dz * dz;
        from = &data_020c5e44.mUnk_0c0.mUnk_234;
        if (d > (data_020c5e44.mUnk_300.mUnk_30 * data_020c5e44.mUnk_300.mUnk_30 >> 18)) {
            cur->x = pos->x;
            cur->y = pos->y;
            cur->z = pos->z;
            from->x = data_020c5e44.mUnk_000.mUnk_98.x;
            from->y = data_020c5e44.mUnk_000.mUnk_98.y;
            from->z = data_020c5e44.mUnk_000.mUnk_98.z;
            data_020c5e44.mUnk_300.mUnk_0c = 1;
        }
        if (data_020c5e44.mUnk_300.mUnk_0c == 0) {
            return;
        }
        data_020c5e44.mUnk_000.mUnk_98.x =
            func_0201fb74(data_020c5e44.mUnk_300.mUnk_0c, data_020c5e44.mUnk_300.mUnk_0d, from->x, cur->x);
        data_020c5e44.mUnk_000.mUnk_98.y =
            func_0201fb74(data_020c5e44.mUnk_300.mUnk_0c, data_020c5e44.mUnk_300.mUnk_0d, from->y, cur->y);
        data_020c5e44.mUnk_000.mUnk_98.z =
            func_0201fb74(data_020c5e44.mUnk_300.mUnk_0c, data_020c5e44.mUnk_300.mUnk_0d, from->z, cur->z);
        data_020c5e44.mUnk_300.mUnk_0c++;
        if (data_020c5e44.mUnk_300.mUnk_0c >= data_020c5e44.mUnk_300.mUnk_0d) {
            data_020c5e44.mUnk_300.mUnk_0c = 0;
        }
    } else {
        data_020c5e44.mUnk_000.mUnk_98.x +=
            (data_020c5e44.mUnk_0c0.mUnk_228.x - data_020c5e44.mUnk_000.mUnk_98.x) / data_020c5e44.mUnk_300.mUnk_4e;
        data_020c5e44.mUnk_000.mUnk_98.y +=
            (data_020c5e44.mUnk_0c0.mUnk_228.y - data_020c5e44.mUnk_000.mUnk_98.y) / data_020c5e44.mUnk_300.mUnk_4e;
        data_020c5e44.mUnk_000.mUnk_98.z +=
            (data_020c5e44.mUnk_0c0.mUnk_228.z - data_020c5e44.mUnk_000.mUnk_98.z) / data_020c5e44.mUnk_300.mUnk_4e;
    }
}

// The same for the camera position, with a goal and a start point of its own.
void func_02033c0c(VecFx32 *pos) {
    u8 *c;
    int t;

    if (data_020c5e44.mUnk_300.mUnk_4d == 0) {
        c = &data_020c5e44.mUnk_300.mUnk_0c;
        func_02033b8c();
        if (c[3] < 2) {
            data_020c5e44.mUnk_000.mUnk_80.x = pos->x;
            data_020c5e44.mUnk_000.mUnk_80.y = pos->y;
            data_020c5e44.mUnk_000.mUnk_80.z = pos->z;
            data_020c5e44.mUnk_300.mUnk_0f = 0;
            return;
        }
        if (c[2] != 0) {
            t = c[3] - 1;
            if (t > 1) {
                data_020c5e44.mUnk_000.mUnk_80.x = func_0201fb74(c[2], t, data_020c5e44.mUnk_0c0.mUnk_21c.x, pos->x);
                data_020c5e44.mUnk_000.mUnk_80.y = func_0201fb74(c[2], t, data_020c5e44.mUnk_0c0.mUnk_21c.y, pos->y);
                data_020c5e44.mUnk_000.mUnk_80.z = func_0201fb74(c[2], t, data_020c5e44.mUnk_0c0.mUnk_21c.z, pos->z);
                c[2]++;
                if (c[2] == c[3]) {
                    c[2] = 0;
                }
            } else {
                data_020c5e44.mUnk_000.mUnk_80.x = pos->x;
                data_020c5e44.mUnk_000.mUnk_80.y = pos->y;
                data_020c5e44.mUnk_000.mUnk_80.z = pos->z;
                c[2] = 0;
            }
        } else if (data_020c5e44.mUnk_000.mUnk_80.x != pos->x || data_020c5e44.mUnk_000.mUnk_80.y != pos->y ||
                   data_020c5e44.mUnk_000.mUnk_80.z != pos->z) {
            data_020c5e44.mUnk_0c0.mUnk_21c.x = data_020c5e44.mUnk_000.mUnk_80.x;
            data_020c5e44.mUnk_0c0.mUnk_21c.y = data_020c5e44.mUnk_000.mUnk_80.y;
            data_020c5e44.mUnk_0c0.mUnk_21c.z = data_020c5e44.mUnk_000.mUnk_80.z;
            data_020c5e44.mUnk_300.mUnk_0e = 1;
        } else {
            data_020c5e44.mUnk_0c0.mUnk_204.x = pos->x;
            data_020c5e44.mUnk_0c0.mUnk_204.y = pos->y;
            data_020c5e44.mUnk_0c0.mUnk_204.z = pos->z;
        }
    } else {
        data_020c5e44.mUnk_000.mUnk_80.x +=
            (data_020c5e44.mUnk_0c0.mUnk_204.x - data_020c5e44.mUnk_000.mUnk_80.x) / data_020c5e44.mUnk_300.mUnk_4d;
        data_020c5e44.mUnk_000.mUnk_80.y +=
            (data_020c5e44.mUnk_0c0.mUnk_204.y - data_020c5e44.mUnk_000.mUnk_80.y) / data_020c5e44.mUnk_300.mUnk_4d;
        data_020c5e44.mUnk_000.mUnk_80.z +=
            (data_020c5e44.mUnk_0c0.mUnk_204.z - data_020c5e44.mUnk_000.mUnk_80.z) / data_020c5e44.mUnk_300.mUnk_4d;
    }
}

// Starts the view angle blend towards a, given in degrees (stored as a 16-bit angle).
void func_02033df4(int a) {
    data_020c5e44.mUnk_300.mUnk_16 = data_020c5e44.mUnk_300.mUnk_14;
    data_020c5e44.mUnk_300.mUnk_18 = ((u32) a << 16) / 360;
    data_020c5e44.mUnk_300.mUnk_10 = 1;
}

// Sets the goal of the position blend (a 1 or 2) or the look-at blend (a 7 or 8) to entry b of chunk 0x5513 (-1:
// the current point) and clears its state; c is stored as the step count.
void func_02033e38(int a, int b, int c) {
    VecFx32 *dst = NULL;
    VecFx32 *src;
    MapEntry *e;

    switch (a) {
        case 1:
        case 2:
            dst = &data_020c5e44.mUnk_0c0.mUnk_204;
            src = &data_020c5e44.mUnk_000.mUnk_80;
            data_020c5e44.mUnk_300.mUnk_4d = 0;
            data_020c5e44.mUnk_300.mUnk_32 = a;
            data_020c5e44.mUnk_300.mUnk_0f = c;
            data_020c5e44.mUnk_300.mUnk_0e = 0;
            break;
        case 7:
        case 8:
            dst = &data_020c5e44.mUnk_0c0.mUnk_228;
            src = &data_020c5e44.mUnk_000.mUnk_98;
            data_020c5e44.mUnk_300.mUnk_4e = 0;
            data_020c5e44.mUnk_300.mUnk_33 = a;
            data_020c5e44.mUnk_300.mUnk_0d = c;
            data_020c5e44.mUnk_300.mUnk_0c = 0;
            break;
        default:
            OS_Terminate();
            break;
    }
    if (b != -1) {
        e = func_0203204c(b, 0);
        if (e == NULL) {
            OS_Terminate();
        }
        dst->x = func_02031f98(e);
        dst->y = func_02031fa4(e);
        dst->z = func_02031fb0(e);
    } else {
        dst->x = src->x;
        dst->y = src->y;
        dst->z = src->z;
    }
}

// Advances an animation track by the frames passed since its last update (1 when it has no timer).
void func_02033f38(CameraTrack *t, s32 *whole, s32 *next, s32 *rest) {
    int step;

    if (t->mUnk_16_5 == 0) {
        if (t->mUnk_16_1 != 1) {
            step = 1;
        } else {
            step = data_020c18bc.mUnk_45c - t->mUnk_0c;
            t->mUnk_0c = data_020c18bc.mUnk_45c;
        }
    } else {
        step = 0;
        t->mUnk_0c = data_020c18bc.mUnk_45c;
    }
    func_0202601c(t->mUnk_10, &t->mUnk_12, t->mUnk_14, step, whole, next, rest, t->mUnk_16_2);
}
#pragma thumb on
