#include <nitro/os.h>
#include <Main/Unknown/functions.h>
#include <Main/Unknown/system_init.h>
#include <Main/Unknown/stage_progress.h>
#include <Main/Unknown/stage_select.h>
#include <Main/Unknown/prompt_screen.h>
#include <Main/Unknown/sprite_slots.h>
#include <Main/Unknown/save_data.h>
#include <Main/Unknown/sound_control.h>
#include <Main/Unknown/state_flow.h>
#include <Main/Unknown/globals.h>
#include <nitro/init.h>

void main(void) {
    GameFlow *flow = &data_020b1194;
    int i;

    data_020c18bc.mUnk_46c = 0;
    if (*(vu16 *) 0x027ffc40 != 2) {
        data_020c18bc.mUnk_2a7 = 0;
        func_0201b7e0(0xfa);
        func_0201eb20();
    }
    data_020c18bc.mUnk_46c |= 0x10820000;
    func_0201f880(3);
    func_02010d34();
    if (*(vu16 *) 0x027ffc40 != 2) {
        func_0202002c(0xfba, 0xd01, 2, 0x1c6, func_02010648, -1);
        func_02010c94();
        func_02020348();
    }
    data_020c18bc.mUnk_310 = 0x40;
    data_020c18bc.mUnk_493 = 1;
    data_020c18bc.mUnk_494 = 1;
    func_0202f00c();
    func_02010388(0, 0x80);
    func_0200fa24();
    func_0200d6c8();
    func_0200dacc();
    func_020143f4();
    func_0200e474();
    func_0202de60(&data_020b6ee0.mUnk_a5d4, data_0201abec[2]);
    data_020b6ee0.mUnk_a91b = 0;
    func_0200dbac();
    data_020b6ee0.mUnk_a91a = 1;
    func_0200fa80(1);
    func_0200fab0();

    while (1) {
        while (flow->mState != flow->mNextState || flow->mUnk_10 == 1) {
            flow->mUnk_10 = 0;
            if (data_0204e908[flow->mState] != NULL) {
                data_0204e908[flow->mState]();
            }
            if (flow->mNextState >= STATE_COUNT) {
                OS_Terminate();
            }
            flow->mPrevState = flow->mState;
            flow->mState = flow->mNextState;
            flow->mUnk_94 = 0;
            if (data_0204e840[flow->mState] != NULL) {
                data_0204e840[flow->mState]();
            }
        }
        if (data_0204e8a4[flow->mState] == NULL) {
            OS_Terminate();
        }
        data_0204e8a4[flow->mState]();
        if (data_0204e824[flow->mState] != 0 && (data_020c18bc.mUnk_12 & 8)) {
            func_0201004c();
        }
        func_020149e0();
        func_020145bc();
        func_0200d8bc();
        func_0200daf8();
        flow->mFrame++;
        func_0202f200();
        func_020103c0();
        func_0201bb30();
    }
}
