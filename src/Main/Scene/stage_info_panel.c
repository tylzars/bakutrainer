#include <nitro/os.h>
#include <Main/clib.h>
#include <nitro/snd.h>
#include <stdio.h>
#include <Main/Input/touch_buttons.h>
#include <Main/Save/save_blocks.h>
#include <Main/Input/mic_input.h>
#include <Main/System/random_angle.h>
#include <Main/System/screen_fade.h>
#include <Main/System/game_math.h>
#include <Main/System/bg_image.h>
#include <Main/Scene/stage_progress.h>
#include <Main/System/sound_control.h>
#include <Main/System/save_data.h>
#include <Main/System/game_util.h>
#include <Main/Graphics/sprite_slots.h>
#include <Main/Scene/mode_select.h>
#include <Main/System/state_flow.h>
#include <Main/Scene/battle_screen.h>
#include <Main/System/prompt_screen.h>
#include <Main/Scene/language_menu.h>
#include <Main/Scene/stage_data.h>
#include <Main/Scene/stage_info_panel.h>
#include <Main/Scene/stage_map_menu.h>
#include <Main/Graphics/text_draw.h>
#include <Main/Graphics/model.h>
#include <Main/Map/map_entries.h>
#include <Main/Graphics/texture_blocks.h>
#include <Main/Sound/wave_player.h>
#include <Main/Graphics/sprite_script.h>
#include <Main/Map/map_floor.h>
#include <Main/Graphics/script_data.h>
#include <Main/Graphics/gx_pool.h>
#include <Main/System/system_init.h>

#define GAUGE_MAX 0x31
#define TABLE_ENTRY_COUNT 100

// Slot screen (state 9) state.
typedef struct Unk_020b13b0 {
    u8 mUnk_00;
    u8 mUnk_01;
    u8 mUnk_02;
    // Set when button 0x200 writes a slot; button 0x101 is drawn only after that.
    u8 mSlotChanged;
    // The stage shown outside the slots (-1 for none); used only while mUnslottedVisible is 1.
    s32 mUnslottedStageIndex;
    // Highlighted entry of the selected stage's row of five (0..4 into mInfo.mTableEntryIds), -1 for none.
    s32 mEntryCursor;
} Unk_020b13b0;

Unk_020b13b0 data_020b13b0;

// The three stage indices of mSlotStages as they were when the screen was entered; button 0x102 writes them back.
s32 gSavedSlotStages[3];

// Loads the image for screen `layer` unless that screen already shows it: in state 10 with stage -1 or a text set,
// image 0x1e4 + mTextImageIndex; otherwise the stage's image (0xdb3 + its stage id). A stage of -1 outside state 10
// reads gStages[-1]; UpdateSlotScreen passes -1 when no stage is selected.
void LoadStageOrTextImage(int stage, int layer) {
    u16 current;
    int id;

    if (layer == 1) {
        current = gSystemState.mImageIds[0];
    } else {
        current = gSystemState.mImageIds[1];
    }
    if (!(stage >= -1 && stage < STAGE_MAX)) {
        OS_Terminate();
    }
    data_020b6ee0.mUnk_a13c = 0;
    if (GetState() == 10 && (stage == -1 || data_020b6ee0.mTextId != -1)) {
        id = data_020b6ee0.mTextImageIndex + 0x1e4;
        func_0200bdcc(2, 8);
    } else {
        id = gStages[stage].mInfo.mStageId + 0xdb3;
    }
    if ((u16) id != current) {
        SetFaded(layer);
        LoadImage(id, layer);
    }
}

void FormatFrameTime(int frames, char *buf) {
    int t;

    if (frames < 0) {
        frames = 0;
    }
    t = frames * 1000 / 60;
    sprintf(buf, "%d%d:%d%d", t / 10000 % 10, t / 1000 % 10, t / 100 % 10, t / 10 % 10);
}

// Formats frames as whole seconds, rounded up, with a sign: "+n" when frames > 0, else "-n" (0 gives "-0").
void FormatSignedSeconds(int frames, char *buf) {
    int t;

    t = (frames >= 0 ? frames : -frames) * 1000 / 60;
    if (t % 1000 != 0) {
        t += 1000;
    }
    if (frames > 0) {
        sprintf(buf, "+%d", t / 1000);
    } else {
        sprintf(buf, "-%d", t / 1000);
    }
}

// Draws the panel shown while a mode runs: score and b (every caller passes gModeSelectState.mScoreThreshold), each as
// at least three digits, and, unless UpdateTimeRemaining(frames) is not 0, the time remaining from frames. stage is
// only range-checked (-1 counts as 0).
void func_0201168c(int stage, u32 score, int b, int frames, int d) {
    char time[12];
    char text[32];

    if (stage == -1) {
        stage = 0;
    }
    if (!(stage >= 0 && stage < STAGE_MAX)) {
        OS_Terminate();
    }
    DrawSprite(0xf38, 0xa3, 0x8b, 0xc8, d);
    DrawSprite(0xf39, 0x53, 0x8b, 0xc8, d);
    DrawSprite(0xf48, 0x15, 0x8e, 0xc8, d);
    DrawText(0x23, 0x95, data_0201a612, GetText(0x2f), 0, 0xc8, 0x10, d);
    sprintf(text, "%003d", score);
    DrawText(0x30, 0xb1, data_0201a694, text, 2, 0x82, 0x10, d);
    DrawText(0x5f, 0x95, data_0201a612, GetText(0x30), 0, 0xc8, 0x10, d);
    sprintf(text, "%003d", b);
    DrawText(0x7b, 0xb0, data_0201a68a, text, 2, 0x82, 0x10, d);
    DrawSprite(0xf49, 0xb2, 0x8e, 0xc8, d);
    DrawText(0xc1, 0x95, data_0201a612, GetText(0x31), 0, 0xc8, 0x10, d);
    if (UpdateTimeRemaining(frames) == 0) {
        FormatFrameTime(frames, time);
        DrawText(0xd2, 0xb0, data_0201a694, time, 2, 0xc8, 0x10, d);
    }
}

int PercentToX(int percent) {
    int low = percent < 100 ? percent : 100;

    if (low < 0) {
        percent = 0;
    } else if (percent >= 100) {
        percent = 100;
    }
    return MapRange(percent, 0, 100, 0, 100) + 14;
}

void DrawPercentGauge(int stage, int layer) {
    StageRec *rec;

    if (stage >= 0) {
        if (!(stage < STAGE_MAX)) {
            OS_Terminate();
        }
        DrawSprite(0xf59, 8, 0x54, 0xa0, layer);
        rec = &gStages[stage];
        DrawSprite(MapRange(rec->mInfo.mPercent, 0, 100, 50, 0) + 0xf5a, 7, 0x58, 0x9f, layer);
        DrawText(0x1c, 0x54, data_0201a612, GetText(10), 0, 0xf0, 0x10, layer);
    }
}

// Draws the info of the stage at index level (nothing when it is negative) and eases mPanelLevel. Always sets
// mTextOffsetY to 0x100, the offset DrawTextPanel eases to 0, also for a negative index.
void DrawStageInfo(int level, int layer) {
    StageRec *rec;
    int total;
    char label[8];
    char number[8];

    if (level >= 0) {
        rec = &gStages[level];
        total = GetFileValue(0x1e8, 0x13);
        if (!(level >= 0 && level < STAGE_MAX)) {
            OS_Terminate();
        }
        EaseLevel(&data_020b6ee0.mPanelLevel, 4);
        MapRange(data_020b6ee0.mPanelLevel, 0, 0x4000, 0x100, 0);
        DrawPercentGauge(level, layer);
        DrawSprite(0xf8d, 0, 0x79, 0xfa, layer);
        DrawText(0x25, 0x83, data_0201a61c, GetText(0xd), 2, 0xfa, 0, layer);
        DrawSprite(0xf4a, 0x27, 0xa3, 0xfa, layer);
        DrawSprite(0xf4a + rec->mInfo.mGrade, 0x27, 0xa3, 0xfa, layer);
        DrawSprite(0xf38, 0x47, 0x8b, 0x94, layer);
        DrawText(0x85, 0x83, data_0201a61c, GetText(0x18), 2, 0xfa, 0, layer);
        DrawSprite(0xf42 + rec->mInfo.mKind, 0x74, 0xaa, 0xfa, layer);
        *FormatDecimal(rec->mInfo.mValue, label) = 0;
        DrawText(0x74, 0xaa, data_0201a694, label, 0, 0xfa, 10, layer);
        if (func_020150c0(&rec->mInfo)) {
            DrawSprite(0xf57 + (gSystemState.mElapsedFrames % 60 <= 3 ? 1 : 0), 0x82, 0xbb, 0x8c, layer);
        }
        sprintf(number, "+%d", rec->mInfo.mValue * total / 100);
        DrawText(0xbe, 0xa4, data_0201a68a, number, 0, 0xfa, 10, layer);
        DrawText(0xe8, 0xb3, data_0201a61c, GetText(0xf), 1, 0xfa, 0, layer);
        DrawSprite(GetFileValue(0x10d5, rec->mInfo.mKind * 2 + 3) + 0xf32, 0xf2, 0xaf, 0xf8, layer);
    }
    data_020b6ee0.mTextOffsetY = 0x100;
}

