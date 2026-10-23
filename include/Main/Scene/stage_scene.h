#pragma once

#include "types.h"
#include <nitro/fx.h>
#include <Main/System/game_util.h>
#include <Main/Scene/move_object.h>
#include <Main/Scene/stage_data.h>

#ifdef __cplusplus
extern "C" {
#endif

// One timed slot of the popup table; the table has 9 of them.
typedef struct PopupSlot {
    s32 mKind;
    // Only ever set to 0 (StartPopup), so POPUP_STATE_WAIT lasts one update in this build.
    s32 mWaitTimer;
    s32 mAppearFrameCount;
    // Counts down from 0x10 in POPUP_STATE_APPEAR; the popup sprite and its touch button are added from 2 on.
    s32 mAppearTimer;
    s32 mShowTimer;
    s32 mDisappearFrameCount;
    // Counts down in POPUP_STATE_DISAPPEAR. HandlePopupTouch also sets it to 30 for POPUP_STATE_TOUCHED, which counts
    // it down the same way.
    s32 mDisappearTimer;
    // Set by StartPopup to the scene's amount for the kind (kind 3 leaves it); never read in this build.
    s32 mAmount;
    s32 mState;
    // Grid cell (3 across) the popup is drawn in. Both StartPopup calls pass the slot's own index, so it always equals
    // the slot's position in mSlots.
    s32 mCellIndex;
} PopupSlot;

#define POPUP_SLOT_COUNT 9

typedef struct PopupPos {
    s32 x;
    s32 y;
} PopupPos;

// The stage scene object; gScene points to it.
typedef struct Scene {
    u8 mUnk_000[0x178];
    StageInfo mStage;
    u8 mUnk_19e[0x230 - 0x19e];
    Model mUnk_230;
    DrawParams mDraw;
    Model mUnk_3ec;
    u8 mUnk_564[0x5a8 - 0x564];
    VecFx32 mBasePos;
    VecFx32 mMapEntry1Pos;
    VecFx32 mMapEntry0Pos;
    VecFx32 mMapEntry2Pos;
    VecFx32 mMapEntry4Pos;
    VecFx32 mShakeTarget;
    VecFx32 mShakeOffset;
    VecFx32 mShakePerFrame;
    u32 mShakeFrameIndex;
    s32 mShakeTimer;
    u32 mScore;
    s32 mEndTimerMax;
    // Counts down in SCENE_STATE_RUN; at 0 the scene state becomes SCENE_STATE_END. Also the time DrawModeScoreAndTime
    // draws. A touched kind-1 popup raises it by mKind1Amount (up to mEndTimerMax). Its ratio to mEndTimerMax places
    // the camera points (UpdateSceneCamera).
    s32 mEndTimer;
    // Row of table 0xd64. AdvancePopupRow advances it every mRowSeconds seconds; AddPopups steps it back while the
    // row's popup count is 0.
    s32 mPopupRow;
    s32 mRowPopupCount;
    // From table 0xd63. A touched kind-3 popup counts it down; at 0 the scene goes to SCENE_STATE_POPUPS_ONLY.
    s32 mKind3TouchesRemaining;
    // AddPopups starts a kind-1 popup first when mTouchCount is more than this.
    s32 mKind1TouchThreshold;
    // Popups touched since the last kind-3 touch or the last kind-1 popup AddPopups started (both reset it to 0).
    s32 mTouchCount;
    u32 mScoreThreshold;
    s32 mPopupTimer;
    s32 mPopupIntervalMs;
    s32 mPopupIntervalMsPerRow;
    s32 mPopupIntervalMinMs;
    s32 mPopupShowMs;
    s32 mPopupShowMsPerRow;
    s32 mPopupShowMinMs;
    s32 mRowSeconds;
    s32 mRowTimer;
    // What a touched popup of the kind adds: score for kinds 2 and 0, frames of mEndTimer for kind 1.
    s32 mKind2Amount;
    s32 mKind1Amount;
    s32 mKind0Amount;
    s32 mUnk_664;
    PopupSlot mSlots[POPUP_SLOT_COUNT];
    // The amount ShowNumber was last given: mKind2Amount or mKind0Amount, when a popup of a kind other than 1 and 3 is
    // touched. DrawNumber draws nothing while it is 0.
    s32 mNumber;
    u16 mNumberLevel;
    u16 mUnk_7d6;
    PopupPos mPos;
} Scene;

extern Scene *gScene;
extern const u16 gSceneMapIds[];
extern const s32 gKind3TouchPositions[][2];

void GetMapEntryPos(int index, VecFx32 *out);
void StartShake(int ms);
void SetupSceneCamera(void);
void UpdateSceneCamera(void);
void SetupScene(void);
void UpdateScene(void);
void LeaveScene(void);
void DrawScene(void);
void DrawKind3Touches(void);
void AdvancePopupRow(void);
void AddPopups(void);
void StartPopup(int slot, int type, int index);
void UpdatePopups(void);
void HandlePopupTouch(PopupSlot *slot);
void LoadSceneValues(void);
void ClearNumber(void);
void ShowNumber(int value, const PopupPos *pos);
void DrawNumber(void);
int GetSceneScoreThreshold(u8 tableRow);

#ifdef __cplusplus
}
#endif
