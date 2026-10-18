#include <Main/Unknown/functions.h>
#include <Main/Unknown/globals.h>
#include <Main/Unknown/language_menu.h>
#include <Main/clib.h>
#include <nitro/os.h>
#include <stdio.h>

Unk_020ae6ec data_020ae6ec;
LangMenu data_020ae870;

void func_0200f374(void) {
    s32 *p = &data_020ae6ec.mUnk_00;
    u32 idx = func_0200fb34(0x14);
    u32 v;
    func_02010294(0x180, 1, 1);
    if (data_0204e378[idx].mUnk_00 >= 0) {
        func_0200f848(data_0204e378[idx].mUnk_00, data_0204e378[idx].mUnk_04);
        data_020c642c.mUnk_40 = 1;
    } else {
        data_020c642c.mUnk_40 = 0;
    }
    func_0201dde0(0x237, 2);
    func_02024208();
    func_020104c0(5, 0x66);
    *p = -1;
    func_02028f0c(0xfbc, func_0200f9c8(data_020b1084.mLanguage), 0x244, 0x246, 2, 0, 0, 0, 0);
    data_020c61ac.mUnk_20 = 0;
    func_0200f5a4("LANGUAGE", data_020b1084.mLanguage);
    v = func_0200f9c8(data_020b1084.mLanguage);
    func_02028f0c(0xfbc, v, 0x244, 0x246, func_0200fb34(0x17), 0, 0, func_0200f588, 0);
    v = func_0200fb34(0x16);
    if (v != 0) {
        func_0200fa80(v);
        func_0200fb18(0x16, 0);
    }
}

u8 func_0200f468(void) {
    Unk_020ae6ec *state = &data_020ae6ec;
    if (data_020ae6ec.mUnk_00 != -1) {
        int i = state->mUnk_00;
        const Unk_0204e218 *e = &data_0204e218[i];
        func_0201fb24(&state->mUnk_04, 8);
        data_020c61ac.mUnk_2a = func_0201fb74(state->mUnk_04, 0x4000, e->mUnk_08, 0);
        func_0202a1cc();
        func_0202cf08(e->mUnk_0c + 0x800000, data_0204e218[i].mUnk_00, e->mUnk_04 + data_020c61ac.mUnk_2a, 0x80, 1);
        return 0x7e;
    }
    return 0;
}

void func_0200f4d4(void) {
    int id;
    u32 idx;
    if (data_020c2660.mUnk_04 == 0x40000400) {
        id = 0xf8f;
    } else {
        id = 0xf8e;
    }
    idx = func_0200fb34(0x14);
    if (data_0204e378[idx].mUnk_00 >= 0) {
        func_0200f898();
    }
    func_0202a1ec(func_0200f468);
    func_0200f5c4();
    if (data_020c61ac.mUnk_1a & 4) {
        if (data_0204e378[idx].mUnk_08 >= 0) {
            func_0201033c(data_0204e378[idx].mUnk_08, 0);
            func_0200fa50();
        } else {
            func_0200fab0();
        }
    }
    if (func_0202a188() != 3) {
        func_02020944(0x400, 0x80, 0x60, 0, 0, id, 0, 0x168);
        func_0202cf08(id, 0x80, 0x60, 0, 2);
    }
}

void func_0200f580(void) {
    func_020102f8();
}

u8 func_0200f588(void) {
    u8 buf[0x20];
    u8 i;
    buf[0] = 0;
    i = data_020c1bbc.mUnk_16++;
    return buf[i];
}

void func_0200f5a4(const char *key, u32 value) {
    char buf[0x20];
    sprintf(buf, "%s=%i", key, value);
    func_020292f4(buf);
}

void func_0200f5c4(void) {
    Unk_EventRec *rec;
    Unk_020ae6ec *state = &data_020ae6ec;
    int ok;
    while ((rec = func_0202a1a4()) != 0) {
        switch (rec->mUnk_00) {
            case 6:
                ok = rec->mUnk_02 >= 0 && rec->mUnk_02 <= 3;
                if (!ok) {
                    OS_Terminate();
                }
                state->mUnk_00 = rec->mUnk_02 - 1;
                state->mUnk_04 = 0;
                break;
            case 7:
                if (!data_020c61ac.mUnk_1c_2) {
                    const Unk_0204e248 *e = &data_0204e248[rec->mUnk_02];
                    u32 base = e->mUnk_02 == 1 ? data_020b1084.mLanguage : 0;
                    func_0202f110(base + e->mUnk_00, 100, 0, 0x40, 0xffff, 0);
                }
                break;
        }
    }
}

