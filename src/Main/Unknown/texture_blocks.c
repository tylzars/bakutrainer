#include <Main/clib.h>
#include <Main/Unknown/globals.h>
#include <Main/Unknown/system_init.h>
#include <Main/Unknown/bg_image.h>
#include <Main/Unknown/functions.h>
#include <Main/Unknown/vram_cache.h>
#include <Main/Unknown/model.h>
#include <nitro/fx.h>
#include <nitro/mi.h>
#include <nitro/os.h>
#include <Main/Unknown/engine_asm.h>
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

// Rearranges `w` x `h` tiles from sequential order into rows of a bitmap `w` tiles wide: for each tile, `mode` 1
// copies eight words (4 bits per pixel), any other mode eight word pairs (8 bits per pixel). Terminates when the
// source and destination are the same.
void func_0202c2f0(void *dst, const void *src, u32 w, u32 h, BOOL mode) {
    u32 *d = dst;
    const u32 *s = src;
    u32 n = 0;
    u32 y;
    u32 x;
    u32 k;
    u32 base = 0;

    if (dst == src) {
        OS_Terminate();
    }
    for (y = 0; y < h; y++) {
        for (x = 0; x < w; x++) {
            if (mode == 1) {
                u32 *p = d + (base * 8 + x);
                for (k = 0; k < 8; k++) {
                    *p = s[n++];
                    p += w;
                }
            } else {
                u32 *p = d + (base * 8 + x) * 2;
                for (k = 0; k < 8; k++) {
                    p[0] = s[n];
                    p[1] = s[n + 1];
                    n += 2;
                    p += w * 2;
                }
            }
        }
        base += w;
    }
}

// Expands the tiles of `src` (`w` tiles wide, 4 or 8 bits per pixel by the entry's byte 13) into 16-bit colours
// at the entry's destination: every non-zero pixel is looked up in the palette memory of the engine the
// destination belongs to and written with bit 15 set; the rows of the destination are func_0201db94(engine)
// halfwords apart. Sets the entry's palette byte to 0xff. In the 8-bit branch the test uses the low four bits of
// the pixel but the palette index is the whole byte.
void func_0202c3e0(Unk_LoadEntry *entry, const void *src, u32 w) {
    u32 dst = entry->mUnk_04;
    u32 size = entry->mUnk_08;
    int engine = (dst >> 20) == 0x60 ? 1 : 2;
    int stride = func_0201db94(engine);
    const u32 *s = src;
    u32 j;
    int k;

    if (entry->mUnk_13 == 0) {
        const u16 *pal = (const u16 *) ((engine == 1 ? 0x5000200 : 0x5000600) + (entry->mUnk_12 >> 4) * 32);
        if (size != 0) {
            do {
                u32 row = dst;
                for (j = 0; j < w; j++) {
                    u32 word = *s++;
                    for (k = 0; k < 8; k++) {
                        if ((word & 0xf) != 0) {
                            *(u16 *) dst = pal[word & 0xf] + 0x8000;
                        }
                        dst += 2;
                        word >>= 4;
                    }
                }
                dst = row + stride * 2;
                size -= w * 4;
            } while (size != 0);
        }
    } else {
        const u16 *pal = (const u16 *) (engine == 1 ? 0x5000200 : 0x5000600);
        if (size != 0) {
            do {
                u32 row = dst;
                for (j = 0; j < w * 2; j++) {
                    const u8 *p = (const u8 *) s++;
                    for (k = 0; k < 4; k++) {
                        u32 b = *p++;
                        if ((b & 0xf) != 0) {
                            *(u16 *) dst = pal[b] + 0x8000;
                        }
                        dst += 2;
                    }
                }
                dst = row + stride * 2;
                size -= w * 8;
            } while (size != 0);
        }
    }
    entry->mUnk_12 = 0xff;
}

// Moves entry `i` of the half's chain table to the end of the list threaded through bytes 4 (previous) and 5
// (next); entry 255 is the list's anchor.
void func_0202c548(u32 i, Unk_020c2694 *half) {
    half->mUnk_518[half->mUnk_518[i].mUnk_4].mUnk_5 = half->mUnk_518[i].mUnk_5;
    half->mUnk_518[half->mUnk_518[i].mUnk_5].mUnk_4 = half->mUnk_518[i].mUnk_4;
    half->mUnk_518[i].mUnk_4 = half->mUnk_518[255].mUnk_4;
    half->mUnk_518[i].mUnk_5 = 0xff;
    half->mUnk_518[255].mUnk_4 = (u8) i;
    half->mUnk_518[half->mUnk_518[i].mUnk_4].mUnk_5 = (u8) i;
}

