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
#include <Main/Unknown/random_angle.h>
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
#include <Main/clib.h>
#include <Main/Unknown/system_init.h>

#define EZ10_MAGIC 0x30315a45
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
    data_020c267c.mUnk_018[0].mUnk_568[0xf] = 0;
    data_020c267c.mUnk_018[1].mUnk_568[0xf] = 0;
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
        if (data_020c267c.mUnk_018[engine].mUnk_57c != 0) {
            data_020c267c.mUnk_018[engine].mUnk_57c = 0;
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
    return func_0201c544() + ((func_0202e760(data_020c267c.mUnk_b24) >> 5) << 2);
}

#pragma thumb off
// Queues a transfer for func_0201c7b8: grows the queue's byte count, waits for the queue to drain when it is full
// (terminating when it stays full), and appends the entry.
void func_0201c580(u16 file, u32 src, u32 dst, u32 size, u32 unpackedSize, u8 mode, u8 flag) {
    Unk_LoadEntry entry;
    int limit;
    if (unpackedSize == 0) {
        unpackedSize = size;
    }
    if (func_02035058() == 0) {
        limit = data_020c18bc.mUnk_47e;
    } else {
        limit = (u32) data_020c18bc.mUnk_47e >> 1;
    }
    if (!(data_020c18bc.mUnk_480 < limit && (u32) data_020c18bc.mUnk_484 < (u32) data_020c18bc.mUnk_47c << 10 &&
          data_020c18bc.mUnk_484 + unpackedSize < (u32) (data_020c18bc.mUnk_47c + 0x40) << 10)) {
        func_0201bb40(3);
        if (data_020c18bc.mUnk_480 >= data_020c18bc.mUnk_47e) {
            OS_Terminate();
        }
    }
    entry.mUnk_12 = mode;
    entry.mUnk_13 = flag;
    entry.mUnk_10 = file;
    entry.mUnk_00 = src;
    entry.mUnk_08 = size;
    entry.mUnk_0c = unpackedSize;
    entry.mUnk_04 = dst;
    if (data_020c18bc.mUnk_480 != 0) {
        memcmp(&entry, &data_020c18bc.mUnk_03c[data_020c18bc.mUnk_480 - 1], sizeof(Unk_LoadEntry));
    }
    data_020c18bc.mUnk_03c[data_020c18bc.mUnk_480++] = entry;
    data_020c18bc.mUnk_484 += unpackedSize;
}

// Queues a transfer whose unpacked size equals its stored size.
void func_0201c6b0(u16 file, u32 src, u32 dst, u32 size) {
    func_0201c580(file, src, dst, size, 0, 0, 0);
}

// Empties the transfer queue when the system is not busy.
void func_0201c6d4(void) {
    if (func_02035058() == 0) {
        data_020c18bc.mUnk_480 = 0;
        data_020c18bc.mUnk_484 = 0;
    }
}

// Limits a transfer size to what fits before the next scanline window (lines 0xbf-0xd6), waiting out the other
// lines when the display mode is 3.
u32 func_0201c704(u32 size) {
    u32 result = size;
    if (data_020c18bc.mUnk_490[0] == 3) {
        int line = *(vu16 *) HW_VCOUNT;
        if (!(line >= 0xbf && line < 0xd6)) {
            result = data_020c18bc.mUnk_48a * 0x16;
            while (*(vu16 *) HW_VCOUNT >= 0xd6 || *(vu16 *) HW_VCOUNT < 0xbf) {
            }
        } else {
            result = (0xd6 - line) * data_020c18bc.mUnk_48a;
        }
    }
    if (result >= size) {
        result = size;
    }
    return result;
}

// In display mode 3, waits until the display reaches line 0xc0.
void func_0201c78c(void) {
    if (data_020c18bc.mUnk_490[0] == 3) {
        while (*(vu16 *) HW_VCOUNT < 0xc0) {
        }
    }
}
#pragma thumb on

