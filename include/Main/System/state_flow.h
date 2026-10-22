#pragma once

#include "types.h"
#include <nitro/fx.h>
#include <Main/Scene/stage_data.h>
#include <Main/Scene/move_object.h>
#include <Main/Minigame/falling_blocks.h>
#include <Main/Scene/stage_camera.h>

#ifdef __cplusplus
extern "C" {
#endif

// Main loop state: frame counter, current/previous/requested state numbers, the queue of requested states (PushState
// terminates at 25 entries) and a table of values the states pass on (a prompt number, a selection, a state to
// push): SetFlowValue sets an entry and GetFlowValue reads it back.
#define QUEUE_SIZE 0x1c
#define FLOW_VALUE_COUNT 24
typedef struct GameFlow {
    // Counted up once per pass of the main loop (main); never read.
    u32 mLoopFrames;
    u32 mState;
    u32 mPrevState;
    u32 mNextState;
    // 1 makes the main loop leave and enter the state again even when it does not change. Only PopStateEx writes it
    // from a parameter, and its only caller passes 0, so it is never 1 in this build.
    s32 mReenterRequested;
    u32 mQueueCount;
    u8 mQueue[QUEUE_SIZE];
    u32 mValues[FLOW_VALUE_COUNT];
    s32 mStateFlag;
} GameFlow;

#define STATE_COUNT 25

typedef void (*StateFunc)(void);

extern GameFlow gGameFlow;

extern const u8 gStatePromptScreenEnabled[STATE_COUNT];
extern const StateFunc gStateEnterFuncs[STATE_COUNT];
extern const StateFunc gStateUpdateFuncs[STATE_COUNT];
extern const StateFunc gStateLeaveFuncs[STATE_COUNT];

typedef struct Unk_020b10fc {
    s32 mSceneState;
    s32 mMode;
    // mInfo.mGrade - 1 of the selected stage (checked 0..11), set when a mode starts; the row of the mode's value
    // tables.
    s32 mTableRow;
    s32 mUnk_0c;
    u32 mScore;
    s32 mBestScores[5];
} Unk_020b10fc;
extern Unk_020b10fc data_020b10fc;

// Language and three more settings words (the language is saved under the key "LANGUAGE"). mUnk_04 and mUnk_08 are
// the mic settings: mUnk_04 is SetupMic's second argument; mUnk_08 (at most 7) sets the level func_020213e8 tests
// the mic against, (8 - mUnk_08) * 25, and picks the entries of data_0204ff50 and data_0204ff70.
typedef struct Unk_020b1084 {
    u32 mLanguage;
    u32 mUnk_04;
    u32 mUnk_08;
    s32 mSaveSlot;
} Unk_020b1084;
extern Unk_020b1084 data_020b1084;

// An entry of data_020b6ee0.mUnk_9024 (mUnk_a149 counts the used ones): a model, a position and a kind. Used only by
// func_020196b4..func_020198f0 in stage_camera.c; func_02019800 removes the kind-1 entry the selected stage comes
// within range of.
typedef struct Unk_020bff04 {
    Model mModel;
    VecFx32 mPos;
    // 0 = free. Kind 1 gets a model and is drawn; only kind 1 is ever added in this build.
    s32 mKind;
} Unk_020bff04;

typedef struct Unk_020c1454 {
    VecFx32 mUnk_00;
    VecFx32 mUnk_0c;
    VecFx32 mDragStartPos;
    s32 mTouchPressX;
    s32 mTouchPressY;
    // Touch position on the previous frame of the gauge drag in panel state 8 (UpdatePanel).
    s32 mPrevTouchX;
    s32 mPrevTouchY;
    s32 mCameraEaseDivisor;
    // x offset of the map menu's buttons, added for 0x101 and subtracted for 0x102; set to 0x28, then eased to 0.
    s32 mMapButtonOffsetX;
    s32 mPanelButtonOffsetX;
    s32 mTextButtonOffsetY;
} Unk_020c1454;

// The stage's character: its model, the draw parameters and the countdown (time left, then the count).
typedef struct Unk_020bfd40 {
    Model mModel;
    DrawParams mDraw;
    // Frames left: func_020186fc counts it down in state 4 and calls StartModeSlide(0, ...) at 0. Also the time
    // DrawModeScoreAndTime draws; AddMapObjectTimeOrScore adds its signed time amounts to it.
    s32 mEndTimer;
    u32 mScore;
} Unk_020bfd40;

// Map menu (state 6) state: the cursor's draw parameters and its per-frame x/z move from the touch drag, the model
// drawn at the reached node, two sprites started when the cursor snaps to a node, the blend of the camera's second
// point and its target, the delay before the cursor shows, and the cursor-snapped and node-info-hidden flags.
typedef struct MapMenu {
    DrawParams mCursor;
    s32 mCursorXPerFrame;
    u8 mUnk_48[4];
    s32 mCursorZPerFrame;
    Model mReachedNodeModel;
    Sprite mUnk_1c8;
    Sprite mUnk_2cc;
    // Weight out of 0x200 of the camera's second point against the first (0x200 keeps the second point, 0x100 is
    // halfway); UpdateMapCursorAndCamera eases it toward mTargetBlend.
    s32 mBlend;
    // 0x100 once the cursor snaps to a node; 0x200 at set-up and when button 0x101 or 0x102 hides the node info.
    s32 mTargetBlend;
    // 0xb4 at set-up and counted down while the cursor sprites are hidden; snapping to a node also waits for 0. A
    // touch press off buttons 0x101 and 0x102 zeroes it.
    s32 mDelayTimer;
    // Set when the cursor is moved onto the selected node; 0 while no node is selected or mDelayTimer is not 0.
    u8 mCursorSnapped;
    // 0 once the cursor snaps to a node (the node info and buttons 0x101 and 0x102 are drawn); 1 at set-up and after
    // button 0x101 or 0x102. While 1 the touch drag can move the cursor and only button 0x102 is drawn, which then
    // leaves the menu.
    u8 mNodeInfoHidden;
    u8 mUnk_3de[2];
} MapMenu;

// Bits of data_020b6ee0.mBattleSlotFlags[slot], set by the battle screen (battle_screen.c) for the three stage slots:
// GRADE_UP when the slot's total was not lower and its grade below the other side's (the grade is raised by one
// afterwards), NOT_PICKED until the slot's stage is picked into a compare, KEPT when its record stayed in at its last
// compare that set a record out. STAGE_INFOS_OUT is used in entry 0 only: set when every gStageInfos record is out at
// the end, cleared when every slot copy is out.
#define BATTLE_SLOT_GRADE_UP 1
#define BATTLE_SLOT_NOT_PICKED 2
#define BATTLE_SLOT_KEPT 4
#define BATTLE_STAGE_INFOS_OUT 0x80

// Large block of game state shared by several screens; only the members the code touches are named.
typedef struct Unk_020b6ee0 {
    u16 mUnk_0000;
    u16 mUnk_0002;
    u8 mUnk_0004;
    u8 mUnk_0005[3];
    Unk_020b6ee8 mUnk_0008[8];
    u8 mUnk_0c48;
    u8 mUnk_0c49[3];
    u16 *mUnk_0c4c;
    s32 mFixedTargetX;
    s32 mFixedTargetZ;
    u8 mFixedTargetObjectIndex;
    u8 mUnk_0c59;
    u16 mFixedTargetTimer;
    // Touch x on the previous frame of UpdateStageMotion's touch drag, a path no call reaches in this build.
    u8 mPrevTouchX;
    u8 mUnk_0c5d;
    u8 mUnk_0c5e[2];
    // Stage index per slot of the slot screen (state 9); -1 = empty.
    s32 mSlotStages[3];
    s32 mUnk_c6c[3];
    // 1 when there are more than 3 stages (SetupSlotScreen). On the slot screen it gates the unslotted stage, button
    // 0x200, the slot cursor and buttons 0x109 and 0x110; it also picks image 0x10ce or 0x10cd, the offset of the slot
    // row and the highlight of the selected slot.
    u8 mUnslottedVisible;
    u8 mSelectedSlotIndex;
    u8 mUnk_c7a[2];
    VecFx32 mUnk_c7c;
    VecFx32 mUnk_c88;
    s32 mUnk_c94;
    s32 mUnk_c98;
    s32 mUnk_c9c;
    u16 mUnk_ca0[3];
    u16 mUnk_ca6;
    u16 mUnk_ca8;
    u8 mUnk_caa[2];
    Model mModel[2];
    // First member of a 0x44-byte block (up to mUnk_fdf) laid out as DrawParams: the battle models' draw parameters.
    VecFx32 mModelPos;
    u8 mUnk_fa8[0xfc8 - 0xfa8];
    VecFx32 mUnk_fc8;
    u8 mUnk_fd4[0xfdf - 0xfd4];
    u8 mUnk_fdf;
    u8 mBattleState;
    u8 mUnk_fe1;
    u16 mUnk_fe2;
    u16 mUnk_fe4;
    // One per row of the stage pick, but the records of both rows appear by [0]. The stage pick clears it together
    // with mUnk_fea and mUnk_fec (8 bytes).
    u16 mSideListLevels[2];
    u16 mUnk_fea;
    u16 mUnk_fec;
    u16 mUnk_fee;
    u16 mUnk_ff0;
    u16 mUnk_ff2;
    // Counted by IsEntryPickDone while side 1's pick is awaited (the pick is done once it passes 90); reset after each
    // turn.
    u16 mEntryFrames;
    u16 mUnk_ff6;
    u16 mUnk_ff8;
    u16 mEntryStartStep;
    u16 mEntryPanelXLevel;
    u16 mEntryPanelYLevel;
    s32 mUnk_1000[3];
    s16 mUnk_100c;
    s16 mUnk_100e[3];
    Unk_020b7ef4 mUnk_1014[48];
    u8 mUnk_5a54;
    u8 mUnk_5a55[3];
    Unk_020bc938 mUnk_5a58[32];
    u8 mUnk_8ad8;
    u8 mUnk_8ad9;
    u8 mUnk_8ada[2];
    FollowerEntry mFollowerEntries[32];
    u8 mFollowerEntryCount;
    u8 mUnk_8e5d[3];
    Unk_020bfd40 mUnk_8e60;
    Unk_020bff04 mUnk_9024[8];
    Model mUnk_9c64;
    s32 mUnk_9ddc[3];
    MoveObj mMoveObj;
    s32 mUnk_a0fc;
    s32 mUnk_a100;
    s32 mUnk_a104;
    s32 mUnk_a108;
    s32 mUnk_a10c;
    s32 mUnk_a110;
    s32 mUnk_a114;
    s32 mUnk_a118;
    s32 mUnk_a11c;
    s32 mTextImageIndex;
    s32 mSavedTextImageIndex;
    s32 mTextId;
    s32 mSavedTextId;
    s32 mTextOffsetY;
    s32 mTextTimer;
    s32 mSavedTextTimer;
    u8 mUnk_a13c;
    u8 mUnk_a13d;
    u8 mUnk_a13e;
    u8 mUnk_a13f;
    u8 mUnk_a140;
    u8 mUnk_a141;
    u8 mUnk_a142;
    u8 mUnk_a143;
    u8 mUnk_a144;
    s8 mUnk_a145;
    u8 mPanelState;
    u8 mUnk_a147;
    u8 mUnk_a148;
    u8 mUnk_a149;
    u8 mUnk_a14a;
    u8 mUnk_a14b;
    u8 mSaveRequested;
    u8 mUnk_a14d;
    u8 mUnk_a14e[2];
    MapMenu mMapMenu;
    s32 mReachedNodeIndex;
    s32 mSelectedNodeIndex;
    s32 mNodeState;
    u32 mNodeStageCount;
    s32 mNodeStages[8];
    // Set to 1 instead of raising mReachedNodeIndex past node 24 (stage_minigame.c); IsReachedNodeSelected returns 0
    // once it is set. Kept in the save record.
    s32 mLastNodeDone;
    s32 mProgressState;
    s32 mUnk_a568;
    u8 mUnk_a56c;
    u8 mUnk_a56d;
    u8 mUnk_a56e;
    u8 mUnk_a56f;
    // Set to 1 by func_02019800 when it removes a kind-1 mUnk_9024 entry and cleared only by ResetProgress;
    // mProgressState 3 waits for it in state 10.
    u8 mUnk_a570;
    u8 mUnk_a571[3];
    Unk_020c1454 mUnk_a574;
    u8 mUnk_a5b8[0xa5d0 - 0xa5b8];
    // 0 from a touch press that hit none of the screen's buttons until a frame with neither touch down nor release;
    // TestButton reports no button while it is 0. The camera drag of panel state 0 (state 10) and the map menu's
    // cursor drag move only while it is 0.
    u8 mButtonsEnabled;
    u8 mUnk_a5d1[3];
    Sprite mSelectionSprite;
    Sprite mSideSprites[2];
    s32 mPrevSelectedStageIndex;
    s32 mSelectedStageIndex;
    s32 mUnk_a8e8;
    u16 mPanelLevel;
    // StageRec records in gStages.
    u8 mStageCount;
    // StageInfo records in gStageInfos (at most 3).
    u8 mStageInfoCount;
    u8 mUnk_a8f0;
    u8 mPanelRow;
    u8 mMapIndex;
    // Only [0..2] belong to the stage slots (the battle set-up writes all 39); [0] also holds BATTLE_STAGE_INFOS_OUT.
    u8 mBattleSlotFlags[39];
    u8 mUnk_a91a;
    u8 mUnk_a91b;
} Unk_020b6ee0;
extern Unk_020b6ee0 data_020b6ee0;

u16 LookupLanguageFileId(int index);
const char *GetText(int id);
const char *GetLanguageText(int id, int index);
void InitGameFlow(void);
void ClearStateQueueAndFlag(void);
void ReenterState(void);
void PushState(int state);
void PopState(void);
void PopStateEx(u32 flag);
u32 GetState(void);
u32 GetNextState(void);
void SetFlowValue(u32 index, u32 value);
u32 GetFlowValue(u32 index);
void SetStateFlag(s32 value);
s32 GetStateFlag(void);

#ifdef __cplusplus
}
#endif
