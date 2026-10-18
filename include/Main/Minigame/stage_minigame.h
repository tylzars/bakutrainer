#pragma once

#include "types.h"
#include <Main/Minigame/falling_blocks.h>
#include <Main/Unknown/game_util.h>
#include <Main/Unknown/move_object.h>
#include <Main/Scene/stage_data.h>

#ifdef __cplusplus
extern "C" {
#endif

// State of the stage choice screen. The code reads it both through the outer object and through a pointer to the
// inner one.
typedef struct Unk_02099774 {
    s32 mUnk_00;
    s32 mUnk_04;
    s32 mUnk_08;
    s32 mUnk_0c;
    s32 mUnk_10;
    u32 mUnk_14;
    u16 mUnk_18[3];
    u8 mUnk_1e[2];
    s32 mUnk_20;
    s32 mUnk_24;
    s32 mUnk_28;
    s32 mUnk_2c;
} Unk_02099774;

typedef struct Unk_02099770 {
    u8 mUnk_00[4];
    Unk_02099774 mIn;
} Unk_02099770;

// One of the 256 moving pieces; the sprite is drawn at the piece's position.
typedef struct Particle {
    u8 mUnk_00;
    u8 mUnk_01[3];
    Vec2i mPos;
    Vec2i mVel;
    u8 mUnk_14;
    u8 mUnk_15;
    u8 mUnk_16;
    u8 mUnk_17;
    Sprite mSprite;
} Particle;

#define PARTICLE_COUNT 256

// State of the piece game: the pieces, their spawn settings and a table of random weights.
typedef struct Unk_020997a4 {
    u8 mUnk_0000;
    u8 mUnk_0001[3];
    u32 mUnk_0004;
    u32 mUnk_0008;
    u8 mUnk_000c[0x184 - 0xc];
    StageInfo mInfo;
    u8 mUnk_01aa[2];
    s32 mUnk_01ac;
    s32 mUnk_01b0;
    u8 mUnk_01b4[0x230 - 0x1b4];
    u16 mUnk_0230;
    u8 mUnk_0232[0x23c - 0x232];
    s32 mUnk_023c;
    s32 mUnk_0240;
    Model mModel;
    Particle mParts[PARTICLE_COUNT];
    u32 mCount;
    s32 mUnk_11fc0;
    u32 mUnk_11fc4[5];
    u32 mUnk_11fd8[5];
    s32 mUnk_11fec;
    s32 mUnk_11ff0;
    s32 mUnk_11ff4;
    s32 mUnk_11ff8;
    s32 mUnk_11ffc;
    u32 mWeights[3][5];
} Unk_020997a4;

extern Unk_02099770 data_02099770;
extern Unk_020997a4 data_020997a4;

void func_0200a384(void);
void func_0200a3f4(void);
void func_0200a3fc(void);
void func_0200a5cc(int value);
void func_0200a5f0(void);
int func_0200a970(int a, int b);
int func_0200a988(int a, int b);
void func_0200a99c(DrawParams *p, u32 a, u32 b, u32 c, s32 d, u16 e, u8 f, u8 g, s32 h);
void func_0200aa2c(Model *obj, u32 a, u32 b, s32 c, u16 d, s32 e);
void func_0200aa68(u16 id, u32 a, u32 b, u32 c, s32 d, u16 e, u8 f, s32 g);
int func_0200aa9c(void);
int func_0200aadc(Particle *src, int kind);
void func_0200acfc(Particle *p, int angle);
void func_0200ad64(Particle *p, int flag);
void func_0200adc0(Particle *p);
void func_0200af28(Particle *p, int angle);
void func_0200b0a0(void);
void func_0200b0e0(void);
void func_0200b244(void);
void func_0200b278(void);
void func_0200b53c(void);
void func_0200b610(void);
void func_0200b884(void);
int func_0200b88c(u8 stage);

#ifdef __cplusplus
}
#endif
