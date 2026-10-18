#pragma once

#include "types.h"
#include <Main/Unknown/globals.h>
#include <Main/Unknown/system_init.h>

#ifdef __cplusplus
extern "C" {
#endif

// Width and height in pixels of each texture size code (index = size code, [0] = width, [1] = height).
extern const u16 data_02050978[16];
extern const u8 data_02050998[16][2];

// One texture record of a block: a packed word (two signed 9-bit values, a 3-bit and a 4-bit code, a 7-bit
// value) and a second word whose low 24 bits are an offset and whose high byte holds two 4-bit values.
typedef struct TexRec {
    s32 mUnk_00_0 : 9;
    s32 mUnk_00_9 : 9;
    u32 mUnk_00_18 : 3;
    u32 mUnk_00_21 : 4;
    u32 mUnk_00_25 : 7;
    u32 mUnk_04;
} TexRec;

void func_0202ad34(void);
void func_0202ad38(void);
void func_0202ad3c(void);
int func_0202ad40(void);
void func_0202ad48(int a);
void func_0202ad4c(void);
void func_0202ad50(void);
void func_0202ad54(void);
u16 func_0202ad58(void);
u16 func_0202ad68(void);
u8 func_0202ad78(int layer);
s16 func_0202ad9c(void);
s8 func_0202adac(void);
u32 *func_0202adc0(u32 index);
u16 func_0202ade0(void);
u32 func_0202adf0(int layer);
void func_0202ae34(int layer);
void func_0202ae84(u32 index, TexInfo *out, u32 sub);
u16 func_0202afb0(u32 index, int layer);
void func_0202b19c(int layer, u32 key);
u16 func_0202b26c(u32 index, int layer);
u32 func_0202b32c(u32 index, int mode);
void func_0202bb30(int layer);
void func_0202bdd8(int a, int b, int c, u32 d, int e);
void func_0202c1c4(void);
void func_0202c1d4(int a);
void func_0202c2f0(void *dst, const void *src, u32 w, u32 h, BOOL mode);
void func_0202c3e0(Unk_LoadEntry *entry, const void *src, u32 w);
void func_0202c548(u32 i, Unk_020c2694 *half);
u32 func_0202c5c0(u32 key, u32 val, Unk_020c2694 *half);
u32 func_0202c5fc(u32 index, u32 key, u32 val, Unk_020c2694 *half);
s32 func_0202c778(u16 *keys, int a, u32 n, int engine);

#ifdef __cplusplus
}
#endif
