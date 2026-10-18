#include <Main/Unknown/functions.h>
#include <Main/Unknown/sound_control.h>
#include <Main/Unknown/sprite_slots.h>
#include <Main/Unknown/state_flow.h>
#include <Main/Unknown/game_util.h>
#include <Main/Unknown/globals.h>
#include <Main/Unknown/prompt_screen.h>
#include <nitro/os.h>

#define PROMPT_STATE_SAVED 0x400000

// Saves the display setup and button flags, then starts the prompt: returns 0 when there is nothing to show.
int func_0200fb68(int *a, int *b) {
    Unk_020b1124 *st;
    int i;

    if (data_020c1d3c.mUnk_1c != 0xffff || (data_020c1cbc.mUnk_6c & 0x80)) {
        st = &data_020b1124;
        func_0201f880(3);
        if (!(data_020c1cbc.mUnk_6c & PROMPT_STATE_SAVED)) {
            st->mUnk_28 = 0;
            st->mUnk_2c = data_020c1b5c.mUnk_0c_4;
            st->mUnk_30 = data_020c1b5c.mUnk_0c_1;
            st->mUnk_34 = data_020c1b5c.mUnk_0c_2;
            st->mUnk_38 = data_020c1b5c.mUnk_0c_0;
            st->mUnk_3c = data_020c1b5c.mUnk_10;
            st->mUnk_40 = data_020c1b5c.mUnk_0c_5;
            data_020c1b5c.mUnk_0c_4 = data_020c1b5c.mUnk_0c_1 = data_020c1b5c.mUnk_0c_2 = data_020c1b5c.mUnk_0c_0 = 0;
            data_020c1b5c.mUnk_10 = 0;
            data_020c1b5c.mUnk_0c_5 = 1;
            data_020c1cbc.mUnk_6c |= PROMPT_STATE_SAVED;
        } else {
            st->mUnk_28 = 1;
        }
        st->mUnk_0c = data_020c1b9c.mUnk_10;
        st->mUnk_10 = data_020c1b9c.mUnk_1a;
        st->mUnk_24 = *(vu16 *) 0x05000400;
        st->mUnk_26 = *(vu16 *) 0x05000402;
        data_020c1b9c.mUnk_1a = 0;
        data_020c1b9c.mUnk_10 = data_020c1b9c.mUnk_1a;
        st->mUnk_14 = data_020c1b3c.mUnk_54;
        st->mUnk_18 = data_020c1b3c.mUnk_58;
        st->mUnk_1c = data_020c1b3c.mUnk_5c;
        st->mUnk_20 = data_020c1b3c.mUnk_60;
        data_020c1b3c.mUnk_60 = 0;
        data_020c1b3c.mUnk_5c = 0;
        data_020c1b3c.mUnk_58 = 0;
        data_020c1b3c.mUnk_54 = 0;
        st->mUnk_6c = *(vu16 *) 0x04001050;
        st->mUnk_6e = *(vu16 *) 0x04001052;
        for (i = 0; i < 2; i++) {
            st->mUnk_00[i] = data_020c18bc.mUnk_49c[i];
        }
        if (st->mUnk_00[0] != 0xffff || st->mUnk_00[1] != 0xffff) {
            if (st->mUnk_00[0] != 0xffff) {
                u16 r = func_0201db94(1);
                if (r == 0x80) {
                    OS_Terminate();
                }
                func_0201ddec(r == 0x100 ? 0x10d3 : 0x10d4, 1, 0, 2);
            }
            if (st->mUnk_00[1] != 0xffff) {
                u16 r = func_0201db94(2);
                if (r == 0x80) {
                    OS_Terminate();
                }
                func_0201ddec(r == 0x100 ? 0x10d3 : 0x10d4, 2, 0, 2);
            }
        }
        func_02034768(0, 0, 0);
        if (func_02020878() == 0) {
            if (a != 0) {
                *a = 2;
            }
            if (b != 0) {
                *b = 1;
            }
        } else {
            if (a != 0) {
                *a = 1;
            }
            if (b != 0) {
                *b = 2;
            }
        }
        st->mUnk_68 = 0;
        st->mUnk_64 = 0;
        st->mUnk_44 = 0x18000;
        st->mUnk_48 = 0x20000;
        st->mUnk_4c = -0x8000;
        *(vu16 *) 0x04001050 = 0;
        *(vu16 *) 0x04001052 = 0;
        data_020c14a0.mUnk_10 = 1;
        return 1;
    }
    return 0;
}