void func_02011a94(int stage, int layer) {
    if (stage >= 0) {
        if (!(stage >= 0 && stage < STAGE_MAX)) {
            OS_Terminate();
        }
        EaseLevel(&data_020b6ee0.mPanelLevel, 4);
        MapRange(data_020b6ee0.mPanelLevel, 0, 0x4000, 0x100, 0);
        if (data_020b6ee0.mUnk_a110 < 11) {
            data_020b6ee0.mUnk_a110 += gSystemState.mElapsedFrames & 1;
        }
        DrawPercentGauge(stage, layer);
        DrawSprite(data_020b6ee0.mUnk_a110 + 0x19, 0x1f, 0x83, 0xf8, layer);
        if (data_020b6ee0.mUnk_a110 == 11) {
            DrawText(0x1f, 0x83, data_0201a61c, GetText(0x10), 0, 0xdc, 0xd, layer);
            DrawText(0x15, 0x97, data_0201a612, GetText(data_020b6ee0.mUnk_100c + 0x229), 0, 0xdc, 0xd, layer);
        }
    }
    data_020b6ee0.mTextOffsetY = 0x100;
}

void func_02011b70(int stage, int layer) {
    if (stage >= 0) {
        if (!(stage >= 0 && stage < STAGE_MAX)) {
            OS_Terminate();
        }
        DrawPercentGauge(stage, layer);
        DrawText(0x12, 0x96, data_0201a612, GetText(data_020b6ee0.mUnk_a13d + 0x5f), 0, 0xdc, 0x10, layer);
    }
    data_020b6ee0.mTextOffsetY = 0x100;
}

// a = -1 clears the shown text id, image index and timer and their saved copies; -2 clears only the shown ones; -3
// restores the saved copies; any other value shows text a with image index b for c frames and saves them. Always
// resets mTextOffsetY to 0x100 and mTextButtonOffsetY to 0x28.
void SetTextAndImage(int a, int b, int c) {
    Unk_020c1454 *p = &data_020b6ee0.mUnk_a574;

    if (a == -1) {
        data_020b6ee0.mSavedTextId = -1;
        data_020b6ee0.mTextId = -1;
        data_020b6ee0.mSavedTextImageIndex = 0;
        data_020b6ee0.mTextImageIndex = 0;
        data_020b6ee0.mSavedTextTimer = 0;
        data_020b6ee0.mTextTimer = 0;
    } else if (a == -2) {
        data_020b6ee0.mTextId = -1;
        data_020b6ee0.mTextImageIndex = 0;
        data_020b6ee0.mTextTimer = 0;
    } else if (a == -3) {
        data_020b6ee0.mTextId = data_020b6ee0.mSavedTextId;
        data_020b6ee0.mTextImageIndex = data_020b6ee0.mSavedTextImageIndex;
        data_020b6ee0.mTextTimer = data_020b6ee0.mSavedTextTimer;
    } else {
        data_020b6ee0.mSavedTextId = a;
        data_020b6ee0.mTextId = a;
        data_020b6ee0.mSavedTextImageIndex = b;
        data_020b6ee0.mTextImageIndex = b;
        data_020b6ee0.mSavedTextTimer = c;
        data_020b6ee0.mTextTimer = c;
    }
    data_020b6ee0.mTextOffsetY = 0x100;
    data_020b6ee0.mUnk_a13c = 1;
    data_020b6ee0.mUnk_a14b = 1;
    p->mTextButtonOffsetY = 0x28;
    if (data_020b6ee0.mTextImageIndex < 0) {
        OS_Terminate();
    }
    if (data_020b6ee0.mTextId < -1) {
        OS_Terminate();
    }
}

// The compiler sorts the file's objects by size and orders equal sizes by the whole list: defining data_0204ff08
// first and data_0204fee2 after data_0204fefc gives the original .rodata layout.
static const s32 data_0204ff08[3] = {1, 1, 1};
static const u16 data_0204fee8[3] = {0xdac, 0xdad, 0xdae};
static const u32 data_0204fef0[3] = {0x25, 0x2c, 0x2e};
static const s32 data_0204fefc[3] = {1, 1, 0x3c};
static const u16 data_0204fee2[3] = {0xdab, 0xdad, 0xdaf};
static const u16 data_0204ff14[7] = {0xe, 0x13, 0x18, 0x1d, 0x22, 0x27, 0x2c};
static const u16 data_0204ff22[7] = {8, 7, 6, 5, 4, 4, 4};
static const s32 data_0204ff30[4] = {10, 10, 10, 20};
static const s32 data_0204ff40[4] = {5, 5, 5, 10};
static const s32 data_0204ff50[8] = {32, 32, 32, 32, 32, 32, 32, 32};
static const s32 data_0204ff70[8] = {128, 158, 188, 218, 248, 278, 308, 338};
static const s32 data_0204ff90[12] = {0, 0, 0, 0, 9, 9, 9, 9, 18, 18, 18, 18};
static const u32 data_0204ffc0[12] = {9, 9, 9, 9, 9, 9, 9, 9, 8, 8, 8, 8};
static const s32 data_0204fff0[6][3] = {{0, 1, 2}, {0, 2, 1}, {1, 0, 2}, {1, 2, 0}, {2, 0, 1}, {2, 1, 0}};
static const s32 data_02050038[STAGE_MAX] = {
    0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x10, 0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1d,
    0x11, 0x11, 0x11, 0x10, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x10, 0x10, 0x10,
};

// Eases mTextOffsetY toward 0 and draws the header (text 0x4b), the percent gauge while 2 < mProgressState < 99
// (stage 0's before 11, then the selected stage's) and, when mTextId is not -1, that text at mTextOffsetY + 0xa0.
// UpdateTextAndStageInfo draws it in panel state 0 even when no text is set.
void DrawTextPanel(void) {
    u32 v;

    EaseToward(&data_020b6ee0.mTextOffsetY, 0, 4);
    DrawText(0x30, 0x11, data_0201a61c, GetText(0x4b), 0, 0xf0, 0xa, 2);
    v = data_020b6ee0.mProgressState;
    if (v > 2 && v < 99) {
        if (v >= 11) {
            DrawPercentGauge(data_020b6ee0.mSelectedStageIndex, 2);
        } else {
            DrawPercentGauge(0, 2);
        }
    }
    if (data_020b6ee0.mTextId != -1) {
        DrawText(0x1b, data_020b6ee0.mTextOffsetY + 0xa0, data_0201a66c, GetText(data_020b6ee0.mTextId), 0, 200, 10, 2);
        DrawSprite(0x40014, 0xd, data_020b6ee0.mTextOffsetY + 0xa3, 200, 2);
    }
}

// Counts the text timer down (SetTextAndImage(-2) when it runs out), then draws by mPanelState: in state 0 the text
// panel; otherwise the text panel while a text is set, else func_02011a94 (states 3 and 4), func_02011b70 (6 to 8)
// or DrawStageInfo (the rest) for the selected stage. In panel states 0 and 1 it also shows button 0x112, which
// clears the text, or, when only a saved text is left, button 0x111, which restores it.
void UpdateTextAndStageInfo(void) {
    Unk_020c1454 *p = &data_020b6ee0.mUnk_a574;

    if (data_020b6ee0.mTextId == -1) {
        if (data_020b6ee0.mTextTimer > 0) {
            data_020b6ee0.mTextTimer--;
        }
    } else if (data_020b6ee0.mTextTimer <= 0) {
        SetTextAndImage(-2, 0, 0);
    } else {
        data_020b6ee0.mTextTimer--;
    }
    switch (data_020b6ee0.mPanelState) {
        case 0:
            DrawTextPanel();
            break;
        case 3:
        case 4:
            if (data_020b6ee0.mTextId != -1) {
                DrawTextPanel();
            } else {
                func_02011a94(data_020b6ee0.mSelectedStageIndex, 2);
            }
            break;
        case 6:
        case 7:
        case 8:
            if (data_020b6ee0.mTextId != -1) {
                DrawTextPanel();
            } else {
                func_02011b70(data_020b6ee0.mSelectedStageIndex, 2);
            }
            break;
        default:
            if (data_020b6ee0.mTextId != -1) {
                DrawTextPanel();
            } else {
                DrawStageInfo(data_020b6ee0.mSelectedStageIndex, 2);
            }
            break;
    }
    if (data_020b6ee0.mPanelState <= 1) {
        if (data_020b6ee0.mTextId != -1) {
            EaseToward(&p->mTextButtonOffsetY, 0, 4);
            func_0200bc50(0x80, p->mTextButtonOffsetY + 0xc0, 0, 0, 0x8c, 0x112, 0xd1, 0, 0, 0, 0, 0x167, 1, 0, 0, 0);
            if (TestButton(0x20000000, 0x112)) {
                SetTextAndImage(-2, 0, 0);
            }
        } else if (data_020b6ee0.mSavedTextId != -1) {
            EaseToward(&p->mTextButtonOffsetY, 0, 4);
            func_0200bc50(0x80, p->mTextButtonOffsetY + 0xc0, 0, 0, 0x8c, 0x111, 0xd3, 0, 0, 0, 0, 0x167, 1, 0, 0, 0);
            if (TestButton(0x20000000, 0x111)) {
                SetTextAndImage(-3, 0, 0);
            }
        }
    }
}

void func_02011ebc(StageRec *rec, u8 index, u32 flags) {
    if (index >= 3) {
        OS_Terminate();
    }
    data_020b6ee0.mUnk_ca8 = !(gSystemState.mElapsedFrames & 1) ? data_0204fee2[index] : data_0204fee8[index];
    data_020b6ee0.mUnk_ca6 = 0;
    data_020b6ee0.mUnk_c94 = 0;
    data_020b6ee0.mUnk_c98 = 0;
    data_020b6ee0.mUnk_c9c = 0;
    rec->mMoveAngle = 0;
    rec->mUnk_1dc = 0;
    rec->mDistancePerFrame = 0;
    rec->mUnk_21c = 0;
    rec->mUnk_134 |= flags;
    PlayWaveKeepAlive(data_0204fef0[index], 100, 0, 0x40, 0xffff, 0);
}

