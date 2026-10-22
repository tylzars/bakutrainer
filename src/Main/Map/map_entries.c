#include <nitro/os.h>
#include <nitro/gx.h>
#include <nitro/mi.h>
#include <Main/Map/map_entries.h>
#include <Main/System/random_angle.h>
#include <Main/System/engine_asm.h>
#include <Main/System/game_math.h>
#include <Main/System/system_init.h>
#include <Main/Unknown/game_util.h>
#include <Main/Graphics/model.h>
#include <Main/Graphics/vram_cache.h>
#include <Main/System/align_util.h>
#include <Main/System/file_loader.h>
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

// func_020312b8: the count bits and the top-height flag of a ring's format byte; the most rings and points it unpacks.
#define MAP_RING_POINTS 0x1f
#define MAP_RING_TOP 0x20
#define MAP_RING_MAX 0x80
#define MAP_RING_MAX_POINTS 0x100

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

// A leaf of the quadtree of chunk 0x5540 (an inner node holds the offsets of its four children instead, and its
// first halfword is never 0): mUnk_02 set means the cell has no rings, mUnk_03 is the number of rings, mUnk_04 the
// shift applied to the point coordinates and mUnk_06 the base height. The rings follow the leaf.
typedef struct MapRingLeaf {
    u16 mUnk_00;
    u8 mUnk_02;
    u8 mUnk_03;
    u8 mUnk_04;
    u8 mUnk_05;
    s16 mUnk_06;
} MapRingLeaf;

// One area of chunk 0x5530: its size in cells, the position of its first cell and w * h halfwords, one per cell
// (the low ten bits name an object, the top four bits tell which neighbours are connected).
typedef struct MapArea {
    u8 mUnk_00;
    u8 mUnk_01;
    u16 mUnk_02;
    u16 mUnk_04;
    u16 mUnk_06[1];
} MapArea;

static int func_02031cc8(int x, int z);
static void func_02031fec(u16 group, u16 key, VecFx32 *out);
static MapEntry *func_02032074(u8 group, u8 key, u8 *count);
static MapEntry *GetMapEntries(void);

#pragma thumb off

// Finds the quadtree leaf of chunk 0x5540 under pos and unpacks its rings: for each ring its address goes to rings,
// its key to keys and its point count to counts (followed by a 0), and its points to out, scaled down by shift. A
// ring is a halfword key, a byte holding the point count (MAP_RING_POINTS) and the point format, a byte, the offset
// of the next ring of the same group (see func_02031c70) and the points: x and z offsets from the leaf's corner
// (bytes, or halfwords at the next even address with bit 7 set), with bit 6 a bottom height above the leaf's base
// height and with MAP_RING_TOP a top height above the bottom.
static void func_020312b8(VecFx32 *pos, MapRingPoint *out, u8 *counts, u16 *keys, u16 **rings, int shift) {
    int total;
    int n;
    int npts;
    int h;
    u16 *base;
    int w;
    int i;
    MapRingLeaf *leaf;
    u16 *node;
    int z0;
    int x0;
    u8 *p;

    base = GetMapChunk(MAP_CHUNK_RINGS);
    x0 = 0;
    w = gMapState.mCamera.mCellCountX * gMapState.mCamera.mCellSize;
    h = gMapState.mCamera.mCellCountZ * gMapState.mCamera.mCellSize;
    node = base;
    z0 = 0;
    total = 0;
    n = 0;

    if (base == NULL) {
        OS_Terminate();
    }
    while (node[0] != 0) {
        int idx = 0;
        w >>= 1;
        h >>= 1;
        if (pos->z > z0 + h) {
            z0 += h;
            idx = 2;
        }
        if (pos->x > x0 + w) {
            x0 += w;
            idx++;
        }
        node = base + node[idx];
    }
    leaf = (MapRingLeaf *) node;
    if (leaf->mUnk_02 != 0) {
        return;
    }
    p = (u8 *) (leaf + 1);
    i = leaf->mUnk_03;
    while (i-- != 0) {
        npts = p[2] & MAP_RING_POINTS;
        if (++n >= MAP_RING_MAX) {
            OS_Terminate();
        }
        *rings++ = (u16 *) p;
        *keys++ = *(u16 *) p;
        counts[0] = npts;
        *++counts = 0;
        total += npts;
        if (total > MAP_RING_MAX_POINTS) {
            OS_Terminate();
        }
        switch ((p[2] >> 6) + ((p[2] & MAP_RING_TOP) >> 3)) {
            case 0:
                p += 6;
                while (npts-- != 0) {
                    out->x = (x0 + (((s8 *) p)[0] << leaf->mUnk_04)) >> shift;
                    out->z = (z0 + (((s8 *) p)[1] << leaf->mUnk_04)) >> shift;
                    out->mUnk_08 = leaf->mUnk_06 >> shift;
                    out->mUnk_0a = 0;
                    p += 2;
                    out++;
                }
                break;
            case 1:
                p += 6;
                while (npts-- != 0) {
                    out->x = (x0 + (((s8 *) p)[0] << leaf->mUnk_04)) >> shift;
                    out->z = (z0 + (((s8 *) p)[1] << leaf->mUnk_04)) >> shift;
                    out->mUnk_08 = ((leaf->mUnk_06 + p[2]) << leaf->mUnk_04) >> shift;
                    out->mUnk_0a = 0;
                    p += 3;
                    out++;
                }
                break;
            case 2:
                p = AlignPtrUp2(p + 6);
                while (npts-- != 0) {
                    out->x = (x0 + (((s16 *) p)[0] << leaf->mUnk_04)) >> shift;
                    out->z = (z0 + (((s16 *) p)[1] << leaf->mUnk_04)) >> shift;
                    out->mUnk_08 = leaf->mUnk_06 >> shift;
                    out->mUnk_0a = 0;
                    p += 4;
                    out++;
                }
                break;
            case 3:
                p = AlignPtrUp2(p + 6);
                while (npts-- != 0) {
                    out->x = (x0 + (((s16 *) p)[0] << leaf->mUnk_04)) >> shift;
                    out->z = (z0 + (((s16 *) p)[1] << leaf->mUnk_04)) >> shift;
                    out->mUnk_08 = ((leaf->mUnk_06 + ((u16 *) p)[2]) << leaf->mUnk_04) >> shift;
                    out->mUnk_0a = 0;
                    p += 6;
                    out++;
                }
                break;
            case 4:
                p += 6;
                while (npts-- != 0) {
                    out->x = (x0 + (((s8 *) p)[0] << leaf->mUnk_04)) >> shift;
                    out->z = (z0 + (((s8 *) p)[1] << leaf->mUnk_04)) >> shift;
                    out->mUnk_08 = leaf->mUnk_06 >> shift;
                    out->mUnk_0a = out->mUnk_08 + ((((s8 *) p)[2] << leaf->mUnk_04) >> shift);
                    p += 3;
                    out++;
                }
                break;
            case 5:
                p += 6;
                while (npts-- != 0) {
                    out->x = (x0 + (((s8 *) p)[0] << leaf->mUnk_04)) >> shift;
                    out->z = (z0 + (((s8 *) p)[1] << leaf->mUnk_04)) >> shift;
                    out->mUnk_08 = ((leaf->mUnk_06 + p[2]) << leaf->mUnk_04) >> shift;
                    out->mUnk_0a = out->mUnk_08 + ((p[3] << leaf->mUnk_04) >> shift);
                    p += 4;
                    out++;
                }
                break;
            case 6:
                p = AlignPtrUp2(p + 6);
                while (npts-- != 0) {
                    out->x = (x0 + (((s16 *) p)[0] << leaf->mUnk_04)) >> shift;
                    out->z = (z0 + (((s16 *) p)[1] << leaf->mUnk_04)) >> shift;
                    out->mUnk_08 = leaf->mUnk_06 >> shift;
                    out->mUnk_0a = out->mUnk_08 + ((((s16 *) p)[2] << leaf->mUnk_04) >> shift);
                    p += 6;
                    out++;
                }
                break;
            case 7:
                p = AlignPtrUp2(p + 6);
                while (npts-- != 0) {
                    out->x = (x0 + (((s16 *) p)[0] << leaf->mUnk_04)) >> shift;
                    out->z = (z0 + (((s16 *) p)[1] << leaf->mUnk_04)) >> shift;
                    out->mUnk_08 = ((leaf->mUnk_06 + ((u16 *) p)[2]) << leaf->mUnk_04) >> shift;
                    out->mUnk_0a = out->mUnk_08 + ((((u16 *) p)[3] << leaf->mUnk_04) >> shift);
                    p += 8;
                    out++;
                }
                break;
        }
    }
}