// Restores what func_0200fb68 saved.
void func_0200fd90(void) {
    Unk_020b1124 *st = &data_020b1124;

    func_0201f880(3);
    if (st->mUnk_00[0] != 0xffff || st->mUnk_00[1] != 0xffff) {
        if (st->mUnk_00[0] == st->mUnk_00[1]) {
            func_0201ddec(st->mUnk_00[0], 3, 0, 2);
        } else {
            if (st->mUnk_00[0] != 0xffff) {
                func_0201ddec(st->mUnk_00[0], 1, 0, 2);
            }
            if (st->mUnk_00[1] != 0xffff) {
                func_0201ddec(st->mUnk_00[1], 2, 0, 2);
            }
        }
    }
    data_020c1b3c.mUnk_54 = st->mUnk_14;
    data_020c1b3c.mUnk_58 = st->mUnk_18;
    data_020c1b3c.mUnk_5c = st->mUnk_1c;
    data_020c1b3c.mUnk_60 = st->mUnk_20;
    data_020c1b9c.mUnk_10 = st->mUnk_0c;
    data_020c1b9c.mUnk_1a = st->mUnk_10;
    *(vu16 *) 0x05000400 = st->mUnk_24;
    *(vu16 *) 0x05000402 = st->mUnk_26;
    *(vu16 *) 0x04001050 = st->mUnk_6c;
    *(vu16 *) 0x04001052 = st->mUnk_6e;
    if (st->mUnk_28 == 0) {
        data_020c1b5c.mUnk_0c_4 = st->mUnk_2c;
        data_020c1b5c.mUnk_0c_1 = st->mUnk_30;
        data_020c1b5c.mUnk_0c_2 = st->mUnk_34;
        data_020c1b5c.mUnk_0c_0 = st->mUnk_38;
        data_020c1b5c.mUnk_10 = st->mUnk_3c;
        data_020c1b5c.mUnk_0c_5 = st->mUnk_40;
        data_020c1cbc.mUnk_6c &= ~PROMPT_STATE_SAVED;
    }
}

// Draws the prompt: the backdrop tiles, the text and the two buttons.
void func_0200feb8(void) {
    int i;
    int flag;
    int mode;

    if (data_020b1124.mUnk_64 == 1) {
        flag = 1;
    } else {
        flag = 0;
    }
    if (data_020c1d3c.mUnk_1c == 0xffff) {
        for (i = 0; (u32) i < 12; i++) {
            func_0202cf08(0x8011ea + i, (i & 3) << 6, (u32) i / 4 << 6, 250, 1);
        }
    }
    if (data_020b1124.mUnk_00[1] == 0xffff) {
        for (i = 0; (u32) i < 12; i++) {
            func_0202cf08(0x11ea + i, (i & 3) << 6, (u32) i / 4 << 6, 250, 2);
        }
    }
    mode = func_02020878() != 0 ? 1 : 2;
    func_020215fc(128, 96, data_0201a64e, func_0200f9f0(36), 2, 200, 16, mode);
    func_0202cf08(0xe22, 47, (data_020b1124.mUnk_4c >> 8) + 17, 140, func_02020878() != 0 ? 2 : 1);
    mode = func_02020878() != 0 ? 2 : 1;
    func_020215fc(47, (data_020b1124.mUnk_4c >> 8) + 17, data_0201a61c, func_0200f9f0(36), 0, 200, 16, mode);
    func_0200bc50((data_020b1124.mUnk_44 >> 8) + 128, 82, 0, 0, 140, 0x400, 0xdda, func_0200f9f0(37), data_0201a61c, 2,
                  0, 0, flag, 0, 0, 0);
    func_0200bc50((data_020b1124.mUnk_48 >> 8) + 128, 122, 0, 0, 140, 0x401, 0xdda, func_0200f9f0(38), data_0201a61c, 2,
                  0, 0, flag, 0, 0, 0);
}

