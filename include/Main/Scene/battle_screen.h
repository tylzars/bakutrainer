#pragma once

#include "types.h"
#include <Main/Unknown/move_object.h>
#include <Main/Scene/stage_data.h>

#ifdef __cplusplus
extern "C" {
#endif

// Progress of the current exchange.
typedef struct BattleState {
    u32 mUnk_00;
    u16 mUnk_04;
    u16 mUnk_06;
    s16 mUnk_08;
    u8 mUnk_0a;
    u8 mUnk_0b;
    u8 mUnk_0c;
    u8 mUnk_0d;
    s8 mUnk_0e[2];
    u8 mUnk_10[2];
    s8 mUnk_12;
    u8 mUnk_13;
    u8 mUnk_14;
    u8 mUnk_15;
    s8 mUnk_16;
    u8 mUnk_17;
    u8 mUnk_18[2];
    s8 mUnk_1a;
    s8 mUnk_1b;
} BattleState;

// The battle screen: two models, both fighters' stage data and the exchange's bookkeeping.
typedef struct BattleView {
    Model mModel[2];
    StageInfo mSide[2];
    s32 mUnk_33c;
    BattleState mState;
} BattleView;

extern BattleView data_02098d84;
extern StageRec data_020990e0[3];

void func_020071a8(void);
void func_020071ac(void);
void func_020071f8(void);
void func_02007244(int a);
void func_020072a8(void);
void func_02007370(void);
void func_02007390(void);
void func_020073b0(void);
void func_020074ac(void);
void func_02007704(void);
void func_02007750(void);
int func_02007758(void);
void func_02007770(void);
int func_020077d0(StageInfo *info);
void func_020077f0(int rec, int slot);
int func_020078f8(void);
void func_02007940(void);
int func_020079a0(const StageInfo *info);
int func_020079cc(const StageInfo *info);
int func_020079ec(int id);
void func_02007a1c(int a);
void func_02007ab0(void);
void func_02007abc(void);
int func_02007b18(int side, int row);
void func_02007c84(void);
int func_02007ed4(void);
int func_02007f64(void);
void func_02007fa4(int which, int flag);
void func_02007ffc(u16 *a, u16 *b);
void func_02008090(StageInfo *info);
void func_0200827c(void);
void func_02008288(void);
int func_020088d8(StageInfo *info, int mode);
int func_02008920(int side);
int func_02008950(int side);
void func_02008994(int side);
void func_02009728(void);
void func_02009fe8(void);

#ifdef __cplusplus
}
#endif
