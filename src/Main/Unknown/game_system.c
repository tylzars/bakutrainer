#include <nitro/os.h>
#include <Main/Unknown/state_flow.h>
#include <Main/Unknown/globals.h>
#include <Main/Unknown/functions.h>
#include <Main/Unknown/save_blocks.h>
#include <Main/Unknown/mic_input.h>
#include <Main/Unknown/random_angle.h>
#include <Main/Unknown/screen_fade.h>
#include <Main/Unknown/bg_image.h>
#include <Main/Unknown/system_init.h>
#include <Main/Unknown/stage_select.h>
#include <Main/Unknown/sound_control.h>
#include <Main/Unknown/sprite_slots.h>
#include <Main/Unknown/game_util.h>
#include <Main/Unknown/prompt_screen.h>
#include <Main/Unknown/save_data.h>
#include <Main/Unknown/stage_progress.h>
#include <Main/Unknown/stage_info_panel.h>
#include <Main/Unknown/language_menu.h>
#include <Main/Unknown/move_object.h>
#include <Main/Unknown/save_slot_menu.h>
#include <Main/clib.h>

// Objects of this file: save menu, settings, sound control, prompt, main loop and model viewer state.
Unk_020b1078 data_020b1078;
Unk_020b1084 data_020b1084;
s32 data_020b1094[8];
SoundCtl data_020b10b4;
StageInfo data_020b10d4;
Unk_020b10fc data_020b10fc;
Unk_020b1124 data_020b1124;
GameFlow data_020b1194;
Unk_020b122c data_020b122c;

u16 func_0200f9c8(int index) {
    u16 table[7] = {587, 590, 593, 595, 597, 599, 601};
    return table[index];
}

const char *func_0200f9f0(int id) {
    if (id != 0) {
        return (const char *) func_02021e18(func_0200f9c8(data_020b1084.mLanguage), id);
    }
    return 0;
}

const char *func_0200fa10(int id, int index) {
    return (const char *) func_02021e18(func_0200f9c8(index), id);
}

void func_0200fa24(void) {
    func_0200fa3c();
    data_020b1194.mState = 0;
    data_020b1194.mPrevState = 0;
    data_020b1194.mUnk_10 = 0;
}

void func_0200fa3c(void) {
    data_020b1194.mQueueCount = 0;
    data_020b1194.mUnk_94 = 0;
}

void func_0200fa50(void) {
    GameFlow *flow = &data_020b1194;
    StateFunc f;

    f = data_0204e908[flow->mState];
    if (f != NULL) {
        f();
    }
    f = data_0204e840[flow->mState];
    if (f != NULL) {
        f();
    }
}

void func_0200fa80(int state) {
    GameFlow *flow = &data_020b1194;
    u32 i;

    if (flow->mQueueCount >= 25) {
        OS_Terminate();
    }
    for (i = flow->mQueueCount; i != 0; i--) {
        flow->mQueue[i] = flow->mQueue[i - 1];
    }
    flow->mQueue[0] = state;
    flow->mQueueCount++;
}

void func_0200fab0(void) {
    func_0200fabc(0);
}

void func_0200fabc(u32 flag) {
    GameFlow *flow = &data_020b1194;
    u32 i;

    if (flow->mQueueCount == 0) {
        OS_Terminate();
    }
    flow->mNextState = flow->mQueue[0];
    flow->mUnk_94 = 0;
    if (flow->mQueueCount > 1) {
        for (i = 0; i < flow->mQueueCount - 1; i++) {
            flow->mQueue[i] = flow->mQueue[i + 1];
        }
    }
    flow->mQueueCount--;
    flow->mUnk_10 = flag;
}

u32 func_0200fb00(void) {
    return data_020b1194.mState;
}

u32 func_0200fb0c(void) {
    return data_020b1194.mNextState;
}

void func_0200fb18(u32 index, u32 value) {
    GameFlow *flow = &data_020b1194;

    if (index >= HANDLER_COUNT) {
        OS_Terminate();
    }
    flow->mHandlers[index] = value;
}

u32 func_0200fb34(u32 index) {
    GameFlow *flow = &data_020b1194;

    if (index >= HANDLER_COUNT) {
        OS_Terminate();
    }
    return flow->mHandlers[index];
}

void func_0200fb50(s32 value) {
    data_020b1194.mUnk_94 = value;
}

s32 func_0200fb5c(void) {
    return data_020b1194.mUnk_94;
}

// Flag of SystemState.mUnk_46c: the per-frame code samples the touch screen while it is set.
#define TOUCH_SAMPLING 0x400000