// Looks for the entry of block `key` whose slot (high nibble of byte 6) is `val`: the chain starts at
// half->mUnk_52c[key] and continues through byte 3 of each entry. Returns 0xff when there is none.
u32 func_0202c5c0(u32 key, u32 val, Unk_020c2694 *half) {
    u32 n = half->mUnk_52c[key];

    if (n == 0xff) {
        return 0xff;
    }
    do {
        if (val == half->mUnk_518[n].mUnk_6 >> 4) {
            return n;
        }
        n = half->mUnk_518[n].mUnk_3;
    } while (n != 0xff);
    return 0xff;
}

// Takes the oldest entry of slot `val` from the list (func_0202c548), loads block `key` into the 32 bytes that
// entry names (from the file when `key` is below the file's block count, otherwise from the preloaded block at
// data_020c267c.mUnk_008 + index * 32), chains the entry at the end of key `key`'s chain and returns its number.
u32 func_0202c5fc(u32 index, u32 key, u32 val, Unk_020c2694 *half) {
    u32 n = half->mUnk_518[255].mUnk_5;
    u32 sel;
    Unk_ChainEntry *t;
    u32 prev;
    u32 next;

    while (val != half->mUnk_518[n].mUnk_6 >> 4) {
        n = half->mUnk_518[n].mUnk_5;
    }
    half->mUnk_51c[val]--;
    func_0202c548(n, half);
    sel = (&data_020c267c.mUnk_018[1] == half) ? 9 : 5;
    if (key < data_020c267c.mUnk_014) {
        func_0201c6b0(data_020c267c.mUnk_b24, key << 5, (half->mUnk_518[n].mUnk_6 << 5) + (sel << 28), 0x20);
    } else {
        func_0201c6b0(0, (u32) (data_020c267c.mUnk_008 + (index << 5)), (half->mUnk_518[n].mUnk_6 << 5) + (sel << 28),
                      0x20);
    }
    prev = half->mUnk_52c[key];
    if (prev == 0xff) {
        half->mUnk_52c[key] = n;
        half->mUnk_518[n].mUnk_0 = key;
        half->mUnk_518[n].mUnk_2 = 0xff;
        half->mUnk_518[n].mUnk_3 = 0xff;
    } else {
        t = half->mUnk_518;
        next = t[prev].mUnk_3;
        while (next != 0xff) {
            prev = next;
            next = t[next].mUnk_3;
        }
        t[prev].mUnk_3 = n;
        half->mUnk_518[n].mUnk_2 = prev;
        half->mUnk_518[n].mUnk_0 = 0xffff;
    }
    return half->mUnk_518[n].mUnk_6;
}

