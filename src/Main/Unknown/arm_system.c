#include <string.h>
#include <Main/Unknown/functions.h>
#include <Main/Unknown/globals.h>
#include <Main/Unknown/arm_system.h>

#pragma thumb off

void func_02001000(void) {
    func_0201c4f0(data_020c66dc);
    func_0201c4f0(data_020c1b30);
    memset(data_020c1b0c, 0, 0x2c);
    data_020c18bc.mUnk_2a2 = 0;
    data_020c18bc.mUnk_2a8 = 0;
    data_020c1abc.mUnk_96 = 0;
    data_020c1abc.mUnk_92 = 0;
    memset(data_020c67e0, 0, 0xfe);
    memset(data_020c68e0, 0, 0x1fc);
    memset(data_020c66e0, 0, 0x40);
    memset(data_020c6720, 0, 0xc0);
    data_020c66d4.mUnk_4 = 0;
}

BOOL func_020010b4(u32 timeout) {
    u32 start = data_020c18bc.mUnk_45c;
    u32 last = data_020c18bc.mUnk_45c;

    if (func_020350f8() != 0) {
        do {
            if (data_020c18bc.mUnk_45c != last) {
                func_0201bb40(3);
                last = data_020c18bc.mUnk_45c;
            }
            if (data_020c18bc.mUnk_45c - start > timeout) {
                return FALSE;
            }
        } while (func_020350f8() != 0);
    }
    return TRUE;
}

void func_02001120(void) {
    data_020c18bc.mUnk_2a6 &= ~2;
    data_020c18bc.mUnk_2a3 = 0x10;
    if (func_020350f8() != 5 && func_020350f8() != 0) {
        func_020350a0();
        func_020010b4(0x3c);
    }
    func_02001000();
    func_02039ba8();
}

void func_02001170(void) {
    switch (func_0203946c()) {
        case 1:
            func_0203947c(4, data_020c1abc.mUnk_94 + 1, data_020c66d4.mUnk_0);
            break;
        case 4:
        case 5:
        case 6:
            func_02034d1c();
            func_02034954();
            break;
    }
}

#pragma thumb on
