#pragma once

#include "types.h"
#include <nitro/fx.h>
#include <Main/System/game_util.h>

// Per-stage settings; the table of records has 0x230 bytes per stage.
typedef struct StageInfo {
    s16 mUnk_00;
    s16 mUnk_02;
    // -9999 marks a record that is out (tested as > -5000 / < -5000). AddValue adds to it and clamps it to
    // columns grade + 4 and grade + 5 of the stage's row of table 0xfd6.
    s16 mValue;
    s16 mKindValue;
    // Subtracted by GetBattleTotal. Table entry kind 5 negates it, so that it then adds; the compare takes the same
    // amount off both sides through it.
    s16 mSubtracted;
    s16 mAdded;
    s16 mOthersHalfSum;
    // The side's battle total before the last table entry applied by either side (stored after ApplyTableEntry);
    // read by table entry kind 2.
    s16 mPrevBattleTotal;
    u8 mKind;
    u8 mGrade;
    u8 mPercent;
    u8 mStageId;
    // Set to 0 by LoadStageInfo; nothing reads it.
    u8 mUnk_14;
    // Set by table entry kind 12; kinds 13-16 triple their amount and clear it (kind 15 only when it applies).
    // GetTableEntryTotalDiff's estimate triples only for kinds 13 and 14.
    u8 mTriplePending;
    // Skips the side's next entry pick: set by table entry kind 4 for the other side, by key bit 2, by button 201 and
    // when FindBestSide1Entry finds none; cleared after the side's turn.
    u8 mSkipEntry;
    // Screen position (mBattleY: y) of the stage's sprite on the battle screen; the sprite's moves start or end there.
    u8 mBattleX;
    u8 mBattleY;
    u8 mOthersHalfSumAdded;
    // A random value 0..3 (func_02014ccc; func_02014cdc picks a different one). The panel compares it with
    // data_020b6ee0.mUnk_a13d while the stage's mUnk_22e is 1. In panel states 2 and 5 the selected stage is drawn
    // with sprite mUnk_1a + 0x42 (DrawStageSprite) while data_020b6ee0.mUnk_a148 is 0.
    u8 mUnk_1a;
    // Index in gStages; set only on the battle copies (gBattleSlotStages), -1 in gStageInfos.
    s8 mStageIndex;
    s16 mTableEntryIds[5];
} StageInfo;

typedef struct StageOffset {
    s32 x;
    s32 y;
} StageOffset;

// A stage: its first 0x178 bytes are the stage's Model (the record is passed to InitModel and UpdateAndDrawModel).
typedef struct StageRec {
    u8 mUnk_000[0x134];
    u32 mSignals;
    u8 mUnk_138[0x142 - 0x138];
    u16 mEventKey;
    u8 mUnk_144[0x158 - 0x144];
    // The stage's position (mPos, or the draw position with the animation offset added), copied into the camera's
    // mLookAtGoal by SetStageCameraFar / SetStageCameraNear. It is Model.mNodePos, which UpdateAndDrawModel leaves
    // alone here: a stage's node index stays 0xff.
    VecFx32 mLookAtPos;
    u8 mUnk_164[0x170 - 0x164];
    u16 mModelId;
    u8 mUnk_172[0x178 - 0x172];
    StageInfo mInfo;
    u8 mUnk_19e[2];
    union {
        DrawParams mDraw;
        struct {
            VecFx32 mPos;
            u8 mUnk_1ac[0x1d8 - 0x1ac];
            // Direction the stage moves in; also mDraw.mUnk_38, the angle the stage is drawn with. The panel
            // (stage_info_panel.c) sets it to 0xe000 and eases it back to 0.
            u16 mMoveAngle;
            u16 mUnk_1da;
            u16 mUnk_1dc;
            u8 mUnk_1de[0x1e4 - 0x1de];
        };
    };
    s32 mStoredOffsetX;
    // 0 when mStoredOffsetX/Z hold an offset from map entry 0 (StoreStageOffsets, the save reader), -1 when they hold
    // none (InitStage); state 10's set-up then starts the stage at entry 0 with mDistancePerFrame 0x6000.
    s32 mNoStoredOffset;
    s32 mStoredOffsetZ;
    s32 mScreenX;
    s32 mScreenY;
    StageOffset mSpriteOffsets[3];
    // mPos at the last func_02016008 call that met no map object (type-0x14 objects do not count); meeting a type-0xf
    // object puts the stage back there.
    VecFx32 mPrevPos;
    // Vertical speed, used with both signs: the panel's bounce (UpdatePanel) adds it to mPos.y and lowers it by 2000 a
    // frame, while UpdateStageMotion subtracts it while the stage is above the ground and raises it by 5120 a frame, up
    // to 51200.
    float mPosYPerFrame;
    s32 mDistancePerFrame;
    // Angle of the camera around the stage (SetStageCameraFar / SetStageCameraNear); only ever 0 in this build.
    u16 mCameraAngle;
    // Polygon group (10..15) the stage lies in, minus 10 (FindStagePolygonGroup); 99 = none. Compared with
    // mInfo.mKind.
    s8 mPolygonGroupIndex;
    u8 mUnk_227[5];
    u8 mUnk_22c;
    u8 mInSlot;
    u8 mUnk_22e;
    // 1 when any part of the stage is in view (func_02031d0c, set every frame of state 10); the selected stage is
    // still projected and drawn while it is 0, but DrawStageSprite draws nothing for it.
    u8 mVisible;
} StageRec;

#define STAGE_MAX 0x27

extern StageRec gStages[];
// A separate list of up to 3 StageInfo records, not the mInfo of gStages.
extern StageInfo gStageInfos[3];
