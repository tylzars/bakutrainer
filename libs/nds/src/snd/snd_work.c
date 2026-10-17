#include <nitro/snd.h>
#include <nitro/os.h>

SNDSharedWork *SNDi_SharedWork;

u16 func_02040a90(void) {
    DC_InvalidateRange(&SNDi_SharedWork->mUnk_008, 2);
    return SNDi_SharedWork->mUnk_008;
}

u32 func_02040abc(void) {
    DC_InvalidateRange(SNDi_SharedWork, 4);
    return SNDi_SharedWork->mUnk_000;
}

void func_02040ae4(SNDSharedWork *work) {
    int i;
    int j;
    work->mUnk_004 = 0;
    work->mUnk_008 = 0;
    work->mUnk_00a = 0;
    work->mUnk_000 = 0;
    for (i = 0; i < 16; i++) {
        work->mUnk_020[i].mUnk_20 = 0;
        for (j = 0; j < 16; j++) {
            work->mUnk_020[i].mUnk_00[j] = -1;
        }
    }
    for (i = 0; i < 16; i++) {
        work->mUnk_260[i] = -1;
    }
    DC_FlushRange(work, sizeof(SNDSharedWork));
}
