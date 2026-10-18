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
#include <Main/Unknown/screen_fade.h>
#include <Main/Unknown/game_math.h>
#include <Main/Unknown/file_stream.h>
#include <Main/Unknown/game_util.h>
#include <Main/Unknown/globals.h>
#include <Main/Unknown/prompt_screen.h>
#include <Main/Unknown/stage_camera.h>
#include <Main/Unknown/stage_select.h>
#include <Main/Unknown/state_flow.h>
#include <nitro/fx.h>
#include <nitro/wm.h>
#include <Main/Unknown/arm_system.h>
#include <Main/Unknown/system_init.h>

#define HW_DISPCNT 0x04000000
#define HW_VCOUNT 0x04000006
#define HW_BG3PA 0x04000030
#define HW_BG3PD 0x04000036
#define HW_BG3X 0x04000038
#define HW_BG3Y 0x0400003c
#define HW_WIN0H 0x04000040
#define HW_WIN1H 0x04000042
#define HW_WIN0V 0x04000044
#define HW_WIN1V 0x04000046
#define HW_KEYINPUT 0x04000130
#define HW_SWAP_BUFFERS 0x04000540
#define HW_SUB 0x1000
#define HW_SUB_OAM 0x07000400
#define HW_MAIN_RAM_END 0x027e0000
// Buttons the second processor shares: bit 15 is set while the console is closed.
#define HW_SHARED_BUTTONS 0x027fffa8
#define LID_CLOSED 0x8000
#define DISPCNT_FORCED_BLANK 0x80
#define DISPCNT_WINDOWS 0x6000

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
    data_020c317c.mUnk_00[0xf] = 0;
    data_020c18bc.mUnk_45c = 0;
    data_020c18bc.mUnk_490[0] = data_020c18bc.mUnk_490[1] = 3;
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