int MapRangeToIndex(int value, int low, int high, int steps) {
    if ((value < high ? value : high) < low) {
        value = low;
    } else if (value >= high) {
        value = high;
    }
    return MapRange(value, low, high, 0, steps - 1);
}

void AddAmountObjSlots(int kind, int x, int y, int delta) {
    char text[0x20];

    if (delta >= 0) {
        sprintf(text, "+%d", delta >= 0 ? delta : -delta);
    } else if (delta < 0) {
        sprintf(text, "-%d", delta >= 0 ? delta : -delta);
    }
    AddAnimObjSlotEx(kind, 0, x - 0xf, y - 5, 0, 0, 0x98, 1);
    AddTextObjSlot(text, 2, data_0201a61c, 0x78, x, y + 6, 0, 0, 0x96, 1, 0);
}

void DrawGauge(int a, int b, int c, u8 d, u8 e, int x, int y, int z, u8 *state, s32 *position) {
    int hi3;
    int lo3;
    int lo4;
    int pos;
    int mid;
    int lo2;
    int hi2;
    int hi4;
    int mid2;
    int mid4;
    int half;
    int v0;
    int base;
    int lo;
    int hi;
    int rem;
    char text[12];

    if (d >= 7) {
        OS_Terminate();
    }
    if (e >= 7) {
        OS_Terminate();
    }
    v0 = data_0204ff22[e];
    base = data_0204ff14[d];
    lo = base - v0;
    lo = (lo < GAUGE_MAX ? lo : GAUGE_MAX) < 0 ? 0 : (lo >= GAUGE_MAX ? GAUGE_MAX : lo);
    hi = base + v0;
    hi = (hi < GAUGE_MAX ? hi : GAUGE_MAX) < 0 ? 0 : (hi >= GAUGE_MAX ? GAUGE_MAX : hi);
    mid = (lo + hi) / 2;
    lo2 = lo * 2 - 1;
    hi2 = hi * 2 + 3;
    mid2 = (lo2 + hi2) / 2;
    if ((a < c ? a : c) < b) {
        a = b;
    } else if (a >= c) {
        a = c;
    }
    pos = MapRange(a, b, c, 0, GAUGE_MAX);
    *position = pos;
    DrawSprite(0xad, x, y, 0xa4, z);
    DrawSprite(pos + 0x78, x, y, 0xa3, z);
    DrawSprite(data_020b6ee0.mUnk_a13d + 0xaa, x, y, 0xa2, z);
    sprintf(text, "+ %d", data_020b6ee0.mUnk_a100);
    if (data_020b6ee0.mUnk_a145 == -1) {
        if (data_020b6ee0.mUnk_a13d == 0) {
            DrawSprite(0xaf, x - 7, y - mid2, 0xa2, z);
        } else {
            DrawSprite(0xb4, x - 7, y - lo2, 0xa2, z);
            DrawSprite(0xaf, x - 7, y - hi2, 0xa2, z);
        }
        DrawText(x - 0x19, y - mid2 + 3, data_0201a612, text, 2, 200, 0x10, z);
    } else {
        half = data_020b6ee0.mUnk_a145 / 2;
        data_020b6ee0.mUnk_a145 = (data_020b6ee0.mUnk_a145 == 0x13) ? -1 : data_020b6ee0.mUnk_a145 + 1;
        if (data_020b6ee0.mUnk_a140 >= 7) {
            OS_Terminate();
        }
        base = data_0204ff14[data_020b6ee0.mUnk_a140];
        lo3 = base - v0;
        lo3 = (lo3 < GAUGE_MAX ? lo3 : GAUGE_MAX) < 0 ? 0 : (lo3 >= GAUGE_MAX ? GAUGE_MAX : lo3);
        hi3 = base + v0;
        hi3 = (hi3 < GAUGE_MAX ? hi3 : GAUGE_MAX) < 0 ? 0 : (hi3 >= GAUGE_MAX ? GAUGE_MAX : hi3);
        lo4 = lo3 * 2 - 1;
        hi4 = hi3 * 2 + 3;
        mid4 = (lo4 + hi4) / 2;
        if (data_020b6ee0.mUnk_a13d == 0) {
            rem = (u32) half % 5;
            DrawSprite(rem + 0xaf, x - 7, y - mid4, 0xa2, z);
        } else {
            rem = (u32) half % 5;
            DrawSprite(rem + 0xb4, x - 7, y - lo4, 0xa2, z);
            DrawSprite(rem + 0xaf, x - 7, y - hi4, 0xa2, z);
        }
        DrawText(x - 0x19, y - mid4 + 3, data_0201a612, text, 2, 200, 0x10, z);
    }
    if (data_020b6ee0.mUnk_a13d == 0) {
        if (pos < mid) {
            *state = 0;
        } else {
            *state = 2;
        }
    } else if (pos < lo) {
        *state = 0;
    } else if (pos > hi) {
        *state = 2;
    } else {
        *state = 1;
    }
}

void StartGauge(u8 type) {
    StageRec *rec;
    u8 grade;
    int ok;

    if (data_020b6ee0.mSelectedStageIndex == -1) {
        OS_Terminate();
    }
    rec = &gStages[data_020b6ee0.mSelectedStageIndex];
    ok = 0;
    grade = rec->mInfo.mGrade;
    if (grade >= 1 && grade <= 12) {
        ok = 1;
    }
    if (ok == 0) {
        OS_Terminate();
    }
    data_020b6ee0.mUnk_a13d = type;
    data_020b6ee0.mUnk_a110 = 0x3cf;
    data_020b6ee0.mUnk_a0fc = 0;
    data_020b6ee0.mUnk_a100 = 0;
    data_020b6ee0.mUnk_a104 = 0;
    data_020b6ee0.mUnk_a108 = 0;
    data_020b6ee0.mUnk_a13f = NextRandom() % 7;
    data_020b6ee0.mUnk_a141 = MapRange(rec->mInfo.mGrade, 1, 12, 0, 6);
    data_020b6ee0.mUnk_a142 = 0;
    data_020b6ee0.mUnk_a143 = 0;
    data_020b6ee0.mUnk_a144 = 0;
    data_020b6ee0.mUnk_a145 = -1;
    data_020b6ee0.mUnk_a140 = 0;
    data_020b6ee0.mUnk_a114 = 0;
    data_020b6ee0.mUnk_a118 = 0;
    data_020b6ee0.mUnk_a11c = 0;
    data_020b6ee0.mUnk_a14d = 0;
    if (data_020b6ee0.mUnk_a13d == 0) {
        gSystemState.mUnk_498 = 0x3f;
        func_0204028c(0x3f);
        SetupMic(0x400, data_020b1084.mUnk_04);
        StartMicSampling(0x82e, 0);
    }
}

// Ends the gauge of the selected stage: moves the panel to state 5, adds the amount ObjSlots for mUnk_a100, stops
// the stage and, when mUnk_a13d is 0, releases the microphone.
void StopGauge(int a) {
    StageRec *rec;
    int step;

    if (data_020b6ee0.mSelectedStageIndex == -1) {
        OS_Terminate();
    }
    rec = &gStages[data_020b6ee0.mSelectedStageIndex];
    data_020b6ee0.mUnk_a13e = a;
    data_020b6ee0.mPanelState = 5;
    data_020b6ee0.mUnk_a148 = 1;
    data_020b6ee0.mUnk_a10c = -1;
    data_020b6ee0.mPanelLevel = 0;
    step = MapRangeToIndex(data_020b6ee0.mUnk_a100, 0, 20, 3);
    func_02011ebc(rec, MapRangeToIndex(data_020b6ee0.mUnk_a100, 0, 20, 3), 1);
    AddAmountObjSlots(step + 0x1b, 0x80, 0x60, data_020b6ee0.mUnk_a100);
    if (data_020b6ee0.mUnk_a13d >= 3) {
        OS_Terminate();
    }
    if (step == 2 && rec->mUnk_22e == 1 && rec->mInfo.mUnk_1a == data_020b6ee0.mUnk_a13d) {
        rec->mUnk_22e = 0;
        func_02014cdc(&rec->mInfo);
    }
    rec->mDistancePerFrame = 0;
    rec->mUnk_21c = 0;
    if (data_020b6ee0.mUnk_a13d == 0) {
        gSystemState.mUnk_498 = 0x7f;
        func_0204028c(0x7f);
        ReleaseMic();
    }
}