// Makes the `n` consecutive blocks starting at keys[0] (or the `n` keys listed there when keys[0] is below the
// file's block count) resident in one slot: a slot already holding all of them is used if there is one, otherwise
// the slot that needs the fewest loads and has room, evicting the oldest entries until one has. Each key is then
// looked up or loaded (func_0202c5c0, func_0202c5fc) and its entry's low nibble appended to the half's index list
// (mUnk_115, count in mUnk_516). Returns the slot. `a` is not used.
s32 func_0202c778(u16 *keys, int a, u32 n, int engine) {
    Unk_020c2694 *half = &data_020c267c.mUnk_018[engine == 2];
    u32 node2;
    u8 count[16];
    int min;
    u16 list[16];
    u32 mask = 0xffff;
    int s;
    u32 k;
    u32 j;
    u32 idx;
    s32 slot;
    u32 node;
    u32 i;
    u32 m;
    u32 avail;
    u32 key;

    if (keys[0] < data_020c267c.mUnk_014) {
        memcpy(list, keys, n * 2);
    } else {
        for (j = 0; j < n; j++) {
            list[j] = keys[0] + j;
        }
    }
    for (i = 0; i < n; i++) {
        key = list[i];
        node = half->mUnk_52c[key];
        avail = 0;
        while (node != 0xff) {
            avail |= 1 << (half->mUnk_518[node].mUnk_6 >> 4);
            node = half->mUnk_518[node].mUnk_3;
        }
        mask &= avail;
    }
    if (mask != 0) {
        slot = 31 - func_02028250(mask);
    } else {
        MI_CpuClear32(count, 16);
        for (m = 0; m < n; m++) {
            node = half->mUnk_52c[list[m]];
            while (node != 0xff) {
                count[half->mUnk_518[node].mUnk_6 >> 4]++;
                node = half->mUnk_518[node].mUnk_3;
            }
        }
        for (;;) {
            slot = -1;
            min = 32;
            for (s = 0; s < 16; s++) {
                int need = n - count[s];
                if (need <= half->mUnk_51c[s]) {
                    if (need < min) {
                        min = need;
                        slot = s;
                    }
                }
            }
            if (slot != -1) {
                break;
            }
            node = half->mUnk_518[255].mUnk_5;
            while (half->mUnk_518[node].mUnk_0 == 0xfffe) {
                node = half->mUnk_518[node].mUnk_5;
            }
            if (half->mUnk_518[node].mUnk_0 != 0xffff) {
                half->mUnk_52c[half->mUnk_518[node].mUnk_0] = half->mUnk_518[node].mUnk_3;
            }
            half->mUnk_518[node].mUnk_2 = 0xff;
            half->mUnk_518[node].mUnk_3 = 0xff;
            half->mUnk_518[node].mUnk_0 = 0xfffe;
            half->mUnk_51c[half->mUnk_518[node].mUnk_6 >> 4]++;
        }
    }
    for (k = 0; k < n; k++) {
        node2 = func_0202c5c0(list[k], slot, half);
        if (node2 == 0xff) {
            node2 = func_0202c5fc(k, list[k], slot, half);
        }
        if (half->mUnk_516 >= 0x400) {
            OS_Terminate();
        }
        idx = half->mUnk_516;
        half->mUnk_516++;
        half->mUnk_115[idx] = half->mUnk_518[node2].mUnk_6 & 0xf;
    }
    return slot;
}

#define VRAM_PLTT_BG 0x5000200

// Hardware OAM entry (8 bytes), and a rotation/scale matrix as stored in a half's matrix list.
typedef struct Oam {
    u16 mUnk_0;
    u16 mUnk_2;
    u16 mUnk_4;
    u16 mUnk_6;
} Oam;

typedef struct Mat4 {
    s16 mUnk_0;
    s16 mUnk_2;
    s16 mUnk_4;
    s16 mUnk_6;
} Mat4;

// Slot list of a half: the keys of the palettes in VRAM, most recently used first. For count keys (a key below
// the preloaded block count comes from the list at ids, which advances; any other key is the first key plus its
// position and is read from the block data) it finds or loads each palette and returns the last slot. With flag
// 0 the slots are also appended to the half's record, and a record that repeats the run before it is removed again.
s32 func_0202ca80(u16 *ids, int flag, u32 count, int mode) {
    Unk_020c2694 *half = &data_020c267c.mUnk_018[mode == 2];
    int base = half->mUnk_516;
    int s;
    u32 i;
    s32 ret;

    if (flag == 0 && half->mUnk_57b == 1) {
        ret = func_0202c778(ids, flag, count, mode);
    } else {
        for (i = 0; i < count; i++) {
            u32 key;
            u32 miss;

            key = *ids;
            if (key < data_020c267c.mUnk_014) {
                ids++;
            } else {
                key = (u16) (key + i);
            }
            miss = 1;
            for (s = 0; s < 16; s++) {
                u32 k = half->mUnk_534[s].mUnk_0;
                if (key == k) {
                    miss = 0;
                    break;
                }
                if (k == 0xffff) {
                    break;
                }
            }
            if (s >= count) {
                u32 keep;
                u32 *p;
                if (s >= 15) {
                    s = 15;
                }
                keep = half->mUnk_534[s].mUnk_2 & 0xff;
                for (p = (u32 *) &half->mUnk_534[s]; s > 0; s--) {
                    *p = p[-1];
                    p--;
                }
                half->mUnk_534[0].mUnk_2 = keep;
                s = 0;
            }
            half->mUnk_534[s].mUnk_0 = key;
            if (miss == 1) {
                u32 pal = (mode == 2) ? 0x400 : 0;
                if (key < data_020c267c.mUnk_014) {
                    func_0201c6b0(data_020c267c.mUnk_b24, key * 32, VRAM_PLTT_BG + pal + half->mUnk_534[s].mUnk_2 * 32,
                                  0x20);
                } else {
                    func_0201c6b0(0, (u32) data_020c267c.mUnk_008 + i * 32,
                                  VRAM_PLTT_BG + pal + half->mUnk_534[s].mUnk_2 * 32, 0x20);
                }
            }
            ret = half->mUnk_534[s].mUnk_2;
            if (flag == 0) {
                if (half->mUnk_516 >= 0x400) {
                    OS_Terminate();
                }
                half->mUnk_115[half->mUnk_516++] = ret;
            }
        }
    }
    if (flag == 0) {
        int d = half->mUnk_516 - base;
        if (base >= d) {
            if (memcmp(&half->mUnk_115[base - d], &half->mUnk_115[base], d) == 0) {
                half->mUnk_516 = base;
            }
        }
    }
    return ret;
}

