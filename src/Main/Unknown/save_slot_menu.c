#include <Main/Unknown/functions.h>
#include <Main/Unknown/stage_select.h>
#include <Main/Unknown/move_object.h>
#include <Main/Unknown/prompt_screen.h>
#include <Main/Unknown/sound_control.h>
#include <Main/Unknown/globals.h>
#include <Main/Unknown/state_flow.h>
#include <Main/Unknown/game_util.h>
#include <Main/Unknown/language_menu.h>
#include <Main/Unknown/save_data.h>
#include <Main/Unknown/save_slot_menu.h>
#include <Main/clib.h>

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
    data_020b10d8.mUnk_0d = 1;
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
            grade = data_020b10d8.mUnk_0d;
            if (grade != 12) {
                grade++;
            }
            data_020b10d8.mUnk_0d = grade;
        }
        if (pad & 0x20) {
            grade = data_020b10d8.mUnk_0d;
            if (grade != 1) {
                grade--;
            }
            data_020b10d8.mUnk_0d = grade;
        }
        func_0200b898(0x10, 0x10, "grade: %i", data_020b10d8.mUnk_0d);
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
            start = data_020b1090[i];
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
            data_020b1094[data_020b1078.mUnk_04] = data_020b1090[data_020b1078.mUnk_04];
            n = data_020b1078.mUnk_04 * ENTRY_STRIDE + 2 + data_020b1094[data_020b1078.mUnk_04];
            if ((u32) (func_02021e08(ENTRY_BLOCK, n) - 2) <= 1) {
                int v = func_02021e44(ENTRY_BLOCK, n);
                if (v < 100) {
                    data_020c17c0.mUnk_12 = v - 1;
                    data_020b6ee0.mUnk_a8e4 = -1;
                    data_020b10f8.mUnk_04[1] = 4;
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
                    data_020b10f8.mUnk_04[1] = 0;
                    func_0200fa80(14);
                    func_0200fab0();
                } else if (v < 900) {
                    data_020b10f8.mUnk_04[1] = 1;
                    func_0200fa80(15);
                    func_0200fab0();
                } else if (v < 1000) {
                    func_0200fa80(19);
                    func_0200fab0();
                } else if (v < 1100) {
                    data_020b10f8.mUnk_04[1] = 2;
                    func_0200fa80(16);
                    func_0200fab0();
                } else if (v < 1200) {
                    func_0200fa80(1);
                    func_0200fab0();
                } else if (v < 1300) {
                    data_020b10f8.mUnk_04[1] = 3;
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
