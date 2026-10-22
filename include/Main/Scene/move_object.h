#pragma once

#include "types.h"
#include <Main/Minigame/falling_blocks.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TRAIL_LEN 10

// Animation state of a model (two copies at 0x14c and 0x152).
typedef struct ModelAnim {
    u16 mId;
    // Counted up by each step of animation 0 (func_020260f4), which can run twice in one frame; the frame shown is
    // mPlayFrames / mFramesPerAnimFrame.
    u16 mPlayFrames;
    u8 mFramesPerAnimFrame;
    u8 mFlags;
} ModelAnim;

// An event set by MC_EVENT. Of the events whose bits are set in both mEventMask and mSignals, UpdateModelScript
// starts the one whose mKey differs from mEventKey and has the highest high byte not below mEventKey's (on a tie, the
// later entry).
typedef struct ModelEvent {
    s16 *mCommand;
    u16 mKey;
    u8 mUnk_6[2];
} ModelEvent;

// A model object: 0x178 bytes.
typedef struct Model {
    // Read only by MC_CALL, which no model script uses in this build.
    s16 *mReturnCommand;
    union {
        s16 mRegs[16];
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
    u8 mFlags;
    u8 mUnk_025[3];
    // Entry 32 is the one an event index of -1 selects.
    ModelEvent mEvents[33];
    u32 mEventMask;
    // Bits the script sees for one update: UpdateModelScript starts an event whose bit is set and then clears them all,
    // and MC_WAIT stops while its bit is set. Only game code sets them in this build (no script uses MC_SIGNAL or
    // writes register 0x19).
    u32 mSignals;
    // Bits of mSignals that UpdateModelScript and MC_WAIT ignore. Only register 0x1a sets it and no script writes that
    // register, so it is always 0 in this build.
    u32 mBlockedSignals;
    // The command ExecModelScript runs next; NULL for a model without a script, which only steps its animation.
    s16 *mCommand;
    // When a script sets mEventKey (register 0x1e), the old key is copied here unless the new key equals this one.
    // Nothing reads it in this build.
    u16 mPrevEventKey;
    // Key of the part of the script that runs, set by the scripts through register 0x1e; an event whose mKey equals it
    // is not started.
    u16 mEventKey;
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
    // Position of node mNodeIndex, stored by UpdateAndDrawModel after the draw while mNodeIndex is not 0xff. Nothing
    // that reads it runs in this build: the model code reads it only after MC_SET_3C or MC_SET_3E, which no script
    // uses, and the camera only in look-at mode 6, which is never set.
    VecFx32 mNodePos;
    // Added to the draw position by DrawModelFileEx once StartModelAnim has set it, which it does only after
    // MC_SET_3E; no script uses that command, so it is never set in this build.
    VecFx32 mPosPerFrame;
    u16 mId;
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
            // Set when animation 0 reaches its end (also set through mUnk_172, as 0x400, by the battle screen); it
            // stays set, even while the animation loops, until the next StartModelAnim or a clear of the whole model.
            u16 mAnimEnded : 1;
            u16 mUnk_172_11 : 1;
            u16 mUnk_172_12 : 1;
            // At the end of animation 0, hold instead of wrapping to the start: the count stops at the full length
            // less 2, which shows the last frame when mFramesPerAnimFrame is 2 or more and the one before it when it
            // is 1. A scripted run of ExecModelScript clears it first and MC_WAIT_OR_IDLE sets it, so a scripted
            // model holds only while its script sits on MC_WAIT_OR_IDLE; SetupSharedModel sets it for loop 0.
            u16 mHoldAtEnd : 1;
            u16 mUnk_172_14 : 1;
            u16 mUnk_172_15 : 1;
        };
    };
    u8 mUnk_174;
    // Pose node whose position UpdateAndDrawModel stores in mNodePos; 0xff (set by StartModelEx) for none.
    u8 mNodeIndex;
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
    u32 mDrawOrder;
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
    void *mStageRec;
} MoveObj;

void SetupMoveObj(MoveObj *o, int sprite, int model, const Vec2i *pos, int e, int f, int g);
void StartMoveObj(MoveObj *o, int speed, const Vec2i *target, int limit);
void SetMoveObjTrail(MoveObj *o, u16 lo, u16 hi);
void StopMoveObj(MoveObj *o);
int UpdateMoveObj(MoveObj *o, const Vec2i *pos);
void GetMoveObjDrawPos(MoveObj *o, int *outX, int *outY, int *layer);
void DrawMoveObj(MoveObj *o);
void ResetAndSetupGraphics(int lcdSwap, int layer1Image, int layer2Image, int d, int e, int f, int g);

#ifdef __cplusplus
}
#endif
