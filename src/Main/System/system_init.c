#include <nitro/os.h>
#include <nitro/fs.h>
#include <nitro/card.h>
#include <nitro/gx.h>
#include <nitro/mi.h>
#include <nitro/mic.h>
#include <nitro/pm.h>
#include <nitro/rtc.h>
#include <nitro/snd.h>
#include <nitro/tp.h>
#include <Main/Input/touch_buttons.h>
#include <Main/Input/mic_input.h>
#include <Main/System/random_angle.h>
#include <Main/System/game_math.h>
#include <nitro/fx.h>
#include <nitro/wm.h>
#include <Main/Unknown/arm_system.h>
#include <Main/clib.h>
#include <nitro/math.h>
#include <Main/Save/save_blocks.h>
#include <Main/Save/backup_access.h>
#include <Main/System/bg_image.h>
#include <Main/System/screen_fade.h>
#include <Main/System/file_stream.h>
#include <Main/System/system_init.h>
#include <Main/Map/map_entries.h>
#include <Main/Graphics/model.h>
#include <Main/Graphics/text_draw.h>
#include <Main/System/engine_asm.h>
#include <Main/Graphics/texture_blocks.h>
#include <Main/Sound/wave_player.h>
#include <Main/Video/stream_audio.h>
#include <Main/System/file_loader.h>
#include <Main/System/align_util.h>
#include <Main/Sound/tracker_player.h>
#include <Main/Link/comm_link.h>
#include <Main/Link/wireless_helper.h>

// JPEG reader state and the start-up code's arena start (defined below, with the JPEG tables).
extern u8 data_020c1818;
extern void *data_020c181c;
extern u8 *data_020c1820;
extern u32 data_020c1824;
extern PMCallbackInfo data_020c1828;

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
#define POWCNT1_SWAP_LCDS 0x8000
#define MATH_BUSY 0x8000

// Upper end of the range func_0201fb24 and func_0201fb5c move a value in.
#define LEVEL_FULL 0x4000

// Half and full turn of an angle.
#define ANGLE_HALF 0x8000
#define ANGLE_FULL 0x10000

// Store kinds accepted by func_0202002c: blocks of 0x200 bytes and blocks of 0x2000 bytes.
#define SAVE_STORE_SMALL 0x901
#define SAVE_STORE_LARGE 0xd01
#define SMALL_BLOCK_SIZE 0x200
#define LARGE_BLOCK_SIZE 0x2000
#define CRC_POLY 0xa001

// Results of func_02020140.
#define BLOCK_CRC_OK 1
#define BLOCK_ID_OK 2
#define BLOCK_INTACT 3

// Marks of a field in the ordering list built by func_02020354.
#define ORDER_END (-1)
#define ORDER_USED (-2)

#define SYSTEM_BACKUP_READY 0x4000

#define TOUCH_BUTTON_COUNT 0x80
// Flag bits of a button id: a polygon hit area, and "insert before the existing buttons".
#define TOUCH_SHAPE 0x8000
#define TOUCH_ADD_FRONT 0x10000
#define TOUCH_NONE 0xffff
#define TOUCH_DOWN 0x40000000
#define TOUCH_PRESSED 0x80000000
#define TOUCH_RELEASED 0x20000000
#define TOUCH_REPEAT 0x10000000
#define TOUCH_QUICK 0x08000000
#define TOUCH_QUICK_FRAMES 0xe
#define SYSTEM_TOUCH_LOCK 0x80000000
#define SYSTEM_TOUCH_QUIET 0x04000000