void UpdateGauge(u8 a, int b) {
    StageRec *rec;
    int level;
    int type;
    int amount;

    if (data_020b6ee0.mSelectedStageIndex == -1) {
        OS_Terminate();
    }
    rec = &gStages[data_020b6ee0.mSelectedStageIndex];
    if (data_020b6ee0.mUnk_a13d > 3) {
        OS_Terminate();
    }
    if (rec->mInfo.mStageId >= STAGE_MAX) {
        OS_Terminate();
    }
    level = data_02050038[rec->mInfo.mStageId] - 6;
    if (level <= 8) {
        OS_Terminate();
    }
    if ((b < GAUGE_MAX ? b : GAUGE_MAX) < 0) {
        b = 0;
    } else if (b >= GAUGE_MAX) {
        b = GAUGE_MAX;
    }
    data_020b6ee0.mUnk_a10c = MapRange(b, 0, GAUGE_MAX, 0, level);
    rec->mUnk_134 |= 8;
    if (data_020b6ee0.mUnk_a144 == 1 && a == 0) {
        data_020b6ee0.mUnk_a144 = 0;
    }
    if (data_020b6ee0.mUnk_a144 == 0 && a > data_020b6ee0.mUnk_a142) {
        data_020b6ee0.mUnk_a142 = a;
    }
    data_020b6ee0.mUnk_a143 = 0;
    type = data_020b6ee0.mUnk_a13d;
    switch (type) {
        case 0:
            if (data_020b6ee0.mUnk_a142 != 0) {
                if (data_020b6ee0.mUnk_a142 == 2) {
                    data_020b6ee0.mUnk_a143 = 1;
                }
            }
            break;
        case 1:
            if (a == 0) {
                if (data_020b6ee0.mUnk_a142 != 0) {
                    if (data_020b6ee0.mUnk_a142 == 1) {
                        data_020b6ee0.mUnk_a143 = 1;
                    } else {
                        data_020b6ee0.mUnk_a143 = 2;
                    }
                }
            }
            break;
        case 2:
            if (a == 1) {
                data_020b6ee0.mUnk_a104++;
                if (data_020b6ee0.mUnk_a104 >= data_0204fefc[type]) {
                    data_020b6ee0.mUnk_a143 = 1;
                }
            } else if (data_020b6ee0.mUnk_a104 != 0) {
                data_020b6ee0.mUnk_a143 = 2;
            }
            break;
    }
    if (data_020b6ee0.mUnk_a143 != 0) {
        if (data_020b6ee0.mUnk_a143 == 1) {
            PlayWaveKeepAlive(0xdc, 100, 0, 0x40, 0xffff, 0);
            data_020b6ee0.mUnk_a145 = 0;
            data_020b6ee0.mUnk_a140 = data_020b6ee0.mUnk_a13f;
            data_020b6ee0.mUnk_a14d = 1;
            if (rec->mUnk_22e == 1 && rec->mInfo.mUnk_1a == data_020b6ee0.mUnk_a13d) {
                amount = data_0204ff30[data_020b6ee0.mUnk_a13d];
            } else {
                amount = data_0204ff40[data_020b6ee0.mUnk_a13d];
            }
            AddPercent(rec, amount, 0, 1);
            data_020b6ee0.mUnk_a100 += amount;
            data_020b6ee0.mUnk_a0fc++;
            if (data_020b6ee0.mUnk_a0fc >= data_0204ff08[data_020b6ee0.mUnk_a13d]) {
                data_020b6ee0.mUnk_a13f = NextRandom() % 7;
                data_020b6ee0.mUnk_a0fc = 0;
                data_020b6ee0.mUnk_a144 = 1;
            }
        }
        data_020b6ee0.mUnk_a142 = 0;
        data_020b6ee0.mUnk_a104 = 0;
    }
    data_020b6ee0.mUnk_a110--;
    if (data_020b6ee0.mUnk_a110 <= 0) {
        if (data_020b6ee0.mUnk_a100 != 0) {
            StopGauge(1);
        } else {
            StopGauge(0);
        }
    }
}

