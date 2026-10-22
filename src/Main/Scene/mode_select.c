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

ModeSelectState data_0206d448;
s32 data_0206d444;

static const u32 data_0204e0e8[2] = {0xe9, 0xf0};
static const u32 data_0204e0f0[3] = {0xf7, 0xfe, 0x105};
static const s32 data_0204e0fc[5] = {14, 15, 16, 17, 4};
static const s32 data_0204e110[5] = {41, 40, 42, 43, 44};
static const s32 data_0204e124[5] = {0x301, 0x300, 0x302, 0x304, 0x303};

u32 data_0206391c = 1800;

// Selects mode 'mode' and stores the value its handler returns for the current stage.
void func_020054a4(int mode) {
    int level;
    u8 stage;
    level = data_020b6ee0.mSelectedStageIndex;
    if (level < 0) {
        level = 0;
    }
    stage = data_020b1990[level].mInfo.mUnk_11 - 1;
    data_020b10fc.mMode = mode;
    switch (mode) {
        case 0:
            data_0206d448.mUnk_00 = func_0200d6b0(stage);
            break;
        case 1:
            data_0206d448.mUnk_00 = func_0200b88c(stage);
            break;
        case 2:
            data_0206d448.mUnk_00 = func_02006fb4(stage);
            break;
        case 3:
            data_0206d448.mUnk_00 = func_02004940(stage);
            break;
        case 4:
            data_0206d448.mUnk_00 = func_020186f0(stage);
            break;
        default:
            data_0206d448.mUnk_00 = 0;
            break;
    }
    data_0206d448.mUnk_04 = data_020b10fc.mBestScores[data_020b10fc.mMode];
}

void func_02005548(void) {
    func_0200c5d8(0, 0x10d1, data_020b1990[data_020b6ee0.mSelectedStageIndex].mInfo.mUnk_13 + 0xdb3, 32, 16, 130, 2);
    func_02024208();
    if (data_0206d444 == 0) {
        func_020054a4(4);
    } else {
        data_0206d444 = 0;
    }
}

// Draws the mode selection screen.
void func_020055a8(void) {
    char text[32];
    StageRec *rec = &data_020b1990[data_020b6ee0.mSelectedStageIndex];
    DrawStageInfo(data_020b6ee0.mSelectedStageIndex, 2);
    func_020215fc(48, 17, data_0201a61c, GetText(26), 0, 200, 16, 1);
    func_0200bb14(255, 161, 161 + 96, 21, 0, 0, 0x17d, 1);
    func_0200bb14(0, 161, 161 + 97, 14, 0, 0, 90, 1);
    func_0200bb58(67, 61, 0, 0, 140, 0x303, data_0204e124[data_020b10fc.mMode], 0x11dc, 0, 0, 140 + 219, 1, 0, 0, 0);
    if (rec->mInfo.mUnk_11 > 1) {
        func_0200bb58(29, 84, 0, 0, 140, 0x300, data_0204e124[data_020b10fc.mMode], 0x11d8, 0, 0, 140 + 219, 1, 0, 0,
                      0);
    }
    if (rec->mInfo.mUnk_11 > 2) {
        func_0200bb58(67, 106, 0, 0, 140, 0x301, data_0204e124[data_020b10fc.mMode], 0x11d4, 0, 0, 140 + 219, 1, 0, 0,
                      0);
    }
    if (rec->mInfo.mUnk_11 > 3) {
        func_0200bb58(29, 129, 0, 0, 140, 0x302, data_0204e124[data_020b10fc.mMode], 0x11da, 0, 0, 140 + 219, 1, 0, 0,
                      0);
    }
    if (rec->mInfo.mUnk_11 > 4) {
        func_0200bb58(67, 152, 0, 0, 140, 0x304, data_0204e124[data_020b10fc.mMode], 0x11d6, 0, 0, 140 + 219, 1, 0, 0,
                      0);
    }
    func_020215fc(99, 108, data_0201a64e, GetText(data_020b10fc.mMode + 31), 0, 200, 16, 1);
    func_020215fc(106, 129, data_0201a61c, GetText(28), 0, 200, 16, 1);
    sprintf(text, "%d", data_0206d448.mUnk_00);
    func_020215fc(215, 129, data_0201a61c, text, 0, 200, 16, 1);
    func_020215fc(106, 144, data_0201a61c, GetText(29), 0, 200, 16, 1);
    sprintf(text, "%d", data_0206d448.mUnk_04);
    func_020215fc(215, 144, data_0201a61c, text, 0, 200, 16, 1);
    func_0200bb14(153, 181, 50 << 4, 0x11de, 0, 0, 0x167, 1);
    func_020215fc(153, 181, data_0201a61c, GetText(30), 2, 200, 16, 1);
}