// Runs the prompt: slides the buttons in, waits for a button, slides them out and restores the display.
void func_0201004c(void) {
    int result;
    int done = 0;
    int running = 1;

    if (func_0200fb68(&result, 0) == 1) {
        func_0202eeec();
        func_02010500(1);
        do {
            if (func_0201f794(3, 3) == 1 && data_020c8254.mUnk_08 == 0 && (data_020c18bc.mUnk_12 & 8)) {
                running = 0;
            }
            switch (data_020b1124.mUnk_64) {
                case 0: {
                    s32 v = data_020b1124.mUnk_44 - 0x1e00;
                    if (v < 0) {
                        v = 0;
                    }
                    data_020b1124.mUnk_44 = v;
                    v = data_020b1124.mUnk_48 - 0x1e00;
                    if (v < 0) {
                        v = 0;
                    }
                    data_020b1124.mUnk_48 = v;
                    v = data_020b1124.mUnk_4c + 0x780;
                    if (v >= 0) {
                        v = 0;
                    }
                    data_020b1124.mUnk_4c = v;
                    if (data_020b1124.mUnk_44 == 0 && data_020b1124.mUnk_48 == 0 && v == 0) {
                        data_020b1124.mUnk_64 = 1;
                    }
                    break;
                }
                case 2: {
                    s32 v = data_020b1124.mUnk_44 - 0x1e00;
                    s32 w;
                    if (v < -0x18000) {
                        v = -0x18000;
                    }
                    data_020b1124.mUnk_44 = v;
                    if (v <= -0x8000) {
                        v = data_020b1124.mUnk_48 - 0x1e00;
                        if (v < -0x18000) {
                            v = -0x18000;
                        }
                        data_020b1124.mUnk_48 = v;
                    }
                    w = data_020b1124.mUnk_4c - 0x780;
                    if (w < -0x4000) {
                        w = -0x4000;
                    }
                    data_020b1124.mUnk_4c = w;
                    if (data_020b1124.mUnk_44 == -0x18000 && data_020b1124.mUnk_48 == -0x18000 && w == -0x4000) {
                        done = 1;
                    }
                    break;
                }
            }
            func_0200feb8();
            if (func_0200bd80(0x20000000, 0x400)) {
                data_020b1124.mUnk_64 = 2;
                data_020b1124.mUnk_68 = 0x400;
            }
            if (func_0200bd80(0x20000000, 0x401)) {
                data_020b1124.mUnk_64 = 2;
                data_020b1124.mUnk_68 = 0x401;
            }
            if (data_020b1124.mUnk_68 != 0 && done != 0) {
                switch (data_020b1124.mUnk_68) {
                    case 0x400:
                        running = 0;
                        break;
                    case 0x401:
                        running = 0;
                        data_020c13e0.mUnk_38 = running;
                        func_02021458();
                        func_02014430(running);
                        func_0200fa80(1);
                        func_0200fab0();
                        break;
                }
            }
            func_020103c0();
            func_0201bb30();
        } while (running == 1);
        func_02010500(2);
        func_0200fd90();
    }
}

void func_020101d0(void) {
    int i;
    int mode;
    int id = data_020c1bbc.mUnk_2c + 4 + data_020b1078.mLanguage * 10;

    if (data_020c1d3c.mUnk_1c == 0xffff) {
        for (i = 0; (u32) i < 12; i++) {
            func_0202cf08(0x8011ea + i, (i & 3) << 6, (u32) i / 4 << 6, 250, 1);
        }
    }
    if (data_020b1124.mUnk_00[1] == 0xffff) {
        for (i = 0; (u32) i < 12; i++) {
            func_0202cf08(0x11ea + i, (i & 3) << 6, (u32) i / 4 << 6, 250, 2);
        }
    }
    mode = func_02020878() != 0 ? 2 : 1;
    func_020215fc(128, 90, data_0201a64e, (const char *) func_02021e18(0xfba, id + 6), 2, 200, 16, mode);
}

void func_02010294(int a, int b, int c) {
    func_0201f880(3);
    func_0201bb30();
    func_0201c41c();
    func_0201eb20();
    func_0202c1c4();
    func_02020854(c);
    func_0202bdd8(0x10cb, 0x10ca, 0x10cc, b, 1);
    func_0202bdd8(0x10cb, 0x10ca, 0x10cc, 2, 2);
    if (a != 0) {
        func_02021fb0(0x1c7, 0x1c6, a, 16);
        func_02024208();
    }
}

void func_020102f8(void) {
    func_0201f880(3);
    func_0201bb30();
    *(vu16 *) 0x04001050 = 0;
    *(vu16 *) 0x04000050 = 0;
    func_0200d6c8();
    func_0200dacc();
    if (data_020c1cbc.mUnk_6c & 0x80) {
        func_02022370();
    }
    data_020c14a0.mUnk_10 = 1;
}

void func_0201033c(int a, int b) {
    if (a >= 0x49) {
        OS_Terminate();
    }
    func_0200fb18(0x14, a);
    func_02010364(data_0204e96c[a].mUnk_00, b);
}

void func_02010364(int a, int b) {
    func_0200fb18(0x17, a);
    func_0200fb18(0x16, b);
    func_0200fa80(3);
    func_0200fab0();
}