void func_0201bb40(int flags) {
    u32 frame = data_020c18bc.mUnk_45c;
    TPData tp2;
    TPData tp;
    u16 wmStatus[0x3e8];
    int engine;
    int i;
    int single;
    if (data_020c18bc.mUnk_2c8 & 0x400000) {
        if (func_02035058() == 0) {
            func_0202e8e4();
        }
    }
    data_020c18bc.mUnk_488 = *(vu16 *) HW_VCOUNT;
    data_020c18bc.mUnk_48c = data_020c18bc.mUnk_48d;
    data_020c18bc.mUnk_48d = -1;
    data_020c18bc.mUnk_48f += data_020c18bc.mUnk_48c;
    if (data_020c18bc.mUnk_45c >= data_020c18bc.mUnk_460 + 0x3c) {
        data_020c18bc.mUnk_48e = 0x3c - data_020c18bc.mUnk_48f;
        data_020c18bc.mUnk_48f = 0;
        data_020c18bc.mUnk_460 = data_020c18bc.mUnk_45c;
    }
    if (data_020c18bc.mUnk_46c & 0x20000000) {
        u16 line;
        *(vu32 *) HW_DISPCNT |= DISPCNT_FORCED_BLANK;
        line = data_020c18bc.mUnk_488;
        while (*(vu16 *) HW_VCOUNT == line) {
        }
        *(vu32 *) HW_DISPCNT &= ~DISPCNT_FORCED_BLANK;
    }
    data_020c5e44.mUnk_6c = 0;
    func_0201fda4();
    single = flags & 1;
    if (single == 0 && (data_020c18bc.mUnk_46c & 0x20000)) {
        if (data_020c18bc.mUnk_490[0] != 2 && data_020c18bc.mUnk_490[1] != 2) {
            func_0201f7f8(3);
        }
    }
    if ((data_020c18bc.mUnk_46c & 0x10000080) == 0x10000080) {
        if (func_02035058() == 0) {
            func_020248fc();
            if (data_020c6144.mUnk_4a == 0 && !(data_020c18bc.mUnk_46c & 0x10000)) {
                *(vu32 *) HW_SWAP_BUFFERS = 3;
            } else {
                *(vu32 *) HW_SWAP_BUFFERS = 1;
            }
        }
    }
    if (single == 0 && (data_020c18bc.mUnk_46c & 0x2000)) {
        func_0202ef24();
    }
    func_0202ad4c();
    func_0201c7b8(1);
    func_0201c7b8(2);
    func_0201c6d4();
    if (((*(vu16 *) HW_SHARED_BUTTONS & LID_CLOSED) >> 15) == 1) {
        if (func_0202fcb4() == 1) {
            func_02020ba0();
            flags |= 2;
        }
    }
    if ((flags & 1) == 0) {
        while (data_020c18bc.mUnk_45c == frame) {
            OS_Halt();
        }
    }
    for (engine = 0; engine < 2; engine++) {
        if (data_020c267c.mUnk_18[engine].mUnk_57c != 0) {
            data_020c267c.mUnk_18[engine].mUnk_57c = 0;
            func_0202bb30(engine == 0 ? 1 : 2);
        }
    }
    if (data_020c18bc.mUnk_4a0 == 1) {
        func_02042dc8();
        func_02042ea8(&tp2);
        if (tp2.touch == 0) {
            data_020c18bc.mUnk_4a0 = 0;
        }
        func_02020ba0();
    } else if ((flags & 3) == 0 && (data_020c18bc.mUnk_46c & 0x400000)) {
        func_02042dc8();
        func_02042ea8(&tp);
        if (tp.touch == 0 || tp.validity == 0) {
            if (data_020c18bc.mUnk_2b0 == 0 || !data_020c18bc.mUnk_2ac_5) {
                data_020c18bc.mUnk_2ad = tp.touch;
            } else {
                data_020c18bc.mUnk_2b0--;
                data_020c18bc.mUnk_2ad = 0;
            }
            data_020c18bc.mUnk_2ac_4 |= data_020c18bc.mUnk_2ad ^ 1;
            data_020c18bc.mUnk_2ad = data_020c18bc.mUnk_2ad & data_020c18bc.mUnk_2ac_4;
            data_020c18bc.mUnk_2ac_0 = (data_020c18bc.mUnk_2ac_1 ^ data_020c18bc.mUnk_2ad) & data_020c18bc.mUnk_2ad;
            data_020c18bc.mUnk_2ac_2 = data_020c18bc.mUnk_2ac_1 & (data_020c18bc.mUnk_2ac_1 ^ data_020c18bc.mUnk_2ad);
            data_020c18bc.mUnk_2ac_1 = data_020c18bc.mUnk_2ad;
            if (data_020c18bc.mUnk_2ac_2 == 1) {
                data_020c18bc.mUnk_2b0 = 8;
                if (data_020c2660.mUnk_10 < 10) {
                    if (func_0201fae4(data_020c18bc.mUnk_2b1 - data_020c18bc.mUnk_2b3,
                                      data_020c18bc.mUnk_2b2 - data_020c18bc.mUnk_2b4) < 0x19) {
                        data_020c18bc.mUnk_2ac_3 = 1;
                    }
                }
            }
            if (data_020c18bc.mUnk_2ac_0 == 1) {
                data_020c18bc.mUnk_2b1 = data_020c18bc.mUnk_2b2 = 0;
                data_020c18bc.mUnk_2b3 = data_020c18bc.mUnk_2b4 = 0xff;
            }
            if (tp.touch == 1) {
                switch (tp.validity) {
                    case 0:
                        data_020c18bc.mUnk_2ae = tp.x;
                        data_020c18bc.mUnk_2af = tp.y;
                        break;
                    case 1:
                        data_020c18bc.mUnk_2af = tp.y;
                        break;
                    case 2:
                        data_020c18bc.mUnk_2ae = tp.x;
                        break;
                    case 3:
                        break;
                }
                data_020c18bc.mUnk_2ac_3 = 0;
                if (data_020c18bc.mUnk_2ae > data_020c18bc.mUnk_2b1) {
                    data_020c18bc.mUnk_2b1 = data_020c18bc.mUnk_2ae;
                }
                if (data_020c18bc.mUnk_2ae < data_020c18bc.mUnk_2b3) {
                    data_020c18bc.mUnk_2b3 = data_020c18bc.mUnk_2ae;
                }
                if (data_020c18bc.mUnk_2af > data_020c18bc.mUnk_2b2) {
                    data_020c18bc.mUnk_2b2 = data_020c18bc.mUnk_2af;
                }
                if (data_020c18bc.mUnk_2af < data_020c18bc.mUnk_2b4) {
                    data_020c18bc.mUnk_2b4 = data_020c18bc.mUnk_2af;
                }
            }
        }
        if (data_020c18bc.mUnk_46c & 0x800000) {
            data_020c2660.mUnk_04 = func_02020bac(0xffff);
        }
        func_02020ba0();
    }
    if (data_020c18bc.mUnk_2a3 == 0) {
        if (data_020c18bc.mUnk_46c & 0x100) {
            data_020c18bc.mUnk_46c &= ~0x100;
            if ((data_020c18bc.mUnk_46c & 0x600) || data_020c18bc.mUnk_2a4[1] > 1) {
                if (data_020c18bc.mUnk_46c & 0x200) {
                    func_02039ba8();
                    func_02034a44();
                    data_020c18bc.mUnk_2a4[1] = -2;
                } else {
                    func_02039ba8();
                }
            }
        }
        if (data_020c18bc.mUnk_2a4[1] > 0) {
            if (func_020465b0(wmStatus) == 0) {
                u16 state = wmStatus[0];
                if (state != 7 && state != 9 && state != 10 && state != 5) {
                    flags |= 4;
                }
            }
        }
        data_020c18bc.mUnk_456 -= (data_020c18bc.mUnk_456 != 0) ? 1 : 0;
        if (data_020c18bc.mUnk_46c & 0x20) {
            func_0202fbc8();
        } else if ((flags & 4) == 0 && ((*(vu16 *) HW_SHARED_BUTTONS & LID_CLOSED) >> 15) == 1 &&
                   data_020c18bc.mUnk_456 == 0) {
            if (data_020c18bc.mUnk_46c & 0x200) {
                data_020c18bc.mUnk_46c |= 0x100;
            } else if (data_020c18bc.mUnk_2a6_1) {
                func_02039b70();
                func_02001120();
            } else if (data_020c18bc.mUnk_2a6_0) {
                func_02039b70();
                func_02039ba8();
            }
            func_0204028c(0);
            while (SND_RecvCommandReply(0) != 0) {
            }
            SND_FlushCommand(2);
            if ((data_020c18bc.mUnk_46c & 0x200) == 0 || data_020c18bc.mUnk_2a4[1] == -2) {
                u32 save = data_020c18bc.mUnk_470 & 4;
                int found = 0;
                func_0202ad34();
                func_0202ad54();
                if (data_020c660c.mUnk_04 == 1 && data_020c660c.mUnk_08 == 0) {
                    found = 1;
                    func_0202f2e8(data_020c660c.mUnk_0c);
                }
                func_02043e88(0xc, 0, 0);
                if (found == 1) {
                    func_0202f290(data_020c660c.mUnk_0c, data_020c660c.mUnk_0c->mUnk_60,
                                  data_020c660c.mUnk_0c->mUnk_4c);
                }
                func_0202ad38();
                if (save & 4) {
                    func_0202ad50();
                }
            }
        }
        if (data_020c18bc.mUnk_46c & 0x200) {
            data_020c9e04 &= ~0x800;
            func_02034c38();
            func_02039d24();
        } else if (data_020c18bc.mUnk_46c & 0x400) {
            data_020c9e04 &= ~0x800;
            func_02039d24();
            func_02001170();
        } else if (data_020c18bc.mUnk_2a4[1] > 1) {
            int offset;
            func_02039d24();
            data_020c18bc.mUnk_2a8 = func_02047264(&data_020c9e60, data_020c9e10, data_020c9e14);
            data_020c9e04 &= ~0x800;
            data_020c18bc.mUnk_288 = 0;
            i = 0;
            offset = 0;
            for (; i < 2; i++) {
                u8 *entry = func_02047ac4(&data_020c9e60, data_020c9e14, (u16) i);
                if (entry != 0) {
                    MIi_CpuCopy16(entry, data_020c9e1c + offset, 0x54);
                    data_020c18bc.mUnk_288 |= 1 << i;
                }
                offset += 0x54;
            }
            func_02034a08();
        }
    } else {
        data_020c18bc.mUnk_2a3--;
    }
    single = flags & 1;
    if (single == 0) {
        func_02039a94();
    }
    if (single == 0) {
        func_0201f648();
        func_0202d908(1);
        func_0202d908(2);
        if (func_02035058() == 0 && (flags & 2) == 0) {
            func_0201c32c();
        }
        if (data_020c18bc.mUnk_46c & 0x2000) {
            func_0202efd4();
        }
        if (data_020c18bc.mUnk_46c & 0x1000000) {
            func_020360f4();
            func_02036194();
        }
        func_0202ad3c();
        if (data_020c18bc.mUnk_492 != 0) {
            data_020c18bc.mUnk_492--;
            if (data_020c18bc.mUnk_495 == 0) {
                u32 length = (0xb4 - data_020c18bc.mUnk_492) * 2;
                u32 tries;
                if (length >= data_020c18bc.mUnk_498) {
                    length = data_020c18bc.mUnk_498;
                }
                func_0204028c(length);
                tries = 0;
                for (; SND_RecvCommandReply(0) != 0 && tries < 10; tries++) {
                }
                SND_FlushCommand(2);
                return;
            }
            if (data_020c18bc.mUnk_492 == 0) {
                data_020c18bc.mUnk_495 = 0;
            }
        }
    }
}

