#include <Main/System/screen_fade.h>
#include <Main/Scene/stage_progress.h>
#include <Main/Scene/stage_info_panel.h>
#include <Main/Unknown/game_util.h>
#include <Main/Unknown/move_object.h>
#include <Main/System/prompt_screen.h>
#include <Main/System/sound_control.h>
#include <Main/Scene/language_menu.h>
#include <Main/System/state_flow.h>
#include <Main/Scene/result_screen.h>
#include <Main/Scene/mode_select.h>
#include <Main/Graphics/text_draw.h>
#include <Main/Graphics/model.h>
#include <Main/Graphics/texture_blocks.h>
#include <Main/Graphics/sprite_script.h>
#include <Main/Graphics/script_data.h>
#include <Main/Input/touch_buttons.h>
#include <Main/System/system_init.h>
#include <stdio.h>
#include <stdlib.h>

// Values of gResultState.mState (and of mPendingState, the state it moves to after a fade): the shared model plays,
// the score and the amounts are shown, the picked table entry is offered.
#define RESULT_STATE_MODEL 0
#define RESULT_STATE_SCORE 1
#define RESULT_STATE_TABLE_ENTRY 2

static const u32 sResultSharedModelIndices[2] = {11, 0};
static const u32 sModeSpriteIds[5] = {0x11d4, 0x11d8, 0x11da, 0x11d6, 0x11dc};

ResultRemainder gResultRemainder;
ResultState gResultState;

// Runs func_02014bf0 (every stage gets a new random mUnk_1a, and 10 percent more when it is not in a slot), saves
// (RunSaveScreen(0)) and goes to game state 10. Called by button 0x101 in RESULT_STATE_SCORE when no table entry was
// picked, and in RESULT_STATE_TABLE_ENTRY.
void func_0200494c(void) {
    func_02014bf0();
    RunSaveScreen(0);
    PushState(10);
    PopState();
}

void SetupResultScreen(void) {
    int i;
    int imageId;
    int stageIndex;
    s32 v;
    imageId = data_020b10fc.mUnk_0c == 1 ? 0x1d1 : 0x1ca;
    stageIndex = data_020b6ee0.mSelectedStageIndex;
    if (stageIndex < 0) {
        stageIndex = 0;
    }
    func_0200c5d8(0, imageId, gStages[stageIndex].mInfo.mStageId + 0xdb3, 256, 16, 1, 2);
    func_02024208();
    gResultState.mState = RESULT_STATE_MODEL;
    gResultState.mPendingState = -1;
    gResultState.mStateFrames = 0;
    gResultState.mStateFramesEnabled = 1;
    gResultState.mStageIndex = stageIndex;
    v = GetFileValue(488, 5) << 8;
    gResultState.mValueGain = v;
    gResultState.mValueGainTotal = (v << 8) >> 8;
    v = GetFileValue(488, 7) << 8;
    gResultState.mNewBestValueGain = v;
    gResultState.mNewBestValueGainTotal = (v << 8) >> 8;
    gResultState.mNewBestSet = 0;
    gResultState.mPercentChange = GetFileValue(488, 9) << 8;
    gResultState.mNextButtonVisible = 0;
    gResultState.mPickedTableEntryId =
        data_020b10fc.mUnk_0c == 1 ? PickWeightedTableEntry(&gStages[gResultState.mStageIndex].mInfo) : -1;
    gResultState.mEntryDecided = 0;
    gResultState.mPrevEntryDecided = 0;
    gResultState.mEntryCursor = 5;
    gResultState.mReplaceEntryIndex = 0;
    for (i = 0; i < 8; i++) {
        gResultState.mSlideOffsets[i] = 0;
    }
    SetupSharedModel(sResultSharedModelIndices[data_020b10fc.mUnk_0c], 0);
    StartSprite(&gResultState.mUnk_68, data_0201abec[3]);
    StartSprite(&gResultState.mUnk_16c, data_0201abec[4]);
    RequestTrack(data_020b10fc.mUnk_0c == 1 ? 7 : 8, 102);
}

