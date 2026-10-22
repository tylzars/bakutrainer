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
#include <Main/Link/link_indicator.h>

// JPEG reader state and the start-up code's arena start (defined below, with the JPEG tables).
extern u8 data_020c1818;
extern void *gArenaLo;
extern u8 *data_020c1820;
extern u32 data_020c1824;
extern PMCallbackInfo gPmCallbackInfo;

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

// Upper end of the range EaseLevel and AddLevel move a value in.
#define LEVEL_FULL 0x4000

// Half and full turn of an angle.
#define ANGLE_HALF 0x8000
#define ANGLE_FULL 0x10000

// Size of the whole store for SAVE_STORE_SMALL and SAVE_STORE_LARGE; GetSaveBlockSize divides it by the set's block
// count.
#define SMALL_BLOCK_SIZE 0x200
#define LARGE_BLOCK_SIZE 0x2000
#define CRC_POLY 0xa001

// Results of ReadSaveBlock.
#define BLOCK_CRC_OK 1
#define BLOCK_ID_OK 2
#define BLOCK_INTACT 3

// Marks of a field in the ordering list built by SortIndicesByKey.
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

void InitSystem(u32 a) {
    InitSystemEx(a, 0, 0);
}

void InitSystemEx(u32 a, int b, int c) {
    u32 size;
    TPCalibrateParam tp;
    int heapEnd;
    u16 mode;
    void *buf;

    (void) REG_IME;
    REG_IME = 1;
    gSystemState.mUnk_2c8 = 0;
    gSystemState.mUnk_470 = 0;
    data_020c3e44.mUnk_2024.mUnk_50 = 0;
    gSystemState.mTransferLimit = a;
    gSystemState.mUnk_30e = 0x20;
    gSystemState.mUnk_310 = 10;
    gSystemState.mUnk_311[0] = 9;
    gSystemState.mUnk_311[1] = 12;
    gSystemState.mUnk_308 = 0x80;
    gSystemState.mUnk_30a[0] = 0;
    gSystemState.mUnk_30a[1] = 0x1f;
    gSystemState.mFrameIrqCallback = 0;
    OS_Init();
    OS_InitTick();
    RTC_Init();
    OS_InitAlarm();
    gArenaLo = OS_GetArenaLo(0);
    OS_SetArenaLo(0, OS_InitAlloc(0, gArenaLo, OS_GetArenaHi(0), 2));
    if (func_0202ad40() == 1) {
        heapEnd = 0x100000;
    } else {
        heapEnd = 0x96000;
    }
    gSystemState.mHeapBoundary = (u8 *) OS_GetArenaHi(0) - heapEnd;
    gSystemState.mLowHeap = OS_CreateHeap(0, OS_GetArenaLo(0), gSystemState.mHeapBoundary);
    gSystemState.mHighHeap = OS_CreateHeap(0, gSystemState.mHeapBoundary + 1, OS_GetArenaHi(0));
    OS_SetCurrentHeap(0, gSystemState.mLowHeap);
    if ((gSystemState.mFlags & 0x200) == 0) {
        func_020421d0(-1);
        func_020421f8(&gBankFile);
        func_02042490(&gBankFile, "bank.bin");
        SeekBank(4);
        ReadBank(&size, 4);
        SeekBank(0);
        gSystemState.mFileTableSize = size;
        buf = Alloc(size);
        gSystemState.mFileTable = buf;
        ReadBank(buf, size);
    }
    InitFileCache();
    gSystemState.mTransfers = Alloc(20 * gSystemState.mTransferLimit);
    gSystemState.mTransferCount = 0;
    gSystemState.mTransferSize = 0;
    gSystemState.mTransferSizeLimitKb = 200;
    gSystemState.mUnk_464 = 0;
    MIi_CpuClearFast(0xc0, (void *) 0x07000000, 0x400);
    MIi_CpuClearFast(0, (void *) 0x05000000, 0x400);
    MIi_CpuClearFast(0xc0, (void *) 0x07000400, 0x400);
    MIi_CpuClearFast(0, (void *) 0x05000400, 0x400);
    gTextureBlocks.mLayers[0].mUnk_577 = 0;
    gTextureBlocks.mLayers[1].mUnk_577 = 0;
    gSystemState.mElapsedFrames = 0;
    gSystemState.mFadeState[0] = gSystemState.mFadeState[1] = FADE_STATE_CLEAR;
    *(vu16 *) 0x0400106c = *(vu16 *) 0x0400006c = 0;
    ResetKeys();
    (void) REG_IME;
    REG_IME = 0;
    func_0203d338();
    OS_SetIrqFunction(1, HandleFrameIrq);
    OS_EnableIrqMask(1);
    OS_EnableIrqMask(0x40000);
    (void) REG_IME;
    REG_IME = 1;
    GX_VBlankIntr(1);
    ResetVramBanks();
    TP_Init();
    if (TP_GetUserInfo(&tp) == 1) {
        TP_SetCalibrateParam(&tp);
    }
    gSystemState.mTouchEnabled = gSystemState.mTouchDown = gSystemState.mTouchReleased = gSystemState.mTouchPressed = 0;
    gSystemState.mTouchReleaseTimer = 0;
    gSystemState.mUseTouchReleaseTimer = 1;
    gSystemState.mUnk_4a0 = 1;
    gSystemState.mFlags |= 0x400000;
    ResetTouchButtons(0, 0);
    if (c != 0) {
        func_0202ad48(c);
    }
    if (func_0202ad40() == 1) {
        mode = 0x1080;
    } else {
        mode = 0xb00;
    }
    gSystemState.mTransferBytesPerLine = mode;
    gSystemState.mUnk_42c = -1;
    gSystemState.mUnk_340[2] = 0x1800;
    gSystemState.mUnk_340[0] = 0x100;
    gSystemState.mUnk_340[1] = 0x19;
    gSystemState.mUnk_340[3] = 0x1800;
    func_02043828();
    func_020443ac(1);
    gPmCallbackInfo.func = func_0201c1d0;
    gPmCallbackInfo.arg = 0;
    func_0204445c(&gPmCallbackInfo);
    func_020402f0();
    *(vu16 *) 0x04000304 &= ~0xc;
    gSystemState.mUnk_24c = func_02034a60;
    gSystemState.mUnk_280 = 0;
    gSystemState.mUnk_2a4[0] = 0;
    gSystemState.mUnk_240 = 0;
    gSystemState.mUnk_230 = 0;
    gSystemState.mUnk_234 = 0;
    gSystemState.mUnk_238 = 0;
    gSystemState.mUnk_23c = 0;
    gSystemState.mUnk_244 = 0;
    gSystemState.mUnk_248 = 0;
    gSystemState.mUnk_2a3 = 0;
    gSystemState.mUnk_450 = 0;
    gSystemState.mClearedSaveBlocks[0] = 0;
    gSystemState.mUnk_454 = 0xffff;
    gSystemState.mUnk_498 = 0x7f;
}

// One frame of the system step, including the wait for the next frame; in the link error state (-2) it never returns.
void UpdateSystemAndWait(void) {
    UpdateSystem(0);
    func_02034be4();
}