// Fills out (8 bytes) for a sprite record: the point func_0202d7a0 gives for type 3 (or 0, 0), the record's two
// 9-bit values, the width and height of its rect and of its size code.
void func_0202cce0(u32 obj, u32 rec, u8 *out) {
    s32 pt[2];
    s32 rect[4];

    func_0202d828(obj, 0, rect);
    if (func_0202d7a0((u16) obj, 3, pt) == 1) {
        out[0] = pt[0];
        out[1] = pt[1];
    } else {
        out[1] = 0;
        out[0] = 0;
    }
    out[6] = rect[2] - rect[0] + 1;
    out[7] = rect[3] - rect[1] + 1;
    out[2] = rec & 0x1ff;
    out[3] = (rec << 14) >> 23;
    out[4] = data_02050998[(rec << 7) >> 28][0];
    out[5] = data_02050998[(rec << 7) >> 28][1];
}

// The record count (mUnk_576) of the half for a layer; layer 3 is invalid.
u8 func_0202cdac(int layer) {
    if (layer == 3) {
        OS_Terminate();
    }
    return data_020c267c.mUnk_018[(layer & 1) == 0].mUnk_576;
}

// Finds or appends the matrix for an angle and two scales (16.12 fixed point, as four halfwords) in the half's
// matrix list and returns its index.
u32 func_0202cde4(int angle, int sx, int sy, u32 flags) {
    Unk_020c2694 *half = &data_020c267c.mUnk_018[(flags & 2) != 0];
    int ix = 0x10000 / sx;
    int iy = ix;
    s32 idx;
    s16 m[4];
    u32 *p;
    u32 *w;
    u32 j;

    if (sx != sy) {
        iy = 0x10000 / sy;
    }
    idx = ((angle << 8) >> 4) << 1;
    m[0] = (ix * FX_SinCosTable_[idx + 1]) >> 12;
    m[1] = (ix * FX_SinCosTable_[idx]) >> 12;
    m[2] = (iy * -FX_SinCosTable_[idx]) >> 12;
    m[3] = (iy * FX_SinCosTable_[idx + 1]) >> 12;
    w = (u32 *) m;
    p = (u32 *) half->mUnk_010;
    for (j = 0; j < half->mUnk_576; j++) {
        if (p[0] == w[0] && p[1] == w[1]) {
            return j;
        }
        p += 2;
    }
    p[0] = w[0];
    p[1] = w[1];
    return ++half->mUnk_576 - 1;
}

// Draws sprite id at x, y with width w on a layer, unrotated and at full scale (0x100).
void func_0202cf08(int id, int x, int y, int w, int mode) {
    func_0202cf38(id, x, y, w, 0, 0x100, 0x100, mode);
}

