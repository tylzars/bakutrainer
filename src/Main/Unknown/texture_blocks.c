#include <Main/clib.h>
#include <Main/Unknown/globals.h>
#include <Main/Unknown/system_init.h>
#include <Main/Unknown/bg_image.h>
#include <Main/Unknown/functions.h>
#include <Main/Unknown/vram_cache.h>
#include <nitro/mi.h>
#include <nitro/os.h>
#include <Main/Unknown/texture_blocks.h>

// Block counts by texture size code, used by func_0202b32c.
const u16 data_02050978[16] = {
    1, 4, 0x10, 0x40, 2, 4, 8, 0x20, 2, 4, 8, 0x20, 0x40, 0x80, 0x80, 0x100,
};

const u8 data_02050998[16][2] = {
    {8, 8},  {16, 16}, {32, 32}, {64, 64}, {16, 8},   {32, 8},   {32, 16},  {64, 32},
    {8, 16}, {8, 32},  {16, 32}, {32, 64}, {128, 32}, {64, 128}, {128, 64}, {128, 128},
};

#pragma thumb off

void func_0202ad34(void) {}

void func_0202ad38(void) {}

void func_0202ad3c(void) {}

int func_0202ad40(void) {
    return 0;
}

void func_0202ad48(int a) {}

void func_0202ad4c(void) {}

void func_0202ad50(void) {}

void func_0202ad54(void) {}

u16 func_0202ad58(void) {
    return data_020c267c.mUnk_b24;
}

u16 func_0202ad68(void) {
    return data_020c267c.mUnk_b26;
}

// Mode byte (mUnk_577) of the half for layer 1 (layer 2 selects the other half).
u8 func_0202ad78(int layer) {
    return data_020c267c.mUnk_018[layer != 1].mUnk_577;
}

s16 func_0202ad9c(void) {
    return data_020c267c.mUnk_b28;
}

s8 func_0202adac(void) {
    return data_020c267c.mUnk_b2c;
}

// Header of block `index` in the packed file's block table: the table holds each block's offset from its start.
u32 *func_0202adc0(u32 index) {
    u8 *table = (u8 *) data_020c267c.mUnk_b20;
    return (u32 *) (table + *(u32 *) (table + ((u16) index << 2) + 0x10));
}

u16 func_0202ade0(void) {
    return data_020c267c.mUnk_b2a;
}

// Number of OAM entries to clear for a layer: 128, or fewer on the screen that func_02039a64 reports when
// the flag byte or bit 9 of the system flags is set.
u32 func_0202adf0(int layer) {
    if (layer == func_02039a64()) {
        if (data_020c18bc.mUnk_250.mUnk_28[0] != 0) {
            return 0x7e;
        }
        if (data_020c18bc.mUnk_46c & 0x200) {
            return 0x7f;
        }
    }
    return 0x80;
}

// Hides every sprite of a layer (OAM filled with 0x200) and clears the flag byte of the other layer's half.
void func_0202ae34(int layer) {
    int sub = layer & 1;
    u32 oam = sub ? 0x7000000 : 0x7000400;

    MIi_CpuClear32(0x200, (void *) oam, func_0202adf0(layer) * 8);
    data_020c267c.mUnk_018[sub == 0].mUnk_57a = 0;
}