// Saves the display setup and button flags, then starts the prompt: returns 0 when there is nothing to show.
int func_0200fb68(int *a, int *b) {
    Unk_020b1124 *st;
    u32 i;

    if (data_020c18bc.mUnk_49c[0] != 0xffff || (data_020c18bc.mUnk_46c & 0x80)) {
        st = &data_020b1124;
        func_0201f880(3);
        if (!(data_020c18bc.mUnk_46c & TOUCH_SAMPLING)) {
            st->mUnk_28 = 0;
            st->mUnk_2c = data_020c18bc.mUnk_2ac_4;
            st->mUnk_30 = data_020c18bc.mUnk_2ac_1;
            st->mUnk_34 = data_020c18bc.mUnk_2ac_2;
            st->mUnk_38 = data_020c18bc.mUnk_2ac_0;
            st->mUnk_3c = data_020c18bc.mUnk_2b0;
            st->mUnk_40 = data_020c18bc.mUnk_2ac_5;
            data_020c18bc.mUnk_2ac_4 = data_020c18bc.mUnk_2ac_1 = data_020c18bc.mUnk_2ac_2 = data_020c18bc.mUnk_2ac_0 =
                0;
            data_020c18bc.mUnk_2b0 = 0;
            data_020c18bc.mUnk_2ac_5 = 1;
            data_020c18bc.mUnk_46c |= TOUCH_SAMPLING;
        } else {
            st->mUnk_28 = 1;
        }
        st->mUnk_0c = data_020c18bc.mUnk_2f0;
        st->mUnk_10 = data_020c18bc.mUnk_2fa;
        st->mUnk_24 = *(vu16 *) 0x05000400;
        st->mUnk_26 = *(vu16 *) 0x05000402;
        data_020c18bc.mUnk_2fa = 0;
        data_020c18bc.mUnk_2f0 = data_020c18bc.mUnk_2fa;
        st->mUnk_14 = data_020c18bc.mUnk_2d4;
        st->mUnk_18 = data_020c18bc.mUnk_2d8;
        st->mUnk_1c = data_020c18bc.mUnk_2dc;
        st->mUnk_20 = data_020c18bc.mUnk_2e0;
        data_020c18bc.mUnk_2e0 = 0;
        data_020c18bc.mUnk_2dc = 0;
        data_020c18bc.mUnk_2d8 = 0;
        data_020c18bc.mUnk_2d4 = 0;
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
        data_020b6ee0.mUnk_a5d0 = 1;
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
    data_020c18bc.mUnk_2d4 = st->mUnk_14;
    data_020c18bc.mUnk_2d8 = st->mUnk_18;
    data_020c18bc.mUnk_2dc = st->mUnk_1c;
    data_020c18bc.mUnk_2e0 = st->mUnk_20;
    data_020c18bc.mUnk_2f0 = st->mUnk_0c;
    data_020c18bc.mUnk_2fa = st->mUnk_10;
    *(vu16 *) 0x05000400 = st->mUnk_24;
    *(vu16 *) 0x05000402 = st->mUnk_26;
    *(vu16 *) 0x04001050 = st->mUnk_6c;
    *(vu16 *) 0x04001052 = st->mUnk_6e;
    if (st->mUnk_28 == 0) {
        data_020c18bc.mUnk_2ac_4 = st->mUnk_2c;
        data_020c18bc.mUnk_2ac_1 = st->mUnk_30;
        data_020c18bc.mUnk_2ac_2 = st->mUnk_34;
        data_020c18bc.mUnk_2ac_0 = st->mUnk_38;
        data_020c18bc.mUnk_2b0 = st->mUnk_3c;
        data_020c18bc.mUnk_2ac_5 = st->mUnk_40;
        data_020c18bc.mUnk_46c &= ~TOUCH_SAMPLING;
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
    if (data_020c18bc.mUnk_49c[0] == 0xffff) {
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
                        data_020b6ee0.mUnk_a538 = running;
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
    int id = data_020c18bc.mUnk_32c + 4 + data_020b1084.mLanguage * 10;

    if (data_020c18bc.mUnk_49c[0] == 0xffff) {
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
    if (data_020c18bc.mUnk_46c & 0x80) {
        func_02022370();
    }
    data_020b6ee0.mUnk_a5d0 = 1;
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

#define SE_COUNT 0x78

void func_02010388(u32 a, s32 b) {
    SoundCtl *ctl = &data_020b10b4;

    if (a >= 9) {
        OS_Terminate();
    }
    data_020c8214.mUnk_10 |= 2;
    ctl->mUnk_14 = 0;
    ctl->mUnk_18 = 0;
    ctl->mUnk_08 = b;
    ctl->mUnk_0c = b;
    ctl->mUnk_00 = 0;
    ctl->mUnk_1c = 0;
    ctl->mUnk_14 = a;
    func_020103c0();
}

void func_020103c0(void) {
    SoundCtl *ctl = &data_020b10b4;
    u8 volume;

    if (data_020c8254.mUnk_08 != 0) {
        return;
    }
    volume = data_020c8214.mUnk_24 == 1 ? func_02038694() : ctl->mUnk_0c;
    if (data_020c8214.mUnk_24 == 0 && ctl->mUnk_18 != 0) {
        ctl->mUnk_18 = 0;
    }
    switch (ctl->mUnk_00) {
        case 1:
            if (volume != 0) {
                func_02038578(volume - 1, 0);
            } else {
                ctl->mUnk_00 = 0;
            }
            break;
        case 3:
            if (volume > ctl->mUnk_0c) {
                func_02038578(volume - 1, 0);
            } else {
                ctl->mUnk_00 = 0;
            }
            break;
        case 2:
            if (volume < ctl->mUnk_0c) {
                func_02038578(volume + 1, 0);
            } else {
                ctl->mUnk_00 = 0;
            }
            break;
    }
    if (ctl->mUnk_1c != 0) {
        ctl->mUnk_1c--;
        if (ctl->mUnk_1c == 0) {
            func_020104c0(ctl->mUnk_10, ctl->mUnk_04);
        }
    }
    if (ctl->mUnk_14 != ctl->mUnk_18) {
        ctl->mUnk_00 = 0;
        ctl->mUnk_0c = ctl->mUnk_08;
        ctl->mUnk_18 = ctl->mUnk_14;
        if (ctl->mUnk_18 >= 9) {
            OS_Terminate();
        }
        if (ctl->mUnk_18 == 0) {
            func_020359d0(1);
            return;
        }
        {
            static const u16 ids[9] = {0, 362, 49, 364, 363, 216, 92, 228, 227};
            func_02035108(ids[ctl->mUnk_18], 0x10d7, ctl->mUnk_0c, 0, 1, 0);
        }
    }
}

void func_020104c0(u32 a, int b) {
    SoundCtl *ctl = &data_020b10b4;

    if (a >= 9) {
        OS_Terminate();
    }
    ctl->mUnk_1c = 0;
    ctl->mUnk_14 = a;
    ctl->mUnk_08 = b;
}

void func_020104e0(u32 a, int b, int c) {
    SoundCtl *ctl = &data_020b10b4;

    if (a >= 9) {
        OS_Terminate();
    }
    ctl->mUnk_1c = c;
    ctl->mUnk_10 = a;
    ctl->mUnk_04 = b;
}

void func_02010500(u32 a) {
    SoundCtl *ctl = &data_020b10b4;

    if (a >= 4) {
        OS_Terminate();
    }
    ctl->mUnk_00 = a;
    if (a == 3) {
        ctl->mUnk_0c = 0x40;
    }
}

void func_02010520(u32 index, u16 volume, int b, int c, u32 d) {
    SeEntry *e;
    u32 pick;
    u16 id;

    if (index == 0 || index >= SE_COUNT) {
        OS_Terminate();
    }
    e = &data_0204edfc[index];
    pick = func_0201fd68() % e->mUnk_00;
    if (pick >= 6) {
        OS_Terminate();
    }
    id = e->mUnk_18[pick];
    if (id != 0 && e->mUnk_14 >= e->mUnk_10) {
        func_0202eb64(id, volume, 0, e->mUnk_04, 0xffff, 0);
        if (b != 0x100) {
            func_0202f240(id, b);
        }
        e->mUnk_14 = 0;
        e->mUnk_10 = c;
        if (e->mUnk_0c != 0) {
            e->mUnk_10 += func_0201fd68() % d;
        }
    } else {
        e->mUnk_14++;
    }
}

void func_020105c0(u32 index) {
    SeEntry *e;

    if (index == 0 || index >= SE_COUNT) {
        OS_Terminate();
    }
    e = &data_0204edfc[index];
    func_02010520(index, e->mUnk_02, 0x100, e->mUnk_08, e->mUnk_0c);
}

void func_020105f4(void *obj) {
    SeOwner *owner = obj;
    u32 index = owner->mUnk_12;
    SeEntry *e;

    if (index != 0) {
        if (index == 0 || index >= SE_COUNT) {
            OS_Terminate();
        }
        e = &data_0204edfc[index];
        if (e->mUnk_05 == 1) {
            func_0202f110(e->mUnk_18[0], e->mUnk_02, 1, e->mUnk_04, 0xffff, 0);
            return;
        }
        func_020105c0(index);
        owner->mUnk_12 = 0;
    }
}

#define STAGE_UNIT 51200

void func_02010648(u32 a) {
    u32 state;
    u32 lang10;
    u32 base;
    u32 id;

    if (a > data_020c18bc.mUnk_32e) {
        func_02021e44(SAVE_BLOCK_ID, 0x1c9);
        state = a;
        lang10 = data_020b1084.mLanguage * 10;
        id = data_020c18bc.mUnk_32c + 4 + lang10;
        base = data_020c18bc.mUnk_32c + 9;
        func_0201fed4(0);
        if (a != 2) {
            data_020c18bc.mUnk_324 = data_020c18bc.mUnk_45c;
            func_0201f880(3);
            func_02020854(1);
            func_0202c1c4();
            func_02022370();
            func_0201eb74(6);
            func_0201eb74(2);
            func_0201dde0(0xdaa, 3);
            func_0202bdd8(0x10cb, 0x10ca, 0x10cc, 2, 2);
        }
        switch (a) {
            case 1:
            case 2:
                id += 1;
                state = 1;
                break;
            case 5:
                data_020c18bc.mUnk_450 |= 2;
                data_020c18bc.mUnk_457 = 0x78;
                id += 4;
                break;
            case 3:
                id += 2;
                break;
            case 4:
                id += 3;
                break;
            case 6:
                id += 6;
                break;
            case 10:
                break;
        }
        func_020215fc(0x80, 0x60, data_0201a6c6, (const char *) func_02021e18(SAVE_BLOCK_ID, id), 2, 0xfa, 0x10, 2);
        if (a == 2) {
            func_020215fc(0x80, 0x88, data_0201a6c6, (const char *) func_02021e18(SAVE_BLOCK_ID, base + lang10), 2,
                          0xfa, 0x10, 2);
        }
        func_0201bb30();
        data_020c18bc.mUnk_46c |= 0x80000;
        func_0201f830(3);
        data_020c18bc.mUnk_32e = state;
    }
}

int func_0201079c(u32 slot) {
    int ok = 0;
    SaveRecord *rec = func_0201ffbc();
    Unk_020b6ee0 *work = &data_020b6ee0;
    int i;
    int j;
    int v;
    StageRec *stage;

    func_02020140(slot, rec, SAVE_BLOCK_ID);
    if (rec->mUnk_10 == 1) {
        ok = 1;
        work->mUnk_a530 = rec->mUnk_14;
        work->mUnk_a560 = rec->mUnk_18;
        work->mUnk_a564 = rec->mUnk_28;
        work->mUnk_a56c = rec->mUnk_2c;
        work->mUnk_a56d = rec->mUnk_30;
        work->mUnk_a56e = rec->mUnk_34;
        work->mUnk_a56f = rec->mUnk_38;
        v = rec->mUnk_3c;
        if (v == 99) {
            v = -1;
        }
        work->mUnk_a120 = v;
        v = rec->mUnk_40;
        if (v == 999) {
            v = -1;
        }
        work->mUnk_a128 = v;
        func_02011bcc(work->mUnk_a128, work->mUnk_a120, 480);
        data_020b10fc.mUnk_14[0] = rec->mUnk_44;
        data_020b10fc.mUnk_14[1] = rec->mUnk_48;
        data_020b10fc.mUnk_14[2] = rec->mUnk_4c;
        data_020b10fc.mUnk_14[3] = rec->mUnk_50;
        data_020b10fc.mUnk_14[4] = rec->mUnk_54;
        data_020b6ee0.mUnk_a8ee = 0;
        for (i = 0; i < STAGE_MAX; i++) {
            if (rec->mUnk_5c[i] == 1) {
                stage = &data_020b1990[func_02014de4(0, i)];
                stage->mUnk_1e4 = (rec->mUnk_f8[i] - 0x100) * STAGE_UNIT;
                stage->mUnk_1e8 = 0;
                stage->mUnk_1ec = (rec->mUnk_194[i] - 0x100) * STAGE_UNIT;
                stage->mInfo.mUnk_12 = rec->mUnk_230[i];
                stage->mInfo.mUnk_04 = rec->mUnk_2cc[i];
                stage->mInfo.mUnk_11 = rec->mUnk_368[i];
                stage->mInfo.mUnk_1c[0] = rec->mUnk_404[i] == 0 ? -1 : func_020144e0(rec->mUnk_404[i] - 1, 2);
                stage->mInfo.mUnk_1c[1] = rec->mUnk_4a0[i] == 0 ? -1 : func_020144e0(rec->mUnk_4a0[i] - 1, 2);
                stage->mInfo.mUnk_1c[2] = rec->mUnk_53c[i] == 0 ? -1 : func_020144e0(rec->mUnk_53c[i] - 1, 2);
                stage->mInfo.mUnk_1c[3] = rec->mUnk_5d8[i] == 0 ? -1 : func_020144e0(rec->mUnk_5d8[i] - 1, 2);
                stage->mInfo.mUnk_1c[4] = rec->mUnk_674[i] == 0 ? -1 : func_020144e0(rec->mUnk_674[i] - 1, 2);
            }
        }
        for (i = 0; i < 3; i++) {
            u8 slotStage = rec->mUnk_1c[i];
            if (slotStage != 0) {
                u8 s = slotStage - 1;
                if (s >= STAGE_MAX) {
                    OS_Terminate();
                }
                j = func_02014f14(0, s);
                if (j == -1) {
                    OS_Terminate();
                }
                func_02015114(i, j);
            } else {
                func_02015114(i, -1);
            }
        }
    }
    func_0201ffcc(&rec);
    return ok;
}

void func_020109f4(u32 slot) {
    SaveRecord *rec = func_0201ffbc();
    Unk_020b6ee0 *work = &data_020b6ee0;
    int i;
    int v;
    StageRec *stage;

    func_02010d4c(rec);
    rec->mUnk_10 = 1;
    rec->mUnk_14 = data_020b6ee0.mUnk_a530;
    rec->mUnk_18 = data_020b6ee0.mUnk_a560;
    rec->mUnk_28 = data_020b6ee0.mUnk_a564;
    rec->mUnk_2c = data_020b6ee0.mUnk_a56c;
    rec->mUnk_30 = data_020b6ee0.mUnk_a56d;
    rec->mUnk_34 = data_020b6ee0.mUnk_a56e;
    rec->mUnk_38 = data_020b6ee0.mUnk_a56f;
    if (data_020b6ee0.mUnk_a128 == -1) {
        v = work->mUnk_a12c;
        if (v == -1) {
            v = 999;
        }
        rec->mUnk_40 = v;
        v = work->mUnk_a124;
        if (v == -1) {
            v = 99;
        }
        rec->mUnk_3c = v;
    } else {
        v = work->mUnk_a128;
        if (v == -1) {
            v = 999;
        }
        rec->mUnk_40 = v;
        v = work->mUnk_a120;
        if (v == -1) {
            v = 99;
        }
        rec->mUnk_3c = v;
    }
    rec->mUnk_44 = data_020b10fc.mUnk_14[0];
    rec->mUnk_48 = data_020b10fc.mUnk_14[1];
    rec->mUnk_4c = data_020b10fc.mUnk_14[2];
    rec->mUnk_50 = data_020b10fc.mUnk_14[3];
    rec->mUnk_54 = data_020b10fc.mUnk_14[4];
    for (i = 0; i < 3; i++) {
        u8 slotStage = 0;
        stage = func_02015190(i);
        if (stage != NULL) {
            slotStage = stage->mInfo.mUnk_13;
            if (slotStage >= STAGE_MAX) {
                OS_Terminate();
            }
            slotStage = slotStage + 1;
        }
        rec->mUnk_1c[i] = slotStage;
    }
    for (i = 0; i < STAGE_MAX; i++) {
        rec->mUnk_5c[i] = 0;
    }
    rec->mUnk_58 = data_020b6ee0.mUnk_a8ee;
    for (i = 0; i < data_020b6ee0.mUnk_a8ee; i++) {
        u8 id;
        stage = &data_020b1990[i];
        id = stage->mInfo.mUnk_13;
        if (id >= STAGE_MAX) {
            OS_Terminate();
        }
        if (rec->mUnk_5c[id] == 1) {
            OS_Terminate();
        }
        rec->mUnk_5c[id] = 1;
        v = stage->mUnk_1e4 / STAGE_UNIT;
        if (v <= -0x100 || v >= 0x100) {
            OS_Terminate();
        }
        rec->mUnk_f8[id] = v + 0x100;
        v = stage->mUnk_1ec / STAGE_UNIT;
        if (v <= -0x100 || v >= 0x100) {
            OS_Terminate();
        }
        rec->mUnk_194[id] = v + 0x100;
        rec->mUnk_230[id] = stage->mInfo.mUnk_12;
        rec->mUnk_2cc[id] = stage->mInfo.mUnk_04;
        rec->mUnk_368[id] = stage->mInfo.mUnk_11;
        rec->mUnk_404[id] = func_020144a4(stage->mInfo.mUnk_1c[0]) + 1;
        rec->mUnk_4a0[id] = func_020144a4(stage->mInfo.mUnk_1c[1]) + 1;
        rec->mUnk_53c[id] = func_020144a4(stage->mInfo.mUnk_1c[2]) + 1;
        rec->mUnk_5d8[id] = func_020144a4(stage->mInfo.mUnk_1c[3]) + 1;
        rec->mUnk_674[id] = func_020144a4(stage->mInfo.mUnk_1c[4]) + 1;
    }
    func_02020258(slot, rec, SAVE_BLOCK_ID);
    func_0201ffcc(&rec);
}

void func_02010c94(void) {
    SaveRecord *buf = func_0201ffbc();
    int i;
    int usedDefaults = 0;
    for (i = 0; (u32) i < SAVE_SLOT_COUNT; i++) {
        func_02020140(i, buf, SAVE_BLOCK_ID);
        if (buf->mUnk_00[0] == 1) {
            data_020b1084.mLanguage = buf->mUnk_00[1];
            data_020b1084.mUnk_04 = buf->mUnk_00[2];
            data_020b1084.mUnk_08 = buf->mUnk_00[3];
            break;
        }
    }
    if (i == SAVE_SLOT_COUNT) {
        func_02010d34();
        usedDefaults = 1;
    }
    func_0201ffcc(&buf);
    if (usedDefaults == 1) {
        data_020c18bc.mUnk_31c(6);
        func_02010cfc();
    }
}

void func_02010cfc(void) {
    SaveRecord *buf = func_0201ffbc();
    int i;
    for (i = 0; (u32) i < SAVE_SLOT_COUNT; i++) {
        func_02020140(i, buf, SAVE_BLOCK_ID);
        func_02010d4c(buf);
        func_02020258(i, buf, SAVE_BLOCK_ID);
    }
    func_0201ffcc(&buf);
}

void func_02010d34(void) {
    data_020b1084.mLanguage = func_0200f9b8();
    data_020b1084.mUnk_04 = 1;
    data_020b1084.mUnk_08 = 3;
}

void func_02010d4c(SaveRecord *rec) {
    rec->mUnk_00[0] = 1;
    rec->mUnk_00[1] = data_020b1084.mLanguage;
    rec->mUnk_00[2] = data_020b1084.mUnk_04;
    rec->mUnk_00[3] = data_020b1084.mUnk_08;
}

#define FONT_STRIDE 0xa
#define ENTRY_BLOCK 0x236
#define ENTRY_STRIDE 0x25

void func_02010d64(void) {
    func_0201c41c();
    func_0201eb20();
    func_0202c1c4();
    func_0202bdd8(0x10cb, 0x10ca, 0x10cc, 0, 2);
    func_0201dde0(0xdaa, 1);
    func_0201dde0(0xde7, 2);
    func_02020854(1);
    func_020104c0(0, 0x80);
    data_020b1078.mUnk_00 = 0;
    data_020b10d4.mUnk_11 = 1;
    data_020b1078.mUnk_04 = 0;
    data_020b1078.mUnk_08 = 0;
}

void func_02010dc8(void) {
    int start;
    int i;
    int rows;
    int last;
    int y;
    int k;
    int j;
    int n;
    u32 pad;
    int grade;
    if (data_020c18bc.mUnk_12 & 0x200) {
        data_020b1078.mUnk_08 = 1 - data_020b1078.mUnk_08;
    }
    if (data_020b1078.mUnk_08 == 1) {
        func_0200bc50(0xe9, 0x30, 0, 0, 0x8c, 0x202, 0xd7, "Card", 0, 2, 0, 0, 1, 0, 0, 0);
        if (func_0200bd80(0x20000000, 0x202)) {
            memset(data_020b10d4.mUnk_1c, -1, 10);
            func_02014cf8(&data_020b10d4);
        }
        pad = data_020c18bc.mUnk_12;
        if (pad & 0x10) {
            grade = data_020b10d4.mUnk_11;
            if (grade != 12) {
                grade++;
            }
            data_020b10d4.mUnk_11 = grade;
        }
        if (pad & 0x20) {
            grade = data_020b10d4.mUnk_11;
            if (grade != 1) {
                grade--;
            }
            data_020b10d4.mUnk_11 = grade;
        }
        func_0200b898(0x10, 0x10, "grade: %i", data_020b10d4.mUnk_11);
        for (i = 0; i < 5; i++) {
            func_0200b898(0x10, 0x1a + i * 10, "%s", func_0200f9f0(func_02014518(data_020b10d4.mUnk_1c[i], 3) + 0x8a));
        }
        for (i = 0; i < 5; i++) {
            func_0200b898(0x88, 0x1a + i * 10, "%i %i", func_02014518(data_020b10d4.mUnk_1c[i], 4),
                          func_02014518(data_020b10d4.mUnk_1c[i], 5));
        }
    } else {
        func_0200bc50(0xe9, 0x30, 0, 0, 0x8c, 0x200, 0xd7, "Save", 0, 2, 0, 0, 1, 0, 0, 0);
        func_0200bc50(0xe9, 0x4e, 0, 0, 0x8c, 0x201, 0xd7, "Load", 0, 2, 0, 0, 1, 0, 0, 0);
        if (func_0200bd80(0x20000000, 0x200)) {
            func_02014430(0);
        }
        if (func_0200bd80(0x20000000, 0x201)) {
            func_0201440c();
        }
        rows = 0;
        for (i = 0; i < data_020b1078.mUnk_04; i++) {
            func_020215fc(0x14 + i * 0x10, 0x1e + rows * 10, data_0201a6c6 + 2 * FONT_STRIDE,
                          (const char *) func_02021e18(ENTRY_BLOCK, i * ENTRY_STRIDE + 2 + data_020b1094[i]), 0, 0xf0,
                          0xa, 2);
            rows++;
        }
        if (data_020b1078.mUnk_04 > 0) {
            start = *(data_020b1094 + i - 1);
        } else {
            start = 0;
        }
        y = 0x1e + rows * 10;
        for (j = start; j < ENTRY_STRIDE; j++) {
            if (j != start) {
                for (k = 0; k < data_020b1078.mUnk_04; k++) {
                    if (func_02021e08(ENTRY_BLOCK, j + (k * ENTRY_STRIDE + 2)) != 0) {
                        break;
                    }
                }
                if (k != data_020b1078.mUnk_04) {
                    break;
                }
            }
            n = data_020b1078.mUnk_04 * ENTRY_STRIDE + 2;
            if (func_02021e08(ENTRY_BLOCK, j + n) != 0) {
                func_020215fc(0x14 + data_020b1078.mUnk_04 * 0x10, y,
                              data_0201a6c6 + (j == data_020b1094[data_020b1078.mUnk_04] ? 2 : 1) * FONT_STRIDE,
                              (const char *) func_02021e18(ENTRY_BLOCK, j + n), 0, 0xf0, 0xa, 2);
                last = j;
                y += 10;
                rows++;
            }
        }
        if (data_020c18bc.mUnk_12 & 0x40) {
            if (data_020b1094[data_020b1078.mUnk_04] > start) {
                do {
                    data_020b1094[data_020b1078.mUnk_04]--;
                    if (func_02021e08(ENTRY_BLOCK, data_020b1078.mUnk_04 * ENTRY_STRIDE + 2 +
                                                       data_020b1094[data_020b1078.mUnk_04]) != 0) {
                        break;
                    }
                } while (data_020b1094[data_020b1078.mUnk_04] > start);
            }
        } else if (data_020c18bc.mUnk_12 & 0x80) {
            if (data_020b1094[data_020b1078.mUnk_04] < last) {
                do {
                    data_020b1094[data_020b1078.mUnk_04]++;
                    if (func_02021e08(ENTRY_BLOCK, data_020b1078.mUnk_04 * ENTRY_STRIDE + 2 +
                                                       data_020b1094[data_020b1078.mUnk_04]) != 0) {
                        break;
                    }
                } while (data_020b1094[data_020b1078.mUnk_04] < last);
            }
        }
        if (data_020c18bc.mUnk_12 & 1) {
            data_020b1078.mUnk_04++;
            data_020b1094[data_020b1078.mUnk_04] = *(data_020b1094 + data_020b1078.mUnk_04 - 1);
            n = data_020b1078.mUnk_04 * ENTRY_STRIDE + 2 + data_020b1094[data_020b1078.mUnk_04];
            if ((u32) (func_02021e08(ENTRY_BLOCK, n) - 2) <= 1) {
                int v = func_02021e44(ENTRY_BLOCK, n);
                if (v < 100) {
                    data_020b6ee0.mUnk_a8f2 = v - 1;
                    data_020b6ee0.mUnk_a8e4 = -1;
                    data_020b10fc.mUnk_04 = 4;
                    func_0200fa80(4);
                    func_0200fab0();
                } else if (v < 200) {
                    func_0200fa80(5);
                    func_0200fab0();
                } else if (v < 300) {
                    func_0200fa80(6);
                    func_0200fab0();
                } else if (v < 400) {
                    data_020b1078.mUnk_04 = 0;
                } else if (v < 500) {
                    func_0200fa80(10);
                    func_0200fab0();
                } else if (v < 600) {
                    data_020b1078.mUnk_04 = 0;
                } else if (v < 700) {
                    func_0200fa80(12);
                    func_0200fab0();
                } else if (v < 800) {
                    data_020b10fc.mUnk_04 = 0;
                    func_0200fa80(14);
                    func_0200fab0();
                } else if (v < 900) {
                    data_020b10fc.mUnk_04 = 1;
                    func_0200fa80(15);
                    func_0200fab0();
                } else if (v < 1000) {
                    func_0200fa80(19);
                    func_0200fab0();
                } else if (v < 1100) {
                    data_020b10fc.mUnk_04 = 2;
                    func_0200fa80(16);
                    func_0200fab0();
                } else if (v < 1200) {
                    func_0200fa80(1);
                    func_0200fab0();
                } else if (v < 1300) {
                    data_020b10fc.mUnk_04 = 3;
                    func_0200fa80(17);
                    func_0200fab0();
                } else if (v < 1400) {
                    func_0200fa80(21);
                    func_0200fab0();
                } else if (v < 1500) {
                    func_0200fa80(22);
                    func_0200fab0();
                } else if (v < 1600) {
                    func_0200fa80(23);
                    func_0200fab0();
                }
            }
        } else if (data_020c18bc.mUnk_12 & 2) {
            if (data_020b1078.mUnk_04 > 0) {
                data_020b1078.mUnk_04--;
            }
        }
    }
}

void func_0201133c(void) {
    func_020102f8();
    func_0201eb20();
}

void func_02011348(void) {
    func_0200c5d8(0, 0xdaa, 0xdaa, 0x100, 0x10, 0x82, 2);
    func_02024208();
    data_020b122c.mUnk_178 = 0;
    data_020b122c.mUnk_17c = 0x8000;
    data_020b122c.mUnk_180 = 0;
    func_020271ec(&data_020b122c.mModel, 0, 0, 0, data_0201ab04[data_020b122c.mUnk_178]);
}

void func_020113a8(void) {
    int idx;
    func_0200b8dc(8, 0xb2, "sphere_%.2d.imd", data_020b122c.mUnk_178);
    if (data_020c18bc.mUnk_10 & 8) {
        data_020b122c.mModel.mUnk_170 = data_020b122c.mUnk_178 * 6 + 0xfdb;
    } else {
        data_020b122c.mModel.mUnk_170 = data_020b122c.mUnk_178 * 6 + 0xfd8;
    }
    func_02024368(&data_020b122c.mModel, 0x80, 0x60, 0x80 << 10, data_020b122c.mUnk_17c, 0);
    func_020105f4(&data_020b122c.mModel);
    if (data_020c18bc.mUnk_10 & 0x20) {
        data_020b122c.mUnk_17c -= 0x100;
    } else if (data_020c18bc.mUnk_10 & 0x10) {
        data_020b122c.mUnk_17c += 0x100;
    }
    if ((data_020c18bc.mUnk_12 & 1) || (data_020c18bc.mUnk_12 & 2) || (data_020c18bc.mUnk_12 & 0x400) ||
        (data_020c18bc.mUnk_12 & 0x800)) {
        data_020b122c.mModel.mUnk_134 |= data_020b122c.mUnk_180 != 0 ? 2 : 1;
        data_020b122c.mUnk_180 = data_020b122c.mUnk_180 == 0 ? 1 : 0;
    }
    if (data_020c18bc.mUnk_12 & 0x200) {
        idx = data_020b122c.mUnk_178 - 1;
        if (idx < 0) {
            idx = 0;
        }
        data_020b122c.mUnk_178 = idx;
        data_020b122c.mUnk_17c = 0x8000;
        data_020b122c.mUnk_180 = 0;
        func_020271ec(&data_020b122c.mModel, 0, 0, 0, data_0201ab04[idx]);
    } else if (data_020c18bc.mUnk_12 & 0x100) {
        idx = data_020b122c.mUnk_178 + 1;
        if (idx >= 0x26) {
            idx = 0x26;
        }
        data_020b122c.mUnk_178 = idx;
        data_020b122c.mUnk_17c = 0x8000;
        data_020b122c.mUnk_180 = 0;
        func_020271ec(&data_020b122c.mModel, 0, 0, 0, data_0201ab04[idx]);
    }
}

void func_02011510(void) {
    func_020102f8();
}