// Draws sprite `id` of a layer (mode 1 or 2) at x, y. The low 16 bits of id select the block; bits 16 and 17 flip
// it horizontally and vertically, bit 18 sets the semi-transparent mode, bit 19 the double size of a rotated
// sprite, bit 20 copies the tiles straight into the layer's background VRAM instead, bit 21 draws nothing and keeps
// the tile and palette of the first record it reaches in mUnk_b28 and mUnk_b2c, bit 22 sets attribute 2 bit 11,
// and bit 23 hands the sprite to func_020243c0 with the alpha in bits 27-31 (0 = 31). w >> 8 is the priority and
// its low byte the sort key of each entry. angle and the scales sx, sy (0x100 = 1) give the sprite a rotation/scale
// matrix.
void func_0202cf38(int id, int x, int y, int w, int angle, int sx, int sy, int mode) {
    u32 bit;
    u32 flags;
    u32 aff;
    u32 w0;
    u16 *ids;
    u32 b0;
    u32 b1;
    int b2;
    u8 count;
    u32 *p;
    u16 key;
    int dx;
    int dy;
    int dbl;
    u32 n;
    int prio;
    s8 rect[8];
    Unk_020c2694 *half;

    key = id;
    flags = id & 0xffff0000;
    bit = mode == 1 ? 1 : 2;
    half = &data_020c267c.mUnk_018[mode == 2];
    aff = -1;
    (void) ((data_020c18bc.mUnk_46c & bit) || (id & 0x800000) || (OS_Terminate(), 0));
    if (id == -1 || key == 0xffff) {
        return;
    }
    data_020c267c.mUnk_b28 = -1;
    data_020c267c.mUnk_b2c = -1;
    if (id & 0x800000) {
        u32 alpha = (u32) id >> 27;

        (void) (mode == 1 || (OS_Terminate(), 0));
        if (alpha == 0) {
            alpha = 0x1f;
        }
        func_020243c0(id | flags, x, y, -w << 6, -angle << 8, sx << 4, sy << 4, alpha);
        return;
    }
    p = func_0202adc0(key);
    w0 = *p;
    b0 = w0 & 0xff;
    b1 = (w0 >> 8) & 0xff;
    b2 = (w0 >> 16) & 0xff;
    count = (w0 >> 24) & 0xff;
    (void) (b1 != 0 || (OS_Terminate(), 0));
    p += b0 + 1 + b1 * 2 + b2 / 2;
    ids = (u16 *) (p - b2 / 2);
    if (data_020c267c.mUnk_000 == 1 && data_020c267c.mUnk_010 == key) {
        ids = &data_020c267c.mUnk_012;
    }
    if (sx != 0x100 || sy != 0x100 || angle != 0) {
        int sxv;
        int syv;
        int cx;
        int cy;

        sxv = sx;
        syv = sy;
        (void) ((sx >= 0 && sy >= 0) || (OS_Terminate(), 0));
        if (flags & 0x10000) {
            angle = (u8) -angle;
            sxv = -sx;
            flags &= ~0x10000;
        }
        if (flags & 0x20000) {
            angle = (u8) -angle;
            syv = -sy;
            flags &= ~0x20000;
        }
        func_0202cce0(key, *p, (u8 *) rect);
        dbl = ((flags & 0x80000) && (angle != 0 || sx > 0x100 || sy > 0x100)) ? 1 : 0;
        aff = func_0202cde4(angle, sxv, syv, mode);
        // Offset from the first record's centre to the object's point, scaled and turned by the matrix.
        cx = (-(rect[2] + (rect[4] / 2 - rect[0])) * ((s16) sx * (s16) sx)) >> 8;
        cy = (-(rect[3] + (rect[5] / 2 - rect[1])) * ((s16) sy * (s16) sy)) >> 8;
        cx = sxv < 0 ? -cx : cx;
        cy = syv < 0 ? -cy : cy;
        dx = (cx * -((Mat4 *) half->mUnk_010)[aff].mUnk_0 + cy * ((Mat4 *) half->mUnk_010)[aff].mUnk_2) >> 16;
        dy = (cx * ((Mat4 *) half->mUnk_010)[aff].mUnk_4 - cy * ((Mat4 *) half->mUnk_010)[aff].mUnk_6) >> 16;
        dx = sxv < 0 ? -dx : dx;
        dy = syv < 0 ? -dy : dy;
        dx = dx - (rect[4] >> (1 - dbl)) + rect[0] * sxv / 256 - rect[2];
        dy = dy - (rect[5] >> (1 - dbl)) + rect[1] * syv / 256 - rect[3];
    }
    (void) (w < 0x400 || (OS_Terminate(), 0));
    n = half->mUnk_57a;
    prio = w >> 8;
    while (count-- != 0 && n < 0x80) {
        int px = ((TexRec *) p)->mUnk_00_0;
        int py = ((TexRec *) p)->mUnk_00_9;
        u32 shape = ((TexRec *) p)->mUnk_00_18;
        u32 size = ((TexRec *) p)->mUnk_00_21;
        u32 c = ((TexRec *) p)->mUnk_00_25;
        u32 w1 = ((TexRec *) p)->mUnk_04;
        u32 off = (w1 << 8) >> 8;
        u32 pi = (w1 << 4) >> 28;
        int cnt = shape == 1 ? 1 : (w1 >> 28) + 1;
        int wd;
        int ht;
        u32 pal;
        u16 tile;
        u16 a0;
        u16 a1;

        wd = data_02050998[size][0];
        ht = data_02050998[size][1];
        p += 2;
        if (flags & 0x10000) {
            px = -px - wd;
        }
        px += x;
        if (flags & 0x20000) {
            py = -py - ht;
        }
        py += y;
        if (aff == -1 && (px + wd < 0 || px >= 0x100 || py + ht < 0 || py >= 0xc0)) {
            continue;
        }
        pal = func_0202ca80(&ids[pi], shape, cnt, mode);
        if (flags & 0x100000) {
            u32 base;
            u32 dst;
            u32 src;
            u32 shift;
            u32 sz;
            u32 fl;
            u32 bits;

            (void) (func_0201db58(mode) == 0x8000 || (OS_Terminate(), 0));
            (void) (shape == 1 || half->mUnk_57b == 0 || (OS_Terminate(), 0));
            (void) ((flags & 0x30000) == 0 || (OS_Terminate(), 0));
            (void) (aff == -1 || (OS_Terminate(), 0));
            base = mode == 1 ? 0x6000000 : 0x6200000;
            dst = base + (py * func_0201db94(mode) + px) * 2;
            if (data_020c267c.mUnk_b2e == 2) {
                src = c + (off << 7);
            } else {
                (void) (data_020c267c.mUnk_b2e == 1 || (OS_Terminate(), 0));
                src = off << 5;
            }
            // shift = log2 of the width: 32 minus the leading zeros of the bits below its lowest set bit. An asm
            // statement: clz has no C expression.
            bits = data_02050998[size][0];
            bits = ~bits & (bits - 1);
            asm { clz bits, bits }
            shift = 32 - bits;
            sz = func_0202b32c(size, shape);
            if (shape == 1) {
                fl = 0;
            } else {
                fl = ((half->mUnk_516 - cnt) << 1) | (mode == 2);
            }
            func_0201c580(data_020c267c.mUnk_b26, src, dst, sz, 0, shift + (pal << 4), fl);
            continue;
        }
        tile = func_0202b35c(size, off, c, shape, cnt, mode);
        if (flags & 0x200000) {
            data_020c267c.mUnk_b28 = tile;
            data_020c267c.mUnk_b2c = pal;
            return;
        }
        if (aff == -1) {
            a0 = (u8) py | ((size >> 2) << 14) | ((shape == 0) << 13) | ((flags & 0x40000) >> 8);
            a1 = (px & 0x1ff) | ((size & 3) << 14) | ((flags & 0x30000) >> 4);
        } else {
            a0 = (u8) (py + dy) | ((size >> 2) << 14) | ((shape == 0) << 13) | 0x100 | (dbl << 9) |
                 ((flags & 0x40000) >> 8);
            a1 = ((px + dx) & 0x1ff) | ((size & 3) << 14) | (aff << 9);
        }
        ((Oam *) half->mUnk_000)[n].mUnk_0 = a0;
        ((Oam *) half->mUnk_000)[n].mUnk_2 = a1;
        ((Oam *) half->mUnk_000)[n].mUnk_4 = tile | (pal << 12) | (prio << 10) | ((flags & 0x400000) >> 11);
        ((u8 *) half->mUnk_530)[n] = w;
        n++;
    }
    half->mUnk_57a = n;
}