// Fills `out` from texture `sub` of block `index`; an empty block gives a zeroed record with mUnk_12 = -1.
// The block header word holds four offsets/counts; the records follow the header's tables.
void func_0202ae84(u32 index, TexInfo *out, u32 sub) {
    u32 *hdr = func_0202adc0(index);
    u32 w = hdr[0];
    u32 b0 = w & 0xff;
    u32 b1 = (w >> 8) & 0xff;
    int b2 = (w >> 16) & 0xff;
    u32 count = (w >> 24) & 0xff;
    TexRec *e;

    if (count == 0) {
        memset(out, 0, 0x18);
        out->mUnk_12 = -1;
        return;
    }
    if (!(sub < count)) {
        OS_Terminate();
    }
    e = (TexRec *) (hdr + (b0 + 1 + b1 * 2 + b2 / 2));
    out->mUnk_04 = (u32) ((u32 *) &e[sub] - b2 / 2);
    out->mUnk_10 = count;
    out->mUnk_08 = e[sub].mUnk_00_0;
    out->mUnk_0a = e[sub].mUnk_00_9;
    out->mUnk_12 = e[sub].mUnk_00_18;
    out->mUnk_11 = e[sub].mUnk_00_21;
    out->mUnk_13 = e[sub].mUnk_00_25;
    out->mUnk_00 = e[sub].mUnk_04 & 0xffffff;
    out->mUnk_14 = (e[sub].mUnk_04 << 4) >> 28;
    out->mUnk_15 = (e[sub].mUnk_04 >> 28) + 1;
    out->mUnk_0c = data_02050998[out->mUnk_11][0];
    out->mUnk_0e = data_02050998[out->mUnk_11][1];
}

// Frees the block at `index` of a layer's block heap: merges it with a free neighbour on each side and
// links it into the free list, which is ordered by size.
u16 func_0202afb0(u32 index, int layer) {
    Unk_020c2694 *half = &data_020c267c.mUnk_018[layer == 2];
    Unk_PoolRange *n = half->mUnk_00c;
    u32 next;
    u32 prev;
    u32 size;

    n[index].mUnk_0 &= ~0x8000;
    next = index + n[index].mUnk_0;
    if (next < half->mUnk_578 && (n[next].mUnk_0 & 0x8000) == 0) {
        n[index].mUnk_0 += n[next].mUnk_0;
        n[index + n[index].mUnk_0].mUnk_6 = index;
        n[n[next].mUnk_4].mUnk_2 = n[next].mUnk_2;
        n[n[next].mUnk_2].mUnk_4 = n[next].mUnk_4;
    }
    prev = n[index].mUnk_6;
    if (prev < half->mUnk_578 && (n[prev].mUnk_0 & 0x8000) == 0) {
        n[prev].mUnk_0 += n[index].mUnk_0;
        index = prev;
        n[index + n[index].mUnk_0].mUnk_6 = index;
        n[n[prev].mUnk_4].mUnk_2 = n[prev].mUnk_2;
        n[n[prev].mUnk_2].mUnk_4 = n[prev].mUnk_4;
    }
    size = n[index].mUnk_0;
    if (size < 4) {
        prev = half->mUnk_578 + 1;
        next = n[prev].mUnk_2;
        while (size > n[next].mUnk_0) {
            prev = next;
            next = n[next].mUnk_2;
        }
        n[index].mUnk_2 = next;
        n[index].mUnk_4 = prev;
        n[prev].mUnk_2 = index;
        n[next].mUnk_4 = index;
    } else {
        u32 q = half->mUnk_578;
        u32 p = n[q].mUnk_4;
        while (size < n[p].mUnk_0) {
            q = p;
            p = n[p].mUnk_4;
        }
        n[index].mUnk_4 = p;
        n[index].mUnk_2 = q;
        n[q].mUnk_4 = index;
        n[p].mUnk_2 = index;
    }
    return n[index].mUnk_0;
}

// Removes `key` from a layer's hash table of cached blocks. The search starts at the key's low 10 bits and steps by
// an odd prime chosen from the next 7 bits; every entry passed on the way loses one from its path count, and an
// entry that has no path left and was marked removed (-2) becomes free again (-1). The key's own entry becomes
// free if no path goes through it, removed otherwise.
void func_0202b19c(int layer, u32 key) {
    Unk_020c2694 *half = &data_020c267c.mUnk_018[layer == 2];
    u32 idx = key & 0x3ff;
    u32 step = data_02050378[(key >> 10) & 0x7f];
    Unk_HashEntry *e;

    for (;;) {
        e = &half->mUnk_008[idx];
        if (key == e->mUnk_8) {
            e->mUnk_8 = e->mUnk_e == 0 ? -1 : -2;
            return;
        }
        if (e->mUnk_e == 0) {
            OS_Terminate();
        }
        half->mUnk_008[idx].mUnk_e--;
        e = &half->mUnk_008[idx];
        if (e->mUnk_e == 0 && e->mUnk_8 == -2) {
            e->mUnk_8 = -1;
        }
        idx = (idx + step) & 0x3ff;
    }
}

