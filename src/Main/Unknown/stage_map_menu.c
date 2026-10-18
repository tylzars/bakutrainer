#include <nitro/os.h>
#include <nitro/fs.h>
#include <nitro/gx.h>
#include <nitro/mi.h>
#include <nitro/pm.h>
#include <nitro/rtc.h>
#include <nitro/snd.h>
#include <nitro/tp.h>
#include <stdio.h>
#include <Main/Unknown/functions.h>
#include <Main/Unknown/system_init.h>
#include <Main/Unknown/game_util.h>
#include <Main/Unknown/globals.h>
#include <Main/Unknown/prompt_screen.h>
#include <Main/Unknown/stage_camera.h>
#include <Main/Unknown/stage_select.h>
#include <Main/Unknown/state_flow.h>
#include <Main/Unknown/stage_map_menu.h>

void func_02019fa0(void) {
    static int prevSel = -1;
    DrawParams dp;
    VecFx32 pos;
    char buf[0x20];
    u32 i;
    u32 j;
    Unk_020c1030 *p = &data_020b6ee0.mUnk_a150;
    Unk_020c1454 *q = &data_020b6ee0.mUnk_a574;

    func_02019b3c(&data_020c180c, &data_020c1800);
    func_02033614(data_020c5e44.mUnk_98.x, data_020c5e44.mUnk_98.z);
    prevSel = data_020b6ee0.mUnk_a534;
    data_020b6ee0.mUnk_a534 = -1;
    for (i = 0; i < MAP_NODE_COUNT; i++) {
        if (i < data_020b6ee0.mUnk_a530) {
            func_0200ba68(0xde6, i + MAP_NODE_POS_ID, 0, 0, 0x1000, 0);
        } else if (i == data_020b6ee0.mUnk_a530) {
            func_02031fc4(i + MAP_NODE_POS_ID, &dp.mPos);
            func_02023f58(&dp, dp.mPos.x, dp.mPos.y, dp.mPos.z, 0x1000, 0, 0, data_020c18bc.mUnk_45c % 400, 0x100);
            func_0202733c(&p->mUnk_50, &dp, 0);
        } else {
            func_0200ba68(0xde3, i + MAP_NODE_POS_ID, 0, 0, 0x1000, 0);
        }
        if (i < data_020b6ee0.mUnk_a530 || i == data_020b6ee0.mUnk_a530) {
            func_02031fc4(i + MAP_NODE_POS_ID, &pos);
            if (func_0200baf8(&p->mUnk_00.mPos, &pos) < 0x3200) {
                data_020b6ee0.mUnk_a534 = i;
            }
        }
    }
    if (data_020b6ee0.mUnk_a534 != -1 && p->mUnk_3d8 == 0) {
        if (data_020b6ee0.mUnk_a534 != prevSel) {
            func_0202f110(0x167, 0x64, 0, 0x40, 0xffff, 0);
        }
        if (p->mUnk_3dc == 0) {
            func_02031fc4(data_020b6ee0.mUnk_a534 + MAP_NODE_POS_ID, &pos);
            p->mUnk_00.mPos.x = pos.x;
            p->mUnk_00.mPos.z = pos.z;
            p->mUnk_00.mPos.y =
                func_02031270(p->mUnk_00.mPos.x, p->mUnk_00.mPos.z, p->mUnk_00.mPos.y + 0x32000, 0, 0, 0) + 0x1900;
            p->mUnk_3dc = 1;
            p->mUnk_3dd = 0;
            p->mUnk_3d4 = 0x100;
            p->mUnk_44 = 0;
            p->mUnk_4c = 0;
            q->mUnk_38 = 0x28;
            func_0202de60(&p->mUnk_1c8, data_0201abec[0]);
            func_0202de60(&p->mUnk_2cc, data_0201abec[1]);
        }
        if (p->mUnk_3dd == 0) {
            int a = func_02014538(data_020b6ee0.mUnk_a534, 0xe);
            int b = func_02014538(data_020b6ee0.mUnk_a534, 7);

            if (p->mUnk_1c8.mUnk_de == 0) {
                func_0202cf08(func_0202db1c(&p->mUnk_1c8), 0, 7, 0xfa, 2);
            } else {
                func_0202cf08(a + 0xed6, 0, 7, 0xfa, 2);
                func_0202cf08(b + 0xedc, 0, 7, 0xfa, 2);
                func_020215fc(0x72, 0x1c, data_0201a630, func_0200f9f0(0x46), 2, 0xf0, 0x10, 2);
                sprintf(buf, "%d", data_020b6ee0.mUnk_a534 + 1);
                func_020215fc(0x72, 0x3b, data_0201a6b2, buf, 2, 0xf0, 0x10, 2);
            }
            for (j = 0; j < 3; j++) {
                int k = func_0201456c(j);

                if (k >= 0) {
                    int v1 = func_02021e44(0xfd6, k * 0x1b + 2);
                    int v2 = func_02021e44(0xfd6, k * 0x1b + 5);

                    func_0202cf08(k + 0x1089, j * 0x37 + 0x22, 0x85, 0xfa, 2);
                    func_0202cf08(0x41088, j * 0x37 + 0x22, 0x85, 0xfc, 2);
                    func_0202cf08(v1 + 0x11a1, j * 0x37 + 0x13, 0x56, 0xfa, 2);
                    sprintf(buf, "%d", v2);
                    func_020215fc(j * 0x37 + 0x13, 0x56, data_0201a6bc, buf, 0, 0x60, 0, 2);
                }
            }
            func_0200b928(&q->mUnk_38, 0, 4);
            func_0200bb14(q->mUnk_38 + 0xff, 0xa1, 0x101, 0x15, 0, 0, 0x17d, 1);
            func_0200bb14(-q->mUnk_38, 0xa1, 0x102, 0xe, 0, 0, 0x5a, 1);
        } else {
            func_020215fc(0x5a, 0x5a, data_0201a64e, func_0200f9f0(0x47), 2, 0xc8, 0x10, 2);
        }
    } else {
        p->mUnk_3dc = 0;
        func_020215fc(0x5a, 0x5a, data_0201a64e, func_0200f9f0(0x47), 2, 0xc8, 0x10, 2);
    }
    if (p->mUnk_3dd == 1) {
        func_0200b928(&q->mUnk_38, 0, 4);
        func_0200bb14(-q->mUnk_38, 0xa1, 0x102, 0xe, 0, 0, 0x5a, 1);
    }
    func_0201a3e0();
}