// Collects the ring segments of the quadtree leaf under pos whose key has a bit of any and none of none, and keeps
// those whose nearest point to pos lies within half of radius (squared distances, in units of 512) and, when the
// segment has a top height, whose height range overlaps pos.y + bottom .. pos.y + top. The camera records the
// nearest one: its ring (mUnk_10), its two ends (mUnk_14, mUnk_20), the nearest point (mUnk_2c), the squared
// distance (mUnk_38) and the ring's key (mUnk_3a). Returns 0 when nothing was found, else the MAP_RING_* result.
// atEnd and the block-local end are u8 and atEnd is set after best because the compiler's output needs it: as int,
// end adds a mask when it is or-ed into atEnd, and an int atEnd or an earlier init reorders the stack stores.
u32 func_02031824(VecFx32 *pos, int radius, int top, int bottom, u16 any, u16 none) {
    MapRingPoint pts[MAP_RING_MAX_POINTS];
    u8 counts[MAP_RING_MAX];
    u16 keys[MAP_RING_MAX];
    u16 *rings[MAP_RING_MAX];
    MapRingPoint nearest;
    int limit;
    s32 pz;
    s32 px;
    int i;
    int base;
    u8 hits;
    u8 tie;
    u32 result;
    u8 atEnd;
    u32 best;
    s16 y;
    int k;

    hits = 0;
    limit = radius * radius / 4;
    tie = 0;
    result = 0;
    best = -1;
    atEnd = 0;
    counts[0] = 0;
    func_020312b8(pos, pts, counts, keys, rings, 9);
    i = 0;
    px = pos->x;
    pz = pos->z;
    y = pos->y >> 9;
    base = 0;
    while (counts[i] != 0) {
        if ((keys[i] & any) != 0 && (keys[i] & none) == 0) {
            for (k = 0; k < counts[i] - 1; k++) {
                int t;
                int sh;
                s32 dx;
                s32 dz;
                MapRingPoint *next;
                MapRingPoint *cur;
                int len2;
                u8 end;
                s32 ax;
                s32 az;
                s16 dh;
                s16 dh2;
                u32 dist;
                int frac;
                int j;

                j = base + k;
                end = 0;
                cur = &pts[j];
                next = &pts[j + 1];
                dx = next->x - cur->x;
                ax = (px >> 9) - cur->x;
                dh = next->mUnk_08 - cur->mUnk_08;
                az = (pz >> 9) - cur->z;
                dz = next->z - cur->z;
                dh2 = next->mUnk_0a - cur->mUnk_0a;
                t = ax * dx + az * dz;

                if (t <= 0) {
                    nearest.x = cur->x;
                    nearest.z = cur->z;
                    nearest.mUnk_08 = cur->mUnk_08;
                    nearest.mUnk_0a = cur->mUnk_0a;
                    end = 1;
                } else {
                    len2 = dx * dx + dz * dz;
                    if (len2 <= t) {
                        nearest.x = next->x;
                        nearest.mUnk_08 = next->mUnk_08;
                        nearest.z = next->z;
                        nearest.mUnk_0a = next->mUnk_0a;
                        end = 1;
                    } else {
                        sh = LeadingZeroCount(t) - 1;
                        if (sh < 9) {
                            frac = (t << sh) / (len2 >> (9 - sh));
                            sh = 9;
                        } else {
                            frac = (t << sh) / len2;
                        }
                        nearest.x = cur->x + ((frac * dx) >> sh);
                        nearest.mUnk_08 = cur->mUnk_08 + ((frac * dh) >> sh);
                        nearest.z = cur->z + ((frac * dz) >> sh);
                        nearest.mUnk_0a = cur->mUnk_0a + ((frac * dh2) >> sh);
                    }
                }
                dist = LengthSquared2d(nearest.x - (px >> 9), nearest.z - (pz >> 9));
                if (dist < limit) {
                    if (nearest.mUnk_0a == 0 || (bottom + y < nearest.mUnk_0a && top + y > nearest.mUnk_08)) {
                        atEnd |= end;
                        hits++;
                        if (dist < best) {
                            best = dist;
                            gMapState.mCamera.mUnk_10 = rings[i];
                            result = (i << 8) + k + MAP_RING_HIT;
                            gMapState.mCamera.mUnk_38 = dist;
                            gMapState.mCamera.mUnk_3a = keys[i];
                            gMapState.mCamera.mUnk_14 = *cur;
                            gMapState.mCamera.mUnk_20 = *next;
                            gMapState.mCamera.mUnk_2c = nearest;
                        } else if (dist == best) {
                            tie = 1;
                        }
                    }
                }
            }
        }
        base += counts[i++];
    }
    if (atEnd != 0) {
        result |= MAP_RING_AT_END;
    }
    if (hits > 1) {
        result |= MAP_RING_SEVERAL;
    }
    if (tie != 0) {
        result |= MAP_RING_TIE;
    }
    return result;
}

#pragma thumb on

// Returns the lowest address among the nodes of the ring of chunk 0x5540 that p belongs to (each node's halfword at
// 4 is the offset of the next node).
void *func_02031c70(void *p) {
    u16 *cur = p;
    u16 *base = GetMapChunk(MAP_CHUNK_RINGS);
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
    u16 *base = GetMapChunk(MAP_CHUNK_RINGS);
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
    int dx = (gMapState.mCamera.mPos.x - x) / 0x1000;
    int dz = (gMapState.mCamera.mPos.z - z) / 0x1000;
    u32 r = (u32) gMapState.mCamera.mUnk_2dc.mUnk_44 >> 12;

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
    if (gMapState.mCamera.mUnk_2dc.mUnk_4a != 0) {
        func_020334f8(0);
    }
    reg_G3X_MTX_MODE = 2;
    G3_LoadMtx43(&gMapState.mUnk_3c);
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
    u32 grid = gMapState.mCamera.mCellSize;
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
    cells = GetMapCellChunk();
    v = cells[cx + cz * gMapState.mCamera.mCellCountX] & 0x7fff;
    if (v != 0) {
        if (gMapState.mUnk_24[v >> 5] & (1 << (v & 0x1f))) {
            return 1;
        }
    }
    chunk = (u8 *) GetMapChunk(MAP_CHUNK_AREAS);
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
                sh = 32 - LeadingZeroCount(w);
                idx = cx + (cz << sh);
                if (gMapState.mUnk_28[idx >> 5] & (1 << (idx & 0x1f))) {
                    return 1;
                }
                return 0;
            }
            p += w * h * 2 + 6;
        } while (n-- != 0);
    }
    return 0;
}

// Returns the data of the chunk with key id, or 0 when the file has none. Loads the file when mMapData is 0 or does
// not start with MAP_FILE_MAGIC (then also refreshing the cell record offsets in mUnk_2dc.mUnk_50), and clears the
// 20 records when the data pointer differs from the one seen last (mUnk_4c).
#pragma thumb off
u16 *GetMapChunk(u32 id) {
    u16 *p = gMapState.mCamera.mMapData;
    int key;

    if (p == 0 || p[0] != MAP_FILE_MAGIC) {
        p = FindOrLoadCachedFile(gMapState.mCamera.mMapFile);
        gMapState.mCamera.mMapData = p;
        gMapState.mCamera.mUnk_2dc.mUnk_50 = GetMapChunk(MAP_CHUNK_CELL_REC_OFFSETS);
    }
    if (gMapState.mCamera.mUnk_4c != gMapState.mCamera.mMapData) {
        gMapState.mCamera.mUnk_4c = gMapState.mCamera.mMapData;
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

s32 GetMapEntryX(MapEntry *entry) {
    return entry->mUnk_04[0] + (entry->mUnk_04[1] << 16);
}

s32 GetMapEntryY(MapEntry *entry) {
    return entry->mUnk_08[0] + (entry->mUnk_08[1] << 16);
}

s32 GetMapEntryZ(MapEntry *entry) {
    return entry->mUnk_0c[0] + (entry->mUnk_0c[1] << 16);
}

u16 GetMapEntryAngle(MapEntry *entry) {
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

    out->x = GetMapEntryX(entry);
    out->y = GetMapEntryY(entry);
    out->z = GetMapEntryZ(entry);
}

// Position (and flags, when asked for) of the index'th entry of chunk 0x5513.
void func_02032018(u32 index, VecFx32 *pos, u8 *flags) {
    MapEntry *entry = GetMapEntries() + index;

    pos->x = GetMapEntryX(entry);
    pos->y = GetMapEntryY(entry);
    pos->z = GetMapEntryZ(entry);
    if (flags != 0) {
        *flags = entry->mUnk_02;
    }
}

// The entry of chunk 0x5513 with id (group in the high byte, key in the low byte); *count receives the number of
// entries when given.
MapEntry *GetMapEntry(u32 a, u8 *count) {
    if (a & 0xffff0000) {
        OS_Terminate();
    }
    return func_02032074((u8) (a >> 8), (u8) a, count);
}

static MapEntry *func_02032074(u8 group, u8 key, u8 *count) {
    MapEntryChunk *chunk = (MapEntryChunk *) GetMapChunk(MAP_CHUNK_ENTRIES);
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

    entries = GetMapEntries();
    count = func_02032244(a, b, keys, 0x30);
    px = pos->x >> 12;
    pz = pos->z >> 12;
    for (i = 0; i < count; i++) {
        verts = func_020321ac(keys[i] >> 8, keys[i] & 0xff, &nverts);
        if (nverts < 3) {
            continue;
        }
        p1.x = GetMapEntryX(entries + verts[0]) >> 12;
        p1.z = GetMapEntryZ(entries + verts[0]) >> 12;
        verts++;
        for (j = 1; j < nverts; verts++, j++) {
            p0 = p1;
            p1.x = GetMapEntryX(entries + verts[0]) >> 12;
            p1.z = GetMapEntryZ(entries + verts[0]) >> 12;
            if (Cross2d(p0.x, p0.z, p1.x, p1.z, px, pz) > 0) {
                break;
            }
        }
        if (j == nverts) {
            return 1;
        }
    }
    return 0;
}

static MapEntry *GetMapEntries(void) {
    MapEntryChunk *chunk = (MapEntryChunk *) GetMapChunk(MAP_CHUNK_ENTRIES);

    return chunk->mUnk_0a;
}

// The vertex index list of the polygon of chunk 0x5516 with group a and key b (-1 matches any); *count receives its
// vertex count and the matched group and key are kept in the camera.
u16 *func_020321ac(int a, int b, u32 *count) {
    MapPoly *p = (MapPoly *) GetMapChunk(MAP_CHUNK_POLYS);
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
                gMapState.mCamera.mUnk_3c = p->mUnk_02;
                gMapState.mCamera.mUnk_3d = p->mUnk_03;
                if (count != NULL) {
                    *count = p[1].mUnk_00 - p->mUnk_00;
                }
                return GetMapChunk(MAP_CHUNK_POLY_VERTS) + p->mUnk_00;
            }
        }
    }
    return NULL;
}

