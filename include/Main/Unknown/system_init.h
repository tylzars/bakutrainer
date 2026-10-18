#pragma once

#include "types.h"
#include <nitro/wm.h>

#ifdef __cplusplus
extern "C" {
#endif

void func_0201b7e0(u32 a);
void func_0201b7ec(u32 a, int b, int c);
void func_0201bb30(void);
void func_0201bb40(int flags);
void func_0201c1d0(void *arg);
void func_0201c1e0(void);
void func_0201c32c(void);
void func_0201c41c(void);
void *func_0201c430(u32 size);
void *func_0201c43c(u32 size, u32 flags);
int func_0201c4f0(void *p);
u16 func_0201c538(void);
u16 func_0201c544(void);
u32 func_0201c550(void);
u32 func_0201c564(void);

// Objects of the per-frame system code.

typedef struct Unk_PoolRange {
    u16 mUnk_0;
    u16 mUnk_2;
    u16 mUnk_4;
    u16 mUnk_6;
} Unk_PoolRange;

// A slot of the second table of a half: a link to its entry and a value.
typedef struct Unk_SlotPair {
    u16 mUnk_0;
    u16 mUnk_2;
} Unk_SlotPair;

// Entry of a half's hash table (16 bytes): the key at +8 (-1 = never used, -2 = removed) and the number of
// search paths that pass through the entry.
typedef struct Unk_HashEntry {
    u8 mUnk_0[8];
    s32 mUnk_8;
    u8 mUnk_c[2];
    u16 mUnk_e;
} Unk_HashEntry;

// Entry of the chain table of a half (8 bytes): a word and five link bytes.
typedef struct Unk_ChainEntry {
    u16 mUnk_0;
    u8 mUnk_2;
    u8 mUnk_3;
    u8 mUnk_4;
    u8 mUnk_5;
    u8 mUnk_6;
    u8 mUnk_7;
} Unk_ChainEntry;

// The two halves of the file cache block (0x580 bytes each): tables for one display engine's cached blocks.
typedef struct Unk_020c2694 {
    void *mUnk_000;
    Unk_PoolRange *mUnk_004;
    Unk_HashEntry *mUnk_008;
    Unk_PoolRange *mUnk_00c;
    void *mUnk_010;
    u16 mUnk_014[0x80];
    u8 mUnk_114;
    u8 mUnk_115[0x516 - 0x115];
    u16 mUnk_516;
    Unk_ChainEntry *mUnk_518;
    u8 mUnk_51c[0x10];
    u8 *mUnk_52c;
    void *mUnk_530;
    Unk_SlotPair mUnk_534[0x10];
    u16 mUnk_574;
    u8 mUnk_576;
    u8 mUnk_577;
    u16 mUnk_578;
    u8 mUnk_57a;
    u8 mUnk_57b;
    u8 mUnk_57c;
    u8 mUnk_57d[3];
} Unk_020c2694;

// File cache block: a preloaded file block (valid flag, its address and its offset), the two halves, the block
// offset table of the packed file, that file's number and its mode.
typedef struct Unk_020c267c {
    u32 mUnk_000;
    void *mUnk_004;
    u8 *mUnk_008;
    u32 mUnk_00c;
    u16 mUnk_010;
    u16 mUnk_012;
    u16 mUnk_014;
    u8 mUnk_016;
    u8 mUnk_017;
    Unk_020c2694 mUnk_018[2];
    u8 mUnk_b18[4];
    u32 *mUnk_b1c;
    u32 *mUnk_b20;
    u16 mUnk_b24;
    u16 mUnk_b26;
    s16 mUnk_b28;
    u16 mUnk_b2a;
    s8 mUnk_b2c;
    u8 mUnk_b2d;
    u8 mUnk_b2e;
} Unk_020c267c;
extern Unk_020c267c data_020c267c;

extern u16 data_020c9e04;
extern void *data_020c9e10;
extern u16 *data_020c9e14;
extern u8 *data_020c9e1c;
extern WMDataSharing data_020c9e60;
void func_0201c580(u32 file, u32 src, u32 dst, u32 size, u32 unpackedSize, u8 mode, u8 flag);
void func_0201c6b0(u16 file, u32 src, u32 dst, u32 size);
u32 func_0201c704(u32 size);
void func_0201c78c(void);

// Bank configuration records (12 bytes each): the bank, its size in KB, the use it is mapped for (0 = the bank's
// LCDC record, which leads each bank's run), its address, then the MST and OFS values of the control register.
typedef struct Unk_02050478 {
    u8 mUnk_0;
    u8 mUnk_1;
    u8 mUnk_2;
    u8 mUnk_3;
    u32 mUnk_4;
    u8 mUnk_8;
    u8 mUnk_9;
    u8 mUnk_a[2];
} Unk_02050478;

// Load statistics: bytes read and blocks unpacked by the transfer code.
typedef struct Unk_020c1834 {
    u8 mUnk_00[8];
    u32 mUnk_08;
    u32 mUnk_0c;
    u8 mUnk_10[0x30];
} Unk_020c1834;

typedef struct Unk_PoolNode {
    u32 mUnk_00;
    u32 mUnk_04;
    u16 mUnk_08;
    u16 mUnk_0a;
    u16 mUnk_0c;
    u8 mUnk_0e[2];
} Unk_PoolNode;

typedef struct Unk_PoolSlot {
    s32 mUnk_00;
    u16 mUnk_04;
    u16 mUnk_06;
} Unk_PoolSlot;

// An animation track: its data, the current and next frame, the blend between them and the mode.
typedef struct Unk_AnimTrack {
    void *mUnk_00;
    u32 mUnk_04;
    u32 mUnk_08;
    u8 mUnk_0c;
    u8 mUnk_0d;
    u8 mUnk_0e[2];
} Unk_AnimTrack;

// A node of a model's pose tree: mUnk_0 is its first child and mUnk_1 its next sibling (0xff = none); scale (s8),
// angles (s8, << 8), position (s16, << 5), its matrix stack slot (-1 = none) and flags.
typedef struct Unk_PoseNode {
    u8 mUnk_0;
    u8 mUnk_1;
    s8 mUnk_2;
    s8 mUnk_3;
    s8 mUnk_4;
    s8 mUnk_5;
    s8 mUnk_6;
    s8 mUnk_7;
    s16 mUnk_8;
    s16 mUnk_a;
    s16 mUnk_c;
    s8 mUnk_e;
    u8 mUnk_f;
} Unk_PoseNode;

// The graphics pools (texture blocks at 0x00, palette blocks at 0x0c: slots, hash entries and block ranges) and the
// model animation state.
typedef struct Unk_020c31dc {
    Unk_PoolNode *mUnk_00;
    Unk_PoolSlot *mUnk_04;
    Unk_PoolRange *mUnk_08;
    Unk_PoolNode *mUnk_0c;
    Unk_PoolSlot *mUnk_10;
    Unk_PoolRange *mUnk_14;
    Unk_AnimTrack mUnk_18[2];
    Unk_PoseNode *mUnk_38;
    u16 mUnk_3c;
    u16 mUnk_3e;
    s16 mUnk_40;
    u8 mUnk_42;
    u8 mUnk_43;
    u8 mUnk_44;
    u8 mUnk_45;
    u8 mUnk_46;
    u8 mUnk_47;
    u8 mUnk_48;
    u8 mUnk_49;
    u8 mUnk_4a;
    u8 mUnk_4b;
    u32 mUnk_4c;
    u32 mUnk_50;
    u16 mUnk_54;
    u16 mUnk_56;
    u16 mUnk_58;
    u16 mUnk_5a;
    u8 mUnk_5c[2];
    u16 mUnk_5e;
    u16 mUnk_60;
    u16 mUnk_62;
    u16 mUnk_64;
    u8 mUnk_66[2];
} Unk_020c31dc;

extern Unk_020c1834 data_020c1834;
extern Unk_020c31dc data_020c31dc;

// VRAM control register of each bank (word address and bit offset of its byte).
extern const u32 data_02050220[9];
extern const u8 data_020502ac[9];
extern const Unk_02050478 data_02050478[];
#ifdef __cplusplus
}
#endif