void UpdatePanel(int stage) {
    StageRec *rec;
    int top;
    int bottom;
    int i;
    int x;
    int y;
    int id;
    int alpha;
    const u8 *font;
    u8 kind;
    int value;
    VecFx32 normal;
    u32 key;
    int gauge;
    Unk_020c1454 *fade = &data_020b6ee0.mUnk_a574;

    if (stage != -1) {
        rec = &gStages[stage];
    } else {
        rec = NULL;
    }
    if (!(stage >= -1 && stage < STAGE_MAX)) {
        OS_Terminate();
    }
    EaseLevel(&data_020b6ee0.mPanelLevel, 4);
    alpha = MapRange(data_020b6ee0.mPanelLevel, 0, 0x4000, 0x100, 0);
    switch (data_020b6ee0.mPanelState) {
        case 0:
            if (gSystemState.mFlags & 0x800) {
                OS_Terminate();
            }
            EaseToward(&fade->mPanelButtonOffsetX, 0, 4);
            func_0200bbd0(fade->mPanelButtonOffsetX + 0xff, 0x5f, 0, 0, 0x8c, 0x5a, 0xc8, 0, 0, 0, 0, 0x167, 1, 0, 0, 0,
                          &data_020b6ee0.mUnk_a56f);
            func_0200bbd0(fade->mPanelButtonOffsetX + 0xff, 0x7d, 0, 0, 0x8c, 0xd, 0xc4, 0, 0, 0, 0, 0x167, 1, 0, 0, 0,
                          &data_020b6ee0.mUnk_a56e);
            top = -1;
            bottom = -1;
            break;
        case 1:
            EaseToward(&fade->mPanelButtonOffsetX, 0, 4);
            func_0200bbd0(fade->mPanelButtonOffsetX + 0xff, 0x23, 0, 0, 0x8c, 0xa, 0xbc, 0, 0, 0, 0, 0x167, 1, 0, 0, 0,
                          &data_020b6ee0.mUnk_a56c);
            func_0200bbd0(fade->mPanelButtonOffsetX + 0xff, 0x5f, 0, 0, 0x8c, 0xb, 0xc8, 0, 0, 0, 0, 0x167, 1, 0, 0, 0,
                          &data_020b6ee0.mUnk_a56f);
            func_0200bbd0(fade->mPanelButtonOffsetX + 0xff, 0x7d, 0, 0, 0x8c, 0xd, 0xc4, 0, 0, 0, 0, 0x167, 1, 0, 0, 0,
                          &data_020b6ee0.mUnk_a56e);
            if (stage == -1 || rec == NULL) {
                OS_Terminate();
            }
            if (rec->mInfo.mPercent == 100) {
                func_0200bbd0(fade->mPanelButtonOffsetX + 0xff, 0x41, 0, 0, 0x8c, 0xc, 0xcc, 0, 0, 0, 0, 0x167, 1, 0, 0,
                              0, &data_020b6ee0.mUnk_a56d);
            } else {
                func_0200bbd0(fade->mPanelButtonOffsetX + 0xff, 0x41, 0, 0, 0x8c, 0xc, 0xcc, 0, 0, 0, 0, 0x167, 0, 0, 0,
                              0, &data_020b6ee0.mUnk_a56d);
            }
            top = -1;
            bottom = 0xe;
            break;
        case 2:
            func_0200bc50(0x39 - alpha, 0x12, 0, 0, 0x8c, 0, 0xd9, GetText(0x17), data_0201a61c, 0, 0, 0, 0, 0, 0, 0);
            func_0200bc50(alpha + 0xff, 0x23, 0, 0, 0x8c, 0xf, 0xc0, 0, 0, 0, 0, 0x167, 1, 0, 0, 0);
            func_0200bc50(alpha + 0xff, 0x41, 0, 0, 0x8c, 0x10, 0xc2, 0, 0, 0, 0, 0x167, 1, 0, 0, 0);
            top = -1;
            bottom = 0x11;
            break;
        case 3:
            x = 0x39 - alpha;
            DrawSprite(0xda, x, 0x12, 0x8c, 1);
            DrawText(x, 0x12, data_0201a61c, GetText(0x10), 0, 0xf0, 0x10, 1);
            gMapState.mCamera.mUnk_2dc.mUnk_4c = 1;
            DrawSprite(0x800017, 0, 0, 0x64, 1);
            for (i = 0, y = 0x54; i < 3; i++, y += 0x16) {
                id = data_020b6ee0.mUnk_1000[i];
                AddTouchButtonWithSounds(i + 0x19, 0x10, y, -1, -1, 0x41, 0, 0x167);
                if (i == data_020b6ee0.mPanelRow) {
                    font = data_0201a644;
                } else {
                    font = data_0201a63a;
                }
                DrawText(0x16, y, font, GetText(data_020b6ee0.mUnk_100e[id] + 0x229), 0, 0xf0, 0x10, 1);
            }
            DrawSprite(0x800041, 0x10, data_020b6ee0.mPanelRow * 0x16 + 0x54, 0x5f, 1);
            if (TestButtonRange(0x80000000, 0x19, 0x1b) == 1) {
                id = (u16) gTouchInput.mTrail.mTouchState.mButtonState - 0x19;
                if (data_020b6ee0.mPanelRow != id) {
                    data_020b6ee0.mPanelRow = id;
                }
            }
            top = 0x17;
            bottom = 0x18;
            break;
        case 4:
            DrawSprite(0xda, 0x39, 0x12, 0x8c, 1);
            DrawText(0x39, 0x12, data_0201a61c, GetText(0x10), 0, 0xf0, 0x10, 1);
            DrawSprite(0x18, 0, 0, 0x8c, 1);
            DrawText(0x12, 0x96, data_0201a63a, GetText(data_020b6ee0.mUnk_100e[0] + 0x229), 0, 0xdc, 0x10, 1);
            top = -1;
            bottom = 0x12;
            break;
        case 5:
            x = 0x39 - alpha;
            DrawSprite(0xde, x, 0x12, 0x8c, 1);
            DrawText(x, 0x12, data_0201a61c, GetText(0x11), 0, 0xf0, 0x10, 1);
            rec->mMoveAngle += AngleDelta(rec->mMoveAngle, 0) / 8;
            if (data_020b6ee0.mUnk_a148 == 0) {
                for (i = 0; i < 3; i++) {
                    func_0200bc50(i * 0x32 + 0x3a, 0x60, 0, 0, 0x8c, i + 0x1e, i * 2 + 0xdf, 0, 0, 0, 0, 0x167, 1, 0, 0,
                                  0);
                }
            }
            top = -1;
            bottom = 0x12;
            break;
        case 6:
            x = 0x39 - alpha;
            DrawSprite(0xdb, x, 0x12, 0x8c, 1);
            DrawText(x, 0x12, data_0201a61c, GetText(0x15), 0, 0xf0, 0x10, 1);
            DrawGauge(data_020b6ee0.mUnk_a8e8, 0, 3000, data_020b6ee0.mUnk_a13f, data_020b6ee0.mUnk_a141, 0x2c - alpha,
                      0x96, 1, &kind, &value);
            if (gSystemState.mUnk_492) {
                DrawText(0x80, 0x60, data_0201a61c, GetText(0xd3), 2, 0xc0, 0x10, 1);
            }
            if (!(gSystemState.mFlags & 0x800)) {
                OS_Terminate();
            }
            if (func_020213e8(((int) (7 - data_020b1084.mUnk_08) + 1) * 25)) {
                if (data_020b6ee0.mUnk_a114 < 3) {
                    data_020b6ee0.mUnk_a114++;
                }
            } else {
                if (data_020b6ee0.mUnk_a114 > 0) {
                    data_020b6ee0.mUnk_a114--;
                }
            }
            if (data_020b1084.mUnk_08 > 7) {
                OS_Terminate();
            }
            id = MapRange(data_020b6ee0.mUnk_a8e8, 0, 3000, data_0204ff50[data_020b1084.mUnk_08],
                          data_0204ff70[data_020b1084.mUnk_08]);
            if (data_020b6ee0.mUnk_a114 == 0) {
                id = 0;
            }
            if (data_020b6ee0.mUnk_a14d == 1) {
                if (data_020b6ee0.mUnk_a8e8 <= 0x100) {
                    data_020b6ee0.mUnk_a14d = 0;
                }
            } else {
                data_020b6ee0.mUnk_a8e8 += id;
            }
            rec->mUnk_1da += AngleDelta(rec->mUnk_1da, 0) / 8;
            rec->mUnk_1dc += AngleDelta(rec->mUnk_1dc, 0) / 8;
            if (rec->mMoveAngle > data_020b6ee0.mUnk_0000) {
                data_020b6ee0.mUnk_0002 += 0x100;
            }
            data_020b6ee0.mUnk_0000 = rec->mMoveAngle;
            if (data_020b6ee0.mUnk_0002 > 4) {
                data_020b6ee0.mUnk_0002 -= 4;
            }
            UpdateGauge(kind, value);
            top = -1;
            bottom = 0x21;
            break;
        case 7: {
            int speed;
            int ground;

            x = 0x39 - alpha;
            DrawSprite(0xdc, x, 0x12, 0x8c, 1);
            DrawText(x, 0x12, data_0201a61c, GetText(0x16), 0, 0xf0, 0x10, 1);
            DrawGauge(rec->mPos.y - 0x32000, 0, 0x12c000, data_020b6ee0.mUnk_a13f, data_020b6ee0.mUnk_a141,
                      0x2c - alpha, 0x96, 1, &kind, &value);
            if (gSystemState.mTouchPressed) {
                speed = rec->mPos.y / 0xc800;
                if (speed < 0) {
                    speed = -speed;
                }
                if ((speed < 40 ? speed : 40) < 0) {
                    speed = 0;
                } else if (speed >= 40) {
                    speed = 40;
                }
                rec->mUnk_21c = -MapRange(speed, 0, 40, 10, 20) << 12;
            }
            rec->mPos.y = rec->mPos.y + rec->mUnk_21c;
            rec->mUnk_21c = rec->mUnk_21c - 2000.0f;
            ground = GetFloorHeightEx(rec->mPos.x, rec->mPos.z, rec->mPos.y + 0xc800, 0, 0, NULL, &normal) + 0x32000;
            if (rec->mPos.y < ground) {
                speed = rec->mUnk_21c >= 0.0f ? rec->mUnk_21c : 0.0f - rec->mUnk_21c;
                if ((speed < 30000 ? speed : 30000) < 0) {
                    speed = 0;
                } else if (speed >= 30000) {
                    speed = 30000;
                }
                speed = MapRange(speed, 0, 30000, 0, 0x30);
                rec->mUnk_21c = (0.0f - rec->mUnk_21c) * speed / 64.0f;
                rec->mPos.y = ground;
                if (rec->mUnk_21c >= 5000.0f) {
                    PlayWaveKeepAlive(0x27, 100, 0, 0x40, 0xffff, 0);
                }
            }
            UpdateGauge(kind, value);
            top = -1;
            bottom = 0x21;
            break;
        }
        case 8:
            x = 0x39 - alpha;
            DrawSprite(0xdd, x, 0x12, 0x8c, 1);
            DrawText(x, 0x12, data_0201a61c, GetText(0x19), 0, 0xf0, 0x10, 1);
            DrawGauge(data_020b6ee0.mUnk_a108, 0, 1000, data_020b6ee0.mUnk_a13f, data_020b6ee0.mUnk_a141, 0x2c - alpha,
                      0x96, 1, &kind, &value);
            if (gSystemState.mTouchPressed) {
                data_020b6ee0.mUnk_a574.mPrevTouchX = gSystemState.mTouchX;
                data_020b6ee0.mUnk_a574.mPrevTouchY = gSystemState.mTouchY;
            } else if (gSystemState.mTouchDown) {
                data_020b6ee0.mUnk_a108 += (data_020b6ee0.mUnk_a574.mPrevTouchX - gSystemState.mTouchX >= 0
                                                ? data_020b6ee0.mUnk_a574.mPrevTouchX - gSystemState.mTouchX
                                                : -(data_020b6ee0.mUnk_a574.mPrevTouchX - gSystemState.mTouchX)) +
                                           (data_020b6ee0.mUnk_a574.mPrevTouchY - gSystemState.mTouchY >= 0
                                                ? data_020b6ee0.mUnk_a574.mPrevTouchY - gSystemState.mTouchY
                                                : -(data_020b6ee0.mUnk_a574.mPrevTouchY - gSystemState.mTouchY));
                gauge = data_020b6ee0.mUnk_a108;
                if ((gauge < 1000 ? gauge : 1000) < 0) {
                    gauge = 0;
                } else if (gauge >= 1000) {
                    gauge = 1000;
                }
                data_020b6ee0.mUnk_a108 = gauge;
                data_020b6ee0.mUnk_a574.mPrevTouchX = gSystemState.mTouchX;
                data_020b6ee0.mUnk_a574.mPrevTouchY = gSystemState.mTouchY;
            }
            if (data_020b6ee0.mUnk_a108 > 0) {
                data_020b6ee0.mUnk_a108 -= 8;
            }
            UpdateGauge(kind, value);
            top = -1;
            bottom = 0x21;
            break;
    }
    if (top >= 0) {
        func_0200bc50(alpha + 0xff, 0xa1, 0, 0, 0x8c, top, 0x15, 0, 0, 0, 0, 0x17d, 1, 0, 0, 0);
    }
    if (bottom >= 0) {
        func_0200bc50(0, 0xa1, 0, 0, 0x8c, bottom, 0xe, 0, 0, 0, 0, 0x5a, 1, 0, 0, 0);
    }
    key = (u16) gTouchInput.mTrail.mTouchState.mButtonState;
    if ((gTouchInput.mTrail.mTouchState.mButtonState & 0x20000000) && key != 0xffff &&
        data_020b6ee0.mButtonsEnabled == 1) {
        switch (key) {
            case 0xa:
                data_020b6ee0.mPanelState = 2;
                data_020b6ee0.mUnk_ca8 = 0;
                data_020b6ee0.mUnk_a148 = 0;
                data_020b6ee0.mPanelLevel = 0;
                fade->mPanelButtonOffsetX = 0x28;
                SetTextAndImage(-2, 0, 0);
                break;
            case 0xb:
                PushState(9);
                PopState();
                break;
            case 0xc:
                PushState(0xd);
                PopState();
                break;
            case 0xd:
                data_020b6ee0.mSelectedStageIndex = -1;
                PushState(6);
                PopState();
                break;
            case 0xe:
                data_020b6ee0.mSelectedStageIndex = -1;
                data_020b6ee0.mUnk_a574.mUnk_0c.x = gMapState.mCamera.mLookAtGoal.x;
                data_020b6ee0.mUnk_a574.mUnk_0c.y = gMapState.mCamera.mLookAtGoal.y;
                data_020b6ee0.mUnk_a574.mUnk_0c.z = gMapState.mCamera.mLookAtGoal.z;
                fade->mPanelButtonOffsetX = 0x28;
                break;
            case 0xf: {
                int r;
                int base;
                int first;
                int ok;
                int k;
                int idx;

                data_020b6ee0.mPanelState = 3;
                data_020b6ee0.mPanelRow = 0;
                data_020b6ee0.mUnk_a13d = 3;
                data_020b6ee0.mUnk_a110 = 0;
                data_020b6ee0.mPanelLevel = 0;
                SetTextAndImage(-2, 0, 0);
                r = NextRandom() % 6;
                if (!(r * 3 + 2 < 18)) {
                    OS_Terminate();
                }
                data_020b6ee0.mUnk_1000[0] = data_0204fff0[r][0];
                data_020b6ee0.mUnk_1000[1] = data_0204fff0[r][1];
                data_020b6ee0.mUnk_1000[2] = data_0204fff0[r][2];
                base = rec->mInfo.mKind * 0x68;
                first = base + 0x1a;
                ok = 0;
                if (rec->mInfo.mGrade != 0 && rec->mInfo.mGrade - 1 < 12) {
                    ok = 1;
                }
                if (!ok) {
                    OS_Terminate();
                }
                idx = rec->mInfo.mGrade - 1;
                k = NextRandom() % data_0204ffc0[idx];
                k += data_0204ff90[idx];
                data_020b6ee0.mUnk_100c = base + k;
                for (i = 0; i < 3; i++) {
                    data_020b6ee0.mUnk_100e[i] = first + (k * 3 + i);
                }
                break;
            }
            case 0x10:
                data_020b6ee0.mPanelState = 5;
                data_020b6ee0.mUnk_a148 = 0;
                data_020b6ee0.mPanelLevel = 0;
                break;
            case 0x11:
                data_020b6ee0.mPanelState = 1;
                break;
            case 0x12:
                ClearObjSlots();
                data_020b6ee0.mPanelState = 2;
                data_020b6ee0.mUnk_a148 = 0;
                data_020b6ee0.mPanelLevel = 0;
                data_020b6ee0.mUnk_ca8 = 0;
                rec->mUnk_134 |= 2;
                rec->mPos.y =
                    GetFloorHeightEx(rec->mPos.x, rec->mPos.z, rec->mPos.y + 0xc800, 0, 0, NULL, NULL) + 0x32000;
                break;
            case 0x17: {
                int got;

                data_020b6ee0.mPanelState = 4;
                if (data_020b6ee0.mUnk_1000[data_020b6ee0.mPanelRow] == 0) {
                    if (data_020b6ee0.mUnk_a13d >= 4) {
                        OS_Terminate();
                    }
                    if (rec->mUnk_22e == 1 && rec->mInfo.mUnk_1a == data_020b6ee0.mUnk_a13d) {
                        id = data_0204ff30[data_020b6ee0.mUnk_a13d];
                    } else {
                        id = data_0204ff40[data_020b6ee0.mUnk_a13d];
                    }
                    data_020b6ee0.mUnk_a13e = 1;
                    data_020b6ee0.mUnk_a100 = id;
                    AddPercent(rec, id, 1, 1);
                } else {
                    data_020b6ee0.mUnk_a13e = 0;
                    data_020b6ee0.mUnk_a100 = 0;
                }
                got = data_020b6ee0.mUnk_a100 == 0 ? 0 : 2;
                func_02011ebc(rec, data_020b6ee0.mUnk_a100 == 0 ? 0 : 2, 1);
                AddAmountObjSlots(got + 0x1b, 0x80, 0x60, data_020b6ee0.mUnk_a100);
                if (data_020b6ee0.mUnk_a13d >= 4) {
                    OS_Terminate();
                }
                if (got == 2 && rec->mUnk_22e == 1 && rec->mInfo.mUnk_1a == data_020b6ee0.mUnk_a13d) {
                    rec->mUnk_22e = 0;
                    func_02014cdc(&rec->mInfo);
                }
                break;
            }
            case 0x18:
                data_020b6ee0.mPanelState = 2;
                data_020b6ee0.mUnk_a148 = 0;
                data_020b6ee0.mPanelLevel = 0;
                break;
            case 0x1e:
                data_020b6ee0.mPanelState = 6;
                StartGauge(0);
                data_020b6ee0.mUnk_0002 = 0;
                data_020b6ee0.mUnk_0004 = 8;
                data_020b6ee0.mUnk_0000 = 0;
                data_020b6ee0.mPanelLevel = 0;
                SetTextAndImage(-2, 0, 0);
                break;
            case 0x1f:
                data_020b6ee0.mPanelState = 7;
                StartGauge(1);
                rec->mUnk_21c = -60000.0f;
                rec->mMoveAngle = 0xe000;
                rec->mUnk_1da = 0;
                rec->mUnk_1dc = 0;
                data_020b6ee0.mPanelLevel = 0;
                SetTextAndImage(-2, 0, 0);
                break;
            case 0x20:
                data_020b6ee0.mPanelState = 8;
                StartGauge(2);
                rec->mMoveAngle = 0xe000;
                rec->mUnk_1da = 0;
                rec->mUnk_1dc = 0;
                data_020b6ee0.mPanelLevel = 0;
                SetTextAndImage(-2, 0, 0);
                break;
            case 0x21:
                StopGauge(0);
                break;
            case 0x29:
                // No action; the original's jump table runs up to this key.
                break;
            case 0x5a:
                data_020b6ee0.mUnslottedVisible = 0;
                PushState(9);
                PopState();
                break;
        }
    }
}