// Releases the block of entry `index` of a layer's cache list: drops its hash entry, frees its block heap node and
// moves the entry to the head of the free chain after the list's sentinel (entry 1). Returns the size
// func_0202afb0 reports.
u16 func_0202b26c(u32 index, int layer) {
    Unk_020c2694 *half = &data_020c267c.mUnk_018[layer == 2];
    s32 key = half->mUnk_008[half->mUnk_004[index].mUnk_0].mUnk_8;
    u16 size;

    if (key < 0) {
        OS_Terminate();
    }
    half->mUnk_574--;
    func_0202b19c(layer, key);
    size = func_0202afb0(half->mUnk_004[index].mUnk_6, layer);
    half->mUnk_004[1].mUnk_4 = half->mUnk_004[index].mUnk_4;
    half->mUnk_004[half->mUnk_004[index].mUnk_4].mUnk_2 = 1;
    half->mUnk_004[index].mUnk_2 = half->mUnk_004[1].mUnk_2;
    half->mUnk_004[1].mUnk_2 = index;
    return size;
}

// Size in bytes for size code `index`: data_02050978 entry times 32, doubled for mode 0 and modes 5 to 7.
u32 func_0202b32c(u32 index, int mode) {
    u32 v = data_02050978[index] << 5;

    if (mode == 0 || (u32) (mode - 5) <= 2) {
        v <<= 1;
    }
    return v;
}

// Resets a layer's tables: the block list (entries 0 and 1 are the ends, the rest chained as free), the hash
// table (all keys unused), the block heap nodes (one free block between two sentinels), the slot pairs and, when
// the layer has a chain table (mUnk_57b == 1), that table's links.
void func_0202bb30(int layer) {
    Unk_020c2694 *half = &data_020c267c.mUnk_018[layer == 2];
    int i;
    Unk_PoolRange *n;
    u32 m;

    func_0202ae34(layer);
    half->mUnk_004[0].mUnk_0 = 0x55aa;
    half->mUnk_004[0].mUnk_6 = 0x55aa;
    half->mUnk_004[1].mUnk_0 = 0x55aa;
    half->mUnk_004[1].mUnk_6 = 0x55aa;
    half->mUnk_004[0].mUnk_2 = 1;
    half->mUnk_004[1].mUnk_4 = 0;
    half->mUnk_004[1].mUnk_2 = 2;
    half->mUnk_574 = 0;
    for (i = half->mUnk_004[1].mUnk_2; i < 0x302; i++) {
        half->mUnk_004[i].mUnk_2 = i + 1;
        half->mUnk_004[i].mUnk_0 = 0xffff;
    }
    for (i = 0; i < 0x400; i++) {
        half->mUnk_008[i].mUnk_8 = -1;
        half->mUnk_008[i].mUnk_e = 0;
    }
    if (data_020c18bc.mUnk_46c & layer) {
        func_0201c4f0(&half->mUnk_00c);
    }
    half->mUnk_00c = func_0201c430((half->mUnk_578 + 2) << 3);
    m = half->mUnk_578;
    n = half->mUnk_00c;
    n[0].mUnk_0 = m;
    half->mUnk_00c[0].mUnk_6 = 0xffff;
    half->mUnk_00c[0].mUnk_2 = half->mUnk_578;
    half->mUnk_00c[0].mUnk_4 = half->mUnk_578 + 1;
    half->mUnk_00c[half->mUnk_578].mUnk_0 = 0xffff;
    half->mUnk_00c[half->mUnk_578].mUnk_4 = 0;
    half->mUnk_00c[half->mUnk_578].mUnk_2 = 0xffff;
    half->mUnk_00c[half->mUnk_578 + 1].mUnk_0 = 0;
    half->mUnk_00c[half->mUnk_578 + 1].mUnk_4 = 0xffff;
    half->mUnk_00c[half->mUnk_578 + 1].mUnk_2 = 0;
    for (i = 0; i < 0x10; i++) {
        half->mUnk_534[i].mUnk_0 = 0xffff;
        half->mUnk_534[i].mUnk_2 = i;
    }
    half->mUnk_114 = 1;
    half->mUnk_516 = 1;
    data_020c18bc.mUnk_250.mUnk_28[0] = 0;
    half->mUnk_57c = 0;
    if (half->mUnk_57b == 1) {
        for (i = 0; i < 0x100; i++) {
            half->mUnk_518[i].mUnk_0 = 0xfffe;
            half->mUnk_518[i].mUnk_6 = i;
            half->mUnk_518[i].mUnk_2 = 0xff;
            half->mUnk_518[i].mUnk_3 = 0xff;
            half->mUnk_518[i].mUnk_4 = i - 1;
            half->mUnk_518[i].mUnk_5 = i + 1;
        }
        for (i = 0; i < 0x10; i++) {
            half->mUnk_51c[i] = 0x10;
        }
        half->mUnk_51c[15] = 0xf;
    }
}

