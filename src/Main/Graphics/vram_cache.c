#include <Main/System/system_init.h>
#include <Main/System/screen_fade.h>
#include <nitro/math.h>
#include <nitro/os.h>
#include <Main/System/random_angle.h>
#include <Main/Graphics/vram_cache.h>
#include <Main/Graphics/model.h>
#include <Main/Graphics/texture_blocks.h>
#include <Main/System/file_loader.h>

#pragma thumb off

// Frees block i of the first pool, merges it with free neighbours and links it into the free list ordered by size.
u32 func_020225dc(u32 i) {
    Unk_PoolRange *n = data_020c31dc.mUnk_08;
    u32 q;
    u32 j;
    u32 k;
    u32 size;

    n[i].mUnk_0 &= ~0x8000;
    j = i + n[i].mUnk_0;
    if (j < data_020c31dc.mUnk_62 && !(n[j].mUnk_0 & 0x8000)) {
        n[i].mUnk_0 += n[j].mUnk_0;
        n[i + n[i].mUnk_0].mUnk_6 = i;
        n[n[j].mUnk_4].mUnk_2 = n[j].mUnk_2;
        n[n[j].mUnk_2].mUnk_4 = n[j].mUnk_4;
    }
    k = n[i].mUnk_6;
    if (k < data_020c31dc.mUnk_62 && !(n[k].mUnk_0 & 0x8000)) {
        n[k].mUnk_0 += n[i].mUnk_0;
        n[k + n[k].mUnk_0].mUnk_6 = k;
        n[n[k].mUnk_4].mUnk_2 = n[k].mUnk_2;
        n[n[k].mUnk_2].mUnk_4 = n[k].mUnk_4;
        i = k;
    }
    size = n[i].mUnk_0;
    if (size < 4) {
        j = data_020c31dc.mUnk_62 + 1;
        q = n[j].mUnk_2;
        while (size > n[q].mUnk_0) {
            j = q;
            q = n[q].mUnk_2;
        }
        n[i].mUnk_2 = q;
        n[i].mUnk_4 = j;
        n[j].mUnk_2 = i;
        n[q].mUnk_4 = i;
    } else {
        j = data_020c31dc.mUnk_62;
        q = n[j].mUnk_4;
        while (size < n[q].mUnk_0) {
            j = q;
            q = n[q].mUnk_4;
        }
        n[i].mUnk_4 = q;
        n[i].mUnk_2 = j;
        n[j].mUnk_4 = i;
        n[q].mUnk_2 = i;
    }
    return n[i].mUnk_0;
}

// Evicts cache slot i of the first pool: clears its hash entry, frees its block and returns the slot to the free
// slot list.
u32 func_020227b4(u32 i) {
    u32 result;

    data_020c31dc.mUnk_60--;
    data_020c31dc.mUnk_04[data_020c31dc.mUnk_00[i].mUnk_08].mUnk_00 = -2;
    result = func_020225dc(data_020c31dc.mUnk_00[i].mUnk_00 >> 9);
    data_020c31dc.mUnk_00[1].mUnk_0c = data_020c31dc.mUnk_00[i].mUnk_0c;
    data_020c31dc.mUnk_00[data_020c31dc.mUnk_00[i].mUnk_0c].mUnk_0a = 1;
    data_020c31dc.mUnk_00[i].mUnk_0a = data_020c31dc.mUnk_00[1].mUnk_0a;
    data_020c31dc.mUnk_00[1].mUnk_0a = i;
    return result;
}

// Finds a free block of at least `size` bytes (counted in 512-byte units) in the first pool, evicting slots until
// one fits, and splits it.
u32 func_0202283c(Unk_PoolRange *n, u32 size, u32 slot) {
    unsigned long q;
    int found;
    u32 p;
    u32 spare;
    u32 next;
    u32 addr;
    u32 victim;

    size = (size >> 9) + ((size & 0x1ff) != 0);
    for (;;) {
        q = n[(data_020c18bc.mUnk_070.mUnk_2c << 14) / 512].mUnk_4;
        found = -1;
        while (size <= n[q].mUnk_0) {
            found = q;
            q = n[q].mUnk_4;
        }
        if (found != -1) {
            break;
        }
        victim = 1;
        do {
            victim = data_020c31dc.mUnk_00[victim].mUnk_0c;
        } while (func_020227b4(victim) < size);
    }
    spare = n[found].mUnk_0;
    n[found].mUnk_0 = size + 0x8000;
    addr = found << 9;
    data_020c31dc.mUnk_00[slot].mUnk_00 = addr;
    p = n[found].mUnk_2;
    n[n[found].mUnk_4].mUnk_2 = p;
    n[n[found].mUnk_2].mUnk_4 = n[found].mUnk_4;
    if (spare - size == 0) {
        return addr;
    }
    next = found + size;
    n[next].mUnk_0 = spare - size;
    n[next].mUnk_6 = found;
    n[next + (spare - size)].mUnk_6 = next;
    while (spare - size < n[q].mUnk_0) {
        p = q;
        q = n[q].mUnk_4;
    }
    n[next].mUnk_4 = q;
    n[next].mUnk_2 = p;
    n[p].mUnk_4 = next;
    n[q].mUnk_2 = next;
    return addr;
}