#define SAMPLE_SHIFT 12
#define POWER_SHIFT 22
#define MAX_SHIFT 9

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
    data_020c3e44.mUnk_2024.mUnk_50 = 0;
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
    data_020c267c.mUnk_018[0].mUnk_577 = 0;
    data_020c267c.mUnk_018[1].mUnk_577 = 0;
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
    data_020c3e44.mUnk_2024.mUnk_48 = 0;
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
            if (data_020c3e44.mUnk_2024.mUnk_2dc.mUnk_4a == 0 && !(data_020c18bc.mUnk_46c & 0x10000)) {
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
                if (data_020c1d60.mUnk_800.mUnk_100.mUnk_10 < 10) {
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
            data_020c1d60.mUnk_800.mUnk_100.mUnk_04 = func_02020bac(0xffff);
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
                if (data_020c648c.mUnk_184 == 1 && data_020c648c.mUnk_188 == 0) {
                    found = 1;
                    func_0202f2e8(data_020c648c.mUnk_18c);
                }
                func_02043e88(0xc, 0, 0);
                if (found == 1) {
                    func_0202f290(data_020c648c.mUnk_18c, data_020c648c.mUnk_18c->mName, data_020c648c.mUnk_18c->mRate);
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
    int i = 0;
    u16 repeat = 0;

    held = keys;
    data_020c18bc.mUnk_016 |= 0xffff ^ keys;
    held &= data_020c18bc.mUnk_016;
    data_020c18bc.mUnk_12 = held & (held ^ data_020c18bc.mUnk_10);
    data_020c18bc.mUnk_014 = data_020c18bc.mUnk_10 & (held ^ data_020c18bc.mUnk_10);
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
    data_020c18bc.mUnk_01a = repeat & (repeat ^ data_020c18bc.mUnk_018);
    data_020c18bc.mUnk_01c = data_020c18bc.mUnk_018 & (repeat ^ data_020c18bc.mUnk_018);
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
    u32 lastDst;
    u32 part;
    u32 saved;
    u32 count;
    u8 *src;
    u8 *tmp;
    u32 hdr[4];
    u8 *tex;
    u32 n;

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
        if (data_020c18bc.mUnk_03c[i].mUnk_08 == 0) {
            continue;
        }
        addr = data_020c18bc.mUnk_03c[i].mUnk_04;
        if ((addr >> 28) == 11 || (addr >> 28) == 10 || (addr >= 0x06000000 && addr < 0x06400000)) {
            if ((flags & 2) && !busy) {
                list[count] = &data_020c18bc.mUnk_03c[i];
                count++;
            }
        } else {
            if ((flags & 1) && !busy) {
                list[count] = &data_020c18bc.mUnk_03c[i];
                count++;
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
                u32 k;

                for (k = 0; k < data_020c18bc.mUnk_070.mUnk_2e; k++) {
                    *data_020c18bc.mUnk_070.mUnk_00[k] = data_020c18bc.mUnk_070.mUnk_28[k];
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
    (void) (size <= 0xfb800 || (OS_Terminate(), 0));
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
        if (data_020c18bc.mUnk_250.mUnk_08 == remaining) {
            if (state-- == 0) {
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
                if (func_02034cf0(cmd + 6) <= data_020c18bc.mUnk_250.mUnk_00 + data_020c18bc.mUnk_250.mUnk_04) {
                } else {
                    goto restart;
                }
            }
        } else {
            remaining = data_020c18bc.mUnk_250.mUnk_08;
            state = 5;
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

// Width and height of a bitmap background for each screen size (control register bits 14-15).
const u16 data_02050200[4] = {0x80, 0x100, 0x200, 0x200};
const u16 data_02050208[4] = {0x80, 0x100, 0x100, 0x200};

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

void func_0201ddec(int id, int target, int x, u16 flags) {
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

void func_0201de48(u8 *data, int target, int x, u16 flags) {
    void *tmp = NULL;
    void *rgb = NULL;
    u32 w;
    u32 h;
    u32 magic;
    u32 count;
    u32 n;
    u32 i;
    u32 j;
    int indexed = 0;
    u16 *top;
    u32 mode;
    u8 *pix;
    u32 h2;
    u32 size;
    u8 *src;
    u8 *pal;
    u16 *out;
    u16 *bottom;
    u16 color;

    magic = *(u32 *) data;
    if (magic == 0xe0ffd8ff) {
        u32 height;

        func_0201e518(data, &w, &h);
        data_020c18bc.mUnk_2cc = w;
        data_020c18bc.mUnk_2ce = h;
        data_020c18bc.mUnk_2d0 = 0x8000;
        height = h;
        if ((target & 3) == 3 && h >= 0x180) {
            height >>= 1;
        }
        if (x == 0) {
            if (!(flags & 2)) {
                func_0201f450(target, 1, w, height);
            }
        } else {
            if (!(flags & 2)) {
                func_0201f450(target, 0, w, height);
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
                size = w * h2;
            } else {
                pal = data + 0x20;
                src = pal + count * 3;
                data_020c18bc.mUnk_2d0 = 0x8000;
                if (!(flags & 2)) {
                    func_0201f450(target, 1, w, h2);
                }
                DC_StoreRange(data, w * h + 0x320);
                size = w * h2 * 2;
                rgb = func_0201c430(w * h * 2);
                out = rgb;
                for (n = w * h; n != 0; n--) {
                    u8 *c;
                    u16 *p = out++;

                    c = pal + *src++ * 3;
                    color = (c[0] >> 3) | ((c[1] >> 3) << 5) | ((c[2] >> 3) << 10);
                    *p = color + 0x8000;
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
                for (i = w * h; i != 0; i--) {
                    *out++ = (u16) ((pal[0] >> 3) | ((pal[1] >> 3) << 5) | ((pal[2] >> 3) << 10)) + 0x8000;
                    pal += 3;
                }
            } else {
                bottom = top + w * (h - 1);
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
            size = w * h2 * 2;
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
// data_020c1828 the sleep callback node the start-up code keeps.
u8 data_020c1818;
void *data_020c181c;
u8 *data_020c1820;
u32 data_020c1824;
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

// Low and high bound of each of the four microphone bands. Defined here, after data_020502a8: the compiler sorts
// the file's constant objects by size and orders equal sizes by the definition list, and this place gives the
// original .rodata layout (with the microphone code or in address order, data_02050200 or data_020502a4 move).
const u16 data_02050210[8] = {90, 150, 150, 200, 200, 300, 300, 400};

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

// Hash steps of the graphics pools and the file cache: 1 and the odd primes up to 719.
const u16 data_02050378[128] = {
    1,   3,   5,   7,   11,  13,  17,  19,  23,  29,  31,  37,  41,  43,  47,  53,  59,  61,  67,  71,  73,  79,
    83,  89,  97,  101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193,
    197, 199, 211, 223, 227, 229, 233, 239, 241, 251, 257, 263, 269, 271, 277, 281, 283, 293, 307, 311, 313, 317,
    331, 337, 347, 349, 353, 359, 367, 373, 379, 383, 389, 397, 401, 409, 419, 421, 431, 433, 439, 443, 449, 457,
    461, 463, 467, 479, 487, 491, 499, 503, 509, 521, 523, 541, 547, 557, 563, 569, 571, 577, 587, 593, 599, 601,
    607, 613, 617, 619, 631, 641, 643, 647, 653, 659, 661, 673, 677, 683, 691, 701, 709, 719,
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
    huffsize = func_0201c430(0x300);
    buf = huffsize;
    huffcode = (u16 *) (buf + 0x100);
    block = func_0201c430(0x80);
    while (data_020c1820[-1] != 0xda) {
        switch (data_020c1820[-1]) {
            case 0xdb:
                q = data_020c1820;
                total = q[1] + (q[0] << 8);
                q += 2;
                total -= 2;
                while (total != 0) {
                    t = *q;
                    q++;
                    for (i = 0; i < 64; i++) {
                        quant[t][data_020502b8[i]] = *q++;
                    }
                    total -= 65;
                }
                break;
            case 0xc4:
                blk = data_020c1820;
                x = blk[1] + (blk[0] << 8);
                blk += 2;
                x -= 2;
                while (x != 0) {
                    si = *blk++;
                    total = 0;
                    t = (si & 1) + (si >> 3);
                    for (l = 1; l <= 16; l++) {
                        bits[t][l] = *blk++;
                        total += bits[t][l];
                    }
                    for (n = 0; n < total; n++) {
                        huffval[t][n] = *blk++;
                    }
                    x -= total + 17;
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
    for (i = 0; i < 0x100; i++) {
        buf[i] = 0;
        buf[i + 0x100] = i;
        buf[i + 0x200] = 0xff;
    }
    data_020c1820 += data_020c1820[1];
    data_020c1818 = 0;
    data_020c1824 = 0;
    dc[0] = dc[1] = dc[2] = 0;
    for (y = 0; y < height; y += 16) {
        for (x = 0; x < width; x += 16) {
            u8 *dst = mcu;

            for (c = 0; c < ncomp; c++) {
                for (n = 0; n < data_020502a4[c]; n++) {
                    func_02028980(block, c, maxcode[0], valptr[0], mincode[0], huffval[0], dc, data_020502b8);
                    func_0201e19c(block, quant[data_020502a8[c]], dst, buf + 0x180);
                    dst += 64;
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
    int i;
    const Unk_02050478 *rec;
    u32 bank;
    u32 n;
    s32 used = 0;
    u32 h;
    int cur;
    u32 best;
    u32 kb;
    s32 bestRem;
    u32 bestN;
    u32 s0;
    s32 rem[5];
    u32 s1;
    int s2;
    int s3;
    u32 k;
    u32 list[5];
    u32 s4;
    u32 addrs[5];

    if (addr == -1) {
        addr = data_02050270[type];
    }
    for (i = 0; i < 9; i++) {
        if (data_020c18bc.mUnk_0b8[i] != 0xff) {
            used |= 1 << i;
        }
    }
    i = 0;
    best = -1;
    for (; data_02050478[i].mUnk_1 != 0; i++) {
        if (type == data_02050478[i].mUnk_2 && !((1 << data_02050478[i].mUnk_0) & used) &&
            addr == data_02050478[i].mUnk_4 && data_02050478[i].mUnk_1 >= size) {
            if (best == -1 || data_02050478[i].mUnk_1 < data_02050478[best].mUnk_1) {
                best = i;
            }
        }
    }
    if (best != -1 && size == data_02050478[best].mUnk_1) {
        n = 1;
        list[1] = best;
    } else {
        addrs[0] = addr;
        rem[0] = size;
        list[0] = 0xff;
        n = 0;
        k = 0;
        bestRem = 0x7fffffff;
        bestN = 0;
        for (;;) {
            for (; rec = &data_02050478[k], data_02050478[k].mUnk_1 != 0; k++) {
                if (type == rec->mUnk_2) {
                    u32 bit = 1 << rec->mUnk_0;

                    if (!(used & bit) && rec->mUnk_4 == addrs[n]) {
                        cur = rem[n];
                        rem[n + 1] = cur - rec->mUnk_1;
                        addrs[n + 1] = rec->mUnk_4 + (rec->mUnk_1 << 10);
                        list[n + 1] = k;
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
            k = list[n--];
            used &= ~(1 << data_02050478[k].mUnk_0);
            k++;
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
    int d2 = pal[index][2] - c2;
    int d1 = pal[index][1] - c1;
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
    u8 *n1;
    u8 *n2;
    u8 *n3;
    u8 *n4;
    u8 *n5;
    u8 *n6;
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
            n1 = cube + cell - 1;
            if (idx != *n1) {
                d = func_0201eea4(pal, r - 1, g, b, idx);
                if (dist[cell - 1] > d) {
                    *n1 = idx;
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
            n2 = cube + cell;
            if (idx != n2[1]) {
                d = func_0201eea4(pal, r + 1, g, b, idx);
                if (dist[cell + 1] > d) {
                    n2[1] = idx;
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
            n3 = cube + cell - 32;
            if (idx != *n3) {
                d = func_0201eea4(pal, r, g - 1, b, idx);
                if (dist[cell - 32] > d) {
                    *n3 = idx;
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
            n4 = cube + cell;
            if (idx != n4[32]) {
                d = func_0201eea4(pal, r, g + 1, b, idx);
                if (dist[cell + 32] > d) {
                    n4[32] = idx;
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
            n5 = cube + cell - 0x400;
            if (idx != *n5) {
                d = func_0201eea4(pal, r, g, b - 1, idx);
                if (dist[cell - 0x400] > d) {
                    *n5 = idx;
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
            n6 = cube + cell;
            if (idx != n6[0x400]) {
                d = func_0201eea4(pal, r, g, b + 1, idx);
                if (dist[cell + 0x400] > d) {
                    n6[0x400] = idx;
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
    u32 size = data_02050200[0];
    u32 rows;
    if (c > data_02050200[0] || d > data_02050208[0]) {
        cls = 1;
        size = data_02050200[1];
        if (c > data_02050200[1] || d > data_02050208[1]) {
            cls = 2;
            size = data_02050200[2];
            if (c > data_02050200[2] || d > data_02050208[2]) {
                cls = 3;
            }
        }
    }
    rows = (size * d) >> 10;
    if (a & 1) {
        *(vu32 *) 0x04000000 =
            (data_020c267c.mUnk_018[0].mUnk_57b << 31) + (data_020c267c.mUnk_018[0].mUnk_577 << 20) + 0x11833;
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
            (data_020c267c.mUnk_018[1].mUnk_57b << 31) + (data_020c267c.mUnk_018[1].mUnk_577 << 20) + 0x11813;
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

// The unused array's initializer is what sits in .rodata just before the register table (01 01 00 00).
void func_0201f648(void) {
    u8 unused[2] = {1, 1};
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

void func_0201f6d0(int a, int b, int c, u16 d, u16 e) {
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

// Quadratic Bezier with the weights in 1/1024 steps: a at t = 0, b at t = 1024, c as the control point.
int func_0201fab0(int a, int b, int c, int t) {
    int u = 1024 - t;
    int x = a * u + c * 2 * t;
    int y = u * x + b * t * t;
    return y / 1024 / 1024;
}

u32 func_0201fae4(int x, int y) {
    return x * x + y * y;
}

u32 func_0201faf0(int x, int y) {
    return func_0201fccc(func_0201fae4(x, y));
}

s64 func_0201fafc(int x, int y) {
    return (s64) x * x + (s64) y * y;
}

// Moves *value toward 0 (speed < 0) or LEVEL_FULL (speed > 0) by 1/|speed| of the distance when it is farther than
// 2 * |speed| away; otherwise it lands on the step directly.
void func_0201fb24(u16 *value, int speed) {
    int target;
    int diff;
    if (speed > 0) {
        target = LEVEL_FULL;
    } else {
        target = 0;
        speed = -speed;
    }
    diff = target - *value;
    if (abs(diff) > speed * 2) {
        diff = diff / speed;
    }
    *value += diff;
}

void func_0201fb5c(u16 *p, int add) {
    if (*p < LEVEL_FULL) {
        int sum = *p + add;
        if (sum < LEVEL_FULL) {
            *p = sum;
        } else {
            *p = LEVEL_FULL;
        }
    }
}

// Linear map of a from [0, b] to [c, d].
int func_0201fb74(int a, int b, int c, int d) {
    int r;
    int bitsRange;
    int bitsA;
    if (a == 0) {
        return c;
    }
    if (a == b) {
        return d;
    }
    bitsRange = 32 - func_02028250(abs(d - c));
    bitsA = 32 - func_02028250(a);
    if (bitsRange + bitsA > 30) {
        r = (s64) a * ((s64) d - c) / b;
    } else {
        r = func_0201fca8((d - c) * a, b);
    }
    return c + r;
}

// Linear map of v from [a, b] to [c, d].
int func_0201fbe4(int v, int a, int b, int c, int d) {
    if (a == b) {
        OS_Terminate();
    }
    return c + func_0201fca8((v - a) * (d - c), b - a);
}

char *func_0201fc08(int value, char *buf) {
    return func_0201fc14(value, buf, 0);
}

// Writes value in decimal to buf, with separator after the digit of each thousands place unless it is 0.
char *func_0201fc14(int value, char *buf, char separator) {
    int pow10[11] = {1000000000, 100000000, 10000000, 1000000, 100000, 10000, 1000, 100, 10, 1, 0};
    char *start = buf;
    int *p = pow10;
    u32 v = value;
    if (value < 0) {
        *buf++ = '-';
        v = -value;
    }
    while (*p > v) {
        p++;
    }
    if (*p == 0) {
        *start = '0';
        return start + 1;
    }
    do {
        int c = '0';
        while (v >= *p) {
            v -= *p;
            c++;
        }
        *buf++ = c;
        if (separator != 0 && (*p == 1000 || *p == 1000000 || *p == 1000000000)) {
            *buf++ = separator;
        }
        p++;
    } while (*p != 0);
    return buf;
}

// a / b rounded to nearest, saturated to 0x0fffffff (negated when a < 0) when b is 0.
int func_0201fca8(int a, int b) {
    int r;
    if (b == 0) {
        return 0x0fffffff ^ (a >> 31);
    }
    r = func_0201fd40(a * 2, b);
    return (r + (r >> 31) + 1) >> 1;
}

u32 func_0201fccc(u32 value) {
    *(volatile u16 *) 0x040002b0 = 0;
    *(volatile u32 *) 0x040002b8 = value;
    while (*(volatile u16 *) 0x040002b0 & MATH_BUSY) {
    }
    return *(volatile u32 *) 0x040002b4;
}

u32 func_0201fcec(u64 value) {
    *(volatile u16 *) 0x040002b0 = 1;
    *(volatile u64 *) 0x040002b8 = value;
    while (*(volatile u16 *) 0x040002b0 & MATH_BUSY) {
    }
    return *(volatile u32 *) 0x040002b4;
}

u32 func_0201fd10(u32 numerator, u32 denominator, u32 *remainder) {
    *(volatile u16 *) 0x04000280 = 0;
    *(volatile u32 *) 0x04000290 = numerator;
    *(volatile u64 *) 0x04000298 = denominator;
    while (*(volatile u16 *) 0x04000280 & MATH_BUSY) {
    }
    *remainder = *(volatile u32 *) 0x040002a8;
    return *(volatile u32 *) 0x040002a0;
}

s32 func_0201fd40(s32 numerator, s32 denominator) {
    *(volatile u16 *) 0x04000280 = 0;
    *(volatile u32 *) 0x04000290 = numerator;
    *(volatile u64 *) 0x04000298 = (u32) denominator;
    while (*(volatile u16 *) 0x04000280 & MATH_BUSY) {
    }
    return *(volatile u32 *) 0x040002a0;
}

// Advances the generator state: add a constant, rotate right by 3.
u32 func_0201fd68(void) {
    u32 s = data_020c18bc.mUnk_478 + 0x5cfae23d;
    s = (s >> 3) + (s << 29);
    data_020c18bc.mUnk_478 = s;
    return s;
}

// Random value between lo and hi. When lo == hi nothing is computed and lo is still in r0; adding `return lo;`
// changes the code.
u32 func_0201fd84(u32 lo, u32 hi) {
    if (lo != hi) {
        return func_0201fb74((u16) func_0201fd68(), ANGLE_FULL, lo, hi);
    }
}

// Mixes the current scanline into the generator state.
void func_0201fda4(void) {
    data_020c18bc.mUnk_478 += *(vu16 *) 0x04000006;
}

// Reads the bits lo..hi (inclusive, bit 0 = lowest bit of the first byte) of a byte array.
u32 func_0201fdbc(const void *data, u32 lo, u32 hi) {
    const u8 *base = (const u8 *) data;
    u32 result = 0;
    u32 shift = 0;
    const u8 *last = base + hi / 8;
    u32 pos = lo;
    while (pos <= hi) {
        const u8 *p = base + pos / 8;
        u32 n;
        if (p != last) {
            n = 8 - (pos & 7);
        } else {
            n = hi - pos + 1;
        }
        result |= (((int) *p >> (pos & 7)) & (u8) ((1 << n) - 1)) << shift;
        pos += n;
        shift += n;
    }
    return result;
}

// Shortest distance between two angles.
u32 func_0201fe14(u32 a, u32 b) {
    int d;
    if (a < b) {
        u32 t = a;
        a = b;
        b = t;
    }
    d = a - b;
    if (d > ANGLE_HALF) {
        b += ANGLE_FULL;
        d = b - a;
    }
    return d;
}

// Shortest signed distance from b to a.
int func_0201fe34(u32 a, u32 b) {
    int d;
    int sign;
    if (a < b) {
        u32 t = a;
        a = b;
        b = t;
        sign = 1;
    } else {
        sign = -1;
    }
    d = a - b;
    if (d > ANGLE_HALF) {
        b += ANGLE_FULL;
        d = b - a;
        sign = -sign;
    }
    if (sign > 0) {
        return d;
    }
    return -d;
}

// 1 when the short way from b to a runs upward.
int func_0201fe6c(u16 a, u16 b) {
    u32 d;
    int up;
    if (b < a) {
        d = a - b;
        up = 1;
    } else {
        d = b - a;
        up = 0;
    }
    if (d > ANGLE_FULL - d) {
        up ^= 1;
    }
    return up;
}

// Moves b toward a by half the shortest distance.
u32 func_0201fe8c(u16 a, u16 b) {
    u32 d = func_0201fe14(a, b);
    int up = func_0201fe6c(a, b);
    return (u16) (b + (up * 2 - 1) * (d >> 1));
}

u32 func_0201feb0(s32 x, s32 y) {
    return FX_Atan2Idx(x, -y);
}

// Cross product of the vectors b->a and b->c.
int func_0201febc(int ax, int ay, int bx, int by, int cx, int cy) {
    return (ax - bx) * (cy - by) - (cx - bx) * (ay - by);
}

void func_0201fed4(int a) {
    u32 elapsed;
    u32 flag;
    u32 limit;
    if (data_020c18bc.mUnk_32e == 0) {
        return;
    }
    if (data_020c18bc.mUnk_32e == 4 && (a & 1) == 0) {
        return;
    }
    elapsed = data_020c18bc.mUnk_45c - data_020c18bc.mUnk_324;
    flag = 0;
    limit = data_020c18bc.mUnk_457;
    if (limit == 0) {
        limit = 180;
    }
    while (func_0201f910(&elapsed, &flag, 60, limit, 3) == 0) {
        func_0201bb30();
    }
    data_020c18bc.mUnk_450 |= 4;
    while (data_020c18bc.mUnk_32e == 3 || data_020c18bc.mUnk_32e == 10 || data_020c18bc.mUnk_32e == 4) {
        func_0201bb30();
    }
    data_020c18bc.mUnk_32f = data_020c18bc.mUnk_32e;
    data_020c18bc.mUnk_32e = 0;
    data_020c18bc.mUnk_46c &= 0xfff7ffff;
}

int func_0201ff80(u32 id) {
    return data_020c18bc.mUnk_328 / func_02021e44(id, data_020c18bc.mUnk_32c + 1);
}

int func_0201ffa0(void) {
    int d = data_020c18bc.mUnk_32c - data_020c18bc.mUnk_32a;
    if (!(data_020c18bc.mUnk_32a < data_020c18bc.mUnk_32c)) {
        OS_Terminate();
    }
    return d + 1;
}

void *func_0201ffbc(void) {
    return func_0201c430(func_0201ffa0() * 4);
}

void func_0201ffcc(void *p) {
    func_0201c4f0(p);
}

// Returns the 16-bit tag of a block: the table bytes at 2 * index and 2 * index + 1, each taken modulo the table's
// length.
u16 func_0201ffd4(int id, u32 index) {
    const u8 *table = func_02021e18(id, data_020c18bc.mUnk_32c + 2);
    u32 count = func_02021f0c(id, data_020c18bc.mUnk_32c + 2, 0);
    if (count < 2) {
        OS_Terminate();
    }
    return (table[(index * 2) % count] << 8) + table[(index * 2 + 1) % count];
}

// Sets up the block store for set a: kind, first and last field, status callback and the value for mUnk_454
// (-1 leaves it). Reads every block and rewrites the ones that do not read back intact, then waits for the store.
void func_0202002c(u32 a, int b, u16 c, int d, void (*fn)(u32), int e) {
    u32 count = func_02021e44(a, d + 1);
    u32 r;
    u32 i;
    if (b != SAVE_STORE_SMALL && b != SAVE_STORE_LARGE) {
        OS_Terminate();
    }
    data_020c18bc.mUnk_31c = fn;
    data_020c18bc.mUnk_32e = 0;
    data_020c18bc.mUnk_32f = data_020c18bc.mUnk_32e;
    data_020c18bc.mUnk_32a = c;
    data_020c18bc.mUnk_32c = d;
    data_020c18bc.mUnk_324 = 0;
    if (e != -1) {
        data_020c18bc.mUnk_454 = e;
        fn(4);
    }
    func_02020ee0(b);
    if (b == SAVE_STORE_SMALL) {
        data_020c18bc.mUnk_328 = SMALL_BLOCK_SIZE;
    } else {
        data_020c18bc.mUnk_328 = LARGE_BLOCK_SIZE;
    }
    // Volatile read of the frame counter; the value is not used.
    data_020c18bc.mUnk_45c;
    for (i = 0; i < count; i++) {
        r = func_02020140(i, 0, a);
        if (r == BLOCK_ID_OK) {
            data_020c18bc.mUnk_31c(1);
        }
        if (r != BLOCK_INTACT) {
            if (data_020c18bc.mUnk_328 > SMALL_BLOCK_SIZE && r != BLOCK_ID_OK) {
                data_020c18bc.mUnk_31c(6);
            }
            func_02020224(i, a);
        }
    }
    func_0201fed4(1);
    if (data_020c18bc.mUnk_32f == 1) {
        data_020c18bc.mUnk_31c(2);
        while (func_02020890() == 0) {
            func_0201bb30();
        }
        data_020c18bc.mUnk_46c &= ~0xc0000;
    }
    data_020c18bc.mUnk_450 |= 4;
}

// Reads block index of set c. Bit 0 of the result: the checksum matches (the record is then unpacked into buf when
// buf is not null); bit 1: the stored tag matches the block's tag.
u32 func_02020140(u32 index, void *buf, int c) {
    u16 table[256];
    u16 crc;
    int size = func_0201ff80(c);
    u8 *data = func_0201c430(size);
    u32 result = 0;
    int check = func_0201ffd4(c, index);
    if (index >= func_02021e44(c, data_020c18bc.mUnk_32c + 1)) {
        OS_Terminate();
    }
    if (func_02020f9c(data, index * size, size) == 0) {
        if (data_020c18bc.mUnk_320 != 0) {
            memcpy(data, data_020c18bc.mUnk_320 + index * size, size);
        } else {
            data_020c18bc.mUnk_31c(10);
        }
    }
    func_02048780(table, CRC_POLY);
    crc = func_02048804(table, data + 4, size - 4);
    if (crc == data[0] + (data[1] << 8)) {
        if (buf != 0) {
            func_0202062c(buf, (u32 *) (data + 4), c);
        }
        result |= BLOCK_CRC_OK;
    }
    if (data[2] == (check & 0xff) && data[3] == (check >> 8)) {
        result |= BLOCK_ID_OK;
    }
    if (data_020c18bc.mUnk_32e != 1) {
        func_0201fed4(0);
    }
    func_0201c4f0(&data);
    return result;
}

// Writes a freshly allocated record to block index of set c and sets the block's bit in mUnk_458.
void func_02020224(u32 index, int c) {
    void *buf = func_0201c430(func_0201ffa0() * 4);
    func_02020258(index, buf, c);
    func_0201c4f0(&buf);
    data_020c18bc.mUnk_458[0] |= 1 << index;
}

// Packs the record in buf into block index of set c with its checksum and tag and writes the block.
void func_02020258(u32 index, void *buf, int c) {
    u16 table[256];
    u16 crc;
    int size = func_0201ff80(c);
    u8 *data = func_0201c430(size);
    int payloadWritten = 1;
    int check = func_0201ffd4(c, index);
    int payload;
    u32 offset;
    int headWritten;
    if (index >= func_02021e44(c, data_020c18bc.mUnk_32c + 1)) {
        OS_Terminate();
    }
    memset(data, 0, size);
    func_0202043c(buf, (u32 *) (data + 4), c);
    func_02048780(table, CRC_POLY);
    payload = size - 4;
    crc = func_02048804(table, data + 4, payload);
    memcpy(data, &crc, 2);
    data[2] = check;
    data[3] = check >> 8;
    if (size > 4) {
        payloadWritten = func_02020f38(data + 4, index * size + 4, payload);
    }
    offset = index * size;
    headWritten = func_02020f38(data, offset, 4);
    if (payloadWritten == 0 || headWritten == 0) {
        if (data_020c18bc.mUnk_320 != 0) {
            memcpy(data_020c18bc.mUnk_320 + offset, data, size);
        } else {
            data_020c18bc.mUnk_31c(4);
        }
    }
    if (data_020c18bc.mUnk_32e != 1) {
        func_0201fed4(1);
    }
    func_0201c4f0(&data);
}

// Clears mUnk_456.
void func_02020348(void) {
    data_020c18bc.mUnk_456 = 0;
}

#pragma thumb off
// Fills order with 0..count-1, ends it with ORDER_END and sorts it by descending key (shell sort).
void func_02020354(u32 *order, const u32 *key, int count) {
    int gap;
    u32 t;
    int j;
    int i;
    int span;
    int a;
    for (i = 0; i < count; i++) {
        order[i] = i;
    }
    order[i] = ORDER_END;
    gap = (1 << (32 - func_02028250(count))) - 1;
    for (; gap > 0; gap = (gap - 1) / 2) {
        span = count - gap;
        for (a = 0; a < span; a++) {
            j = 0;
            if (key[order[a]] < key[order[a + gap]]) {
                do {
                    t = order[a + j];
                    order[a + j] = order[a + gap + j];
                    order[a + gap + j] = t;
                    j -= gap;
                } while (a + j >= 0 && key[order[a + j]] < key[order[a + gap + j]]);
            }
        }
    }
}

// Packs the field values into 32-bit words, mixed radix: each field's radix is its largest value plus one. Returns
// the number of bytes written to out.
int func_0202043c(const u32 *values, u32 *out, u32 id) {
    int fields;
    u32 *radix;
    u32 *order;
    int i;
    int outCount;
    u64 prod;
    u64 old;
    u32 acc;
    fields = func_0201ffa0();
    radix = func_0201c430(fields * 4);
    order = func_0201c430((fields + 1) * 4);
    for (i = 0; i < fields; i++) {
        radix[i] = func_02021e44(id, data_020c18bc.mUnk_32a + i) + 1;
    }
    func_02020354(order, radix, fields);
    outCount = 0;
    while (1) {
        i = 0;
        acc = 0;
        prod = 1;
        if (order[0] != ORDER_END) {
            do {
                if (order[i] != ORDER_USED) {
                    old = prod;
                    prod = prod * radix[order[i]];
                    if (prod - 1 > 0xffffffff) {
                        prod = old;
                    } else {
                        if (values[order[i]] >= radix[order[i]]) {
                            OS_Terminate();
                        }
                        acc = radix[order[i]] * acc + values[order[i]];
                        order[i] = ORDER_USED;
                        if (prod * 2 - 1 > 0xffffffff) {
                            break;
                        }
                    }
                }
                i++;
            } while (order[i] != ORDER_END);
        }
        if (prod == 1) {
            break;
        }
        out[outCount] = acc;
        outCount++;
    }
    func_0201c4f0(&radix);
    func_0201c4f0(&order);
    return outCount * 4;
}

// Unpacks the words written by func_0202043c back into the field values.
void func_0202062c(u32 *out, u32 *packed, u32 id) {
    int fields = func_0201ffa0();
    u32 *radix = func_0201c430(fields * 4);
    u32 *order = func_0201c430((fields + 1) * 4);
    u32 list[32];
    int i;
    int w;
    int n;
    u64 prod;
    for (i = 0; i < fields; i++) {
        radix[i] = func_02021e44(id, data_020c18bc.mUnk_32a + i) + 1;
    }
    func_02020354(order, radix, fields);
    for (w = 0;; w++) {
        i = 0;
        n = 0;
        prod = 1;
        if (order[0] != ORDER_END) {
            do {
                if (order[i] != ORDER_USED) {
                    u32 f = order[i];
                    u32 r = radix[f];
                    u64 old = prod;
                    prod = prod * r;
                    if (prod - 1 > 0xffffffff) {
                        prod = old;
                    } else {
                        order[i] = ORDER_USED;
                        list[n] = f;
                        n++;
                        if (prod * 2 - 1 > 0xffffffff) {
                            break;
                        }
                    }
                }
                i++;
            } while (order[i] != ORDER_END);
        }
        if (n == 0) {
            break;
        }
        while (n-- != 0) {
            u32 f = list[n];
            out[f] = packed[w] % radix[f];
            packed[w] = packed[w] / radix[f];
        }
    }
    func_0201c4f0(&radix);
    func_0201c4f0(&order);
}
#pragma thumb on

// Sets (a == 1) or clears the LCD swap bit of the power control register.
void func_02020854(int a) {
    if (a == 1) {
        *(vu16 *) 0x04000304 |= POWCNT1_SWAP_LCDS;
    } else {
        *(vu16 *) 0x04000304 &= ~POWCNT1_SWAP_LCDS;
    }
}

// Whether the display-swap bit of the power control register is set.
int func_02020878(void) {
    if (*(vu16 *) 0x04000304 & POWCNT1_SWAP_LCDS) {
        return 1;
    }
    return 0;
}

int func_02020890(void) {
    if (data_020c18bc.mUnk_490[0] == 3 && data_020c18bc.mUnk_490[1] == 3 &&
        ((data_020c18bc.mUnk_12 & 1) || data_020c18bc.mUnk_2ac_0 == 1)) {
        data_020c18bc.mUnk_12 &= ~1;
        data_020c18bc.mUnk_2ac_0 = 0;
        return 1;
    }
    return 0;
}

// Sets the sound ids and key mask of the newest button (or of the first one when ids are inserted in front).
void func_020208dc(int flags, u16 a, u16 b, u16 c) {
    int index;
    if ((flags & TOUCH_ADD_FRONT) == 0) {
        index = data_020c1d60.mUnk_800.mUnk_100.mUnk_08 - 1;
    } else {
        index = 0;
    }
    data_020c1d60.mUnk_000[index].mUnk_c = b;
    data_020c1d60.mUnk_000[index].mUnk_a = a;
    data_020c1d60.mUnk_000[index].mUnk_e = c;
}

void func_02020914(int a, int b, int c, int d, int e, int f, u16 g, u16 h, u16 i) {
    func_02020944(a, b, c, d, e, f, g, h);
    func_020208dc(a, g, h, i);
}

void func_02020944(int a, int b, int c, int d, int e, int f, u16 g, u16 h) {
    if (!(data_020c18bc.mUnk_46c & SYSTEM_TOUCH_LOCK)) {
        func_02020978(a, b, c, d, e, f);
        func_020208dc(a, g, h, 0);
    }
}

void func_02020978(int id, int x, int y, int a, int b, int c) {
    s32 rect[4];
    int index;
    int shifting;
    Unk_TouchButton *button;
    if (data_020c1d60.mUnk_800.mUnk_100.mUnk_08 >= TOUCH_BUTTON_COUNT) {
        OS_Terminate();
    }
    if (data_020c18bc.mUnk_46c & SYSTEM_TOUCH_LOCK) {
        return;
    }
    shifting = id & TOUCH_ADD_FRONT;
    if (shifting == 0) {
        index = data_020c1d60.mUnk_800.mUnk_100.mUnk_08;
    } else {
        index = data_020c1d60.mUnk_800.mUnk_100.mUnk_08++;
        while (index != 0) {
            data_020c1d60.mUnk_000[index] = data_020c1d60.mUnk_000[index - 1];
            index--;
        }
    }
    button = &data_020c1d60.mUnk_000[index];
    button->mUnk_c = 0;
    button->mUnk_a = 0;
    button->mUnk_e = 0;
    if (c == -1) {
        button->mUnk_2 = y;
        button->mUnk_6 = b;
        button->mUnk_0 = x;
        button->mUnk_4 = a;
    } else if (!(id & TOUCH_SHAPE)) {
        if (func_0202d828(c, 0, rect) == 0) {
            return;
        }
        button->mUnk_2 = y + rect[1];
        button->mUnk_6 = y + rect[3];
        button->mUnk_0 = x + rect[0];
        button->mUnk_4 = x + rect[2];
    } else {
        button->mUnk_0 = x;
        button->mUnk_2 = y;
        button->mUnk_6 = c;
    }
    if (data_020c1d60.mUnk_800.mUnk_100.mUnk_16 != 0) {
        int mode = func_02020878() == 1 ? 2 : 1;
        func_0202cf08(data_020c1d60.mUnk_800.mUnk_100.mUnk_16, button->mUnk_0, button->mUnk_2, 0, mode);
        func_0202cf08(data_020c1d60.mUnk_800.mUnk_100.mUnk_16, button->mUnk_4, button->mUnk_6, 0, mode);
    }
    button->mUnk_8 = id;
    if (shifting == 0) {
        data_020c1d60.mUnk_800.mUnk_100.mUnk_08++;
    }
}

int func_02020a90(int x, int y, int shape) {
    if (data_020c18bc.mUnk_2ac_1) {
        return func_02020ab8(x, y, shape, data_020c18bc.mUnk_2ae, data_020c18bc.mUnk_2af);
    }
    return 0;
}

typedef struct TouchPoint {
    s32 x;
    s32 y;
} TouchPoint;

// Tests whether the point (px, py) lies inside the polygon the shape table describes, placed at (x, y).
int func_02020ab8(int x, int y, int shape, int px, int py) {
    int found = 1;
    int next = 0;
    int inside;
    u32 i;
    TouchPoint points[16];
    s32 corner[2];
    u32 count;
    TouchPoint *p;
    while (next == 0 || found == 1) {
        count = 0;
        inside = 0;
        for (;;) {
            found = func_0202d7a0(shape, next++ + 2, corner);
            if (found != 1) {
                break;
            }
            if (count != 0 && points[0].x == x + corner[0] && points[0].y == y + corner[1]) {
                break;
            }
            points[count].x = x + corner[0];
            points[count++].y = y + corner[1];
        }
        if (count != 0 && count <= 2) {
            OS_Terminate();
        }
        if (count > 2) {
            points[count] = points[0];
            p = points;
            for (i = 0; i < count; i++) {
                inside = inside * 2 + ((func_0201febc(p[0].x, p[0].y, p[1].x, p[1].y, px, py) >> 31) & 1);
                p++;
            }
            if (inside == (1 << count) - 1) {
                return 1;
            }
        }
    }
    return 0;
}

void func_02020ba0(void) {
    data_020c1d60.mUnk_800.mUnk_100.mUnk_08 = 0;
}

// Polls the touch screen and the keys and returns the button state: the button id in the low half and the
// TOUCH_* flags above it.
u32 func_02020bac(u32 mask) {
    u32 result = data_020c1d60.mUnk_800.mUnk_100.mUnk_12;
    u32 wanted = result;
    int hit = -1;
    u8 pos[2];
    u32 i;
    if (mask != TOUCH_NONE && mask != result) {
        wanted = mask;
    }
    if (func_02020e3c(&pos[1], &pos[0]) == 1) {
        for (i = 0; i < data_020c1d60.mUnk_800.mUnk_100.mUnk_08; i++) {
            Unk_TouchButton *button = &data_020c1d60.mUnk_000[i];
            if (!(button->mUnk_8 & TOUCH_SHAPE)) {
                if (pos[0] >= button->mUnk_2 && pos[0] <= button->mUnk_6 && pos[1] >= button->mUnk_0 &&
                    pos[1] <= button->mUnk_4) {
                    hit = i;
                    data_020c1d60.mUnk_800.mUnk_100.mUnk_12 = data_020c1d60.mUnk_000[i].mUnk_8 & 0x7fff;
                    break;
                }
            } else if (func_02020a90(button->mUnk_0, button->mUnk_2, button->mUnk_6)) {
                hit = i;
                data_020c1d60.mUnk_800.mUnk_100.mUnk_12 = data_020c1d60.mUnk_000[i].mUnk_8 & 0x7fff;
                break;
            }
        }
        if (i == data_020c1d60.mUnk_800.mUnk_100.mUnk_08) {
            data_020c1d60.mUnk_800.mUnk_100.mUnk_12 = TOUCH_NONE;
        }
        result = data_020c1d60.mUnk_800.mUnk_100.mUnk_12 | TOUCH_DOWN;
        if (data_020c1d60.mUnk_800.mUnk_100.mUnk_14 == 0 ||
            (wanted != data_020c1d60.mUnk_800.mUnk_100.mUnk_12 && !(data_020c18bc.mUnk_46c & SYSTEM_TOUCH_QUIET))) {
            result |= TOUCH_PRESSED;
            if (wanted == data_020c1d60.mUnk_800.mUnk_100.mUnk_12) {
                result |= TOUCH_REPEAT;
                if (data_020c18bc.mUnk_45c - data_020c1d60.mUnk_800.mUnk_100.mUnk_0c < TOUCH_QUICK_FRAMES) {
                    result |= TOUCH_QUICK;
                }
            }
            if (data_020c1d60.mUnk_800.mUnk_100.mUnk_14 == 0) {
                data_020c1d60.mUnk_800.mUnk_100.mUnk_0c = data_020c18bc.mUnk_45c;
            }
        }
        data_020c1d60.mUnk_800.mUnk_100.mUnk_14 = 1;
    } else {
        if (data_020c1d60.mUnk_800.mUnk_100.mUnk_14 == 1) {
            result |= TOUCH_RELEASED;
            for (i = 0; i < data_020c1d60.mUnk_800.mUnk_100.mUnk_08; i++) {
                if ((u16) result == data_020c1d60.mUnk_000[i].mUnk_8) {
                    hit = i;
                    break;
                }
            }
        }
        data_020c1d60.mUnk_800.mUnk_100.mUnk_14 = 0;
    }
    if ((result & (TOUCH_DOWN | TOUCH_PRESSED | TOUCH_RELEASED)) == 0) {
        if ((u16) (data_020c18bc.mUnk_10 | data_020c18bc.mUnk_12 | data_020c18bc.mUnk_014) != 0) {
            for (i = 0; i < data_020c1d60.mUnk_800.mUnk_100.mUnk_08; i++) {
                u16 keys = data_020c1d60.mUnk_000[i].mUnk_e;
                if (keys != 0) {
                    if (data_020c18bc.mUnk_10 & keys) {
                        result = (result & 0xffff0000) | data_020c1d60.mUnk_000[i].mUnk_8 | TOUCH_DOWN;
                    }
                    if (data_020c18bc.mUnk_12 & keys) {
                        result = (result & 0xffff0000) | data_020c1d60.mUnk_000[i].mUnk_8 | TOUCH_PRESSED;
                    }
                    if (keys & data_020c18bc.mUnk_014) {
                        result = (result & 0xffff0000) | data_020c1d60.mUnk_000[i].mUnk_8 | TOUCH_RELEASED;
                    }
                }
            }
        }
    }
    data_020c1d60.mUnk_800.mUnk_100.mUnk_08 = 0;
    if (hit != -1 && (u16) result != TOUCH_NONE) {
        if (result & TOUCH_RELEASED) {
            if (data_020c1d60.mUnk_000[hit].mUnk_c != 0) {
                func_0202eb64(data_020c1d60.mUnk_000[hit].mUnk_c, 100, 0, 0x40, TOUCH_NONE, 0);
                if (data_020c1d60.mUnk_000[hit].mUnk_a != 0) {
                    func_0202edd0(data_020c1d60.mUnk_000[hit].mUnk_a);
                }
            }
        } else if (result & TOUCH_PRESSED) {
            if (data_020c1d60.mUnk_000[hit].mUnk_a != 0) {
                func_0202eb64(data_020c1d60.mUnk_000[hit].mUnk_a, 100, 0, 0x40, TOUCH_NONE, 0);
            }
        }
    }
    return result;
}

void func_02020df4(void) {
    data_020c1d60.mUnk_800.mUnk_100.mUnk_18 = 0;
    data_020c1d60.mUnk_800.mUnk_100.mUnk_10 = 0;
    MIi_CpuClear16(0xffff, data_020c1d60.mUnk_800.mUnk_000, sizeof(data_020c1d60.mUnk_800.mUnk_000));
}

void func_02020e18(u16 a, int b) {
    data_020c1d60.mUnk_800.mUnk_100.mUnk_12 = TOUCH_NONE;
    data_020c1d60.mUnk_800.mUnk_100.mUnk_08 = 0;
    data_020c1d60.mUnk_800.mUnk_100.mUnk_14 = 0;
    data_020c1d60.mUnk_800.mUnk_100.mUnk_0c = 0;
    data_020c1d60.mUnk_800.mUnk_100.mUnk_16 = a;
    data_020c1d60.mUnk_800.mUnk_100.mUnk_04 = TOUCH_NONE;
    func_02020df4();
}

int func_02020e3c(u8 *x, u8 *y) {
    if (data_020c18bc.mUnk_2ac_1 == 0) {
        if (x != 0) {
            *x = 0xff;
        }
        if (y != 0) {
            *y = 0xff;
        }
        if (data_020c1d60.mUnk_800.mUnk_100.mUnk_18 == 1) {
            data_020c1d60.mUnk_800.mUnk_100.mUnk_00[2] = data_020c18bc.mUnk_2ae;
            data_020c1d60.mUnk_800.mUnk_100.mUnk_00[3] = data_020c18bc.mUnk_2af;
            data_020c1d60.mUnk_800.mUnk_100.mUnk_18 = 2;
        } else if (data_020c1d60.mUnk_800.mUnk_100.mUnk_18 == 2) {
            data_020c1d60.mUnk_800.mUnk_100.mUnk_18 = 3;
        }
        return 0;
    }
    if (data_020c1d60.mUnk_800.mUnk_100.mUnk_18 != 1) {
        func_02020df4();
        data_020c1d60.mUnk_800.mUnk_100.mUnk_00[0] = data_020c18bc.mUnk_2ae;
        data_020c1d60.mUnk_800.mUnk_100.mUnk_00[1] = data_020c18bc.mUnk_2af;
        data_020c1d60.mUnk_800.mUnk_100.mUnk_18 = 1;
    }
    {
        u8 px = data_020c18bc.mUnk_2ae;
        u32 n = data_020c1d60.mUnk_800.mUnk_100.mUnk_10;
        data_020c1d60.mUnk_800.mUnk_000[n].mUnk_0 = px;
        data_020c1d60.mUnk_800.mUnk_000[n].mUnk_1 = data_020c18bc.mUnk_2af;
        data_020c1d60.mUnk_800.mUnk_100.mUnk_10 = (data_020c1d60.mUnk_800.mUnk_100.mUnk_10 + 1) & 0x7f;
        if (x != 0) {
            *x = px;
        }
    }
    if (y != 0) {
        *y = data_020c18bc.mUnk_2af;
    }
    return 1;
}

// Marks the backup memory as ready after taking the card lock id and checking the backup type.
void func_02020ee0(u32 a) {
    int ok;
    if (!(data_020c18bc.mUnk_46c & SYSTEM_BACKUP_READY)) {
        data_020c18bc.mUnk_456 = 0xb4;
        func_02045dc4();
        data_020c18bc.mUnk_330 = OS_GetLockID();
        func_02044f54(data_020c18bc.mUnk_330);
        ok = func_020455fc(a);
        func_02044f64(data_020c18bc.mUnk_330);
        if (ok != 0) {
            data_020c18bc.mUnk_46c |= SYSTEM_BACKUP_READY;
        }
    }
}

// Writes size bytes from buf to the backup memory at address; returns 0 when the backup memory is not ready.
int func_02020f38(const void *buf, u32 address, u32 size) {
    int result;
    if (!(data_020c18bc.mUnk_46c & SYSTEM_BACKUP_READY)) {
        return 0;
    }
    data_020c18bc.mUnk_456 = 0xf0;
    func_02044f54(data_020c18bc.mUnk_330);
    result = func_02045500((u32) buf, (void *) address, size, 0, 0, 0, 8, 10, 2);
    func_02044f64(data_020c18bc.mUnk_330);
    return result;
}

int func_02020f9c(void *buf, u32 address, u32 size) {
    int result;
    if ((data_020c18bc.mUnk_46c & SYSTEM_BACKUP_READY) == 0) {
        return 0;
    }
    func_02044f54(data_020c18bc.mUnk_330);
    result = func_02045500(address, buf, size, 0, 0, 0, 6, 1, 0);
    func_02044f64(data_020c18bc.mUnk_330);
    return result;
}

void func_02020ff4(int size, int b) {
    if (data_020c18bc.mUnk_46c & MIC_FLAG_ACTIVE) {
        func_02021458();
    }
    data_020c18bc.mUnk_0cc = size;
    data_020c18bc.mUnk_0c4 = func_0201c430(size + 0x1f);
    data_020c18bc.mUnk_0c8 = func_0201c430(size);
    MIC_Init();
    func_02043d98(1);
    func_02043de0(b);
    data_020c18bc.mUnk_46c |= MIC_FLAG_ACTIVE;
    func_02021194();
}

void func_02021054(int a) {
    if ((data_020c18bc.mUnk_46c & MIC_FLAG_ACTIVE) == 0) {
        OS_Terminate();
    }
    func_02043de0(a);
}

void func_02021074(int rate, int type) {
    data_020c18bc.mUnk_0d0.buffer = (void *) func_02034918((u32) data_020c18bc.mUnk_0c4);
    data_020c18bc.mUnk_0d0.size = data_020c18bc.mUnk_0cc;
    data_020c18bc.mUnk_0d0.type = type;
    data_020c18bc.mUnk_0d0.rate = rate;
    data_020c18bc.mUnk_0d0.loop = 1;
    data_020c18bc.mUnk_0d0.full = 0;
    data_020c18bc.mUnk_492 = 0xb4;
    data_020c18bc.mUnk_495 = 1;
    if (MIC_StartAutoSampling(&data_020c18bc.mUnk_0d0) != 0) {
        OS_Terminate();
    }
    func_02021194();
}

void func_020210c4(void) {
    MIC_StopAutoSampling();
}

u32 func_020210cc(void) {
    u32 pos = func_02043454();
    if (pos >= func_02034918((u32) data_020c18bc.mUnk_0c4) &&
        pos < func_02034918((u32) data_020c18bc.mUnk_0c4) + data_020c18bc.mUnk_0cc && data_020c18bc.mUnk_492 == 0) {
        DC_InvalidateRange((void *) (pos & ~0x1f), 0x20);
        return pos;
    }
    return 0;
}

void func_0202111c(u32 count) {
    u32 i;
    u32 pos;
    u32 start;
    if ((data_020c18bc.mUnk_46c & MIC_FLAG_ACTIVE) == 0) {
        OS_Terminate();
    }
    pos = func_020210cc();
    if (pos != 0) {
        pos -= count;
        start = func_02034918((u32) data_020c18bc.mUnk_0c4);
        if (start == 0) {
            OS_Terminate();
        }
        if (pos < start) {
            pos += data_020c18bc.mUnk_0cc;
        }
        for (i = 0; i < count; i++) {
            if ((pos & 0x1f) == 0) {
                DC_InvalidateRange((void *) pos, 0x20);
                if (pos >= start + data_020c18bc.mUnk_0cc) {
                    pos = start;
                }
            }
            data_020c18bc.mUnk_0c8[i] = *(u8 *) pos;
            pos++;
        }
    }
}

void func_02021194(void) {
    int j;
    int i;
    for (i = 0; i < MIC_HISTORY; i++) {
        for (j = 0; j < MIC_MAX_BANDS; j++) {
            data_020c18bc.mUnk_0ec[i][j] = 0;
        }
    }
    MI_CpuClearFast(data_020c18bc.mUnk_20c, 0x20);
    MI_CpuClearFast(data_020c18bc.mUnk_1ec, 0x20);
}

// Sums the spectrum over each band's range, stores the sums in the current history slot and totals each band over
// the history.
void func_020211d8(const u8 *spectrum, const u16 *ranges, u32 count, u32 *totals, u32 *sums) {
    int i;
    u32 j;
    u32 lo;
    u32 hi;
    if (count >= MIC_MAX_BANDS) {
        OS_Terminate();
    }
    if (data_020c18bc.mUnk_0d0.buffer == 0) {
        OS_Terminate();
    }
    for (i = 0; i < count; i++) {
        lo = func_02021364(ranges[0]);
        hi = func_02021364(ranges[1]);
        sums[i] = 0;
        for (; lo < hi; lo++) {
            sums[i] += spectrum[lo];
        }
        ranges += 2;
    }
    j = data_020c18bc.mUnk_45c & 0xf;
    for (i = 0; i < count; i++) {
        data_020c18bc.mUnk_0ec[j][i] = sums[i];
    }
    for (j = 0; j < count; j++) {
        totals[j] = 0;
        for (i = 0; i < MIC_HISTORY; i++) {
            totals[j] += data_020c18bc.mUnk_0ec[i][j];
        }
    }
}

typedef struct Unk_Complex {
    fx32 re;
    fx32 im;
} Unk_Complex;

void func_020212b4(u8 *out, u32 nShift) {
    u32 count = 1 << nShift;
    fx16 sinTable[(3 << MAX_SHIFT) / 4];
    Unk_Complex buf[1 << MAX_SHIFT];
    u32 i;
    if (nShift > MAX_SHIFT) {
        OS_Terminate();
    }
    data_020c18bc.mUnk_22c = nShift;
    func_0202111c(count);
    for (i = 0; i < count; i++) {
        buf[i].re = data_020c18bc.mUnk_0c8[i] << SAMPLE_SHIFT;
        buf[i].im = 0;
    }
    func_0204882c(sinTable, nShift);
    func_02048af8((fx32 *) buf, nShift, sinTable);
    for (i = 1; i < count; i++) {
        out[i - 1] = (buf[i].re * buf[i].re + buf[i].im * buf[i].im) >> POWER_SHIFT;
    }
}

u32 func_02021364(u32 x) {
    return (x << data_020c18bc.mUnk_22c) / data_020c18bc.mUnk_0d0.rate;
}

u32 func_02021380(u32 index) {
    return data_020c18bc.mUnk_20c[index] >> 4;
}

void func_02021390(void) {
    u8 spectrum[0x400];
    if ((data_020c18bc.mUnk_46c & MIC_FLAG_ACTIVE) == 0) {
        func_02020ff4(0x400, 1);
        func_02021074(0x82e, 0);
    }
    func_020212b4(spectrum, 8);
    func_020211d8(spectrum, data_02050210, 4, data_020c18bc.mUnk_20c, data_020c18bc.mUnk_1ec);
}

int func_020213e8(u32 a) {
    int v0;
    int v1;
    int v2;
    int v3;
    u32 limit;
    func_02021390();
    v0 = func_02021380(0);
    v1 = func_02021380(1);
    v2 = func_02021380(2);
    v3 = func_02021380(3);
    if (v0 > a && 40 * v3 > v0 && v3 < v0 && v1 < v0 && abs(v1 - v2) < v1 / 2 && v0 > 2 * v3) {
        limit = a * 6;
        if (v0 >= limit) {
            v0 = limit;
        }
        return func_0201fb74(v0, limit, 1, 7);
    }
    return 0;
}

void func_02021458(void) {
    if (data_020c18bc.mUnk_46c & MIC_FLAG_ACTIVE) {
        func_020210c4();
        func_02043d98(0);
        func_0201c4f0(&data_020c18bc.mUnk_0c4);
        func_0201c4f0(&data_020c18bc.mUnk_0c8);
        data_020c18bc.mUnk_46c &= ~MIC_FLAG_ACTIVE;
    }
}
