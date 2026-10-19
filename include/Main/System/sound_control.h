#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Volume and track control: mode 1 fades down, 2 fades up, 3 fades down to a target volume.
typedef struct SoundCtl {
    u32 mUnk_00;
    u32 mUnk_04;
    u32 mUnk_08;
    u32 mUnk_0c;
    u32 mUnk_10;
    u32 mUnk_14;
    u32 mUnk_18;
    u32 mUnk_1c;
} SoundCtl;

// Sound effect definitions, 0x24 bytes each; mUnk_18 holds six alternative sound ids.
typedef struct SeEntry {
    u16 mUnk_00;
    u16 mUnk_02;
    u8 mUnk_04;
    u8 mUnk_05;
    u8 mUnk_06[2];
    u32 mUnk_08;
    u32 mUnk_0c;
    u32 mUnk_10;
    u32 mUnk_14;
    u16 mUnk_18[6];
} SeEntry;

// Object whose member 0x12 is a sound effect index.
typedef struct SeOwner {
    u8 mUnk_00[0x12];
    u16 mUnk_12;
} SeOwner;

extern SoundCtl data_020b10b4;
extern const SeEntry data_0204edfc[];

void func_02010388(u32 a, s32 b);
void func_020103c0(void);
void func_020104c0(u32 a, int b);
void func_020104e0(u32 a, int b, int c);
void func_02010500(u32 a);
void func_02010520(u32 index, u16 volume, int b, int c, u32 d);
void func_020105c0(u32 index);
void func_020105f4(void *obj);

#ifdef __cplusplus
}
#endif
