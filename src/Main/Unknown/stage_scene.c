#include <Main/Unknown/functions.h>
#include <Main/Unknown/touch_buttons.h>
#include <Main/Unknown/random_angle.h>
#include <Main/Unknown/screen_fade.h>
#include <Main/Unknown/game_math.h>
#include <Main/Unknown/system_init.h>
#include <Main/Unknown/stage_info_panel.h>
#include <Main/Unknown/sound_control.h>
#include <Main/Unknown/game_util.h>
#include <Main/Unknown/globals.h>
#include <Main/Unknown/mode_select.h>
#include <Main/Unknown/move_object.h>
#include <Main/Unknown/stage_scene.h>
#include <Main/Unknown/sprite_slots.h>
#include <Main/Unknown/text_draw.h>
#include <Main/Unknown/camera.h>
#include <Main/clib.h>
#include <nitro/fx.h>
#include <nitro/gx.h>
#include <nitro/os.h>

void func_0200c65c(int index, VecFx32 *out) {
    void *entry = func_0203204c(index, 0);
    if (entry == 0) {
        OS_Terminate();
    }
    out->x = func_02031f98(entry);
    out->y = func_02031fa4(entry);
    out->z = func_02031fb0(entry);
}

void func_0200c688(int n) {
    data_02063940->mUnk_60c = (60 * (u32) n) / 1000;
}

void func_0200c6ac(void) {
    data_020c5e44.mUnk_300.mUnk_44 = 0x030d4000;
    data_020c5e44.mUnk_300.mUnk_54 = 0x640;
    func_02033e38(1, 1, 1);
    func_02033e38(8, 0, 1);
    data_020c5e44.mUnk_300.mUnk_30 = 1;
    func_0200c65c(1, &data_02063940->mUnk_5b4);
    func_0200c65c(0, &data_02063940->mUnk_5c0);
    func_0200c65c(2, &data_02063940->mUnk_5cc);
    func_0200c65c(4, &data_02063940->mUnk_5d8);
    memset(&data_02063940->mUnk_5e4, 0, 12);
    memset(&data_02063940->mUnk_5f0, 0, 12);
    memset(&data_02063940->mUnk_5fc, 0, 12);
    data_02063940->mUnk_608 = 0;
    data_02063940->mUnk_60c = 0;
    data_020c5e44.mUnk_300.mUnk_38 = 0;
    data_020c5e44.mUnk_300.mUnk_40 = 0;
    data_020c5e44.mUnk_300.mUnk_3c = 0;
    data_020c5e44.mUnk_000.mUnk_7f &= ~2;
    func_02033df4(25);
    data_020c5e44.mUnk_300.mUnk_14 = data_020c5e44.mUnk_300.mUnk_18;
    data_020c5e44.mUnk_300.mUnk_10 = 0;
}

