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
#include <Main/Unknown/game_util.h>
#include <Main/Graphics/sprite_slots.h>
#include <Main/Scene/mode_select.h>
#include <Main/System/state_flow.h>
#include <Main/Scene/battle_screen.h>
#include <Main/System/prompt_screen.h>
#include <Main/Scene/language_menu.h>
#include <Main/Scene/stage_data.h>
#include <Main/Scene/stage_info_panel.h>
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

// Stage selection screen state: member 3 is set once the shown stage was changed with a button, member 4 is the stage
// shown in the large panel (-1 for none) and member 8 the highlighted entry of the row of five (-1 for none).
typedef struct Unk_020b13b0 {
    u8 mUnk_00;
    u8 mUnk_01;
    u8 mUnk_02;
    u8 mUnk_03;
    s32 mUnk_04;
    s32 mUnk_08;
} Unk_020b13b0;

Unk_020b13b0 data_020b13b0;

// The three stage numbers of the list as they were when the screen was entered; a cancel puts them back.
s32 data_020b13bc[3];

void func_02011518(int stage, int mode) {
    u16 current;
    int id;

    if (mode == 1) {
        current = data_020c18bc.mUnk_49c[0];
    } else {
        current = data_020c18bc.mUnk_49c[1];
    }
    if (!(stage >= -1 && stage < STAGE_MAX)) {
        OS_Terminate();
    }
    data_020b6ee0.mUnk_a13c = 0;
    if (func_0200fb00() == 10 && (stage == -1 || data_020b6ee0.mUnk_a128 != -1)) {
        id = data_020b6ee0.mUnk_a120 + 0x1e4;
        func_0200bdcc(2, 8);
    } else {
        id = data_020b1990[stage].mInfo.mUnk_13 + 0xdb3;
    }
    if ((u16) id != current) {
        func_0201f880(mode);
        func_0201dde0(id, mode);
    }
}

void func_020115ac(int frames, char *buf) {
    int t;

    if (frames < 0) {
        frames = 0;
    }
    t = frames * 1000 / 60;
    sprintf(buf, "%d%d:%d%d", t / 10000 % 10, t / 1000 % 10, t / 100 % 10, t / 10 % 10);
}

