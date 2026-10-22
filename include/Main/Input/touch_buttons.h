// Side header of system_init.c: its touch buttons and touch trail.
#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// A point of the touch trail (two bytes) that ReadTouchAndAppendTrail appends (see TouchTrail).
typedef struct TrailPoint {
    u8 mX;
    u8 mY;
} TrailPoint;

// A touch button: the box mMinX..mMaxX by mMinY..mMaxY, except for a TOUCH_SHAPE button, whose mMinX and mMinY are
// the shape's position and mMaxY the shape id. mId is the caller's id with the TOUCH_SHAPE bit. mReleaseSoundId plays
// on release and, when non-zero, also lets the release stop mPressSoundId.
typedef struct TouchButton {
    s16 mMinX;
    s16 mMinY;
    s16 mMaxX;
    s16 mMaxY;
    u16 mId;
    u16 mPressSoundId;
    u16 mReleaseSoundId;
    u16 mKeys;
} TouchButton;

// The touch state that follows the trail. mUnk_00 holds the stroke's start x and y, then its end x and y; mUnk_18 is
// the stroke step (1 while touching, 2 once released with the end stored, then 3).
typedef struct TouchState {
    u8 mUnk_00[4];
    u32 mButtonState;
    u32 mButtonCount;
    // Frame (gSystemState.mElapsedFrames) of the last touch-down.
    u32 mPressFrame;
    u16 mTrailCount;
    u16 mHitId;
    u8 mPrevDown;
    u8 mUnk_15;
    // Sprite drawn at two corners of each button added while it is non-zero; never set in this build (both
    // ResetTouchButtons calls pass 0).
    u16 mCornerSpriteId;
    u8 mUnk_18;
} TouchState;

// The touch trail (the points ReadTouchAndAppendTrail appends; their count is mTouchState.mTrailCount, wrapping at
// 0x80) and the touch state that follows it form one object.
typedef struct TouchTrail {
    TrailPoint mPoints[0x80];
    TouchState mTouchState;
} TouchTrail;

// The touch buttons, followed by the touch trail and its state, form one object.
typedef struct TouchInput {
    TouchButton mButtons[0x80];
    TouchTrail mTrail;
} TouchInput;
extern TouchInput gTouchInput;

int IsLcdSwapped(void);
int func_02020890(void);
void SetTouchButtonSoundsAndKeys(int flags, u16 a, u16 b, u16 c);
void AddTouchButtonEx(int a, int b, int c, int d, int e, int f, u16 g, u16 h, u16 i);
void AddTouchButtonWithSounds(int a, int b, int c, int d, int e, int f, u16 g, u16 h);
void AddTouchButton(int id, int x, int y, int a, int b, int c);
int IsTouchInShape(int x, int y, int shape);
int IsPointInShape(int x, int y, int shape, int px, int py);
void ClearTouchButtons(void);
u32 UpdateTouchButtons(u32 mask);
void ResetTouchTrail(void);
void ResetTouchButtons(u16 a, int b);
int ReadTouchAndAppendTrail(u8 *x, u8 *y);

#ifdef __cplusplus
}
#endif
