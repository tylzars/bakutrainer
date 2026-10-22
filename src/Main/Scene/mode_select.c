#include <Main/System/random_angle.h>
#include <Main/Minigame/stage_minigame.h>
#include <Main/Scene/stage_info_panel.h>
#include <Main/Unknown/game_util.h>
#include <Main/Scene/stage_scene.h>
#include <Main/System/prompt_screen.h>
#include <Main/System/state_flow.h>
#include <Main/Scene/mode_select.h>
#include <Main/Scene/field_objects.h>
#include <Main/Graphics/text_draw.h>
#include <Main/Graphics/model.h>
#include <Main/Graphics/texture_blocks.h>
#include <Main/Sound/wave_player.h>
#include <Main/Graphics/script_data.h>
#include <Main/System/system_init.h>
#include <stdio.h>

ModeSelectState gModeSelectState;
s32 gKeepSelectedMode;

static const u32 sThresholdNotMetWaveIds[2] = {0xe9, 0xf0};
static const u32 sThresholdMetWaveIds[3] = {0xf7, 0xfe, 0x105};
static const s32 sModeStates[5] = {14, 15, 16, 17, 4};
static const s32 sModePrompts[5] = {41, 40, 42, 43, 44};
static const s32 sModeButtonIds[5] = {0x301, 0x300, 0x302, 0x304, 0x303};

u32 gPrevTimeLeft = 1800;

// Selects mode 'mode' and stores its score threshold for the selected stage's grade (row grade - 1 of the mode's
// table; 0 for a mode outside 0..4) and its best score.
void SelectMode(int mode) {
    int stageIndex;
    u8 tableRow;
    stageIndex = data_020b6ee0.mSelectedStageIndex;
    if (stageIndex < 0) {
        stageIndex = 0;
    }
    tableRow = gStages[stageIndex].mInfo.mGrade - 1;
    data_020b10fc.mMode = mode;
    switch (mode) {
        case 0:
            gModeSelectState.mScoreThreshold = func_0200d6b0(tableRow);
            break;
        case 1:
            gModeSelectState.mScoreThreshold = func_0200b88c(tableRow);
            break;
        case 2:
            gModeSelectState.mScoreThreshold = GetFieldScoreThreshold(tableRow);
            break;
        case 3:
            gModeSelectState.mScoreThreshold = func_02004940(tableRow);
            break;
        case 4:
            gModeSelectState.mScoreThreshold = func_020186f0(tableRow);
            break;
        default:
            gModeSelectState.mScoreThreshold = 0;
            break;
    }
    gModeSelectState.mBestScore = data_020b10fc.mBestScores[data_020b10fc.mMode];
}

void SetupModeSelect(void) {
    func_0200c5d8(0, 0x10d1, gStages[data_020b6ee0.mSelectedStageIndex].mInfo.mStageId + 0xdb3, 32, 16, 130, 2);
    func_02024208();
    if (gKeepSelectedMode == 0) {
        SelectMode(4);
    } else {
        gKeepSelectedMode = 0;
    }
}

void DrawModeSelect(void) {
    char text[32];
    StageRec *rec = &gStages[data_020b6ee0.mSelectedStageIndex];
    DrawStageInfo(data_020b6ee0.mSelectedStageIndex, 2);
    DrawText(48, 17, data_0201a61c, GetText(26), 0, 200, 16, 1);
    func_0200bb14(255, 161, 161 + 96, 21, 0, 0, 0x17d, 1);
    func_0200bb14(0, 161, 161 + 97, 14, 0, 0, 90, 1);
    func_0200bb58(67, 61, 0, 0, 140, 0x303, sModeButtonIds[data_020b10fc.mMode], 0x11dc, 0, 0, 140 + 219, 1, 0, 0, 0);
    if (rec->mInfo.mGrade > 1) {
        func_0200bb58(29, 84, 0, 0, 140, 0x300, sModeButtonIds[data_020b10fc.mMode], 0x11d8, 0, 0, 140 + 219, 1, 0, 0,
                      0);
    }
    if (rec->mInfo.mGrade > 2) {
        func_0200bb58(67, 106, 0, 0, 140, 0x301, sModeButtonIds[data_020b10fc.mMode], 0x11d4, 0, 0, 140 + 219, 1, 0, 0,
                      0);
    }
    if (rec->mInfo.mGrade > 3) {
        func_0200bb58(29, 129, 0, 0, 140, 0x302, sModeButtonIds[data_020b10fc.mMode], 0x11da, 0, 0, 140 + 219, 1, 0, 0,
                      0);
    }
    if (rec->mInfo.mGrade > 4) {
        func_0200bb58(67, 152, 0, 0, 140, 0x304, sModeButtonIds[data_020b10fc.mMode], 0x11d6, 0, 0, 140 + 219, 1, 0, 0,
                      0);
    }
    DrawText(99, 108, data_0201a64e, GetText(data_020b10fc.mMode + 31), 0, 200, 16, 1);
    DrawText(106, 129, data_0201a61c, GetText(28), 0, 200, 16, 1);
    sprintf(text, "%d", gModeSelectState.mScoreThreshold);
    DrawText(215, 129, data_0201a61c, text, 0, 200, 16, 1);
    DrawText(106, 144, data_0201a61c, GetText(29), 0, 200, 16, 1);
    sprintf(text, "%d", gModeSelectState.mBestScore);
    DrawText(215, 144, data_0201a61c, text, 0, 200, 16, 1);
    func_0200bb14(153, 181, 50 << 4, 0x11de, 0, 0, 0x167, 1);
    DrawText(153, 181, data_0201a61c, GetText(30), 2, 200, 16, 1);
}