// Sets up a layer: checks the engine number `e` (1 = main, 2 = sub) and the mode `d` (low 6 bits; bit 7 selects
// the chain tables), programs the engine's display control register and VRAM banks, allocates the layer's tables
// unless the layer is already set up, reads the header of packed file `b` unless file `b`/`c` is already loaded
// ("PACK" files keep their table in memory, "EZ01" files are read on demand), then resets the layer.
void func_0202bdd8(int a, int b, int c, u32 d, int e) {
    u32 sub = d >> 7;
    Unk_020c2694 *half = &data_020c267c.mUnk_018[e == 2];
    u32 mask = e == 1 ? 1 : 2;
    vu32 *reg = e == 1 ? (vu32 *) 0x4000000 : (vu32 *) 0x4001000;
    int size;
    u32 v;
    u32 hdr[2];

    d &= 0x3f;
    if (!(e == 1 || d == 0 || d == 2)) {
        OS_Terminate();
    }
    if (!(e == 2 || d <= 3)) {
        OS_Terminate();
    }
    if (!(e == 1 || e == 2)) {
        OS_Terminate();
    }
    data_020c18bc.mUnk_46c &= ~0xc0000;
    half->mUnk_578 = (e == 2 && d == 0) ? 0x200 : 0x400;
    if (e & 1) {
        v = *(vu32 *) 0x4000000;
        v = (v & 0x7f1fefcf) | (0x1030 | (sub << 31));
        *(vu32 *) 0x4000000 = v;
        half->mUnk_57b = sub;
    }
    if (e & 2) {
        v = *(vu32 *) 0x4001000;
        v = (v & 0x7f3fefef) | (0x1010 | (sub << 31));
        *(vu32 *) 0x4001000 = v;
        half->mUnk_57b = sub;
    }
    if (!(data_020c18bc.mUnk_46c & mask)) {
        half->mUnk_577 = d;
        size = 0x20 << d;
        v = *reg;
        v = (v & ~0x300000) | (d << 20);
        *reg = v;
        if (sub == 1) {
            func_0201ebf4(e == 1 ? 5 : 9, 0x10, -1);
        }
        if (half->mUnk_578 < 0x400) {
            size >>= 1;
        }
        func_0201ebf4(e == 1 ? 3 : 7, size, -1);
        func_0201c4f0(half);
        func_0201c4f0(&half->mUnk_530);
        func_0201c4f0(&half->mUnk_010);
        func_0201c4f0(&half->mUnk_008);
        func_0201c4f0(&half->mUnk_004);
        half->mUnk_000 = func_0201c430(0x400);
        half->mUnk_530 = func_0201c430(0x80);
        half->mUnk_57a = 0;
        half->mUnk_010 = func_0201c430(0x100);
        half->mUnk_576 = 0;
        half->mUnk_008 = func_0201c430(0x4000);
        half->mUnk_004 = func_0201c430(0x1810);
    }
    if (!(data_020c267c.mUnk_b26 == b && data_020c267c.mUnk_b24 == c && data_020c267c.mUnk_b20 != 0)) {
        if (!(data_020c267c.mUnk_b26 == b && data_020c267c.mUnk_b24 == c)) {
            data_020c267c.mUnk_014 = func_0202e760(c) >> 5;
            if (data_020c267c.mUnk_000 != 0) {
                func_0201c4f0(&data_020c267c.mUnk_004);
                data_020c267c.mUnk_000 = 0;
            }
            data_020c267c.mUnk_016 = 0;
            data_020c267c.mUnk_012 = data_020c267c.mUnk_014;
        }
        func_0202e4fc(b, hdr, 8, 0);
        if (hdr[0] == 0x4b434150) {
            data_020c267c.mUnk_b2e = 1;
            data_020c267c.mUnk_b1c = func_0201c430(hdr[1] - 4);
            func_0202e4fc(b, data_020c267c.mUnk_b1c, hdr[1] - 4, 4);
        } else if (hdr[0] == 0x30315a45) {
            data_020c267c.mUnk_b2e = 2;
            data_020c267c.mUnk_b1c = 0;
        } else {
            data_020c267c.mUnk_b2e = 0;
            data_020c267c.mUnk_b1c = 0;
        }
        func_0201c4f0(&data_020c267c.mUnk_b20);
        data_020c267c.mUnk_b20 = (u32 *) func_0202e86c(a);
        data_020c267c.mUnk_b24 = c;
        data_020c267c.mUnk_b26 = b;
        data_020c267c.mUnk_b2a = (data_020c267c.mUnk_b20[4] - 0x10) >> 2;
    }
    func_0201c4f0(&half->mUnk_52c);
    func_0201c4f0(&half->mUnk_518);
    if (sub == 1) {
        half->mUnk_518 = func_0201c430(0x800);
        half->mUnk_52c = func_0201c430(data_020c267c.mUnk_014 + 0x200);
        memset(half->mUnk_52c, 0xff, data_020c267c.mUnk_014 + 0x200);
    }
    data_020c18bc.mUnk_46c |= mask;
    func_0202bb30(e);
}