// Looks up the point of type index in the object's record and flips it for the flip flags in bits 16 and 17 of
// flags; returns whether it exists.
int func_0202d7a0(u32 flags, int index, s32 *out) {
    u32 *hdr = func_0202adc0(flags);
    u32 i;
    u32 count = hdr[0] & 0xff;
    u32 *rec = hdr + 1;

    for (i = 0; i < count; i++) {
        s32 w = *rec++;
        if (index == (w >> 18)) {
            out[0] = (w << 23) >> 23;
            out[1] = (w << 14) >> 23;
            if (flags & 0x10000) {
                out[0] = -out[0];
            }
            if (flags & 0x20000) {
                out[1] = -out[1];
            }
            return 1;
        }
    }
    return 0;
}

// Looks up the rect of type a in the object's record, flipped for bits 16 and 17 of obj; returns whether it exists.
int func_0202d828(int obj, int a, s32 *rect) {
    u32 *hdr = func_0202adc0((u16) obj);
    u32 i = 0;
    u32 w = hdr[0];
    u32 b0 = w & 0xff;
    u32 *rec = hdr + (b0 + 1);
    u32 count = (w >> 8) & 0xff;

    for (; i < count; i++) {
        s32 lo = rec[0];
        s32 hi = rec[1];
        rec += 2;
        if (((lo >> 18) & 0x1f) == a) {
            s32 t;
            rect[0] = (lo << 23) >> 23;
            rect[2] = (hi << 23) >> 23;
            if (obj & 0x10000) {
                t = rect[0];
                rect[0] = -rect[2];
                rect[2] = -t;
            }
            rect[1] = (lo << 14) >> 23;
            rect[3] = (hi << 14) >> 23;
            if (obj & 0x20000) {
                t = rect[1];
                rect[1] = -rect[3];
                rect[3] = -t;
            }
            return 1;
        }
    }
    return 0;
}