void DrawResultScreenAndHandleButtons(void) {
    char text0[16];
    char text1[16];
    char text2[16];
    char text3[16];
    char text4[16];
    char text5[16];
    char text6[16];
    StageRec *rec;
    int ready;
    int tableEntryId;
    s32 v;
    if (gResultState.mState != RESULT_STATE_TABLE_ENTRY) {
        DrawStageInfo(gResultState.mStageIndex, 2);
    } else {
        s32 t = gResultState.mEntryCursor;
        if (t < 0 || (t == 5 && gResultState.mPrevEntryDecided != 0) ||
            (t >= 0 && t < 5 && gStages[gResultState.mStageIndex].mInfo.mTableEntryIds[t] < 0)) {
            DrawStageInfo(gResultState.mStageIndex, 1);
        }
    }
    if (gResultState.mState == RESULT_STATE_MODEL) {
        DrawSharedModel();
        return;
    }
    if (gResultState.mState == RESULT_STATE_SCORE) {
        DrawText(51, 21, data_0201a61c, GetText(data_020b10fc.mMode + 31), 0, 200, 16, 1);
        DrawSprite(sModeSpriteIds[data_020b10fc.mMode], 25, 22, 130, 1);
        switch (data_020b10fc.mUnk_0c) {
            case 1:
                DrawText(89, 66, data_0201a64e, GetText(44), 2, 200, 16, 1);
                if (gResultState.mStateFrames >= 18) {
                    sprintf(text0, "%d", data_020b10fc.mScore);
                    DrawText(35, 128, data_0201a64e, GetText(43), 0, 200, 16, 1);
                    DrawText(180, 128, data_0201a64e, text0, 1, 200, 16, 1);
                }
                if (gResultState.mStateFrames >= 36 && gResultState.mValueGain > 0) {
                    sprintf(text1, "+G %d", gResultState.mValueGain >> 8);
                    DrawText(140, 143, data_0201a61c, text1, 0, 200, 16, 1);
                }
                if (gResultState.mStateFrames >= 72) {
                    sprintf(text2, "%d", data_020b10fc.mBestScores[data_020b10fc.mMode]);
                    DrawText(35, 148, data_0201a64e, GetText(46), 0, 200, 16, 1);
                    DrawText(180, 148, data_0201a64e, text2, 1, 200, 16, 1);
                }
                if (gResultState.mNewBestSet != 0 && gResultState.mNewBestValueGain > 0) {
                    sprintf(text3, "+G %d", gResultState.mNewBestValueGain >> 8);
                    DrawText(140, 163, data_0201a61c, text3, 0, 200, 16, 1);
                }
                break;
            case 0:
                DrawText(89, 66, data_0201a64e, GetText(45), 2, 200, 16, 1);
                sprintf(text4, "%d", data_020b10fc.mScore);
                DrawText(35, 128, data_0201a64e, GetText(43), 0, 200, 16, 1);
                DrawText(180, 128, data_0201a64e, text4, 1, 200, 16, 1);
                sprintf(text5, "%d", data_020b10fc.mBestScores[data_020b10fc.mMode]);
                DrawText(35, 148, data_0201a64e, GetText(46), 0, 200, 16, 1);
                DrawText(180, 148, data_0201a64e, text5, 1, 200, 16, 1);
                if (gResultState.mStateFrames >= 18) {
                    if (gResultState.mPercentChange < 0) {
                        sprintf(text6, "%s %d", GetText(10), gResultState.mPercentChange >> 8);
                        DrawText(70, 163, data_0201a61c, text6, 0, 200, 16, 1);
                    }
                }
                break;
        }
        if (gResultState.mNextButtonVisible != 0) {
            func_0200bb14(255, 161, 161 + 96, 21, 0, 0, 0x17d, 1);
        }
        return;
    }
    if (gResultState.mState != RESULT_STATE_TABLE_ENTRY) {
        return;
    }
    rec = &gStages[gResultState.mStageIndex];
    tableEntryId = gResultState.mPickedTableEntryId;
    ready = 0;
    if (IsFadeState(3, FADE_STATE_CLEAR)) {
        v = gResultState.mSlideOffsets[0] - (5 << 10);
        if (v < 0) {
            v = 0;
        }
        gResultState.mSlideOffsets[0] = v;
        v = gResultState.mSlideOffsets[1] + (5 << 10);
        if (v >= 0) {
            v = 0;
        }
        gResultState.mSlideOffsets[1] = v;
        v = gResultState.mSlideOffsets[2] + (5 << 10);
        if (v >= 0) {
            v = 0;
        }
        gResultState.mSlideOffsets[2] = v;
        v = gResultState.mSlideOffsets[3] + (5 << 10);
        if (v >= 0) {
            v = 0;
        }
        gResultState.mSlideOffsets[3] = v;
        v = gResultState.mSlideOffsets[4] - (5 << 10);
        if (v < 0) {
            v = 0;
        }
        gResultState.mSlideOffsets[4] = v;
        if (gResultState.mSlideOffsets[0] == 0 && gResultState.mSlideOffsets[1] == 0 &&
            gResultState.mSlideOffsets[2] == 0 && gResultState.mSlideOffsets[3] == 0 && v == 0) {
            ready = 1;
        }
    }
    if (gResultState.mPrevEntryDecided == 1) {
        func_0200bb14(255, 161, 161 + 96, 21, 0, 0, 0x17d, 1);
    }
    if (gResultState.mEntryDecided == 1) {
        func_0200bb14(0, 161, 161 + 97, 14, 0, 0, 90, 1);
    }
    if (tableEntryId != -1 && gResultState.mEntryDecided == 0) {
        DrawText(70 + (gResultState.mSlideOffsets[0] >> 8), 37, data_0201a64e, GetText(52), 0, 256, 16, 2);
        if (gResultState.mSlideOffsets[0] == 0 && gResultState.mSlideOffsets[1] == 0) {
            UpdateSprite(&gResultState.mUnk_68);
            DrawSprite(gResultState.mUnk_68.mUnk_04, 57, 103, 150, 2);
        }
        func_02013d48(&rec->mInfo, tableEntryId, gResultState.mEntryCursor == 5, 0x505, 57,
                      (gResultState.mSlideOffsets[2] >> 8) + 103, 2, 0);
        if (gResultState.mEntryCursor == 5) {
            func_02013dc8(&rec->mInfo, tableEntryId, 1, 0);
        }
        if (func_0200bd80(0x80000000, 0x505) && ready) {
            gResultState.mEntryCursor = 5;
        }
        if (gResultState.mUnk_68.mUnk_de != 0) {
            UpdateSprite(&gResultState.mUnk_16c);
            DrawSprite(gResultState.mUnk_16c.mUnk_04, 0, 137, 130, 2);
            if (gResultState.mUnk_16c.mUnk_de != 0) {
                DrawSprite(0xfb4, gResultState.mReplaceEntryIndex * 26 + 116, 103, 141, 2);
            }
        }
        func_0200bc50((gResultState.mSlideOffsets[4] >> 8) + 181, 174, 0, 0, 140, 512, 0xfb5, GetText(41),
                      data_0201a61c, 2, 0, 359, ready, 0, 0, 0);
        if (func_0200bd80(0x20000000, 0x200) && ready) {
            gResultRemainder.mEntryReplaced = 1;
            gResultState.mReplacedTableEntryId = rec->mInfo.mTableEntryIds[gResultState.mReplaceEntryIndex];
            rec->mInfo.mTableEntryIds[gResultState.mReplaceEntryIndex] = tableEntryId;
            gResultState.mEntryDecided = 1;
        }
        func_0200bc50((gResultState.mSlideOffsets[3] >> 8) + 73, 174, 0, 0, 140, 513, 0xfd7, GetText(61), data_0201a61c,
                      2, 0, 90, ready, 0, 0, 0);
        if (func_0200bd80(0x20000000, 0x201) && ready) {
            gResultRemainder.mEntryReplaced = 0;
            gResultState.mEntryDecided = 1;
        }
        if (func_0200bdac(0x80000000, 0x500, 0x504) && ready) {
            gResultState.mReplaceEntryIndex = (u16) data_020c1d60.mUnk_800.mUnk_100.mUnk_04 - 0x500;
        }
    }
    if (func_0200bd80(0x20000000, 0x102)) {
        gResultState.mEntryDecided = 0;
        if (gResultRemainder.mEntryReplaced != 0) {
            rec->mInfo.mTableEntryIds[gResultState.mReplaceEntryIndex] = gResultState.mReplacedTableEntryId;
        }
    }
    DrawSprite(rec->mInfo.mStageId + 0x1089, (gResultState.mSlideOffsets[1] >> 8) + 40, 45, gSystemState.mUnk_308 - 4,
               2);
    DrawSprite(0x1088, (gResultState.mSlideOffsets[1] >> 8) + 40, 45, gSystemState.mUnk_308 - 2, 2);
    func_02013f74(gResultState.mStageIndex, &gResultState.mEntryCursor, 168, 103, 2, (ready == 1 ? 0 : 4) + 2);
    gResultState.mPrevEntryDecided = gResultState.mEntryDecided;
}

