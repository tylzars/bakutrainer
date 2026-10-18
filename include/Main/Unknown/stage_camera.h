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
    void *mUnk_180;
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

// Float position used by the followers.
typedef struct Vec3Float {
    float x;
    float y;
    float z;
} Vec3Float;

// A point the followers chase.
typedef struct Unk_Follower {
    Vec3Float mUnk_00;
    Vec3Float mUnk_0c;
    float mUnk_18;
    float mUnk_1c;
    u16 mUnk_20;
    u16 mUnk_22;
    u16 mUnk_24;
    u16 mUnk_26[8];
    u8 mUnk_36;
    u8 mUnk_37;
} Unk_Follower;

typedef struct Unk_020b1598 {
    Unk_Follower mUnk_000[17];
    u8 mUnk_3b8[0x3f0 - 0x3b8];
    s16 mUnk_3f0;
    s16 mUnk_3f2;
    s16 mUnk_3f4;
    u8 mUnk_3f6;
    u8 mUnk_3f7;
} Unk_020b1598;
extern Unk_020b1598 data_020b1598;

typedef struct Unk_020bf9bc {
    VecFx32 mUnk_00;
    s32 mUnk_0c[2];
    u8 mUnk_14[4];
    u8 mUnk_18;
    u8 mUnk_19;
    u8 mUnk_1a;
    u8 mUnk_1b;
} Unk_020bf9bc;

void func_02015d4c(void);
void func_02015d58(int stage);
void func_02015e08(int stage);
void func_02015e94(int stage);
void func_02015f34(void);
void func_02016008(int stage);
void func_020164e0(int stage);

void func_02016624(void);
void func_02016ae8(int stage);
void func_02016db4(Unk_020b1598 *followers, int index, u16 angle);
void func_02017298(Unk_020b1598 *followers);
int func_0201750c(int index, int *sel);
void func_020175e8(int index, int *sel, int flags);
int func_020182b8(int row, int column);
void func_020182cc(void);
void func_02018558(void);
void func_020186cc(void);
int func_020186f0(u8 stage);
void func_020186fc(int stage);
void func_02018770(StageRec *rec, int kind, int amount);
void func_02018840(StageRec *rec, int kind, int index);
void func_0201896c(void);
int func_02018ccc(const u16 *a, const u16 *b);
void func_02018cd4(void);
void func_020192d4(void);
void func_02019438(void);
void func_0201944c(void);
void func_020194ac(int stage);
void func_02019504(void);
void func_020196b4(void);
int func_020196e0(int value);
void func_02019750(u32 index);
void func_02019788(void);
extern VecFx32 data_020c1800;
extern VecFx32 data_020c180c;
int func_02019874(VecFx32 *pos);
void func_02019a38(StageRec *rec, int x, int y, int c, int d, int e);
void func_02019b38(void);
void func_02019b3c(VecFx32 *a, VecFx32 *b);
void func_02019df8(void);
void func_02019800(StageRec *rec);
void func_020198f0(void);
void func_02019974(StageRec *rec, int a, int b, int c, int d, int e);
void func_02019ad8(void);
void func_02019aec(void);
#ifdef __cplusplus
}
#endif