// Writes a layer's queued sprites into OAM ordered by their sort key, hides the rest, copies the matrices to the
// OAM parameter slots and empties the queues.
void func_0202d908(int layer) {
    int sub = (layer == 2);
    u32 bit = (layer == 1) ? 1 : 2;
    u32 other = (layer == 1) ? 0x40000 : 0x80000;
    Unk_020c2694 *half = &data_020c267c.mUnk_018[sub];
    Oam *oam;
    u16 *affine;
    int n;
    int i;
    u8 hist[256];

    if (bit != (data_020c18bc.mUnk_46c & (bit | other))) {
        return;
    }
    n = 0;
    oam = (layer == 1) ? (Oam *) 0x7000000 : (Oam *) 0x7000400;
    if (half->mUnk_57a != 0) {
        u16 *src = (u16 *) half->mUnk_000;

        MI_CpuClearFast(hist, 0x100);
        n = half->mUnk_57a;
        for (i = 0; i < n; i++) {
            hist[((u8 *) half->mUnk_530)[i]]++;
        }
        n = hist[0];
        for (i = 1; i < 256; i++) {
            n += hist[i];
            hist[i] = n;
        }
        for (i = 0; i < half->mUnk_57a; i++) {
            u8 key = ((u8 *) half->mUnk_530)[i];
            u8 v = --hist[key];

            ((u16 *) oam)[v * 4] = src[0];
            ((u16 *) oam)[v * 4 + 1] = src[1];
            ((u16 *) oam)[v * 4 + 2] = src[2];
            src += 4;
        }
    }
    i = func_0202adf0(layer);
    for (; n < i; n++) {
        oam[n].mUnk_0 = 0x200;
    }
    affine = (layer == 1) ? (u16 *) 0x7000006 : (u16 *) 0x7000406;
    for (i = 0; i < half->mUnk_576; i++) {
        affine[0] = ((Mat4 *) half->mUnk_010)[i].mUnk_0;
        affine[4] = ((Mat4 *) half->mUnk_010)[i].mUnk_2;
        affine[8] = ((Mat4 *) half->mUnk_010)[i].mUnk_4;
        affine[12] = ((Mat4 *) half->mUnk_010)[i].mUnk_6;
        affine += 16;
    }
    half->mUnk_57a = 0;
    half->mUnk_576 = 0;
    half->mUnk_114 = 1;
    half->mUnk_516 = 1;
}

#pragma thumb on
