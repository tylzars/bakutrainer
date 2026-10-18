#include <Main/Unknown/functions.h>
#include <Main/Unknown/mode_select.h>
#include <Main/Unknown/globals.h>
#include <Main/Unknown/popup_slots.h>
#include <nitro/os.h>

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
    PopupWork *p;

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
    PopupWork *p;
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
                p->mUnk_364 |= 2;
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
    if (data_020b10fc.mUnk_00 < 2 && data_020c1b5c.mFlag) {
        if (func_0200bdac(0x80000000, 0x100, 0x108)) {
            func_0200d2a4(&data_02063940->mSlots[(u16) data_020c2660.mUnk_04 - 0x100]);
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
        data_02063940->mUnk_3b0 = data_02063940->mUnk_5b0 - 0xc800;
        func_0202f110(0x74, 100, 0, 0x40, 0xffff, 0);
    } else {
        func_020059d0(&data_02063940->mUnk_610, data_02063940->mUnk_660, 0);
        func_0200d60c(data_02063940->mUnk_660, &pos);
        data_02063940->mUnk_3b0 = data_02063940->mUnk_5b0 - 0xc800;
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
    PopupWork *p = data_02063940;
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