void SetupSlotScreen(void) {
    int stage;
    int id;

    SetFaded(3);
    UpdateSystemAndWait();
    ResetKeys();
    ResetVramBanks();
    ReleaseTextureLayers();
    SetLcdSwap(1);
    SetupTextureLayer(0x10cb, 0x10ca, 0x10cc, 0x82, 1);
    SetupTextureLayer(0x10cb, 0x10ca, 0x10cc, 0x82, 2);
    func_02024208();
    data_020b13b0.mSlotChanged = 0;
    data_020b13b0.mUnslottedStageIndex = data_020b6ee0.mSelectedStageIndex;
    memcpy(gSavedSlotStages, data_020b6ee0.mSlotStages, 0xc);
    data_020b13b0.mEntryCursor = -1;
    data_020b6ee0.mSelectedSlotIndex = 1;
    if (data_020b6ee0.mStageCount <= 3) {
        data_020b6ee0.mUnslottedVisible = 0;
    } else {
        data_020b6ee0.mUnslottedVisible = 1;
        if (data_020b6ee0.mSelectedStageIndex != -1) {
            int slot = FindStageSlot(data_020b6ee0.mSelectedStageIndex);
            if (slot != -1) {
                data_020b13b0.mUnslottedStageIndex = FindFirstUnslottedStage();
                data_020b6ee0.mSelectedSlotIndex = slot;
            }
        } else {
            data_020b13b0.mUnslottedStageIndex = FindFirstUnslottedStage();
        }
    }
    stage = data_020b6ee0.mSelectedStageIndex;
    if (stage == -1) {
        id = 0x237;
    } else {
        id = gStages[stage].mInfo.mStageId + 0xdb3;
    }
    LoadImage(id, 1);
    LoadImage(data_020b6ee0.mUnslottedVisible == 1 ? 0x10ce : 0x10cd, 2);
}

void UpdateSlotScreen(void) {
    StageRec *sel;
    int ox;
    int oy;
    int i;
    int s;
    int x;
    int y;
    int sub;

    if (data_020b13b0.mUnslottedStageIndex != -1) {
        sel = &gStages[data_020b13b0.mUnslottedStageIndex];
    } else {
        sel = NULL;
    }
    UpdateSlotScreenInput();
    if (data_020b6ee0.mPrevSelectedStageIndex != data_020b6ee0.mSelectedStageIndex) {
        LoadStageOrTextImage(data_020b6ee0.mSelectedStageIndex, 1);
        data_020b6ee0.mPrevSelectedStageIndex = data_020b6ee0.mSelectedStageIndex;
    } else if (data_020b6ee0.mSelectedStageIndex == -1 && data_020b6ee0.mUnk_a13c == 1) {
        LoadStageOrTextImage(data_020b6ee0.mSelectedStageIndex, 1);
    }
    func_02013f74(data_020b6ee0.mSelectedStageIndex, &data_020b13b0.mEntryCursor, 0x80, 0xa1, 2, 0);
    if (data_020b13b0.mEntryCursor == -1) {
        DrawStageInfo(data_020b6ee0.mSelectedStageIndex, 1);
    }
    if (data_020b6ee0.mUnslottedVisible == 1) {
        ox = 0;
        oy = 0;
    } else {
        ox = -0x2e;
        oy = 0x14;
    }
    DrawText(0x3a, 0x11, data_0201a61c, GetText(0x27), 0, 0xf0, 0x10, 2);
    DrawText(ox + 0xad, oy + 0x26, data_0201a61c, GetText(0x28), 2, 0xf0, 0x10, 2);
    i = 0;
    y = oy + 0x4e;
    x = ox + 0x80;
    for (; i < 3; i++) {
        s = data_020b6ee0.mSlotStages[i];
        if (s != -1) {
            if (s >= STAGE_MAX) {
                OS_Terminate();
            }
            DrawSprite(gStages[s].mInfo.mStageId + 0x1089, x, y, 0x64, 2);
            DrawSprite(0x1088, x, y, 0x66, 2);
            if (s == data_020b6ee0.mSelectedStageIndex) {
                sub = (gSystemState.mElapsedFrames >> 1) & 7;
            } else {
                sub = 0;
            }
            if (s == data_020b6ee0.mSelectedStageIndex ||
                (data_020b6ee0.mUnslottedVisible == 1 && i == data_020b6ee0.mSelectedSlotIndex)) {
                DrawSprite(0x11c6 + sub, x, y, 0x65, 2);
            }
        } else {
            DrawSprite(0x1088, x, y, 0x66, 2);
        }
        if (gSystemState.mTouchDown == 1 && Length2d(gSystemState.mTouchX - x, gSystemState.mTouchY - y + 0xc) < 0x10 &&
            data_020b6ee0.mSelectedStageIndex != s) {
            if (s != -1) {
                data_020b6ee0.mSelectedStageIndex = s;
            }
            data_020b6ee0.mSelectedSlotIndex = i;
            data_020b13b0.mEntryCursor = -1;
            PlayWaveKeepAlive(0x167, 0x64, 0, 0x40, 0xffff, 0);
        }
        x += 0x2d;
    }
    if (data_020b6ee0.mUnslottedVisible == 1) {
        s = data_020b13b0.mUnslottedStageIndex;
        DrawSprite(sel->mInfo.mStageId + 0x1089, 0x32, 0x4e, 0x64, 2);
        DrawSprite(0x1088, 0x32, 0x4e, 0x66, 2);
        if (data_020b13b0.mUnslottedStageIndex == data_020b6ee0.mSelectedStageIndex) {
            sub = (gSystemState.mElapsedFrames >> 1) & 7;
        } else {
            sub = 0;
        }
        DrawSprite(0x11be + sub, 0x32, 0x4e, 0x65, 2);
        if (gSystemState.mTouchDown == 1 && Length2d(gSystemState.mTouchX - 0x32, gSystemState.mTouchY - 0x42) < 0x10 &&
            data_020b6ee0.mSelectedStageIndex != s) {
            if (s != -1) {
                data_020b6ee0.mSelectedStageIndex = s;
            }
            data_020b13b0.mEntryCursor = -1;
            PlayWaveKeepAlive(0x167, 0x64, 0, 0x40, 0xffff, 0);
        }
        func_0200bc50(0x38, 0x77, 0, 0, 0xc8, 0x200, 0x11d2, GetText(0x29), data_0201a61c, 2, 0, 0x17d, 1, 0, 0, 0);
        ox += 0x80;
        oy += 0x4e;
        DrawSprite(0x11d1, ox + data_020b6ee0.mSelectedSlotIndex * 0x2d, oy, 0xc9, 2);
    }
    if (data_020b6ee0.mUnslottedVisible == 1 && data_020b6ee0.mStageCount >= 5) {
        func_0200bb14(0x1b, 0x29, 0x110, 0x12, 0, 0x20, 0, 1);
        func_0200bb14(0x4e, 0x29, 0x109, 0x10, 0, 0x10, 0, 1);
    }
    func_0200bb14(0, 0xa1, 0x102, 0xe, 0, 0, 0x5a, 1);
    if (data_020b6ee0.mUnslottedVisible == 1 && data_020b13b0.mSlotChanged == 1) {
        func_0200bb14(0xff, 0xa1, 0x101, 0x15, 0, 0, 0x17d, 1);
    }
}

