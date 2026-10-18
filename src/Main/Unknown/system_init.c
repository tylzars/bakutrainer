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
#include <Main/Unknown/touch_buttons.h>
#include <Main/Unknown/random_angle.h>
#include <Main/Unknown/game_math.h>
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
#include <nitro/math.h>
#include <Main/Unknown/save_blocks.h>
#include <Main/Unknown/bg_image.h>
#include <Main/Unknown/screen_fade.h>
#include <Main/Unknown/file_stream.h>
#include <Main/Unknown/system_init.h>
#include <Main/Unknown/camera.h>
#include <Main/Unknown/model.h>
#include <Main/Unknown/engine_asm.h>

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
    data_020c5e44.mUnk_000.mUnk_74 = 0;
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
    data_020c181c = OS_GetArenaLo(0);
    OS_SetArenaLo(0, OS_InitAlloc(0, data_020c181c, OS_GetArenaHi(0), 2));
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
    data_020c1828.func = func_0201c1d0;
    data_020c1828.arg = 0;
    func_0204445c(&data_020c1828);
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
    data_020c5e44.mUnk_000.mUnk_6c = 0;
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
            if (data_020c5e44.mUnk_300.mUnk_4a == 0 && !(data_020c18bc.mUnk_46c & 0x10000)) {
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
                if (data_020c2560.mUnk_100.mUnk_10 < 10) {
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
            data_020c2560.mUnk_100.mUnk_04 = func_02020bac(0xffff);
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

void func_0201c1d0(void *arg) {
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
    return data_020c31dc.mUnk_58;
}

u16 func_0201c544(void) {
    return data_020c31dc.mUnk_5a;
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
void func_0201c580(u32 file, u32 src, u32 dst, u32 size, u32 unpackedSize, u8 mode, u8 flag) {
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

void func_0201d9a8(int offset) {
    if ((data_020c18bc.mUnk_46c & 0x200) == 0) {
        func_020425f4(&data_020c1874, offset, 0);
    } else {
        data_020c18bc.mUnk_250.mUnk_00 = offset;
    }
}

void func_0201d9d4(void *dst, u32 size) {
    int remaining;
    int state;
    int i;
    int j;
    u32 len;
    u32 sum;
    u8 *cmd = data_020c67e0;

    if ((data_020c18bc.mUnk_46c & 0x200) == 0) {
        func_020425e4(&data_020c1874, dst, size);
        return;
    }
restart:
    state = 0;
    i = 0;
    remaining = size;
    if (size > 0xfb800) {
        OS_Terminate();
    }
    for (; remaining > 0; remaining -= 0x1f7) {
        data_020c18bc.mUnk_250.mUnk_24[i >> 3] &= ~(1 << (i & 7));
        i++;
    }
    data_020c18bc.mUnk_250.mUnk_24[i >> 3] |= 1 << (i & 7);
    data_020c18bc.mUnk_250.mUnk_08 = size;
    data_020c18bc.mUnk_250.mUnk_04 = size;
    data_020c18bc.mUnk_250.mUnk_0c = dst;
    data_020c18bc.mUnk_46c |= 3 << 18;
    remaining = size;
    while (data_020c18bc.mUnk_250.mUnk_08 != 0 && (s8) data_020c18bc.mUnk_2a4[1] >= 0) {
        if (data_020c18bc.mUnk_250.mUnk_08 != remaining) {
            remaining = data_020c18bc.mUnk_250.mUnk_08;
            state = 5;
        } else if (state-- == 0) {
            i = 0;
            while (data_020c18bc.mUnk_250.mUnk_24[i >> 3] & (1 << (i & 7))) {
                i++;
            }
            j = i;
            sum = 0;
            while (!(data_020c18bc.mUnk_250.mUnk_24[j >> 3] & (1 << (j & 7)))) {
                j++;
                sum += 0x1f7;
            }
            len = (j - i) * 0x1f7;
            if (len >= data_020c18bc.mUnk_250.mUnk_04) {
                len = data_020c18bc.mUnk_250.mUnk_04;
            }
            cmd[0] = 0x51;
            func_02034ce0(data_020c18bc.mUnk_250.mUnk_00 + 0x1f7 * i, cmd + 6);
            func_02034ce0(len, cmd + 2);
            state = 5;
            if (func_02034cf0(cmd + 6) > data_020c18bc.mUnk_250.mUnk_00 + data_020c18bc.mUnk_250.mUnk_04) {
                goto restart;
            }
        }
        func_0201bb30();
    }
    data_020c18bc.mUnk_250.mUnk_00 += size;
    data_020c18bc.mUnk_46c &= 0xfff3ffff;
}

u32 func_0201db58(int layer) {
    u16 control;

    if (layer != 1 && layer != 2) {
        OS_Terminate();
    }
    if (layer & 1) {
        control = *(vu16 *) 0x0400000e;
    } else {
        control = *(vu16 *) 0x0400100e;
    }
    if ((control & 0x84) == 0x84) {
        return 0x8000;
    }
    return 0x100;
}

int func_0201db94(int layer) {
    if (layer != 1 && layer != 2) {
        OS_Terminate();
    }
    if (layer & 1) {
        return data_02050200[*(vu16 *) 0x0400000e >> 14];
    }
    return data_02050200[*(vu16 *) 0x0400100e >> 14];
}

int func_0201dbd4(int layer) {
    int n = func_0201db94(layer);

    if (func_0201db58(layer) == 0x8000) {
        n <<= 1;
    }
    return n;
}

#pragma thumb off
void func_0201dbf4(u16 *src, u32 dst, u32 width, u32 stride, u8 *lut) {
    u32 row = 16;
    int i;
    u16 *out;

    if (lut == NULL) {
        do {
            MIi_CpuCopy16(src, (void *) dst, width * 2);
            src += 0x200;
            dst += stride * 2;
        } while (--row != 0);
        return;
    }
    out = (u16 *) ((dst & 0xfff00000) + ((dst & 0xfffff) >> 1));
    do {
        for (i = 0; i < width / 2; i++) {
            u8 lo = lut[src[0] - 0x8000];
            u8 hi = lut[src[1] - 0x8000];
            src += 2;
            *out++ = lo | (hi << 8);
        }
        src += 0x200 - width;
        out += stride - width;
    } while (--row != 0);
}
#pragma thumb on

void func_0201dcc0(u16 *src, u32 layers, u32 row, u32 width, u32 total, u8 *lut) {
    u32 half;
    int n;

    switch (layers & 3) {
        case 1:
            n = func_0201db94(layers);
            func_0201dbf4(src, 0x06000000 + row * n * 2, width, n, lut);
            break;
        case 2:
            n = func_0201db94(layers);
            func_0201dbf4(src, 0x06200000 + row * n * 2, width, n, lut);
            break;
        case 3:
            half = total >> 1;
            if (row < half) {
                n = func_0201db94(1);
                func_0201dbf4(src, 0x06000000 + row * n * 2, width, n, lut);
            } else {
                n = func_0201db94(2);
                func_0201dbf4(src, 0x06200000 + (row - half) * n * 2, width, n, lut);
            }
            break;
    }
}

void func_0201dd74(u8 *src, u32 dst, u32 width, u32 rows) {
    int layer;
    int step;

    if (dst == 0x06000000) {
        layer = 1;
    } else {
        layer = 2;
    }
    if (func_0201db58(layer) == 0x8000) {
        width += width;
    }
    if (func_0204873c(width) == 1) {
        MIi_CpuCopy16(src, (void *) dst, width * rows);
        return;
    }
    step = func_0201dbd4(layer);
    while (rows != 0) {
        MIi_CpuCopy16(src, (void *) dst, width);
        src += width;
        dst += step;
        rows--;
    }
}

void func_0201dde0(int id, int target) {
    func_0201ddec(id, target, 0, 0);
}

void func_0201ddec(int id, int target, int x, int flags) {
    int found;
    void *buf;

    found = func_0202e988(id);
    if (found > 0) {
        func_0201de48((u8 *) found, target, x, flags);
    } else {
        buf = (void *) func_0202e86c(id);
        func_0201de48(buf, target, x, flags);
        func_0201c4f0(&buf);
    }
    if (target == 3) {
        data_020c18bc.mUnk_49c[1] = id;
        data_020c18bc.mUnk_49c[0] = data_020c18bc.mUnk_49c[1];
    } else if (target == 1) {
        data_020c18bc.mUnk_49c[0] = id;
    } else if (target == 2) {
        data_020c18bc.mUnk_49c[1] = id;
    }
}

void func_0201de48(u8 *data, int target, int x, int flags) {
    void *tmp = NULL;
    void *rgb = NULL;
    u32 w;
    u32 h;
    u32 h2;
    u32 magic;
    u32 mode;
    u32 size;
    u32 count;
    u32 n;
    u32 i;
    u32 j;
    int indexed = 0;
    u8 *pix;
    u8 *src;
    u8 *pal;
    u16 *out;
    u16 *top;
    u16 *bottom;
    u16 color;

    magic = *(u32 *) data;
    if (magic == 0xe0ffd8ff) {
        func_0201e518(data, &w, &h);
        data_020c18bc.mUnk_2cc = w;
        data_020c18bc.mUnk_2ce = h;
        data_020c18bc.mUnk_2d0 = 0x8000;
        h2 = h;
        if ((target & 3) == 3 && h >= 0x180) {
            h2 >>= 1;
        }
        if (x == 0) {
            if (!(flags & 2)) {
                func_0201f450(target, 1, w, h2);
            }
        } else {
            if (!(flags & 2)) {
                func_0201f450(target, 0, w, h2);
            }
            tmp = func_0201eee4(x, target);
        }
        if (target & 1) {
            func_0201e56c(data, target, tmp);
        }
        if (target & 2) {
            func_0201e56c(data, 2, tmp);
        }
        func_0201c4f0(&tmp);
    } else {
        if (magic == 0x6177686d) {
            w = (data[8] << 8) + data[9];
            h = (data[10] << 8) + data[11];
            h2 = h;
            count = (data[12] << 8) + data[13];
        } else {
            w = *(u16 *) (data + 0xc);
            h = *(u16 *) (data + 0xe);
            h2 = h;
            if (*(u16 *) (data + 0x10) != 0xf) {
                OS_Terminate();
            }
            count = 0;
            indexed = 1;
        }
        data_020c18bc.mUnk_2cc = w;
        data_020c18bc.mUnk_2ce = h;
        mode = target & 3;
        if (mode == 3 && h >= 0x180) {
            h2 = h >> 1;
        }
        if (count != 0) {
            if (!(flags & 1)) {
                data_020c18bc.mUnk_2d0 = 0x100;
                if (!(flags & 2)) {
                    func_0201f450(target, 0, w, h2);
                }
                pix = func_0201f5bc(data + 0x20, count, 0, target);
                DC_StoreRange(pix, w * h + 0x320);
                size = h2 * w;
            } else {
                pal = data + 0x20;
                src = pal + count * 3;
                data_020c18bc.mUnk_2d0 = 0x8000;
                if (!(flags & 2)) {
                    func_0201f450(target, 1, w, h2);
                }
                DC_StoreRange(data, w * h + 0x320);
                size = h2 * w * 2;
                rgb = func_0201c430(h * w * 2);
                out = rgb;
                for (n = h * w; n != 0; n--) {
                    u8 *c = pal + *src * 3;

                    color = (c[0] >> 3) | ((c[1] >> 3) << 5) | ((c[2] >> 3) << 10);
                    *out++ = color + 0x8000;
                    src++;
                }
                pix = rgb;
            }
        } else {
            top = (u16 *) (data + 0x12);
            pal = data + 0x20;
            data_020c18bc.mUnk_2d0 = 0x8000;
            if (!(flags & 2)) {
                func_0201f450(target, 1, w, h2);
            }
            DC_StoreRange(data, w * h * 3 + 0x20);
            pix = pal;
            if (indexed == 0) {
                out = (u16 *) pal;
                for (n = h * w; n != 0; n--) {
                    color = (pal[0] >> 3) | ((pal[1] >> 3) << 5) | ((pal[2] >> 3) << 10);
                    *out++ = color + 0x8000;
                    pal += 3;
                }
            } else {
                bottom = top + (h - 1) * w;
                pix = (u8 *) top;
                for (i = 0; i < h / 2; i++) {
                    for (j = 0; j < w; j++) {
                        u16 a = bottom[j];
                        u16 b = top[j];

                        top[j] = a + 0x8000;
                        bottom[j] = b + 0x8000;
                    }
                    top += w;
                    bottom -= w;
                }
            }
            size = h2 * w * 2;
        }
        switch (mode) {
            case 1:
                func_0201dd74(pix, 0x06000000, w, h);
                break;
            case 2:
                func_0201dd74(pix, 0x06200000, w, h);
                break;
            case 3:
                func_0201dd74(pix, 0x06000000, w, h2);
                if (h < 0x180) {
                    size = 0;
                }
                func_0201dd74(pix + size, 0x06200000, w, h2);
                break;
        }
    }
    func_0201c4f0(&rgb);
}

// JPEG reader state (the asm block decoder func_02028980 uses it too): the count of buffered bits, the read
// pointer (just past the marker of the current segment) and the bit buffer; data_020c181c is the arena start and
// data_020c1828 the sleep callback node the start-up code keeps. This definition order gives the original .bss
// layout 1818, 181c, 1820, 1824, 1828 (ties in size keep definition order except that the first goes last).
u8 data_020c1818;
u32 data_020c1824;
u8 *data_020c1820;
void *data_020c181c;
PMCallbackInfo data_020c1828;

// VRAM control register of each bank (word address; data_020502ac holds the bit offset of the bank's byte).
const u32 data_02050220[9] = {
    0x04000240, 0x04000240, 0x04000240, 0x04000240, 0x04000244, 0x04000244, 0x04000244, 0x04000248, 0x04000248,
};

// VRAM address of each use, taken when the caller passes -1.
const u32 data_02050270[13] = {
    0x06800000, 0x00000000, 0x06000000, 0x06400000, 0x00000000, 0x00000000, 0x06200000,
    0x06600000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// Blocks per MCU and quantisation table of each colour component (4:2:0 Y, Cb, Cr).
const u8 data_020502a4[4] = {4, 1, 1, 0};

const u8 data_020502a8[4] = {0, 1, 1, 0};

const u8 data_020502ac[9] = {0, 8, 16, 24, 0, 8, 16, 0, 8};

// Zigzag order of the 64 coefficients of a block.
const u8 data_020502b8[64] = {
    0,  1,  8,  16, 9,  2,  3,  10, 17, 24, 32, 25, 18, 11, 4,  5,  12, 19, 26, 33, 40, 48,
    41, 34, 27, 20, 13, 6,  7,  14, 21, 28, 35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23,
    30, 37, 44, 51, 58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63,
};

// Inverse DCT scale factors (4.12 fixed point) applied to the quantisation tables.
const u16 data_020502f8[64] = {
    0x4000, 0x58c5, 0x539f, 0x4b42, 0x4000, 0x3249, 0x22a3, 0x11a8, 0x58c5, 0x7b21, 0x73fc, 0x6862, 0x58c5,
    0x45bf, 0x300b, 0x187e, 0x539f, 0x73fc, 0x6d41, 0x6254, 0x539f, 0x41b3, 0x2d41, 0x1712, 0x4b42, 0x6862,
    0x6254, 0x587e, 0x4b42, 0x3b21, 0x28ba, 0x14c3, 0x4000, 0x58c5, 0x539f, 0x4b42, 0x4000, 0x3249, 0x22a3,
    0x11a8, 0x3249, 0x45bf, 0x41b3, 0x3b21, 0x3249, 0x2782, 0x1b37, 0x0de0, 0x22a3, 0x300b, 0x2d41, 0x28ba,
    0x22a3, 0x1b37, 0x12bf, 0x098e, 0x11a8, 0x187e, 0x1712, 0x14c3, 0x11a8, 0x0de0, 0x098e, 0x04df,
};

// VRAM bank records, each bank's LCDC record first; a record with size 0 ends the table.
const Unk_02050478 data_02050478[] = {
    {0, 128, 0, 0, 0x06800000, 0, 0},  {0, 128, 2, 0, 0x06000000, 1, 0},  {0, 128, 2, 0, 0x06020000, 1, 1},
    {0, 128, 2, 0, 0x06040000, 1, 2},  {0, 128, 2, 0, 0x06060000, 1, 3},  {0, 128, 3, 0, 0x06400000, 2, 0},
    {0, 128, 3, 0, 0x06420000, 2, 1},  {0, 128, 10, 0, 0x00000000, 3, 0}, {0, 128, 10, 0, 0x00020000, 3, 1},
    {0, 128, 10, 0, 0x00040000, 3, 2}, {0, 128, 10, 0, 0x00060000, 3, 3}, {1, 128, 0, 0, 0x06820000, 0, 0},
    {1, 128, 2, 0, 0x06000000, 1, 0},  {1, 128, 2, 0, 0x06020000, 1, 1},  {1, 128, 2, 0, 0x06040000, 1, 2},
    {1, 128, 2, 0, 0x06060000, 1, 3},  {1, 128, 3, 0, 0x06400000, 2, 0},  {1, 128, 3, 0, 0x06420000, 2, 1},
    {1, 128, 10, 0, 0x00000000, 3, 0}, {1, 128, 10, 0, 0x00020000, 3, 1}, {1, 128, 10, 0, 0x00040000, 3, 2},
    {1, 128, 10, 0, 0x00060000, 3, 3}, {2, 128, 0, 0, 0x06840000, 0, 0},  {2, 128, 2, 0, 0x06000000, 1, 0},
    {2, 128, 2, 0, 0x06020000, 1, 1},  {2, 128, 2, 0, 0x06040000, 1, 2},  {2, 128, 2, 0, 0x06060000, 1, 3},
    {2, 128, 1, 0, 0x06000000, 2, 0},  {2, 128, 1, 0, 0x06020000, 2, 1},  {2, 128, 10, 0, 0x00000000, 3, 0},
    {2, 128, 10, 0, 0x00020000, 3, 1}, {2, 128, 10, 0, 0x00040000, 3, 2}, {2, 128, 10, 0, 0x00060000, 3, 3},
    {2, 128, 6, 0, 0x06200000, 4, 0},  {3, 128, 0, 0, 0x06860000, 0, 0},  {3, 128, 2, 0, 0x06000000, 1, 0},
    {3, 128, 2, 0, 0x06020000, 1, 1},  {3, 128, 2, 0, 0x06040000, 1, 2},  {3, 128, 2, 0, 0x06060000, 1, 3},
    {3, 128, 1, 0, 0x06000000, 2, 0},  {3, 128, 1, 0, 0x06020000, 2, 1},  {3, 128, 10, 0, 0x00000000, 3, 0},
    {3, 128, 10, 0, 0x00020000, 3, 1}, {3, 128, 10, 0, 0x00040000, 3, 2}, {3, 128, 10, 0, 0x00060000, 3, 3},
    {3, 128, 7, 0, 0x06600000, 4, 0},  {4, 64, 0, 0, 0x06880000, 0, 0},   {4, 64, 2, 0, 0x06000000, 1, 0},
    {4, 64, 3, 0, 0x06400000, 2, 0},   {4, 64, 11, 0, 0x00000000, 3, 0},  {4, 32, 12, 0, 0x00000000, 4, 0},
    {5, 16, 0, 0, 0x06890000, 0, 0},   {5, 16, 2, 0, 0x06000000, 1, 0},   {5, 16, 2, 0, 0x06004000, 1, 1},
    {5, 16, 2, 0, 0x06010000, 1, 2},   {5, 16, 2, 0, 0x06014000, 1, 3},   {5, 16, 3, 0, 0x06400000, 2, 0},
    {5, 16, 3, 0, 0x06404000, 2, 1},   {5, 16, 3, 0, 0x06410000, 2, 2},   {5, 16, 3, 0, 0x06414000, 2, 3},
    {5, 16, 11, 0, 0x00000000, 3, 0},  {5, 16, 11, 0, 0x00010000, 3, 1},  {5, 16, 11, 0, 0x00014000, 3, 2},
    {5, 16, 11, 0, 0x00018000, 3, 3},  {5, 16, 12, 0, 0x00000000, 4, 1},  {5, 16, 5, 0, 0x00000000, 5, 0},
    {6, 16, 0, 0, 0x06894000, 0, 0},   {6, 16, 2, 0, 0x06000000, 1, 0},   {6, 16, 2, 0, 0x06004000, 1, 1},
    {6, 16, 2, 0, 0x06010000, 1, 2},   {6, 16, 2, 0, 0x06014000, 1, 3},   {6, 16, 3, 0, 0x06400000, 2, 0},
    {6, 16, 3, 0, 0x06404000, 2, 1},   {6, 16, 3, 0, 0x06410000, 2, 2},   {6, 16, 3, 0, 0x06414000, 2, 3},
    {6, 16, 11, 0, 0x00000000, 3, 0},  {6, 16, 11, 0, 0x00010000, 3, 1},  {6, 16, 11, 0, 0x00014000, 3, 2},
    {6, 16, 11, 0, 0x00018000, 3, 3},  {6, 16, 12, 0, 0x00000000, 4, 1},  {6, 16, 5, 0, 0x00000000, 5, 0},
    {7, 32, 0, 0, 0x06898000, 0, 0},   {7, 32, 6, 0, 0x06200000, 1, 0},   {8, 16, 0, 0, 0x068a0000, 0, 0},
    {8, 16, 6, 0, 0x06208000, 1, 0},   {8, 16, 7, 0, 0x06600000, 2, 0},   {8, 16, 9, 0, 0x00000000, 3, 0},
    {0, 0, 0, 0, 0x00000000, 0, 0},
};

#pragma thumb off
u8 *func_0201e178(void) {
    u8 *p = data_020c1820;

    return p + (p[0] << 8) + p[1] + 2;
}

// Inverse DCT of one 8x8 block: dequantised columns into an s16 workspace, then rows through the clamp table.
void func_0201e19c(s16 *in, s16 *quant, u8 *out, u8 *clamp) {
    s16 work[64];
    s16 *ws = work;
    int n;
    int a0, a1, a2, a3;
    int b0, b1, b2, b3;
    int e0, e1, e2, e3;
    int z5, c10, c11, c12, c13;

    for (n = 8; n > 0; n--) {
        if ((in[8] || in[16] || in[24] || in[32] || in[40] || in[48] || in[56]) == 0) {
            ws[0] = ws[8] = ws[16] = ws[24] = ws[32] = ws[40] = ws[48] = ws[56] = in[0] * quant[0];
            in++;
            quant++;
            ws++;
            continue;
        }
        a0 = in[0] * quant[0];
        a1 = in[16] * quant[16];
        a2 = in[32] * quant[32];
        a3 = in[48] * quant[48];
        c10 = a0 + a2;
        c11 = a0 - a2;
        c13 = a1 + a3;
        c12 = (((a1 - a3) * 362) >> 8) - c13;
        e0 = c10 + c13;
        e3 = c10 - c13;
        e1 = c11 + c12;
        e2 = c11 - c12;

        b0 = in[8] * quant[8];
        b1 = in[24] * quant[24];
        b2 = in[40] * quant[40];
        b3 = in[56] * quant[56];
        a0 = b2 + b1;
        a1 = b2 - b1;
        a2 = b0 + b3;
        a3 = b0 - b3;
        b3 = a2 + a0;
        c11 = ((a2 - a0) * 362) >> 8;
        z5 = ((a1 + a3) * 473) >> 8;
        c10 = ((a3 * 277) >> 8) - z5;
        c12 = ((a1 * -669) >> 8) + z5;
        b2 = c12 - b3;
        b1 = c11 - b2;
        b0 = c10 + b1;

        ws[0] = e0 + b3;
        ws[56] = e0 - b3;
        ws[8] = e1 + b2;
        ws[48] = e1 - b2;
        ws[16] = e2 + b1;
        ws[40] = e2 - b1;
        ws[32] = e3 + b0;
        ws[24] = e3 - b0;
        in++;
        quant++;
        ws++;
    }

    ws = work;
    for (n = 0; n < 8; n++) {
        c10 = ws[0] + ws[4];
        c11 = ws[0] - ws[4];
        c13 = ws[2] + ws[6];
        c12 = (((ws[2] - ws[6]) * 362) >> 8) - c13;
        e0 = c10 + c13;
        e3 = c10 - c13;
        e1 = c11 + c12;
        e2 = c11 - c12;

        a0 = ws[5] + ws[3];
        a1 = ws[5] - ws[3];
        a2 = ws[1] + ws[7];
        a3 = ws[1] - ws[7];
        b3 = a2 + a0;
        c11 = ((a2 - a0) * 362) >> 8;
        z5 = ((a1 + a3) * 473) >> 8;
        c10 = ((a3 * 277) >> 8) - z5;
        c12 = ((a1 * -669) >> 8) + z5;
        b2 = c12 - b3;
        b1 = c11 - b2;
        b0 = c10 + b1;

        out[0] = clamp[(e0 + b3) >> 5];
        out[7] = clamp[(e0 - b3) >> 5];
        out[1] = clamp[(e1 + b2) >> 5];
        out[6] = clamp[(e1 - b2) >> 5];
        out[2] = clamp[(e2 + b1) >> 5];
        out[5] = clamp[(e2 - b1) >> 5];
        out[4] = clamp[(e3 + b0) >> 5];
        out[3] = clamp[(e3 - b0) >> 5];
        ws += 8;
        out += 8;
    }
}

void func_0201e518(u8 *data, u32 *width, u32 *height) {
    data += 4;
    while (data[-1] != 0xc0) {
        data += (data[0] << 8) + data[1] + 2;
    }
    *height = (data[3] << 8) + data[4];
    *width = (data[5] << 8) + data[6];
}

void func_0201e56c(u8 *data, int target, void *lut) {
    s16 quant[2][64];
    u8 bits[4][32];
    u8 huffval[4][256];
    u8 valptr[4][32];
    s16 maxcode[4][32];
    u16 mincode[4][32];
    s16 dc[3];
    u8 *buf;
    s16 *block;
    u16 *out;
    u8 *mcu;
    u8 *huffsize;
    u16 *huffcode;
    u16 height;
    u32 y;
    u16 width;
    u32 ncomp;
    u32 x;
    u32 c;
    u32 n;
    u32 i;
    u32 l;
    u32 k;
    u32 len;
    u32 total;
    u32 t;
    u32 code;
    u32 si;
    u8 *q;
    u8 *blk;
    u32 marker;

    ncomp = 0;
    height = 0;
    width = 0;
    data_020c1820 = data + 4;
    buf = func_0201c430(0x300);
    huffsize = buf;
    huffcode = (u16 *) (buf + 0x100);
    block = func_0201c430(0x80);
    while (data_020c1820[-1] != 0xda) {
        switch (data_020c1820[-1]) {
            case 0xdb:
                q = data_020c1820;
                len = q[1] + (q[0] << 8);
                q += 2;
                len -= 2;
                while (len != 0) {
                    s16 *table = quant[*q++];

                    for (k = 0; k < 64; k++) {
                        table[data_020502b8[k]] = *q++;
                    }
                    len -= 65;
                }
                break;
            case 0xc4:
                q = data_020c1820;
                len = q[1] + (q[0] << 8);
                q += 2;
                len -= 2;
                while (len != 0) {
                    c = *q++;
                    total = 0;
                    t = (c & 1) + (c >> 3);
                    for (l = 1; l <= 16; l++) {
                        bits[t][l] = *q++;
                        total += bits[t][l];
                    }
                    for (i = 0; i < total; i++) {
                        huffval[t][i] = *q++;
                    }
                    len -= total + 17;
                    k = 0;
                    for (l = 1; l <= 16; l++) {
                        n = bits[t][l];
                        while (n--) {
                            huffsize[k++] = l;
                        }
                    }
                    huffsize[k] = 0;
                    code = 0;
                    si = huffsize[0];
                    k = 0;
                    while (huffsize[k] != 0) {
                        while (huffsize[k] == si) {
                            huffcode[k++] = code++;
                        }
                        code <<= 1;
                        si++;
                    }
                    k = 0;
                    for (l = 1; l <= 16; l++) {
                        if (bits[t][l] == 0) {
                            maxcode[t][l] = -1;
                        } else {
                            valptr[t][l] = k;
                            mincode[t][l] = huffcode[k];
                            k += bits[t][l] - 1;
                            maxcode[t][l] = huffcode[k];
                            k++;
                        }
                    }
                }
                break;
            case 0xc0:
                height = (data_020c1820[3] << 8) + data_020c1820[4];
                if (height > 192 && !(target & 1)) {
                    goto end;
                }
                width = (data_020c1820[5] << 8) + data_020c1820[6];
                ncomp = data_020c1820[7];
                break;
        }
        data_020c1820 = func_0201e178();
    }
    out = func_0201c430(0x4000);
    if (width > 0x200) {
        OS_Terminate();
    }
    for (t = 0; t < (ncomp == 1 ? 1 : 2); t++) {
        for (k = 0; k < 64; k++) {
            quant[t][k] = (quant[t][k] * data_020502f8[k]) >> 12;
        }
    }
    mcu = func_0201c430(0x1c0);
    y = 0;
    for (i = 0; i < 0x100; i++) {
        buf[i] = 0;
        buf[i + 0x100] = i;
        buf[i + 0x200] = 0xff;
    }
    data_020c1820 += data_020c1820[1];
    data_020c1818 = 0;
    data_020c1824 = 0;
    dc[0] = dc[1] = dc[2] = 0;
    for (; y < height; y += 16) {
        for (x = 0; x < width; x += 16) {
            blk = mcu;
            for (c = 0; c < ncomp; c++) {
                for (n = 0; n < data_020502a4[c]; n++) {
                    func_02028980(block, c, maxcode[0], valptr[0], mincode[0], huffval[0], dc, data_020502b8);
                    func_0201e19c(block, quant[data_020502a8[c]], blk, buf + 0x180);
                    blk += 64;
                }
            }
            func_02028b8c(mcu, out + x, buf + 0x100);
        }
        func_0203e4a4();
        func_0201dcc0(out, target, y, width, height, lut);
    }
end:
    func_0201c4f0(&out);
    func_0201c4f0(&mcu);
    func_0201c4f0(&buf);
    func_0201c4f0(&block);
}
#pragma thumb on

void func_0201eb20(void) {
    u32 i;

    for (i = 0; i < 9; i++) {
        data_020c18bc.mUnk_0b8[i] = 0xff;
        *(vu8 *) (data_02050220[i] + (data_020502ac[i] / 8)) = 0;
    }
    data_020c18bc.mUnk_0c1 = 1;
    data_020c18bc.mUnk_49c[0] = data_020c18bc.mUnk_49c[1] = 0xffff;
}

void func_0201eb74(int type) {
    u32 i;

    if (type == 2) {
        data_020c18bc.mUnk_49c[0] = 0xffff;
    } else if (type == 6) {
        data_020c18bc.mUnk_49c[1] = 0xffff;
    }
    for (i = 0; i < 9; i++) {
        if (data_020c18bc.mUnk_0b8[i] != 0xff && type == data_02050478[data_020c18bc.mUnk_0b8[i]].mUnk_2) {
            *(vu8 *) (data_02050220[i] + (data_020502ac[i] / 8)) = 0;
            data_020c18bc.mUnk_0b8[i] = 0xff;
        }
    }
    data_020c18bc.mUnk_0c1 = 1;
}

void func_0201ebf4(int type, int size, int addr) {
    if (type == 10) {
        data_020c18bc.mUnk_070.mUnk_2c = 0;
    } else if (type == 11) {
        data_020c18bc.mUnk_040.mUnk_2c = 0;
    }
    func_0201eb74(type);
    func_0201ec2c(type, size, addr);
}

void func_0201ec2c(int type, u32 size, u32 addr) {
    const Unk_02050478 *rec;
    u32 used = 0;
    int i;
    int best;
    u32 j;
    u32 n;
    u32 k;
    u32 h;
    u32 bank;
    u32 kb;
    s32 bestRem;
    u32 bestN;
    u32 s0;
    u32 s1;
    u32 s2;
    u32 s3;
    u32 s4;
    s32 rem[5];
    u32 list[5];
    u32 addrs[5];

    if (addr == -1) {
        addr = data_02050270[type];
    }
    for (i = 0; i < 9; i++) {
        if (data_020c18bc.mUnk_0b8[i] != 0xff) {
            used |= 1 << i;
        }
    }
    j = 0;
    best = -1;
    for (; data_02050478[j].mUnk_1 != 0; j++) {
        if (type == data_02050478[j].mUnk_2 && !((1 << data_02050478[j].mUnk_0) & used) &&
            addr == data_02050478[j].mUnk_4 && data_02050478[j].mUnk_1 >= size) {
            if (best == -1 || data_02050478[j].mUnk_1 < data_02050478[best].mUnk_1) {
                best = j;
            }
        }
    }
    if (best != -1 && size == data_02050478[best].mUnk_1) {
        n = 1;
        list[1] = best;
    } else {
        rem[0] = size;
        n = 0;
        list[0] = 0xff;
        bestRem = 0x7fffffff;
        addrs[0] = addr;
        j = 0;
        bestN = 0;
        for (;;) {
            for (; (rec = &data_02050478[j])->mUnk_1 != 0; j++) {
                if (type == rec->mUnk_2) {
                    u32 bit = 1 << rec->mUnk_0;

                    if (!(used & bit) && rec->mUnk_4 == addrs[n]) {
                        rem[n + 1] = rem[n] - rec->mUnk_1;
                        addrs[n + 1] = rec->mUnk_4 + (rec->mUnk_1 << 10);
                        list[n + 1] = j;
                        n++;
                        used |= bit;
                        if (rem[n] < 0 && -rem[n] < bestRem) {
                            bestRem = -rem[n];
                            s0 = list[0];
                            bestN = n;
                            s1 = list[1];
                            s2 = list[2];
                            s3 = list[3];
                            s4 = list[4];
                        }
                        if (rem[n] == 0 || n >= 4) {
                            break;
                        }
                    }
                }
            }
            if (n == 0) {
                if (best != -1) {
                    list[1] = best;
                    n = 1;
                }
                break;
            }
            if (rem[n] == 0) {
                break;
            }
            j = list[n--];
            used &= ~(1 << data_02050478[j].mUnk_0);
            j++;
        }
    }
    if (n == 0) {
        list[0] = s0;
        n = bestN;
        list[1] = s1;
        list[2] = s2;
        list[3] = s3;
        list[4] = s4;
    }
    if (n == 0) {
        OS_Terminate();
    }
    while (n != 0) {
        k = list[n--];
        for (h = k; data_02050478[h].mUnk_2 != 0; h--) {
        }
        bank = data_02050478[k].mUnk_0;
        *(vu8 *) (data_02050220[bank] + data_020502ac[bank] / 8) = 0x80;
        kb = data_02050478[h].mUnk_1;
        MI_DmaFill32(3, (void *) data_02050478[h].mUnk_4, 0, kb << 10);
        *(vu8 *) (data_02050220[bank] + data_020502ac[bank] / 8) =
            (data_02050478[k].mUnk_9 << 3) + 0x80 + data_02050478[k].mUnk_8;
        data_020c18bc.mUnk_0b8[bank] = k;
        if (type == 10) {
            data_020c18bc.mUnk_070.mUnk_2c += kb >> 4;
        } else if (type == 11) {
            data_020c18bc.mUnk_040.mUnk_2c += kb >> 4;
        }
    }
    data_020c18bc.mUnk_0c1 = 1;
}

#pragma thumb off
u16 func_0201eea4(u8 (*pal)[3], int c0, int c1, int c2, int index) {
    u8 *p = pal[index];
    int d1 = p[1] - c1;
    int d2 = p[2] - c2;
    int d0 = pal[index][0] - c0;

    return d2 * d2 + d1 * d1 + d0 * d0;
}

void *func_0201eee4(int id, int target) {
    u8 *cube;
    u32 r;
    u32 g;
    u16 d;
    u32 b;
    u8 *p;
    u32 idx;
    u32 head;
    u32 tail;
    u8 *n;
    u8 *src;
    u8(*pal)[3];
    int cell;
    u32 i;
    u16 *dist;
    Unk_ColorQueue *queue;

    src = func_0202ded8(id);
    pal = func_0201c430(0x300);
    cube = func_0201c430(0x8000);
    dist = func_0201c430(0x10000);
    queue = func_0201c430(0x8000);
    tail = 0;
    head = 0;
    func_0201f5bc(src, 0x100, 0, target);
    MIi_CpuClear16(0xffff, dist, 0x10000);
    for (i = 0; i < 0x100; i++) {
        p = pal[i];
        p[0] = src[0] >> 3;
        p[1] = src[1] >> 3;
        p[2] = src[2] >> 3;
        src += 3;
        queue[tail].mUnk_0 = p[0];
        queue[tail].mUnk_1 = p[1];
        queue[tail].mUnk_2 = p[2];
        queue[tail++].mUnk_3 = i;
        cube[p[0] + ((p[2] << 10) + (p[1] << 5))] = i;
        dist[p[0] + ((p[2] << 10) + (p[1] << 5))] = 0;
    }
    while (head != tail) {
        r = queue[head].mUnk_0;
        g = queue[head].mUnk_1;
        b = queue[head].mUnk_2;
        idx = queue[head].mUnk_3;
        head = (head + 1) & 0x1fff;
        cell = r + ((b << 10) + (g << 5));
        // Offers the entry idx to each neighbouring cell: the cell takes it when it is nearer to the cell's colour
        // than the entry the cell has, and joins the queue.
        if (r != 0) {
            n = cube + cell - 1;
            if (idx != *n) {
                d = func_0201eea4(pal, r - 1, g, b, idx);
                if (dist[cell - 1] > d) {
                    *n = idx;
                    dist[cell - 1] = d;
                    queue[tail].mUnk_0 = r - 1;
                    queue[tail].mUnk_1 = g;
                    queue[tail].mUnk_2 = b;
                    queue[tail].mUnk_3 = idx;
                    tail = (tail + 1) & 0x1fff;
                }
            }
        }
        if (r < 31) {
            n = cube + cell + 1;
            if (idx != *n) {
                d = func_0201eea4(pal, r + 1, g, b, idx);
                if (dist[cell + 1] > d) {
                    *n = idx;
                    dist[cell + 1] = d;
                    queue[tail].mUnk_0 = r + 1;
                    queue[tail].mUnk_1 = g;
                    queue[tail].mUnk_2 = b;
                    queue[tail].mUnk_3 = idx;
                    tail = (tail + 1) & 0x1fff;
                }
            }
        }
        if (g != 0) {
            n = cube + cell - 32;
            if (idx != *n) {
                d = func_0201eea4(pal, r, g - 1, b, idx);
                if (dist[cell - 32] > d) {
                    *n = idx;
                    dist[cell - 32] = d;
                    queue[tail].mUnk_0 = r;
                    queue[tail].mUnk_1 = g - 1;
                    queue[tail].mUnk_2 = b;
                    queue[tail].mUnk_3 = idx;
                    tail = (tail + 1) & 0x1fff;
                }
            }
        }
        if (g < 31) {
            n = cube + cell + 32;
            if (idx != *n) {
                d = func_0201eea4(pal, r, g + 1, b, idx);
                if (dist[cell + 32] > d) {
                    *n = idx;
                    dist[cell + 32] = d;
                    queue[tail].mUnk_0 = r;
                    queue[tail].mUnk_1 = g + 1;
                    queue[tail].mUnk_2 = b;
                    queue[tail].mUnk_3 = idx;
                    tail = (tail + 1) & 0x1fff;
                }
            }
        }
        if (b != 0) {
            n = cube + cell - 0x400;
            if (idx != *n) {
                d = func_0201eea4(pal, r, g, b - 1, idx);
                if (dist[cell - 0x400] > d) {
                    *n = idx;
                    dist[cell - 0x400] = d;
                    queue[tail].mUnk_0 = r;
                    queue[tail].mUnk_1 = g;
                    queue[tail].mUnk_2 = b - 1;
                    queue[tail].mUnk_3 = idx;
                    tail = (tail + 1) & 0x1fff;
                }
            }
        }
        if (b < 31) {
            n = cube + cell + 0x400;
            if (idx != *n) {
                d = func_0201eea4(pal, r, g, b + 1, idx);
                if (dist[cell + 0x400] > d) {
                    *n = idx;
                    dist[cell + 0x400] = d;
                    queue[tail].mUnk_0 = r;
                    queue[tail].mUnk_1 = g;
                    queue[tail].mUnk_2 = b + 1;
                    queue[tail].mUnk_3 = idx;
                    tail = (tail + 1) & 0x1fff;
                }
            }
        }
    }
    func_0201c4f0(&pal);
    func_0201c4f0(&dist);
    func_0201c4f0(&queue);
    return cube;
}
#pragma thumb on

void func_0201f450(int a, int b, u32 c, u32 d) {
    u32 cls = 0;
    u32 size = 0x80;
    u32 rows;
    if (c > 0x80 || d > 0x80) {
        cls = 1;
        size = 0x100;
        if (c > 0x100 || d > 0x100) {
            cls = 2;
            size = 0x200;
            if (c > 0x200 || d > 0x100) {
                cls = 3;
            }
        }
    }
    rows = (size * d) >> 10;
    if (a & 1) {
        *(vu32 *) 0x04000000 =
            (data_020c267c.mUnk_018[0].mUnk_578[3] << 31) + (data_020c267c.mUnk_018[0].mUnk_568[0xf] << 20) + 0x11833;
        data_020c18bc.mUnk_474[0] &= ~4;
        data_020c18bc.mUnk_2d4 = 0;
        data_020c18bc.mUnk_2d8 = 0;
        data_020c18bc.mUnk_2e4 = 0x100;
        data_020c18bc.mUnk_2e6 = 0x100;
        *(vu16 *) 0x04000030 = 0x100;
        *(vu16 *) 0x04000036 = 0x100;
        *(vu16 *) 0x04000032 = 0;
        *(vu16 *) 0x04000034 = 0;
        *(vu32 *) 0x04000038 = 0;
        *(vu32 *) 0x0400003c = 0;
        switch (b) {
            case 0:
                func_0201ebf4(2, rows, -1);
                *(vu16 *) 0x0400000e = (cls << 14) + 0x80;
                break;
            case 1:
                func_0201ebf4(2, rows * 2, -1);
                *(vu16 *) 0x0400000e = (cls << 14) + 0x84;
                break;
            default:
                OS_Terminate();
                break;
        }
        MIi_CpuClear16(0, (void *) 0x05000000, 0x200);
    }
    if (a & 2) {
        *(vu32 *) 0x04001000 =
            (data_020c267c.mUnk_018[1].mUnk_578[3] << 31) + (data_020c267c.mUnk_018[1].mUnk_568[0xf] << 20) + 0x11813;
        data_020c18bc.mUnk_474[0] &= ~8;
        data_020c18bc.mUnk_2dc = 0;
        data_020c18bc.mUnk_2e0 = 0;
        *(vu16 *) 0x04001030 = 0x100;
        *(vu16 *) 0x04001036 = 0x100;
        *(vu16 *) 0x04001032 = 0;
        *(vu16 *) 0x04001034 = 0;
        *(vu32 *) 0x04001038 = 0;
        *(vu32 *) 0x0400103c = 0;
        switch (b) {
            case 0:
                func_0201ebf4(6, rows, -1);
                *(vu16 *) 0x0400100e = (cls << 14) + 0x80;
                break;
            case 1:
                func_0201ebf4(6, rows * 2, -1);
                *(vu16 *) 0x0400100e = (cls << 14) + 0x84;
                break;
        }
        MIi_CpuClear16(0, (void *) 0x05000400, 0x200);
    }
}

u8 *func_0201f5bc(u8 *palette, u32 count, int a, int target) {
    u8 *start = palette;
    u32 i;
    u16 *dst;
    u8 r;
    u8 g;
    u8 b;

    if (target & 1) {
        dst = (u16 *) 0x05000000 + a;
        for (i = 0; i < count; i++) {
            r = palette[0] >> 3;
            g = palette[1] >> 3;
            b = palette[2] >> 3;
            *dst = r + (g << 5) + (b << 10);
            dst++;
            palette += 3;
        }
    }
    if (target & 2) {
        palette = start;
        dst = (u16 *) 0x05000400 + a;
        for (i = 0; i < count; i++) {
            r = palette[0] >> 3;
            g = palette[1] >> 3;
            b = palette[2] >> 3;
            *dst = r + (g << 5) + (b << 10);
            dst++;
            palette += 3;
        }
    }
    return palette;
}

void func_0201f648(void) {
    vu16 *regs[2] = {(vu16 *) 0x0400006c, (vu16 *) 0x0400106c};
    int i;

    for (i = 0; i < 2; i++) {
        if (data_020c18bc.mUnk_490[i] == 1) {
            u32 level = *regs[i] & 0x1f;
            if (level != 0) {
                *regs[i] = 0x8000 | (level - 1);
            } else {
                *regs[i] = 0;
                data_020c18bc.mUnk_490[i] = 3;
            }
        } else if (data_020c18bc.mUnk_490[i] == 2) {
            u32 level = *regs[i] & 0x1f;
            if (level < 0x10) {
                *regs[i] = 0x8000 | (level + 1);
            } else {
                *regs[i] = 0x8011;
                data_020c18bc.mUnk_490[i] = 0;
            }
        }
    }
}

void func_0201f6d0(int a, int b, int c, int d, u16 e) {
    int phase = 0;
    int touched = 0;
    u32 frames = 0;
    u32 step = 0;

    func_0201f880(3);
    func_0201dde0(d, 3);
    func_0202c1c4();
    func_0202bdd8(b, c, a, 0, 1);
    func_02020854(1);
    while (func_0201f910(&frames, &touched, 0x3c, 0x78, 3) == 0) {
        if (phase == 0) {
            if ((data_020c18bc.mUnk_45c & 3) == 0) {
                step++;
                if (step == 10) {
                    phase = 1;
                }
            }
        } else {
            step++;
            if (step >= 0x19) {
                step = 10;
            }
        }
        func_0201bb30();
        func_0202cf08(e + step, 0x2c, 0x5d, 0, 1);
    }
    data_020c18bc.mUnk_46c |= 0x40000;
    func_0201f858(3);
    data_020c18bc.mUnk_46c &= ~0x40000;
}

int func_0201f794(int a, int b) {
    if ((a & 1) && data_020c18bc.mUnk_490[0] != b) {
        return 0;
    }
    if ((a & 2) && data_020c18bc.mUnk_490[1] != b) {
        return 0;
    }
    return 1;
}

int func_0201f7c0(int a, int b) {
    if (a & b) {
        return 1;
    }
    return 0;
}

void func_0201f7cc(int a) {
    if (func_0201f7c0(a, 1) == 1) {
        data_020c18bc.mUnk_490[0] = 2;
    }
    if (func_0201f7c0(a, 2) == 1) {
        data_020c18bc.mUnk_490[1] = 2;
    }
}

void func_0201f7f8(int a) {
    if (func_0201f7c0(a, 1) == 1) {
        if (data_020c18bc.mUnk_490[0] != 3) {
            data_020c18bc.mUnk_490[0] = 1;
        }
    }
    if (func_0201f7c0(a, 2) == 1) {
        if (data_020c18bc.mUnk_490[1] != 3) {
            data_020c18bc.mUnk_490[1] = 1;
        }
    }
}

void func_0201f830(int a) {
    func_0201f7f8(a);
    while (func_0201f794(a, 3) != 1) {
        func_0201bb30();
    }
}

void func_0201f858(int a) {
    while (func_0201f794(a, 0) != 1) {
        func_0201f7cc(a);
        func_0201bb30();
    }
}

void func_0201f880(int a) {
    if (func_0201f7c0(a, 1) == 1) {
        data_020c18bc.mUnk_490[0] = 0;
        *(vu16 *) 0x0400006c = 0x8011;
        func_0202ae34(1);
    }
    if (func_0201f7c0(a, 2) == 1) {
        data_020c18bc.mUnk_490[1] = 0;
        *(vu16 *) 0x0400106c = 0x8011;
        func_0202ae34(2);
    }
}

void func_0201f8d0(int a) {
    if (func_0201f7c0(a, 1) == 1) {
        data_020c18bc.mUnk_490[0] = 3;
        *(vu16 *) 0x0400006c = 0;
    }
    if (func_0201f7c0(a, 2) == 1) {
        data_020c18bc.mUnk_490[1] = 3;
        *(vu16 *) 0x0400106c = 0;
    }
}

int func_0201f910(u32 *frames, int *touched, u32 minFrames, u32 maxFrames, int layers) {
    if ((data_020c18bc.mUnk_10 & 9) || func_02020e3c(0, 0) == 1) {
        *touched = 1;
    }
    if (!(data_020c18bc.mUnk_10 & 9) && func_02020e3c(0, 0) == 0 && *frames > minFrames && *touched == 1) {
        return 1;
    }
    if (*frames >= maxFrames) {
        return 1;
    }
    if (func_0201f794(layers, 3) == 1) {
        (*frames)++;
    }
    return 0;
}

void func_0201f980(int a, int b, int c, int d, int e) {
    u32 frames = 0;
    int touched = 0;

    func_0201f858(e);
    func_0201dde0(a, e);
    func_0201f7f8(e);
    while (func_0201f910(&frames, &touched, b, c, e) == 0) {
        func_0201bb30();
    }
    if (d != 0) {
        if (d == 2) {
            func_0201f858(e);
        }
    } else {
        func_0201f880(e);
    }
    if (e & 1) {
        func_0201eb74(2);
    }
    if (e & 2) {
        func_0201eb74(6);
    }
}

#pragma thumb off
// Moves an entry to the end of the list.
void func_0201fa08(IndexNode16 *list, u32 index) {
    IndexNode16 *node = &list[index];
    list[node->mNext].mPrev = node->mPrev;
    list[node->mPrev].mNext = node->mNext;
    node->mNext = 0;
    node->mPrev = list[0].mPrev;
    list[0].mPrev = index;
    list[node->mPrev].mNext = index;
}
#pragma thumb on

#pragma thumb off
// Moves an entry to the end of the list.
void func_0201fa5c(IndexNode *list, u32 index) {
    IndexNode *node = &list[index];
    list[node->mNext].mPrev = node->mPrev;
    list[node->mPrev].mNext = node->mNext;
    node->mNext = 0;
    node->mPrev = list[0].mPrev;
    list[0].mPrev = index;
    list[node->mPrev].mNext = index;
}
#pragma thumb on