void func_0201a3d4(void) {
    func_020102f8();
    func_0201eb20();
}

void func_0201a3e0(void) {
    Unk_020c1030 *p = &data_020b6ee0.mUnk_a150;
    Unk_020c1454 *q = &data_020b6ee0.mUnk_a574;
    if (func_0200bd80(0x20000000, 0x101)) {
        p->mUnk_3dd = 1;
        p->mUnk_3d4 = 0x200;
        data_020b6ee0.mUnk_a538 = 1;
        func_0200fb50(1);
    }
    if (func_0200bd80(0x20000000, 0x102)) {
        if (p->mUnk_3dd == 0) {
            p->mUnk_3dd = 1;
            p->mUnk_3d4 = 0x200;
            q->mUnk_38 = 0x28;
        } else {
            func_0200fa80(10);
            func_0200fab0();
        }
    }
    if (data_020c18bc.mUnk_2ac_0 == 1) {
        if (func_0200bd80(0x80000000, 0x101) == 0 && func_0200bd80(0x80000000, 0x102) == 0) {
            data_020b6ee0.mUnk_a574.mUnk_34 = 0x10;
            data_020b6ee0.mUnk_a5d0 = 0;
            p->mUnk_3d8 = 0;
        }
    }
    if (data_020c18bc.mUnk_2ac_1 == 0 && data_020c18bc.mUnk_2ac_2 == 0) {
        data_020b6ee0.mUnk_a5d0 = 1;
    }
}

void func_0201a4b0(void) {
    func_0201f880(3);
    func_0201bb30();
    func_0201c41c();
    func_0201eb20();
    func_0202c1c4();
    func_02020854(0);
    func_0202bdd8(0x10cb, 0x10ca, 0x10cc, 0x82, 1);
    func_0202bdd8(0x10cb, 0x10ca, 0x10cc, 0x82, 2);
    func_0201dde0(0x237, 3);
    data_020c17fc = -8;
}

void func_0201a50c(void) {
    int sel;

    func_020215fc(0x7f, 0x3a, data_0201a61c, "Recap", 2, 0xf0, 0x10, 1);
    sel = func_02014538(data_020b6ee0.mUnk_a530, 0xf);
    if (sel != -1) {
        data_020c17fc += data_020c18bc.mUnk_45c % 3 / 2;
        if (data_020c17fc >= 0) {
            if (data_020c17fc <= 8) {
                func_0202cf08(0xfa7 - data_020c17fc, 0x7f, 0x62, 0x64, 1);
            } else {
                func_0202cf08(sel + 0x1089, 0x7f, 0x62, 0x64, 1);
            }
        }
        func_0202cf08(0x1088, 0x7f, 0x62, 0x66, 1);
    } else {
        func_0202cf08(0x1088, 0x7f, 0x62, 0x66, 1);
    }
    func_0200bb14(0xff, 0xa1, 0x101, 0x15, 0, 0, 0x17d, 1);
    if (func_0200bd80(0x20000000, 0x101)) {
        func_0200fb50(1);
    }
}

void func_0201a600(void) {
    func_020102f8();
}
