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

static const u32 data_0204e0cc[2] = {11, 0};
static const u32 data_0204e0d4[5] = {0x11d4, 0x11d8, 0x11da, 0x11d6, 0x11dc};

ResultRemainder data_0206d1c0;
ResultState data_0206d1d4;

void func_0200494c(void) {
    func_02014bf0();
    RunSaveScreen(0);
    PushState(10);
    PopState();
}

void func_02004964(void) {
    int i;
    int stage;
    int level;
    s32 v;
    stage = data_020b10fc.mUnk_0c == 1 ? 0x1d1 : 0x1ca;
    level = data_020b6ee0.mSelectedStageIndex;
    if (level < 0) {
        level = 0;
    }
    func_0200c5d8(0, stage, data_020b1990[level].mInfo.mUnk_13 + 0xdb3, 256, 16, 1, 2);
    func_02024208();
    data_0206d1d4.mUnk_00 = 0;
    data_0206d1d4.mUnk_04 = -1;
    data_0206d1d4.mUnk_08 = 0;
    data_0206d1d4.mUnk_0c = 1;
    data_0206d1d4.mUnk_10 = level;
    v = GetFileValue(488, 5) << 8;
    data_0206d1d4.mUnk_14 = v;
    data_0206d1d4.mUnk_18 = (v << 8) >> 8;
    v = GetFileValue(488, 7) << 8;
    data_0206d1d4.mUnk_1c = v;
    data_0206d1d4.mUnk_20 = (v << 8) >> 8;
    data_0206d1d4.mUnk_24 = 0;
    data_0206d1d4.mUnk_28 = GetFileValue(488, 9) << 8;
    data_0206d1d4.mUnk_2c = 0;
    data_0206d1d4.mUnk_30 =
        data_020b10fc.mUnk_0c == 1 ? PickWeightedTableEntry(&data_020b1990[data_0206d1d4.mUnk_10].mInfo) : -1;
    data_0206d1d4.mUnk_34 = 0;
    data_0206d1d4.mUnk_38 = 0;
    data_0206d1d4.mUnk_3c = 5;
    data_0206d1d4.mUnk_40 = 0;
    for (i = 0; i < 8; i++) {
        data_0206d1d4.mUnk_48[i] = 0;
    }
    func_0200f848(data_0204e0cc[data_020b10fc.mUnk_0c], 0);
    StartSprite(&data_0206d1d4.mUnk_68, data_0201abec[3]);
    StartSprite(&data_0206d1d4.mUnk_16c, data_0201abec[4]);
    RequestTrack(data_020b10fc.mUnk_0c == 1 ? 7 : 8, 102);
}

