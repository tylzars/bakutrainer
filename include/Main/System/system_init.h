#pragma once

#include "types.h"
#include <nitro/wm.h>
#include <nitro/fs.h>
#include <nitro/pm.h>
#include <nitro/mic.h>
#include <nitro/fx.h>

#ifdef __cplusplus
extern "C" {
#endif

// The system state block and its parts. Only the members the code touches are named.
// Block cleared together at start-up: 0x24 bytes and the object at member 0x24.
typedef struct Unk_020c1b0c {
    s32 mUnk_00;
    u32 mUnk_04;
    s32 mUnk_08;
    void *mUnk_0c;
    u8 mUnk_10[4];
    u32 mUnk_14[2];
    u32 mUnk_1c[2];
    u8 *mUnk_24;
    u8 mUnk_28[4];
} Unk_020c1b0c;

// One queued transfer (0x14 bytes): the source (a file offset, packed with a block index for some files, or an
// address when the file is 0), the destination, the stored and the unpacked size, the file and two format bytes.
typedef struct Unk_LoadEntry {
    u32 mUnk_00;
    u32 mUnk_04;
    u32 mUnk_08;
    u32 mUnk_0c;
    u16 mUnk_10;
    u8 mUnk_12;
    u8 mUnk_13;
} Unk_LoadEntry;

// VRAM banks mapped for one use: each bank's control register, its size in KB, its LCDC address and the control
// value to restore, then the number of banks.
typedef struct Unk_VramGroup {
    vu8 *mUnk_00[4];
    u16 mUnk_10[4];
    u32 mUnk_18[4];
    u8 mUnk_28[4];
    u16 mUnk_2c;
    u8 mUnk_2e;
    u8 mUnk_2f;
} Unk_VramGroup;

// One VRAM bank: its control register, its LCDC address and the control value to restore.
typedef struct Unk_VramBank {
    vu8 *mUnk_0;
    u32 mUnk_4;
    u8 mUnk_8;
    u8 mUnk_9[3];
} Unk_VramBank;

typedef struct SystemState {
    void *mUnk_000;
    u8 mUnk_004[0xc];
    u16 mUnk_10;
    u16 mUnk_12;
    u16 mUnk_014;
    u16 mUnk_016;
    u16 mUnk_018;
    u16 mUnk_01a;
    u16 mUnk_01c;
    u8 mUnk_01e[16];
    u8 mUnk_02e;
    u8 mUnk_02f;
    u8 *mUnk_030;
    s32 mUnk_034;
    s32 mUnk_038;
    Unk_LoadEntry *mUnk_03c;
    Unk_VramGroup mUnk_040;
    Unk_VramGroup mUnk_070;
    Unk_VramBank mUnk_0a0;
    Unk_VramBank mUnk_0ac;
    u8 mUnk_0b8[9];
    u8 mUnk_0c1;
    u8 mUnk_0c2[2];
    void *mUnk_0c4;
    u8 *mUnk_0c8;
    u16 mUnk_0cc;
    u8 mUnk_0ce[2];
    MICAutoParam mUnk_0d0;
    u16 mUnk_0ec[16][8];
    u32 mUnk_1ec[8];
    u32 mUnk_20c[8];
    u8 mUnk_22c;
    u8 mUnk_22d[3];
    void (*mUnk_230)(u16);
    s32 mUnk_234;
    s32 mUnk_238;
    s32 mUnk_23c;
    void (*mUnk_240)(void);
    s32 mUnk_244;
    s32 mUnk_248;
    void (*mUnk_24c)(int);
    Unk_020c1b0c mUnk_250;
    u8 mUnk_27c[4];
    s32 mUnk_280;
    u16 *mUnk_284;
    u16 mUnk_288;
    u16 mUnk_28a;
    u16 mUnk_28c;
    u16 mUnk_28e;
    u16 mUnk_290;
    u16 mUnk_292;
    u16 mUnk_294;
    u16 mUnk_296;
    u8 *mUnk_298;
    u8 mUnk_29c[0x2a2 - 0x29c];
    u8 mUnk_2a2;
    u8 mUnk_2a3;
    s8 mUnk_2a4[2];
    u8 mUnk_2a6_0 : 1;
    u8 mUnk_2a6_1 : 1;
    u8 mUnk_2a6_2 : 6;
    u8 mUnk_2a7;
    s32 mUnk_2a8;
    u8 mUnk_2ac_0 : 1;
    u8 mUnk_2ac_1 : 1;
    u8 mUnk_2ac_2 : 1;
    u8 mUnk_2ac_3 : 1;
    u8 mUnk_2ac_4 : 1;
    u8 mUnk_2ac_5 : 1;
    u8 mUnk_2ac_6 : 2;
    u8 mUnk_2ad;
    u8 mUnk_2ae;
    u8 mUnk_2af;
    u8 mUnk_2b0;
    u8 mUnk_2b1;
    u8 mUnk_2b2;
    u8 mUnk_2b3;
    u8 mUnk_2b4;
    u8 mUnk_2b5[0x2b8 - 0x2b5];
    s32 mUnk_2b8;
    s32 mUnk_2bc;
    s32 mUnk_2c0;
    s32 mUnk_2c4;
    s32 mUnk_2c8;
    u16 mUnk_2cc;
    u16 mUnk_2ce;
    u16 mUnk_2d0;
    u8 mUnk_2d2[2];
    s32 mUnk_2d4;
    s32 mUnk_2d8;
    s32 mUnk_2dc;
    s32 mUnk_2e0;
    u16 mUnk_2e4;
    u16 mUnk_2e6;
    u16 mUnk_2e8;
    u16 mUnk_2ea;
    u16 mUnk_2ec;
    u16 mUnk_2ee;
    u8 mUnk_2f0;
    u8 mUnk_2f1;
    u16 mUnk_2f2;
    u16 mUnk_2f4;
    u16 mUnk_2f6;
    u16 mUnk_2f8;
    u8 mUnk_2fa;
    u8 mUnk_2fb;
    s16 mUnk_2fc;
    s16 mUnk_2fe;
    s16 mUnk_300;
    s16 mUnk_302;
    u32 (*mUnk_304)(u32);
    u16 mUnk_308;
    u8 mUnk_30a[2];
    u16 mUnk_30c;
    u16 mUnk_30e;
    u8 mUnk_310;
    u8 mUnk_311[2];
    u8 mUnk_313;
    u8 mUnk_314;
    u8 mUnk_315;
    u8 mUnk_316;
    u8 mUnk_317_0 : 1;
    u8 mUnk_317_1 : 7;
    u8 mUnk_318;
    u8 mUnk_319[3];
    void (*mUnk_31c)(u32);
    u8 *mUnk_320;
    s32 mUnk_324;
    u16 mUnk_328;
    u16 mUnk_32a;
    u16 mUnk_32c;
    u8 mUnk_32e;
    u8 mUnk_32f;
    u16 mUnk_330;
    u8 mUnk_332[2];
    s32 mUnk_334;
    u32 mUnk_338;
    u16 mUnk_33c;
    u8 mUnk_33e[2];
    u16 mUnk_340[4];
    u8 mUnk_348[4];
    u8 *mUnk_34c;
    u32 mUnk_350;
    u16 mUnk_354;
    u8 mUnk_356[0x390 - 0x356];
    u32 mUnk_390[(0x42c - 0x390) / 4];
    s32 mUnk_42c;
    u8 mUnk_430[0x450 - 0x430];
    u32 mUnk_450;
    u16 mUnk_454;
    u8 mUnk_456;
    u8 mUnk_457;
    u8 mUnk_458[4];
    vu32 mUnk_45c;
    u32 mUnk_460;
    void (*mUnk_464)(void *data, u32 size);
    void (*mUnk_468)(void);
    u32 mUnk_46c;
    s32 mUnk_470;
    u8 mUnk_474[4];
    u32 mUnk_478;
    u16 mUnk_47c;
    u16 mUnk_47e;
    u16 mUnk_480;
    u8 mUnk_482[2];
    s32 mUnk_484;
    u16 mUnk_488;
    u16 mUnk_48a;
    s8 mUnk_48c;
    s8 mUnk_48d;
    u8 mUnk_48e;
    u8 mUnk_48f;
    u8 mUnk_490[2];
    u8 mUnk_492;
    u8 mUnk_493;
    u8 mUnk_494;
    u8 mUnk_495;
    s16 mUnk_496;
    s32 mUnk_498;
    u16 mUnk_49c[2];
    s32 mUnk_4a0;
} SystemState;

extern SystemState data_020c18bc;
// A file of the start-up code; comm_link.c also reads through it.
extern FSFile data_020c1874;

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
    u32 mUnk_0[2];
    s32 mUnk_8;
    u16 mUnk_c;
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
void func_0201c6d4(void);
u32 func_0201c704(u32 size);
void func_0201c78c(void);
void func_0201c7b8(int a);

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

// Hash steps of the graphics pools and the file cache: 1 and the odd primes up to 719.
extern const u16 data_02050378[128];
#ifdef __cplusplus
}
#endif