void func_0201c1d0(void) {
    data_020c18bc.mUnk_492 = 0xb4;
    data_020c18bc.mUnk_495 = 0;
}

void func_0201c1e0(void) {
    if (data_020c18bc.mUnk_468 != 0) {
        data_020c18bc.mUnk_468();
    }
    if ((data_020c18bc.mUnk_450 & 2) && data_020c18bc.mUnk_454 != 0xffff) {
        int angle = (data_020c18bc.mUnk_45c & 0xff) << 8;
        if (data_020c18bc.mUnk_450 & 4) {
            data_020c18bc.mUnk_450 &= ~6;
        } else {
            int index = (angle >> 4) * 2;
            vu16 *oam = (vu16 *) HW_SUB_OAM;
            oam[3] = FX_SinCosTable_[index + 1] >> 4;
            oam[15] = oam[3];
            oam[7] = FX_SinCosTable_[index] >> 4;
            oam[11] = -oam[7];
        }
    }
    if (!(data_020c18bc.mUnk_46c & 0x100000)) {
        *(vu32 *) HW_BG3X = data_020c18bc.mUnk_2d4;
        *(vu32 *) HW_BG3Y = data_020c18bc.mUnk_2d8;
        *(vu32 *) (HW_BG3X + HW_SUB) = data_020c18bc.mUnk_2dc;
        *(vu32 *) (HW_BG3Y + HW_SUB) = data_020c18bc.mUnk_2e0;
    }
    if (!(data_020c18bc.mUnk_46c & 0x40000000)) {
        *(vu16 *) HW_BG3PA = data_020c18bc.mUnk_2e4;
        *(vu16 *) HW_BG3PD = data_020c18bc.mUnk_2e6;
    }
    if (!(data_020c18bc.mUnk_46c & 0x200000)) {
        *(vu16 *) HW_WIN0H = data_020c18bc.mUnk_2e8;
        *(vu16 *) HW_WIN0V = data_020c18bc.mUnk_2ea;
        *(vu16 *) HW_WIN1H = data_020c18bc.mUnk_2ec;
        *(vu16 *) HW_WIN1V = data_020c18bc.mUnk_2ee;
        *(vu16 *) HW_DISPCNT = (*(vu16 *) HW_DISPCNT & ~DISPCNT_WINDOWS) | (data_020c18bc.mUnk_2f0 << 13);
        *(vu16 *) (HW_WIN0H + HW_SUB) = data_020c18bc.mUnk_2f2;
        *(vu16 *) (HW_WIN0V + HW_SUB) = data_020c18bc.mUnk_2f4;
        *(vu16 *) (HW_WIN1H + HW_SUB) = data_020c18bc.mUnk_2f6;
        *(vu16 *) (HW_WIN1V + HW_SUB) = data_020c18bc.mUnk_2f8;
        *(vu16 *) (HW_DISPCNT + HW_SUB) =
            (*(vu16 *) (HW_DISPCNT + HW_SUB) & ~DISPCNT_WINDOWS) | (data_020c18bc.mUnk_2fa << 13);
    }
    data_020c18bc.mUnk_45c++;
    data_020c18bc.mUnk_48d++;
    {
        vu32 *shared = (vu32 *) HW_MAIN_RAM_END;
        shared[0xffe] |= 1;
    }
}

