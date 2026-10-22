#include <Main/clib.h>
#include <Main/System/system_init.h>
#include <Main/Unknown/arm_system.h>
#include <Main/Link/comm_link.h>
#include <Main/Link/wireless_helper.h>
#include <Main/Link/link_indicator.h>

StateC data_020c66d4[1];
u32 data_020c66e0[16] __attribute__((aligned(32)));
u8 data_020c6720[0xc0] __attribute__((aligned(32)));
u8 data_020c67e0[0x100] __attribute__((aligned(32)));
u8 data_020c68e0[0x1fc] __attribute__((aligned(32)));

#pragma thumb off

void func_02001000(void) {
    Free(&data_020c66d4->mUnk_8);
    Free(&gSystemState.mUnk_250.mUnk_24);
    memset(&gSystemState.mUnk_250, 0, 0x2c);
    gSystemState.mUnk_2a2 = 0;
    gSystemState.mUnk_2a8 = 0;
    gSystemState.mUnk_296 = 0;
    gSystemState.mUnk_292 = 0;
    memset(data_020c67e0, 0, 0xfe);
    memset(data_020c68e0, 0, 0x1fc);
    memset(data_020c66e0, 0, 0x40);
    memset(data_020c6720, 0, 0xc0);
    data_020c66d4->mUnk_4 = 0;
}

BOOL func_020010b4(u32 timeout) {
    u32 start = gSystemState.mElapsedFrames;
    u32 last = gSystemState.mElapsedFrames;

    if (func_020350f8() != 0) {
        do {
            if (gSystemState.mElapsedFrames != last) {
                UpdateSystem(3);
                last = gSystemState.mElapsedFrames;
            }
            if (gSystemState.mElapsedFrames - start > timeout) {
                return FALSE;
            }
        } while (func_020350f8() != 0);
    }
    return TRUE;
}

void func_02001120(void) {
    gSystemState.mUnk_2a6_1 = 0;
    gSystemState.mUnk_2a3 = 0x10;
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
            func_0203947c(4, gSystemState.mUnk_294 + 1, data_020c66d4->mUnk_0);
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