// Handles the screen's touch buttons, then draws it: 0x102 goes to game state 10, 0x300-0x304 select a mode, 0x101
// enters the selected mode's game state (through its prompt while its best score is 0), and 0x320 shows the mode's
// prompt with state 13 to follow, keeping the selected mode (gKeepSelectedMode).
void UpdateModeSelect(void) {
    int idx;
    if (func_0200bd80(0x20000000, 0x102)) {
        PushState(10);
        PopState();
    }
    if (func_0200bd80(0x20000000, 0x300)) {
        SelectMode(1);
    } else if (func_0200bd80(0x20000000, 0x301)) {
        SelectMode(0);
    } else if (func_0200bd80(0x20000000, 0x302)) {
        SelectMode(2);
    } else if (func_0200bd80(0x20000000, 0x304)) {
        SelectMode(3);
    } else if (func_0200bd80(0x20000000, 0x303)) {
        SelectMode(4);
    }
    if (func_0200bd80(0x20000000, 0x101)) {
        idx = data_020b10fc.mMode;
        if (data_020b10fc.mBestScores[idx] == 0) {
            func_0201033c(sModePrompts[idx], sModeStates[idx]);
        } else {
            PushState(sModeStates[idx]);
            PopState();
        }
    }
    if (func_0200bd80(0x20000000, 0x320)) {
        gKeepSelectedMode = 1;
        func_0201033c(sModePrompts[data_020b10fc.mMode], 13);
    }
    DrawModeSelect();
}

void LeaveModeSelect(void) {
    func_020102f8();
}

void StartResultScreen(int a, u32 b) {
    data_020b10fc.mUnk_0c = a;
    data_020b10fc.mScore = b;
    PushState(18);
    PopState();
}

void func_020059cc(u32 value) {}

// Adds 'add' to *value and limits the result to 999 << shift.
void AddCappedAt999Shifted(u32 *value, u32 add, int shift) {
    *value += add;
    *value = *value < (999 << shift) ? *value : (999 << shift);
}

void AddCappedAt5999(u32 *value, u32 add) {
    *value += add;
    *value = *value < 5999 ? *value : 5999;
}

// Stores the new time value; plays the countdown sound when it falls below one of the thresholds and returns 1 while
// the value is nonzero, at most 600 and the clock low bits are at most 8.
int UpdateTimeLeft(u32 value) {
    u32 prev = gPrevTimeLeft;
    if ((prev >= 600 && value < 600) || (prev >= 300 && value < 300) || (prev >= 240 && value < 240) ||
        (prev >= 180 && value < 180) || (prev >= 120 && value < 120) || (prev >= 60 && value < 60)) {
        func_0202f110(361, 100, 0, 64, 0xffff, 0);
    }
    gPrevTimeLeft = value;
    if (value <= 600 && value != 0 && (gSystemState.mElapsedFrames & 0x1f) <= 8) {
        return 1;
    }
    return 0;
}

void StartModeSlide(int a, int b) {
    gModeSelectState.mSlideState = a == 1 ? MODE_SLIDE_STATE_START : MODE_SLIDE_STATE_END;
    gModeSelectState.mSlideX = 6 << 14;
    gModeSelectState.mSlideHoldTimer = 120;
    gModeSelectState.mScoreThresholdMet = b;
    if (a == 0) {
        if (b == 1) {
            func_0202f110(data_020b1084.mLanguage + sThresholdMetWaveIds[NextRandom() % 3], 100, 0, 64, 0xffff, 0);
        } else if (b == 0) {
            func_0202f110(data_020b1084.mLanguage + sThresholdNotMetWaveIds[NextRandom() & 1], 100, 0, 64, 0xffff, 0);
        }
    }
}

int UpdateModeSlide(int mode) {
    s32 v;
    switch (gModeSelectState.mSlideState) {
        case MODE_SLIDE_STATE_START:
            if (gModeSelectState.mSlideHoldTimer != 0) {
                v = gModeSelectState.mSlideX - (30 << 8);
                if (v < 0) {
                    v = 0;
                }
                gModeSelectState.mSlideX = v;
                if (v == 0) {
                    gModeSelectState.mSlideHoldTimer--;
                }
            } else {
                v = gModeSelectState.mSlideX - (30 << 8);
                if (v < -65536) {
                    v = -65536;
                }
                gModeSelectState.mSlideX = v;
                if (v == -65536) {
                    gModeSelectState.mSlideState = MODE_SLIDE_STATE_NONE;
                    return 0;
                }
            }
            DrawSprite(data_020b1084.mLanguage + 0xe77, (gModeSelectState.mSlideX >> 8) + 128, 96, 10, mode);
            return 1;
        case MODE_SLIDE_STATE_END: {
            int id = gModeSelectState.mScoreThresholdMet == 1 ? 0xec5 : 0xe7d;
            if (gModeSelectState.mSlideHoldTimer != 0) {
                v = gModeSelectState.mSlideX - (30 << 8);
                if (v < 0) {
                    v = 0;
                }
                gModeSelectState.mSlideX = v;
                if (v == 0) {
                    gModeSelectState.mSlideHoldTimer--;
                }
            } else {
                return 0;
            }
            DrawSprite(id + data_020b1084.mLanguage, (gModeSelectState.mSlideX >> 8) + 128, 96, 10, mode);
            return 1;
        }
        default:
            return 0;
    }
}

u8 GetModeSlideState(void) {
    return gModeSelectState.mSlideState;
}

s32 GetScoreThresholdMet(void) {
    return gModeSelectState.mScoreThresholdMet;
}