void func_0201c32c(void) {
    u16 keys = (((*(vu16 *) HW_KEYINPUT) | (*(vu16 *) HW_SHARED_BUTTONS)) ^ 0x2fff) & 0x2fff;
    u16 held;
    s32 prev;
    int i = 0;
    u16 repeat = 0;
    u16 last;
    held = keys;
    data_020c18bc.mUnk_016 |= 0xffff ^ keys;
    held &= data_020c18bc.mUnk_016;
    prev = data_020c18bc.mUnk_10;
    data_020c18bc.mUnk_014 = prev & (held ^ prev);
    data_020c18bc.mUnk_12 = held & (held ^ prev);
    data_020c18bc.mUnk_10 = held;
    for (; held != 0; held >>= 1) {
        if (held & 1) {
            if (data_020c18bc.mUnk_004[i] != 0) {
                if (data_020c18bc.mUnk_004[i] < 0x10 || (data_020c18bc.mUnk_018 & (1 << i))) {
                    repeat = repeat | 1 << i;
                }
            }
            data_020c18bc.mUnk_004[i] = 0x10;
        } else if (data_020c18bc.mUnk_004[i] != 0) {
            data_020c18bc.mUnk_004[i]--;
        }
        ++i;
    }
    for (; i < 12; i++) {
        if (data_020c18bc.mUnk_004[i] != 0) {
            data_020c18bc.mUnk_004[i]--;
        }
    }
    last = data_020c18bc.mUnk_018;
    data_020c18bc.mUnk_01c = last & (repeat ^ last);
    data_020c18bc.mUnk_01a = repeat & (repeat ^ last);
    data_020c18bc.mUnk_018 = repeat;
    if (data_020c18bc.mUnk_12 & 0xf00) {
        data_020c18bc.mUnk_02e = (1 + data_020c18bc.mUnk_02e) & 0xf;
        data_020c18bc.mUnk_01e[data_020c18bc.mUnk_02e] = (data_020c18bc.mUnk_12 >> 8) & 0xf;
    }
}

