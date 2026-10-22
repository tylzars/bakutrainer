#pragma once

#include "types.h"
#include <Main/Unknown/move_object.h>

#ifdef __cplusplus
extern "C" {
#endif

// A save slot's written flag and node progress, copied from the slot's record by SetupTitleScreen (the node values only
// when the slot is written).
typedef struct SaveSlotProgress {
    s32 mWritten;
    u32 mReachedNodeIndex;
    s32 mLastNodeDone;
} SaveSlotProgress;

// Values of TitleScreen.mSlideState (UpdateTitleSlide): the title state's sprites and buttons move in from their
// offsets, stay in place (touch buttons added), move out.
#define TITLE_SLIDE_STATE_IN 0
#define TITLE_SLIDE_STATE_IN_PLACE 1
#define TITLE_SLIDE_STATE_OUT 2

// The title screen (game state 1). mUnk_1e0 is the offset of the opening sprite pair 0xdfa / 0xdfb: 0x10000 at set-up,
// 0x1900 less each frame, -1 once the two meet. mUnk_1e4 is 0 while that opening and the shake after it run and 1
// afterwards (at once when the state is entered with a return state); the slide and the buttons run only at 1.
typedef struct TitleScreen {
    SaveSlotProgress mSaveSlots[3];
    u32 mState;
    s32 mNextState;
    s32 mReadSelected;
    s32 mAnySaveWritten;
    // Set to 1 instead of 0x4b0 at set-up when data_020b6ee0.mUnk_a91a is set and there is no return state, so the
    // intro runs after one idle frame.
    s32 mIntroTimer;
    s8 mReturnState;
    u8 mUnk_39[3];
    Model mModel;
    // Copied from mLanguage when state 4 is requested and set by its language buttons. State 4 is never reached in this
    // build: only state 5's button 0x702 requests it, and no button 0x702 is ever added.
    u32 mSelectedLanguage;
    u8 mSlideState;
    u8 mUnk_1b9[3];
    // In 1/256 pixel, mostly x; the header sprite 0xe22 moves in y by [3] (by [0] in state 4, which is never reached).
    s32 mSlideOffsets[8];
    // -1 or 1 (RequestTitleState's b): the direction of the slide-out, and negated, the side the next state's offsets
    // start from. State 1's slide-out ignores it.
    s32 mSlideDirection;
    s32 mUnk_1e0;
    s32 mUnk_1e4;
} TitleScreen;

extern TitleScreen gTitleScreen;

int UpdateTitleSlide(void);
void RequestTitleState(int a, int b);
void OpenMicEdit(void);
void CloseMicEdit(int a);
void ResetTitleReturnState(void);
void SetupTitleImageAndModel(void);
void SetupTitleScreen(void);
void UpdateTitleScreen(void);
void LeaveTitleScreen(void);

#ifdef __cplusplus
}
#endif