// Looks up `key` in the first pool's hash table; on a miss it claims a slot and a block and starts the transfer
// into it.
u32 func_020229b4(u32 kind, u32 key, u32 size, u32 srcSize) {
    u32 idx;
    int first = -1;
    u32 slot;
    u32 step;
    u32 off;
    TexInfo info;

    step = data_02050378[(key >> 10) & 0x7f];
    idx = key & 0x3ff;
    data_020c31dc.mUnk_4c++;
    for (;;) {
        data_020c31dc.mUnk_50++;
        if (key == data_020c31dc.mUnk_04[idx].mUnk_00) {
            slot = data_020c31dc.mUnk_04[idx].mUnk_04;
            func_0201fa08((IndexNode16 *) data_020c31dc.mUnk_00, slot);
            return data_020c31dc.mUnk_00[slot].mUnk_00;
        }
        if (data_020c31dc.mUnk_04[idx].mUnk_00 == -2 && first == -1) {
            first = idx;
        } else if (data_020c31dc.mUnk_04[idx].mUnk_00 == -1 ||
                   (data_020c31dc.mUnk_04[idx].mUnk_00 == -2 && idx == first)) {
            u16 count = data_020c31dc.mUnk_60 + 1;

            data_020c31dc.mUnk_60 = count;
            if (count > 0x2bc) {
                func_020227b4(data_020c31dc.mUnk_00[1].mUnk_0c);
            }
            if (srcSize == 0) {
                srcSize = size;
            }
            slot = data_020c31dc.mUnk_00[1].mUnk_0a;
            data_020c31dc.mUnk_00[1].mUnk_0a = data_020c31dc.mUnk_00[slot].mUnk_0a;
            data_020c31dc.mUnk_00[slot].mUnk_0a = data_020c31dc.mUnk_00[0].mUnk_0a;
            data_020c31dc.mUnk_00[slot].mUnk_0c = 0;
            data_020c31dc.mUnk_00[0].mUnk_0a = slot;
            data_020c31dc.mUnk_00[data_020c31dc.mUnk_00[slot].mUnk_0a].mUnk_0c = slot;
            if (first != -1) {
                idx = first;
            }
            data_020c31dc.mUnk_04[idx].mUnk_00 = key;
            data_020c31dc.mUnk_04[idx].mUnk_04 = slot;
            data_020c31dc.mUnk_00[slot].mUnk_08 = idx;
            data_020c31dc.mUnk_00[slot].mUnk_00 = func_0202283c(data_020c31dc.mUnk_08, size, slot);
            if (kind == 1) {
                u32 shift;
                u8 flag;
                s16 bits;

                off = key - func_0201c538();
                func_0202ae84(off >> 6, &info, off & 0x3f);
                flag = (info.mUnk_12 != 1 && info.mUnk_12 != 4) ? 0xff : 0;
                if (data_020c267c.mUnk_b2e == 2) {
                    kind = (info.mUnk_00 << 7) + info.mUnk_13;
                } else {
                    kind = info.mUnk_00 << 5;
                }
                bits = info.mUnk_0c;
                shift = 32 - CountLeadingZeros(~bits & (bits - 1));
                func_0201c580(func_0202ad68(), kind, data_020c31dc.mUnk_00[slot].mUnk_00 | 0xa0000000, size, 0, shift,
                              flag);
            } else if (kind == 2) {
                off = key - func_0201c550();
                func_0201c580(data_020c18bc.mUnk_42c, off, data_020c31dc.mUnk_00[slot].mUnk_00 | 0xa0000000, srcSize,
                              size, 0, 0);
            } else {
                func_0201c580(data_020c31dc.mUnk_54, kind, data_020c31dc.mUnk_00[slot].mUnk_00 | 0xa0000000, srcSize,
                              size, 0, 0);
            }
            data_020c18bc.mUnk_2bc += size;
            data_020c18bc.mUnk_2b8 += size;
            return data_020c31dc.mUnk_00[slot].mUnk_00;
        }
        idx = (idx + step) & 0x3ff;
    }
}