void func_02004aa8(void) {
    char text0[16];
    char text1[16];
    char text2[16];
    char text3[16];
    char text4[16];
    char text5[16];
    char text6[16];
    StageRec *rec;
    int ready;
    int slot;
    s32 v;
    if (data_0206d1d4.mUnk_00 != 2) {
        DrawStageInfo(data_0206d1d4.mUnk_10, 2);
    } else {
        s32 t = data_0206d1d4.mUnk_3c;
        if (t < 0 || (t == 5 && data_0206d1d4.mUnk_38 != 0) ||
            (t >= 0 && t < 5 && data_020b1990[data_0206d1d4.mUnk_10].mInfo.mUnk_1c[t] < 0)) {
            DrawStageInfo(data_0206d1d4.mUnk_10, 1);
        }
    }
    if (data_0206d1d4.mUnk_00 == 0) {
        func_0200f898();
        return;
    }
    if (data_0206d1d4.mUnk_00 == 1) {
        DrawText(51, 21, data_0201a61c, GetText(data_020b10fc.mMode + 31), 0, 200, 16, 1);
        DrawSprite(data_0204e0d4[data_020b10fc.mMode], 25, 22, 130, 1);
        switch (data_020b10fc.mUnk_0c) {
            case 1:
                DrawText(89, 66, data_0201a64e, GetText(44), 2, 200, 16, 1);
                if (data_0206d1d4.mUnk_08 >= 18) {
                    sprintf(text0, "%d", data_020b10fc.mScore);
                    DrawText(35, 128, data_0201a64e, GetText(43), 0, 200, 16, 1);
                    DrawText(180, 128, data_0201a64e, text0, 1, 200, 16, 1);
                }
                if (data_0206d1d4.mUnk_08 >= 36 && data_0206d1d4.mUnk_14 > 0) {
                    sprintf(text1, "+G %d", data_0206d1d4.mUnk_14 >> 8);
                    DrawText(140, 143, data_0201a61c, text1, 0, 200, 16, 1);
                }
                if (data_0206d1d4.mUnk_08 >= 72) {
                    sprintf(text2, "%d", data_020b10fc.mBestScores[data_020b10fc.mMode]);
                    DrawText(35, 148, data_0201a64e, GetText(46), 0, 200, 16, 1);
                    DrawText(180, 148, data_0201a64e, text2, 1, 200, 16, 1);
                }
                if (data_0206d1d4.mUnk_24 != 0 && data_0206d1d4.mUnk_1c > 0) {
                    sprintf(text3, "+G %d", data_0206d1d4.mUnk_1c >> 8);
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
                if (data_0206d1d4.mUnk_08 >= 18) {
                    if (data_0206d1d4.mUnk_28 < 0) {
                        sprintf(text6, "%s %d", GetText(10), data_0206d1d4.mUnk_28 >> 8);
                        DrawText(70, 163, data_0201a61c, text6, 0, 200, 16, 1);
                    }
                }
                break;
        }
        if (data_0206d1d4.mUnk_2c != 0) {
            func_0200bb14(255, 161, 161 + 96, 21, 0, 0, 0x17d, 1);
        }
        return;
    }
    if (data_0206d1d4.mUnk_00 != 2) {
        return;
    }
    rec = &data_020b1990[data_0206d1d4.mUnk_10];
    slot = data_0206d1d4.mUnk_30;
    ready = 0;
    if (IsFadeState(3, FADE_STATE_CLEAR)) {
        v = data_0206d1d4.mUnk_48[0] - (5 << 10);
        if (v < 0) {
            v = 0;
        }
        data_0206d1d4.mUnk_48[0] = v;
        v = data_0206d1d4.mUnk_48[1] + (5 << 10);
        if (v >= 0) {
            v = 0;
        }
        data_0206d1d4.mUnk_48[1] = v;
        v = data_0206d1d4.mUnk_48[2] + (5 << 10);
        if (v >= 0) {
            v = 0;
        }
        data_0206d1d4.mUnk_48[2] = v;
        v = data_0206d1d4.mUnk_48[3] + (5 << 10);
        if (v >= 0) {
            v = 0;
        }
        data_0206d1d4.mUnk_48[3] = v;
        v = data_0206d1d4.mUnk_48[4] - (5 << 10);
        if (v < 0) {
            v = 0;
        }
        data_0206d1d4.mUnk_48[4] = v;
        if (data_0206d1d4.mUnk_48[0] == 0 && data_0206d1d4.mUnk_48[1] == 0 && data_0206d1d4.mUnk_48[2] == 0 &&
            data_0206d1d4.mUnk_48[3] == 0 && v == 0) {
            ready = 1;
        }
    }
    if (data_0206d1d4.mUnk_38 == 1) {
        func_0200bb14(255, 161, 161 + 96, 21, 0, 0, 0x17d, 1);
    }
    if (data_0206d1d4.mUnk_34 == 1) {
        func_0200bb14(0, 161, 161 + 97, 14, 0, 0, 90, 1);
    }
    if (slot != -1 && data_0206d1d4.mUnk_34 == 0) {
        DrawText(70 + (data_0206d1d4.mUnk_48[0] >> 8), 37, data_0201a64e, GetText(52), 0, 256, 16, 2);
        if (data_0206d1d4.mUnk_48[0] == 0 && data_0206d1d4.mUnk_48[1] == 0) {
            UpdateSprite(&data_0206d1d4.mUnk_68);
            DrawSprite(data_0206d1d4.mUnk_68.mUnk_04, 57, 103, 150, 2);
        }
        func_02013d48(&rec->mInfo, slot, data_0206d1d4.mUnk_3c == 5, 0x505, 57, (data_0206d1d4.mUnk_48[2] >> 8) + 103,
                      2, 0);
        if (data_0206d1d4.mUnk_3c == 5) {
            func_02013dc8(&rec->mInfo, slot, 1, 0);
        }
        if (func_0200bd80(0x80000000, 0x505) && ready) {
            data_0206d1d4.mUnk_3c = 5;
        }
        if (data_0206d1d4.mUnk_68.mUnk_de != 0) {
            UpdateSprite(&data_0206d1d4.mUnk_16c);
            DrawSprite(data_0206d1d4.mUnk_16c.mUnk_04, 0, 137, 130, 2);
            if (data_0206d1d4.mUnk_16c.mUnk_de != 0) {
                DrawSprite(0xfb4, data_0206d1d4.mUnk_40 * 26 + 116, 103, 141, 2);
            }
        }
        func_0200bc50((data_0206d1d4.mUnk_48[4] >> 8) + 181, 174, 0, 0, 140, 512, 0xfb5, GetText(41), data_0201a61c, 2,
                      0, 359, ready, 0, 0, 0);
        if (func_0200bd80(0x20000000, 0x200) && ready) {
            data_0206d1c0.mUnk_10 = 1;
            data_0206d1d4.mUnk_44 = rec->mInfo.mUnk_1c[data_0206d1d4.mUnk_40];
            rec->mInfo.mUnk_1c[data_0206d1d4.mUnk_40] = slot;
            data_0206d1d4.mUnk_34 = 1;
        }
        func_0200bc50((data_0206d1d4.mUnk_48[3] >> 8) + 73, 174, 0, 0, 140, 513, 0xfd7, GetText(61), data_0201a61c, 2,
                      0, 90, ready, 0, 0, 0);
        if (func_0200bd80(0x20000000, 0x201) && ready) {
            data_0206d1c0.mUnk_10 = 0;
            data_0206d1d4.mUnk_34 = 1;
        }
        if (func_0200bdac(0x80000000, 0x500, 0x504) && ready) {
            data_0206d1d4.mUnk_40 = (u16) data_020c1d60.mUnk_800.mUnk_100.mUnk_04 - 0x500;
        }
    }
    if (func_0200bd80(0x20000000, 0x102)) {
        data_0206d1d4.mUnk_34 = 0;
        if (data_0206d1c0.mUnk_10 != 0) {
            rec->mInfo.mUnk_1c[data_0206d1d4.mUnk_40] = data_0206d1d4.mUnk_44;
        }
    }
    DrawSprite(rec->mInfo.mUnk_13 + 0x1089, (data_0206d1d4.mUnk_48[1] >> 8) + 40, 45, gSystemState.mUnk_308 - 4, 2);
    DrawSprite(0x1088, (data_0206d1d4.mUnk_48[1] >> 8) + 40, 45, gSystemState.mUnk_308 - 2, 2);
    func_02013f74(data_0206d1d4.mUnk_10, &data_0206d1d4.mUnk_3c, 168, 103, 2, (ready == 1 ? 0 : 4) + 2);
    data_0206d1d4.mUnk_38 = data_0206d1d4.mUnk_34;
}

void func_02005194(void) {
    s32 old;
    s32 prev;
    u32 whole;
    if (data_0206d1d4.mUnk_04 >= 0) {
        if (data_0206d1d4.mUnk_04 == 2) {
            if (IsFadeState(3, FADE_STATE_FADED)) {
                data_0206d1d4.mUnk_00 = data_0206d1d4.mUnk_04;
                data_0206d1d4.mUnk_04 = -1;
                func_020102f8();
                func_0200c5d8(1, data_020b1990[data_0206d1d4.mUnk_10].mInfo.mUnk_13 + 0xdb3, 456, 128, 16, 1, 130);
                data_0206d1d4.mUnk_48[0] = 1 << 16;
                data_0206d1d4.mUnk_48[1] = -0x8000;
                data_0206d1d4.mUnk_48[2] = -0x8000 << 2;
                data_0206d1d4.mUnk_48[3] = -0x48000;
                data_0206d1d4.mUnk_48[4] = 18 << 14;
                gSystemState.mFlags |= 2 << 16;
                StartFadeClear(3);
            }
        } else {
            if (IsFadeState(1, FADE_STATE_FADED)) {
                data_0206d1d4.mUnk_00 = data_0206d1d4.mUnk_04;
                data_0206d1d4.mUnk_04 = -1;
                gSystemState.mFlags |= 2 << 16;
                StartFadeClear(1);
            }
        }
    }
    if (data_0206d1d4.mUnk_00 == 0) {
        if (func_0200f95c()) {
            if (++data_0206d1d4.mUnk_08 >= 60) {
                data_0206d1d4.mUnk_04 = 1;
                data_0206d1d4.mUnk_08 = 0;
                StartFade(1);
                gSystemState.mFlags &= ~(2 << 16);
            }
        }
    } else if (data_0206d1d4.mUnk_00 == 1) {
        if (data_0206d1d4.mUnk_0c != 0) {
            data_0206d1d4.mUnk_08++;
        }
        data_0206d1d4.mUnk_0c = 1;
        switch (data_020b10fc.mUnk_0c) {
            case 1:
                if (data_0206d1d4.mUnk_08 >= 54 && data_0206d1d4.mUnk_14 > 0) {
                    old = data_0206d1d4.mUnk_14;
                    data_0206d1d4.mUnk_0c = 0;
                    prev = data_0206d1d4.mUnk_14 - data_0206d1d4.mUnk_18 / 60;
                    data_0206d1d4.mUnk_14 = prev;
                    if (prev < 0) {
                        prev = 0;
                    }
                    data_0206d1d4.mUnk_14 = prev;
                    data_0206d1c0.mUnk_04 += old - prev;
                    whole = data_0206d1c0.mUnk_04 >> 8;
                    if (whole != 0) {
                        if (func_02014f98(&data_020b1990[data_0206d1d4.mUnk_10], whole)) {
                            data_0206d1d4.mUnk_14 = 0;
                        }
                        data_0206d1c0.mUnk_04 -= data_0206d1c0.mUnk_04 & ~0xff;
                    }
                }
                if (data_0206d1d4.mUnk_08 >= 72) {
                    if (data_020b10fc.mScore > data_020b10fc.mBestScores[data_020b10fc.mMode]) {
                        if (data_0206d1d4.mUnk_08 >= 90) {
                            data_0206d1d4.mUnk_24 = 1;
                            data_020b10fc.mBestScores[data_020b10fc.mMode] = data_020b10fc.mScore;
                        }
                    } else if (data_0206d1d4.mUnk_24 == 0) {
                        data_0206d1d4.mUnk_2c = 1;
                    }
                }
                if (data_0206d1d4.mUnk_08 >= 108 && data_0206d1d4.mUnk_24 != 0) {
                    old = data_0206d1d4.mUnk_1c;
                    if (old > 0) {
                        data_0206d1d4.mUnk_0c = 0;
                        prev = data_0206d1d4.mUnk_1c - data_0206d1d4.mUnk_20 / 60;
                        data_0206d1d4.mUnk_1c = prev;
                        if (prev < 0) {
                            prev = 0;
                        }
                        data_0206d1d4.mUnk_1c = prev;
                        data_0206d1c0.mUnk_00 += old - prev;
                        whole = data_0206d1c0.mUnk_00 >> 8;
                        if (whole != 0) {
                            if (func_02014f98(&data_020b1990[data_0206d1d4.mUnk_10], whole)) {
                                data_0206d1d4.mUnk_1c = 0;
                            }
                            data_0206d1c0.mUnk_00 -= data_0206d1c0.mUnk_00 & ~0xff;
                        }
                    } else {
                        data_0206d1d4.mUnk_2c = 1;
                    }
                }

                break;
            case 0:
                if (data_0206d1d4.mUnk_08 >= 36 && data_0206d1d4.mUnk_28 < 0) {
                    old = data_0206d1d4.mUnk_28;
                    data_0206d1d4.mUnk_0c = 0;
                    data_0206d1d4.mUnk_28 = old + 128;
                    if (data_0206d1d4.mUnk_28 > 0) {
                        data_0206d1d4.mUnk_28 = 0;
                    }
                    data_0206d1c0.mUnk_08 += abs(old) - abs(data_0206d1d4.mUnk_28);
                    whole = data_0206d1c0.mUnk_08 >> 8;
                    if (whole != 0) {
                        u32 left = data_020b1990[data_0206d1d4.mUnk_10].mInfo.mUnk_12;
                        if (whole <= left) {
                            data_020b1990[data_0206d1d4.mUnk_10].mInfo.mUnk_12 = left - whole;
                        } else {
                            data_020b1990[data_0206d1d4.mUnk_10].mInfo.mUnk_12 = 0;
                            data_0206d1d4.mUnk_28 = 0;
                        }
                        data_0206d1c0.mUnk_08 -= data_0206d1c0.mUnk_08 & ~0xff;
                    }
                }
                if (data_0206d1d4.mUnk_28 == 0) {
                    data_0206d1d4.mUnk_2c = 1;
                }
                break;
        }
        if (func_0200bd80(0x20000000, 0x101)) {
            if (data_0206d1d4.mUnk_30 < 0) {
                func_0200494c();
            } else {
                data_0206d1d4.mUnk_04 = 2;
                data_0206d1d4.mUnk_08 = 0;
                StartFade(3);
                gSystemState.mFlags &= ~(2 << 16);
            }
        }
    } else if (data_0206d1d4.mUnk_00 == 2) {
        if (func_0200bd80(0x20000000, 0x101)) {
            func_0200494c();
        }
    }
    func_02004aa8();
}

void func_0200548c(void) {
    func_020102f8();
    gSystemState.mFlags |= 2 << 16;
}
