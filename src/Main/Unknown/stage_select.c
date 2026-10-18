#include <nitro/os.h>
#include <Main/clib.h>
#include <Main/Unknown/functions.h>
#include <Main/Unknown/system_init.h>
#include <Main/Unknown/stage_progress.h>
#include <Main/Unknown/sound_control.h>
#include <Main/Unknown/save_data.h>
#include <Main/Unknown/game_util.h>
#include <Main/Unknown/state_flow.h>
#include <Main/Unknown/battle_screen.h>
#include <Main/Unknown/prompt_screen.h>
#include <Main/Unknown/language_menu.h>
#include <Main/Unknown/globals.h>
#include <Main/Unknown/stage_data.h>
#include <Main/Unknown/stage_info_panel.h>
#include <Main/Unknown/stage_select.h>

#define TABLE_ENTRY_COUNT 100

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
    data_020c5e44.mUnk_28 = func_0201418c;
    data_020c5e44.mUnk_354 = 0x640;
    data_020c5e44.mUnk_344 = 0x128e000;
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
    func_02033614(data_020c5e44.mUnk_a4.x, data_020c5e44.mUnk_a4.z);
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
