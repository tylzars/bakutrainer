#pragma once

#include "types.h"
#include <Main/Unknown/move_object.h>

#ifdef __cplusplus
extern "C" {
#endif

// Scene objects: each starts with a model (the ones in the first array are 0x188 bytes) or, in the third kind, has
// the position first and the model after it.
typedef struct Unk_020b6ee8 {
    Model mModel;
    s32 mUnk_178;
    s32 mUnk_17c;
    u8 mUnk_180[4];
    u8 mUnk_184;
    u8 mUnk_185;
    s8 mUnk_186;
    u8 mUnk_187;
} Unk_020b6ee8;

typedef struct Unk_020b7ef4 {
    Model mModel;
    s32 mUnk_178;
    s32 mUnk_17c;
    u8 mUnk_180[2];
    u8 mUnk_182;
    u8 mUnk_183;
    u8 mUnk_184;
    u8 mUnk_185;
    u8 mUnk_186;
    u8 mUnk_187;
    u8 mUnk_188;
    u8 mUnk_189[3];
} Unk_020b7ef4;

typedef struct Unk_020bc938 {
    s32 mUnk_00;
    s32 mUnk_04;
    Model mModel;
    u8 mUnk_180;
    u8 mUnk_181[2];
    u8 mUnk_183;
} Unk_020bc938;

void func_02015d4c(void);
void func_02015d58(int stage);
void func_02015e08(int stage);
void func_02015e94(int stage);
void func_02015f34(void);
void func_02016008(int stage);
void func_020164e0(int stage);

#ifdef __cplusplus
}
#endif