// Same as func_020225dc for the second pool.
u32 func_02022cdc(u32 i) {
    Unk_PoolRange *n = data_020c31dc.mUnk_14;
    u32 q;
    u32 j;
    u32 k;
    u32 size;

    n[i].mUnk_0 &= ~0x8000;
    j = i + n[i].mUnk_0;
    if (j < data_020c31dc.mUnk_64 && !(n[j].mUnk_0 & 0x8000)) {
        n[i].mUnk_0 += n[j].mUnk_0;
        n[i + n[i].mUnk_0].mUnk_6 = i;
        n[n[j].mUnk_4].mUnk_2 = n[j].mUnk_2;
        n[n[j].mUnk_2].mUnk_4 = n[j].mUnk_4;
    }
    k = n[i].mUnk_6;
    if (k < data_020c31dc.mUnk_64 && !(n[k].mUnk_0 & 0x8000)) {
        n[k].mUnk_0 += n[i].mUnk_0;
        n[k + n[k].mUnk_0].mUnk_6 = k;
        n[n[k].mUnk_4].mUnk_2 = n[k].mUnk_2;
        n[n[k].mUnk_2].mUnk_4 = n[k].mUnk_4;
        i = k;
    }
    size = n[i].mUnk_0;
    if (size < 4) {
        j = data_020c31dc.mUnk_64 + 1;
        q = n[j].mUnk_2;
        while (size > n[q].mUnk_0) {
            j = q;
            q = n[q].mUnk_2;
        }
        n[i].mUnk_2 = q;
        n[i].mUnk_4 = j;
        n[j].mUnk_2 = i;
        n[q].mUnk_4 = i;
    } else {
        j = data_020c31dc.mUnk_64;
        q = n[j].mUnk_4;
        while (size < n[q].mUnk_0) {
            j = q;
            q = n[q].mUnk_4;
        }
        n[i].mUnk_4 = q;
        n[i].mUnk_2 = j;
        n[j].mUnk_4 = i;
        n[q].mUnk_2 = i;
    }
    return n[i].mUnk_0;
}

// Same as func_020227b4 for the second pool.
u32 func_02022eb4(u32 i) {
    u32 result;

    data_020c31dc.mUnk_5e--;
    data_020c31dc.mUnk_10[data_020c31dc.mUnk_0c[i].mUnk_08].mUnk_00 = -2;
    result = func_02022cdc(data_020c31dc.mUnk_0c[i].mUnk_00 >> 5);
    data_020c31dc.mUnk_0c[1].mUnk_0c = data_020c31dc.mUnk_0c[i].mUnk_0c;
    data_020c31dc.mUnk_0c[data_020c31dc.mUnk_0c[i].mUnk_0c].mUnk_0a = 1;
    data_020c31dc.mUnk_0c[i].mUnk_0a = data_020c31dc.mUnk_0c[1].mUnk_0a;
    data_020c31dc.mUnk_0c[1].mUnk_0a = i;
    return result;
}

// Same as func_0202283c for the second pool (counted in 32-byte units).
u32 func_02022f40(Unk_PoolRange *n, u32 size, u32 slot) {
    unsigned long q;
    int found;
    u32 p;
    u32 spare;
    u32 next;
    u32 addr;
    u32 victim;

    size = (size >> 5) + ((size & 0x1f) != 0);
    for (;;) {
        q = n[(data_020c18bc.mUnk_040.mUnk_2c << 14) / 32].mUnk_4;
        found = -1;
        while (size <= n[q].mUnk_0) {
            found = q;
            q = n[q].mUnk_4;
        }
        if (found != -1) {
            break;
        }
        victim = 1;
        do {
            victim = data_020c31dc.mUnk_0c[victim].mUnk_0c;
        } while (func_02022eb4(victim) < size);
    }
    spare = n[found].mUnk_0;
    n[found].mUnk_0 = size + 0x8000;
    addr = found << 5;
    data_020c31dc.mUnk_0c[slot].mUnk_00 = addr;
    p = n[found].mUnk_2;
    n[n[found].mUnk_4].mUnk_2 = p;
    n[n[found].mUnk_2].mUnk_4 = n[found].mUnk_4;
    if (spare - size == 0) {
        return addr;
    }
    next = found + size;
    n[next].mUnk_0 = spare - size;
    n[next].mUnk_6 = found;
    n[next + (spare - size)].mUnk_6 = next;
    while (spare - size < n[q].mUnk_0) {
        p = q;
        q = n[q].mUnk_4;
    }
    n[next].mUnk_4 = q;
    n[next].mUnk_2 = p;
    n[p].mUnk_4 = next;
    n[q].mUnk_2 = next;
    return addr;
}

