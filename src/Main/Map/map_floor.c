#include <nitro/os.h>
#include <Main/Map/map_floor.h>
#include <Main/Map/map_entries.h>
#include <Main/Graphics/model.h>
#include <Main/System/random_angle.h>
#include <Main/System/game_math.h>

#pragma thumb off

int func_02030830(fx32 x, fx32 z, fx32 y, int a, u32 mask, u8 *out, VecFx32 *vec) {
    int cz;
    int gz;
    int len;
    int t;
    int idx;
    int k;
    int count;
    int x0;
    int ha;
    int n;
    int off;
    int gx;
    u8 attr;
    u8 id;
    u32 bits;
    int h00;
    int h01;
    int h10;
    MapFloorRegion *region;
    int h11;
    int h;
    int z0;
    int ix;
    int cx;
    u8 *data;
    int nx;
    int inv;
    int d;
    int ny;
    int i;
    MapFloorShape *rec;
    int last;
    int dx;
    int cw;
    int vx;
    int ch;
    int vz;
    u16 *entry;
    int hc;
    VecFx32 *normp;
    VecFx32 *best;
    int dz;
    u8 *attrp;
    int iz;
    int idx2;
    int miss;
    u8 *regions;
    u16 *head;
    int ux;
    int px;
    int hb;
    int uz;
    int nz;
    u16 *grid;
    int info;
    int res;
    s16 lo;
    u16 *lists;
    int diff;
    int pz;
    s16 hi;
    int w;
    VecFx32 normals[12];
    s32 heights[12];
    u8 attrs[12];

    grid = func_02031eb8(MAP_FLOOR_CELLS);
    lists = func_02031eb8(MAP_FLOOR_LISTS);
    regions = (u8 *) func_02031eb8(MAP_FLOOR_REGIONS);
    count = 0;
    cx = x / 512;
    cz = z / 512;
    if (grid == NULL) {
        OS_Terminate();
    }
    if (lists == NULL) {
        OS_Terminate();
    }
    if (regions == NULL) {
        OS_Terminate();
    }

    gx = x / (s32) data_020c3e44.mUnk_2024.mUnk_40;
    gz = z / (s32) data_020c3e44.mUnk_2024.mUnk_40;
    if (gx >= data_020c3e44.mUnk_2024.mUnk_54 || gz >= data_020c3e44.mUnk_2024.mUnk_56) {
        if (out != NULL) {
            *out = 0;
        }
        if (vec != NULL) {
            vec->x = 0;
            vec->y = 0x1000;
            vec->z = 0;
        }
        return 0;
    }

    entry = lists + grid[data_020c3e44.mUnk_2024.mUnk_54 * gz + gx];
    normp = normals;
    attrp = attrs;
    do {
        region = (MapFloorRegion *) (regions + (*entry & MAP_FLOOR_INDEX) * 16);
        if (y >= (region->mUnk_10 << 9) - a && (a == 0 || y <= (region->mUnk_12 << 9)) && region->mUnk_00 <= cx &&
            region->mUnk_08 >= cx && region->mUnk_04 <= cz && region->mUnk_0c >= cz) {
            head = &region->mUnk_14;
            rec = (MapFloorShape *) (head + 2);
            miss = 0;
            if (!(*head & MAP_FLOOR_SINGLE)) {
                off = *head & 0x7fff;
                for (k = 0; k < region->mUnk_16; k++) {
                    miss = 1;
                    px = region->mUnk_00 + rec->mUnk_02[0].mUnk_00;
                    pz = region->mUnk_04 + rec->mUnk_02[0].mUnk_02;
                    id = rec->mUnk_01;
                    if (id == MAP_FLOOR_NONE || (mask & (1 << id))) {
                        rec = (MapFloorShape *) ((u8 *) rec + rec->mUnk_00 * 4 + 2);
                        continue;
                    }
                    for (n = 1; n < rec->mUnk_00; n++) {
                        int ox = px;
                        int oz = pz;
                        px = region->mUnk_00 + rec->mUnk_02[n].mUnk_00;
                        pz = region->mUnk_04 + rec->mUnk_02[n].mUnk_02;
                        if (Cross2d(ox, oz, px, pz, cx, cz) > 0) {
                            break;
                        }
                    }
                    if (n != rec->mUnk_00) {
                        rec = (MapFloorShape *) ((u8 *) rec + rec->mUnk_00 * 4 + 2);
                        continue;
                    }
                    miss = 0;
                    attr = id;
                    break;
                }
            } else {
                attr = *head & 0xff;
                off = 2;
            }
            if (!miss) {
                data = (u8 *) head + off;
                hi = region->mUnk_12;
                lo = region->mUnk_10;
                if (lo == hi) {
                    if (vec != NULL) {
                        normp->x = 0;
                        normp->y = 0x1000;
                        normp->z = 0;
                    }
                } else {
                    info = *(u16 *) data;
                    res = info & 0xff;
                    dx = cx - region->mUnk_00;
                    w = region->mUnk_08 - region->mUnk_00;
                    dz = cz - region->mUnk_04;
                    d = region->mUnk_0c - region->mUnk_04;
                    ix = dx * (res - 1) / w;
                    iz = dz * (res - 1) / d;
                    inv = 0x10000 / (res - 1);
                    x0 = ix * w * inv >> 16;
                    cw = ((ix + 1) * w * inv >> 16) - x0;
                    dx -= x0;
                    z0 = iz * d * inv >> 16;
                    ch = ((iz + 1) * d * inv >> 16) - z0;
                    dz -= z0;
                    idx = iz * res + ix;
                    if (!(info & MAP_FLOOR_WIDE)) {
                        idx2 = idx + res;
                        h00 = data[idx + 2];
                        h01 = data[idx + 3];
                        h10 = data[idx2 + 2];
                        h11 = data[idx2 + 3];
                        bits = 8;
                    } else {
                        idx2 = idx + res;
                        h00 = ((u16 *) data)[idx + 1];
                        h01 = ((u16 *) data)[idx + 2];
                        h10 = ((u16 *) data)[idx2 + 1];
                        h11 = ((u16 *) data)[idx2 + 2];
                        bits = 16;
                    }

                    if (vec != NULL) {
                        diff = hi - lo;
                        if (dx > cw >> 1) {
                            if (dz > ch >> 1) {
                                ha = diff * h11 >> bits;
                                hb = diff * h10 >> bits;
                                hc = diff * h01 >> bits;
                                vx = -cw;
                                vz = 0;
                                ux = 0;
                                uz = -ch;
                            } else {
                                ha = diff * h01 >> bits;
                                hb = diff * h11 >> bits;
                                hc = diff * h00 >> bits;
                                vx = 0;
                                vz = ch;
                                ux = -cw;
                                uz = 0;
                            }
                        } else {
                            if (dz > ch >> 1) {
                                ha = diff * h10 >> bits;
                                hb = diff * h00 >> bits;
                                hc = diff * h11 >> bits;
                                vx = 0;
                                vz = -ch;
                                ux = cw;
                                uz = 0;
                            } else {
                                ha = diff * h00 >> bits;
                                hb = diff * h01 >> bits;
                                hc = diff * h10 >> bits;
                                vx = cw;
                                vz = 0;
                                ux = 0;
                                uz = ch;
                            }
                        }
                        hb -= ha;
                        hc -= ha;
                        nx = hc * vz - uz * hb;
                        ny = uz * vx - ux * vz;
                        nz = ux * hb - hc * vx;
                        while ((nx >= 0 ? nx : -nx) + (ny >= 0 ? ny : -ny) + (nz >= 0 ? nz : -nz) >= 0x8000) {
                            nx >>= 1;
                            ny >>= 1;
                            nz >>= 1;
                        }
                        len = SqrtU32(nx * nx + ny * ny + nz * nz);
                        if (len == 0) {
                            normp->x = 0;
                            normp->y = 0x1000;
                            normp->z = 0;
                        } else {
                            normp->x = (nx << 12) / len;
                            normp->y = (ny << 12) / len;
                            normp->z = (nz << 12) / len;
                        }
                    }

                    if (bits > 8) {
                        s64 sum = (s64) h00 * ((s64) (cw - dx) * (ch - dz));
                        sum += (s64) h01 * ((s64) dx * (ch - dz));
                        sum += (s64) h10 * ((s64) dz * (cw - dx));
                        sum += (s64) h11 * ((s64) dx * dz);
                        if (sum < cw * ch) {
                            miss = 1;
                        } else {
                            t = sum / (cw * ch);
                        }
                    } else {
                        t = h00 * ((cw - dx) * (ch - dz));
                        t += h01 * (dx * (ch - dz));
                        t += h10 * (dz * (cw - dx));
                        t += h11 * (dx * dz);
                        if (t < cw * ch) {
                            miss = 1;
                        } else {
                            t /= cw * ch;
                        }
                    }
                }
                if (!miss) {
                    int slot;

                    h = (region->mUnk_10 + ((region->mUnk_12 - region->mUnk_10) * t >> bits)) << 9;
                    if (count >= 12) {
                        OS_Terminate();
                    }
                    slot = count;
                    *attrp = attr;
                    attrp++;
                    normp++;
                    count++;
                    heights[slot] = h;
                }
            }
        }
        h = *entry;
        entry++;
    } while (!(h & MAP_FLOOR_LAST));

    last = count - 1;
    for (i = 0; i < last; i++) {
        if (heights[i] < heights[i + 1]) {
            s32 th = heights[i];
            u8 ta;
            heights[i] = heights[i + 1];
            heights[i + 1] = th;
            ta = attrs[i];
            attrs[i] = attrs[i + 1];
            attrs[i + 1] = ta;
            if (vec != NULL) {
                VecFx32 tv = normals[i];
                normals[i] = normals[i + 1];
                normals[i + 1] = tv;
            }
            i = -1;
        }
    }

    best = NULL;
    attr = MAP_FLOOR_NONE;
    for (i = 0; i < count; i++) {
        h = heights[i];
        if (y > h - a) {
            if (y <= h || a == 0) {
                attr = attrs[i];
            }
            best = &normals[i];
            y = h;
            break;
        }
    }
    if (out != NULL) {
        *out = attr;
    }
    if (vec != NULL) {
        if (best != NULL) {
            *vec = *best;
        } else {
            vec->x = 0;
            vec->y = 0x1000;
            vec->z = 0;
        }
    }
    return y;
}

int func_02031270(fx32 x, fx32 z, fx32 y, int a, u32 mask, u8 *out) {
    return func_02030830(x, z, y, a, mask, out, NULL);
}

#pragma thumb on

u32 func_0203129c(void) {
    return AngleOfVector(data_020c3e44.mUnk_2024.mUnk_14.x - data_020c3e44.mUnk_2024.mUnk_20.x,
                         data_020c3e44.mUnk_2024.mUnk_14.z - data_020c3e44.mUnk_2024.mUnk_20.z);
}