void func_0200c7a4(void) {
    Unk_020c5f04 *cam = &data_020c5e44.mUnk_0c0;
    VecFx32 a;
    VecFx32 b;
    a.x = func_0201fbe4(data_02063940->mUnk_618, 0, data_02063940->mUnk_614, data_02063940->mUnk_5cc.x >> 8,
                        data_02063940->mUnk_5b4.x >> 8)
          << 8;
    a.y = func_0201fbe4(data_02063940->mUnk_618, 0, data_02063940->mUnk_614, data_02063940->mUnk_5cc.y >> 8,
                        data_02063940->mUnk_5b4.y >> 8)
          << 8;
    a.z = func_0201fbe4(data_02063940->mUnk_618, 0, data_02063940->mUnk_614, data_02063940->mUnk_5cc.z >> 8,
                        data_02063940->mUnk_5b4.z >> 8)
          << 8;
    b.x = func_0201fbe4(data_02063940->mUnk_618, 0, data_02063940->mUnk_614, data_02063940->mUnk_5d8.x >> 8,
                        data_02063940->mUnk_5c0.x >> 8)
          << 8;
    b.y = func_0201fbe4(data_02063940->mUnk_618, 0, data_02063940->mUnk_614, data_02063940->mUnk_5d8.y >> 8,
                        data_02063940->mUnk_5c0.y >> 8)
          << 8;
    b.z = func_0201fbe4(data_02063940->mUnk_618, 0, data_02063940->mUnk_614, data_02063940->mUnk_5d8.z >> 8,
                        data_02063940->mUnk_5c0.z >> 8)
          << 8;
    func_0200b960(&cam->mUnk_204, a, 4);
    func_0200b960(&cam->mUnk_228, b, 4);
    if (data_02063940->mUnk_60c != 0) {
        data_02063940->mUnk_60c--;
        data_02063940->mUnk_608 = (data_02063940->mUnk_608 + 1) % 3;
        if (data_02063940->mUnk_608 == 0) {
            data_02063940->mUnk_5e4.x = ((func_0201fd68() & 7) - 5) << 12;
            data_02063940->mUnk_5e4.y = ((func_0201fd68() & 7) - 5) << 12;
            data_02063940->mUnk_5e4.z = ((func_0201fd68() & 7) - 5) << 12;
            data_02063940->mUnk_5fc.x = (data_02063940->mUnk_5e4.x - data_02063940->mUnk_5f0.x) / 2;
            data_02063940->mUnk_5fc.y = (data_02063940->mUnk_5e4.y - data_02063940->mUnk_5f0.y) / 2;
            data_02063940->mUnk_5fc.z = (data_02063940->mUnk_5e4.z - data_02063940->mUnk_5f0.z) / 2;
        }
        data_02063940->mUnk_5f0.x += data_02063940->mUnk_5fc.x;
        data_02063940->mUnk_5f0.y += data_02063940->mUnk_5fc.y;
        data_02063940->mUnk_5f0.z += data_02063940->mUnk_5fc.z;
        cam->mUnk_228.x += data_02063940->mUnk_5f0.x;
        cam->mUnk_228.y += data_02063940->mUnk_5f0.y;
        cam->mUnk_228.z += data_02063940->mUnk_5f0.z;
    }
    func_02033fc8();
}

void func_0200ca14(void) {
    u16 area[5];
    int i;
    int stage = data_020b6ee0.mUnk_a8e4;
    int ok;
    s32 idx;
    if (stage < 0) {
        stage = 0;
    }
    data_02063940->mStage = data_020b1990[stage].mInfo;
    idx = data_02063940->mStage.mUnk_11 - 1;
    ok = 0;
    data_020b10fc.mUnk_08 = idx;
    if (idx >= 0 && idx < 12) {
        ok = 1;
    }
    if (!ok) {
        OS_Terminate();
    }
    func_0200c5d8(0, 0, data_02063940->mStage.mUnk_13 + 0xdb3, 0x180, 16, 0, 2);
    func_02024208();
    func_020104c0(4, 0x4c);
    func_0200dc08(data_0204e1c0[data_02063940->mStage.mUnk_10], &area[4], &area[3], &area[2], &area[1], &area[0], 0);
    func_020325b0(area[4], area[2], area[3], 0, area[1], area[0]);
    func_0201bb30();
    func_0200c6ac();
    G3X_SetClearColor(0, 8, 0x7fff, 0, 0);
    memset(&data_02063940->mUnk_3a8, 0, sizeof(DrawParams));
    func_02031fc4(3, &data_02063940->mUnk_3a8.mPos);
    data_02063940->mUnk_3a8.mUnk_43 = 0;
    func_020271ec(&data_02063940->mUnk_230, 0, 0, 0, data_0201abbc[data_02063940->mStage.mUnk_10 + 1]);
    data_02063940->mUnk_5a8 = data_02063940->mUnk_3a8.mPos;
    func_020271ec(&data_02063940->mUnk_3ec, 0, 0, 0, data_0201abbc[0]);
    data_020b10fc.mUnk_00 = 0;
    data_02063940->mUnk_61c = data_020b10fc.mUnk_08 - 1;
    data_02063940->mUnk_610 = 0;
    data_02063940->mUnk_654 = 0;
    func_0200d434();
    func_0200ce24();
    func_0200d5f0();
    for (i = 0; i < 9; i++) {
        data_02063940->mSlots[i].mUnk_20 = 7;
    }
    func_02005a8c(1, 0);
}

