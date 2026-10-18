#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// The stream audio object data_020c661c.
typedef struct Unk_MovieAudio {
    u8 mUnk_00[0x2c];
    /* 2c */ u32 mUnk_2c;
    u8 mUnk_30[4];
    /* 34 */ u8 *mUnk_34;
    u8 mUnk_38[4];
    /* 3c */ u32 mUnk_3c;
    /* 40 */ u32 mUnk_40;
    u8 mUnk_44[0x100];
} Unk_MovieAudio;
extern Unk_MovieAudio data_020c661c;

void *func_0202f500(u32 size);
void func_0202f50c(void *p);
void func_0202f520(void *arg);
void func_0202f538(u32 a, u32 b);
void func_0202f614(void *arg);
void *func_0202f628(u32 screen);

#ifdef __cplusplus
}
#endif