void func_02011624(int frames, char *buf) {
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

void func_0201168c(int level, u32 a, int b, int c, int d) {
    char time[12];
    char text[32];

    if (level == -1) {
        level = 0;
    }
    if (!(level >= 0 && level < STAGE_MAX)) {
        OS_Terminate();
    }
    func_0202cf08(0xf38, 0xa3, 0x8b, 0xc8, d);
    func_0202cf08(0xf39, 0x53, 0x8b, 0xc8, d);
    func_0202cf08(0xf48, 0x15, 0x8e, 0xc8, d);
    func_020215fc(0x23, 0x95, data_0201a612, func_0200f9f0(0x2f), 0, 0xc8, 0x10, d);
    sprintf(text, "%003d", a);
    func_020215fc(0x30, 0xb1, data_0201a694, text, 2, 0x82, 0x10, d);
    func_020215fc(0x5f, 0x95, data_0201a612, func_0200f9f0(0x30), 0, 0xc8, 0x10, d);
    sprintf(text, "%003d", b);
    func_020215fc(0x7b, 0xb0, data_0201a68a, text, 2, 0x82, 0x10, d);
    func_0202cf08(0xf49, 0xb2, 0x8e, 0xc8, d);
    func_020215fc(0xc1, 0x95, data_0201a612, func_0200f9f0(0x31), 0, 0xc8, 0x10, d);
    if (func_02005a00(c) == 0) {
        func_020115ac(c, time);
        func_020215fc(0xd2, 0xb0, data_0201a694, time, 2, 0xc8, 0x10, d);
    }
}

int func_020117e0(int percent) {
    int low = percent < 100 ? percent : 100;

    if (low < 0) {
        percent = 0;
    } else if (percent >= 100) {
        percent = 100;
    }
    return func_0201fbe4(percent, 0, 100, 0, 100) + 14;
}

void func_0201180c(int stage, int mode) {
    StageRec *rec;

    if (stage >= 0) {
        if (!(stage < STAGE_MAX)) {
            OS_Terminate();
        }
        func_0202cf08(0xf59, 8, 0x54, 0xa0, mode);
        rec = &data_020b1990[stage];
        func_0202cf08(func_0201fbe4(rec->mInfo.mUnk_12, 0, 100, 50, 0) + 0xf5a, 7, 0x58, 0x9f, mode);
        func_020215fc(0x1c, 0x54, data_0201a612, func_0200f9f0(10), 0, 0xf0, 0x10, mode);
    }
}

void func_02011890(int level, int mode) {
    StageRec *rec;
    int total;
    char label[8];
    char number[8];

    if (level >= 0) {
        rec = &data_020b1990[level];
        total = func_02021e44(0x1e8, 0x13);
        if (!(level >= 0 && level < STAGE_MAX)) {
            OS_Terminate();
        }
        func_0201fb24(&data_020b6ee0.mUnk_a8ec, 4);
        func_0201fbe4(data_020b6ee0.mUnk_a8ec, 0, 0x4000, 0x100, 0);
        func_0201180c(level, mode);
        func_0202cf08(0xf8d, 0, 0x79, 0xfa, mode);
        func_020215fc(0x25, 0x83, data_0201a61c, func_0200f9f0(0xd), 2, 0xfa, 0, mode);
        func_0202cf08(0xf4a, 0x27, 0xa3, 0xfa, mode);
        func_0202cf08(0xf4a + rec->mInfo.mUnk_11, 0x27, 0xa3, 0xfa, mode);
        func_0202cf08(0xf38, 0x47, 0x8b, 0x94, mode);
        func_020215fc(0x85, 0x83, data_0201a61c, func_0200f9f0(0x18), 2, 0xfa, 0, mode);
        func_0202cf08(0xf42 + rec->mInfo.mUnk_10, 0x74, 0xaa, 0xfa, mode);
        *func_0201fc08(rec->mInfo.mUnk_04, label) = 0;
        func_020215fc(0x74, 0xaa, data_0201a694, label, 0, 0xfa, 10, mode);
        if (func_020150c0(&rec->mInfo)) {
            func_0202cf08(0xf57 + (data_020c18bc.mUnk_45c % 60 <= 3 ? 1 : 0), 0x82, 0xbb, 0x8c, mode);
        }
        sprintf(number, "+%d", rec->mInfo.mUnk_04 * total / 100);
        func_020215fc(0xbe, 0xa4, data_0201a68a, number, 0, 0xfa, 10, mode);
        func_020215fc(0xe8, 0xb3, data_0201a61c, func_0200f9f0(0xf), 1, 0xfa, 0, mode);
        func_0202cf08(func_02021e44(0x10d5, rec->mInfo.mUnk_10 * 2 + 3) + 0xf32, 0xf2, 0xaf, 0xf8, mode);
    }
    data_020b6ee0.mUnk_a130 = 0x100;
}

void func_02011a94(int stage, int mode) {
    if (stage >= 0) {
        if (!(stage >= 0 && stage < STAGE_MAX)) {
            OS_Terminate();
        }
        func_0201fb24(&data_020b6ee0.mUnk_a8ec, 4);
        func_0201fbe4(data_020b6ee0.mUnk_a8ec, 0, 0x4000, 0x100, 0);
        if (data_020b6ee0.mUnk_a110 < 11) {
            data_020b6ee0.mUnk_a110 += data_020c18bc.mUnk_45c & 1;
        }
        func_0201180c(stage, mode);
        func_0202cf08(data_020b6ee0.mUnk_a110 + 0x19, 0x1f, 0x83, 0xf8, mode);
        if (data_020b6ee0.mUnk_a110 == 11) {
            func_020215fc(0x1f, 0x83, data_0201a61c, func_0200f9f0(0x10), 0, 0xdc, 0xd, mode);
            func_020215fc(0x15, 0x97, data_0201a612, func_0200f9f0(data_020b6ee0.mUnk_100c + 0x229), 0, 0xdc, 0xd,
                          mode);
        }
    }
    data_020b6ee0.mUnk_a130 = 0x100;
}

void func_02011b70(int stage, int mode) {
    if (stage >= 0) {
        if (!(stage >= 0 && stage < STAGE_MAX)) {
            OS_Terminate();
        }
        func_0201180c(stage, mode);
        func_020215fc(0x12, 0x96, data_0201a612, func_0200f9f0(data_020b6ee0.mUnk_a13d + 0x5f), 0, 0xdc, 0x10, mode);
    }
    data_020b6ee0.mUnk_a130 = 0x100;
}

void func_02011bcc(int a, int b, int c) {
    Unk_020c1454 *p = &data_020b6ee0.mUnk_a574;

    if (a == -1) {
        data_020b6ee0.mUnk_a12c = -1;
        data_020b6ee0.mUnk_a128 = -1;
        data_020b6ee0.mUnk_a124 = 0;
        data_020b6ee0.mUnk_a120 = 0;
        data_020b6ee0.mUnk_a138 = 0;
        data_020b6ee0.mUnk_a134 = 0;
    } else if (a == -2) {
        data_020b6ee0.mUnk_a128 = -1;
        data_020b6ee0.mUnk_a120 = 0;
        data_020b6ee0.mUnk_a134 = 0;
    } else if (a == -3) {
        data_020b6ee0.mUnk_a128 = data_020b6ee0.mUnk_a12c;
        data_020b6ee0.mUnk_a120 = data_020b6ee0.mUnk_a124;
        data_020b6ee0.mUnk_a134 = data_020b6ee0.mUnk_a138;
    } else {
        data_020b6ee0.mUnk_a12c = a;
        data_020b6ee0.mUnk_a128 = a;
        data_020b6ee0.mUnk_a124 = b;
        data_020b6ee0.mUnk_a120 = b;
        data_020b6ee0.mUnk_a138 = c;
        data_020b6ee0.mUnk_a134 = c;
    }
    data_020b6ee0.mUnk_a130 = 0x100;
    data_020b6ee0.mUnk_a13c = 1;
    data_020b6ee0.mUnk_a14b = 1;
    p->mUnk_40 = 0x28;
    if (data_020b6ee0.mUnk_a120 < 0) {
        OS_Terminate();
    }
    if (data_020b6ee0.mUnk_a128 < -1) {
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

void func_02011c64(void) {
    u32 v;

    func_0200b928(&data_020b6ee0.mUnk_a130, 0, 4);
    func_020215fc(0x30, 0x11, data_0201a61c, func_0200f9f0(0x4b), 0, 0xf0, 0xa, 2);
    v = data_020b6ee0.mUnk_a564;
    if (v > 2 && v < 99) {
        if (v >= 11) {
            func_0201180c(data_020b6ee0.mUnk_a8e4, 2);
        } else {
            func_0201180c(0, 2);
        }
    }
    if (data_020b6ee0.mUnk_a128 != -1) {
        func_020215fc(0x1b, data_020b6ee0.mUnk_a130 + 0xa0, data_0201a66c, func_0200f9f0(data_020b6ee0.mUnk_a128), 0,
                      200, 10, 2);
        func_0202cf08(0x40014, 0xd, data_020b6ee0.mUnk_a130 + 0xa3, 200, 2);
    }
}

void func_02011d1c(void) {
    Unk_020c1454 *p = &data_020b6ee0.mUnk_a574;

    if (data_020b6ee0.mUnk_a128 == -1) {
        if (data_020b6ee0.mUnk_a134 > 0) {
            data_020b6ee0.mUnk_a134--;
        }
    } else if (data_020b6ee0.mUnk_a134 <= 0) {
        func_02011bcc(-2, 0, 0);
    } else {
        data_020b6ee0.mUnk_a134--;
    }
    switch (data_020b6ee0.mUnk_a146) {
        case 0:
            func_02011c64();
            break;
        case 3:
        case 4:
            if (data_020b6ee0.mUnk_a128 != -1) {
                func_02011c64();
            } else {
                func_02011a94(data_020b6ee0.mUnk_a8e4, 2);
            }
            break;
        case 6:
        case 7:
        case 8:
            if (data_020b6ee0.mUnk_a128 != -1) {
                func_02011c64();
            } else {
                func_02011b70(data_020b6ee0.mUnk_a8e4, 2);
            }
            break;
        default:
            if (data_020b6ee0.mUnk_a128 != -1) {
                func_02011c64();
            } else {
                func_02011890(data_020b6ee0.mUnk_a8e4, 2);
            }
            break;
    }
    if (data_020b6ee0.mUnk_a146 <= 1) {
        if (data_020b6ee0.mUnk_a128 != -1) {
            func_0200b928(&p->mUnk_40, 0, 4);
            func_0200bc50(0x80, p->mUnk_40 + 0xc0, 0, 0, 0x8c, 0x112, 0xd1, 0, 0, 0, 0, 0x167, 1, 0, 0, 0);
            if (func_0200bd80(0x20000000, 0x112)) {
                func_02011bcc(-2, 0, 0);
            }
        } else if (data_020b6ee0.mUnk_a12c != -1) {
            func_0200b928(&p->mUnk_40, 0, 4);
            func_0200bc50(0x80, p->mUnk_40 + 0xc0, 0, 0, 0x8c, 0x111, 0xd3, 0, 0, 0, 0, 0x167, 1, 0, 0, 0);
            if (func_0200bd80(0x20000000, 0x111)) {
                func_02011bcc(-3, 0, 0);
            }
        }
    }
}

void func_02011ebc(StageRec *rec, u8 index, u32 flags) {
    if (index >= 3) {
        OS_Terminate();
    }
    data_020b6ee0.mUnk_ca8 = !(data_020c18bc.mUnk_45c & 1) ? data_0204fee2[index] : data_0204fee8[index];
    data_020b6ee0.mUnk_ca6 = 0;
    data_020b6ee0.mUnk_c94 = 0;
    data_020b6ee0.mUnk_c98 = 0;
    data_020b6ee0.mUnk_c9c = 0;
    rec->mUnk_1d8 = 0;
    rec->mUnk_1dc = 0;
    rec->mUnk_220 = 0;
    rec->mUnk_21c = 0;
    rec->mUnk_134 |= flags;
    func_0202f110(data_0204fef0[index], 100, 0, 0x40, 0xffff, 0);
}

int func_02011f44(int value, int low, int high, int steps) {
    if ((value < high ? value : high) < low) {
        value = low;
    } else if (value >= high) {
        value = high;
    }
    return func_0201fbe4(value, low, high, 0, steps - 1);
}

void func_02011f70(int kind, int x, int y, int delta) {
    char text[0x20];

    if (delta >= 0) {
        sprintf(text, "+%d", delta >= 0 ? delta : -delta);
    } else if (delta < 0) {
        sprintf(text, "-%d", delta >= 0 ? delta : -delta);
    }
    func_0200d78c(kind, 0, x - 0xf, y - 5, 0, 0, 0x98, 1);
    func_0200d838(text, 2, data_0201a61c, 0x78, x, y + 6, 0, 0, 0x96, 1, 0);
}

void func_02011fec(int a, int b, int c, u8 d, u8 e, int x, int y, int z, u8 *state, s32 *position) {
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
    if ((lo < GAUGE_MAX ? lo : GAUGE_MAX) < 0) {
        lo = 0;
    } else if (lo >= GAUGE_MAX) {
        lo = GAUGE_MAX;
    }
    hi = base + v0;
    if ((hi < GAUGE_MAX ? hi : GAUGE_MAX) < 0) {
        hi = 0;
    } else if (hi >= GAUGE_MAX) {
        hi = GAUGE_MAX;
    }
    mid = (lo + hi) / 2;
    lo2 = lo * 2 - 1;
    hi2 = hi * 2 + 3;
    mid2 = (lo2 + hi2) / 2;
    if ((a < c ? a : c) < b) {
        a = b;
    } else if (a >= c) {
        a = c;
    }
    pos = func_0201fbe4(a, b, c, 0, GAUGE_MAX);
    *position = pos;
    func_0202cf08(0xad, x, y, 0xa4, z);
    func_0202cf08(pos + 0x78, x, y, 0xa3, z);
    func_0202cf08(data_020b6ee0.mUnk_a13d + 0xaa, x, y, 0xa2, z);
    sprintf(text, "+ %d", data_020b6ee0.mUnk_a100);
    if (data_020b6ee0.mUnk_a145 == -1) {
        if (data_020b6ee0.mUnk_a13d == 0) {
            func_0202cf08(0xaf, x - 7, y - mid2, 0xa2, z);
        } else {
            func_0202cf08(0xb4, x - 7, y - lo2, 0xa2, z);
            func_0202cf08(0xaf, x - 7, y - hi2, 0xa2, z);
        }
        func_020215fc(x - 0x19, y - mid2 + 3, data_0201a612, text, 2, 200, 0x10, z);
    } else {
        half = data_020b6ee0.mUnk_a145 / 2;
        data_020b6ee0.mUnk_a145 = (data_020b6ee0.mUnk_a145 == 0x13) ? -1 : data_020b6ee0.mUnk_a145 + 1;
        if (data_020b6ee0.mUnk_a140 >= 7) {
            OS_Terminate();
        }
        base = data_0204ff14[data_020b6ee0.mUnk_a140];
        lo3 = base - v0;
        if ((lo3 < GAUGE_MAX ? lo3 : GAUGE_MAX) < 0) {
            lo3 = 0;
        } else if (lo3 >= GAUGE_MAX) {
            lo3 = GAUGE_MAX;
        }
        hi3 = base + v0;
        if ((hi3 < GAUGE_MAX ? hi3 : GAUGE_MAX) < 0) {
            hi3 = 0;
        } else if (hi3 >= GAUGE_MAX) {
            hi3 = GAUGE_MAX;
        }
        lo4 = lo3 * 2 - 1;
        hi4 = hi3 * 2 + 3;
        mid4 = (lo4 + hi4) / 2;
        if (data_020b6ee0.mUnk_a13d == 0) {
            rem = (u32) half % 5;
            func_0202cf08(rem + 0xaf, x - 7, y - mid4, 0xa2, z);
        } else {
            rem = (u32) half % 5;
            func_0202cf08(rem + 0xb4, x - 7, y - lo4, 0xa2, z);
            func_0202cf08(rem + 0xaf, x - 7, y - hi4, 0xa2, z);
        }
        func_020215fc(x - 0x19, y - mid4 + 3, data_0201a612, text, 2, 200, 0x10, z);
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

void func_020122c0(u8 type) {
    StageRec *rec;
    u8 grade;
    int ok;

    if (data_020b6ee0.mUnk_a8e4 == -1) {
        OS_Terminate();
    }
    rec = &data_020b1990[data_020b6ee0.mUnk_a8e4];
    ok = 0;
    grade = rec->mInfo.mUnk_11;
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
    data_020b6ee0.mUnk_a13f = func_0201fd68() % 7;
    data_020b6ee0.mUnk_a141 = func_0201fbe4(rec->mInfo.mUnk_11, 1, 12, 0, 6);
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
        data_020c18bc.mUnk_498 = 0x3f;
        func_0204028c(0x3f);
        func_02020ff4(0x400, data_020b1084.mUnk_04);
        func_02021074(0x82e, 0);
    }
}

void func_020123a4(int a) {
    StageRec *rec;
    int step;

    if (data_020b6ee0.mUnk_a8e4 == -1) {
        OS_Terminate();
    }
    rec = &data_020b1990[data_020b6ee0.mUnk_a8e4];
    data_020b6ee0.mUnk_a13e = a;
    data_020b6ee0.mUnk_a146 = 5;
    data_020b6ee0.mUnk_a148 = 1;
    data_020b6ee0.mUnk_a10c = -1;
    data_020b6ee0.mUnk_a8ec = 0;
    step = func_02011f44(data_020b6ee0.mUnk_a100, 0, 20, 3);
    func_02011ebc(rec, func_02011f44(data_020b6ee0.mUnk_a100, 0, 20, 3), 1);
    func_02011f70(step + 0x1b, 0x80, 0x60, data_020b6ee0.mUnk_a100);
    if (data_020b6ee0.mUnk_a13d >= 3) {
        OS_Terminate();
    }
    if (step == 2 && rec->mUnk_22e == 1 && rec->mInfo.mUnk_1a == data_020b6ee0.mUnk_a13d) {
        rec->mUnk_22e = 0;
        func_02014cdc(&rec->mInfo);
    }
    rec->mUnk_220 = 0;
    rec->mUnk_21c = 0;
    if (data_020b6ee0.mUnk_a13d == 0) {
        data_020c18bc.mUnk_498 = 0x7f;
        func_0204028c(0x7f);
        func_02021458();
    }
}

void func_02012490(u8 a, int b) {
    StageRec *rec;
    int level;
    int type;
    int amount;

    if (data_020b6ee0.mUnk_a8e4 == -1) {
        OS_Terminate();
    }
    rec = &data_020b1990[data_020b6ee0.mUnk_a8e4];
    if (data_020b6ee0.mUnk_a13d > 3) {
        OS_Terminate();
    }
    if (rec->mInfo.mUnk_13 >= STAGE_MAX) {
        OS_Terminate();
    }
    level = data_02050038[rec->mInfo.mUnk_13] - 6;
    if (level <= 8) {
        OS_Terminate();
    }
    if ((b < GAUGE_MAX ? b : GAUGE_MAX) < 0) {
        b = 0;
    } else if (b >= GAUGE_MAX) {
        b = GAUGE_MAX;
    }
    data_020b6ee0.mUnk_a10c = func_0201fbe4(b, 0, GAUGE_MAX, 0, level);
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
            func_0202f110(0xdc, 100, 0, 0x40, 0xffff, 0);
            data_020b6ee0.mUnk_a145 = 0;
            data_020b6ee0.mUnk_a140 = data_020b6ee0.mUnk_a13f;
            data_020b6ee0.mUnk_a14d = 1;
            if (rec->mUnk_22e == 1 && rec->mInfo.mUnk_1a == data_020b6ee0.mUnk_a13d) {
                amount = data_0204ff30[data_020b6ee0.mUnk_a13d];
            } else {
                amount = data_0204ff40[data_020b6ee0.mUnk_a13d];
            }
            func_02014ffc(rec, amount, 0, 1);
            data_020b6ee0.mUnk_a100 += amount;
            data_020b6ee0.mUnk_a0fc++;
            if (data_020b6ee0.mUnk_a0fc >= data_0204ff08[data_020b6ee0.mUnk_a13d]) {
                data_020b6ee0.mUnk_a13f = func_0201fd68() % 7;
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
            func_020123a4(1);
        } else {
            func_020123a4(0);
        }
    }
}

void func_020126a0(int stage) {
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
        rec = &data_020b1990[stage];
    } else {
        rec = NULL;
    }
    if (!(stage >= -1 && stage < STAGE_MAX)) {
        OS_Terminate();
    }
    func_0201fb24(&data_020b6ee0.mUnk_a8ec, 4);
    alpha = func_0201fbe4(data_020b6ee0.mUnk_a8ec, 0, 0x4000, 0x100, 0);
    switch (data_020b6ee0.mUnk_a146) {
        case 0:
            if (data_020c18bc.mUnk_46c & 0x800) {
                OS_Terminate();
            }
            func_0200b928(&fade->mUnk_3c, 0, 4);
            func_0200bbd0(fade->mUnk_3c + 0xff, 0x5f, 0, 0, 0x8c, 0x5a, 0xc8, 0, 0, 0, 0, 0x167, 1, 0, 0, 0,
                          &data_020b6ee0.mUnk_a56f);
            func_0200bbd0(fade->mUnk_3c + 0xff, 0x7d, 0, 0, 0x8c, 0xd, 0xc4, 0, 0, 0, 0, 0x167, 1, 0, 0, 0,
                          &data_020b6ee0.mUnk_a56e);
            top = -1;
            bottom = -1;
            break;
        case 1:
            func_0200b928(&fade->mUnk_3c, 0, 4);
            func_0200bbd0(fade->mUnk_3c + 0xff, 0x23, 0, 0, 0x8c, 0xa, 0xbc, 0, 0, 0, 0, 0x167, 1, 0, 0, 0,
                          &data_020b6ee0.mUnk_a56c);
            func_0200bbd0(fade->mUnk_3c + 0xff, 0x5f, 0, 0, 0x8c, 0xb, 0xc8, 0, 0, 0, 0, 0x167, 1, 0, 0, 0,
                          &data_020b6ee0.mUnk_a56f);
            func_0200bbd0(fade->mUnk_3c + 0xff, 0x7d, 0, 0, 0x8c, 0xd, 0xc4, 0, 0, 0, 0, 0x167, 1, 0, 0, 0,
                          &data_020b6ee0.mUnk_a56e);
            if (stage == -1 || rec == NULL) {
                OS_Terminate();
            }
            if (rec->mInfo.mUnk_12 == 100) {
                func_0200bbd0(fade->mUnk_3c + 0xff, 0x41, 0, 0, 0x8c, 0xc, 0xcc, 0, 0, 0, 0, 0x167, 1, 0, 0, 0,
                              &data_020b6ee0.mUnk_a56d);
            } else {
                func_0200bbd0(fade->mUnk_3c + 0xff, 0x41, 0, 0, 0x8c, 0xc, 0xcc, 0, 0, 0, 0, 0x167, 0, 0, 0, 0,
                              &data_020b6ee0.mUnk_a56d);
            }
            top = -1;
            bottom = 0xe;
            break;
        case 2:
            func_0200bc50(0x39 - alpha, 0x12, 0, 0, 0x8c, 0, 0xd9, func_0200f9f0(0x17), data_0201a61c, 0, 0, 0, 0, 0, 0,
                          0);
            func_0200bc50(alpha + 0xff, 0x23, 0, 0, 0x8c, 0xf, 0xc0, 0, 0, 0, 0, 0x167, 1, 0, 0, 0);
            func_0200bc50(alpha + 0xff, 0x41, 0, 0, 0x8c, 0x10, 0xc2, 0, 0, 0, 0, 0x167, 1, 0, 0, 0);
            top = -1;
            bottom = 0x11;
            break;
        case 3:
            x = 0x39 - alpha;
            func_0202cf08(0xda, x, 0x12, 0x8c, 1);
            func_020215fc(x, 0x12, data_0201a61c, func_0200f9f0(0x10), 0, 0xf0, 0x10, 1);
            data_020c3e44.mUnk_2024.mUnk_2dc.mUnk_4c = 1;
            func_0202cf08(0x800017, 0, 0, 0x64, 1);
            for (i = 0, y = 0x54; i < 3; i++, y += 0x16) {
                id = data_020b6ee0.mUnk_1000[i];
                func_02020944(i + 0x19, 0x10, y, -1, -1, 0x41, 0, 0x167);
                if (i == data_020b6ee0.mUnk_a8f1) {
                    font = data_0201a644;
                } else {
                    font = data_0201a63a;
                }
                func_020215fc(0x16, y, font, func_0200f9f0(data_020b6ee0.mUnk_100e[id] + 0x229), 0, 0xf0, 0x10, 1);
            }
            func_0202cf08(0x800041, 0x10, data_020b6ee0.mUnk_a8f1 * 0x16 + 0x54, 0x5f, 1);
            if (func_0200bdac(0x80000000, 0x19, 0x1b) == 1) {
                id = (u16) data_020c1d60.mUnk_800.mUnk_100.mUnk_04 - 0x19;
                if (data_020b6ee0.mUnk_a8f1 != id) {
                    data_020b6ee0.mUnk_a8f1 = id;
                }
            }
            top = 0x17;
            bottom = 0x18;
            break;
        case 4:
            func_0202cf08(0xda, 0x39, 0x12, 0x8c, 1);
            func_020215fc(0x39, 0x12, data_0201a61c, func_0200f9f0(0x10), 0, 0xf0, 0x10, 1);
            func_0202cf08(0x18, 0, 0, 0x8c, 1);
            func_020215fc(0x12, 0x96, data_0201a63a, func_0200f9f0(data_020b6ee0.mUnk_100e[0] + 0x229), 0, 0xdc, 0x10,
                          1);
            top = -1;
            bottom = 0x12;
            break;
        case 5:
            x = 0x39 - alpha;
            func_0202cf08(0xde, x, 0x12, 0x8c, 1);
            func_020215fc(x, 0x12, data_0201a61c, func_0200f9f0(0x11), 0, 0xf0, 0x10, 1);
            rec->mUnk_1d8 += func_0201fe34(rec->mUnk_1d8, 0) / 8;
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
            func_0202cf08(0xdb, x, 0x12, 0x8c, 1);
            func_020215fc(x, 0x12, data_0201a61c, func_0200f9f0(0x15), 0, 0xf0, 0x10, 1);
            func_02011fec(data_020b6ee0.mUnk_a8e8, 0, 3000, data_020b6ee0.mUnk_a13f, data_020b6ee0.mUnk_a141,
                          0x2c - alpha, 0x96, 1, &kind, &value);
            if (data_020c18bc.mUnk_492) {
                func_020215fc(0x80, 0x60, data_0201a61c, func_0200f9f0(0xd3), 2, 0xc0, 0x10, 1);
            }
            if (!(data_020c18bc.mUnk_46c & 0x800)) {
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
            id = func_0201fbe4(data_020b6ee0.mUnk_a8e8, 0, 3000, data_0204ff50[data_020b1084.mUnk_08],
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
            rec->mUnk_1da += func_0201fe34(rec->mUnk_1da, 0) / 8;
            rec->mUnk_1dc += func_0201fe34(rec->mUnk_1dc, 0) / 8;
            if (rec->mUnk_1d8 > data_020b6ee0.mUnk_0000) {
                data_020b6ee0.mUnk_0002 += 0x100;
            }
            data_020b6ee0.mUnk_0000 = rec->mUnk_1d8;
            if (data_020b6ee0.mUnk_0002 > 4) {
                data_020b6ee0.mUnk_0002 -= 4;
            }
            func_02012490(kind, value);
            top = -1;
            bottom = 0x21;
            break;
        case 7: {
            int speed;
            int ground;

            x = 0x39 - alpha;
            func_0202cf08(0xdc, x, 0x12, 0x8c, 1);
            func_020215fc(x, 0x12, data_0201a61c, func_0200f9f0(0x16), 0, 0xf0, 0x10, 1);
            func_02011fec(rec->mUnk_1a0.y - 0x32000, 0, 0x12c000, data_020b6ee0.mUnk_a13f, data_020b6ee0.mUnk_a141,
                          0x2c - alpha, 0x96, 1, &kind, &value);
            if (data_020c18bc.mUnk_2ac_0) {
                speed = rec->mUnk_1a0.y / 0xc800;
                if (speed < 0) {
                    speed = -speed;
                }
                if ((speed < 40 ? speed : 40) < 0) {
                    speed = 0;
                } else if (speed >= 40) {
                    speed = 40;
                }
                rec->mUnk_21c = -func_0201fbe4(speed, 0, 40, 10, 20) << 12;
            }
            rec->mUnk_1a0.y = rec->mUnk_1a0.y + rec->mUnk_21c;
            rec->mUnk_21c = rec->mUnk_21c - 2000.0f;
            ground = func_02030830(rec->mUnk_1a0.x, rec->mUnk_1a0.z, rec->mUnk_1a0.y + 0xc800, 0, 0, NULL, &normal) +
                     0x32000;
            if (rec->mUnk_1a0.y < ground) {
                speed = rec->mUnk_21c >= 0.0f ? rec->mUnk_21c : 0.0f - rec->mUnk_21c;
                if ((speed < 30000 ? speed : 30000) < 0) {
                    speed = 0;
                } else if (speed >= 30000) {
                    speed = 30000;
                }
                speed = func_0201fbe4(speed, 0, 30000, 0, 0x30);
                rec->mUnk_21c = (0.0f - rec->mUnk_21c) * speed / 64.0f;
                rec->mUnk_1a0.y = ground;
                if (rec->mUnk_21c >= 5000.0f) {
                    func_0202f110(0x27, 100, 0, 0x40, 0xffff, 0);
                }
            }
            func_02012490(kind, value);
            top = -1;
            bottom = 0x21;
            break;
        }
        case 8:
            x = 0x39 - alpha;
            func_0202cf08(0xdd, x, 0x12, 0x8c, 1);
            func_020215fc(x, 0x12, data_0201a61c, func_0200f9f0(0x19), 0, 0xf0, 0x10, 1);
            func_02011fec(data_020b6ee0.mUnk_a108, 0, 1000, data_020b6ee0.mUnk_a13f, data_020b6ee0.mUnk_a141,
                          0x2c - alpha, 0x96, 1, &kind, &value);
            if (data_020c18bc.mUnk_2ac_0) {
                data_020b6ee0.mUnk_a574.mUnk_2c = data_020c18bc.mUnk_2ae;
                data_020b6ee0.mUnk_a574.mUnk_30 = data_020c18bc.mUnk_2af;
            } else if (data_020c18bc.mUnk_2ac_1) {
                data_020b6ee0.mUnk_a108 += (data_020b6ee0.mUnk_a574.mUnk_2c - data_020c18bc.mUnk_2ae >= 0
                                                ? data_020b6ee0.mUnk_a574.mUnk_2c - data_020c18bc.mUnk_2ae
                                                : -(data_020b6ee0.mUnk_a574.mUnk_2c - data_020c18bc.mUnk_2ae)) +
                                           (data_020b6ee0.mUnk_a574.mUnk_30 - data_020c18bc.mUnk_2af >= 0
                                                ? data_020b6ee0.mUnk_a574.mUnk_30 - data_020c18bc.mUnk_2af
                                                : -(data_020b6ee0.mUnk_a574.mUnk_30 - data_020c18bc.mUnk_2af));
                gauge = data_020b6ee0.mUnk_a108;
                if ((gauge < 1000 ? gauge : 1000) < 0) {
                    gauge = 0;
                } else if (gauge >= 1000) {
                    gauge = 1000;
                }
                data_020b6ee0.mUnk_a108 = gauge;
                data_020b6ee0.mUnk_a574.mUnk_2c = data_020c18bc.mUnk_2ae;
                data_020b6ee0.mUnk_a574.mUnk_30 = data_020c18bc.mUnk_2af;
            }
            if (data_020b6ee0.mUnk_a108 > 0) {
                data_020b6ee0.mUnk_a108 -= 8;
            }
            func_02012490(kind, value);
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
    key = (u16) data_020c1d60.mUnk_800.mUnk_100.mUnk_04;
    if ((data_020c1d60.mUnk_800.mUnk_100.mUnk_04 & 0x20000000) && key != 0xffff && data_020b6ee0.mUnk_a5d0 == 1) {
        switch (key) {
            case 0xa:
                data_020b6ee0.mUnk_a146 = 2;
                data_020b6ee0.mUnk_ca8 = 0;
                data_020b6ee0.mUnk_a148 = 0;
                data_020b6ee0.mUnk_a8ec = 0;
                fade->mUnk_3c = 0x28;
                func_02011bcc(-2, 0, 0);
                break;
            case 0xb:
                func_0200fa80(9);
                func_0200fab0();
                break;
            case 0xc:
                func_0200fa80(0xd);
                func_0200fab0();
                break;
            case 0xd:
                data_020b6ee0.mUnk_a8e4 = -1;
                func_0200fa80(6);
                func_0200fab0();
                break;
            case 0xe:
                data_020b6ee0.mUnk_a8e4 = -1;
                data_020b6ee0.mUnk_a574.mUnk_0c.x = data_020c3e44.mUnk_2024.mUnk_8c.x;
                data_020b6ee0.mUnk_a574.mUnk_0c.y = data_020c3e44.mUnk_2024.mUnk_8c.y;
                data_020b6ee0.mUnk_a574.mUnk_0c.z = data_020c3e44.mUnk_2024.mUnk_8c.z;
                fade->mUnk_3c = 0x28;
                break;
            case 0xf: {
                int r;
                int base;
                int first;
                int ok;
                int k;
                int idx;

                data_020b6ee0.mUnk_a146 = 3;
                data_020b6ee0.mUnk_a8f1 = 0;
                data_020b6ee0.mUnk_a13d = 3;
                data_020b6ee0.mUnk_a110 = 0;
                data_020b6ee0.mUnk_a8ec = 0;
                func_02011bcc(-2, 0, 0);
                r = func_0201fd68() % 6;
                if (!(r * 3 + 2 < 18)) {
                    OS_Terminate();
                }
                data_020b6ee0.mUnk_1000[0] = data_0204fff0[r][0];
                data_020b6ee0.mUnk_1000[1] = data_0204fff0[r][1];
                data_020b6ee0.mUnk_1000[2] = data_0204fff0[r][2];
                base = rec->mInfo.mUnk_10 * 0x68;
                first = base + 0x1a;
                ok = 0;
                if (rec->mInfo.mUnk_11 != 0 && rec->mInfo.mUnk_11 - 1 < 12) {
                    ok = 1;
                }
                if (!ok) {
                    OS_Terminate();
                }
                idx = rec->mInfo.mUnk_11 - 1;
                k = func_0201fd68() % data_0204ffc0[idx];
                k += data_0204ff90[idx];
                data_020b6ee0.mUnk_100c = base + k;
                for (i = 0; i < 3; i++) {
                    data_020b6ee0.mUnk_100e[i] = first + (k * 3 + i);
                }
                break;
            }
            case 0x10:
                data_020b6ee0.mUnk_a146 = 5;
                data_020b6ee0.mUnk_a148 = 0;
                data_020b6ee0.mUnk_a8ec = 0;
                break;
            case 0x11:
                data_020b6ee0.mUnk_a146 = 1;
                break;
            case 0x12:
                func_0200d6c8();
                data_020b6ee0.mUnk_a146 = 2;
                data_020b6ee0.mUnk_a148 = 0;
                data_020b6ee0.mUnk_a8ec = 0;
                data_020b6ee0.mUnk_ca8 = 0;
                rec->mUnk_134 |= 2;
                rec->mUnk_1a0.y =
                    func_02030830(rec->mUnk_1a0.x, rec->mUnk_1a0.z, rec->mUnk_1a0.y + 0xc800, 0, 0, NULL, NULL) +
                    0x32000;
                break;
            case 0x17: {
                int got;

                data_020b6ee0.mUnk_a146 = 4;
                if (data_020b6ee0.mUnk_1000[data_020b6ee0.mUnk_a8f1] == 0) {
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
                    func_02014ffc(rec, id, 1, 1);
                } else {
                    data_020b6ee0.mUnk_a13e = 0;
                    data_020b6ee0.mUnk_a100 = 0;
                }
                got = data_020b6ee0.mUnk_a100 == 0 ? 0 : 2;
                func_02011ebc(rec, data_020b6ee0.mUnk_a100 == 0 ? 0 : 2, 1);
                func_02011f70(got + 0x1b, 0x80, 0x60, data_020b6ee0.mUnk_a100);
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
                data_020b6ee0.mUnk_a146 = 2;
                data_020b6ee0.mUnk_a148 = 0;
                data_020b6ee0.mUnk_a8ec = 0;
                break;
            case 0x1e:
                data_020b6ee0.mUnk_a146 = 6;
                func_020122c0(0);
                data_020b6ee0.mUnk_0002 = 0;
                data_020b6ee0.mUnk_0004 = 8;
                data_020b6ee0.mUnk_0000 = 0;
                data_020b6ee0.mUnk_a8ec = 0;
                func_02011bcc(-2, 0, 0);
                break;
            case 0x1f:
                data_020b6ee0.mUnk_a146 = 7;
                func_020122c0(1);
                rec->mUnk_21c = -60000.0f;
                rec->mUnk_1d8 = 0xe000;
                rec->mUnk_1da = 0;
                rec->mUnk_1dc = 0;
                data_020b6ee0.mUnk_a8ec = 0;
                func_02011bcc(-2, 0, 0);
                break;
            case 0x20:
                data_020b6ee0.mUnk_a146 = 8;
                func_020122c0(2);
                rec->mUnk_1d8 = 0xe000;
                rec->mUnk_1da = 0;
                rec->mUnk_1dc = 0;
                data_020b6ee0.mUnk_a8ec = 0;
                func_02011bcc(-2, 0, 0);
                break;
            case 0x21:
                func_020123a4(0);
                break;
            case 0x29:
                // No action; the original's jump table runs up to this key.
                break;
            case 0x5a:
                data_020b6ee0.mUnk_c78 = 0;
                func_0200fa80(9);
                func_0200fab0();
                break;
        }
    }
}

void func_0201353c(void) {
    int stage;
    int id;

    func_0201f880(3);
    func_0201bb30();
    func_0201c41c();
    func_0201eb20();
    func_0202c1c4();
    func_02020854(1);
    func_0202bdd8(0x10cb, 0x10ca, 0x10cc, 0x82, 1);
    func_0202bdd8(0x10cb, 0x10ca, 0x10cc, 0x82, 2);
    func_02024208();
    data_020b13b0.mUnk_03 = 0;
    data_020b13b0.mUnk_04 = data_020b6ee0.mUnk_a8e4;
    memcpy(data_020b13bc, data_020b6ee0.mUnk_c60, 0xc);
    data_020b13b0.mUnk_08 = -1;
    data_020b6ee0.mUnk_c79 = 1;
    if (data_020b6ee0.mUnk_a8ee <= 3) {
        data_020b6ee0.mUnk_c78 = 0;
    } else {
        data_020b6ee0.mUnk_c78 = 1;
        if (data_020b6ee0.mUnk_a8e4 != -1) {
            int slot = func_020151cc(data_020b6ee0.mUnk_a8e4);
            if (slot != -1) {
                data_020b13b0.mUnk_04 = func_02015260();
                data_020b6ee0.mUnk_c79 = slot;
            }
        } else {
            data_020b13b0.mUnk_04 = func_02015260();
        }
    }
    stage = data_020b6ee0.mUnk_a8e4;
    if (stage == -1) {
        id = 0x237;
    } else {
        id = data_020b1990[stage].mInfo.mUnk_13 + 0xdb3;
    }
    func_0201dde0(id, 1);
    func_0201dde0(data_020b6ee0.mUnk_c78 == 1 ? 0x10ce : 0x10cd, 2);
}

void func_0201364c(void) {
    StageRec *sel;
    int ox;
    int oy;
    int i;
    int s;
    int x;
    int y;
    int sub;

    if (data_020b13b0.mUnk_04 != -1) {
        sel = &data_020b1990[data_020b13b0.mUnk_04];
    } else {
        sel = NULL;
    }
    func_02013a34();
    if (data_020b6ee0.mUnk_a8e0 != data_020b6ee0.mUnk_a8e4) {
        func_02011518(data_020b6ee0.mUnk_a8e4, 1);
        data_020b6ee0.mUnk_a8e0 = data_020b6ee0.mUnk_a8e4;
    } else if (data_020b6ee0.mUnk_a8e4 == -1 && data_020b6ee0.mUnk_a13c == 1) {
        func_02011518(data_020b6ee0.mUnk_a8e4, 1);
    }
    func_02013f74(data_020b6ee0.mUnk_a8e4, &data_020b13b0.mUnk_08, 0x80, 0xa1, 2, 0);
    if (data_020b13b0.mUnk_08 == -1) {
        func_02011890(data_020b6ee0.mUnk_a8e4, 1);
    }
    if (data_020b6ee0.mUnk_c78 == 1) {
        ox = 0;
        oy = 0;
    } else {
        ox = -0x2e;
        oy = 0x14;
    }
    func_020215fc(0x3a, 0x11, data_0201a61c, func_0200f9f0(0x27), 0, 0xf0, 0x10, 2);
    func_020215fc(ox + 0xad, oy + 0x26, data_0201a61c, func_0200f9f0(0x28), 2, 0xf0, 0x10, 2);
    i = 0;
    y = oy + 0x4e;
    x = ox + 0x80;
    for (; i < 3; i++) {
        s = data_020b6ee0.mUnk_c60[i];
        if (s != -1) {
            if (s >= STAGE_MAX) {
                OS_Terminate();
            }
            func_0202cf08(data_020b1990[s].mInfo.mUnk_13 + 0x1089, x, y, 0x64, 2);
            func_0202cf08(0x1088, x, y, 0x66, 2);
            if (s == data_020b6ee0.mUnk_a8e4) {
                sub = (data_020c18bc.mUnk_45c >> 1) & 7;
            } else {
                sub = 0;
            }
            if (s == data_020b6ee0.mUnk_a8e4 || (data_020b6ee0.mUnk_c78 == 1 && i == data_020b6ee0.mUnk_c79)) {
                func_0202cf08(0x11c6 + sub, x, y, 0x65, 2);
            }
        } else {
            func_0202cf08(0x1088, x, y, 0x66, 2);
        }
        if (data_020c18bc.mUnk_2ac_1 == 1 &&
            func_0201faf0(data_020c18bc.mUnk_2ae - x, data_020c18bc.mUnk_2af - y + 0xc) < 0x10 &&
            data_020b6ee0.mUnk_a8e4 != s) {
            if (s != -1) {
                data_020b6ee0.mUnk_a8e4 = s;
            }
            data_020b6ee0.mUnk_c79 = i;
            data_020b13b0.mUnk_08 = -1;
            func_0202f110(0x167, 0x64, 0, 0x40, 0xffff, 0);
        }
        x += 0x2d;
    }
    if (data_020b6ee0.mUnk_c78 == 1) {
        s = data_020b13b0.mUnk_04;
        func_0202cf08(sel->mInfo.mUnk_13 + 0x1089, 0x32, 0x4e, 0x64, 2);
        func_0202cf08(0x1088, 0x32, 0x4e, 0x66, 2);
        if (data_020b13b0.mUnk_04 == data_020b6ee0.mUnk_a8e4) {
            sub = (data_020c18bc.mUnk_45c >> 1) & 7;
        } else {
            sub = 0;
        }
        func_0202cf08(0x11be + sub, 0x32, 0x4e, 0x65, 2);
        if (data_020c18bc.mUnk_2ac_1 == 1 &&
            func_0201faf0(data_020c18bc.mUnk_2ae - 0x32, data_020c18bc.mUnk_2af - 0x42) < 0x10 &&
            data_020b6ee0.mUnk_a8e4 != s) {
            if (s != -1) {
                data_020b6ee0.mUnk_a8e4 = s;
            }
            data_020b13b0.mUnk_08 = -1;
            func_0202f110(0x167, 0x64, 0, 0x40, 0xffff, 0);
        }
        func_0200bc50(0x38, 0x77, 0, 0, 0xc8, 0x200, 0x11d2, func_0200f9f0(0x29), data_0201a61c, 2, 0, 0x17d, 1, 0, 0,
                      0);
        ox += 0x80;
        oy += 0x4e;
        func_0202cf08(0x11d1, ox + data_020b6ee0.mUnk_c79 * 0x2d, oy, 0xc9, 2);
    }
    if (data_020b6ee0.mUnk_c78 == 1 && data_020b6ee0.mUnk_a8ee >= 5) {
        func_0200bb14(0x1b, 0x29, 0x110, 0x12, 0, 0x20, 0, 1);
        func_0200bb14(0x4e, 0x29, 0x109, 0x10, 0, 0x10, 0, 1);
    }
    func_0200bb14(0, 0xa1, 0x102, 0xe, 0, 0, 0x5a, 1);
    if (data_020b6ee0.mUnk_c78 == 1 && data_020b13b0.mUnk_03 == 1) {
        func_0200bb14(0xff, 0xa1, 0x101, 0x15, 0, 0, 0x17d, 1);
    }
}

void func_02013a2c(void) {
    func_020102f8();
}

void func_02013a34(void) {
    int i;

    if (func_0200bd80(0x20000000, 0x109) && data_020b6ee0.mUnk_c78 == 1) {
        data_020b13b0.mUnk_04 = data_020b6ee0.mUnk_a8e4 = func_02015294(data_020b13b0.mUnk_04);
        func_0202f110(0x167, 0x64, 0, 0x40, 0xffff, 0);
    }
    if (func_0200bd80(0x20000000, 0x110) && data_020b6ee0.mUnk_c78 == 1) {
        data_020b13b0.mUnk_04 = data_020b6ee0.mUnk_a8e4 = func_0201530c(data_020b13b0.mUnk_04);
        func_0202f110(0x167, 0x64, 0, 0x40, 0xffff, 0);
    }
    if (func_0200bd80(0x20000000, 0x200)) {
        data_020b13b0.mUnk_04 = func_02015114(data_020b6ee0.mUnk_c79, data_020b13b0.mUnk_04);
        data_020b13b0.mUnk_03 = 1;
    }
    if (func_0200bd80(0x20000000, 0x101)) {
        data_020b6ee0.mUnk_a147 = 1;
        func_0200fa80(10);
        func_0200fab0();
    }
    if (func_0200bd80(0x20000000, 0x102)) {
        for (i = 0; i < 3; i++) {
            func_02015114(i, data_020b13bc[i]);
        }
        data_020b6ee0.mUnk_a147 = 1;
        func_0200fa80(10);
        func_0200fab0();
    }
}

void func_02013b48(StageInfo *info, int slot, int *a, int *b) {
    int index;
    int base;
    int v4;
    int v5;

    index = func_020079ec(slot);
    base = index * 0x14;
    v4 = func_02021e44(0x1d9, base + 4);
    v5 = func_02021e44(0x1d9, base + 5);
    if (slot < 100) {
        *a = index + 0x10b4;
        *b = -1;
    } else if (slot < 200) {
        *a = 0x10c1;
        *b = v4 / 10 + 0x113f;
    } else if (slot < 300) {
        *a = info->mUnk_10 + 0x10c2;
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

    index = func_020079ec(slot);
    base = index * 0x14;
    v4 = func_02021e44(0x1d9, base + 4);
    v5 = func_02021e44(0x1d9, base + 5);
    if (e != NULL) {
        *e = func_02021e44(0x1d9, base + 3);
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
        *c = info->mUnk_10 + 0x10dd;
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
        func_0202cf08(func_0202db1c(&data_020b6ee0.mUnk_a5d4), b, c, 0x93, d);
    }
    if (!(e & 4)) {
        func_02020944(a, b, c, 0, 0, 0x10b4, 0, 0x167);
    }
    func_0202cf08(icon, b, c, 0x94, d);
    func_0202cf08(name, b + 8, c - 4, 0x92, d);
}

void func_02013dc8(StageInfo *info, s16 slot, int a, int b) {
    int frame;
    int icon;
    int level;
    int index;
    int kind;
    int kindId;

    func_02013c24(info, slot, &index, &frame, &icon, &level, &kind, &kindId);
    func_0202cf08(frame, 0x35, 0x85, 0x96, a);
    func_0202cf08(icon, 0x35, 0x85, 0x8c, a);
    func_0202cf08(level, 0x4e, 0x7b, 0x82, a);
    if (b & 1) {
        func_0202cf08(0x80113f, 5, 0xbf, 0xfa, a);
    }
    if (kind < 0xe) {
        func_020215fc(0x5f, 0x93, data_0201a61c, func_0200f9f0(index + 0x8a), 0, 0xf0, 0x10, 1);
        func_020215fc(0x12, 0xa4, data_0201a612, func_0200f9f0(index + 0x9b), 0, 0xe8, 0xa, 1);
    } else if (kind == 0xe) {
        func_020215fc(0x5f, 0x93, data_0201a61c, func_0200f9f0(info->mUnk_10 + 0xb8), 0, 0xf0, 0x10, 1);
        func_020215fc(0x12, 0xa4, data_0201a612, func_0200f9f0(info->mUnk_10 + 0xac), 0, 0xe8, 0xa, 1);
    } else if (kind == 0xf) {
        func_020215fc(0x5f, 0x93, data_0201a61c, func_0200f9f0(kindId + 0xbe), 0, 0xf0, 0x10, 1);
        func_020215fc(0x12, 0xa4, data_0201a612, func_0200f9f0(kindId + 0xb2), 0, 0xe8, 0xa, 1);
    } else if (kind == 0x10) {
        func_020215fc(0x5f, 0x93, data_0201a61c, func_0200f9f0(index + 0x8a), 0, 0xf0, 0x10, 1);
        func_020215fc(0x12, 0xa4, data_0201a612, func_0200f9f0(index + 0x9b), 0, 0xe8, 0xa, 1);
    }
}

void func_02013f74(int stage, int *cursor, int x, int y, int mode, int flags) {
    StageRec *rec;
    int mode2;
    int i;
    u8 sel;
    int xx;
    StageInfo *info;

    if (stage != -1) {
        rec = &data_020b1990[stage];
    } else {
        rec = NULL;
    }
    if (mode == 1) {
        mode2 = 2;
    } else {
        mode2 = 1;
    }
    if (!(stage >= -1 && stage < STAGE_MAX)) {
        OS_Terminate();
    }
    if (stage != -1) {
        func_0202cf08(0xfeb, x + 2, y + 0xb, 0x96, mode);
        for (i = 0; i < 5; i++) {
            info = &rec->mInfo;
            sel = (*cursor == i);
            xx = x - 0x34;
            if (info->mUnk_1c[i] >= 0) {
                func_02013d48(info, info->mUnk_1c[i], sel, 0x500 + i, xx, y, mode, flags);
                if (sel) {
                    func_02013dc8(info, info->mUnk_1c[i], mode2, flags);
                }
            } else if (flags & 2) {
                if (!(flags & 4)) {
                    func_02020944(0x500 + i, xx, y, 0, 0, 0x10b4, 0, 0x167);
                }
                if (sel) {
                    func_0202cf08(func_0202db1c(&data_020b6ee0.mUnk_a5d4), xx, y, 0x93, mode);
                }
            }
            if (func_0200bd80(0x80000000, 0x500 + i)) {
                *cursor = i;
            }
            x += 0x1a;
        }
    }
}

void func_020140d4(void) {
    func_0201eb20();
    func_0202c1c4();
    func_02022370();
    func_0202bdd8(0x10cb, 0x10ca, 0x10cc, 0, 1);
    func_02021fb0(0x1c7, 0x1c6, 0x100, 0x10);
    func_02020854(1);
    func_02024208();
    data_020b13b0.mUnk_02 = 0;
    func_0200f848(0, 1);
}

void func_02014124(void) {
    u32 sel;

    func_0200f898();
    if (data_020c18bc.mUnk_12 & 0x100) {
        sel = data_020b13b0.mUnk_02 + 1;
        if (sel >= func_0200f988() - 1) {
            sel = func_0200f988() - 1;
        }
        data_020b13b0.mUnk_02 = sel;
        func_0200f848(data_020b13b0.mUnk_02, 1);
    } else if (data_020c18bc.mUnk_12 & 0x200) {
        int prev = data_020b13b0.mUnk_02 - 1;
        if (prev < 0) {
            prev = 0;
        }
        data_020b13b0.mUnk_02 = prev;
        func_0200f848(data_020b13b0.mUnk_02, 1);
    }
}

void func_02014184(void) {
    func_020102f8();
}

void func_0201418c(void) {}

void func_02014190(void) {
    func_0201eb20();
    func_0202c1c4();
    func_02022370();
    func_0202bdd8(0x10cb, 0x10ca, 0x10cc, 0, 1);
    func_02021fb0(0x1c7, 0x1c6, 0x100, 0x10);
    func_02020854(1);
    func_020220c8(0x10d0);
    func_020325b0(0xf87, 0, 0xf88, 0, 0, 0);
    data_020c3e44.mUnk_2024.mUnk_04 = func_0201418c;
    data_020c3e44.mUnk_2024.mUnk_2dc.mUnk_54 = 0x640;
    data_020c3e44.mUnk_2024.mUnk_2dc.mUnk_44 = 0x128e000;
    memset(&data_020b6ee0.mUnk_f9c, 0, 0x44);
    func_02031fc4(0, &data_020b6ee0.mUnk_f9c);
    data_020b6ee0.mUnk_fdf &= ~7;
    func_020271ec(&data_020b6ee0.mModel[0], 0, 0, 0, data_0201aa60[0]);
    data_020b13b0.mUnk_01 = 0;
    data_020b13b0.mUnk_00 = 0;
}

void func_02014260(void) {
    u8 order[5] = {0, 1, 2, 3, 4};
    u16 keys;

    func_020073b0();
    func_02033614(data_020c3e44.mUnk_2024.mUnk_80.x, data_020c3e44.mUnk_2024.mUnk_80.z);
    func_020072a8();
    func_0200b8dc(10, 10, "t:%i a:%i", data_020b13b0.mUnk_00, data_020b13b0.mUnk_01);
    keys = data_020c18bc.mUnk_12;
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
            func_02007244(data_020b13b0.mUnk_01);
            data_020b6ee0.mModel[0].mUnk_142 = 0;
            data_020b6ee0.mModel[0].mUnk_134 |= 1 << order[data_020b13b0.mUnk_00];
        }
    } else if (keys & 0x80) {
        if (data_020b13b0.mUnk_01 != 0) {
            data_020b13b0.mUnk_01--;
            func_02007244(data_020b13b0.mUnk_01);
            data_020b6ee0.mModel[0].mUnk_142 = 0;
            data_020b6ee0.mModel[0].mUnk_134 |= 1 << order[data_020b13b0.mUnk_00];
        }
    }
}

void func_0201438c(void) {}

void func_02014390(int a) {
    int i;

    data_020b6ee0.mUnk_a530 = 0;
    data_020b6ee0.mUnk_a560 = 0;
    data_020b6ee0.mUnk_a538 = 0;
    data_020b6ee0.mUnk_a53c = 0;
    data_020b6ee0.mUnk_a8e4 = -1;
    data_020b6ee0.mUnk_a147 = 0;
    data_020b6ee0.mUnk_a14a = 1;
    data_020b6ee0.mUnk_a14c = 0;
    data_020b6ee0.mUnk_a564 = 1;
    data_020b6ee0.mUnk_a568 = 0;
    data_020b6ee0.mUnk_a56c = 0;
    data_020b6ee0.mUnk_a56d = 0;
    data_020b6ee0.mUnk_a56e = 0;
    data_020b6ee0.mUnk_a56f = 0;
    data_020b6ee0.mUnk_a570 = 0;
    data_020b6ee0.mUnk_a5d0 = 1;
    func_02011bcc(-1, 0, 0);
    data_020b10fc.mUnk_08 = 0;
    for (i = 0; i < 5; i++) {
        data_020b10fc.mUnk_14[i] = 0;
    }
}

void func_020143f4(void) {
    func_02014390(0);
    func_02014d8c(0);
    func_020150ec(0);
}

void func_0201440c(void) {
    func_02014390(1);
    func_02014d8c(1);
    func_020150ec(1);
    func_0201079c(data_020b1084.mUnk_0c);
}

void func_02014430(int a) {
    int local;
    int timer;
    int fired;
    int running = 1;

    if (func_0200fb68(&local, 0) == 1) {
        timer = 0x78;
        fired = 0;
        func_0202eeec();
        func_02010500(running);
        do {
            timer--;
            if (timer == 0) {
                running = 0;
            }
            if (func_0201f794(3, 3) != 0 && fired == 0) {
                fired = 1;
                func_0201944c();
                switch (a) {
                    case 0:
                        func_020109f4(data_020b1084.mUnk_0c);
                        break;
                    case 1:
                        func_02010cfc();
                        break;
                }
            }
            func_020101d0();
            func_020103c0();
            func_0201bb30();
        } while (running == 1);
        func_02010500(2);
        func_0200fd90();
    }
}

int func_020144a4(int a) {
    int i;

    if (a != -1) {
        for (i = 0; i < TABLE_ENTRY_COUNT; i++) {
            if (a == func_02021e44(0x1d9, i * 20 + 2)) {
                return i;
            }
        }
        if (i >= TABLE_ENTRY_COUNT) {
            OS_Terminate();
        }
    }
    return -1;
}

int func_020144e0(int a, int b) {
    if ((u32) a >= TABLE_ENTRY_COUNT) {
        OS_Terminate();
    }
    if ((u32) (func_02021e08(0x1d9, b + a * 20) - 2) <= 1) {
        return func_02021e44(0x1d9, b + a * 20);
    }
    return -1;
}

int func_02014518(int stage, int field) {
    int index = func_020144a4(stage);

    if (index == -1) {
        OS_Terminate();
    }
    return func_020144e0(index, field);
}

int func_02014538(int a, int b) {
    int index;

    if ((u32) a >= 0x19) {
        OS_Terminate();
    }
    index = b + a * 17;
    if ((u32) (func_02021e08(0xda2, index) - 2) <= 1) {
        return func_02021e44(0xda2, index);
    }
    return -1;
}

int func_0201456c(int a) {
    int result;
    int other;

    if ((u32) a >= 3) {
        OS_Terminate();
    }
    if (data_020b6ee0.mUnk_a534 == -1) {
        OS_Terminate();
    }
    result = func_02014538(data_020b6ee0.mUnk_a534, a + 8);
    if (func_020149b0() == 0) {
        other = func_02014538(data_020b6ee0.mUnk_a534, a + 11);
        if (other != -1) {
            result = other;
        }
    }
    return result;
}
