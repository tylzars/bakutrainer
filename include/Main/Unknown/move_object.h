#pragma once

#include "types.h"
#include <Main/Unknown/falling_blocks.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TRAIL_LEN 10

// Animation state of a model (two copies at 0x14c and 0x152).
typedef struct ModelAnim {
    u16 mUnk_0;
    u16 mUnk_2;
    u8 mUnk_4;
    u8 mUnk_5;
} ModelAnim;

// A model object: 0x178 bytes with a model id at 0x170.
typedef struct Model {
    u8 mUnk_000[4];
    u16 mUnk_004;
    u8 mUnk_006[6];
    s16 mUnk_00c;
    u8 mUnk_00e[2];
    s16 mUnk_010;
    u8 mUnk_012[0x22 - 0x12];
    s16 mUnk_022;
    u8 mUnk_024[0x134 - 0x24];
    u32 mUnk_134;
    u8 mUnk_138[0x142 - 0x138];
    u16 mUnk_142;
    u8 mUnk_144[0x14c - 0x144];
    ModelAnim mUnk_14c;
    ModelAnim mUnk_152;
    u8 mUnk_158[0x170 - 0x158];
    u16 mUnk_170;
    union {
        u16 mUnk_172;
        struct {
            u16 mUnk_172_0 : 9;
            u16 mUnk_172_9 : 1;
            u16 mUnk_172_10 : 1;
            u16 mUnk_172_11 : 5;
        };
    };
    u8 mUnk_174;
    u8 mUnk_175;
    u8 mUnk_176[2];
} Model;

// A sprite and/or model that moves toward a target and leaves a trail of earlier positions.
typedef struct MoveObj {
    Sprite mSprite;
    Model mModel;
    u8 mFlags;
    u8 mUnk_27d[3];
    s32 mActive;
    s32 mRepeat;
    s32 mX;
    s32 mY;
    s32 mUnk_290;
    u32 mUnk_294;
    s32 mScale;
    s32 mSpeed;
    s32 mTargetX;
    s32 mTargetY;
    s32 mStartX;
    s32 mStartY;
    s32 mProgress;
    s32 mUnk_2b4;
    s32 mArrived;
    Vec2i mTrail[TRAIL_LEN];
    u16 mTrailStart;
    u16 mTrailEnd;
    u8 mUnk_310[4];
} MoveObj;

void func_0200c034(MoveObj *o, int sprite, int model, const Vec2i *pos, int e, int f, int g);
void func_0200c0dc(MoveObj *o, int speed, const Vec2i *target, int limit);
void func_0200c124(MoveObj *o, u16 lo, u16 hi);
void func_0200c164(MoveObj *o);
int func_0200c174(MoveObj *o, const Vec2i *pos);
void func_0200c3d8(MoveObj *o, int *outX, int *outY, int *state);
void func_0200c470(MoveObj *o);
void func_0200c5d8(int a, int b, int c, int d, int e, int f, int g);

#ifdef __cplusplus
}
#endif