void func_0200cbe8(void) {
    u8 mode;
    int busy;
    func_020059cc(data_02063940->mUnk_610);
    mode = func_02005bf8();
    if (mode != 0) {
        func_0200c7a4();
        func_0200cd2c();
        busy = func_02005b1c(1);
        if (mode == 2 && busy == 0) {
            func_020059b4(func_02005c04(), data_02063940->mUnk_610);
        }
        return;
    }
    func_0200c7a4();
    func_0200cd2c();
    func_0200b960(&data_02063940->mUnk_3a8.mPos, data_02063940->mUnk_5a8, 12);
    switch (data_020b10fc.mUnk_00) {
        case 0:
            func_0200ceac();
            data_020b10fc.mUnk_00 = 1;
        case 1:
            data_02063940->mUnk_654--;
            if (data_02063940->mUnk_654 <= 0) {
                func_0200ce24();
            }
            data_02063940->mUnk_634--;
            if (data_02063940->mUnk_634 <= 0) {
                func_0200ceac();
            }
            data_02063940->mUnk_618--;
            if (data_02063940->mUnk_618 <= 0) {
                data_020b10fc.mUnk_00 = 3;
                data_02063940->mUnk_230.mUnk_134 |= 2;
            }
        case 2:
            func_0200d098();
            break;
        case 3:
            if (data_02063940->mUnk_230.mUnk_172_9) {
                func_02005a8c(0, data_02063940->mUnk_610 >= data_02063940->mUnk_630);
            }
            break;
    }
}

void func_0200cd1c(void) {
    func_0201f880(3);
    func_02022370();
}

void func_0200cd2c(void) {
    func_0201168c(data_020b6ee0.mUnk_a8e4, data_02063940->mUnk_610, data_0206d448, data_02063940->mUnk_618, 2);
    func_0200cdc4();
    func_02033614(data_020c5e44.mUnk_0c0.mUnk_228.x, data_020c5e44.mUnk_0c0.mUnk_228.z);
    func_0202733c(&data_02063940->mUnk_230, &data_02063940->mUnk_3a8, 0xb0000002);
    func_020105f4(&data_02063940->mUnk_230);
    func_02024368(&data_02063940->mUnk_3ec, 255, 0, 0x1000, 0, 0);
    if (data_020b10fc.mUnk_00 < 3) {
        func_0200d638();
    }
}

void func_0200cdc4(void) {
    int n = 3 - data_02063940->mUnk_624;
    int i;
    func_0202de80(0xe3a, 8, 84, 100, -1, 2);
    for (i = 0; i < n; i++) {
        func_0202de80(0xe39, data_0204e1cc[i][0], data_0204e1cc[i][1], 100, -1, 2);
    }
}

void func_0200ce24(void) {
    data_02063940->mUnk_61c++;
    data_02063940->mUnk_654 = data_02063940->mUnk_650 * 60;
    if (data_02063940->mUnk_638 >= data_02063940->mUnk_640 + data_02063940->mUnk_63c) {
        data_02063940->mUnk_638 -= data_02063940->mUnk_63c;
    } else {
        data_02063940->mUnk_638 = data_02063940->mUnk_640;
    }
    if (data_02063940->mUnk_644 >= data_02063940->mUnk_64c + data_02063940->mUnk_648) {
        data_02063940->mUnk_644 -= data_02063940->mUnk_648;
    } else {
        data_02063940->mUnk_644 = data_02063940->mUnk_64c;
    }
}

void func_0200ceac(void) {
    u32 mask;
    s32 cum[4];
    s32 total;
    int i;
    int n;
    int k;
    int slot;
    int roll;
    Scene *p;

    total = -1;
    mask = func_0200bf54(9);
    p = data_02063940;
    p->mUnk_634 = p->mUnk_638 * 60 / 1000;
    for (i = 0; i < POPUP_SLOT_COUNT; i++) {
        if (data_02063940->mSlots[i].mUnk_20 != 7) {
            mask |= 1 << data_02063940->mSlots[i].mUnk_24;
        }
    }
    if (mask != -1) {
        do {
            int t = data_02063940->mUnk_61c * 5;
            int kind = func_02021e08(0xd64, t + 6);
            if (kind != 2 && kind != 3) {
                OS_Terminate();
            }
            data_02063940->mUnk_620 = func_02021e44(0xd64, t + 6);
            if (data_02063940->mUnk_620 == 0) {
                data_02063940->mUnk_61c--;
            }
        } while (data_02063940->mUnk_620 == 0);
        for (i = 0; i < 4; i++) {
            total += func_02021e44(0xd64, data_02063940->mUnk_61c * 5 + 2 + i);
            cum[i] = total;
        }
        n = 0;
        if (data_02063940->mUnk_62c > data_02063940->mUnk_628) {
            data_02063940->mUnk_62c = 0;
            slot = func_0200bf68(&mask);
            func_0200d014(slot, 1, slot);
            n++;
        }
        for (; n < data_02063940->mUnk_620; n++) {
            if (mask == -1) {
                break;
            }
            roll = func_0201fd84(0, total - 1);
            k = 0;
            while (cum[k] < roll) {
                k++;
            }
            slot = func_0200bf68(&mask);
            func_0200d014(slot, k, slot);
        }
    }
}