// Handles the touch regions of the mode selection screen.
void func_02005898(void) {
    int idx;
    if (func_0200bd80(0x20000000, 0x102)) {
        PushState(10);
        PopState();
    }
    if (func_0200bd80(0x20000000, 0x300)) {
        func_020054a4(1);
    } else if (func_0200bd80(0x20000000, 0x301)) {
        func_020054a4(0);
    } else if (func_0200bd80(0x20000000, 0x302)) {
        func_020054a4(2);
    } else if (func_0200bd80(0x20000000, 0x304)) {
        func_020054a4(3);
    } else if (func_0200bd80(0x20000000, 0x303)) {
        func_020054a4(4);
    }
    if (func_0200bd80(0x20000000, 0x101)) {
        idx = data_020b10fc.mMode;
        if (data_020b10fc.mBestScores[idx] == 0) {
            func_0201033c(data_0204e110[idx], data_0204e0fc[idx]);
        } else {
            PushState(data_0204e0fc[idx]);
            PopState();
        }
    }
    if (func_0200bd80(0x20000000, 0x320)) {
        data_0206d444 = 1;
        func_0201033c(data_0204e110[data_020b10fc.mMode], 13);
    }
    func_020055a8();
}

void func_020059ac(void) {
    func_020102f8();
}

void func_020059b4(int a, u32 b) {
    data_020b10fc.mUnk_0c = a;
    data_020b10fc.mScore = b;
    PushState(18);
    PopState();
}

void func_020059cc(u32 value) {}

// Adds 'add' to *value and limits the result to 999 << shift.
void func_020059d0(u32 *value, u32 add, int shift) {
    *value += add;
    *value = *value < (999 << shift) ? *value : (999 << shift);
}

// Adds 'add' to *value and limits the result to 5999.
void func_020059e8(u32 *value, u32 add) {
    *value += add;
    *value = *value < 5999 ? *value : 5999;
}

// Stores the new time value; plays the countdown sound when it falls below one of the thresholds and returns 1 while
// the value is nonzero, at most 600 and the clock low bits are at most 8.
int func_02005a00(u32 value) {
    u32 prev = data_0206391c;
    if ((prev >= 600 && value < 600) || (prev >= 300 && value < 300) || (prev >= 240 && value < 240) ||
        (prev >= 180 && value < 180) || (prev >= 120 && value < 120) || (prev >= 60 && value < 60)) {
        func_0202f110(361, 100, 0, 64, 0xffff, 0);
    }
    data_0206391c = value;
    if (value <= 600 && value != 0 && (gSystemState.mElapsedFrames & 0x1f) <= 8) {
        return 1;
    }
    return 0;
}

void func_02005a8c(int a, int b) {
    data_0206d448.mUnk_08 = a == 1 ? 1 : 2;
    data_0206d448.mUnk_0c = 6 << 14;
    data_0206d448.mUnk_10 = 120;
    data_0206d448.mUnk_14 = b;
    if (a == 0) {
        if (b == 1) {
            func_0202f110(data_020b1084.mLanguage + data_0204e0f0[NextRandom() % 3], 100, 0, 64, 0xffff, 0);
        } else if (b == 0) {
            func_0202f110(data_020b1084.mLanguage + data_0204e0e8[NextRandom() & 1], 100, 0, 64, 0xffff, 0);
        }
    }
}

int func_02005b1c(int mode) {
    s32 v;
    switch (data_0206d448.mUnk_08) {
        case 1:
            if (data_0206d448.mUnk_10 != 0) {
                v = data_0206d448.mUnk_0c - (30 << 8);
                if (v < 0) {
                    v = 0;
                }
                data_0206d448.mUnk_0c = v;
                if (v == 0) {
                    data_0206d448.mUnk_10--;
                }
            } else {
                v = data_0206d448.mUnk_0c - (30 << 8);
                if (v < -65536) {
                    v = -65536;
                }
                data_0206d448.mUnk_0c = v;
                if (v == -65536) {
                    data_0206d448.mUnk_08 = 0;
                    return 0;
                }
            }
            func_0202cf08(data_020b1084.mLanguage + 0xe77, (data_0206d448.mUnk_0c >> 8) + 128, 96, 10, mode);
            return 1;
        case 2: {
            int id = data_0206d448.mUnk_14 == 1 ? 0xec5 : 0xe7d;
            if (data_0206d448.mUnk_10 != 0) {
                v = data_0206d448.mUnk_0c - (30 << 8);
                if (v < 0) {
                    v = 0;
                }
                data_0206d448.mUnk_0c = v;
                if (v == 0) {
                    data_0206d448.mUnk_10--;
                }
            } else {
                return 0;
            }
            func_0202cf08(id + data_020b1084.mLanguage, (data_0206d448.mUnk_0c >> 8) + 128, 96, 10, mode);
            return 1;
        }
        default:
            return 0;
    }
}

u8 func_02005bf8(void) {
    return data_0206d448.mUnk_08;
}

s32 func_02005c04(void) {
    return data_0206d448.mUnk_14;
}
