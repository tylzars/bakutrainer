#include <Main/clib.h>
#include <Main/Unknown/globals.h>
#include <Main/Unknown/system_init.h>
#include <nitro/mi.h>
#include <nitro/os.h>
#include <Main/Unknown/texture_blocks.h>

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

// Byte 0xf of the 0x10-byte tail of the half for layer 1 (layer 2 selects the other half).
u8 func_0202ad78(int layer) {
    return data_020c267c.mUnk_018[layer != 1].mUnk_568[0xf];
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

#pragma thumb on
