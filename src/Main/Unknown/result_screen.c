#include <Main/Unknown/functions.h>
#include <Main/Unknown/stage_progress.h>
#include <Main/Unknown/stage_select.h>
#include <Main/Unknown/stage_info_panel.h>
#include <Main/Unknown/game_util.h>
#include <Main/Unknown/move_object.h>
#include <Main/Unknown/prompt_screen.h>
#include <Main/Unknown/sound_control.h>
#include <Main/Unknown/language_menu.h>
#include <Main/Unknown/state_flow.h>
#include <Main/Unknown/globals.h>
#include <Main/Unknown/result_screen.h>
#include <Main/Unknown/mode_select.h>
#include <stdio.h>
#include <stdlib.h>

void func_0200494c(void) {
    func_02014bf0();
    func_02014430(0);
    func_0200fa80(10);
    func_0200fab0();
}

void func_02004964(void) {
    int i;
    int stage;
    int level;
    s32 v;
    stage = data_020b10fc.mUnk_0c == 1 ? 0x1d1 : 0x1ca;
    level = data_020b6ee0.mUnk_a8e4;
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
    v = func_02021e44(488, 5) << 8;
    data_0206d1d4.mUnk_14 = v;
    data_0206d1d4.mUnk_18 = (v << 8) >> 8;
    v = func_02021e44(488, 7) << 8;
    data_0206d1d4.mUnk_1c = v;
    data_0206d1d4.mUnk_20 = (v << 8) >> 8;
    data_0206d1d4.mUnk_24 = 0;
    data_0206d1d4.mUnk_28 = func_02021e44(488, 9) << 8;
    data_0206d1d4.mUnk_2c = 0;
    data_0206d1d4.mUnk_30 =
        data_020b10fc.mUnk_0c == 1 ? func_02014d2c(&data_020b1990[data_0206d1d4.mUnk_10].mInfo) : -1;
    data_0206d1d4.mUnk_34 = 0;
    data_0206d1d4.mUnk_38 = 0;
    data_0206d1d4.mUnk_3c = 5;
    data_0206d1d4.mUnk_40 = 0;
    for (i = 0; i < 8; i++) {
        data_0206d1d4.mUnk_48[i] = 0;
    }
    func_0200f848(data_0204e0cc[data_020b10fc.mUnk_0c]);
    func_0202de60(data_0206d23c, data_0201abec[3]);
    func_0202de60(data_0206d340, data_0201abec[4]);
    func_020104c0(data_020b10fc.mUnk_0c == 1 ? 7 : 8, 102);
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
        func_02011890(data_0206d1d4.mUnk_10, 2);
    } else {
        s32 t = data_0206d1d4.mUnk_3c;
        if (t < 0 || (t == 5 && data_0206d1d4.mUnk_38 != 0) ||
            (t >= 0 && t < 5 && data_020b1990[data_0206d1d4.mUnk_10].mInfo.mUnk_1c[t] < 0)) {
            func_02011890(data_0206d1d4.mUnk_10, 1);
        }
    }
    if (data_0206d1d4.mUnk_00 == 0) {
        func_0200f898();
        return;
    }
    if (data_0206d1d4.mUnk_00 == 1) {
        func_020215fc(51, 21, data_0201a61c, func_0200f9f0(data_020b10fc.mUnk_04 + 31), 0, 200, 16, 1);
        func_0202cf08(data_0204e0d4[data_020b10fc.mUnk_04], 25, 22, 130, 1);
        switch (data_020b10fc.mUnk_0c) {
            case 1:
                func_020215fc(89, 66, data_0201a64e, func_0200f9f0(44), 2, 200, 16, 1);
                if (data_0206d1d4.mUnk_08 >= 18) {
                    sprintf(text0, "%d", data_020b10fc.mUnk_10);
                    func_020215fc(35, 128, data_0201a64e, func_0200f9f0(43), 0, 200, 16, 1);
                    func_020215fc(180, 128, data_0201a64e, text0, 1, 200, 16, 1);
                }
                if (data_0206d1d4.mUnk_08 >= 36 && data_0206d1d4.mUnk_14 > 0) {
                    sprintf(text1, "+G %d", data_0206d1d4.mUnk_14 >> 8);
                    func_020215fc(140, 143, data_0201a61c, text1, 0, 200, 16, 1);
                }
                if (data_0206d1d4.mUnk_08 >= 72) {
                    sprintf(text2, "%d", data_020b1110[data_020b10fc.mUnk_04]);
                    func_020215fc(35, 148, data_0201a64e, func_0200f9f0(46), 0, 200, 16, 1);
                    func_020215fc(180, 148, data_0201a64e, text2, 1, 200, 16, 1);
                }
                if (data_0206d1d4.mUnk_24 != 0 && data_0206d1d4.mUnk_1c > 0) {
                    sprintf(text3, "+G %d", data_0206d1d4.mUnk_1c >> 8);
                    func_020215fc(140, 163, data_0201a61c, text3, 0, 200, 16, 1);
                }
                break;
            case 0:
                func_020215fc(89, 66, data_0201a64e, func_0200f9f0(45), 2, 200, 16, 1);
                sprintf(text4, "%d", data_020b10fc.mUnk_10);
                func_020215fc(35, 128, data_0201a64e, func_0200f9f0(43), 0, 200, 16, 1);
                func_020215fc(180, 128, data_0201a64e, text4, 1, 200, 16, 1);
                sprintf(text5, "%d", data_020b1110[data_020b10fc.mUnk_04]);
                func_020215fc(35, 148, data_0201a64e, func_0200f9f0(46), 0, 200, 16, 1);
                func_020215fc(180, 148, data_0201a64e, text5, 1, 200, 16, 1);
                if (data_0206d1d4.mUnk_08 >= 18) {
                    if (data_0206d1d4.mUnk_28 < 0) {
                        sprintf(text6, "%s %d", func_0200f9f0(10), data_0206d1d4.mUnk_28 >> 8);
                        func_020215fc(70, 163, data_0201a61c, text6, 0, 200, 16, 1);
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
    if (func_0201f794(3, 3)) {
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
        func_020215fc(70 + (data_0206d1d4.mUnk_48[0] >> 8), 37, data_0201a64e, func_0200f9f0(52), 0, 256, 16, 2);
        if (data_0206d1d4.mUnk_48[0] == 0 && data_0206d1d4.mUnk_48[1] == 0) {
            func_0202db1c(data_0206d23c);
            func_0202cf08(data_0206d1d4.mUnk_6c, 57, 103, 150, 2);
        }
        func_02013d48(&rec->mInfo, slot, data_0206d1d4.mUnk_3c == 5, 0x505, 57, (data_0206d1d4.mUnk_48[2] >> 8) + 103,
                      2, 0);
        if (data_0206d1d4.mUnk_3c == 5) {
            func_02013dc8(&rec->mInfo, slot, 1, 0);
        }
        if (func_0200bd80(0x80000000, 0x505) && ready) {
            data_0206d1d4.mUnk_3c = 5;
        }
        if (data_0206d314.mUnk_06 != 0) {
            func_0202db1c(data_0206d340);
            func_0202cf08(data_0206d2d4.mUnk_70, 0, 137, 130, 2);
            if (data_0206d414.mUnk_0a != 0) {
                func_0202cf08(0xfb4, data_0206d1d4.mUnk_40 * 26 + 116, 103, 141, 2);
            }
        }
        func_0200bc50((data_0206d1d4.mUnk_48[4] >> 8) + 181, 174, 0, 0, 140, 512, 0xfb5, func_0200f9f0(41),
                      data_0201a61c, 2, 0, 359, ready, 0, 0, 0);
        if (func_0200bd80(0x20000000, 0x200) && ready) {
            data_0206d1c0.mUnk_10 = 1;
            data_0206d1d4.mUnk_44 = rec->mInfo.mUnk_1c[data_0206d1d4.mUnk_40];
            rec->mInfo.mUnk_1c[data_0206d1d4.mUnk_40] = slot;
            data_0206d1d4.mUnk_34 = 1;
        }
        func_0200bc50((data_0206d1d4.mUnk_48[3] >> 8) + 73, 174, 0, 0, 140, 513, 0xfd7, func_0200f9f0(61),
                      data_0201a61c, 2, 0, 90, ready, 0, 0, 0);
        if (func_0200bd80(0x20000000, 0x201) && ready) {
            data_0206d1c0.mUnk_10 = 0;
            data_0206d1d4.mUnk_34 = 1;
        }
        if (func_0200bdac(0x80000000, 0x500, 0x504) && ready) {
            data_0206d1d4.mUnk_40 = (u16) data_020c2660.mUnk_04 - 0x500;
        }
    }
    if (func_0200bd80(0x20000000, 0x102)) {
        data_0206d1d4.mUnk_34 = 0;
        if (data_0206d1c0.mUnk_10 != 0) {
            rec->mInfo.mUnk_1c[data_0206d1d4.mUnk_40] = data_0206d1d4.mUnk_44;
        }
    }
    func_0202cf08(rec->mInfo.mUnk_13 + 0x1089, (data_0206d1d4.mUnk_48[1] >> 8) + 40, 45, data_020c1bbc.mUnk_08 - 4, 2);
    func_0202cf08(0x1088, (data_0206d1d4.mUnk_48[1] >> 8) + 40, 45, data_020c1bbc.mUnk_08 - 2, 2);
    func_02013f74(data_0206d1d4.mUnk_10, data_0206d210, 168, 103, 2, (ready == 1 ? 0 : 4) + 2);
    data_0206d1d4.mUnk_38 = data_0206d1d4.mUnk_34;
}

void func_02005194(void) {
    s32 old;
    s32 prev;
    s32 cur;
    s32 diff;
    u32 whole;
    if (data_0206d1d4.mUnk_04 >= 0) {
        if (data_0206d1d4.mUnk_04 == 2) {
            if (func_0201f794(3, 0)) {
                data_0206d1d4.mUnk_00 = data_0206d1d4.mUnk_04;
                data_0206d1d4.mUnk_04 = -1;
                func_020102f8();
                func_0200c5d8(1, data_020b1990[data_0206d1d4.mUnk_10].mInfo.mUnk_13 + 0xdb3, 456, 128, 16, 1, 130);
                data_0206d1d4.mUnk_48[0] = 1 << 16;
                data_0206d1d4.mUnk_48[1] = -0x8000;
                data_0206d1d4.mUnk_48[2] = -0x8000 << 2;
                data_0206d1d4.mUnk_48[3] = -0x48000;
                data_0206d1d4.mUnk_48[4] = 18 << 14;
                data_020c1cbc.mUnk_6c |= 2 << 16;
                func_0201f7f8(3);
            }
        } else {
            if (func_0201f794(1, 0)) {
                data_0206d1d4.mUnk_00 = data_0206d1d4.mUnk_04;
                data_0206d1d4.mUnk_04 = -1;
                data_020c1cbc.mUnk_6c |= 2 << 16;
                func_0201f7f8(1);
            }
        }
    }
    if (data_0206d1d4.mUnk_00 == 0) {
        if (func_0200f95c()) {
            if (++data_0206d1d4.mUnk_08 >= 60) {
                data_0206d1d4.mUnk_04 = 1;
                data_0206d1d4.mUnk_08 = 0;
                func_0201f7cc();
                data_020c1cbc.mUnk_6c &= ~(2 << 16);
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
                    if (data_0206d1c0.mUnk_04 >> 8 != 0) {
                        if (func_02014f98(&data_020b1990[data_0206d1d4.mUnk_10])) {
                            data_0206d1d4.mUnk_14 = 0;
                        }
                        data_0206d1c0.mUnk_04 -= data_0206d1c0.mUnk_04 & ~0xff;
                    }
                }
                if (data_0206d1d4.mUnk_08 >= 72) {
                    if (data_020b10fc.mUnk_10 > data_020b1110[data_020b10fc.mUnk_04]) {
                        if (data_0206d1d4.mUnk_08 >= 90) {
                            data_0206d1d4.mUnk_24 = 1;
                            data_020b1110[data_020b10fc.mUnk_04] = data_020b10fc.mUnk_10;
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
                        if (data_0206d1c0.mUnk_00 >> 8 != 0) {
                            if (func_02014f98(&data_020b1990[data_0206d1d4.mUnk_10])) {
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
                func_0201f7cc();
                data_020c1cbc.mUnk_6c &= ~(2 << 16);
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
    data_020c1cbc.mUnk_6c |= 2 << 16;
}