void func_0200d014(int slot, int type, int index) {
    PopupSlot *slots = data_02063940->mSlots;
    PopupSlot *e = &slots[slot];
    e->mUnk_20 = 0;
    e->mUnk_24 = index;
    slots[slot].mUnk_00 = type;
    e->mUnk_08 = 0x10;
    e->mUnk_0c = 0x10;
    e->mUnk_14 = 0x10;
    e->mUnk_18 = 0x10;
    e->mUnk_10 = data_02063940->mUnk_644 * 60 / 1000;
    e->mUnk_04 = 0;
    if (slots[slot].mUnk_00 == 2) {
        e->mUnk_1c = data_02063940->mUnk_658;
    } else if (slots[slot].mUnk_00 == 1) {
        e->mUnk_1c = data_02063940->mUnk_65c;
    } else if (slots[slot].mUnk_00 != 3) {
        e->mUnk_1c = data_02063940->mUnk_660;
    }
}

void func_0200d098(void) {
    int i;
    Scene *p;
    PopupSlot *e;
    int x;
    int y;
    int redraw;

    for (i = 0; i < POPUP_SLOT_COUNT; i++) {
        p = data_02063940;
        e = &p->mSlots[i];
        redraw = 0;
        x = e->mUnk_24 % 3 * 64 + 63;
        y = e->mUnk_24 / 3 * 48 + 64;
        switch (e->mUnk_20) {
            case 0:
                e->mUnk_04--;
                if (e->mUnk_04 <= 0) {
                    e->mUnk_20 = 1;
                }
                break;
            case 1:
                e->mUnk_0c--;
                func_0202de80(func_0200bfc4(e->mUnk_0c, e->mUnk_08, 0, 0, 6) + 0xe85, x, y, 0xbe, -1, 1);
                if (e->mUnk_0c <= 2) {
                    redraw = 1;
                }
                if (e->mUnk_0c <= 0) {
                    e->mUnk_20 = 2;
                }
                break;
            case 2:
                e->mUnk_10--;
                redraw = 1;
                if (e->mUnk_10 <= 0) {
                    e->mUnk_20 = 3;
                }
                break;
            case 3:
                e->mUnk_18--;
                func_0202de80(func_0200bfc4(e->mUnk_18, e->mUnk_14, 0, 6, 0) + 0xe85, x, y, 0xbe, -1, 1);
                if (e->mUnk_18 <= 0) {
                    e->mUnk_20 = 7;
                }
                break;
            case 4:
                e->mUnk_18--;
                func_020242e8(0xd0f, x, y, 0x1000, 0x1000, 0, 0xff, 0);
                if (e->mUnk_18 <= 0) {
                    e->mUnk_20 = 7;
                }
                break;
            case 5:
                data_020b10fc.mUnk_00 = 3;
                p->mUnk_230.mUnk_134 |= 2;
                break;
            case 6:
            case 7:
                break;
        }
        if (redraw == 1) {
            func_020242e8(e->mUnk_00 + 0xd0b, x, y, 0x1000, 0x1000, 0, 0xff, 0);
            func_02020978(i + 0x100, x, y, 0, 0, 0xe83);
        }
    }
    if (data_020b10fc.mUnk_00 < 2 && data_020c18bc.mUnk_2ac_0) {
        if (func_0200bdac(0x80000000, 0x100, 0x108)) {
            func_0200d2a4(&data_02063940->mSlots[(u16) data_020c2560.mUnk_100.mUnk_04 - 0x100]);
        }
    }
}

