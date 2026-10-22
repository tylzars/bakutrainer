#pragma once

#include "types.h"
#include <Main/Minigame/falling_blocks.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ResultState {
    u16 mState;
    u8 mPad_02[2];
    s32 mPendingState;
    // Frames in the current state. In RESULT_STATE_MODEL it counts only once the shared model is done; in
    // RESULT_STATE_SCORE it skips the frame after one on which an amount moved (mStateFramesEnabled).
    u32 mStateFrames;
    // Cleared on a frame on which an amount moves, so mStateFrames does not advance on the next frame.
    s32 mStateFramesEnabled;
    s32 mStageIndex;
    // Amount still to add to the stage's mValue, in 1/256 units (only while data_020b10fc.mUnk_0c is 1): from frame 54
    // it falls by a sixtieth of mValueGainTotal per frame and the whole units go to AddValue; 0 once mValue
    // reaches its upper bound.
    s32 mValueGain;
    // Start amount (file 488, value 5), never changed.
    s32 mValueGainTotal;
    // Moved like mValueGain (start amount file 488, value 7), from frame 108 and only once mNewBestSet is set.
    s32 mNewBestValueGain;
    s32 mNewBestValueGainTotal;
    // Set from frame 90 when the score is above the mode's best score, which is then replaced by it.
    s32 mNewBestSet;
    // Amount added to the stage's mPercent, in 1/256 units (file 488, value 9; only while data_020b10fc.mUnk_0c is
    // 0). Only a negative value moves (from frame 36, 128 per frame toward 0) and button 0x101 appears only at 0, so a
    // positive value would neither move nor show it.
    s32 mPercentChange;
    // Button 0x101 is added and drawn while set; it starts RESULT_STATE_TABLE_ENTRY, or leaves the screen
    // (func_0200494c) when no table entry was picked.
    s32 mNextButtonVisible;
    // -1: none (always while data_020b10fc.mUnk_0c is not 1).
    s32 mPickedTableEntryId;
    // Set by button 0x200 (the picked id replaces an entry) or 0x201 (the entries stay), cleared by 0x102.
    s32 mEntryDecided;
    s32 mPrevEntryDecided;
    // 0..4: one of the stage's five entries (DrawTableEntryRowAndHandleButtons); 5: the picked table entry (button
    // 0x505).
    s32 mEntryCursor;
    // 0..4 into the stage's mTableEntryIds (buttons 0x500-0x504): button 0x200 writes the picked id there and 0x102
    // puts mReplacedTableEntryId back.
    s32 mReplaceEntryIndex;
    s32 mReplacedTableEntryId;
    // In 1/256 pixel; entries 0-4 are used ([2] is a y offset, the others x) and 5-7 are never read.
    s32 mSlideOffsets[8];
    // mUnk_68 and mUnk_16c are started at set-up (gSpriteScripts[3] and [4]) and played in RESULT_STATE_TABLE_ENTRY,
    // mUnk_16c only once mUnk_68's mUnk_de is set.
    Sprite mUnk_68;
    Sprite mUnk_16c;
} ResultState;

// The parts below one unit of the three amounts the result screen moves, kept between frames and visits (nothing
// resets them), and mEntryReplaced. Nothing uses mUnk_0c.
typedef struct ResultRemainder {
    u32 mNewBestValueGainRemainder;
    u32 mValueGainRemainder;
    u32 mPercentChangeRemainder;
    s32 mUnk_0c;
    // Set by button 0x200 (an entry was replaced), cleared by 0x201; the 0x102 undo restores the entry only when set.
    s32 mEntryReplaced;
} ResultRemainder;

extern ResultState gResultState;
extern ResultRemainder gResultRemainder;

void func_0200494c(void);
void SetupResultScreen(void);
void DrawResultScreenAndHandleButtons(void);
void UpdateResultScreen(void);
void LeaveResultScreen(void);

#ifdef __cplusplus
}
#endif