// Frees the tables of all set-up layers, leaving the loaded file's table.
void func_0202c1c4(void) {
    func_0202c1d4(0);
}

// Frees the tables of the layers in use and their VRAM banks; with a == 1 also frees the loaded file's table.
void func_0202c1d4(int a) {
    u32 i;

    if (!(data_020c18bc.mUnk_46c & 3)) {
        return;
    }
    for (i = 0; i < 2; i++) {
        if (data_020c18bc.mUnk_46c & (i == 0 ? 1 : 2)) {
            func_0201c4f0(&data_020c267c.mUnk_b1c);
            func_0201c4f0(&data_020c267c.mUnk_018[i].mUnk_000);
            func_0201c4f0(&data_020c267c.mUnk_018[i].mUnk_530);
            func_0201c4f0(&data_020c267c.mUnk_018[i].mUnk_008);
            func_0201c4f0(&data_020c267c.mUnk_018[i].mUnk_004);
            func_0201c4f0(&data_020c267c.mUnk_018[i].mUnk_00c);
            func_0201c4f0(&data_020c267c.mUnk_018[i].mUnk_010);
            func_0201c4f0(&data_020c267c.mUnk_018[i].mUnk_518);
            func_0201eb74(i == 0 ? 3 : 7);
        }
    }
    if (a == 1) {
        func_0201c4f0(&data_020c267c.mUnk_b20);
    }
    data_020c18bc.mUnk_46c &= ~3;
}

#pragma thumb on
