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
    // 0..2; the switches also have cases 3 and 4, which are never set in this build.
    s32 mState;
    s32 mUnk_08;
    s32 mUnk_0c;
    // Frames in the state, counted only while both screens' fades are clear; in state 0 also the offset of the
    // picture shown.
    s32 mStateFrames;
    u32 mUnk_14;
    u16 mLevels[3];
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
    u8 mKind;
    u8 mUnk_01[3];
    Vec2i mPos;
    Vec2i mVel;
    u8 mPresent;
    u8 mEntered;
    u8 mFacingHitTimer;
    u8 mUnk_17;
    Sprite mSprite;
} Particle;

#define PARTICLE_COUNT 256

// State of the particle game: the particles, their spawn settings and a table of random weights.
typedef struct ParticleGame {
    u8 mUnk_0000;
    u8 mUnk_0001[3];
    u32 mTimer;
    u32 mScore;
    u8 mUnk_000c[0x184 - 0xc];
    StageInfo mInfo;
    u8 mUnk_01aa[2];
    // Position of the steered object (the model), 8 fractional bits.
    s32 mX;
    s32 mY;
    u8 mUnk_01b4[0x230 - 0x1b4];
    u16 mAngle;
    u8 mUnk_0232[0x23c - 0x232];
    s32 mVelX;
    s32 mVelY;
    Model mModel;
    Particle mParts[PARTICLE_COUNT];
    u32 mCount;
    s32 mSpawnTimer;
    u32 mKindScores[5];
    u32 mKindSeconds[5];
    // Bound of SpawnParticles only: the particles a hit splits off are not held to it.
    s32 mParticleLimit;
    s32 mTimerStart;
    s32 mScoreThreshold;
    // Bounds of the spawn delay draw (RandomRange); mSpawnDelayLimit itself can be drawn.
    s32 mSpawnDelayMin;
    s32 mSpawnDelayLimit;
    u32 mWeights[3][5];
} ParticleGame;

extern Unk_02099770 data_02099770;
extern ParticleGame gParticleGame;

void func_0200a384(void);
void func_0200a3f4(void);
void func_0200a3fc(void);
void func_0200a5cc(int value);
void func_0200a5f0(void);
int LookupParticleKindValue(int a, int b);
int LookupParticleGameValue(int a, int b);
void func_0200a99c(DrawParams *p, u32 a, u32 b, u32 c, s32 d, u16 e, u8 f, u8 g, s32 h);
void func_0200aa2c(Model *obj, u32 a, u32 b, s32 c, u16 d, s32 e);
void func_0200aa68(u16 id, u32 a, u32 b, u32 c, s32 d, u16 e, u8 f, s32 g);
int PickParticleKind(void);
int AddParticle(Particle *src, int kind);
void SetParticleDirection(Particle *p, int angle);
void RemoveParticle(Particle *p, int flag);
void ApplyParticleNearHit(Particle *p);
void ApplyParticleFacingHit(Particle *p, int angle);
void DrawParticles(void);
void UpdateParticles(void);
void SpawnParticles(void);
void SetupParticleGame(void);
void DrawParticleGame(void);
void UpdateParticleGame(void);
void LeaveParticleGame(void);
int GetParticleGameScoreThreshold(u8 stage);

#ifdef __cplusplus
}
#endif