void LeaveSlotScreen(void) {
    func_020102f8();
}

void UpdateSlotScreenInput(void) {
    int i;

    if (TestButton(0x20000000, 0x109) && data_020b6ee0.mUnslottedVisible == 1) {
        data_020b13b0.mUnslottedStageIndex = data_020b6ee0.mSelectedStageIndex =
            FindNextUnslottedStage(data_020b13b0.mUnslottedStageIndex);
        PlayWaveKeepAlive(0x167, 0x64, 0, 0x40, 0xffff, 0);
    }
    if (TestButton(0x20000000, 0x110) && data_020b6ee0.mUnslottedVisible == 1) {
        data_020b13b0.mUnslottedStageIndex = data_020b6ee0.mSelectedStageIndex =
            FindPrevUnslottedStage(data_020b13b0.mUnslottedStageIndex);
        PlayWaveKeepAlive(0x167, 0x64, 0, 0x40, 0xffff, 0);
    }
    if (TestButton(0x20000000, 0x200)) {
        data_020b13b0.mUnslottedStageIndex =
            SetSlotStage(data_020b6ee0.mSelectedSlotIndex, data_020b13b0.mUnslottedStageIndex);
        data_020b13b0.mSlotChanged = 1;
    }
    if (TestButton(0x20000000, 0x101)) {
        data_020b6ee0.mUnk_a147 = 1;
        PushState(10);
        PopState();
    }
    if (TestButton(0x20000000, 0x102)) {
        for (i = 0; i < 3; i++) {
            SetSlotStage(i, gSavedSlotStages[i]);
        }
        data_020b6ee0.mUnk_a147 = 1;
        PushState(10);
        PopState();
    }
}

void func_02013b48(StageInfo *info, int slot, int *a, int *b) {
    int index;
    int base;
    int v4;
    int v5;

    index = FindTableEntryIndexUnbounded(slot);
    base = index * 0x14;
    v4 = GetFileValue(0x1d9, base + 4);
    v5 = GetFileValue(0x1d9, base + 5);
    if (slot < 100) {
        *a = index + 0x10b4;
        *b = -1;
    } else if (slot < 200) {
        *a = 0x10c1;
        *b = v4 / 10 + 0x113f;
    } else if (slot < 300) {
        *a = info->mKind + 0x10c2;
        *b = v4 / 10 + 0x113f;
    } else if (slot < 400) {
        *a = v5 + 0x10c8;
        *b = v4 / 10 + 0x113f;
    } else if (slot < 500) {
        *a = 0x10ce;
        *b = v4 / 10 + 0x113f;
    }
}

void func_02013c24(StageInfo *info, int slot, int *a, int *b, int *c, int *d, int *e, int *f) {
    int index;
    int v4;
    int v5;
    int base;

    index = FindTableEntryIndexUnbounded(slot);
    base = index * 0x14;
    v4 = GetFileValue(0x1d9, base + 4);
    v5 = GetFileValue(0x1d9, base + 5);
    if (e != NULL) {
        *e = GetFileValue(0x1d9, base + 3);
    }
    if (f != NULL) {
        *f = v5;
    }
    if (slot < 100) {
        *a = index;
        *b = 0x10ea;
        *c = index + 0x10cf;
        *d = -1;
        return;
    }
    *a = slot / 100 + 0xc;
    if (slot < 200) {
        *b = 0x10eb;
        *c = 0x10dc;
        *d = v4 / 10 + 0x10ee;
    } else if (slot < 300) {
        *b = 0x10ec;
        *c = info->mKind + 0x10dd;
        *d = v4 / 10 + 0x10ee;
    } else if (slot < 400) {
        *b = 0x10ed;
        *c = v5 + 0x10dd;
        *d = v4 / 10 + 0x10ee;
    } else if (slot < 500) {
        *b = 0x10ee;
        *c = 0x10e9;
        *d = v4 / 10 + 0x10ee;
    }
}

void func_02013d48(StageInfo *info, s16 slot, u8 flag, int a, int b, int c, int d, int e) {
    int icon;
    int name;

    func_02013b48(info, slot, &icon, &name);
    if (flag == 1) {
        DrawSprite(UpdateSprite(&data_020b6ee0.mSelectionSprite), b, c, 0x93, d);
    }
    if (!(e & 4)) {
        AddTouchButtonWithSounds(a, b, c, 0, 0, 0x10b4, 0, 0x167);
    }
    DrawSprite(icon, b, c, 0x94, d);
    DrawSprite(name, b + 8, c - 4, 0x92, d);
}

void func_02013dc8(StageInfo *info, s16 slot, int a, int b) {
    int frame;
    int icon;
    int level;
    int index;
    int kind;
    int kindId;

    func_02013c24(info, slot, &index, &frame, &icon, &level, &kind, &kindId);
    DrawSprite(frame, 0x35, 0x85, 0x96, a);
    DrawSprite(icon, 0x35, 0x85, 0x8c, a);
    DrawSprite(level, 0x4e, 0x7b, 0x82, a);
    if (b & 1) {
        DrawSprite(0x80113f, 5, 0xbf, 0xfa, a);
    }
    if (kind < 0xe) {
        DrawText(0x5f, 0x93, data_0201a61c, GetText(index + 0x8a), 0, 0xf0, 0x10, 1);
        DrawText(0x12, 0xa4, data_0201a612, GetText(index + 0x9b), 0, 0xe8, 0xa, 1);
    } else if (kind == 0xe) {
        DrawText(0x5f, 0x93, data_0201a61c, GetText(info->mKind + 0xb8), 0, 0xf0, 0x10, 1);
        DrawText(0x12, 0xa4, data_0201a612, GetText(info->mKind + 0xac), 0, 0xe8, 0xa, 1);
    } else if (kind == 0xf) {
        DrawText(0x5f, 0x93, data_0201a61c, GetText(kindId + 0xbe), 0, 0xf0, 0x10, 1);
        DrawText(0x12, 0xa4, data_0201a612, GetText(kindId + 0xb2), 0, 0xe8, 0xa, 1);
    } else if (kind == 0x10) {
        DrawText(0x5f, 0x93, data_0201a61c, GetText(index + 0x8a), 0, 0xf0, 0x10, 1);
        DrawText(0x12, 0xa4, data_0201a612, GetText(index + 0x9b), 0, 0xe8, 0xa, 1);
    }
}

void func_02013f74(int stage, int *cursor, int x, int y, int layer, int flags) {
    StageRec *rec;
    int otherLayer;
    int i;
    u8 sel;
    int xx;
    StageInfo *info;

    if (stage != -1) {
        rec = &gStages[stage];
    } else {
        rec = NULL;
    }
    if (layer == 1) {
        otherLayer = 2;
    } else {
        otherLayer = 1;
    }
    if (!(stage >= -1 && stage < STAGE_MAX)) {
        OS_Terminate();
    }
    if (stage != -1) {
        DrawSprite(0xfeb, x + 2, y + 0xb, 0x96, layer);
        for (i = 0; i < 5; i++) {
            info = &rec->mInfo;
            sel = (*cursor == i);
            xx = x - 0x34;
            if (info->mTableEntryIds[i] >= 0) {
                func_02013d48(info, info->mTableEntryIds[i], sel, 0x500 + i, xx, y, layer, flags);
                if (sel) {
                    func_02013dc8(info, info->mTableEntryIds[i], otherLayer, flags);
                }
            } else if (flags & 2) {
                if (!(flags & 4)) {
                    AddTouchButtonWithSounds(0x500 + i, xx, y, 0, 0, 0x10b4, 0, 0x167);
                }
                if (sel) {
                    DrawSprite(UpdateSprite(&data_020b6ee0.mSelectionSprite), xx, y, 0x93, layer);
                }
            }
            if (TestButton(0x80000000, 0x500 + i)) {
                *cursor = i;
            }
            x += 0x1a;
        }
    }
}