void UpdateResultScreen(void) {
    s32 old;
    s32 prev;
    u32 whole;
    if (gResultState.mPendingState >= 0) {
        if (gResultState.mPendingState == RESULT_STATE_TABLE_ENTRY) {
            if (IsFadeState(3, FADE_STATE_FADED)) {
                gResultState.mState = gResultState.mPendingState;
                gResultState.mPendingState = -1;
                func_020102f8();
                func_0200c5d8(1, gStages[gResultState.mStageIndex].mInfo.mStageId + 0xdb3, 456, 128, 16, 1, 130);
                gResultState.mSlideOffsets[0] = 1 << 16;
                gResultState.mSlideOffsets[1] = -0x8000;
                gResultState.mSlideOffsets[2] = -0x8000 << 2;
                gResultState.mSlideOffsets[3] = -0x48000;
                gResultState.mSlideOffsets[4] = 18 << 14;
                gSystemState.mFlags |= 2 << 16;
                StartFadeClear(3);
            }
        } else {
            if (IsFadeState(1, FADE_STATE_FADED)) {
                gResultState.mState = gResultState.mPendingState;
                gResultState.mPendingState = -1;
                gSystemState.mFlags |= 2 << 16;
                StartFadeClear(1);
            }
        }
    }
    if (gResultState.mState == RESULT_STATE_MODEL) {
        if (IsSharedModelDone()) {
            if (++gResultState.mStateFrames >= 60) {
                gResultState.mPendingState = RESULT_STATE_SCORE;
                gResultState.mStateFrames = 0;
                StartFade(1);
                gSystemState.mFlags &= ~(2 << 16);
            }
        }
    } else if (gResultState.mState == RESULT_STATE_SCORE) {
        if (gResultState.mStateFramesEnabled != 0) {
            gResultState.mStateFrames++;
        }
        gResultState.mStateFramesEnabled = 1;
        switch (data_020b10fc.mUnk_0c) {
            case 1:
                if (gResultState.mStateFrames >= 54 && gResultState.mValueGain > 0) {
                    old = gResultState.mValueGain;
                    gResultState.mStateFramesEnabled = 0;
                    prev = gResultState.mValueGain - gResultState.mValueGainTotal / 60;
                    gResultState.mValueGain = prev;
                    if (prev < 0) {
                        prev = 0;
                    }
                    gResultState.mValueGain = prev;
                    gResultRemainder.mValueGainRemainder += old - prev;
                    whole = gResultRemainder.mValueGainRemainder >> 8;
                    if (whole != 0) {
                        if (func_02014f98(&gStages[gResultState.mStageIndex], whole)) {
                            gResultState.mValueGain = 0;
                        }
                        gResultRemainder.mValueGainRemainder -= gResultRemainder.mValueGainRemainder & ~0xff;
                    }
                }
                if (gResultState.mStateFrames >= 72) {
                    if (data_020b10fc.mScore > data_020b10fc.mBestScores[data_020b10fc.mMode]) {
                        if (gResultState.mStateFrames >= 90) {
                            gResultState.mNewBestSet = 1;
                            data_020b10fc.mBestScores[data_020b10fc.mMode] = data_020b10fc.mScore;
                        }
                    } else if (gResultState.mNewBestSet == 0) {
                        gResultState.mNextButtonVisible = 1;
                    }
                }
                if (gResultState.mStateFrames >= 108 && gResultState.mNewBestSet != 0) {
                    old = gResultState.mNewBestValueGain;
                    if (old > 0) {
                        gResultState.mStateFramesEnabled = 0;
                        prev = gResultState.mNewBestValueGain - gResultState.mNewBestValueGainTotal / 60;
                        gResultState.mNewBestValueGain = prev;
                        if (prev < 0) {
                            prev = 0;
                        }
                        gResultState.mNewBestValueGain = prev;
                        gResultRemainder.mNewBestValueGainRemainder += old - prev;
                        whole = gResultRemainder.mNewBestValueGainRemainder >> 8;
                        if (whole != 0) {
                            if (func_02014f98(&gStages[gResultState.mStageIndex], whole)) {
                                gResultState.mNewBestValueGain = 0;
                            }
                            gResultRemainder.mNewBestValueGainRemainder -=
                                gResultRemainder.mNewBestValueGainRemainder & ~0xff;
                        }
                    } else {
                        gResultState.mNextButtonVisible = 1;
                    }
                }

                break;
            case 0:
                if (gResultState.mStateFrames >= 36 && gResultState.mPercentChange < 0) {
                    old = gResultState.mPercentChange;
                    gResultState.mStateFramesEnabled = 0;
                    gResultState.mPercentChange = old + 128;
                    if (gResultState.mPercentChange > 0) {
                        gResultState.mPercentChange = 0;
                    }
                    gResultRemainder.mPercentChangeRemainder += abs(old) - abs(gResultState.mPercentChange);
                    whole = gResultRemainder.mPercentChangeRemainder >> 8;
                    if (whole != 0) {
                        u32 left = gStages[gResultState.mStageIndex].mInfo.mPercent;
                        if (whole <= left) {
                            gStages[gResultState.mStageIndex].mInfo.mPercent = left - whole;
                        } else {
                            gStages[gResultState.mStageIndex].mInfo.mPercent = 0;
                            gResultState.mPercentChange = 0;
                        }
                        gResultRemainder.mPercentChangeRemainder -= gResultRemainder.mPercentChangeRemainder & ~0xff;
                    }
                }
                if (gResultState.mPercentChange == 0) {
                    gResultState.mNextButtonVisible = 1;
                }
                break;
        }
        if (func_0200bd80(0x20000000, 0x101)) {
            if (gResultState.mPickedTableEntryId < 0) {
                func_0200494c();
            } else {
                gResultState.mPendingState = RESULT_STATE_TABLE_ENTRY;
                gResultState.mStateFrames = 0;
                StartFade(3);
                gSystemState.mFlags &= ~(2 << 16);
            }
        }
    } else if (gResultState.mState == RESULT_STATE_TABLE_ENTRY) {
        if (func_0200bd80(0x20000000, 0x101)) {
            func_0200494c();
        }
    }
    DrawResultScreenAndHandleButtons();
}

void LeaveResultScreen(void) {
    func_020102f8();
    gSystemState.mFlags |= 2 << 16;
}
