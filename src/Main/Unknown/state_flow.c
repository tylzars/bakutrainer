#include <nitro/os.h>
#include <Main/Unknown/state_flow.h>
#include <Main/Unknown/globals.h>
#include <Main/Unknown/functions.h>

u16 func_0200f9c8(int index) {
    u16 table[7] = {587, 590, 593, 595, 597, 599, 601};
    return table[index];
}

const char *func_0200f9f0(int id) {
    if (id != 0) {
        return func_02021e18(func_0200f9c8(data_020b1078.mLanguage), id);
    }
    return 0;
}

const char *func_0200fa10(int id, int index) {
    return func_02021e18(func_0200f9c8(index), id);
}

void func_0200fa24(void) {
    func_0200fa3c();
    data_020b1194.mState = 0;
    data_020b1194.mPrevState = 0;
    data_020b1194.mUnk_10 = 0;
}

void func_0200fa3c(void) {
    data_020b1194.mQueueCount = 0;
    data_020b1214.mUnk_14 = 0;
}

void func_0200fa50(void) {
    GameFlow *flow = &data_020b1194;
    StateFunc f;

    f = data_0204e908[flow->mState];
    if (f != NULL) {
        f();
    }
    f = data_0204e840[flow->mState];
    if (f != NULL) {
        f();
    }
}

void func_0200fa80(int state) {
    GameFlow *flow = &data_020b1194;
    u32 i;

    if (flow->mQueueCount >= 25) {
        OS_Terminate();
    }
    for (i = flow->mQueueCount; i != 0; i--) {
        flow->mQueue[i] = flow->mQueue[i - 1];
    }
    flow->mQueue[0] = state;
    flow->mQueueCount++;
}

void func_0200fab0(void) {
    func_0200fabc(0);
}

void func_0200fabc(u32 flag) {
    GameFlow *flow = &data_020b1194;
    u32 i;

    if (flow->mQueueCount == 0) {
        OS_Terminate();
    }
    flow->mNextState = flow->mQueue[0];
    flow->mUnk_94 = 0;
    if (flow->mQueueCount > 1) {
        for (i = 0; i < flow->mQueueCount - 1; i++) {
            flow->mQueue[i] = flow->mQueue[i + 1];
        }
    }
    flow->mQueueCount--;
    flow->mUnk_10 = flag;
}

u32 func_0200fb00(void) {
    return data_020b1194.mState;
}

u32 func_0200fb0c(void) {
    return data_020b1194.mNextState;
}

void func_0200fb18(u32 index, u32 value) {
    GameFlow *flow = &data_020b1194;

    if (index >= HANDLER_COUNT) {
        OS_Terminate();
    }
    flow->mHandlers[index] = value;
}

u32 func_0200fb34(u32 index) {
    GameFlow *flow = &data_020b1194;

    if (index >= HANDLER_COUNT) {
        OS_Terminate();
    }
    return flow->mHandlers[index];
}

void func_0200fb50(s32 value) {
    data_020b1214.mUnk_14 = value;
}

s32 func_0200fb5c(void) {
    return data_020b1214.mUnk_14;
}