#pragma thumb off
// Runs the queued transfers: maps the VRAM banks to LCDC when the bank layout changed, picks the queued entries for
// VRAM (flags & 2) or other memory (flags & 1), sorts them by file and offset, reads and unpacks them into one
// buffer (packed block files, EZ10 blocks, plain file reads, memory copies), converts textures and palettes, then
// copies each entry to its destination with the banks mapped. When the system is busy, finished entries are
// removed from the queue instead.
void func_0201c7b8(int flags) {
    int i;
    u32 count;
    u32 n;
    Unk_LoadEntry **list;
    Unk_LoadEntry *e;
    int j;
    BOOL busy;
    u16 cachedFile;
    u32 total;
    int cachedSize;
    u8 *buf;
    int cachedOff;
    u32 bits;
    int rest;
    u8 *dst;
    u8 *src;
    u32 part;
    u32 saved;
    u8 *tmp;
    u32 hdr[4];
    u8 *tex;
    u32 lastDst;

    busy = func_02035058();
    if (data_020c18bc.mUnk_480 == 0) {
        return;
    }
    if (busy && (data_020c18bc.mUnk_46c & 3) == 0) {
        return;
    }
    list = func_0201c430(data_020c18bc.mUnk_47e << 2);
    bits = 0;
    cachedFile = 0;
    tmp = 0;
    tex = 0;
    if (data_020c18bc.mUnk_0c1 == 1) {
        data_020c18bc.mUnk_070.mUnk_2e = 0;
        data_020c18bc.mUnk_040.mUnk_2e = 0;
        for (i = 0; i < 9; i++) {
            u32 idx = data_020c18bc.mUnk_0b8[i];
            u32 use;
            const Unk_02050478 *head;
            const Unk_02050478 *rec;
            if (idx == 0xff) {
                continue;
            }
            use = data_02050478[idx].mUnk_2;
            if (use != 11 && use != 10 && use != 5 && use != 9) {
                continue;
            }
            for (j = idx - 1; data_02050478[j].mUnk_2 != 0; j--) {
            }
            head = &data_02050478[j];
            rec = &data_02050478[idx];
            switch (use) {
                case 11:
                    data_020c18bc.mUnk_040.mUnk_00[data_020c18bc.mUnk_040.mUnk_2e] =
                        (vu8 *) (data_02050220[head->mUnk_0] + (data_020502ac[head->mUnk_0] / 8));
                    data_020c18bc.mUnk_040.mUnk_28[data_020c18bc.mUnk_040.mUnk_2e] =
                        (rec->mUnk_9 << 3) + 0x80 + rec->mUnk_8;
                    data_020c18bc.mUnk_040.mUnk_18[data_020c18bc.mUnk_040.mUnk_2e] = head->mUnk_4;
                    data_020c18bc.mUnk_040.mUnk_2e++;
                    break;
                case 10:
                    data_020c18bc.mUnk_070.mUnk_00[data_020c18bc.mUnk_070.mUnk_2e] =
                        (vu8 *) (data_02050220[head->mUnk_0] + (data_020502ac[head->mUnk_0] / 8));
                    data_020c18bc.mUnk_070.mUnk_28[data_020c18bc.mUnk_070.mUnk_2e] =
                        (rec->mUnk_9 << 3) + 0x80 + rec->mUnk_8;
                    data_020c18bc.mUnk_070.mUnk_10[data_020c18bc.mUnk_070.mUnk_2e] = head->mUnk_1;
                    data_020c18bc.mUnk_070.mUnk_18[data_020c18bc.mUnk_070.mUnk_2e] = head->mUnk_4;
                    data_020c18bc.mUnk_070.mUnk_2e++;
                    break;
                case 5:
                    data_020c18bc.mUnk_0a0.mUnk_0 =
                        (vu8 *) (data_02050220[head->mUnk_0] + (data_020502ac[head->mUnk_0] / 8));
                    data_020c18bc.mUnk_0a0.mUnk_8 = (rec->mUnk_9 << 3) + 0x80 + rec->mUnk_8;
                    data_020c18bc.mUnk_0a0.mUnk_4 = head->mUnk_4;
                    break;
                case 9:
                    data_020c18bc.mUnk_0ac.mUnk_0 =
                        (vu8 *) (data_02050220[head->mUnk_0] + (data_020502ac[head->mUnk_0] / 8));
                    data_020c18bc.mUnk_0ac.mUnk_8 = (rec->mUnk_9 << 3) + 0x80 + rec->mUnk_8;
                    data_020c18bc.mUnk_0ac.mUnk_4 = head->mUnk_4;
                    break;
            }
        }
        for (i = 1; i < data_020c18bc.mUnk_070.mUnk_2e; i++) {
            if (data_020c18bc.mUnk_070.mUnk_18[i] !=
                data_020c18bc.mUnk_070.mUnk_18[i - 1] + (data_020c18bc.mUnk_070.mUnk_10[i - 1] << 10)) {
                OS_Terminate();
            }
        }
        data_020c18bc.mUnk_0c1 = 0;
    }

    count = 0;
    for (i = 0; i < data_020c18bc.mUnk_480; i++) {
        u32 addr;
        e = &data_020c18bc.mUnk_03c[i];
        if (e->mUnk_08 == 0) {
            continue;
        }
        addr = e->mUnk_04;
        if ((addr >> 28) == 11 || (addr >> 28) == 10 || (addr >= 0x06000000 && addr < 0x06400000)) {
            if ((flags & 2) && !busy) {
                list[count++] = e;
            }
        } else {
            if ((flags & 1) && !busy) {
                list[count++] = e;
            }
        }
    }
    if (count == 0) {
        func_0201c4f0(&list);
        return;
    }

    if (count > 1) {
        int gap = (1 << (32 - func_02028250(count))) - 1;
        while (gap > 0) {
            for (i = 0; i < (int) (count - gap); i++) {
                j = i;
                while (list[j]->mUnk_10 < list[j + gap]->mUnk_10 ||
                       (list[j]->mUnk_10 == list[j + gap]->mUnk_10 && list[j]->mUnk_00 > list[j + gap]->mUnk_00)) {
                    Unk_LoadEntry *t = list[j];
                    list[j] = list[j + gap];
                    list[j + gap] = t;
                    j -= gap;
                    if (j < 0) {
                        break;
                    }
                }
            }
            gap = (gap - 1) / 2;
        }
    }

    total = 0;
    for (i = 0; i < count; i++) {
        total += list[i]->mUnk_0c;
    }
    buf = func_0201c43c(total + 0x80, 1);
    dst = buf;
    cachedOff = 0;
    cachedSize = 0;
    for (i = 0; i < count; i++) {
        e = list[i];
        if (data_020c267c.mUnk_b26 == e->mUnk_10 && data_020c267c.mUnk_b2e == 1) {
            if (data_020c267c.mUnk_000 != 0 && e->mUnk_00 == data_020c267c.mUnk_00c << 3) {
                MIi_CpuCopy32(data_020c267c.mUnk_004, dst, e->mUnk_0c);
                data_020c267c.mUnk_017 = 0;
            } else {
                u32 *offsets = data_020c267c.mUnk_b1c;
                u32 first = e->mUnk_00 >> 11;
                u32 last = (e->mUnk_00 + e->mUnk_08) >> 11;
                u32 next;
                int size;
                for (j = i + 1; j < count && (next = (list[j]->mUnk_00 + list[j]->mUnk_08) >> 11) <= last + 1 &&
                                offsets[next + 1] - offsets[first] <= 0x8000;
                     j++) {
                    if (next > last) {
                        last = next;
                    }
                }
                size = offsets[last + 1] - offsets[first];
                if (tmp == 0) {
                    tmp = func_0201c43c(0x8000, 1);
                }
                if ((int) data_020c267c.mUnk_b1c[first] + size > cachedOff + cachedSize || j == 0 ||
                    list[i]->mUnk_10 != cachedFile) {
                    func_0202e4fc(list[i]->mUnk_10, tmp, size, data_020c267c.mUnk_b1c[first]);
                    cachedSize = size;
                    cachedOff = data_020c267c.mUnk_b1c[first];
                }
                func_02028150(tmp + (data_020c267c.mUnk_b1c[first] - cachedOff), dst, list[i]->mUnk_00 & 0x7ff,
                              list[i]->mUnk_08);
            }
        } else if (data_020c267c.mUnk_b26 == e->mUnk_10 && data_020c267c.mUnk_b2e == 2) {
            u32 sub = (e->mUnk_00 << 25) >> 20;
            u32 off;
            e->mUnk_00 = (e->mUnk_00 >> 7) << 2;
            e = list[i];
            off = e->mUnk_00;
            if (data_020c267c.mUnk_000 != 0 && data_020c267c.mUnk_00c == off + (sub << 20)) {
                MIi_CpuCopy32(data_020c267c.mUnk_004, dst, e->mUnk_0c);
                data_020c267c.mUnk_017 = 0;
            } else if (cachedOff != off || cachedFile != e->mUnk_10) {
                func_0202e4fc(e->mUnk_10, hdr, 0x10, off);
                if (hdr[0] == EZ10_MAGIC) {
                    u8 *packed;
                    if (tmp == 0) {
                        tmp = func_0201c43c(0x8000, 1);
                    }
                    if (hdr[1] > 0x8000) {
                        OS_Terminate();
                    }
                    if (hdr[1] < 0x10) {
                        OS_Terminate();
                    }
                    packed = (u8 *) (((u32) tmp + 0x8000 - hdr[3]) & ~3);
                    memcpy(packed, hdr, 0x10);
                    func_0202e4fc(list[i]->mUnk_10, packed + 0x10, hdr[3] - 0x10, off + 0x10);
                    func_0202875c(packed, tmp);
                    MIi_CpuCopy32(tmp + sub, dst, list[i]->mUnk_0c);
                    cachedOff = off;
                    cachedFile = list[i]->mUnk_10;
                } else {
                    func_0202e4fc(list[i]->mUnk_10, dst, list[i]->mUnk_0c, off + sub);
                }
            } else {
                MIi_CpuCopy32(tmp + sub, dst, e->mUnk_0c);
            }
        } else if (data_020c18bc.mUnk_42c == e->mUnk_10) {
            u32 sub = (e->mUnk_00 << 26) >> 20;
            u32 off = (e->mUnk_00 >> 6) << 2;
            if (data_020c18bc.mUnk_350 != off || data_020c18bc.mUnk_354 != e->mUnk_10) {
                u32 *packed;
                if (data_020c18bc.mUnk_34c == 0) {
                    data_020c18bc.mUnk_34c = func_0201c430(0x1200);
                }
                packed = (u32 *) (((u32) data_020c18bc.mUnk_34c + 0x1000 - list[i]->mUnk_08) & ~3);
                func_0202e4fc(list[i]->mUnk_10, packed, list[i]->mUnk_08, off);
                if (packed[0] != EZ10_MAGIC) {
                    data_020c18bc.mUnk_350 = off;
                    data_020c18bc.mUnk_354 = list[i]->mUnk_10;
                    MIi_CpuCopyFast(packed, data_020c18bc.mUnk_34c, list[i]->mUnk_08);
                    MIi_CpuCopyFast(data_020c18bc.mUnk_34c + sub, dst, list[i]->mUnk_0c);
                    list[i]->mUnk_08 = list[i]->mUnk_0c;
                } else {
                    if (packed[0] != EZ10_MAGIC) {
                        OS_Terminate();
                    }
                    if (packed[1] > 0x8000) {
                        OS_Terminate();
                    }
                    if (packed[3] != list[i]->mUnk_08) {
                        OS_Terminate();
                    }
                    func_0202875c(packed, data_020c18bc.mUnk_34c);
                    MIi_CpuCopyFast(data_020c18bc.mUnk_34c + sub, dst, list[i]->mUnk_0c);
                    data_020c18bc.mUnk_350 = off;
                    data_020c18bc.mUnk_354 = list[i]->mUnk_10;
                    list[i]->mUnk_08 = list[i]->mUnk_0c;
                }
            } else {
                MIi_CpuCopyFast(data_020c18bc.mUnk_34c + sub, dst, e->mUnk_0c);
                list[i]->mUnk_08 = list[i]->mUnk_0c;
            }
        } else if (data_020c31dc.mUnk_54 == e->mUnk_10 && e->mUnk_08 < e->mUnk_0c) {
            u32 size = e->mUnk_08;
            u32 *packed = (u32 *) (((u32) dst + e->mUnk_0c + 0x80 - size) & ~3);
            func_0202e4fc(e->mUnk_10, packed, size, e->mUnk_00);
            data_020c1834.mUnk_08 += size;
            data_020c1834.mUnk_0c++;
            if (packed[0] != EZ10_MAGIC) {
                OS_Terminate();
            }
            if (packed[1] != list[i]->mUnk_0c) {
                OS_Terminate();
            }
            if (-packed[2] >= list[i]->mUnk_0c + 0x80 - size) {
                OS_Terminate();
            }
            func_0202875c(packed, dst);
            list[i]->mUnk_08 = list[i]->mUnk_0c;
        } else if (e->mUnk_10 != 0) {
            u32 size = e->mUnk_08;
            j = i + 1;
            if (cachedFile != e->mUnk_10 || e->mUnk_00 + size > cachedOff + cachedSize ||
                (i > 0 && list[i - 1]->mUnk_00 == e->mUnk_00) || (lastDst & ~0x3ffff) != (e->mUnk_04 & ~0x3ffff)) {
                cachedFile = list[i]->mUnk_10;
                cachedOff = list[i]->mUnk_00;
                lastDst = list[j - 1]->mUnk_04;
                cachedSize = size;
                func_0202e4fc(cachedFile, dst, size, cachedOff);
                if (data_020c18bc.mUnk_464 != 0 && (list[i]->mUnk_04 >> 28) == 11) {
                    DC_FlushRange(dst, size);
                    data_020c18bc.mUnk_464(dst, size);
                }
            }
        } else {
            if (e->mUnk_08 & 1) {
                OS_Terminate();
            }
            MIi_CpuCopy16((void *) list[i]->mUnk_00, dst, list[i]->mUnk_08);
        }

        e = list[i];
        if (data_020c267c.mUnk_b26 == e->mUnk_10) {
            BOOL bg = 0;
            u32 area = e->mUnk_04 >> 20;
            if (area < 0x64 && area >= 0x60) {
                bg = 1;
            }
            if (e->mUnk_13 != 0 && e->mUnk_13 != 0xff) {
                Unk_020c2694 *half = &data_020c267c.mUnk_018[e->mUnk_13 & 1];
                func_02028928((u16 *) dst, &half->mUnk_115[half->mUnk_014[e->mUnk_13 >> 1]], e->mUnk_08 >> 1);
            }
            e = list[i];
            if ((e->mUnk_04 >> 28) == 10 || bg == 1) {
                int w = (1 << (e->mUnk_12 & 0xf)) / 8;
                u32 h = e->mUnk_08 / (e->mUnk_13 == 0 ? 0x20 : 0x40) / w;
                if (tex == 0) {
                    tex = func_0201c43c(0x4000, 1);
                }
                func_0202c2f0(tex, dst, w, h, list[i]->mUnk_13 == 0);
                e = list[i];
                if ((e->mUnk_04 >> 28) == 10) {
                    MIi_CpuCopy16(tex, dst, e->mUnk_08);
                } else {
                    func_0202c3e0(e, tex, w);
                }
            }
        }
        dst += list[i]->mUnk_08;
    }

    func_0203e4a4();
    src = buf;
    for (i = 0; i < count;) {
        u32 base;
        u32 run;
        rest = 0;
        part = 0;
        e = list[i];
        if (e->mUnk_12 == 0xff) {
            src += e->mUnk_08;
            i++;
            continue;
        }
        base = e->mUnk_00;
        run = e->mUnk_08;
        for (j = i + 1; j < count && base + run == list[j]->mUnk_00 && run == 0 && list[i]->mUnk_10 == list[j]->mUnk_10;
             j++) {
            run += list[j]->mUnk_08;
        }
        while (i < j) {
            u32 addr;
            u32 type;
            e = list[i];
            addr = e->mUnk_04;
            type = addr >> 28;
            if (type == 5) {
                bits |= 0x20;
                e->mUnk_04 = data_020c18bc.mUnk_0a0.mUnk_4 + (addr & 0x1fff);
                func_0201c78c();
                *data_020c18bc.mUnk_0a0.mUnk_0 = 0x80;
            } else if (type == 9) {
                bits |= 0x200;
                e->mUnk_04 = data_020c18bc.mUnk_0ac.mUnk_4 + (addr & 0x1fff);
                func_0201c78c();
                *data_020c18bc.mUnk_0ac.mUnk_0 = 0x80;
            } else if (type == 11) {
                if (data_020c18bc.mUnk_040.mUnk_2e != 0) {
                    saved = addr;
                    e->mUnk_04 = data_020c18bc.mUnk_040.mUnk_18[0] + (addr & 0x7fffff);
                    bits |= 0x800;
                    part = func_0201c704(list[i]->mUnk_08);
                    e = list[i];
                    rest = e->mUnk_08 - part;
                    e->mUnk_08 = part;
                    *data_020c18bc.mUnk_040.mUnk_00[0] = 0x80;
                } else {
                    rest = 0;
                    goto next;
                }
            } else if (type == 10) {
                Unk_VramGroup *group = &data_020c18bc.mUnk_070;
                if (data_020c18bc.mUnk_070.mUnk_2e != 0) {
                    saved = addr;
                    e->mUnk_04 = group->mUnk_18[0] + (addr & 0x7fffff);
                    bits |= 0x400;
                    part = func_0201c704(list[i]->mUnk_08);
                    e = list[i];
                    rest = e->mUnk_08 - part;
                    e->mUnk_08 = part;
                    for (n = 0; n < group->mUnk_2e; n++) {
                        Unk_LoadEntry *f = list[i];
                        if (group->mUnk_18[n] <= f->mUnk_04 + f->mUnk_08 &&
                            group->mUnk_18[n] + (group->mUnk_10[n] << 10) >= f->mUnk_04) {
                            *data_020c18bc.mUnk_070.mUnk_00[n] = 0x80;
                        }
                    }
                } else {
                    rest = 0;
                    goto next;
                }
            }
            e = list[i];
            if (e->mUnk_08 < 0x100) {
                MIi_CpuCopyFast(src, (void *) e->mUnk_04, e->mUnk_08);
            } else {
                MI_DmaCopy32(3, src, (void *) e->mUnk_04, e->mUnk_08);
            }
            if (bits & 0x400) {
                for (n = 0; n < data_020c18bc.mUnk_070.mUnk_2e; n++) {
                    *data_020c18bc.mUnk_070.mUnk_00[n] = data_020c18bc.mUnk_070.mUnk_28[n];
                }
                bits &= ~0x400;
            } else if (bits & 0x800) {
                *data_020c18bc.mUnk_040.mUnk_00[0] = data_020c18bc.mUnk_040.mUnk_28[0];
                bits &= ~0x800;
            } else if (bits & 0x20) {
                *data_020c18bc.mUnk_0a0.mUnk_0 = data_020c18bc.mUnk_0a0.mUnk_8;
                bits &= ~0x20;
            } else if (bits & 0x200) {
                *data_020c18bc.mUnk_0ac.mUnk_0 = data_020c18bc.mUnk_0ac.mUnk_8;
                bits &= ~0x200;
            }
        next:
            e = list[i];
            src += e->mUnk_08;
            if (rest > 0) {
                e->mUnk_04 = saved + part;
                list[i]->mUnk_08 = rest;
                rest = 0;
            } else {
                i++;
            }
        }
    }

    func_0201c4f0(&tmp);
    func_0201c4f0(&tex);
    func_0201c4f0(&buf);
    func_0201c4f0(&list);
    if (busy == 1) {
        i = 0;
        while (1) {
            if (i == data_020c18bc.mUnk_480) {
                return;
            }
            e = &data_020c18bc.mUnk_03c[i];
            if (e->mUnk_08 == 0 || e->mUnk_12 == 0xff || data_020c267c.mUnk_b26 == e->mUnk_10) {
                if (i != data_020c18bc.mUnk_480 - 1) {
                    memcpy(e, &data_020c18bc.mUnk_03c[data_020c18bc.mUnk_480 - 1], sizeof(Unk_LoadEntry));
                }
                data_020c18bc.mUnk_480--;
            } else {
                i++;
            }
        }
    }
}
#pragma thumb on
