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
#include <Main/Unknown/file_stream.h>
#include <Main/Unknown/game_util.h>
#include <Main/Unknown/globals.h>
#include <Main/Unknown/prompt_screen.h>
#include <Main/Unknown/stage_camera.h>
#include <Main/Unknown/stage_select.h>
#include <Main/Unknown/state_flow.h>
#include <Main/Unknown/system_init.h>

void func_0201b7e0(u32 a) {
    func_0201b7ec(a, 0, 0);
}

void func_0201b7ec(u32 a, int b, int c) {
    u32 size;
    TPCalibrateParam tp;
    int heapEnd;
    u16 mode;
    void *buf;

    (void) REG_IME;
    REG_IME = 1;
    data_020c18bc.mUnk_2c8 = 0;
    data_020c18bc.mUnk_470 = 0;
    data_020c5e44.mUnk_74 = 0;
    data_020c18bc.mUnk_47e = a;
    data_020c18bc.mUnk_30e = 0x20;
    data_020c18bc.mUnk_310 = 10;
    data_020c18bc.mUnk_311[0] = 9;
    data_020c18bc.mUnk_311[1] = 12;
    data_020c18bc.mUnk_308 = 0x80;
    data_020c18bc.mUnk_30a[0] = 0;
    data_020c18bc.mUnk_30a[1] = 0x1f;
    data_020c18bc.mUnk_468 = 0;
    OS_Init();
    OS_InitTick();
    RTC_Init();
    OS_InitAlarm();
    data_020c1818.mUnk_04 = OS_GetArenaLo(0);
    OS_SetArenaLo(0, OS_InitAlloc(0, data_020c1818.mUnk_04, OS_GetArenaHi(0), 2));
    if (func_0202ad40() == 1) {
        heapEnd = 0x100000;
    } else {
        heapEnd = 0x96000;
    }
    data_020c18bc.mUnk_030 = (u8 *) OS_GetArenaHi(0) - heapEnd;
    data_020c18bc.mUnk_034 = OS_CreateHeap(0, OS_GetArenaLo(0), data_020c18bc.mUnk_030);
    data_020c18bc.mUnk_038 = OS_CreateHeap(0, data_020c18bc.mUnk_030 + 1, OS_GetArenaHi(0));
    OS_SetCurrentHeap(0, data_020c18bc.mUnk_034);
    if ((data_020c18bc.mUnk_46c & 0x200) == 0) {
        func_020421d0(-1);
        func_020421f8(&data_020c1874);
        func_02042490(&data_020c1874, "bank.bin");
        func_0201d9a8(4);
        func_0201d9d4(&size, 4);
        func_0201d9a8(0);
        data_020c18bc.mUnk_334 = size;
        buf = func_0201c430(size);
        data_020c18bc.mUnk_000 = buf;
        func_0201d9d4(buf, size);
    }
    func_0202de94();
    data_020c18bc.mUnk_03c = func_0201c430(20 * data_020c18bc.mUnk_47e);
    data_020c18bc.mUnk_480 = 0;
    data_020c18bc.mUnk_484 = 0;
    data_020c18bc.mUnk_47c = 200;
    data_020c18bc.mUnk_464 = 0;
    MIi_CpuClearFast(0xc0, (void *) 0x07000000, 0x400);
    MIi_CpuClearFast(0, (void *) 0x05000000, 0x400);
    MIi_CpuClearFast(0xc0, (void *) 0x07000400, 0x400);
    MIi_CpuClearFast(0, (void *) 0x05000400, 0x400);
    data_020c2bfc[0xf] = 0;
    data_020c317c[0xf] = 0;
    data_020c18bc.mUnk_45c = 0;
    data_020c18bc.mUnk_490 = data_020c18bc.mUnk_491 = 3;
    *(vu16 *) 0x0400106c = *(vu16 *) 0x0400006c = 0;
    func_0201c41c();
    (void) REG_IME;
    REG_IME = 0;
    func_0203d338();
    OS_SetIrqFunction(1, func_0201c1e0);
    OS_EnableIrqMask(1);
    OS_EnableIrqMask(0x40000);
    (void) REG_IME;
    REG_IME = 1;
    GX_VBlankIntr(1);
    func_0201eb20();
    TP_Init();
    if (TP_GetUserInfo(&tp) == 1) {
        TP_SetCalibrateParam(&tp);
    }
    data_020c18bc.mUnk_2ac_4 = data_020c18bc.mUnk_2ac_1 = data_020c18bc.mUnk_2ac_2 = data_020c18bc.mUnk_2ac_0 = 0;
    data_020c18bc.mUnk_2b0 = 0;
    data_020c18bc.mUnk_2ac_5 = 1;
    data_020c18bc.mUnk_4a0 = 1;
    data_020c18bc.mUnk_46c |= 0x400000;
    func_02020e18(0, 0);
    if (c != 0) {
        func_0202ad48(c);
    }
    if (func_0202ad40() == 1) {
        mode = 0x1080;
    } else {
        mode = 0xb00;
    }
    data_020c18bc.mUnk_48a = mode;
    data_020c18bc.mUnk_42c = -1;
    data_020c18bc.mUnk_340[2] = 0x1800;
    data_020c18bc.mUnk_340[0] = 0x100;
    data_020c18bc.mUnk_340[1] = 0x19;
    data_020c18bc.mUnk_340[3] = 0x1800;
    func_02043828();
    func_020443ac(1);
    data_020c1818.mUnk_10 = func_0201c1d0;
    data_020c1818.mUnk_14 = 0;
    func_0204445c((PMCallbackInfo *) &data_020c1818.mUnk_10);
    func_020402f0();
    *(vu16 *) 0x04000304 &= ~0xc;
    data_020c18bc.mUnk_24c = func_02034a60;
    data_020c18bc.mUnk_280 = 0;
    data_020c18bc.mUnk_2a4[0] = 0;
    data_020c18bc.mUnk_240 = 0;
    data_020c18bc.mUnk_230 = 0;
    data_020c18bc.mUnk_234 = 0;
    data_020c18bc.mUnk_238 = 0;
    data_020c18bc.mUnk_23c = 0;
    data_020c18bc.mUnk_244 = 0;
    data_020c18bc.mUnk_248 = 0;
    data_020c18bc.mUnk_2a3 = 0;
    data_020c18bc.mUnk_450 = 0;
    data_020c18bc.mUnk_458[0] = 0;
    data_020c18bc.mUnk_454 = 0xffff;
    data_020c18bc.mUnk_498 = 0x7f;
}

void func_0201bb30(void) {
    func_0201bb40(0);
    func_02034be4();
}