// Same as func_020229b4 for the second pool.
u32 func_020230b0(u32 kind, u32 key, u32 size) {
    u32 idx = key & 0xff;
    int first = -1;
    u32 step = data_02050378[(key >> 8) & 0x7f];
    u32 slot;

    for (;;) {
        if (key == data_020c31dc.mUnk_10[idx].mUnk_00) {
            slot = data_020c31dc.mUnk_10[idx].mUnk_04;
            func_0201fa08((IndexNode16 *) data_020c31dc.mUnk_0c, slot);
            return data_020c31dc.mUnk_0c[slot].mUnk_00;
        }
        if (data_020c31dc.mUnk_10[idx].mUnk_00 == -2 && first == -1) {
            first = idx;
        } else if (data_020c31dc.mUnk_10[idx].mUnk_00 == -1) {
            data_020c31dc.mUnk_5e++;
            if (data_020c31dc.mUnk_5e > 200) {
                func_02022eb4(data_020c31dc.mUnk_0c[1].mUnk_0c);
            }
            slot = data_020c31dc.mUnk_0c[1].mUnk_0a;
            data_020c31dc.mUnk_0c[1].mUnk_0a = data_020c31dc.mUnk_0c[slot].mUnk_0a;
            data_020c31dc.mUnk_0c[slot].mUnk_0a = data_020c31dc.mUnk_0c[0].mUnk_0a;
            data_020c31dc.mUnk_0c[slot].mUnk_0c = 0;
            data_020c31dc.mUnk_0c[0].mUnk_0a = slot;
            data_020c31dc.mUnk_0c[data_020c31dc.mUnk_0c[slot].mUnk_0a].mUnk_0c = slot;
            if (first != -1) {
                idx = first;
            }
            data_020c31dc.mUnk_10[idx].mUnk_00 = key;
            data_020c31dc.mUnk_10[idx].mUnk_04 = slot;
            data_020c31dc.mUnk_0c[slot].mUnk_08 = idx;
            data_020c31dc.mUnk_0c[slot].mUnk_00 = func_02022f40(data_020c31dc.mUnk_14, size, slot);
            data_020c31dc.mUnk_0c[slot].mUnk_04 = size;
            if (key < func_0201c544()) {
                func_0201c6b0(data_020c31dc.mUnk_56, kind, data_020c31dc.mUnk_0c[slot].mUnk_00 | 0xb0000000, size);
            } else if (key < func_0201c564()) {
                func_0201c6b0(func_0202ad58(), kind << 5, data_020c31dc.mUnk_0c[slot].mUnk_00 | 0xb0000000, size);
            } else {
                func_0201c6b0(0, data_020c18bc.mUnk_390[key - func_0201c564()] + 4,
                              data_020c31dc.mUnk_0c[slot].mUnk_00 | 0xb0000000, size);
            }
            data_020c18bc.mUnk_2c4 += size;
            data_020c18bc.mUnk_2c0 += size;
            return data_020c31dc.mUnk_0c[slot].mUnk_00;
        }
        idx = (idx + step) & 0x3ff;
    }
}

// Evaluates map `map` into out/pos/vec. With *done set it combines the new result with the values already in out.
void func_020232f0(u32 map, u16 *out, s32 *pos, s32 *vec, u32 d, int *done, const Unk_PoseNode *node) {
    u16 r[3];
    s32 p[3];
    s32 v[3];
    u32 kind;

    if (data_020c31dc.mUnk_18[map].mUnk_00 == NULL) {
        return;
    }
    if (*done == 0) {
        func_0202350c(out, pos, vec, d, map);
        *done = 1;
        return;
    }
    func_0202350c(r, p, v, d, map);
    kind = data_020c31dc.mUnk_18[map].mUnk_0d & 3;
    if (kind == 1) {
        out[0] += func_0201fe34((u16) (node->mUnk_5 << 8), r[0]);
        out[1] += func_0201fe34((u16) (node->mUnk_6 << 8), r[1]);
        out[2] += func_0201fe34((u16) (node->mUnk_7 << 8), r[2]);
    } else if (kind == 2) {
        if (func_0201fe14((u16) (node->mUnk_5 << 8), r[0]) > 0x100 ||
            func_0201fe14((u16) (node->mUnk_6 << 8), r[1]) > 0x100 ||
            func_0201fe14((u16) (node->mUnk_7 << 8), r[2]) > 0x100) {
            out[0] = r[0];
            out[1] = r[1];
            out[2] = r[2];
        }
    } else {
        OS_Terminate();
    }
}

void func_02023478(u16 a, u16 *rot, VecFx32 *pos, VecFx32 *vec, int e, int f, int g, int h) {
    data_020c31dc.mUnk_18[0].mUnk_0c = g;
    data_020c31dc.mUnk_18[0].mUnk_08 = f;
    data_020c31dc.mUnk_18[0].mUnk_04 = e;
    data_020c31dc.mUnk_18[0].mUnk_00 = func_0202ded8(a);
    func_0202350c(rot, (s32 *) pos, (s32 *) vec, h, 0);
    if (pos != NULL) {
        pos->x >>= 3;
        pos->y >>= 3;
        pos->z >>= 3;
    }
}

#pragma thumb on
