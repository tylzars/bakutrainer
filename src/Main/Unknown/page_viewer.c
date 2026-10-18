#include <Main/Unknown/functions.h>
#include <Main/Unknown/globals.h>
#include <Main/Unknown/page_viewer.h>

PageViewState data_02098d74;

void func_02006fc0(void) {
    func_0201bb30();
    func_0201f880(3);
    func_0201c41c();
    func_0201eb20();
    func_0202c1c4();
    func_02020854(1);
    func_0201dde0(0xdaa, 3);
    func_0202bdd8(0x10cb, 0x10ca, 0x10cc, 2, 1);
    func_0202bdd8(0x10cb, 0x10ca, 0x10cc, 2, 2);
    func_020104c0(1, 0x80);
    data_02098d74.mUnk_04 = 0;
    func_02007168(1);
    if (data_020c1020.mUnk_0c == 1) {
        func_02014430(0);
        data_020c1020.mUnk_0c = 0;
    }
}

void func_02007040(void) {
    PageViewState *p = &data_02098d74;
    switch (data_02098d74.mState) {
        case 0:
            func_0200bb14(0x80, 0x60, 0x600, 0xf8e, 0, 0, 0, 1);
            break;
        case 1:
            func_0200bb14(0, 0xa1, 0x600, 14, 0, 0, 90, 1);
            if (p->mUnk_08 == 0) {
                func_0200bb14(0xff, 0xa1, 0x601, 21, 0, 0, 0x17d, 1);
            }
            break;
    }
}

void func_020070c4(void) {
    PageViewState *p = &data_02098d74;
    func_02007040();
    switch (data_02098d74.mState) {
        case 0:
            if (func_0200bd80(0x20000000, 0x600) != 0) {
                p->mUnk_04 = 0;
                func_02007168(1);
            }
            break;
        case 1:
            if (func_0200bd80(0x20000000, 0x600) != 0) {
                if (data_020c17e0.mUnk_1b != 0) {
                    data_020c17e0.mUnk_1b = 0;
                    func_0200fa80(10);
                    func_0200fab0();
                } else {
                    func_0200fa80(1);
                    func_0200fab0();
                }
            }
            if (p->mUnk_08 == 0) {
                if (func_0200bd80(0x20000000, 0x601) != 0) {
                    p->mUnk_04++;
                    if (p->mUnk_04 == 11) {
                        p->mUnk_04 = 0;
                    }
                    func_02007168(1);
                }
            } else {
                p->mUnk_08--;
            }
            break;
    }
}

void func_02007168(int state) {
    PageViewState *p = &data_02098d74;
    func_0201f880(3);
    switch (state) {
        case 0:
            break;
        case 1:
            func_0201ddec(p->mUnk_04 + 0x1e9, 3, 0, 2);
            p->mUnk_08 = 120;
            break;
    }
    p->mState = state;
}

void func_0200719c(void) {
    func_0201f880(3);
}