void func_0200f660(void) {
    LangMenu *menu = &data_020ae870;
    int i;
    char buf[0x20];
    func_02010294(0x100, 1, 0);
    func_0201dde0(0xdaa, 3);
    func_02024208();
    func_020104c0(0, 0x80);
    i = 2;
    data_020ae870.mCount = 0;
    for (;; i++) {
        if (func_02021e08(0x246, i) == 4) {
            const u8 *s = func_02021e18(0x246, i);
            if (s[0] != 0) {
                func_0202938c(buf, s);
                if (func_020290ac("LAST", buf) == 1) {
                    break;
                }
                if (menu->mCount >= LANG_MENU_MAX) {
                    OS_Terminate();
                }
                menu->mEntries[menu->mCount].mTextId = i;
                menu->mEntries[menu->mCount].mIndex = menu->mCount - 1;
                strcpy(menu->mEntries[menu->mCount].mText, buf);
                menu->mCount++;
            }
        }
    }
    menu->mSelected = func_0200fb34(0x15);
    if (menu->mCount <= 1) {
        OS_Terminate();
    }
    if (menu->mSelected >= menu->mCount) {
        OS_Terminate();
    }
}

void func_0200f750(void) {
    func_020102f8();
}

void func_0200f758(void) {
    LangMenu *menu = &data_020ae870;
    func_0200b920(0x10, "CS (U/D): %i", data_020ae870.mSelected - 1);
    func_0200b920(0x20, "Language (LB/RB): %s", func_0200fa10(data_020b1084.mLanguage + 2, data_020b1084.mLanguage));
    if ((data_020c18bc.mUnk_12 & 0x80) && menu->mSelected > 1) {
        menu->mSelected--;
    } else if (data_020c18bc.mUnk_12 & 0x40) {
        if (menu->mSelected < menu->mCount - 1) {
            menu->mSelected++;
        }
    }
    if ((data_020c18bc.mUnk_12 & 0x200) && data_020b1084.mLanguage != 0) {
        data_020b1084.mLanguage--;
    }
    func_020215fc(0x80, 0x60, data_0201a6c6, menu->mEntries[menu->mSelected].mText, 2, 0xe0, 0x10, 1);
    if (data_020c18bc.mUnk_12 & 1) {
        func_0200fb18(0x14, menu->mEntries[menu->mSelected].mIndex);
        func_0200fb18(0x15, menu->mSelected);
        func_02010364(menu->mEntries[menu->mSelected].mTextId, 0xc);
    }
}

void func_0200f848(u32 a, int b) {
    if (a >= func_0200f988()) {
        OS_Terminate();
    }
    func_02027230(data_020ae6f8, a * 2 + 0x1f5, a * 2 + 0x1f4, 2, 0);
    data_020ae6f4 = b;
    if (b == 0) {
        data_020ae834.mUnk_36_13 = 1;
    }
}

void func_0200f898(void) {
    u8 params[0x44];
    int idx = (data_020ae834.mUnk_34 - 0x1f5) / 2;
    int a, x, y, z;
    func_020242a8(params, 0xff, 0, 0, 0x1000, 0x8000, -1, 0x100);
    params[0x43] |= 8;
    func_020262a0(data_020ae6f8);
    func_02024b5c(data_020ae834.mUnk_34, params, data_020ae6f8, 0);
    a = func_02021e44(0x235, idx * 4 + 2);
    if (data_020ae834.mUnk_12 >= a) {
        x = func_02021e44(0x235, idx * 4 + 4);
        y = func_02021e44(0x235, idx * 4 + 5);
        z = func_02021e44(0x235, idx * 4 + 3);
        if (data_020ae834.mUnk_12 < a + 4) {
            x = func_0201fbe4(data_020ae834.mUnk_12, a, a + 4, x - 0x78, x);
        }
        func_0202cf08(z * 6 + 0xee, x, y, 0x64, 1);
    }
}

int func_0200f95c(void) {
    if (data_020ae6f4 != 0) {
        return 0;
    }
    return data_020ae834.mUnk_36_10;
}

u16 func_0200f97c(void) {
    return data_020ae834.mUnk_12;
}

int func_0200f988(void) {
    int n = 0;
    while (func_02021e44(0x235, 2 + n * 4) != -1) {
        n++;
    }
    return n;
}

int func_0200f9b8(void) {
    OSOwnerInfo info;
    OS_GetOwnerInfo(&info);
    return 0;
}
