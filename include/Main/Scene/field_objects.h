#pragma once

#include "types.h"
#include <Main/Scene/stage_data.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Pos2 {
    s32 mX;
    s32 mY;
} Pos2;

// Extent of the playing field in 8 x 8 cells.
typedef struct FieldInfo {
    VecFx32 mUnk_00;
    VecFx32 mUnk_0c;
    s32 mScrolledDistance;
    s32 mGridStartX;
    s32 mGridY;
    s32 mGridStartZ;
    s32 mFirstGroupEndX;
    s32 mUnk_2c;
    s32 mGridEndZ;
} FieldInfo;

// One object of the field; the field has 4 x 8 x 8 of them in a grid plus a list of 128.
typedef struct FieldObject {
    u8 mType;
    u8 mKind;
    u8 mUnk_02[2];
    s32 mPosX;
    s32 mPosY;
    s32 mPosZ;
    u8 mUnk_10[0x20];
    s32 mUnk_30;
    s32 mUnk_34;
    s32 mUnk_38;
    u8 mUnk_3c[0xc];
    u8 mModel[0x178];
    s8 mMoveDirZ;
    u8 mUnk_1c1;
    s16 mListIndex;
} FieldObject;

typedef struct FieldObjects {
    u8 mUnk_000[4];
    u32 mEndTimer;
    // In 1/256 units; the mode's score is mScore >> 8 (at most 999).
    u32 mScore;
    s32 mNoScoreTimer;
    // Bytes 0x010-0x240 have StageRec's layout: the stage's Model (this, mUnk_144 and mUnk_148), mStage as its mInfo,
    // and from mStagePos the 0x44 bytes of draw parameters passed to UpdateAndDrawModel (mUnk_1d8, mUnk_1e8 and
    // mUnk_1f3 are their mUnk_28, mUnk_38 and mUnk_43).
    u8 mStageModel[0x134];
    u32 mUnk_144;
    u8 mUnk_148[0x40];
    StageInfo mStage;
    u8 mPad_1ae[2];
    VecFx32 mStagePos;
    u8 mUnk_1bc[0x1c];
    u32 mUnk_1d8;
    u8 mUnk_1dc[0xc];
    u16 mUnk_1e8;
    u8 mUnk_1ea[9];
    u8 mUnk_1f3;
    u8 mUnk_1f4[0x4c];
    // Offset of the stage from the scroll position (the camera's mUnk_228): mX along x, mY along z.
    Pos2 mStageOffset;
    // Change of mStageOffset.mX per 60 frames; a sixtieth is added each frame.
    s32 mOffsetSpeedX;
    // The same for mStageOffset.mY, which moves the stage along z.
    s32 mOffsetSpeedY;
    Pos2 mDriveStartOffset;
    s32 mDriveDistance;
    u32 mDriveTimer;
    u8 mDrivePlusX;
    u8 mUnk_261[3];
    // Set while mEndTimer runs, mDrivePlusX is 0 and mStageOffset.mX is below -0x106800 (it then blocks
    // StartDriveMinusX); cleared, while no drive runs, once the offset is back at or above that bound.
    s32 mBelowMinX;
    s32 mAboveMaxX;
    // In units of 512: the x / z distance from mScrollOrigin to mInfo.mUnk_00. The grid groups shift each time the
    // scroll passes it.
    s32 mGridShiftDistance;
    s32 mScrollSpeed;
    VecFx32 mScrollOrigin;
    FieldInfo mInfo;
    FieldObject mGrid[4][8][8];
    FieldObject mList[128];
    u8 mPatterns[64][8][8];
    u8 mPatternList[0x40];
    // Entries of mPatternList, including the one stored for the negative value that ends the table's list.
    u8 mPatternListCount;
    u8 mUnk_2b8f5[3];
    u32 mMinScrollSpeed;
    u32 mMaxScrollSpeed;
    u32 mPatternListColumn;
    u32 mEndFrameCount;
    u32 mScoreThreshold;
    u32 mAddSeconds;
    u32 mNoScoreFrameCount;
} FieldObjects;

extern FieldObjects gFieldObjects;

s32 GetFieldTableValue(int a, int b);
s32 GetFieldPatternValue(int a, int b);
s32 GetFieldPatternListValue(int a, int b);
void GetGridCell(s32 x, s32 z, s32 *outGroup, s32 *outSub, s32 *outRow);
void DrawFieldObjects(void);
void EmptyGridGroup(u8 group);
void FillGridGroup(u8 group, u8 pattern);
void MoveListObjects(void);
void ShiftGridGroups(void);
int IsXInputEnabled(void);
int IsYInputEnabled(void);
void StartDriveMinusX(s32 value, u32 time);
void StartDrivePlusX(s32 value, u32 time);
void ApplyFieldObject(FieldObject *obj);
void ApplyObjectsAtStage(void);
void SetupFieldCameraAndInfo(void);
void UpdateFieldScrollAndScore(void);
void SetupField(void);
void DrawField(void);
void UpdateField(void);
void LeaveField(void);
int GetFieldScoreThreshold(u8 tableRow);

#ifdef __cplusplus
}
#endif