void UpdateSystem(int flags) {
    u32 frame = gSystemState.mElapsedFrames;
    TPData tp2;
    TPData tp;
    u16 wmStatus[0x3e8];
    int engine;
    int i;
    int single;
    if (gSystemState.mUnk_2c8 & 0x400000) {
        if (func_02035058() == 0) {
            func_0202e8e4();
        }
    }
    gSystemState.mFrameStartLine = *(vu16 *) HW_VCOUNT;
    gSystemState.mUnk_48c = gSystemState.mUnk_48d;
    gSystemState.mUnk_48d = -1;
    gSystemState.mUnk_48f += gSystemState.mUnk_48c;
    if (gSystemState.mElapsedFrames >= gSystemState.mUnk_460 + 0x3c) {
        gSystemState.mUnk_48e = 0x3c - gSystemState.mUnk_48f;
        gSystemState.mUnk_48f = 0;
        gSystemState.mUnk_460 = gSystemState.mElapsedFrames;
    }
    if (gSystemState.mFlags & 0x20000000) {
        u16 line;
        *(vu32 *) HW_DISPCNT |= DISPCNT_FORCED_BLANK;
        line = gSystemState.mFrameStartLine;
        while (*(vu16 *) HW_VCOUNT == line) {
        }
        *(vu32 *) HW_DISPCNT &= ~DISPCNT_FORCED_BLANK;
    }
    data_020c3e44.mUnk_2024.mUnk_48 = 0;
    MixRandomSeed();
    single = flags & 1;
    if (single == 0 && (gSystemState.mFlags & 0x20000)) {
        if (gSystemState.mFadeState[0] != FADE_STATE_TO_FADED && gSystemState.mFadeState[1] != FADE_STATE_TO_FADED) {
            StartFadeClear(3);
        }
    }
    if ((gSystemState.mFlags & 0x10000080) == 0x10000080) {
        if (func_02035058() == 0) {
            func_020248fc();
            if (data_020c3e44.mUnk_2024.mUnk_2dc.mUnk_4a == 0 && !(gSystemState.mFlags & 0x10000)) {
                *(vu32 *) HW_SWAP_BUFFERS = 3;
            } else {
                *(vu32 *) HW_SWAP_BUFFERS = 1;
            }
        }
    }
    if (single == 0 && (gSystemState.mFlags & 0x2000)) {
        func_0202ef24();
    }
    func_0202ad4c();
    ProcessTransfers(1);
    ProcessTransfers(2);
    ClearTransferQueue();
    if (((*(vu16 *) HW_SHARED_BUTTONS & LID_CLOSED) >> 15) == 1) {
        if (func_0202fcb4() == 1) {
            ClearTouchButtons();
            flags |= 2;
        }
    }
    if ((flags & 1) == 0) {
        while (gSystemState.mElapsedFrames == frame) {
            OS_Halt();
        }
    }
    for (engine = 0; engine < 2; engine++) {
        if (gTextureBlocks.mLayers[engine].mUnk_57c != 0) {
            gTextureBlocks.mLayers[engine].mUnk_57c = 0;
            func_0202bb30(engine == 0 ? 1 : 2);
        }
    }
    if (gSystemState.mUnk_4a0 == 1) {
        func_02042dc8();
        func_02042ea8(&tp2);
        if (tp2.touch == 0) {
            gSystemState.mUnk_4a0 = 0;
        }
        ClearTouchButtons();
    } else if ((flags & 3) == 0 && (gSystemState.mFlags & 0x400000)) {
        func_02042dc8();
        func_02042ea8(&tp);
        if (tp.touch == 0 || tp.validity == 0) {
            if (gSystemState.mTouchReleaseTimer == 0 || !gSystemState.mUseTouchReleaseTimer) {
                gSystemState.mTouchSample = tp.touch;
            } else {
                gSystemState.mTouchReleaseTimer--;
                gSystemState.mTouchSample = 0;
            }
            gSystemState.mTouchEnabled |= gSystemState.mTouchSample ^ 1;
            gSystemState.mTouchSample = gSystemState.mTouchSample & gSystemState.mTouchEnabled;
            gSystemState.mTouchPressed =
                (gSystemState.mTouchDown ^ gSystemState.mTouchSample) & gSystemState.mTouchSample;
            gSystemState.mTouchReleased =
                gSystemState.mTouchDown & (gSystemState.mTouchDown ^ gSystemState.mTouchSample);
            gSystemState.mTouchDown = gSystemState.mTouchSample;
            if (gSystemState.mTouchReleased == 1) {
                gSystemState.mTouchReleaseTimer = 8;
                if (data_020c1d60.mUnk_800.mUnk_100.mUnk_10 < 10) {
                    if (LengthSquared2d(gSystemState.mTouchMaxX - gSystemState.mTouchMinX,
                                        gSystemState.mTouchMaxY - gSystemState.mTouchMinY) < 0x19) {
                        gSystemState.mUnk_2ac_3 = 1;
                    }
                }
            }
            if (gSystemState.mTouchPressed == 1) {
                gSystemState.mTouchMaxX = gSystemState.mTouchMaxY = 0;
                gSystemState.mTouchMinX = gSystemState.mTouchMinY = 0xff;
            }
            if (tp.touch == 1) {
                switch (tp.validity) {
                    case 0:
                        gSystemState.mTouchX = tp.x;
                        gSystemState.mTouchY = tp.y;
                        break;
                    case 1:
                        gSystemState.mTouchY = tp.y;
                        break;
                    case 2:
                        gSystemState.mTouchX = tp.x;
                        break;
                    case 3:
                        break;
                }
                gSystemState.mUnk_2ac_3 = 0;
                if (gSystemState.mTouchX > gSystemState.mTouchMaxX) {
                    gSystemState.mTouchMaxX = gSystemState.mTouchX;
                }
                if (gSystemState.mTouchX < gSystemState.mTouchMinX) {
                    gSystemState.mTouchMinX = gSystemState.mTouchX;
                }
                if (gSystemState.mTouchY > gSystemState.mTouchMaxY) {
                    gSystemState.mTouchMaxY = gSystemState.mTouchY;
                }
                if (gSystemState.mTouchY < gSystemState.mTouchMinY) {
                    gSystemState.mTouchMinY = gSystemState.mTouchY;
                }
            }
        }
        if (gSystemState.mFlags & 0x800000) {
            data_020c1d60.mUnk_800.mUnk_100.mUnk_04 = UpdateTouchButtons(0xffff);
        }
        ClearTouchButtons();
    }
    if (gSystemState.mUnk_2a3 == 0) {
        if (gSystemState.mFlags & 0x100) {
            gSystemState.mFlags &= ~0x100;
            if ((gSystemState.mFlags & 0x600) || gSystemState.mUnk_2a4[1] > 1) {
                if (gSystemState.mFlags & 0x200) {
                    func_02039ba8();
                    func_02034a44();
                    gSystemState.mUnk_2a4[1] = -2;
                } else {
                    func_02039ba8();
                }
            }
        }
        if (gSystemState.mUnk_2a4[1] > 0) {
            if (func_020465b0(wmStatus) == 0) {
                u16 state = wmStatus[0];
                if (state != 7 && state != 9 && state != 10 && state != 5) {
                    flags |= 4;
                }
            }
        }
        gSystemState.mBackupTimer -= (gSystemState.mBackupTimer != 0) ? 1 : 0;
        if (gSystemState.mFlags & 0x20) {
            func_0202fbc8();
        } else if ((flags & 4) == 0 && ((*(vu16 *) HW_SHARED_BUTTONS & LID_CLOSED) >> 15) == 1 &&
                   gSystemState.mBackupTimer == 0) {
            if (gSystemState.mFlags & 0x200) {
                gSystemState.mFlags |= 0x100;
            } else if (gSystemState.mUnk_2a6_1) {
                func_02039b70();
                func_02001120();
            } else if (gSystemState.mUnk_2a6_0) {
                func_02039b70();
                func_02039ba8();
            }
            func_0204028c(0);
            while (SND_RecvCommandReply(0) != 0) {
            }
            SND_FlushCommand(2);
            if ((gSystemState.mFlags & 0x200) == 0 || gSystemState.mUnk_2a4[1] == -2) {
                u32 save = gSystemState.mUnk_470 & 4;
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
        if (gSystemState.mFlags & 0x200) {
            data_020c9e04 &= ~0x800;
            func_02034c38();
            func_02039d24();
        } else if (gSystemState.mFlags & 0x400) {
            data_020c9e04 &= ~0x800;
            func_02039d24();
            func_02001170();
        } else if (gSystemState.mUnk_2a4[1] > 1) {
            int offset;
            func_02039d24();
            gSystemState.mUnk_2a8 = func_02047264(&data_020c9e60, data_020c9e10, data_020c9e14);
            data_020c9e04 &= ~0x800;
            gSystemState.mUnk_288 = 0;
            i = 0;
            offset = 0;
            for (; i < 2; i++) {
                u8 *entry = func_02047ac4(&data_020c9e60, data_020c9e14, (u16) i);
                if (entry != 0) {
                    MIi_CpuCopy16(entry, data_020c9e1c + offset, 0x54);
                    gSystemState.mUnk_288 |= 1 << i;
                }
                offset += 0x54;
            }
            func_02034a08();
        }
    } else {
        gSystemState.mUnk_2a3--;
    }
    single = flags & 1;
    if (single == 0) {
        func_02039a94();
    }
    if (single == 0) {
        UpdateFade();
        func_0202d908(1);
        func_0202d908(2);
        if (func_02035058() == 0 && (flags & 2) == 0) {
            UpdateKeys();
        }
        if (gSystemState.mFlags & 0x2000) {
            func_0202efd4();
        }
        if (gSystemState.mFlags & 0x1000000) {
            func_020360f4();
            func_02036194();
        }
        func_0202ad3c();
        if (gSystemState.mUnk_492 != 0) {
            gSystemState.mUnk_492--;
            if (gSystemState.mUnk_495 == 0) {
                u32 length = (0xb4 - gSystemState.mUnk_492) * 2;
                u32 tries;
                if (length >= gSystemState.mUnk_498) {
                    length = gSystemState.mUnk_498;
                }
                func_0204028c(length);
                tries = 0;
                for (; SND_RecvCommandReply(0) != 0 && tries < 10; tries++) {
                }
                SND_FlushCommand(2);
                return;
            }
            if (gSystemState.mUnk_492 == 0) {
                gSystemState.mUnk_495 = 0;
            }
        }
    }
}

void func_0201c1d0(void *arg) {
    gSystemState.mUnk_492 = 0xb4;
    gSystemState.mUnk_495 = 0;
}

void HandleFrameIrq(void) {
    if (gSystemState.mFrameIrqCallback != 0) {
        gSystemState.mFrameIrqCallback();
    }
    if ((gSystemState.mUnk_450 & 2) && gSystemState.mUnk_454 != 0xffff) {
        int angle = (gSystemState.mElapsedFrames & 0xff) << 8;
        if (gSystemState.mUnk_450 & 4) {
            gSystemState.mUnk_450 &= ~6;
        } else {
            int index = (angle >> 4) * 2;
            vu16 *oam = (vu16 *) HW_SUB_OAM;
            oam[3] = FX_SinCosTable_[index + 1] >> 4;
            oam[15] = oam[3];
            oam[7] = FX_SinCosTable_[index] >> 4;
            oam[11] = -oam[7];
        }
    }
    if (!(gSystemState.mFlags & 0x100000)) {
        *(vu32 *) HW_BG3X = gSystemState.mBg3X;
        *(vu32 *) HW_BG3Y = gSystemState.mBg3Y;
        *(vu32 *) (HW_BG3X + HW_SUB) = gSystemState.mSubBg3X;
        *(vu32 *) (HW_BG3Y + HW_SUB) = gSystemState.mSubBg3Y;
    }
    if (!(gSystemState.mFlags & 0x40000000)) {
        *(vu16 *) HW_BG3PA = gSystemState.mBg3Pa;
        *(vu16 *) HW_BG3PD = gSystemState.mBg3Pd;
    }
    if (!(gSystemState.mFlags & 0x200000)) {
        *(vu16 *) HW_WIN0H = gSystemState.mWin0H;
        *(vu16 *) HW_WIN0V = gSystemState.mWin0V;
        *(vu16 *) HW_WIN1H = gSystemState.mWin1H;
        *(vu16 *) HW_WIN1V = gSystemState.mWin1V;
        *(vu16 *) HW_DISPCNT = (*(vu16 *) HW_DISPCNT & ~DISPCNT_WINDOWS) | (gSystemState.mWindowBits << 13);
        *(vu16 *) (HW_WIN0H + HW_SUB) = gSystemState.mSubWin0H;
        *(vu16 *) (HW_WIN0V + HW_SUB) = gSystemState.mSubWin0V;
        *(vu16 *) (HW_WIN1H + HW_SUB) = gSystemState.mSubWin1H;
        *(vu16 *) (HW_WIN1V + HW_SUB) = gSystemState.mSubWin1V;
        *(vu16 *) (HW_DISPCNT + HW_SUB) =
            (*(vu16 *) (HW_DISPCNT + HW_SUB) & ~DISPCNT_WINDOWS) | (gSystemState.mSubWindowBits << 13);
    }
    gSystemState.mElapsedFrames++;
    gSystemState.mUnk_48d++;
    {
        vu32 *shared = (vu32 *) HW_MAIN_RAM_END;
        shared[0xffe] |= 1;
    }
}

void UpdateKeys(void) {
    u16 keys = (((*(vu16 *) HW_KEYINPUT) | (*(vu16 *) HW_SHARED_BUTTONS)) ^ 0x2fff) & 0x2fff;
    u16 held;
    int i = 0;
    u16 repeat = 0;

    held = keys;
    gSystemState.mKeysEnabled |= 0xffff ^ keys;
    held &= gSystemState.mKeysEnabled;
    gSystemState.mKeysPressed = held & (held ^ gSystemState.mKeysDown);
    gSystemState.mKeysReleased = gSystemState.mKeysDown & (held ^ gSystemState.mKeysDown);
    gSystemState.mKeysDown = held;
    for (; held != 0; held >>= 1) {
        if (held & 1) {
            if (gSystemState.mKeyRepeatTimers[i] != 0) {
                if (gSystemState.mKeyRepeatTimers[i] < 0x10 || (gSystemState.mKeysRepeatDown & (1 << i))) {
                    repeat = repeat | 1 << i;
                }
            }
            gSystemState.mKeyRepeatTimers[i] = 0x10;
        } else if (gSystemState.mKeyRepeatTimers[i] != 0) {
            gSystemState.mKeyRepeatTimers[i]--;
        }
        ++i;
    }
    for (; i < 12; i++) {
        if (gSystemState.mKeyRepeatTimers[i] != 0) {
            gSystemState.mKeyRepeatTimers[i]--;
        }
    }
    gSystemState.mKeysRepeatPressed = repeat & (repeat ^ gSystemState.mKeysRepeatDown);
    gSystemState.mKeysRepeatReleased = gSystemState.mKeysRepeatDown & (repeat ^ gSystemState.mKeysRepeatDown);
    gSystemState.mKeysRepeatDown = repeat;
    if (gSystemState.mKeysPressed & 0xf00) {
        gSystemState.mKeyHistoryIndex = (1 + gSystemState.mKeyHistoryIndex) & 0xf;
        gSystemState.mKeyHistory[gSystemState.mKeyHistoryIndex] = (gSystemState.mKeysPressed >> 8) & 0xf;
    }
}

void ResetKeys(void) {
    gSystemState.mKeysEnabled = 0;
    gSystemState.mKeysReleased = 0;
    gSystemState.mKeysPressed = gSystemState.mKeysDown = gSystemState.mKeysReleased;
}

void *Alloc(u32 size) {
    return AllocWithFlags(size, 0);
}

// Flags: 1 leaves the block uncleared, 2 forces the low heap, 4 takes the high heap at any size (no caller passes 4
// in this build). While the block does not fit, least recently used file cache entries are dropped.
void *AllocWithFlags(u32 size, u32 flags) {
    int first = 1;
    int useHigh;
    void *p;
    size = ((((size >> 1) | size) & 1) + (size >> 2)) << 2;
    if ((flags & 2) == 0 && ((flags & 4) != 0 || size < 0x8000) && func_0203ed14(0, gSystemState.mHighHeap) >= size) {
        OS_SetCurrentHeap(0, gSystemState.mHighHeap);
        useHigh = 1;
    } else {
        OS_SetCurrentHeap(0, gSystemState.mLowHeap);
        useHigh = 0;
    }
    if (size != 0) {
        p = OS_AllocFromHeap(0, -1, size);
        while (p == 0) {
            if (useHigh == 1) {
                OS_SetCurrentHeap(0, gSystemState.mLowHeap);
                useHigh = 0;
            } else {
                if (first == 1) {
                    first = 0;
                }
                RemoveLeastRecentCacheEntry();
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

int Free(void *handle) {
    void **pp = handle;
    u32 *block;
    if (pp != 0 && (block = *pp) != 0) {
        if ((u8 *) block > gSystemState.mHeapBoundary) {
            OS_SetCurrentHeap(0, gSystemState.mHighHeap);
        } else {
            OS_SetCurrentHeap(0, gSystemState.mLowHeap);
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
    return func_0201c544() + ((GetStoredFileSize(gTextureBlocks.mPaletteFile) >> 5) << 2);
}

#pragma thumb off
// Queues a transfer for ProcessTransfers: grows the queue's byte count, waits for the queue to drain when it is full
// (terminating when it stays full), and appends the entry.
void QueueTransferEx(u32 file, u32 src, u32 dst, u32 size, u32 unpackedSize, u8 mode, u8 flag) {
    Unk_LoadEntry entry;
    int limit;
    if (unpackedSize == 0) {
        unpackedSize = size;
    }
    if (func_02035058() == 0) {
        limit = gSystemState.mTransferLimit;
    } else {
        limit = (u32) gSystemState.mTransferLimit >> 1;
    }
    if (!(gSystemState.mTransferCount < limit &&
          (u32) gSystemState.mTransferSize < (u32) gSystemState.mTransferSizeLimitKb << 10 &&
          gSystemState.mTransferSize + unpackedSize < (u32) (gSystemState.mTransferSizeLimitKb + 0x40) << 10)) {
        UpdateSystem(3);
        if (gSystemState.mTransferCount >= gSystemState.mTransferLimit) {
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
    if (gSystemState.mTransferCount != 0) {
        memcmp(&entry, &gSystemState.mTransfers[gSystemState.mTransferCount - 1], sizeof(Unk_LoadEntry));
    }
    gSystemState.mTransfers[gSystemState.mTransferCount++] = entry;
    gSystemState.mTransferSize += unpackedSize;
}

// Queues a transfer whose unpacked size equals its stored size.
void QueueTransfer(u16 file, u32 src, u32 dst, u32 size) {
    QueueTransferEx(file, src, dst, size, 0, 0, 0);
}

// Empties the transfer queue when the system is not busy.
void ClearTransferQueue(void) {
    if (func_02035058() == 0) {
        gSystemState.mTransferCount = 0;
        gSystemState.mTransferSize = 0;
    }
}

// While screen 1's fade state is FADE_STATE_CLEAR, limits a transfer size to what fits before the end of the scanline
// window (lines 0xbf-0xd6), first waiting out the other lines; otherwise the size is returned as it is.
u32 LimitTransferSize(u32 size) {
    u32 result = size;
    if (gSystemState.mFadeState[0] == FADE_STATE_CLEAR) {
        int line = *(vu16 *) HW_VCOUNT;
        if (!(line >= 0xbf && line < 0xd6)) {
            result = gSystemState.mTransferBytesPerLine * 0x16;
            while (*(vu16 *) HW_VCOUNT >= 0xd6 || *(vu16 *) HW_VCOUNT < 0xbf) {
            }
        } else {
            result = (0xd6 - line) * gSystemState.mTransferBytesPerLine;
        }
    }
    if (result >= size) {
        result = size;
    }
    return result;
}

// While screen 1's fade state is FADE_STATE_CLEAR, waits until the display reaches line 0xc0.
void func_0201c78c(void) {
    if (gSystemState.mFadeState[0] == FADE_STATE_CLEAR) {
        while (*(vu16 *) HW_VCOUNT < 0xc0) {
        }
    }
}
#pragma thumb on

#pragma thumb off
// Runs the queued transfers: maps the VRAM banks to LCDC when the bank layout changed, picks the queued entries for
// VRAM (flags & 2) or other memory (flags & 1), sorts them by file and offset, reads and unpacks them into one
// buffer (packed block files, EZ10 blocks, plain file reads, memory copies), converts textures and palettes, then
// copies each entry to its destination with the banks mapped. The removal of finished entries at the end, for a busy
// system, never runs: while busy no entry is picked and the function returns before it.
// The block-run search and the plain file read take the list pointer through its address and the file id through
// a cast of the entry pointer.
void ProcessTransfers(int flags) {
    int i;
    Unk_LoadEntry **list;
    Unk_LoadEntry *e;
    int j;
    BOOL busy;
    u32 cachedFile;
    u32 total;
    int cachedSize;
    u8 *buf;
    u32 bits;
    int rest;
    u8 *dst;
    int cachedOff;
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
    if (gSystemState.mTransferCount == 0) {
        return;
    }
    if (busy && (gSystemState.mFlags & 3) == 0) {
        return;
    }
    list = Alloc(gSystemState.mTransferLimit << 2);
    bits = 0;
    cachedFile = 0;
    tmp = 0;
    tex = 0;
    if (gSystemState.mVramBanksChanged == 1) {
        gSystemState.mTexVramGroup.mBankCount = 0;
        gSystemState.mPlttVramGroup.mBankCount = 0;
        for (i = 0; i < 9; i++) {
            u32 idx = gSystemState.mVramBankRecordIndex[i];
            u32 use;
            const VramBankRecord *head;
            const VramBankRecord *rec;
            if (idx == 0xff) {
                continue;
            }
            use = gVramBankRecords[idx].mUse;
            if (use != 11 && use != 10 && use != 5 && use != 9) {
                continue;
            }
            for (j = idx - 1; gVramBankRecords[j].mUse != 0; j--) {
            }
            head = &gVramBankRecords[j];
            rec = &gVramBankRecords[idx];
            switch (use) {
                case 11:
                    gSystemState.mPlttVramGroup.mCtrl[gSystemState.mPlttVramGroup.mBankCount] =
                        (vu8 *) (gVramCtrlAddrs[head->mBank] + (gVramCtrlBitOffsets[head->mBank] / 8));
                    gSystemState.mPlttVramGroup.mCtrlValue[gSystemState.mPlttVramGroup.mBankCount] =
                        (rec->mUnk_9 << 3) + 0x80 + rec->mUnk_8;
                    gSystemState.mPlttVramGroup.mAddr[gSystemState.mPlttVramGroup.mBankCount] = head->mAddr;
                    gSystemState.mPlttVramGroup.mBankCount++;
                    break;
                case 10:
                    gSystemState.mTexVramGroup.mCtrl[gSystemState.mTexVramGroup.mBankCount] =
                        (vu8 *) (gVramCtrlAddrs[head->mBank] + (gVramCtrlBitOffsets[head->mBank] / 8));
                    gSystemState.mTexVramGroup.mCtrlValue[gSystemState.mTexVramGroup.mBankCount] =
                        (rec->mUnk_9 << 3) + 0x80 + rec->mUnk_8;
                    gSystemState.mTexVramGroup.mSizeKb[gSystemState.mTexVramGroup.mBankCount] = head->mSizeKb;
                    gSystemState.mTexVramGroup.mAddr[gSystemState.mTexVramGroup.mBankCount] = head->mAddr;
                    gSystemState.mTexVramGroup.mBankCount++;
                    break;
                case 5:
                    gSystemState.mUnk_0a0.mCtrl =
                        (vu8 *) (gVramCtrlAddrs[head->mBank] + (gVramCtrlBitOffsets[head->mBank] / 8));
                    gSystemState.mUnk_0a0.mCtrlValue = (rec->mUnk_9 << 3) + 0x80 + rec->mUnk_8;
                    gSystemState.mUnk_0a0.mAddr = head->mAddr;
                    break;
                case 9:
                    gSystemState.mUnk_0ac.mCtrl =
                        (vu8 *) (gVramCtrlAddrs[head->mBank] + (gVramCtrlBitOffsets[head->mBank] / 8));
                    gSystemState.mUnk_0ac.mCtrlValue = (rec->mUnk_9 << 3) + 0x80 + rec->mUnk_8;
                    gSystemState.mUnk_0ac.mAddr = head->mAddr;
                    break;
            }
        }
        for (i = 1; i < gSystemState.mTexVramGroup.mBankCount; i++) {
            if (gSystemState.mTexVramGroup.mAddr[i] !=
                gSystemState.mTexVramGroup.mAddr[i - 1] + (gSystemState.mTexVramGroup.mSizeKb[i - 1] << 10)) {
                OS_Terminate();
            }
        }
        gSystemState.mVramBanksChanged = 0;
    }

    count = 0;
    for (i = 0; i < gSystemState.mTransferCount; i++) {
        u32 addr;
        if (gSystemState.mTransfers[i].mUnk_08 == 0) {
            continue;
        }
        addr = gSystemState.mTransfers[i].mUnk_04;
        if ((addr >> 28) == 11 || (addr >> 28) == 10 || (addr >= 0x06000000 && addr < 0x06400000)) {
            if ((flags & 2) && !busy) {
                list[count] = &gSystemState.mTransfers[i];
                count++;
            }
        } else {
            if ((flags & 1) && !busy) {
                list[count] = &gSystemState.mTransfers[i];
                count++;
            }
        }
    }
    if (count == 0) {
        Free(&list);
        return;
    }

    if (count > 1) {
        int gap = (1 << (32 - LeadingZeroCount(count))) - 1;
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
    buf = AllocWithFlags(total + 0x80, 1);
    dst = buf;
    cachedOff = 0;
    cachedSize = 0;
    for (i = 0; i < count; i++) {
        e = list[i];
        if (gTextureBlocks.mBlockFile == e->mUnk_10 && gTextureBlocks.mBlockFileKind == 1) {
            if (gTextureBlocks.mUnk_000 != 0 && e->mUnk_00 == gTextureBlocks.mUnk_00c << 3) {
                MIi_CpuCopy32(gTextureBlocks.mUnk_004, dst, e->mUnk_0c);
                gTextureBlocks.mUnk_017 = 0;
            } else {
                int size;
                unsigned long first;
                u32 last;
                Unk_LoadEntry **p;
                u32 next;
                int k;
                first = e->mUnk_00 >> 11;
                last = (e->mUnk_00 + e->mUnk_08) >> 11;
                for (k = i + 1, p = *(Unk_LoadEntry ***) &list;
                     k < count && (next = (p[k]->mUnk_00 + p[k]->mUnk_08) >> 11) <= last + 1 &&
                     gTextureBlocks.mPackOffsets[next + 1] - gTextureBlocks.mPackOffsets[first] <= 0x8000;
                     k++) {
                    if (next > last) {
                        last = next;
                    }
                }
                size = gTextureBlocks.mPackOffsets[last + 1] - gTextureBlocks.mPackOffsets[first];
                if (tmp == 0) {
                    tmp = AllocWithFlags(0x8000, 1);
                }
                if ((int) gTextureBlocks.mPackOffsets[first] + size > cachedOff + cachedSize || k == 0 ||
                    list[i]->mUnk_10 != cachedFile) {
                    ReadFileBytes(list[i]->mUnk_10, tmp, size, gTextureBlocks.mPackOffsets[first]);
                    cachedSize = size;
                    cachedOff = gTextureBlocks.mPackOffsets[first];
                }
                UnpackRlePart(tmp + (gTextureBlocks.mPackOffsets[first] - cachedOff), dst, list[i]->mUnk_00 & 0x7ff,
                              list[i]->mUnk_08);
            }
        } else if (gTextureBlocks.mBlockFile == e->mUnk_10 && gTextureBlocks.mBlockFileKind == 2) {
            u32 sub = (e->mUnk_00 << 25) >> 20;
            u32 off;
            e->mUnk_00 = (e->mUnk_00 >> 7) << 2;
            e = list[i];
            off = e->mUnk_00;
            if (gTextureBlocks.mUnk_000 != 0 && gTextureBlocks.mUnk_00c == off + (sub << 20)) {
                MIi_CpuCopy32(gTextureBlocks.mUnk_004, dst, e->mUnk_0c);
                gTextureBlocks.mUnk_017 = 0;
            } else if (cachedOff != off || cachedFile != e->mUnk_10) {
                ReadFileBytes(e->mUnk_10, hdr, 0x10, off);
                if (hdr[0] == EZ10_MAGIC) {
                    u8 *packed;
                    if (tmp == 0) {
                        tmp = AllocWithFlags(0x8000, 1);
                    }
                    if (hdr[1] > 0x8000) {
                        OS_Terminate();
                    }
                    if (hdr[1] < 0x10) {
                        OS_Terminate();
                    }
                    packed = (u8 *) (((u32) tmp + 0x8000 - hdr[3]) & ~3);
                    memcpy(packed, hdr, 0x10);
                    ReadFileBytes(list[i]->mUnk_10, packed + 0x10, hdr[3] - 0x10, off + 0x10);
                    UnpackEz10(packed, tmp);
                    MIi_CpuCopy32(tmp + sub, dst, list[i]->mUnk_0c);
                    cachedOff = off;
                    cachedFile = list[i]->mUnk_10;
                } else {
                    ReadFileBytes(list[i]->mUnk_10, dst, list[i]->mUnk_0c, off + sub);
                }
            } else {
                MIi_CpuCopy32(tmp + sub, dst, e->mUnk_0c);
            }
        } else if (gSystemState.mUnk_42c == e->mUnk_10) {
            u32 sub = (e->mUnk_00 << 26) >> 20;
            u32 off = (e->mUnk_00 >> 6) << 2;
            if (gSystemState.mUnk_350 != off || gSystemState.mUnk_354 != e->mUnk_10) {
                u32 *packed;
                if (gSystemState.mUnk_34c == 0) {
                    gSystemState.mUnk_34c = Alloc(0x1200);
                }
                packed = (u32 *) (((u32) gSystemState.mUnk_34c + 0x1000 - list[i]->mUnk_08) & ~3);
                ReadFileBytes(list[i]->mUnk_10, packed, list[i]->mUnk_08, off);
                if (packed[0] != EZ10_MAGIC) {
                    gSystemState.mUnk_350 = off;
                    gSystemState.mUnk_354 = list[i]->mUnk_10;
                    MIi_CpuCopyFast(packed, gSystemState.mUnk_34c, list[i]->mUnk_08);
                    MIi_CpuCopyFast(gSystemState.mUnk_34c + sub, dst, list[i]->mUnk_0c);
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
                    UnpackEz10(packed, gSystemState.mUnk_34c);
                    MIi_CpuCopyFast(gSystemState.mUnk_34c + sub, dst, list[i]->mUnk_0c);
                    gSystemState.mUnk_350 = off;
                    gSystemState.mUnk_354 = list[i]->mUnk_10;
                    list[i]->mUnk_08 = list[i]->mUnk_0c;
                }
            } else {
                MIi_CpuCopyFast(gSystemState.mUnk_34c + sub, dst, e->mUnk_0c);
                list[i]->mUnk_08 = list[i]->mUnk_0c;
            }
        } else if (data_020c31dc.mUnk_54 == e->mUnk_10 && e->mUnk_08 < e->mUnk_0c) {
            u32 size = e->mUnk_08;
            u32 *packed = (u32 *) (((u32) dst + e->mUnk_0c + 0x80 - size) & ~3);
            ReadFileBytes(e->mUnk_10, packed, size, e->mUnk_00);
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
            UnpackEz10(packed, dst);
            list[i]->mUnk_08 = list[i]->mUnk_0c;
        } else if (e->mUnk_10 != 0) {
            u32 size = e->mUnk_08;
            j = i + 1;
            if (cachedFile != e->mUnk_10 || e->mUnk_00 + size > cachedOff + cachedSize ||
                (i > 0 && list[i - 1]->mUnk_00 == list[i]->mUnk_00) ||
                (lastDst & ~0x3ffff) != (e->mUnk_04 & ~0x3ffff)) {
                cachedFile = ((Unk_LoadEntry *) e)->mUnk_10;
                cachedOff = e->mUnk_00;
                lastDst = (*(Unk_LoadEntry ***) &list)[j - 1]->mUnk_04;
                cachedSize = size;
                ReadFileBytes(cachedFile, dst, size, cachedOff);
                if (gSystemState.mUnk_464 != 0 && (list[i]->mUnk_04 >> 28) == 11) {
                    DC_FlushRange(dst, size);
                    gSystemState.mUnk_464(dst, size);
                }
            }
        } else {
            if (e->mUnk_08 & 1) {
                OS_Terminate();
            }
            MIi_CpuCopy16((void *) list[i]->mUnk_00, dst, list[i]->mUnk_08);
        }

        e = list[i];
        if (gTextureBlocks.mBlockFile == e->mUnk_10) {
            BOOL bg = 0;
            u32 area = e->mUnk_04 >> 20;
            if (area < 0x64 && area >= 0x60) {
                bg = 1;
            }
            if (e->mUnk_13 != 0 && e->mUnk_13 != 0xff) {
                Unk_020c2694 *half = &gTextureBlocks.mLayers[e->mUnk_13 & 1];
                RemapPaletteRows((u16 *) dst, &half->mUnk_115[half->mUnk_014[e->mUnk_13 >> 1]], e->mUnk_08 >> 1);
            }
            e = list[i];
            if ((e->mUnk_04 >> 28) == 10 || bg == 1) {
                u32 w = (u32) ((1 << (e->mUnk_12 & 0xf)) / 8);
                u32 h = e->mUnk_08 / (e->mUnk_13 == 0 ? 0x20 : 0x40) / w;
                if (tex == 0) {
                    tex = AllocWithFlags(0x4000, 1);
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
                e->mUnk_04 = gSystemState.mUnk_0a0.mAddr + (addr & 0x1fff);
                func_0201c78c();
                *gSystemState.mUnk_0a0.mCtrl = 0x80;
            } else if (type == 9) {
                bits |= 0x200;
                e->mUnk_04 = gSystemState.mUnk_0ac.mAddr + (addr & 0x1fff);
                func_0201c78c();
                *gSystemState.mUnk_0ac.mCtrl = 0x80;
            } else if (type == 11) {
                if (gSystemState.mPlttVramGroup.mBankCount != 0) {
                    saved = addr;
                    e->mUnk_04 = gSystemState.mPlttVramGroup.mAddr[0] + (addr & 0x7fffff);
                    bits |= 0x800;
                    part = LimitTransferSize(list[i]->mUnk_08);
                    e = list[i];
                    rest = e->mUnk_08 - part;
                    e->mUnk_08 = part;
                    *gSystemState.mPlttVramGroup.mCtrl[0] = 0x80;
                } else {
                    rest = 0;
                    goto next;
                }
            } else if (type == 10) {
                VramGroup *group = &gSystemState.mTexVramGroup;
                if (gSystemState.mTexVramGroup.mBankCount != 0) {
                    saved = addr;
                    e->mUnk_04 = group->mAddr[0] + (addr & 0x7fffff);
                    bits |= 0x400;
                    part = LimitTransferSize(list[i]->mUnk_08);
                    e = list[i];
                    rest = e->mUnk_08 - part;
                    e->mUnk_08 = part;
                    for (n = 0; n < group->mBankCount; n++) {
                        Unk_LoadEntry *f = list[i];
                        if (group->mAddr[n] <= f->mUnk_04 + f->mUnk_08 &&
                            group->mAddr[n] + (group->mSizeKb[n] << 10) >= f->mUnk_04) {
                            *gSystemState.mTexVramGroup.mCtrl[n] = 0x80;
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

                for (k = 0; k < gSystemState.mTexVramGroup.mBankCount; k++) {
                    *gSystemState.mTexVramGroup.mCtrl[k] = gSystemState.mTexVramGroup.mCtrlValue[k];
                }
                bits &= ~0x400;
            } else if (bits & 0x800) {
                *gSystemState.mPlttVramGroup.mCtrl[0] = gSystemState.mPlttVramGroup.mCtrlValue[0];
                bits &= ~0x800;
            } else if (bits & 0x20) {
                *gSystemState.mUnk_0a0.mCtrl = gSystemState.mUnk_0a0.mCtrlValue;
                bits &= ~0x20;
            } else if (bits & 0x200) {
                *gSystemState.mUnk_0ac.mCtrl = gSystemState.mUnk_0ac.mCtrlValue;
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

    Free(&tmp);
    Free(&tex);
    Free(&buf);
    Free(&list);
    if (busy == 1) {
        i = 0;
        while (1) {
            if (i == gSystemState.mTransferCount) {
                return;
            }
            e = &gSystemState.mTransfers[i];
            if (e->mUnk_08 == 0 || e->mUnk_12 == 0xff || gTextureBlocks.mBlockFile == e->mUnk_10) {
                if (i != gSystemState.mTransferCount - 1) {
                    memcpy(e, &gSystemState.mTransfers[gSystemState.mTransferCount - 1], sizeof(Unk_LoadEntry));
                }
                gSystemState.mTransferCount--;
            } else {
                i++;
            }
        }
    }
}
#pragma thumb on

void SeekBank(int offset) {
    if ((gSystemState.mFlags & 0x200) == 0) {
        func_020425f4(&gBankFile, offset, 0);
    } else {
        gSystemState.mUnk_250.mUnk_00 = offset;
    }
}

void ReadBank(void *dst, u32 size) {
    int remaining;
    int state;
    int i;
    int j;
    u32 len;
    u32 sum;
    u8 *cmd = data_020c67e0;

    if ((gSystemState.mFlags & 0x200) == 0) {
        func_020425e4(&gBankFile, dst, size);
        return;
    }
restart:
    state = 0;
    i = 0;
    remaining = size;
    (void) (size <= 0xfb800 || (OS_Terminate(), 0));
    for (; remaining > 0; remaining -= 0x1f7) {
        gSystemState.mUnk_250.mUnk_24[i >> 3] &= ~(1 << (i & 7));
        i++;
    }
    gSystemState.mUnk_250.mUnk_24[i >> 3] |= 1 << (i & 7);
    gSystemState.mUnk_250.mUnk_08 = size;
    gSystemState.mUnk_250.mUnk_04 = size;
    gSystemState.mUnk_250.mUnk_0c = dst;
    gSystemState.mFlags |= 3 << 18;
    remaining = size;
    while (gSystemState.mUnk_250.mUnk_08 != 0 && (s8) gSystemState.mUnk_2a4[1] >= 0) {
        if (gSystemState.mUnk_250.mUnk_08 == remaining) {
            if (state-- == 0) {
                i = 0;
                while (gSystemState.mUnk_250.mUnk_24[i >> 3] & (1 << (i & 7))) {
                    i++;
                }
                j = i;
                sum = 0;
                while (!(gSystemState.mUnk_250.mUnk_24[j >> 3] & (1 << (j & 7)))) {
                    j++;
                    sum += 0x1f7;
                }
                len = (j - i) * 0x1f7;
                if (len >= gSystemState.mUnk_250.mUnk_04) {
                    len = gSystemState.mUnk_250.mUnk_04;
                }
                cmd[0] = 0x51;
                func_02034ce0(gSystemState.mUnk_250.mUnk_00 + 0x1f7 * i, cmd + 6);
                func_02034ce0(len, cmd + 2);
                state = 5;
                if (func_02034cf0(cmd + 6) <= gSystemState.mUnk_250.mUnk_00 + gSystemState.mUnk_250.mUnk_04) {
                } else {
                    goto restart;
                }
            }
        } else {
            remaining = gSystemState.mUnk_250.mUnk_08;
            state = 5;
        }
        UpdateSystemAndWait();
    }
    gSystemState.mUnk_250.mUnk_00 += size;
    gSystemState.mFlags &= 0xfff3ffff;
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
const u16 gLayerWidths[4] = {0x80, 0x100, 0x200, 0x200};
const u16 gLayerHeights[4] = {0x80, 0x100, 0x100, 0x200};

int GetLayerWidth(int layer) {
    if (layer != 1 && layer != 2) {
        OS_Terminate();
    }
    if (layer & 1) {
        return gLayerWidths[*(vu16 *) 0x0400000e >> 14];
    }
    return gLayerWidths[*(vu16 *) 0x0400100e >> 14];
}

int GetLayerRowSize(int layer) {
    int n = GetLayerWidth(layer);

    if (func_0201db58(layer) == 0x8000) {
        n <<= 1;
    }
    return n;
}

#pragma thumb off
void CopyStripRows(u16 *src, u32 dst, u32 width, u32 stride, u8 *lut) {
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

void CopyStripToLayers(u16 *src, u32 layers, u32 row, u32 width, u32 total, u8 *lut) {
    u32 half;
    int n;

    switch (layers & 3) {
        case 1:
            n = GetLayerWidth(layers);
            CopyStripRows(src, 0x06000000 + row * n * 2, width, n, lut);
            break;
        case 2:
            n = GetLayerWidth(layers);
            CopyStripRows(src, 0x06200000 + row * n * 2, width, n, lut);
            break;
        case 3:
            half = total >> 1;
            if (row < half) {
                n = GetLayerWidth(1);
                CopyStripRows(src, 0x06000000 + row * n * 2, width, n, lut);
            } else {
                n = GetLayerWidth(2);
                CopyStripRows(src, 0x06200000 + (row - half) * n * 2, width, n, lut);
            }
            break;
    }
}

void CopyImageToLayer(u8 *src, u32 dst, u32 width, u32 rows) {
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
    step = GetLayerRowSize(layer);
    while (rows != 0) {
        MIi_CpuCopy16(src, (void *) dst, width);
        src += width;
        dst += step;
        rows--;
    }
}

void LoadImage(int id, int target) {
    LoadImageEx(id, target, 0, 0);
}

void LoadImageEx(int id, int target, int x, u16 flags) {
    int found;
    void *buf;

    found = FindCachedFile(id);
    if (found > 0) {
        LoadImageData((u8 *) found, target, x, flags);
    } else {
        buf = (void *) LoadFile(id);
        LoadImageData(buf, target, x, flags);
        Free(&buf);
    }
    if (target == 3) {
        gSystemState.mImageIds[1] = id;
        gSystemState.mImageIds[0] = gSystemState.mImageIds[1];
    } else if (target == 1) {
        gSystemState.mImageIds[0] = id;
    } else if (target == 2) {
        gSystemState.mImageIds[1] = id;
    }
}

void LoadImageData(u8 *data, int target, int x, u16 flags) {
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
        gSystemState.mImageWidth = w;
        gSystemState.mImageHeight = h;
        gSystemState.mUnk_2d0 = 0x8000;
        height = h;
        if ((target & 3) == 3 && h >= 0x180) {
            height >>= 1;
        }
        if (x == 0) {
            if (!(flags & 2)) {
                SetupImageLayer(target, 1, w, height);
            }
        } else {
            if (!(flags & 2)) {
                SetupImageLayer(target, 0, w, height);
            }
            tmp = CreateNearestColorTable(x, target);
        }
        if (target & 1) {
            func_0201e56c(data, target, tmp);
        }
        if (target & 2) {
            func_0201e56c(data, 2, tmp);
        }
        Free(&tmp);
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
        gSystemState.mImageWidth = w;
        gSystemState.mImageHeight = h;
        mode = target & 3;
        if (mode == 3 && h >= 0x180) {
            h2 = h >> 1;
        }
        if (count != 0) {
            if (!(flags & 1)) {
                gSystemState.mUnk_2d0 = 0x100;
                if (!(flags & 2)) {
                    SetupImageLayer(target, 0, w, h2);
                }
                pix = UploadPalette(data + 0x20, count, 0, target);
                DC_StoreRange(pix, w * h + 0x320);
                size = w * h2;
            } else {
                pal = data + 0x20;
                src = pal + count * 3;
                gSystemState.mUnk_2d0 = 0x8000;
                if (!(flags & 2)) {
                    SetupImageLayer(target, 1, w, h2);
                }
                DC_StoreRange(data, w * h + 0x320);
                size = w * h2 * 2;
                rgb = Alloc(w * h * 2);
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
            gSystemState.mUnk_2d0 = 0x8000;
            if (!(flags & 2)) {
                SetupImageLayer(target, 1, w, h2);
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
                CopyImageToLayer(pix, 0x06000000, w, h);
                break;
            case 2:
                CopyImageToLayer(pix, 0x06200000, w, h);
                break;
            case 3:
                CopyImageToLayer(pix, 0x06000000, w, h2);
                if (h < 0x180) {
                    size = 0;
                }
                CopyImageToLayer(pix + size, 0x06200000, w, h2);
                break;
        }
    }
    Free(&rgb);
}

// JPEG reader state (the asm block decoder func_02028980 uses it too): the count of buffered bits, the read
// pointer (just past the marker of the current segment) and the bit buffer; gArenaLo is the arena start and
// gPmCallbackInfo the sleep callback node the start-up code keeps.
u8 data_020c1818;
void *gArenaLo;
u8 *data_020c1820;
u32 data_020c1824;
PMCallbackInfo gPmCallbackInfo;

// Word address of each bank's control register, shared by up to four banks.
const u32 gVramCtrlAddrs[9] = {
    0x04000240, 0x04000240, 0x04000240, 0x04000240, 0x04000244, 0x04000244, 0x04000244, 0x04000248, 0x04000248,
};

// VRAM address of each use, taken when the caller passes -1.
const u32 gVramUseAddrs[13] = {
    0x06800000, 0x00000000, 0x06000000, 0x06400000, 0x00000000, 0x00000000, 0x06200000,
    0x06600000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// Blocks per MCU and quantisation table of each colour component (4:2:0 Y, Cb, Cr).
const u8 data_020502a4[4] = {4, 1, 1, 0};

const u8 data_020502a8[4] = {0, 1, 1, 0};

// Low and high bound of each of the four microphone bands. Defined here, after data_020502a8: the compiler sorts
// the file's constant objects by size and orders equal sizes by the definition list, and this place gives the
// original .rodata layout (with the microphone code or in address order, gLayerWidths or data_020502a4 move).
const u16 gMicBandRanges[8] = {90, 150, 150, 200, 200, 300, 300, 400};

const u8 gVramCtrlBitOffsets[9] = {0, 8, 16, 24, 0, 8, 16, 0, 8};

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
const u16 gHashSteps[128] = {
    1,   3,   5,   7,   11,  13,  17,  19,  23,  29,  31,  37,  41,  43,  47,  53,  59,  61,  67,  71,  73,  79,
    83,  89,  97,  101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193,
    197, 199, 211, 223, 227, 229, 233, 239, 241, 251, 257, 263, 269, 271, 277, 281, 283, 293, 307, 311, 313, 317,
    331, 337, 347, 349, 353, 359, 367, 373, 379, 383, 389, 397, 401, 409, 419, 421, 431, 433, 439, 443, 449, 457,
    461, 463, 467, 479, 487, 491, 499, 503, 509, 521, 523, 541, 547, 557, 563, 569, 571, 577, 587, 593, 599, 601,
    607, 613, 617, 619, 631, 641, 643, 647, 653, 659, 661, 673, 677, 683, 691, 701, 709, 719,
};

// VRAM bank records, each bank's LCDC record first; a record with size 0 ends the table.
const VramBankRecord gVramBankRecords[] = {
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

    ncomp = 0;
    height = 0;
    width = 0;
    data_020c1820 = data + 4;
    huffsize = Alloc(0x300);
    buf = huffsize;
    huffcode = (u16 *) (buf + 0x100);
    block = Alloc(0x80);
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
                    for (n = 0; n < 64; n++) {
                        quant[t][data_020502b8[n]] = *q++;
                    }
                    total -= 65;
                }
                break;
            case 0xc4:
                q = data_020c1820;
                x = q[1] + (q[0] << 8);
                q += 2;
                x -= 2;
                while (x != 0) {
                    si = *q++;
                    total = 0;
                    t = (si & 1) + (si >> 3);
                    for (l = 1; l <= 16; l++) {
                        bits[t][l] = *q++;
                        total += bits[t][l];
                    }
                    for (n = 0; n < total; n++) {
                        huffval[t][n] = *q++;
                    }
                    x -= total + 17;
                    k = 0;
                    for (si = 1; si <= 16; si++) {
                        l = bits[t][si];
                        while (l--) {
                            huffsize[k++] = si;
                        }
                    }
                    huffsize[k] = 0;
                    code = 0;
                    si = huffsize[0];
                    k = 0;
                    while (huffsize[k] != 0) {
                        while (huffsize[k] == si) {
                            huffcode[k] = code;
                            k++;
                            code++;
                        }
                        code <<= 1;
                        si++;
                    }
                    k = 0;
                    for (l = 1; l <= 16; l++) {
                        if (bits[t][l] != 0) {
                            valptr[t][l] = k;
                            mincode[t][l] = huffcode[k];
                            k += bits[t][l] - 1;
                            maxcode[t][l] = huffcode[k];
                            k++;
                        } else {
                            maxcode[t][l] = -1;
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
    out = Alloc(0x4000);
    if (width > 0x200) {
        OS_Terminate();
    }
    for (t = 0; t < (ncomp == 1 ? 1 : 2); t++) {
        for (k = 0; k < 64; k++) {
            quant[t][k] = (quant[t][k] * data_020502f8[k]) >> 12;
        }
    }
    mcu = Alloc(0x1c0);
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
        CopyStripToLayers(out, target, y, width, height, lut);
    }
end:
    Free(&out);
    Free(&mcu);
    Free(&buf);
    Free(&block);
}
#pragma thumb on

void ResetVramBanks(void) {
    u32 i;

    for (i = 0; i < 9; i++) {
        gSystemState.mVramBankRecordIndex[i] = 0xff;
        *(vu8 *) (gVramCtrlAddrs[i] + (gVramCtrlBitOffsets[i] / 8)) = 0;
    }
    gSystemState.mVramBanksChanged = 1;
    gSystemState.mImageIds[0] = gSystemState.mImageIds[1] = 0xffff;
}

void FreeVramBanks(int type) {
    u32 i;

    if (type == 2) {
        gSystemState.mImageIds[0] = 0xffff;
    } else if (type == 6) {
        gSystemState.mImageIds[1] = 0xffff;
    }
    for (i = 0; i < 9; i++) {
        if (gSystemState.mVramBankRecordIndex[i] != 0xff &&
            type == gVramBankRecords[gSystemState.mVramBankRecordIndex[i]].mUse) {
            *(vu8 *) (gVramCtrlAddrs[i] + (gVramCtrlBitOffsets[i] / 8)) = 0;
            gSystemState.mVramBankRecordIndex[i] = 0xff;
        }
    }
    gSystemState.mVramBanksChanged = 1;
}

void ReallocVramBanks(int type, int size, int addr) {
    if (type == 10) {
        gSystemState.mTexVramGroup.mSize16Kb = 0;
    } else if (type == 11) {
        gSystemState.mPlttVramGroup.mSize16Kb = 0;
    }
    FreeVramBanks(type);
    AllocVramBanks(type, size, addr);
}

void AllocVramBanks(int type, u32 size, u32 addr) {
    int i;
    const VramBankRecord *rec;
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
        addr = gVramUseAddrs[type];
    }
    for (i = 0; i < 9; i++) {
        if (gSystemState.mVramBankRecordIndex[i] != 0xff) {
            used |= 1 << i;
        }
    }
    i = 0;
    best = -1;
    for (; gVramBankRecords[i].mSizeKb != 0; i++) {
        if (type == gVramBankRecords[i].mUse && !((1 << gVramBankRecords[i].mBank) & used) &&
            addr == gVramBankRecords[i].mAddr && gVramBankRecords[i].mSizeKb >= size) {
            if (best == -1 || gVramBankRecords[i].mSizeKb < gVramBankRecords[best].mSizeKb) {
                best = i;
            }
        }
    }
    if (best != -1 && size == gVramBankRecords[best].mSizeKb) {
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
            for (; rec = &gVramBankRecords[k], gVramBankRecords[k].mSizeKb != 0; k++) {
                if (type == rec->mUse) {
                    u32 bit = 1 << rec->mBank;

                    if (!(used & bit) && rec->mAddr == addrs[n]) {
                        cur = rem[n];
                        rem[n + 1] = cur - rec->mSizeKb;
                        addrs[n + 1] = rec->mAddr + (rec->mSizeKb << 10);
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
            used &= ~(1 << gVramBankRecords[k].mBank);
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
        for (h = k; gVramBankRecords[h].mUse != 0; h--) {
        }
        bank = gVramBankRecords[k].mBank;
        *(vu8 *) (gVramCtrlAddrs[bank] + gVramCtrlBitOffsets[bank] / 8) = 0x80;
        kb = gVramBankRecords[h].mSizeKb;
        MI_DmaFill32(3, (void *) gVramBankRecords[h].mAddr, 0, kb << 10);
        *(vu8 *) (gVramCtrlAddrs[bank] + gVramCtrlBitOffsets[bank] / 8) =
            (gVramBankRecords[k].mUnk_9 << 3) + 0x80 + gVramBankRecords[k].mUnk_8;
        gSystemState.mVramBankRecordIndex[bank] = k;
        if (type == 10) {
            gSystemState.mTexVramGroup.mSize16Kb += kb >> 4;
        } else if (type == 11) {
            gSystemState.mPlttVramGroup.mSize16Kb += kb >> 4;
        }
    }
    gSystemState.mVramBanksChanged = 1;
}

#pragma thumb off
u16 ColorDistanceSquared(u8 (*pal)[3], int c0, int c1, int c2, int index) {
    int d2 = pal[index][2] - c2;
    int d1 = pal[index][1] - c1;
    int d0 = pal[index][0] - c0;

    return d2 * d2 + d1 * d1 + d0 * d0;
}

void *CreateNearestColorTable(int id, int target) {
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

    src = FindOrLoadCachedFile(id);
    pal = Alloc(0x300);
    cube = Alloc(0x8000);
    dist = Alloc(0x10000);
    queue = Alloc(0x8000);
    tail = 0;
    head = 0;
    UploadPalette(src, 0x100, 0, target);
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
                d = ColorDistanceSquared(pal, r - 1, g, b, idx);
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
                d = ColorDistanceSquared(pal, r + 1, g, b, idx);
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
                d = ColorDistanceSquared(pal, r, g - 1, b, idx);
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
                d = ColorDistanceSquared(pal, r, g + 1, b, idx);
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
                d = ColorDistanceSquared(pal, r, g, b - 1, idx);
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
                d = ColorDistanceSquared(pal, r, g, b + 1, idx);
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
    Free(&pal);
    Free(&dist);
    Free(&queue);
    return cube;
}
#pragma thumb on

void SetupImageLayer(int a, int b, u32 c, u32 d) {
    u32 cls = 0;
    u32 size = gLayerWidths[0];
    u32 rows;
    if (c > gLayerWidths[0] || d > gLayerHeights[0]) {
        cls = 1;
        size = gLayerWidths[1];
        if (c > gLayerWidths[1] || d > gLayerHeights[1]) {
            cls = 2;
            size = gLayerWidths[2];
            if (c > gLayerWidths[2] || d > gLayerHeights[2]) {
                cls = 3;
            }
        }
    }
    rows = (size * d) >> 10;
    if (a & 1) {
        *(vu32 *) 0x04000000 =
            (gTextureBlocks.mLayers[0].mUnk_57b << 31) + (gTextureBlocks.mLayers[0].mUnk_577 << 20) + 0x11833;
        gSystemState.mUnk_474[0] &= ~4;
        gSystemState.mBg3X = 0;
        gSystemState.mBg3Y = 0;
        gSystemState.mBg3Pa = 0x100;
        gSystemState.mBg3Pd = 0x100;
        *(vu16 *) 0x04000030 = 0x100;
        *(vu16 *) 0x04000036 = 0x100;
        *(vu16 *) 0x04000032 = 0;
        *(vu16 *) 0x04000034 = 0;
        *(vu32 *) 0x04000038 = 0;
        *(vu32 *) 0x0400003c = 0;
        switch (b) {
            case 0:
                ReallocVramBanks(2, rows, -1);
                *(vu16 *) 0x0400000e = (cls << 14) + 0x80;
                break;
            case 1:
                ReallocVramBanks(2, rows * 2, -1);
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
            (gTextureBlocks.mLayers[1].mUnk_57b << 31) + (gTextureBlocks.mLayers[1].mUnk_577 << 20) + 0x11813;
        gSystemState.mUnk_474[0] &= ~8;
        gSystemState.mSubBg3X = 0;
        gSystemState.mSubBg3Y = 0;
        *(vu16 *) 0x04001030 = 0x100;
        *(vu16 *) 0x04001036 = 0x100;
        *(vu16 *) 0x04001032 = 0;
        *(vu16 *) 0x04001034 = 0;
        *(vu32 *) 0x04001038 = 0;
        *(vu32 *) 0x0400103c = 0;
        switch (b) {
            case 0:
                ReallocVramBanks(6, rows, -1);
                *(vu16 *) 0x0400100e = (cls << 14) + 0x80;
                break;
            case 1:
                ReallocVramBanks(6, rows * 2, -1);
                *(vu16 *) 0x0400100e = (cls << 14) + 0x84;
                break;
        }
        MIi_CpuClear16(0, (void *) 0x05000400, 0x200);
    }
}

u8 *UploadPalette(u8 *palette, u32 count, int a, int target) {
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
void UpdateFade(void) {
    u8 unused[2] = {1, 1};
    vu16 *regs[2] = {(vu16 *) 0x0400006c, (vu16 *) 0x0400106c};
    int i;

    for (i = 0; i < 2; i++) {
        if (gSystemState.mFadeState[i] == FADE_STATE_TO_CLEAR) {
            u32 level = *regs[i] & 0x1f;
            if (level != 0) {
                *regs[i] = 0x8000 | (level - 1);
            } else {
                *regs[i] = 0;
                gSystemState.mFadeState[i] = FADE_STATE_CLEAR;
            }
        } else if (gSystemState.mFadeState[i] == FADE_STATE_TO_FADED) {
            u32 level = *regs[i] & 0x1f;
            if (level < 0x10) {
                *regs[i] = 0x8000 | (level + 1);
            } else {
                *regs[i] = 0x8011;
                gSystemState.mFadeState[i] = FADE_STATE_FADED;
            }
        }
    }
}

void func_0201f6d0(int a, int b, int c, u16 d, u16 e) {
    int phase = 0;
    int touched = 0;
    u32 frames = 0;
    u32 step = 0;

    SetFaded(3);
    LoadImage(d, 3);
    func_0202c1c4();
    func_0202bdd8(b, c, a, 0, 1);
    SetLcdSwap(1);
    while (func_0201f910(&frames, &touched, 0x3c, 0x78, 3) == 0) {
        if (phase == 0) {
            if ((gSystemState.mElapsedFrames & 3) == 0) {
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
        UpdateSystemAndWait();
        func_0202cf08(e + step, 0x2c, 0x5d, 0, 1);
    }
    gSystemState.mFlags |= 0x40000;
    FadeAndWait(3);
    gSystemState.mFlags &= ~0x40000;
}

int IsFadeState(int screens, int state) {
    if ((screens & 1) && gSystemState.mFadeState[0] != state) {
        return 0;
    }
    if ((screens & 2) && gSystemState.mFadeState[1] != state) {
        return 0;
    }
    return 1;
}

int TestBits(int a, int b) {
    if (a & b) {
        return 1;
    }
    return 0;
}

void StartFade(int a) {
    if (TestBits(a, 1) == 1) {
        gSystemState.mFadeState[0] = FADE_STATE_TO_FADED;
    }
    if (TestBits(a, 2) == 1) {
        gSystemState.mFadeState[1] = FADE_STATE_TO_FADED;
    }
}

void StartFadeClear(int a) {
    if (TestBits(a, 1) == 1) {
        if (gSystemState.mFadeState[0] != FADE_STATE_CLEAR) {
            gSystemState.mFadeState[0] = FADE_STATE_TO_CLEAR;
        }
    }
    if (TestBits(a, 2) == 1) {
        if (gSystemState.mFadeState[1] != FADE_STATE_CLEAR) {
            gSystemState.mFadeState[1] = FADE_STATE_TO_CLEAR;
        }
    }
}

void FadeClearAndWait(int a) {
    StartFadeClear(a);
    while (IsFadeState(a, FADE_STATE_CLEAR) != 1) {
        UpdateSystemAndWait();
    }
}

void FadeAndWait(int a) {
    while (IsFadeState(a, FADE_STATE_FADED) != 1) {
        StartFade(a);
        UpdateSystemAndWait();
    }
}

void SetFaded(int a) {
    if (TestBits(a, 1) == 1) {
        gSystemState.mFadeState[0] = FADE_STATE_FADED;
        *(vu16 *) 0x0400006c = 0x8011;
        func_0202ae34(1);
    }
    if (TestBits(a, 2) == 1) {
        gSystemState.mFadeState[1] = FADE_STATE_FADED;
        *(vu16 *) 0x0400106c = 0x8011;
        func_0202ae34(2);
    }
}

void SetFadeClear(int a) {
    if (TestBits(a, 1) == 1) {
        gSystemState.mFadeState[0] = FADE_STATE_CLEAR;
        *(vu16 *) 0x0400006c = 0;
    }
    if (TestBits(a, 2) == 1) {
        gSystemState.mFadeState[1] = FADE_STATE_CLEAR;
        *(vu16 *) 0x0400106c = 0;
    }
}

int func_0201f910(u32 *frames, int *touched, u32 minFrames, u32 maxFrames, int layers) {
    if ((gSystemState.mKeysDown & 9) || ReadTouchAndAppendTrail(0, 0) == 1) {
        *touched = 1;
    }
    if (!(gSystemState.mKeysDown & 9) && ReadTouchAndAppendTrail(0, 0) == 0 && *frames > minFrames && *touched == 1) {
        return 1;
    }
    if (*frames >= maxFrames) {
        return 1;
    }
    if (IsFadeState(layers, FADE_STATE_CLEAR) == 1) {
        (*frames)++;
    }
    return 0;
}

// Shows image `a` on the screens in mask e: waits for FADE_STATE_FADED, loads the image and starts the fade to clear,
// then waits until a key (bits 0 or 3) or the touch is released after more than b frames, or until c frames have
// passed (frames count while the screens are clear). Then d == 0 sets the screens faded, d == 2 fades them and waits,
// d == 1 leaves them; the VRAM banks of uses 2 and 6 are freed for screens 1 and 2.
void RunImage(int a, int b, int c, int d, int e) {
    u32 frames = 0;
    int touched = 0;

    FadeAndWait(e);
    LoadImage(a, e);
    StartFadeClear(e);
    while (func_0201f910(&frames, &touched, b, c, e) == 0) {
        UpdateSystemAndWait();
    }
    if (d != 0) {
        if (d == 2) {
            FadeAndWait(e);
        }
    } else {
        SetFaded(e);
    }
    if (e & 1) {
        FreeVramBanks(2);
    }
    if (e & 2) {
        FreeVramBanks(6);
    }
}

#pragma thumb off
void MoveIndexNode16ToEnd(IndexNode16 *list, u32 index) {
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
void MoveIndexNodeToEnd(IndexNode *list, u32 index) {
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
int InterpolateQuadratic(int a, int b, int c, int t) {
    int u = 1024 - t;
    int x = a * u + c * 2 * t;
    int y = u * x + b * t * t;
    return y / 1024 / 1024;
}

u32 LengthSquared2d(int x, int y) {
    return x * x + y * y;
}

u32 Length2d(int x, int y) {
    return SqrtU32(LengthSquared2d(x, y));
}

s64 LengthSquared2d64(int x, int y) {
    return (s64) x * x + (s64) y * y;
}

// Moves *value toward 0 (speed < 0) or LEVEL_FULL (speed > 0) by 1/|speed| of the distance when it is farther than
// 2 * |speed| away; otherwise it lands on the step directly.
void EaseLevel(u16 *value, int speed) {
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

void AddLevel(u16 *p, int add) {
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
int MapRangeFromZero(int a, int b, int c, int d) {
    int r;
    int bitsRange;
    int bitsA;
    if (a == 0) {
        return c;
    }
    if (a == b) {
        return d;
    }
    bitsRange = 32 - LeadingZeroCount(abs(d - c));
    bitsA = 32 - LeadingZeroCount(a);
    if (bitsRange + bitsA > 30) {
        r = (s64) a * ((s64) d - c) / b;
    } else {
        r = DivideRounded((d - c) * a, b);
    }
    return c + r;
}

// Linear map of v from [a, b] to [c, d].
int MapRange(int v, int a, int b, int c, int d) {
    if (a == b) {
        OS_Terminate();
    }
    return c + DivideRounded((v - a) * (d - c), b - a);
}

char *FormatDecimal(int value, char *buf) {
    return FormatDecimalGrouped(value, buf, 0);
}

// Writes value in decimal to buf, with separator after the digit of each thousands place unless it is 0.
char *FormatDecimalGrouped(int value, char *buf, char separator) {
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
int DivideRounded(int a, int b) {
    int r;
    if (b == 0) {
        return 0x0fffffff ^ (a >> 31);
    }
    r = Divide(a * 2, b);
    return (r + (r >> 31) + 1) >> 1;
}

u32 SqrtU32(u32 value) {
    *(volatile u16 *) 0x040002b0 = 0;
    *(volatile u32 *) 0x040002b8 = value;
    while (*(volatile u16 *) 0x040002b0 & MATH_BUSY) {
    }
    return *(volatile u32 *) 0x040002b4;
}

u32 SqrtU64(u64 value) {
    *(volatile u16 *) 0x040002b0 = 1;
    *(volatile u64 *) 0x040002b8 = value;
    while (*(volatile u16 *) 0x040002b0 & MATH_BUSY) {
    }
    return *(volatile u32 *) 0x040002b4;
}

u32 DivideWithRemainder(u32 numerator, u32 denominator, u32 *remainder) {
    *(volatile u16 *) 0x04000280 = 0;
    *(volatile u32 *) 0x04000290 = numerator;
    *(volatile u64 *) 0x04000298 = denominator;
    while (*(volatile u16 *) 0x04000280 & MATH_BUSY) {
    }
    *remainder = *(volatile u32 *) 0x040002a8;
    return *(volatile u32 *) 0x040002a0;
}

s32 Divide(s32 numerator, s32 denominator) {
    *(volatile u16 *) 0x04000280 = 0;
    *(volatile u32 *) 0x04000290 = numerator;
    *(volatile u64 *) 0x04000298 = (u32) denominator;
    while (*(volatile u16 *) 0x04000280 & MATH_BUSY) {
    }
    return *(volatile u32 *) 0x040002a0;
}

// Advances the generator state: add a constant, rotate right by 3.
u32 NextRandom(void) {
    u32 s = gSystemState.mRandomSeed + 0x5cfae23d;
    s = (s >> 3) + (s << 29);
    gSystemState.mRandomSeed = s;
    return s;
}

// Random value between lo and hi. When lo == hi nothing is computed and lo is still in r0; adding `return lo;`
// changes the code.
u32 RandomRange(u32 lo, u32 hi) {
    if (lo != hi) {
        return MapRangeFromZero((u16) NextRandom(), ANGLE_FULL, lo, hi);
    }
}

// Mixes the current scanline into the generator state.
void MixRandomSeed(void) {
    gSystemState.mRandomSeed += *(vu16 *) 0x04000006;
}

// Reads the bits lo..hi (inclusive, bit 0 = lowest bit of the first byte) of a byte array.
u32 ReadBits(const void *data, u32 lo, u32 hi) {
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
u32 AngleDistance(u32 a, u32 b) {
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

// Shortest signed distance from a to b (b - a, taken the short way round).
int AngleDelta(u32 a, u32 b) {
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

// 1 when the short way from `from` to `to` runs upward.
int IsAngleIncreasing(u16 to, u16 from) {
    u32 d;
    int up;
    if (from < to) {
        d = to - from;
        up = 1;
    } else {
        d = from - to;
        up = 0;
    }
    if (d > ANGLE_FULL - d) {
        up ^= 1;
    }
    return up;
}

// Moves b toward a by half the shortest distance.
u32 AngleHalfway(u16 a, u16 b) {
    u32 d = AngleDistance(a, b);
    int up = IsAngleIncreasing(a, b);
    return (u16) (b + (up * 2 - 1) * (d >> 1));
}

u32 AngleOfVector(s32 x, s32 y) {
    return FX_Atan2Idx(x, -y);
}

// Cross product of the vectors b->a and b->c.
int Cross2d(int ax, int ay, int bx, int by, int cx, int cy) {
    return (ax - bx) * (cy - by) - (cx - bx) * (ay - by);
}

void func_0201fed4(int a) {
    u32 elapsed;
    u32 flag;
    u32 limit;
    if (gSystemState.mUnk_32e == 0) {
        return;
    }
    if (gSystemState.mUnk_32e == 4 && (a & 1) == 0) {
        return;
    }
    elapsed = gSystemState.mElapsedFrames - gSystemState.mUnk_324;
    flag = 0;
    limit = gSystemState.mUnk_457;
    if (limit == 0) {
        limit = 180;
    }
    while (func_0201f910(&elapsed, &flag, 60, limit, 3) == 0) {
        UpdateSystemAndWait();
    }
    gSystemState.mUnk_450 |= 4;
    while (gSystemState.mUnk_32e == 3 || gSystemState.mUnk_32e == 10 || gSystemState.mUnk_32e == 4) {
        UpdateSystemAndWait();
    }
    gSystemState.mUnk_32f = gSystemState.mUnk_32e;
    gSystemState.mUnk_32e = 0;
    gSystemState.mFlags &= 0xfff7ffff;
}

int GetSaveBlockSize(u32 id) {
    return gSystemState.mSaveStoreSize / func_02021e44(id, gSystemState.mSaveLastField + 1);
}

int GetSaveFieldCount(void) {
    int d = gSystemState.mSaveLastField - gSystemState.mSaveFirstField;
    if (!(gSystemState.mSaveFirstField < gSystemState.mSaveLastField)) {
        OS_Terminate();
    }
    return d + 1;
}

void *AllocSaveRecord(void) {
    return Alloc(GetSaveFieldCount() * 4);
}

void FreeSaveRecord(void *p) {
    Free(p);
}

// Returns the 16-bit tag of a block: the table bytes at 2 * index and 2 * index + 1, each taken modulo the table's
// length.
u16 GetSaveBlockTag(int id, u32 index) {
    const u8 *table = func_02021e18(id, gSystemState.mSaveLastField + 2);
    u32 count = func_02021f0c(id, gSystemState.mSaveLastField + 2, 0);
    if (count < 2) {
        OS_Terminate();
    }
    return (table[(index * 2) % count] << 8) + table[(index * 2 + 1) % count];
}

// Sets up the block store for set a: kind, first and last field, status callback and the value for mUnk_454 (-1
// leaves it; otherwise the callback is also called with 4). Reads every block, writes a zero record
// (WriteZeroSaveBlock) over each one that does not read back intact, then waits for the store.
void InitSaveStore(u32 a, int b, u16 c, int d, void (*fn)(u32), int e) {
    u32 count = func_02021e44(a, d + 1);
    u32 r;
    u32 i;
    if (b != SAVE_STORE_SMALL && b != SAVE_STORE_LARGE) {
        OS_Terminate();
    }
    gSystemState.mSaveCallback = fn;
    gSystemState.mUnk_32e = 0;
    gSystemState.mUnk_32f = gSystemState.mUnk_32e;
    gSystemState.mSaveFirstField = c;
    gSystemState.mSaveLastField = d;
    gSystemState.mUnk_324 = 0;
    if (e != -1) {
        gSystemState.mUnk_454 = e;
        fn(4);
    }
    InitBackup(b);
    if (b == SAVE_STORE_SMALL) {
        gSystemState.mSaveStoreSize = SMALL_BLOCK_SIZE;
    } else {
        gSystemState.mSaveStoreSize = LARGE_BLOCK_SIZE;
    }
    // Volatile read of the frame counter; the value is not used.
    gSystemState.mElapsedFrames;
    for (i = 0; i < count; i++) {
        r = ReadSaveBlock(i, 0, a);
        if (r == BLOCK_ID_OK) {
            gSystemState.mSaveCallback(1);
        }
        if (r != BLOCK_INTACT) {
            if (gSystemState.mSaveStoreSize > SMALL_BLOCK_SIZE && r != BLOCK_ID_OK) {
                gSystemState.mSaveCallback(6);
            }
            WriteZeroSaveBlock(i, a);
        }
    }
    func_0201fed4(1);
    if (gSystemState.mUnk_32f == 1) {
        gSystemState.mSaveCallback(2);
        while (func_02020890() == 0) {
            UpdateSystemAndWait();
        }
        gSystemState.mFlags &= ~0xc0000;
    }
    gSystemState.mUnk_450 |= 4;
}

// Reads block index of set c. Bit 0 of the result: the checksum matches (the record is then unpacked into buf when
// buf is not null); bit 1: the stored tag matches the block's tag.
u32 ReadSaveBlock(u32 index, void *buf, int c) {
    u16 table[256];
    u16 crc;
    int size = GetSaveBlockSize(c);
    u8 *data = Alloc(size);
    u32 result = 0;
    int check = GetSaveBlockTag(c, index);
    if (index >= func_02021e44(c, gSystemState.mSaveLastField + 1)) {
        OS_Terminate();
    }
    if (ReadBackup(data, index * size, size) == 0) {
        if (gSystemState.mUnk_320 != 0) {
            memcpy(data, gSystemState.mUnk_320 + index * size, size);
        } else {
            gSystemState.mSaveCallback(10);
        }
    }
    func_02048780(table, CRC_POLY);
    crc = func_02048804(table, data + 4, size - 4);
    if (crc == data[0] + (data[1] << 8)) {
        if (buf != 0) {
            UnpackSaveFields(buf, (u32 *) (data + 4), c);
        }
        result |= BLOCK_CRC_OK;
    }
    if (data[2] == (check & 0xff) && data[3] == (check >> 8)) {
        result |= BLOCK_ID_OK;
    }
    if (gSystemState.mUnk_32e != 1) {
        func_0201fed4(0);
    }
    Free(&data);
    return result;
}

// Writes a zero-filled record (Alloc clears it) to block index of set c and sets the block's bit in
// mClearedSaveBlocks.
void WriteZeroSaveBlock(u32 index, int c) {
    void *buf = Alloc(GetSaveFieldCount() * 4);
    WriteSaveBlock(index, buf, c);
    Free(&buf);
    gSystemState.mClearedSaveBlocks[0] |= 1 << index;
}

// Packs the record in buf into block index of set c with its checksum and tag and writes the block.
void WriteSaveBlock(u32 index, void *buf, int c) {
    u16 table[256];
    u16 crc;
    int size = GetSaveBlockSize(c);
    u8 *data = Alloc(size);
    int payloadWritten = 1;
    int check = GetSaveBlockTag(c, index);
    int payload;
    u32 offset;
    int headWritten;
    if (index >= func_02021e44(c, gSystemState.mSaveLastField + 1)) {
        OS_Terminate();
    }
    memset(data, 0, size);
    PackSaveFields(buf, (u32 *) (data + 4), c);
    func_02048780(table, CRC_POLY);
    payload = size - 4;
    crc = func_02048804(table, data + 4, payload);
    memcpy(data, &crc, 2);
    data[2] = check;
    data[3] = check >> 8;
    if (size > 4) {
        payloadWritten = WriteBackup(data + 4, index * size + 4, payload);
    }
    offset = index * size;
    headWritten = WriteBackup(data, offset, 4);
    if (payloadWritten == 0 || headWritten == 0) {
        if (gSystemState.mUnk_320 != 0) {
            memcpy(gSystemState.mUnk_320 + offset, data, size);
        } else {
            gSystemState.mSaveCallback(4);
        }
    }
    if (gSystemState.mUnk_32e != 1) {
        func_0201fed4(1);
    }
    Free(&data);
}

void ClearBackupTimer(void) {
    gSystemState.mBackupTimer = 0;
}

#pragma thumb off
// Fills order with 0..count-1, ends it with ORDER_END and sorts it by descending key (shell sort).
void SortIndicesByKey(u32 *order, const u32 *key, int count) {
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
    gap = (1 << (32 - LeadingZeroCount(count))) - 1;
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
int PackSaveFields(const u32 *values, u32 *out, u32 id) {
    int fields;
    u32 *radix;
    u32 *order;
    int i;
    int outCount;
    u64 prod;
    u64 old;
    u32 acc;
    fields = GetSaveFieldCount();
    radix = Alloc(fields * 4);
    order = Alloc((fields + 1) * 4);
    for (i = 0; i < fields; i++) {
        radix[i] = func_02021e44(id, gSystemState.mSaveFirstField + i) + 1;
    }
    SortIndicesByKey(order, radix, fields);
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
    Free(&radix);
    Free(&order);
    return outCount * 4;
}

// Unpacks the words written by PackSaveFields back into the field values; the packed words are divided in place.
void UnpackSaveFields(u32 *out, u32 *packed, u32 id) {
    int fields = GetSaveFieldCount();
    u32 *radix = Alloc(fields * 4);
    u32 *order = Alloc((fields + 1) * 4);
    u32 list[32];
    int i;
    int w;
    int n;
    u64 prod;
    for (i = 0; i < fields; i++) {
        radix[i] = func_02021e44(id, gSystemState.mSaveFirstField + i) + 1;
    }
    SortIndicesByKey(order, radix, fields);
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
    Free(&radix);
    Free(&order);
}
#pragma thumb on

// Sets (a == 1) or clears the LCD swap bit of the power control register.
void SetLcdSwap(int a) {
    if (a == 1) {
        *(vu16 *) 0x04000304 |= POWCNT1_SWAP_LCDS;
    } else {
        *(vu16 *) 0x04000304 &= ~POWCNT1_SWAP_LCDS;
    }
}

int IsLcdSwapped(void) {
    if (*(vu16 *) 0x04000304 & POWCNT1_SWAP_LCDS) {
        return 1;
    }
    return 0;
}

int func_02020890(void) {
    if (gSystemState.mFadeState[0] == FADE_STATE_CLEAR && gSystemState.mFadeState[1] == FADE_STATE_CLEAR &&
        ((gSystemState.mKeysPressed & 1) || gSystemState.mTouchPressed == 1)) {
        gSystemState.mKeysPressed &= ~1;
        gSystemState.mTouchPressed = 0;
        return 1;
    }
    return 0;
}

// Sets the sound ids and key mask of the newest button (or of the first one when ids are inserted in front).
void SetTouchButtonSoundsAndKeys(int flags, u16 a, u16 b, u16 c) {
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

// Under SYSTEM_TOUCH_LOCK no button is added, but the sounds and key mask are still written to the newest button
// in the list.
void AddTouchButtonEx(int a, int b, int c, int d, int e, int f, u16 g, u16 h, u16 i) {
    AddTouchButtonWithSounds(a, b, c, d, e, f, g, h);
    SetTouchButtonSoundsAndKeys(a, g, h, i);
}

void AddTouchButtonWithSounds(int a, int b, int c, int d, int e, int f, u16 g, u16 h) {
    if (!(gSystemState.mFlags & SYSTEM_TOUCH_LOCK)) {
        AddTouchButton(a, b, c, d, e, f);
        SetTouchButtonSoundsAndKeys(a, g, h, 0);
    }
}

void AddTouchButton(int id, int x, int y, int a, int b, int c) {
    s32 rect[4];
    int index;
    int shifting;
    Unk_TouchButton *button;
    if (data_020c1d60.mUnk_800.mUnk_100.mUnk_08 >= TOUCH_BUTTON_COUNT) {
        OS_Terminate();
    }
    if (gSystemState.mFlags & SYSTEM_TOUCH_LOCK) {
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
        int mode = IsLcdSwapped() == 1 ? 2 : 1;
        func_0202cf08(data_020c1d60.mUnk_800.mUnk_100.mUnk_16, button->mUnk_0, button->mUnk_2, 0, mode);
        func_0202cf08(data_020c1d60.mUnk_800.mUnk_100.mUnk_16, button->mUnk_4, button->mUnk_6, 0, mode);
    }
    button->mUnk_8 = id;
    if (shifting == 0) {
        data_020c1d60.mUnk_800.mUnk_100.mUnk_08++;
    }
}

int IsTouchInShape(int x, int y, int shape) {
    if (gSystemState.mTouchDown) {
        return IsPointInShape(x, y, shape, gSystemState.mTouchX, gSystemState.mTouchY);
    }
    return 0;
}

typedef struct TouchPoint {
    s32 x;
    s32 y;
} TouchPoint;

// Tests whether the point (px, py) lies inside any of the polygons the shape table describes, placed at (x, y).
int IsPointInShape(int x, int y, int shape, int px, int py) {
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
                inside = inside * 2 + ((Cross2d(p[0].x, p[0].y, p[1].x, p[1].y, px, py) >> 31) & 1);
                p++;
            }
            if (inside == (1 << count) - 1) {
                return 1;
            }
        }
    }
    return 0;
}

void ClearTouchButtons(void) {
    data_020c1d60.mUnk_800.mUnk_100.mUnk_08 = 0;
}

// Polls the touch screen and the keys and returns the button state: the button id in the low half and the
// TOUCH_* flags above it. It then empties the button list and plays the hit button's sound on a press or release.
u32 UpdateTouchButtons(u32 mask) {
    u32 result = data_020c1d60.mUnk_800.mUnk_100.mUnk_12;
    u32 wanted = result;
    int hit = -1;
    u8 pos[2];
    u32 i;
    if (mask != TOUCH_NONE && mask != result) {
        wanted = mask;
    }
    if (ReadTouchAndAppendTrail(&pos[1], &pos[0]) == 1) {
        for (i = 0; i < data_020c1d60.mUnk_800.mUnk_100.mUnk_08; i++) {
            Unk_TouchButton *button = &data_020c1d60.mUnk_000[i];
            if (!(button->mUnk_8 & TOUCH_SHAPE)) {
                if (pos[0] >= button->mUnk_2 && pos[0] <= button->mUnk_6 && pos[1] >= button->mUnk_0 &&
                    pos[1] <= button->mUnk_4) {
                    hit = i;
                    data_020c1d60.mUnk_800.mUnk_100.mUnk_12 = data_020c1d60.mUnk_000[i].mUnk_8 & 0x7fff;
                    break;
                }
            } else if (IsTouchInShape(button->mUnk_0, button->mUnk_2, button->mUnk_6)) {
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
            (wanted != data_020c1d60.mUnk_800.mUnk_100.mUnk_12 && !(gSystemState.mFlags & SYSTEM_TOUCH_QUIET))) {
            result |= TOUCH_PRESSED;
            if (wanted == data_020c1d60.mUnk_800.mUnk_100.mUnk_12) {
                result |= TOUCH_REPEAT;
                if (gSystemState.mElapsedFrames - data_020c1d60.mUnk_800.mUnk_100.mUnk_0c < TOUCH_QUICK_FRAMES) {
                    result |= TOUCH_QUICK;
                }
            }
            if (data_020c1d60.mUnk_800.mUnk_100.mUnk_14 == 0) {
                data_020c1d60.mUnk_800.mUnk_100.mUnk_0c = gSystemState.mElapsedFrames;
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
        if ((u16) (gSystemState.mKeysDown | gSystemState.mKeysPressed | gSystemState.mKeysReleased) != 0) {
            for (i = 0; i < data_020c1d60.mUnk_800.mUnk_100.mUnk_08; i++) {
                u16 keys = data_020c1d60.mUnk_000[i].mUnk_e;
                if (keys != 0) {
                    if (gSystemState.mKeysDown & keys) {
                        result = (result & 0xffff0000) | data_020c1d60.mUnk_000[i].mUnk_8 | TOUCH_DOWN;
                    }
                    if (gSystemState.mKeysPressed & keys) {
                        result = (result & 0xffff0000) | data_020c1d60.mUnk_000[i].mUnk_8 | TOUCH_PRESSED;
                    }
                    if (keys & gSystemState.mKeysReleased) {
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

void ResetTouchTrail(void) {
    data_020c1d60.mUnk_800.mUnk_100.mUnk_18 = 0;
    data_020c1d60.mUnk_800.mUnk_100.mUnk_10 = 0;
    MIi_CpuClear16(0xffff, data_020c1d60.mUnk_800.mUnk_000, sizeof(data_020c1d60.mUnk_800.mUnk_000));
}

void ResetTouchButtons(u16 a, int b) {
    data_020c1d60.mUnk_800.mUnk_100.mUnk_12 = TOUCH_NONE;
    data_020c1d60.mUnk_800.mUnk_100.mUnk_08 = 0;
    data_020c1d60.mUnk_800.mUnk_100.mUnk_14 = 0;
    data_020c1d60.mUnk_800.mUnk_100.mUnk_0c = 0;
    data_020c1d60.mUnk_800.mUnk_100.mUnk_16 = a;
    data_020c1d60.mUnk_800.mUnk_100.mUnk_04 = TOUCH_NONE;
    ResetTouchTrail();
}

int ReadTouchAndAppendTrail(u8 *x, u8 *y) {
    if (gSystemState.mTouchDown == 0) {
        if (x != 0) {
            *x = 0xff;
        }
        if (y != 0) {
            *y = 0xff;
        }
        if (data_020c1d60.mUnk_800.mUnk_100.mUnk_18 == 1) {
            data_020c1d60.mUnk_800.mUnk_100.mUnk_00[2] = gSystemState.mTouchX;
            data_020c1d60.mUnk_800.mUnk_100.mUnk_00[3] = gSystemState.mTouchY;
            data_020c1d60.mUnk_800.mUnk_100.mUnk_18 = 2;
        } else if (data_020c1d60.mUnk_800.mUnk_100.mUnk_18 == 2) {
            data_020c1d60.mUnk_800.mUnk_100.mUnk_18 = 3;
        }
        return 0;
    }
    if (data_020c1d60.mUnk_800.mUnk_100.mUnk_18 != 1) {
        ResetTouchTrail();
        data_020c1d60.mUnk_800.mUnk_100.mUnk_00[0] = gSystemState.mTouchX;
        data_020c1d60.mUnk_800.mUnk_100.mUnk_00[1] = gSystemState.mTouchY;
        data_020c1d60.mUnk_800.mUnk_100.mUnk_18 = 1;
    }
    {
        u8 px = gSystemState.mTouchX;
        u32 n = data_020c1d60.mUnk_800.mUnk_100.mUnk_10;
        data_020c1d60.mUnk_800.mUnk_000[n].mUnk_0 = px;
        data_020c1d60.mUnk_800.mUnk_000[n].mUnk_1 = gSystemState.mTouchY;
        data_020c1d60.mUnk_800.mUnk_100.mUnk_10 = (data_020c1d60.mUnk_800.mUnk_100.mUnk_10 + 1) & 0x7f;
        if (x != 0) {
            *x = px;
        }
    }
    if (y != 0) {
        *y = gSystemState.mTouchY;
    }
    return 1;
}

// Marks the backup memory as ready after taking the card lock id and checking the backup type.
void InitBackup(u32 a) {
    int ok;
    if (!(gSystemState.mFlags & SYSTEM_BACKUP_READY)) {
        gSystemState.mBackupTimer = 0xb4;
        func_02045dc4();
        gSystemState.mBackupLockId = OS_GetLockID();
        func_02044f54(gSystemState.mBackupLockId);
        ok = func_020455fc(a);
        func_02044f64(gSystemState.mBackupLockId);
        if (ok != 0) {
            gSystemState.mFlags |= SYSTEM_BACKUP_READY;
        }
    }
}

// Writes size bytes from buf to the backup memory at address; returns 0 when the backup memory is not ready.
int WriteBackup(const void *buf, u32 address, u32 size) {
    int result;
    if (!(gSystemState.mFlags & SYSTEM_BACKUP_READY)) {
        return 0;
    }
    gSystemState.mBackupTimer = 0xf0;
    func_02044f54(gSystemState.mBackupLockId);
    result = func_02045500((u32) buf, (void *) address, size, 0, 0, 0, 8, 10, 2);
    func_02044f64(gSystemState.mBackupLockId);
    return result;
}

int ReadBackup(void *buf, u32 address, u32 size) {
    int result;
    if ((gSystemState.mFlags & SYSTEM_BACKUP_READY) == 0) {
        return 0;
    }
    func_02044f54(gSystemState.mBackupLockId);
    result = func_02045500(address, buf, size, 0, 0, 0, 6, 1, 0);
    func_02044f64(gSystemState.mBackupLockId);
    return result;
}

void InitMic(int size, int b) {
    if (gSystemState.mFlags & MIC_FLAG_ACTIVE) {
        ReleaseMic();
    }
    gSystemState.mMicBufferSize = size;
    gSystemState.mMicBuffer = Alloc(size + 0x1f);
    gSystemState.mMicSamples = Alloc(size);
    MIC_Init();
    func_02043d98(1);
    func_02043de0(b);
    gSystemState.mFlags |= MIC_FLAG_ACTIVE;
    ClearMicBands();
}

void func_02021054(int a) {
    if ((gSystemState.mFlags & MIC_FLAG_ACTIVE) == 0) {
        OS_Terminate();
    }
    func_02043de0(a);
}

void StartMicSampling(int rate, int type) {
    gSystemState.mMicParam.buffer = (void *) AlignUp32((u32) gSystemState.mMicBuffer);
    gSystemState.mMicParam.size = gSystemState.mMicBufferSize;
    gSystemState.mMicParam.type = type;
    gSystemState.mMicParam.rate = rate;
    gSystemState.mMicParam.loop = 1;
    gSystemState.mMicParam.full = 0;
    gSystemState.mUnk_492 = 0xb4;
    gSystemState.mUnk_495 = 1;
    if (MIC_StartAutoSampling(&gSystemState.mMicParam) != 0) {
        OS_Terminate();
    }
    ClearMicBands();
}

void StopMicSampling(void) {
    MIC_StopAutoSampling();
}

u32 GetMicSampleAddr(void) {
    u32 pos = func_02043454();
    if (pos >= AlignUp32((u32) gSystemState.mMicBuffer) &&
        pos < AlignUp32((u32) gSystemState.mMicBuffer) + gSystemState.mMicBufferSize && gSystemState.mUnk_492 == 0) {
        DC_InvalidateRange((void *) (pos & ~0x1f), 0x20);
        return pos;
    }
    return 0;
}

void CopyMicSamples(u32 count) {
    u32 i;
    u32 pos;
    u32 start;
    if ((gSystemState.mFlags & MIC_FLAG_ACTIVE) == 0) {
        OS_Terminate();
    }
    pos = GetMicSampleAddr();
    if (pos != 0) {
        pos -= count;
        start = AlignUp32((u32) gSystemState.mMicBuffer);
        if (start == 0) {
            OS_Terminate();
        }
        if (pos < start) {
            pos += gSystemState.mMicBufferSize;
        }
        for (i = 0; i < count; i++) {
            if ((pos & 0x1f) == 0) {
                DC_InvalidateRange((void *) pos, 0x20);
                if (pos >= start + gSystemState.mMicBufferSize) {
                    pos = start;
                }
            }
            gSystemState.mMicSamples[i] = *(u8 *) pos;
            pos++;
        }
    }
}

void ClearMicBands(void) {
    int j;
    int i;
    for (i = 0; i < MIC_HISTORY; i++) {
        for (j = 0; j < MIC_MAX_BANDS; j++) {
            gSystemState.mMicBandHistory[i][j] = 0;
        }
    }
    MI_CpuClearFast(gSystemState.mMicBandHistoryTotals, 0x20);
    MI_CpuClearFast(gSystemState.mMicBandSums, 0x20);
}

// Sums the spectrum over each band's range, stores the sums in the current history slot and totals each band over
// the history.
void SumMicBands(const u8 *spectrum, const u16 *ranges, u32 count, u32 *totals, u32 *sums) {
    int i;
    u32 j;
    u32 lo;
    u32 hi;
    if (count >= MIC_MAX_BANDS) {
        OS_Terminate();
    }
    if (gSystemState.mMicParam.buffer == 0) {
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
    j = gSystemState.mElapsedFrames & 0xf;
    for (i = 0; i < count; i++) {
        gSystemState.mMicBandHistory[j][i] = sums[i];
    }
    for (j = 0; j < count; j++) {
        totals[j] = 0;
        for (i = 0; i < MIC_HISTORY; i++) {
            totals[j] += gSystemState.mMicBandHistory[i][j];
        }
    }
}

typedef struct Unk_Complex {
    fx32 re;
    fx32 im;
} Unk_Complex;

void ComputeMicSpectrum(u8 *out, u32 nShift) {
    u32 count = 1 << nShift;
    fx16 sinTable[(3 << MAX_SHIFT) / 4];
    Unk_Complex buf[1 << MAX_SHIFT];
    u32 i;
    if (nShift > MAX_SHIFT) {
        OS_Terminate();
    }
    gSystemState.mMicSpectrumShift = nShift;
    CopyMicSamples(count);
    for (i = 0; i < count; i++) {
        buf[i].re = gSystemState.mMicSamples[i] << SAMPLE_SHIFT;
        buf[i].im = 0;
    }
    func_0204882c(sinTable, nShift);
    func_02048af8((fx32 *) buf, nShift, sinTable);
    for (i = 1; i < count; i++) {
        out[i - 1] = (buf[i].re * buf[i].re + buf[i].im * buf[i].im) >> POWER_SHIFT;
    }
}

u32 func_02021364(u32 x) {
    return (x << gSystemState.mMicSpectrumShift) / gSystemState.mMicParam.rate;
}

u32 GetMicBandAverage(u32 index) {
    return gSystemState.mMicBandHistoryTotals[index] >> 4;
}

void UpdateMicBands(void) {
    u8 spectrum[0x400];
    if ((gSystemState.mFlags & MIC_FLAG_ACTIVE) == 0) {
        InitMic(0x400, 1);
        StartMicSampling(0x82e, 0);
    }
    ComputeMicSpectrum(spectrum, 8);
    SumMicBands(spectrum, gMicBandRanges, 4, gSystemState.mMicBandHistoryTotals, gSystemState.mMicBandSums);
}

int func_020213e8(u32 a) {
    int v0;
    int v1;
    int v2;
    int v3;
    u32 limit;
    UpdateMicBands();
    v0 = GetMicBandAverage(0);
    v1 = GetMicBandAverage(1);
    v2 = GetMicBandAverage(2);
    v3 = GetMicBandAverage(3);
    if (v0 > a && 40 * v3 > v0 && v3 < v0 && v1 < v0 && abs(v1 - v2) < v1 / 2 && v0 > 2 * v3) {
        limit = a * 6;
        if (v0 >= limit) {
            v0 = limit;
        }
        return MapRangeFromZero(v0, limit, 1, 7);
    }
    return 0;
}

void ReleaseMic(void) {
    if (gSystemState.mFlags & MIC_FLAG_ACTIVE) {
        StopMicSampling();
        func_02043d98(0);
        Free(&gSystemState.mMicBuffer);
        Free(&gSystemState.mMicSamples);
        gSystemState.mFlags &= ~MIC_FLAG_ACTIVE;
    }
}