void func_0201c41c(void) {
    data_020c18bc.mUnk_016 = 0;
    data_020c18bc.mUnk_014 = 0;
    data_020c18bc.mUnk_12 = data_020c18bc.mUnk_10 = data_020c18bc.mUnk_014;
}

void *func_0201c430(u32 size) {
    return func_0201c43c(size, 0);
}

void *func_0201c43c(u32 size, u32 flags) {
    int first = 1;
    int big;
    void *p;
    size = ((((size >> 1) | size) & 1) + (size >> 2)) << 2;
    if ((flags & 2) == 0 && ((flags & 4) != 0 || size < 0x8000) && func_0203ed14(0, data_020c18bc.mUnk_038) >= size) {
        OS_SetCurrentHeap(0, data_020c18bc.mUnk_038);
        big = 1;
    } else {
        OS_SetCurrentHeap(0, data_020c18bc.mUnk_034);
        big = 0;
    }
    if (size != 0) {
        p = OS_AllocFromHeap(0, -1, size);
        while (p == 0) {
            if (big == 1) {
                OS_SetCurrentHeap(0, data_020c18bc.mUnk_034);
                big = 0;
            } else {
                if (first == 1) {
                    first = 0;
                }
                func_0202e494();
            }
            p = OS_AllocFromHeap(0, -1, size);
        }
        if ((flags & 1) == 0) {
            MIi_CpuClearFast(0, p, size);
        }
    } else {
        p = 0;
    }
    if (p == 0) {
        OS_Terminate();
    }
    return p;
}

int func_0201c4f0(void *handle) {
    void **pp = handle;
    u32 *block;
    if (pp != 0 && (block = *pp) != 0) {
        if ((u8 *) block > data_020c18bc.mUnk_030) {
            OS_SetCurrentHeap(0, data_020c18bc.mUnk_038);
        } else {
            OS_SetCurrentHeap(0, data_020c18bc.mUnk_034);
        }
        OS_FreeToHeap(0, -1, block);
        *block = 0x43495245;
        *pp = 0;
        return 1;
    }
    return 0;
}

u16 func_0201c538(void) {
    return data_020c321c.mUnk_18;
}

u16 func_0201c544(void) {
    return data_020c321c.mUnk_1a;
}

u32 func_0201c550(void) {
    return func_0201c538() + (func_0202ade0() << 7);
}

u32 func_0201c564(void) {
    return func_0201c544() + ((func_0202e760(data_020c317c.mUnk_24) >> 5) << 2);
}