// Stores the group and key of up to max polygons matching (a, b) (-1 matches any) in out; returns how many.
u32 func_02032244(int a, int b, u16 *out, u32 max) {
    MapPoly *p = (MapPoly *) GetMapChunk(MAP_CHUNK_POLYS);
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

// Replaces the pair table with an empty one of count entries (when count is not 0) and marks it sorted, so
// func_0203249c skips the sort. Nothing in this build adds pairs to it.
void AllocMapPairs(u16 count) {
    if (count != 0) {
        if (gMapState.mPairs != NULL) {
            Free(&gMapState.mPairs);
        }
        gMapState.mPairs = Alloc(count * 4);
        gMapState.mPairCount = 0;
        gMapState.mUnk_18 = count;
        gMapState.mUnk_20 = 1;
        gMapState.mUnk_21 = 0;
    }
}

// The distance in cells from the camera's map position to the rectangle (x, z) of w by h cells, 0 when it lies inside.
u32 func_020322f4(int x, int z, int w, int h) {
    u32 k = gMapState.mCamera.mCellSize;
    int cw = k * w;
    int ch = k * h;
    int cx = gMapState.mCamera.mPos.x;

    if (cx < x) {
        int cz = gMapState.mCamera.mPos.z;
        if (cz >= z) {
            z += ch;
            if (cz < z) {
                z = cz;
            }
        }
    } else if (cx < x + cw) {
        int cz = gMapState.mCamera.mPos.z;
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
        int cz = gMapState.mCamera.mPos.z;
        x += cw;
        if (cz >= z) {
            z += ch;
            if (cz < z) {
                z = cz;
            }
        }
    }
    return LengthSquared2d((x - cx) / 512, (z - gMapState.mCamera.mPos.z) / 512);
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
        MapState *st = &gMapState;

        if (gMapState.mUnk_00 != gMapState.mCamera.mUnk_2dc.mUnk_44) {
            base = (gMapState.mUnk_10 * 5 * 256) / 256 - 256;
            k = 0;
            t = 0;
            for (i = 0; i < 3; i++) {
                int v;
                int r;
                t += 256;
                v = InterpolateQuadratic(0, 0x400, base, t);
                r = ((u32) gMapState.mCamera.mUnk_2dc.mUnk_44 >> 6) * 41 >> 15;
                v = v * r / 1024;
                st->mUnk_04[k++] = v * v;
            }
            gMapState.mUnk_00 = gMapState.mCamera.mUnk_2dc.mUnk_44;
        }
        d = func_020322f4(x, z, w, h);
        if (d == 0) {
            return flags & MAP_LOD_KEEP;
        }
        code = 0;
        switch (flags >> MAP_LOD_SHIFT) {
            case 1:
                if (d > MAP_LOD_UNIT * gMapState.mUnk_04[0]) {
                    code = MAP_LOD_1;
                }
                break;
            case 2:
                if (d > MAP_LOD_UNIT * gMapState.mUnk_04[1]) {
                    code = MAP_LOD_2;
                } else if (d > MAP_LOD_UNIT * gMapState.mUnk_04[0]) {
                    code = MAP_LOD_1;
                }
                break;
            case 3:
                if (d > MAP_LOD_UNIT * gMapState.mUnk_04[2]) {
                    code = MAP_LOD_3;
                } else if (d > MAP_LOD_UNIT * gMapState.mUnk_04[1]) {
                    code = MAP_LOD_2;
                } else if (d > MAP_LOD_UNIT * gMapState.mUnk_04[0]) {
                    code = MAP_LOD_1;
                }
                break;
        }
        return (flags & MAP_LOD_KEEP) + code;
    }
    return flags;
}

// Maps key through the sorted pair table (sorting it on first use); key itself when it has no entry. In this build
// mPairCount is never raised, so the search does not run and key is returned.
u32 func_0203249c(u32 key) {
    if (gMapState.mUnk_21 == 0 && gMapState.mPairCount != 0) {
        if (gMapState.mUnk_20 == 0) {
            MapPair *e;
            int gap;

            e = gMapState.mPairs;
            gap = 1 << (31 - LeadingZeroCount(gMapState.mPairCount));
            do {
                int i;
                for (i = 0; i < gMapState.mPairCount - gap; i++) {
                    int j;
                    for (j = i; j >= 0 && e[j].mUnk_0 > e[j + gap].mUnk_0; j -= gap) {
                        MapPair tmp = e[j];
                        e[j] = e[j + gap];
                        e[j + gap] = tmp;
                    }
                }
                gap >>= 1;
            } while (gap > 0);
            while (gMapState.mPairCount != 0 && e[gMapState.mPairCount - 1].mUnk_0 == 0xffff) {
                gMapState.mPairCount--;
            }
            gMapState.mUnk_16 = 1 << (31 - LeadingZeroCount(gMapState.mPairCount));
            gMapState.mUnk_20 = 1;
        }
        {
            int n;
            MapPair *e;
            int step;
            int pos;

            step = gMapState.mUnk_16;
            n = gMapState.mPairCount;
            e = gMapState.mPairs;
            pos = 0;
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

// Starts the map: initialises with a, b, c and the pair table size d and stores the camera's two halfwords
// mUnk_00/mUnk_02.
void SetupMap(u16 a, u16 b, u16 c, u16 d, u16 e, u16 f) {
    func_020325d0(a, b, c, d);
    gMapState.mCamera.mUnk_00 = e;
    gMapState.mCamera.mUnk_02 = f;
}

// Resets the map object and the camera for a new stage, sets up the 3D display and loads the buffers and the two
// camera positions from the stage data.
void func_020325d0(u16 a, u16 b, u16 c, u16 count) {
    u16 *hdr;
    u16 *offsets;
    MapEntry *entry;

    ReleaseMap();
    gMapState.mCamera.mMapData = NULL;
    gMapState.mCamera.mUnk_4c = NULL;
    gMapState.mCamera.mMapFile = a;
    gMapState.mCamera.mUnk_52 = b;
    gMapState.mCamera.mUnk_58 = c;
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
    hdr = GetMapChunk(MAP_CHUNK_ENTRIES);
    if (hdr == NULL) {
        OS_Terminate();
    }
    gMapState.mCamera.mCellCountX = hdr[0];
    gMapState.mCamera.mCellCountZ = hdr[1];
    gMapState.mCamera.mCellSize = (hdr[3] << 16) + hdr[2];
    offsets = GetMapChunk(MAP_CHUNK_CELL_REC_OFFSETS);
    gMapState.mCamera.mUnk_2dc.mUnk_50 = offsets;
    gMapState.mUnk_34 = (((offsets[-1] * 8) / 8 + 16) & ~3);
    gMapState.mUnk_24 = Alloc(gMapState.mUnk_34);
    gMapState.mUnk_28 = Alloc(0x400);
    gMapState.mMarkers = Alloc(0x1200);
    gMapState.mCells = Alloc(0x9c40);
    gMapState.mCamera.mUnk_2dc.mUnk_56 = 0;
    entry = GetMapEntry(0, NULL);
    if (entry != NULL) {
        gMapState.mCamera.mUnk_9c.mUnk_204.x = GetMapEntryX(entry);
        gMapState.mCamera.mUnk_9c.mUnk_204.y = GetMapEntryY(entry);
        gMapState.mCamera.mUnk_9c.mUnk_204.z = GetMapEntryZ(entry);
    } else {
        gMapState.mCamera.mUnk_9c.mUnk_204.x = 0x32000;
        gMapState.mCamera.mUnk_9c.mUnk_204.y = 0x32000 >> 2;
        gMapState.mCamera.mUnk_9c.mUnk_204.z = 0x32000;
    }
    gMapState.mCamera.mUnk_2dc.mUnk_32 = 1;
    gMapState.mCamera.mUnk_9c.mUnk_21c = gMapState.mCamera.mUnk_9c.mUnk_204;
    entry = GetMapEntry(1, NULL);
    if (entry != NULL) {
        gMapState.mCamera.mUnk_9c.mUnk_228.x = GetMapEntryX(entry);
        gMapState.mCamera.mUnk_9c.mUnk_228.y = GetMapEntryY(entry);
        gMapState.mCamera.mUnk_9c.mUnk_228.z = GetMapEntryZ(entry);
    } else {
        gMapState.mCamera.mUnk_9c.mUnk_228.x = 0x4b000;
        gMapState.mCamera.mUnk_9c.mUnk_228.y = 0x19000;
        gMapState.mCamera.mUnk_9c.mUnk_228.z = 0x4b000;
    }
    gMapState.mCamera.mUnk_2dc.mUnk_33 = 8;
    *(VecFx32 *) &gMapState.mCamera.mUnk_2dc = gMapState.mCamera.mUnk_9c.mUnk_228;
    gMapState.mUnk_38 = 0;
    gMapState.mCamera.mUnk_2dc.mUnk_54 = 0x190;
    gMapState.mCamera.mUnk_2dc.mUnk_30 = 0x3c00;
    gMapState.mCamera.mUnk_2dc.mUnk_0f = 1;
    gMapState.mCamera.mUnk_2dc.mUnk_11[0] = 4;
    gMapState.mCamera.mUnk_2dc.mUnk_0e = 0;
    gMapState.mCamera.mUnk_2dc.mUnk_10 = 0;
    gMapState.mCamera.mUnk_2dc.mUnk_11[1] = 0;
    gMapState.mCamera.mUnk_2dc.mUnk_14 = 0xe38;
    gMapState.mCamera.mUnk_2dc.mUnk_16 = gMapState.mCamera.mUnk_2dc.mUnk_14;
    gMapState.mCamera.mUnk_2dc.mUnk_18 = gMapState.mCamera.mUnk_2dc.mUnk_14;
    gMapState.mCamera.mUnk_68 = 1;
    gMapState.mCamera.mUnk_6c = 0x1000;
    gMapState.mCamera.mUnk_70 = 0;
    gMapState.mCamera.mPos.x = gMapState.mCamera.mUnk_9c.mUnk_204.x;
    gMapState.mCamera.mPos.y = gMapState.mCamera.mUnk_9c.mUnk_204.y;
    gMapState.mCamera.mPos.z = gMapState.mCamera.mUnk_9c.mUnk_204.z;
    gMapState.mCamera.mLookAt.x = gMapState.mCamera.mUnk_9c.mUnk_228.x;
    gMapState.mCamera.mLookAt.y = gMapState.mCamera.mUnk_9c.mUnk_228.y;
    gMapState.mCamera.mLookAt.z = gMapState.mCamera.mUnk_9c.mUnk_228.z;
    gMapState.mCamera.mUnk_2dc.mUnk_40 = 0;
    gMapState.mCamera.mUnk_2dc.mUnk_42[0] = 0;
    gMapState.mCamera.mUnk_2dc.mUnk_44 = 0x1f4000;
    gMapState.mCamera.mUnk_2dc.mUnk_64 = 0;
    gMapState.mUnk_00 = -1;
    gMapState.mUnk_10 = 0x80;
    gMapState.mCamera.mUnk_04 = NULL;
    AllocMapPairs(count);
    gMapState.mCamera.mUnk_2dc.mUnk_5a = 0;
    gMapState.mCamera.mUnk_2dc.mUnk_5c = 0;
    gMapState.mCamera.mUnk_5b_1 = 0;
    gSystemState.mFlags &= ~0x10000;
}

// Frees the map buffers and clears the records.
void ReleaseMap(void) {
    gMapState.mPairCount = 0;
    func_02032b88(1);
    Free(&gMapState.mPairs);
    Free(&gMapState.mUnk_24);
    Free(&gMapState.mUnk_28);
    Free(&gMapState.mMarkers);
    Free(&gMapState.mCells);
}

// The cell table of chunk 0x5510; terminates when the chunk is missing.
u16 *GetMapCellChunk(void) {
    u16 *cells = GetMapChunk(MAP_CHUNK_CELLS);

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
            if (gMapState.mCamera.mUnk_04 == NULL) {
                OS_Terminate();
            }
            gMapState.mCamera.mUnk_04(a, rec, (u16) (b + gMapState.mCamera.mUnk_02));
        }
        if (a == 0x3f || (gMapState.mCamera.mUnk_44 & (1 << a))) {
            anim = b;
            kind = (rec->mUnk_00->mUnk_04[i] >> 12) & 7;
        }
    }
    if (anim != 0x3f) {
        anim += gMapState.mCamera.mUnk_02;
    } else {
        anim = 0;
    }
    if (rec->mUnk_150 != anim) {
        if (rec->mUnk_176_10 == 1 || rec->mUnk_150 == 0) {
            StartModel(rec->mUnk_04, rec->mUnk_174, anim, 2, 0);
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
            rec = &gMapState.mRecords[i];
            if (rec->mUnk_00 == def) {
                break;
            }
        }
        if (i == MAP_RECORDS) {
            u8 *entries = (u8 *) GetMapChunk(MAP_CHUNK_ENTRIES) + 0xa;
            MapRecord *r;
            int off;

            r = gMapState.mRecords;
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
            fresh->mUnk_174 = def->mUnk_02 + gMapState.mCamera.mUnk_00;
            fresh->mUnk_176_10 = 0;
            fresh->mUnk_176_4 = 0;
            fresh->mUnk_176_9 = 0;
            fresh->mUnk_176_12 = 0;
            rec = fresh;
            off = fresh->mUnk_00->mUnk_00 * 16;
            fresh->mUnk_17c.x = GetMapEntryX((MapEntry *) (entries + off));
            fresh->mUnk_17c.y = GetMapEntryY((MapEntry *) (entries + off));
            fresh->mUnk_17c.z = GetMapEntryZ((MapEntry *) (entries + off));
            fresh->mUnk_188 = GetMapEntryAngle((MapEntry *) (entries + off));
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
                gMapState.mCamera.mUnk_2dc.mUnk_60++;
                UpdateAndDrawModel(rec->mUnk_04, &dp, flags);
            }
        }
        p += def->mUnk_03 * 2 + 4;
    }
}

