#pragma once

#include "types.h"
#include <nitro/fx.h>
#include <Main/Unknown/game_util.h>
#include <Main/Unknown/move_object.h>
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
    // Counts down in POPUP_STATE_DISAPPEAR; HandlePopupTouch also sets it to 30 for POPUP_STATE_TOUCHED, which counts
    // it down the same way.
    s32 mDisappearTimer;
    s32 mUnk_1c;
    s32 mState;
    // Grid cell (3 across) the popup is drawn in; both StartPopup calls pass the slot's own index, so it always equals
    // the slot's position in mSlots.
    s32 mCellIndex;
} PopupSlot;

#define POPUP_SLOT_COUNT 9

typedef struct PopupPos {
    s32 x;
    s32 y;
} PopupPos;

// The stage scene object (gScene points to it): the stage settings, two models, the draw parameters, the
// scene's vectors and counters, and the timed popup slots.
typedef struct Scene {
    u8 mUnk_000[0x178];
    StageInfo mStage;
    u8 mUnk_19e[0x230 - 0x19e];
    Model mUnk_230;
    DrawParams mUnk_3a8;
    Model mUnk_3ec;
    u8 mUnk_564[0x5a8 - 0x564];
    VecFx32 mBasePos;
    VecFx32 mUnk_5b4;
    VecFx32 mUnk_5c0;
    VecFx32 mUnk_5cc;
    VecFx32 mUnk_5d8;
    VecFx32 mShakeTarget;
    VecFx32 mShakeOffset;
    VecFx32 mShakePerFrame;
    u32 mShakeFrameIndex;
    s32 mShakeTimer;
    u32 mScore;
    s32 mEndTimerMax;
    // Counts down in SCENE_STATE_RUN; at 0 the scene state becomes SCENE_STATE_END. Also the time func_0201168c draws,
    // raised by mUnk_65c (up to mEndTimerMax) when a kind-1 popup is touched, and its ratio to mEndTimerMax places the
    // camera points (UpdateSceneCamera).
    s32 mEndTimer;
    // Row of table 0xd64: advanced by func_0200ce24 every mRowSeconds seconds, stepped back by AddPopups while the
    // row's popup count is 0.
    s32 mPopupRow;
    s32 mRowPopupCount;
    s32 mUnk_624;
    s32 mUnk_628;
    s32 mUnk_62c;
    u32 mScoreThreshold;
    s32 mPopupTimer;
    s32 mPopupIntervalMs;
    s32 mUnk_63c;
    s32 mPopupIntervalMinMs;
    s32 mPopupShowMs;
    s32 mUnk_648;
    s32 mPopupShowMinMs;
    s32 mRowSeconds;
    s32 mRowTimer;
    s32 mUnk_658;
    s32 mUnk_65c;
    s32 mUnk_660;
    s32 mUnk_664;
    PopupSlot mSlots[POPUP_SLOT_COUNT];
    // The amount ShowNumber was last given (mUnk_658 or mUnk_660, when a popup of a kind other than 1 and 3 is
    // touched); DrawNumber draws nothing while it is 0.
    s32 mNumber;
    u16 mNumberLevel;
    u16 mUnk_7d6;
    PopupPos mPos;
} Scene;

extern Scene *gScene;
extern const u16 data_0204e1c0[];
extern const s32 data_0204e1cc[][2];

void GetMapEntryPos(int index, VecFx32 *out);
void StartShake(int ms);
void SetupSceneCamera(void);
void UpdateSceneCamera(void);
void SetupScene(void);
void UpdateScene(void);
void LeaveScene(void);
void DrawScene(void);
void func_0200cdc4(void);
void func_0200ce24(void);
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
