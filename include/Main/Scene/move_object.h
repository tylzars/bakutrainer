#pragma once

#include "types.h"
#include <Main/Minigame/falling_blocks.h>

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

// A queued event of a model: the command-list position it starts and the 16-bit key that orders it.
typedef struct ModelEvent {
    s16 *mUnk_0;
    u16 mUnk_4;
    u8 mUnk_6[2];
} ModelEvent;

// A model object: 0x178 bytes with a model id at 0x170.
typedef struct Model {
    s16 *mUnk_000;
    union {
        s16 mUnk_004_all[16];
        struct {
            u16 mUnk_004;
            u8 mUnk_006[6];
            s16 mUnk_00c;
            u8 mUnk_00e[2];
            s16 mUnk_010;
            u8 mUnk_012[0x22 - 0x12];
            s16 mUnk_022;
        };
    };
    u8 mUnk_024;
    u8 mUnk_025[3];
    // Entry 32 is the one an event index of -1 selects.
    ModelEvent mUnk_028[33];
    u32 mUnk_130;
    u32 mUnk_134;
    u32 mUnk_138;
    s16 *mUnk_13c;
    u16 mUnk_140;
    u16 mUnk_142;
    s16 mUnk_144;
    s16 mUnk_146;
    u8 mUnk_148;
    u8 mUnk_149;
    u8 mUnk_14a[2];
    union {
        struct {
            ModelAnim mUnk_14c;
            ModelAnim mUnk_152;
        };
        ModelAnim mAnim[2];
    };
    VecFx32 mUnk_158;
    VecFx32 mUnk_164;
    u16 mUnk_170;
    union {
        u16 mUnk_172;
        struct {
            u16 mUnk_172_0 : 1;
            u16 mUnk_172_1 : 1;
            u16 mUnk_172_2 : 1;
            u16 mUnk_172_3 : 1;
            u16 mUnk_172_4 : 1;
            u16 mUnk_172_5 : 1;
            u16 mUnk_172_6 : 1;
            u16 mUnk_172_7 : 1;
            u16 mUnk_172_8 : 1;
            u16 mUnk_172_9 : 1;
            u16 mUnk_172_10 : 1;
            u16 mUnk_172_11 : 1;
            u16 mUnk_172_12 : 1;
            u16 mUnk_172_13 : 1;
            u16 mUnk_172_14 : 1;
            u16 mUnk_172_15 : 1;
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
    s32 mPresent;
    s32 mRepeat;
    s32 mX;
    s32 mY;
    s32 mLayer;
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
    void *mUnk_310;
} MoveObj;

void SetupMoveObj(MoveObj *o, int sprite, int model, const Vec2i *pos, int e, int f, int g);
void StartMoveObj(MoveObj *o, int speed, const Vec2i *target, int limit);
void SetMoveObjTrail(MoveObj *o, u16 lo, u16 hi);
void StopMoveObj(MoveObj *o);
int UpdateMoveObj(MoveObj *o, const Vec2i *pos);
void GetMoveObjDrawPos(MoveObj *o, int *outX, int *outY, int *layer);
void DrawMoveObj(MoveObj *o);
void func_0200c5d8(int a, int b, int c, int d, int e, int f, int g);

#ifdef __cplusplus
}
#endif