// Clears the 20 records (and the camera's halfwords mUnk_00 and mUnk_02 when a == 1).
void func_02032b88(int a) {
    int i;

    for (i = 0; i < MAP_RECORDS; i++) {
        gMapState.mRecords[i].mUnk_00 = NULL;
    }
    if (a == 1) {
        gMapState.mCamera.mUnk_00 = 0;
        gMapState.mCamera.mUnk_02 = 0;
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
    c = gMapState.mCells + (px + pz * w);
    kind = c->mUnk_3;
    if (kind == 0) {
        k = 2;
        fx = px;
        fz = pz;
        nx = x0 + (stepx != 0 ? stepx >> 31 | 1 : 0);
        nz = z0 + (stepz != 0 ? stepz >> 31 | 1 : 0);
        c = gMapState.mCells + (nx + nz * w);
        kind = c->mUnk_3;
        if (kind == 0) {
            return 0;
        }
    }
    cur = gMapState.mCells + (x0 + z0 * w);
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
    u8 *area;
    int x0;
    int z0;
    int w;
    u32 saved = gMapState.mMarkerCount;
    u32 j;
    int tail;
    int head;
    int shift;
    int cx;
    int cz;
    int nr;
    u32 low;
    u32 b1;
    u32 b2;
    int col;
    int row;
    u32 idx;
    int left;
    int h;
    u32 idx0;
    u32 result;
    s16 box[6];
    u16 queue[256];
    u32 q;
    u32 v;
    u32 x;
    u32 z;
    int n;

    if (!gMapState.mCamera.mUnk_5b_3) {
        return;
    }
    area = (u8 *) GetMapChunk(MAP_CHUNK_AREAS);
    if (area == NULL) {
        OS_Terminate();
    }
    cx = px / gMapState.mCamera.mCellSize;
    cz = pz / gMapState.mCamera.mCellSize;
    n = *area;
    area += 2;
    left = n - 1;
    if (n != 0) {
        do {
            w = ((MapArea *) area)->mUnk_00;
            h = ((MapArea *) area)->mUnk_01;
            x0 = ((MapArea *) area)->mUnk_02;
            z0 = ((MapArea *) area)->mUnk_04;
            if (cx >= x0 && cx < x0 + w && cz >= z0 && cz < z0 + h) {
                cx -= x0;
                cz -= z0;
                break;
            }
            area += w * h * 2 + 6;
        } while (left-- != 0);
    }
    if (left < 0) {
        return;
    }
    tail = 0;
    head = 0;
    shift = 32 - LeadingZeroCount(w);
    if (h * w >= 10000) {
        OS_Terminate();
    }
    reg_G3X_POLYGON_ATTR = 0x10c0;
    reg_G3X_MTX_MODE = 2;
    G3_LoadMtx43(&gMapState.mUnk_3c);
    reg_G3X_MTX_STORE = 0x1e;
    box[0] = 0;
    box[1] = -0x1000;
    box[2] = 0;
    box[3] = 0x1000;
    box[4] = 0x2000;
    box[5] = 0x1000;
    MIi_CpuClearFast(0, gMapState.mUnk_28, 0x400);
    MIi_CpuClearFast(0, gMapState.mCells, 0x9c40);
    gMapState.mCells[cx + cz * w].mUnk_3 = 1;
    idx0 = cx + (cz << shift);
    gMapState.mUnk_28[idx0 >> 5] |= 1 << (idx0 & 0x1f);
    queue[tail++] = idx0;
    if (tail != head) {
        low = (1 << shift) - 1;
        b1 = *(u32 *) &box[2];
        b2 = *(u32 *) &box[4];
        do {
            q = queue[head];
            head = (u8) (head + 1);
            row = q >> shift;
            col = q & low;
            v = ((MapArea *) area)->mUnk_06[col + row * w];
            x = gMapState.mCamera.mCellSize * (col + x0);
            z = gMapState.mCamera.mCellSize * (row + z0);
            if ((v & 0x3ff) == 0) {
                gMapState.mCamera.mUnk_5b_2 = 1;
                gMapState.mUnk_dc = x;
                gMapState.mUnk_e0 = z;
            } else {
                gMapState.mMarkers[gMapState.mMarkerCount].mUnk_04 = x;
                gMapState.mMarkers[gMapState.mMarkerCount].mUnk_08 = z;
                gMapState.mMarkers[gMapState.mMarkerCount].mUnk_00 =
                    gMapState.mCamera.mUnk_58 - 1 + func_0203249c(v & 0x3ff);
                gMapState.mMarkerCount++;
                if (gMapState.mMarkerCount >= 0x180) {
                    OS_Terminate();
                }
            }
            j = 0;
            v &= 0xf000;
            while (v != 0) {
                if (v & 0x8000) {
                    int nc;
                    u32 bit;
                    u32 word;

                    nc = col + sMapDirs[j + 1];
                    nr = row + sMapDirs[j];
                    idx = nc + (nr << shift);
                    bit = 1 << (idx & 0x1f);
                    word = gMapState.mUnk_28[idx >> 5];

                    if (!(bit & word)) {
                        gMapState.mUnk_28[idx >> 5] = word | bit;
                        *(vu32 *) 0x04000450 = 0x1e;
                        PutTrans(gMapState.mCamera.mCellSize * (nc + x0), 0, gMapState.mCamera.mCellSize * (nr + z0));
                        PutScale(gMapState.mCamera.mCellSize, gMapState.mCamera.mCellSize << 4,
                                 gMapState.mCamera.mCellSize);
                        reg_G3X_BEGIN_VTXS = 0;
                        reg_G3X_END_VTXS = 0;
                        reg_G3X_BOX_TEST = *(u32 *) &box[0];
                        reg_G3X_BOX_TEST = b1;
                        reg_G3X_BOX_TEST = b2;
                        while (G3X_GetBoxTestResult(&result) != 0) {
                        }
                        if (result != 0 && func_02032bb8(nc, nr, w, cx, cz) != 0) {
                            gMapState.mCells[nc + nr * w].mUnk_3 = 1;
                            queue[tail] = idx;
                            tail = (u8) (tail + 1);
                        }
                    }
                }
                v = (v << 1) & 0xf000;
                j++;
            }
        } while (tail != head);
    }
    if (!gMapState.mCamera.mUnk_5b_2) {
        if (gMapState.mCamera.mUnk_5a == 1) {
            gMapState.mCamera.mUnk_5b_3 = 0;
            gMapState.mMarkerCount = saved;
        } else {
            gMapState.mCamera.mUnk_5b_4 = 0;
            gMapState.mCamera.mUnk_0c = 0;
            MIi_CpuClearFast(0, gMapState.mUnk_24, gMapState.mUnk_34);
            for (j = saved; j < gMapState.mMarkerCount; j++) {
                gMapState.mMarkers[j - saved] = gMapState.mMarkers[j];
            }
            gMapState.mMarkerCount -= saved;
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
// within the view distance is box-tested, kept in the table at MapState.mMarkers (with its class) when visible and its
// listed cells that are not marked yet are marked in the first bit set and queued for the same treatment.
void func_0203315c(int x, int z) {
    u32 mask = 0x7fff;
    int n;
    u16 *chunk;
    int m;
    u16 list[256];
    u32 result;
    u16 box[6];
    int cx;
    int cz;
    u16 *cells;
    u16 v;
    u16 cols;
    u16 rows;

    reg_G3X_POLYGON_ATTR = 0x10c0;
    n = 0;
    m = 0;
    reg_G3X_MTX_MODE = 2;
    G3_LoadMtx43(&gMapState.mUnk_3c);
    reg_G3X_MTX_STORE = 0x1e;
    box[0] = 0;
    box[1] = 0;
    box[2] = 0;
    box[3] = 0x1000;
    box[4] = 0x1000;
    box[5] = 0x1000;
    cx = (u32) x / gMapState.mCamera.mCellSize;
    cz = (u32) z / gMapState.mCamera.mCellSize;
    cols = gMapState.mCamera.mCellCountX;
    rows = gMapState.mCamera.mCellCountZ;
    if (!(cz < rows && cx < cols)) {
        return;
    }
    cells = GetMapCellChunk();
    chunk = GetMapChunk(MAP_CHUNK_CELL_RECS);
    if (chunk == 0) {
        OS_Terminate();
    }
    v = cells[cx + cz * gMapState.mCamera.mCellCountX];
    if (v & 0x8000) {
        v &= 0x7fff;
        gMapState.mCamera.mUnk_5b_3 = 1;
    }
    if (v != 0) {
        gMapState.mUnk_24[v >> 5] |= 1 << (v & 0x1f);
        list[0] = v;
        n++;
        gMapState.mCamera.mUnk_5b_4 = 1;
    }
    if (n != 0) {
        cx = *(u32 *) &box[2];
        cz = *(u32 *) &box[4];
        do {
            u16 idx = list[m] & 0x7fff;
            u16 *offs;
            MapCellRec *rec;
            u16 *p;
            u32 listlen;
            u32 i;
            int px, pz, py;
            u32 r;

            m = (u8) (m + 1);
            offs = gMapState.mCamera.mUnk_2dc.mUnk_50;
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
            r = (u32) gMapState.mCamera.mUnk_2dc.mUnk_44 >> 9;
            if (func_020322f4(px, pz, rec->mUnk_02, rec->mUnk_03) > r * r) {
                continue;
            }
            *(vu32 *) 0x04000450 = 0x1e;
            PutTrans(px, py, pz);
            PutScale(gMapState.mCamera.mCellSize * rec->mUnk_02, gMapState.mCamera.mCellSize * rec->mUnk_04,
                     gMapState.mCamera.mCellSize * rec->mUnk_03);
            reg_G3X_BEGIN_VTXS = 0;
            reg_G3X_END_VTXS = 0;
            reg_G3X_BOX_TEST = *(u32 *) &box[0];
            reg_G3X_BOX_TEST = cx;
            reg_G3X_BOX_TEST = cz;
            while (G3X_GetBoxTestResult(&result) != 0) {
            }
            if (result != 0 && rec->mUnk_12 > 1) {
                gMapState.mCamera.mUnk_08[gMapState.mCamera.mUnk_0c++] = &rec->mUnk_13;
            }
            if (result == 0 && gMapState.mMarkerCount != 0) {
                continue;
            }
            if (result != 0) {
                u32 k = func_02032368(rec->mUnk_00, px, pz, rec->mUnk_02, rec->mUnk_03);

                gMapState.mMarkers[gMapState.mMarkerCount].mUnk_04 = px;
                gMapState.mMarkers[gMapState.mMarkerCount].mUnk_08 = pz;
                gMapState.mMarkers[gMapState.mMarkerCount].mUnk_00 = gMapState.mCamera.mUnk_58 - 1 + func_0203249c(k);
                gMapState.mMarkerCount++;
                if (gMapState.mMarkerCount >= 0x180) {
                    OS_Terminate();
                }
            }
            p = (u16 *) rec + (rec->mUnk_12 - 1);
            for (i = 0; i < listlen; p++, i++) {
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
                if (gMapState.mUnk_24[c >> 5] & bit) {
                    continue;
                }
                gMapState.mUnk_24[c >> 5] |= bit;
                list[n] = c;
                n = (u8) (n + 1);
                if (n == m) {
                    OS_Terminate();
                }
            }
            mask = 0xffff;
        } while (n != m);
    }
}

// Moves the camera's look-at point and position a step (1 / 2^mEaseShift of the way) towards mLookAtGoal and
// mPosGoal, takes one step of the view angle blend and sets up the projection.
void EaseCameraToGoals(void) {
    u8 s = gMapState.mCamera.mEaseShift;

    gMapState.mCamera.mLookAt.x += (gMapState.mCamera.mLookAtGoal.x - gMapState.mCamera.mLookAt.x) >> s;
    gMapState.mCamera.mLookAt.y += (gMapState.mCamera.mLookAtGoal.y - gMapState.mCamera.mLookAt.y) >> s;
    gMapState.mCamera.mLookAt.z += (gMapState.mCamera.mLookAtGoal.z - gMapState.mCamera.mLookAt.z) >> s;
    gMapState.mCamera.mPos.x += (gMapState.mCamera.mPosGoal.x - gMapState.mCamera.mPos.x) >> s;
    gMapState.mCamera.mPos.y += (gMapState.mCamera.mPosGoal.y - gMapState.mCamera.mPos.y) >> s;
    gMapState.mCamera.mPos.z += (gMapState.mCamera.mPosGoal.z - gMapState.mCamera.mPos.z) >> s;
    func_02033b8c();
    func_02033520();
}

// The camera's field of view angle.
u16 func_020334ec(void) {
    return gMapState.mCamera.mUnk_2dc.mUnk_14;
}

// Sets the projection with the far distance scaled by 8 when a is 1, then restores it.
void func_020334f8(int a) {
    s32 saved = gMapState.mCamera.mUnk_2dc.mUnk_44;
    int sh;

    if (a == 1) {
        sh = 3;
    } else {
        sh = 0;
    }
    gMapState.mCamera.mUnk_2dc.mUnk_44 <<= sh;
    func_02033520();
    gMapState.mCamera.mUnk_2dc.mUnk_44 = saved;
}

// Sets up the projection (from the field of view angle and the far distance) and the view matrix from the camera
// position and target, once a map is loaded, and keeps the view matrix's rotation and the negated translation
// derived from it.
void func_02033520(void) {
    int a = func_020334ec();

    if (gMapState.mCamera.mMapFile != 0) {
        int i;

        func_0203cbe0();
        if (gMapState.mCamera.mPos.x == gMapState.mCamera.mLookAt.x &&
            gMapState.mCamera.mPos.y == gMapState.mCamera.mLookAt.y &&
            gMapState.mCamera.mPos.z == gMapState.mCamera.mLookAt.z) {
            gMapState.mCamera.mPos.y += 0xc800;
        }
        i = (a >> 4) * 2;
        func_0203d030(FX_SinCosTable_[i], FX_SinCosTable_[i + 1], 0x1555, 0x400, gMapState.mCamera.mUnk_2dc.mUnk_44,
                      0x1000, 1, &gMapState.mUnk_9c);
        *(vu32 *) 0x040004c8 = 0x20000000;
        func_0203d0f0(&gMapState.mCamera.mPos, (VecFx32 *) &gMapState.mCamera.mUnk_68, &gMapState.mCamera.mLookAt, 1,
                      &gMapState.mUnk_3c);
        func_0203b8bc((MtxFx33 *) &gMapState.mUnk_3c, (MtxFx33 *) &gMapState.mUnk_6c);
        func_0203b9b0((VecFx32 *) &gMapState.mUnk_3c._[9], (MtxFx33 *) &gMapState.mUnk_6c,
                      (VecFx32 *) &gMapState.mUnk_6c._[9]);
        gMapState.mUnk_6c._[9] = -gMapState.mUnk_6c._[9];
        gMapState.mUnk_6c._[10] = -gMapState.mUnk_6c._[10];
        gMapState.mUnk_6c._[11] = -gMapState.mUnk_6c._[11];
        reg_G3X_MTX_MODE = 2;
        gMapState.mCamera.mUnk_2dc.mUnk_4a = 0;
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

    gMapState.mCamera.mUnk_08 = Alloc(0x80);
    gMapState.mCamera.mUnk_0c = 0;
    if (gMapState.mCamera.mUnk_2dc.mUnk_42[0] != 0) {
        a = gMapState.mCamera.mUnk_2dc.mUnk_38;
        b = gMapState.mCamera.mUnk_2dc.mUnk_3c;
    } else {
        a = gMapState.mCamera.mUnk_2dc.mUnk_38 * 0xc800;
        b = gMapState.mCamera.mUnk_2dc.mUnk_3c * 0xc800;
    }
    SetFog(a, b, gMapState.mCamera.mUnk_2dc.mUnk_40);
    ClearLightMask();
    gMapState.mCamera.mUnk_44 = 0;
    gMapState.mCamera.mUnk_5b_4 = 0;
    gMapState.mCamera.mUnk_5b_3 = 0;
    gMapState.mCamera.mUnk_5b_2 = 0;
    gMapState.mMarkerCount = 0;
    MIi_CpuClearFast(0, gMapState.mUnk_24, gMapState.mUnk_34);
    func_0203315c(x, z);
    func_02032d50(x, z);
    if (gMapState.mCamera.mUnk_5b_2 == 1 && gMapState.mCamera.mUnk_5b_4 == 0) {
        func_0203315c(gMapState.mUnk_dc, gMapState.mUnk_e0);
    }
    gMapState.mCamera.mUnk_5a = (gMapState.mCamera.mUnk_5b_3 << 1) + gMapState.mCamera.mUnk_5b_4;
    gMapState.mCamera.mUnk_2dc.mUnk_60 = 0;
    if (gMapState.mCamera.mUnk_2dc.mUnk_5c == 0) {
        for (i = 0; i < gMapState.mCamera.mUnk_0c; i++) {
            func_020329d0(gMapState.mCamera.mUnk_08[i]);
        }
    }
    Free(&gMapState.mCamera.mUnk_08);
    marker.mUnk_43_4 = 0;
    marker.mUnk_43_3 = marker.mUnk_43_4;
    marker.mUnk_43 = 0;
    marker.mPos.y = 0;
    marker.mUnk_38 = 0;
    marker.mUnk_3a = 0;
    marker.mUnk_3c = 0;
    marker.mUnk_3e = 0xff;
    {
        s32 v = -(gMapState.mCamera.mUnk_2dc.mUnk_5a == 0);
        marker.mUnk_2c = v;
        marker.mUnk_30 = v;
        marker.mUnk_34 = v;
    }
    for (i = 0; i < gMapState.mMarkerCount; i++) {
        if ((gMapState.mUnk_38 & 0x8000) == 0 ||
            (gMapState.mUnk_38 & ~0x8000) + gMapState.mCamera.mUnk_58 == gMapState.mMarkers[i].mUnk_00) {
            MapMarker *m = &gMapState.mMarkers[i];
            marker.mPos.x = m->mUnk_04;
            marker.mPos.z = m->mUnk_08;
            DrawModelFile(m->mUnk_00 & 0x7fff, &marker, gMapState.mCamera.mUnk_5b_0 == 1 ? 0x10 : 0);
            if (*(vu16 *) 0x04000604 > gMapState.mCamera.mUnk_2dc.mUnk_54) {
                break;
            }
        }
    }
    if (gMapState.mCamera.mUnk_52 == 0) {
        return;
    }
    func_020334ec();
    func_020334f8(1);
    camera.mPos.x = gMapState.mCamera.mPos.x;
    camera.mPos.y = gMapState.mCamera.mPos.y;
    camera.mPos.z = gMapState.mCamera.mPos.z;
    camera.mUnk_43_3 = 0;
    camera.mUnk_43_4 = 0;
    camera.mUnk_38 = 0;
    camera.mUnk_3c = 0;
    camera.mUnk_3a = 0;
    camera.mUnk_28 = 0x1000;
    camera.mUnk_43 = 0;
    DrawModelFile(gMapState.mCamera.mUnk_52, &camera, gMapState.mCamera.mUnk_5b_1 == 0);
    func_020334f8(0);
}

// Moves the look-at point: with the divisor mUnk_4e set, 1 / mUnk_4e of the way to the blend point mUnk_9c.mUnk_228;
// otherwise towards pos in mUnk_0d steps from the point where pos last moved far (squared distance beyond mUnk_30
// squared) away.
void func_0203393c(VecFx32 *pos) {
    VecFx32 *from;
    VecFx32 *cur;
    u32 d;
    int dx;
    int dy;
    int dz;

    if (gMapState.mCamera.mUnk_2dc.mUnk_4e == 0) {
        if (gMapState.mCamera.mUnk_2dc.mUnk_0d < 2) {
            gMapState.mCamera.mLookAt.x = pos->x;
            gMapState.mCamera.mLookAt.y = pos->y;
            gMapState.mCamera.mLookAt.z = pos->z;
            gMapState.mCamera.mUnk_2dc.mUnk_0c = 0;
            return;
        }
        cur = &gMapState.mCamera.mUnk_2dc.mUnk_00;
        dy = (cur->y - pos->y) >> 9;
        dx = (cur->x - pos->x) >> 9;
        dz = (cur->z - pos->z) >> 9;
        d = dx * dx + dy * dy + dz * dz;
        from = &gMapState.mCamera.mUnk_9c.mUnk_234;
        if (d > (gMapState.mCamera.mUnk_2dc.mUnk_30 * gMapState.mCamera.mUnk_2dc.mUnk_30 >> 18)) {
            cur->x = pos->x;
            cur->y = pos->y;
            cur->z = pos->z;
            from->x = gMapState.mCamera.mLookAt.x;
            from->y = gMapState.mCamera.mLookAt.y;
            from->z = gMapState.mCamera.mLookAt.z;
            gMapState.mCamera.mUnk_2dc.mUnk_0c = 1;
        }
        if (gMapState.mCamera.mUnk_2dc.mUnk_0c == 0) {
            return;
        }
        gMapState.mCamera.mLookAt.x =
            MapRangeFromZero(gMapState.mCamera.mUnk_2dc.mUnk_0c, gMapState.mCamera.mUnk_2dc.mUnk_0d, from->x, cur->x);
        gMapState.mCamera.mLookAt.y =
            MapRangeFromZero(gMapState.mCamera.mUnk_2dc.mUnk_0c, gMapState.mCamera.mUnk_2dc.mUnk_0d, from->y, cur->y);
        gMapState.mCamera.mLookAt.z =
            MapRangeFromZero(gMapState.mCamera.mUnk_2dc.mUnk_0c, gMapState.mCamera.mUnk_2dc.mUnk_0d, from->z, cur->z);
        gMapState.mCamera.mUnk_2dc.mUnk_0c++;
        if (gMapState.mCamera.mUnk_2dc.mUnk_0c >= gMapState.mCamera.mUnk_2dc.mUnk_0d) {
            gMapState.mCamera.mUnk_2dc.mUnk_0c = 0;
        }
    } else {
        gMapState.mCamera.mLookAt.x +=
            (gMapState.mCamera.mUnk_9c.mUnk_228.x - gMapState.mCamera.mLookAt.x) / gMapState.mCamera.mUnk_2dc.mUnk_4e;
        gMapState.mCamera.mLookAt.y +=
            (gMapState.mCamera.mUnk_9c.mUnk_228.y - gMapState.mCamera.mLookAt.y) / gMapState.mCamera.mUnk_2dc.mUnk_4e;
        gMapState.mCamera.mLookAt.z +=
            (gMapState.mCamera.mUnk_9c.mUnk_228.z - gMapState.mCamera.mLookAt.z) / gMapState.mCamera.mUnk_2dc.mUnk_4e;
    }
}

// Moves the view angle a quarter of the way towards the direction from the camera to the look-at point (skipped
// once when mUnk_34[0] is set).
void func_02033b04(void) {
    s32 dx;
    s32 dz;
    s32 diff;
    VecFx32 *target;

    if (gMapState.mCamera.mUnk_2dc.mUnk_34[0] == 0) {
        target = &gMapState.mCamera.mLookAt;
        dx = gMapState.mCamera.mPos.x - target->x;
        dz = gMapState.mCamera.mPos.z - target->z;
        diff = AngleDelta(gMapState.mCamera.mUnk_2dc.mUnk_56, AngleOfVector(-dx >> 6, -dz >> 6));
        gMapState.mCamera.mUnk_2dc.mUnk_56 += (u16) (diff / 4);
    } else {
        gMapState.mCamera.mUnk_2dc.mUnk_34[0] = 0;
    }
}

// One step of the view angle blend from mUnk_16 to mUnk_18 over mUnk_11[0] steps (mUnk_10 counts them); c points at
// the blend counters from mUnk_0c on.
void func_02033b8c(void) {
    u8 *c = &gMapState.mCamera.mUnk_2dc.mUnk_0c;
    if (c[4] == 0) {
        return;
    }
    if (c[5] - 1 > 1) {
        gMapState.mCamera.mUnk_2dc.mUnk_14 =
            MapRangeFromZero(c[4], c[5] - 1, gMapState.mCamera.mUnk_2dc.mUnk_16, gMapState.mCamera.mUnk_2dc.mUnk_18);
        c[4]++;
        if (c[4] == c[5]) {
            c[4] = 0;
        }
    } else {
        gMapState.mCamera.mUnk_2dc.mUnk_14 = gMapState.mCamera.mUnk_2dc.mUnk_18;
        c[4] = 0;
    }
}

// The same for the camera position, with a blend point (mUnk_9c.mUnk_204) and a start point of its own.
void func_02033c0c(VecFx32 *pos) {
    u8 *c;
    int t;

    if (gMapState.mCamera.mUnk_2dc.mUnk_4d == 0) {
        c = &gMapState.mCamera.mUnk_2dc.mUnk_0c;
        func_02033b8c();
        if (c[3] < 2) {
            gMapState.mCamera.mPos.x = pos->x;
            gMapState.mCamera.mPos.y = pos->y;
            gMapState.mCamera.mPos.z = pos->z;
            gMapState.mCamera.mUnk_2dc.mUnk_0f = 0;
            return;
        }
        if (c[2] != 0) {
            t = c[3] - 1;
            if (t > 1) {
                gMapState.mCamera.mPos.x = MapRangeFromZero(c[2], t, gMapState.mCamera.mUnk_9c.mUnk_21c.x, pos->x);
                gMapState.mCamera.mPos.y = MapRangeFromZero(c[2], t, gMapState.mCamera.mUnk_9c.mUnk_21c.y, pos->y);
                gMapState.mCamera.mPos.z = MapRangeFromZero(c[2], t, gMapState.mCamera.mUnk_9c.mUnk_21c.z, pos->z);
                c[2]++;
                if (c[2] == c[3]) {
                    c[2] = 0;
                }
            } else {
                gMapState.mCamera.mPos.x = pos->x;
                gMapState.mCamera.mPos.y = pos->y;
                gMapState.mCamera.mPos.z = pos->z;
                c[2] = 0;
            }
        } else if (gMapState.mCamera.mPos.x != pos->x || gMapState.mCamera.mPos.y != pos->y ||
                   gMapState.mCamera.mPos.z != pos->z) {
            gMapState.mCamera.mUnk_9c.mUnk_21c.x = gMapState.mCamera.mPos.x;
            gMapState.mCamera.mUnk_9c.mUnk_21c.y = gMapState.mCamera.mPos.y;
            gMapState.mCamera.mUnk_9c.mUnk_21c.z = gMapState.mCamera.mPos.z;
            gMapState.mCamera.mUnk_2dc.mUnk_0e = 1;
        } else {
            gMapState.mCamera.mUnk_9c.mUnk_204.x = pos->x;
            gMapState.mCamera.mUnk_9c.mUnk_204.y = pos->y;
            gMapState.mCamera.mUnk_9c.mUnk_204.z = pos->z;
        }
    } else {
        gMapState.mCamera.mPos.x +=
            (gMapState.mCamera.mUnk_9c.mUnk_204.x - gMapState.mCamera.mPos.x) / gMapState.mCamera.mUnk_2dc.mUnk_4d;
        gMapState.mCamera.mPos.y +=
            (gMapState.mCamera.mUnk_9c.mUnk_204.y - gMapState.mCamera.mPos.y) / gMapState.mCamera.mUnk_2dc.mUnk_4d;
        gMapState.mCamera.mPos.z +=
            (gMapState.mCamera.mUnk_9c.mUnk_204.z - gMapState.mCamera.mPos.z) / gMapState.mCamera.mUnk_2dc.mUnk_4d;
    }
}

// Starts the view angle blend towards a, given in degrees (stored as a 16-bit angle).
void func_02033df4(int a) {
    gMapState.mCamera.mUnk_2dc.mUnk_16 = gMapState.mCamera.mUnk_2dc.mUnk_14;
    gMapState.mCamera.mUnk_2dc.mUnk_18 = ((u32) a << 16) / 360;
    gMapState.mCamera.mUnk_2dc.mUnk_10 = 1;
}

// Sets the blend point of the position blend (a 1 or 2, mUnk_9c.mUnk_204) or the look-at blend (a 7 or 8,
// mUnk_9c.mUnk_228) to entry b of chunk 0x5513 (-1: the current point) and clears its state; c is stored as the step
// count.
void func_02033e38(int a, int b, int c) {
    VecFx32 *dst = NULL;
    VecFx32 *src;
    MapEntry *e;

    switch (a) {
        case 1:
        case 2:
            dst = &gMapState.mCamera.mUnk_9c.mUnk_204;
            src = &gMapState.mCamera.mPos;
            gMapState.mCamera.mUnk_2dc.mUnk_4d = 0;
            gMapState.mCamera.mUnk_2dc.mUnk_32 = a;
            gMapState.mCamera.mUnk_2dc.mUnk_0f = c;
            gMapState.mCamera.mUnk_2dc.mUnk_0e = 0;
            break;
        case 7:
        case 8:
            dst = &gMapState.mCamera.mUnk_9c.mUnk_228;
            src = &gMapState.mCamera.mLookAt;
            gMapState.mCamera.mUnk_2dc.mUnk_4e = 0;
            gMapState.mCamera.mUnk_2dc.mUnk_33 = a;
            gMapState.mCamera.mUnk_2dc.mUnk_0d = c;
            gMapState.mCamera.mUnk_2dc.mUnk_0c = 0;
            break;
        default:
            OS_Terminate();
            break;
    }
    if (b != -1) {
        e = GetMapEntry(b, 0);
        if (e == NULL) {
            OS_Terminate();
        }
        dst->x = GetMapEntryX(e);
        dst->y = GetMapEntryY(e);
        dst->z = GetMapEntryZ(e);
    } else {
        dst->x = src->x;
        dst->y = src->y;
        dst->z = src->z;
    }
}

// Advances an animation track by the frames passed since its last update (1 when it has no timer).
void AdvanceCameraTrack(CameraTrack *t, s32 *whole, s32 *next, s32 *rest) {
    int step;

    if (t->mUnk_16_5 == 0) {
        if (t->mUnk_16_1 != 1) {
            step = 1;
        } else {
            step = gSystemState.mElapsedFrames - t->mUnk_0c;
            t->mUnk_0c = gSystemState.mElapsedFrames;
        }
    } else {
        step = 0;
        t->mUnk_0c = gSystemState.mElapsedFrames;
    }
    SplitAnimFrameAndAdvance(t->mUnk_10, &t->mUnk_12, t->mUnk_14, step, whole, next, rest, t->mUnk_16_2);
}
#pragma thumb on

// Moves the camera (position mode mUnk_32, look-at mode mUnk_33), then sets up the view. Position modes: 0 orbits
// target mUnk_1c at distance mUnk_28 and height mUnk_2c, turned by its angle; 1 and 2 blend towards mUnk_204; 3 plays
// track mUnk_1bc; 4 follows the target; 10 plays track mUnk_1ec and 11 the camera's own model, both giving a position
// and a look-at point. Look-at modes: 5 the position of target mUnk_20 and 6 its model point, raised by mUnk_24; 7
// and 8 blend towards mUnk_228; 9 plays track mUnk_1d4; 10 and 11 take the point mode 10 or 11 gave (jumping there
// when flag 3 or 4 of track mUnk_1ec is set, else moving an eighth of the way). While the top bit of
// gSystemState.mFlags is set only the tracks' timestamps follow the frame counter.
#pragma thumb off
void UpdateCamera(void) {
    VecFx32 eye;
    VecFx32 at;
    VecFx32 pos;
    s32 whole;
    s32 next;
    s32 rest;
    CameraTarget *target;
    u32 count;
    s32 dx;
    s32 dz;
    int idx;
    s32 x;
    s32 y;
    s32 z;

    if (gSystemState.mFlags & 0x80000000) {
        gMapState.mCamera.mUnk_9c.mUnk_1ec.mUnk_0c = gSystemState.mElapsedFrames;
        gMapState.mCamera.mUnk_9c.mUnk_1d4.mUnk_0c = gSystemState.mElapsedFrames;
        gMapState.mCamera.mUnk_9c.mUnk_1bc.mUnk_0c = gSystemState.mElapsedFrames;
        func_02033520();
        return;
    }
    target = gMapState.mCamera.mUnk_2dc.mUnk_1c;
    switch (gMapState.mCamera.mUnk_2dc.mUnk_32) {
        case 0:
            idx = (u16) (target->mDraw.mUnk_38 + 0x8000) >> 4 << 1;
            dx = gMapState.mCamera.mUnk_2dc.mUnk_28 * FX_SinCosTable_[idx];
            dz = gMapState.mCamera.mUnk_2dc.mUnk_28 * FX_SinCosTable_[idx + 1];
            y = target->mDraw.mPos.y + gMapState.mCamera.mUnk_2dc.mUnk_2c;
            x = target->mDraw.mPos.x + dx / 4096;
            z = target->mDraw.mPos.z - dz / 4096;
            gMapState.mCamera.mPos.y = y;
            gMapState.mCamera.mPos.x = x;
            gMapState.mCamera.mPos.z = z;
            break;
        case 3:
            AdvanceCameraTrack(&gMapState.mCamera.mUnk_9c.mUnk_1bc, &whole, &next, &rest);
            ReadAnimFileNode(gMapState.mCamera.mUnk_9c.mUnk_1bc.mUnk_10, NULL, &eye, NULL, whole, next, rest, 0);
            gMapState.mCamera.mUnk_9c.mUnk_1bc.mUnk_16_0 = gMapState.mCamera.mUnk_9c.mUnk_1bc.mUnk_12 == 0;
            if (gMapState.mCamera.mUnk_9c.mUnk_1bc.mUnk_16_2 == 1) {
                gMapState.mCamera.mUnk_9c.mUnk_1bc.mUnk_16_0 = gMapState.mCamera.mUnk_9c.mUnk_1bc.mUnk_12 == 0;
            } else {
                gMapState.mCamera.mUnk_9c.mUnk_1bc.mUnk_16_0 =
                    gMapState.mCamera.mUnk_9c.mUnk_1bc.mUnk_12 / gMapState.mCamera.mUnk_9c.mUnk_1bc.mUnk_14 ==
                    GetAnimFrameCount(gMapState.mCamera.mUnk_9c.mUnk_1bc.mUnk_10) - 2;
            }
            gMapState.mCamera.mPos.x = gMapState.mCamera.mUnk_9c.mUnk_1bc.mUnk_00.x + eye.x;
            gMapState.mCamera.mPos.y = gMapState.mCamera.mUnk_9c.mUnk_1bc.mUnk_00.y + eye.y;
            gMapState.mCamera.mPos.z = gMapState.mCamera.mUnk_9c.mUnk_1bc.mUnk_00.z + eye.z;
            func_02033b8c();
            break;
        case 1:
        case 2:
            func_02033c0c(&gMapState.mCamera.mUnk_9c.mUnk_204);
            break;
        case 4:
            gMapState.mCamera.mUnk_9c.mUnk_204 = target->mDraw.mPos;
            func_02033c0c(&gMapState.mCamera.mUnk_9c.mUnk_204);
            break;
        case 11:
            if (gMapState.mCamera.mUnk_9c.mModel.mUnk_170 != 0) {
                OS_Terminate();
            }
            UpdateAndDrawModel(&gMapState.mCamera.mUnk_9c.mModel, &gMapState.mCamera.mUnk_9c.mDraw, 0);
            count = gMapState.mCamera.mUnk_9c.mModel.mUnk_14c.mUnk_4;
            whole = DivideWithRemainder(gMapState.mCamera.mUnk_9c.mModel.mUnk_14c.mUnk_2, count, (u32 *) &rest);
            next = (u32) (whole + 1) % GetAnimFrameCount(gMapState.mCamera.mUnk_9c.mModel.mUnk_14c.mUnk_0);
            rest = (rest << 8) / count;
            ReadAnimFileNode(gMapState.mCamera.mUnk_9c.mModel.mUnk_14c.mUnk_0, NULL, &eye, NULL, whole, next, rest, 1);
            ReadAnimFileNode(gMapState.mCamera.mUnk_9c.mModel.mUnk_14c.mUnk_0, NULL, &at, NULL, whole, next, rest, 2);
            break;
        case 10:
            AdvanceCameraTrack(&gMapState.mCamera.mUnk_9c.mUnk_1ec, &whole, &next, &rest);
            if (gMapState.mCamera.mUnk_9c.mUnk_1ec.mUnk_16_2 == 1) {
                gMapState.mCamera.mUnk_9c.mUnk_1ec.mUnk_16_0 = gMapState.mCamera.mUnk_9c.mUnk_1ec.mUnk_12 == 0;
            } else {
                gMapState.mCamera.mUnk_9c.mUnk_1ec.mUnk_16_0 =
                    (u32) (gMapState.mCamera.mUnk_9c.mUnk_1ec.mUnk_12 / gMapState.mCamera.mUnk_9c.mUnk_1ec.mUnk_14) >=
                    GetAnimFrameCount(gMapState.mCamera.mUnk_9c.mUnk_1ec.mUnk_10) - 2;
            }
            ReadAnimFileNode(gMapState.mCamera.mUnk_9c.mUnk_1ec.mUnk_10, NULL, &eye, NULL, whole, next, rest, 1);
            ReadAnimFileNode(gMapState.mCamera.mUnk_9c.mUnk_1ec.mUnk_10, NULL, &at, NULL, whole, next, rest, 2);
            break;
        default:
            OS_Terminate();
            break;
    }
    target = gMapState.mCamera.mUnk_2dc.mUnk_20;
    switch (gMapState.mCamera.mUnk_2dc.mUnk_33) {
        case 6:
            pos = target->mModel.mUnk_158;
            pos.y += gMapState.mCamera.mUnk_2dc.mUnk_24;
            func_0203393c(&pos);
            func_02033b04();
            break;
        case 5:
            pos = target->mDraw.mPos;
            pos.y += gMapState.mCamera.mUnk_2dc.mUnk_24;
            func_0203393c(&pos);
            func_02033b04();
            break;
        case 7:
        case 8:
            func_0203393c(&gMapState.mCamera.mUnk_9c.mUnk_228);
            func_02033b04();
            break;
        case 9:
            AdvanceCameraTrack(&gMapState.mCamera.mUnk_9c.mUnk_1d4, &whole, &next, &rest);
            ReadAnimFileNode(gMapState.mCamera.mUnk_9c.mUnk_1d4.mUnk_10, NULL, &eye, NULL, whole, next, rest, 0);
            if (gMapState.mCamera.mUnk_9c.mUnk_1d4.mUnk_16_2 == 1) {
                gMapState.mCamera.mUnk_9c.mUnk_1d4.mUnk_16_0 = gMapState.mCamera.mUnk_9c.mUnk_1d4.mUnk_12 == 0;
            } else {
                gMapState.mCamera.mUnk_9c.mUnk_1d4.mUnk_16_0 =
                    gMapState.mCamera.mUnk_9c.mUnk_1d4.mUnk_12 / gMapState.mCamera.mUnk_9c.mUnk_1d4.mUnk_14 ==
                    GetAnimFrameCount(gMapState.mCamera.mUnk_9c.mUnk_1d4.mUnk_10) - 2;
            }
            gMapState.mCamera.mLookAt.x = gMapState.mCamera.mUnk_9c.mUnk_1d4.mUnk_00.x + eye.x;
            gMapState.mCamera.mLookAt.y = gMapState.mCamera.mUnk_9c.mUnk_1d4.mUnk_00.y + eye.y;
            gMapState.mCamera.mLookAt.z = gMapState.mCamera.mUnk_9c.mUnk_1d4.mUnk_00.z + eye.z;
            break;
        case 10:
        case 11:
            if (gMapState.mCamera.mUnk_9c.mUnk_1ec.mUnk_16_3 == 1 ||
                gMapState.mCamera.mUnk_9c.mUnk_1ec.mUnk_16_4 == 1) {
                gMapState.mCamera.mUnk_9c.mUnk_1ec.mUnk_16_3 = 0;
                gMapState.mCamera.mPos.x = gMapState.mCamera.mUnk_9c.mUnk_1ec.mUnk_00.x + eye.x;
                gMapState.mCamera.mPos.y = gMapState.mCamera.mUnk_9c.mUnk_1ec.mUnk_00.y + eye.y;
                gMapState.mCamera.mPos.z = gMapState.mCamera.mUnk_9c.mUnk_1ec.mUnk_00.z + eye.z;
                gMapState.mCamera.mLookAt.x = gMapState.mCamera.mUnk_9c.mUnk_1ec.mUnk_00.x + at.x;
                gMapState.mCamera.mLookAt.y = gMapState.mCamera.mUnk_9c.mUnk_1ec.mUnk_00.y + at.y;
                gMapState.mCamera.mLookAt.z = gMapState.mCamera.mUnk_9c.mUnk_1ec.mUnk_00.z + at.z;
            } else {
                gMapState.mCamera.mPos.x +=
                    (gMapState.mCamera.mUnk_9c.mUnk_1ec.mUnk_00.x + eye.x - gMapState.mCamera.mPos.x) / 8;
                gMapState.mCamera.mPos.y +=
                    (gMapState.mCamera.mUnk_9c.mUnk_1ec.mUnk_00.y + eye.y - gMapState.mCamera.mPos.y) / 8;
                gMapState.mCamera.mPos.z +=
                    (gMapState.mCamera.mUnk_9c.mUnk_1ec.mUnk_00.z + eye.z - gMapState.mCamera.mPos.z) / 8;
                gMapState.mCamera.mLookAt.x +=
                    (gMapState.mCamera.mUnk_9c.mUnk_1ec.mUnk_00.x + at.x - gMapState.mCamera.mLookAt.x) / 8;
                gMapState.mCamera.mLookAt.y +=
                    (gMapState.mCamera.mUnk_9c.mUnk_1ec.mUnk_00.y + at.y - gMapState.mCamera.mLookAt.y) / 8;
                gMapState.mCamera.mLookAt.z +=
                    (gMapState.mCamera.mUnk_9c.mUnk_1ec.mUnk_00.z + at.z - gMapState.mCamera.mLookAt.z) / 8;
            }
            func_02033b8c();
            break;
        default:
            OS_Terminate();
            break;
    }
    func_02033520();
}

// Sets the fog with the default density table.
void SetFog(int a, int b, int c) {
    SetFogEx(a, b, c, NULL);
}

// Turns the fog off when a and b are both 0. Otherwise it switches it on with the shift and offset worked out from
// a and b, sets the fog colour to c (opaque) and the clear colour to c, and loads the density table: the given one,
// else the camera's pending one (which it then forgets), else a ramp.
void SetFogEx(int a, int b, int c, const u16 *table) {
    u32 lim;
    u32 t;
    u32 i;
    u32 tab[8];

    if (a == 0 && b == 0) {
        func_0203cd9c(0, 0, 0, 0);
        return;
    }
    t = (u32) b >> 5;
    lim = 0x400;
    for (i = 0; i < 10; i++) {
        if (lim <= (t >> 9)) {
            break;
        }
        lim >>= 1;
    }
    func_0203cd9c(1, 0, i, (u32) (a - t) >> 9);
    *(vu32 *) 0x04000358 = (u16) c | 0x1f0000;
    G3X_SetClearColor((u16) c, 8, 0x7fff, 0, 0);
    if (table == NULL) {
        if (gMapState.mCamera.mUnk_2dc.mUnk_64 == 0) {
            for (i = 0; i < 8; i++) {
                tab[i] = 15 * i | (15 * i + 4) << 8 | (15 * i + 8) << 16 | (15 * i + 12) << 24;
            }
            func_0203cdf4((u16 *) tab);
        } else {
            func_0203cdf4(gMapState.mCamera.mUnk_2dc.mUnk_64);
            gMapState.mCamera.mUnk_2dc.mUnk_64 = 0;
        }
    } else {
        func_0203cdf4(table);
    }
}

// Light 0: colour (r, g, b, 5 bits each) and a direction that has only the component -z (in 1/8 units, 10 bits),
// under the identity matrix; marks light 0 as on in the camera object.
void SetLight0(int r, int g, int b, int z) {
    u32 color = (u16) (r | g << 5 | b << 10);
    s16 neg = -z;

    *(vu32 *) 0x04000454 = 0;
    *(vu32 *) 0x04000440 = 2;
    *(vu32 *) 0x040004cc = color;
    *(vu32 *) 0x040004c8 = (u32) ((neg >> 3) << 22) >> 2;
    gMapState.mCamera.mUnk_2dc.mLightMask = 1;
}

// Marks all lights as off.
void ClearLightMask(void) {
    gMapState.mCamera.mUnk_2dc.mLightMask = 0;
}
#pragma thumb on