void func_0200d2a4(PopupSlot *slot) {
    PopupPos pos;

    pos.x = slot->mUnk_24 % 3 * 64 + 63;
    pos.y = slot->mUnk_24 / 3 * 48 + 64;
    func_0200d76c(5, pos.x, pos.y, 30, 1);
    slot->mUnk_20 = 4;
    slot->mUnk_18 = 30;
    data_02063940->mUnk_62c++;
    if (slot->mUnk_00 == 3) {
        data_02063940->mUnk_624--;
        data_02063940->mUnk_62c = 0;
        if (data_02063940->mUnk_624 <= 0) {
            slot->mUnk_10 = 0;
            slot->mUnk_20 = 5;
            data_020b10fc.mUnk_00 = 2;
        }
        func_0200c688(200);
        func_0202f110(0x72, 100, 0, 0x40, 0xffff, 0);
    } else if (slot->mUnk_00 == 1) {
        data_02063940->mUnk_618 += data_02063940->mUnk_65c;
        data_02063940->mUnk_618 =
            data_02063940->mUnk_618 < data_02063940->mUnk_614 ? data_02063940->mUnk_618 : data_02063940->mUnk_614;
        func_0202f110(0x75, 100, 0, 0x40, 0xffff, 0);
    } else if (slot->mUnk_00 == 2) {
        func_020059d0(&data_02063940->mUnk_610, data_02063940->mUnk_658, 0);
        func_0200d60c(data_02063940->mUnk_658, &pos);
        data_02063940->mUnk_3a8.mPos.z = data_02063940->mUnk_5a8.z - 0xc800;
        func_0202f110(0x74, 100, 0, 0x40, 0xffff, 0);
    } else {
        func_020059d0(&data_02063940->mUnk_610, data_02063940->mUnk_660, 0);
        func_0200d60c(data_02063940->mUnk_660, &pos);
        data_02063940->mUnk_3a8.mPos.z = data_02063940->mUnk_5a8.z - 0xc800;
        func_0202f110(0x73, 100, 0, 0x40, 0xffff, 0);
    }
}

static inline int lookup_d63(int idx, int k) {
    return func_02021e44(0xd63, idx * 3 + k);
}

static inline int lookup_d62(int idx, int k) {
    return func_02021e44(0xd62, 11 * idx + k);
}

void func_0200d434(void) {
    data_02063940->mUnk_618 = lookup_d63(data_020b10fc.mUnk_08, 2) * 60;
    data_02063940->mUnk_614 = data_02063940->mUnk_618;
    data_02063940->mUnk_630 = lookup_d63(data_020b10fc.mUnk_08 + 1, 0);
    data_02063940->mUnk_624 = lookup_d63(data_020b10fc.mUnk_08, 4);
    data_02063940->mUnk_638 = lookup_d62(data_020b10fc.mUnk_08, 2);
    data_02063940->mUnk_63c = lookup_d62(data_020b10fc.mUnk_08, 3);
    data_02063940->mUnk_640 = lookup_d62(data_020b10fc.mUnk_08, 4);
    data_02063940->mUnk_650 = lookup_d62(data_020b10fc.mUnk_08, 5);
    data_02063940->mUnk_644 = lookup_d62(data_020b10fc.mUnk_08, 6);
    data_02063940->mUnk_648 = lookup_d62(data_020b10fc.mUnk_08, 7);
    data_02063940->mUnk_64c = lookup_d62(data_020b10fc.mUnk_08, 8);
    data_02063940->mUnk_660 = lookup_d62(data_020b10fc.mUnk_08 + 1, 0);
    data_02063940->mUnk_65c = lookup_d62(data_020b10fc.mUnk_08, 10) * 60;
    data_02063940->mUnk_658 = lookup_d62(data_020b10fc.mUnk_08, 9);
    data_02063940->mUnk_628 = lookup_d62(data_020b10fc.mUnk_08, 12);
}

void func_0200d5f0(void) {
    data_02063940->mUnk_7d4 = 0;
    data_02063940->mUnk_7d0 = 0;
}

void func_0200d60c(int value, const PopupPos *pos) {
    data_02063940->mUnk_7d4 = 0;
    data_02063940->mPos = *pos;
    data_02063940->mUnk_7d0 = value;
}

void func_0200d638(void) {
    Scene *p = data_02063940;
    int x;
    int y;

    if (p->mUnk_7d4 < 0x4000 && p->mUnk_7d0 != 0) {
        x = p->mPos.x;
        func_0201fb24(&p->mUnk_7d4, 12);
        p = data_02063940;
        y = p->mPos.y - func_0201fbe4(p->mUnk_7d4, 0, 0x4000, 0, 30);
        if (y <= 20) {
            y = 20;
        }
        func_0200bffc(x, y, p->mUnk_7d0, data_0201a680, 2, 1);
    }
}

int func_0200d6b0(u8 stage) {
    return func_02021e44(0xd63, (stage + 1) * 3);
}
