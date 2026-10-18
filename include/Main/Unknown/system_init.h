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
void func_0201c1d0(void);
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
typedef struct Unk_020c321c {
    u8 mUnk_00[6];
    u8 mUnk_06;
    u8 mUnk_07[0x14 - 7];
    u16 mUnk_14;
    u16 mUnk_16;
    u16 mUnk_18;
    u16 mUnk_1a;
    u8 mUnk_1c[2];
    u16 mUnk_1e;
    u16 mUnk_20;
    u16 mUnk_22;
    u16 mUnk_24;
} Unk_020c321c;
extern Unk_020c321c data_020c321c;

// The two halves of the file cache block (0x580 bytes each): an offset table and the data it indexes.
typedef struct Unk_020c2694 {
    u8 mUnk_000[0x14];
    u16 mUnk_014[0x80];
    u8 mUnk_114;
    u8 mUnk_115[0x568 - 0x115];
    u8 mUnk_568[0x10];
    u8 mUnk_578[4];
    u8 mUnk_57c;
    u8 mUnk_57d[3];
} Unk_020c2694;

// File cache block: a preloaded file block (valid flag, its address and its offset), the two halves, the block
// offset table of the packed file, that file's number and its mode.
typedef struct Unk_020c267c {
    u32 mUnk_000;
    void *mUnk_004;
    u8 mUnk_008[4];
    u32 mUnk_00c;
    u8 mUnk_010[7];
    u8 mUnk_017;
    Unk_020c2694 mUnk_018[2];
    u8 mUnk_b18[4];
    u32 *mUnk_b1c;
    u8 mUnk_b20[4];
    u16 mUnk_b24;
    u16 mUnk_b26;
    u8 mUnk_b28[2];
    u16 mUnk_b2a;
    u8 mUnk_b2c[2];
    u8 mUnk_b2e;
} Unk_020c267c;
extern Unk_020c267c data_020c267c;

typedef struct Unk_020c660c_Obj {
    u8 mUnk_00[0x4c];
    u32 mUnk_4c;
    u8 mUnk_50[0x10];
    char mUnk_60[1];
} Unk_020c660c_Obj;

typedef struct Unk_020c660c {
    u32 mUnk_00;
    s32 mUnk_04;
    s32 mUnk_08;
    Unk_020c660c_Obj *mUnk_0c;
} Unk_020c660c;
extern Unk_020c660c data_020c660c;

extern u16 data_020c9e04;
extern void *data_020c9e10;
extern u16 *data_020c9e14;
extern u8 *data_020c9e1c;
extern WMDataSharing data_020c9e60;
void func_0201c580(u16 file, u32 src, u32 dst, u32 size, u32 unpackedSize, u8 mode, u8 flag);
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
    u8 mUnk_00[0xa];
    u16 mUnk_0a;
    u16 mUnk_0c;
    u8 mUnk_0e[2];
} Unk_PoolNode;

typedef struct Unk_PoolSlot {
    s32 mUnk_00;
    s32 mUnk_04;
} Unk_PoolSlot;

typedef struct Unk_PoolRange {
    u16 mUnk_0;
    u16 mUnk_2;
    u16 mUnk_4;
    u16 mUnk_6;
} Unk_PoolRange;

typedef struct Unk_020c31dc {
    Unk_PoolNode *mUnk_00;
    Unk_PoolSlot *mUnk_04;
    Unk_PoolRange *mUnk_08;
    Unk_PoolNode *mUnk_0c;
    Unk_PoolSlot *mUnk_10;
    Unk_PoolRange *mUnk_14;
    u8 mUnk_18[0x4c - 0x18];
    u32 mUnk_4c;
    u32 mUnk_50;
    u16 mUnk_54;
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