void func_020140d4(void) {
    ResetVramBanks();
    ReleaseTextureLayers();
    ReleaseGxPool();
    SetupTextureLayer(0x10cb, 0x10ca, 0x10cc, 0, 1);
    InitGxPool(0x1c7, 0x1c6, 0x100, 0x10);
    SetLcdSwap(1);
    func_02024208();
    data_020b13b0.mUnk_02 = 0;
    SetupSharedModel(0, 1);
}

void func_02014124(void) {
    u32 sel;

    DrawSharedModel();
    if (gSystemState.mKeysPressed & 0x100) {
        sel = data_020b13b0.mUnk_02 + 1;
        if (sel >= CountSharedModels() - 1) {
            sel = CountSharedModels() - 1;
        }
        data_020b13b0.mUnk_02 = sel;
        SetupSharedModel(data_020b13b0.mUnk_02, 1);
    } else if (gSystemState.mKeysPressed & 0x200) {
        int prev = data_020b13b0.mUnk_02 - 1;
        if (prev < 0) {
            prev = 0;
        }
        data_020b13b0.mUnk_02 = prev;
        SetupSharedModel(data_020b13b0.mUnk_02, 1);
    }
}

void func_02014184(void) {
    func_020102f8();
}

void func_0201418c(void) {}

void func_02014190(void) {
    ResetVramBanks();
    ReleaseTextureLayers();
    ReleaseGxPool();
    SetupTextureLayer(0x10cb, 0x10ca, 0x10cc, 0, 1);
    InitGxPool(0x1c7, 0x1c6, 0x100, 0x10);
    SetLcdSwap(1);
    func_020220c8(0x10d0);
    SetupMap(0xf87, 0, 0xf88, 0, 0, 0);
    gMapState.mCamera.mUnk_04 = func_0201418c;
    gMapState.mCamera.mUnk_2dc.mUnk_54 = 0x640;
    gMapState.mCamera.mUnk_2dc.mUnk_44 = 0x128e000;
    memset(&data_020b6ee0.mModelPos, 0, 0x44);
    func_02031fc4(0, &data_020b6ee0.mModelPos);
    data_020b6ee0.mUnk_fdf &= ~7;
    InitModel(&data_020b6ee0.mModel[0], 0, 0, 0, data_0201aa60[0]);
    data_020b13b0.mUnk_01 = 0;
    data_020b13b0.mUnk_00 = 0;
}

void func_02014260(void) {
    u8 order[5] = {0, 1, 2, 3, 4};
    u16 keys;

    SetCameraFromModel();
    func_02033614(gMapState.mCamera.mPosGoal.x, gMapState.mCamera.mPosGoal.z);
    DrawBattleModels();
    func_0200b8dc(10, 10, "t:%i a:%i", data_020b13b0.mUnk_00, data_020b13b0.mUnk_01);
    keys = gSystemState.mKeysPressed;
    if (keys & 0x10) {
        if (data_020b13b0.mUnk_00 < 4) {
            data_020b13b0.mUnk_00++;
            data_020b6ee0.mModel[0].mUnk_142 = 0;
            data_020b6ee0.mModel[0].mUnk_134 |= 1 << order[data_020b13b0.mUnk_00];
        }
    } else if (keys & 0x20) {
        if (data_020b13b0.mUnk_00 != 0) {
            data_020b13b0.mUnk_00--;
            data_020b6ee0.mModel[0].mUnk_142 = 0;
            data_020b6ee0.mModel[0].mUnk_134 |= 1 << order[data_020b13b0.mUnk_00];
        }
    }
    if (keys & 0x40) {
        if (data_020b13b0.mUnk_01 < 0x26) {
            data_020b13b0.mUnk_01++;
            SetupStageModels(data_020b13b0.mUnk_01);
            data_020b6ee0.mModel[0].mUnk_142 = 0;
            data_020b6ee0.mModel[0].mUnk_134 |= 1 << order[data_020b13b0.mUnk_00];
        }
    } else if (keys & 0x80) {
        if (data_020b13b0.mUnk_01 != 0) {
            data_020b13b0.mUnk_01--;
            SetupStageModels(data_020b13b0.mUnk_01);
            data_020b6ee0.mModel[0].mUnk_142 = 0;
            data_020b6ee0.mModel[0].mUnk_134 |= 1 << order[data_020b13b0.mUnk_00];
        }
    }
}

void func_0201438c(void) {}

void ResetProgress(int a) {
    int i;

    data_020b6ee0.mReachedNodeIndex = 0;
    data_020b6ee0.mLastNodeDone = 0;
    data_020b6ee0.mNodeState = 0;
    data_020b6ee0.mNodeStageCount = 0;
    data_020b6ee0.mSelectedStageIndex = -1;
    data_020b6ee0.mUnk_a147 = 0;
    data_020b6ee0.mUnk_a14a = 1;
    data_020b6ee0.mSaveRequested = 0;
    data_020b6ee0.mProgressState = 1;
    data_020b6ee0.mUnk_a568 = 0;
    data_020b6ee0.mUnk_a56c = 0;
    data_020b6ee0.mUnk_a56d = 0;
    data_020b6ee0.mUnk_a56e = 0;
    data_020b6ee0.mUnk_a56f = 0;
    data_020b6ee0.mUnk_a570 = 0;
    data_020b6ee0.mButtonsEnabled = 1;
    SetTextAndImage(-1, 0, 0);
    data_020b10fc.mTableRow = 0;
    for (i = 0; i < 5; i++) {
        data_020b10fc.mBestScores[i] = 0;
    }
}

void ResetAllProgress(void) {
    ResetProgress(0);
    ResetStages(0);
    ResetSlots(0);
}

void ResetAndReadSaveSlot(void) {
    ResetProgress(1);
    ResetStages(1);
    ResetSlots(1);
    ReadSaveSlot(data_020b1084.mSaveSlot);
}

// Only when OpenPromptScreen returns 1 (else nothing is saved): runs a 0x78-frame loop with the volume faded down
// and, once IsFadeState(3, FADE_STATE_CLEAR), stores the stage offsets and writes the save: a = 0 the slot mSaveSlot
// (WriteSaveSlot), a = 1 every slot through func_02010cfc. Then closes the prompt screen.
void RunSaveScreen(int a) {
    int local;
    int timer;
    int fired;
    int running = 1;

    if (OpenPromptScreen(&local, 0) == 1) {
        timer = 0x78;
        fired = 0;
        StopAllWaves();
        // running is 1 here: VOLUME_FADE_DOWN.
        StartVolumeFade(running);
        do {
            timer--;
            if (timer == 0) {
                running = 0;
            }
            if (IsFadeState(3, FADE_STATE_CLEAR) != 0 && fired == 0) {
                fired = 1;
                StoreStageOffsets();
                switch (a) {
                    case 0:
                        WriteSaveSlot(data_020b1084.mSaveSlot);
                        break;
                    case 1:
                        func_02010cfc();
                        break;
                }
            }
            func_020101d0();
            UpdateSoundCtl();
            UpdateSystemAndWait();
        } while (running == 1);
        StartVolumeFade(VOLUME_FADE_UP);
        ClosePromptScreen();
    }
}

// Row of table 0x1d9 whose column 2 holds id; -1 for id -1. Terminates when no row holds id.
int FindTableEntryIndex(int id) {
    int i;

    if (id != -1) {
        for (i = 0; i < TABLE_ENTRY_COUNT; i++) {
            if (id == GetFileValue(0x1d9, i * 20 + 2)) {
                return i;
            }
        }
        if (i >= TABLE_ENTRY_COUNT) {
            OS_Terminate();
        }
    }
    return -1;
}

int GetTableEntryValue(int row, int column) {
    if ((u32) row >= TABLE_ENTRY_COUNT) {
        OS_Terminate();
    }
    if ((u32) (GetFileValueType(0x1d9, column + row * 20) - 2) <= 1) {
        return GetFileValue(0x1d9, column + row * 20);
    }
    return -1;
}

int GetTableEntryValueById(int id, int column) {
    int index = FindTableEntryIndex(id);

    if (index == -1) {
        OS_Terminate();
    }
    return GetTableEntryValue(index, column);
}

int GetNodeValue(int node, int column) {
    int index;

    if ((u32) node >= MAP_NODE_COUNT) {
        OS_Terminate();
    }
    index = column + node * 17;
    if ((u32) (GetFileValueType(0xda2, index) - 2) <= 1) {
        return GetFileValue(0xda2, index);
    }
    return -1;
}

int GetSelectedNodeStageId(int a) {
    int result;
    int other;

    if ((u32) a >= 3) {
        OS_Terminate();
    }
    if (data_020b6ee0.mSelectedNodeIndex == -1) {
        OS_Terminate();
    }
    result = GetNodeValue(data_020b6ee0.mSelectedNodeIndex, a + 8);
    if (IsReachedNodeSelected() == 0) {
        other = GetNodeValue(data_020b6ee0.mSelectedNodeIndex, a + 11);
        if (other != -1) {
            result = other;
        }
    }
    return result;
}
