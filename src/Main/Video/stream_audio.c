#include <nitro/os.h>
#include <nitro/snd.h>
#include <Main/Video/stream_audio.h>
#include <Main/System/system_init.h>

#include <nitro/fs.h>
#include <nitro/gx.h>
#include <nitro/pm.h>
#include <Main/System/bg_image.h>
#include <Main/System/screen_fade.h>
#include <mobiclip.h>

VideoState gVideoState;

#pragma thumb off

// The allocation hooks of the movie player library (libs/mobiclip).
void *MobiclipAlloc(u32 size) {
    return Alloc(size);
}

// Frees through a local copy of p, so the caller's pointer is not cleared.
void MobiclipFree(void *p) {
    void *q = p;
    Free(&q);
}

void VideoAudioCallback(void *arg) {
    gVideoState.mAudioCallbackCount++;
}

void StartVideoAudio(u32 a, u32 b) {
    u32 step = 0xffb0ff / a;
    u32 total = (b >> 6) * (step * 2);
    DC_StoreRange(gVideoState.mAudioBuffer, (gVideoState.mAudioEntryCount * b) << 1);
    func_02040198(1, 0);
    func_02040240(0, 1, (u32) gVideoState.mAudioBuffer, 1, 0, (gVideoState.mAudioEntryCount * b) >> 1, 0x7f, 0, step,
                  0x40);
    func_02040158(0, total, total, VideoAudioCallback, 0);
    func_020400cc(1, 0, 1, 0);
    SND_FlushCommand(1);
}

// The periodic alarm StartVideo sets: sets mUnk_40, which nothing reads.
void VideoAlarmCallback(void *arg) {
    gVideoState.mUnk_40 = 1;
}

void *GetImageLayerAddr(u32 layer) {
    return (layer & 1) ? (void *) 0x6000000 : (void *) 0x6200000;
}

// Starts playing the movie file `path` on layer `layer`: reads the player's header, allocates the decoded audio ring
// and starts the alarm that paces the audio.
void StartVideo(const char *path, int layer) {
    u32 a;
    u32 c;
    u32 lo;
    u32 x;
    u32 y;
    u32 z;
    if (gSystemState.mFlags & 0x20) {
        StopVideo();
    }
    gVideoState.mLayer = layer;
    gVideoState.mUnk_ac = 0;
    gVideoState.mAudioStarted = 0;
    gVideoState.mScreensOff = 0;
    gSystemState.mFlags |= 0x20;
    if (func_0201db58(layer) != 0x8000 || GetLayerRowSize(layer) != 0x200) {
        SetupImageLayer(layer, 1, 0x100, 0xc0);
    }
    lo = (u32) OS_GetArenaLo(3);
    a = (u32) OS_GetArenaHi(3) - lo;
    lo = (u32) OS_GetArenaLo(4);
    c = (u32) OS_GetArenaHi(4) - lo;
    if (a >= func_0203b528()) {
        a = func_0203b528();
    }
    func_0203b53c(OS_GetArenaLo(3), a);
    if (c >= func_0203b534()) {
        c = func_0203b534();
    }
    func_0203b55c(OS_GetArenaLo(4), c);
    func_020421f8(&gVideoState.mFile);
    if (func_02042490(&gVideoState.mFile, path) == 0) {
        OS_Terminate();
    }
    gVideoState.mUnk_98 = 6;
    gVideoState.mPlayer = func_02039e08(&gVideoState.mFile, 6);
    if (gVideoState.mPlayer == 0) {
        OS_Terminate();
    }
    gVideoState.mUnk_a4 = func_02039f40(gVideoState.mPlayer);
    gVideoState.mAudioEntryLength = func_02039f58(gVideoState.mPlayer);
    if (gVideoState.mAudioEntryLength != 0) {
        x = func_02039f10(gVideoState.mPlayer);
        y = func_02039f28(gVideoState.mPlayer);
        z = gVideoState.mAudioEntryLength;
        gVideoState.mUnk_a0 = (u32) (((u64) x << 24) / ((u64) z * y)) + 1;
        gVideoState.mAudioEntryCount = gVideoState.mUnk_a0 * (gVideoState.mUnk_98 + 1);
        gVideoState.mAudioBuffer = Alloc(gVideoState.mAudioEntryCount * gVideoState.mAudioEntryLength * 2);
        gVideoState.mAudioCallbackCount = 0;
        gVideoState.mAudioWritePos = 0;
        gVideoState.mAudioWriteCount = 0;
        OS_CreateAlarm(&gVideoState.mAlarm);
        {
            OSTick tick = OS_GetTick();
            u32 d = func_02039f28(gVideoState.mPlayer);
            u64 step = 0xf4240000000ULL / d;
            u32 period = (step * 0x82ea >> 6) / 1000;
            OS_SetPeriodicAlarm(&gVideoState.mAlarm, tick + 5, period, VideoAlarmCallback, 0);
        }
    }
    gVideoState.mUnk_44 = 0;
    gVideoState.mUnk_a8 = 0;
}

// Decodes the next movie frame and its audio into the ring, starts the audio once enough frames are queued, and
// pauses or resumes the player to keep the audio and video in step. Returns 0 when the movie is over.
int UpdateVideo(void) {
    u32 i;
    if (gVideoState.mScreensOff == 0 || gVideoState.mAudioStarted == 0) {
        if (func_02039f70(gVideoState.mPlayer) == 0) {
            return 0;
        }
        func_02039f94(gVideoState.mPlayer);
        gVideoState.mUnk_ac++;
    }
    gVideoState.mUnk_b0 = func_0203a000(gVideoState.mPlayer);
    for (i = 0; i < gVideoState.mUnk_b0; i++) {
        if (gVideoState.mAudioStarted != 0 && gVideoState.mAudioCallbackCount < gVideoState.mAudioWriteCount) {
            while (gVideoState.mAudioEntryCount - (gVideoState.mAudioWriteCount - gVideoState.mAudioCallbackCount) ==
                   0) {
            }
        }
        func_0203a018(gVideoState.mPlayer, gVideoState.mAudioBuffer + gVideoState.mAudioWritePos);
        DC_StoreRange(gVideoState.mAudioBuffer + gVideoState.mAudioWritePos, gVideoState.mAudioEntryLength * 2);
        gVideoState.mAudioWritePos += gVideoState.mAudioEntryLength;
        if (gVideoState.mAudioWritePos == gVideoState.mAudioEntryCount * gVideoState.mAudioEntryLength) {
            gVideoState.mAudioWritePos = 0;
        }
        gVideoState.mAudioWriteCount++;
        if (gVideoState.mAudioStarted == 0 && gVideoState.mUnk_ac >= gVideoState.mUnk_98) {
            StartVideoAudio(func_02039f10(gVideoState.mPlayer), gVideoState.mAudioEntryLength);
            gVideoState.mAudioStarted = 1;
        }
    }
    if (gVideoState.mAudioStarted == 1 || func_02039f10(gVideoState.mPlayer) == 0) {
        s32 queued = gVideoState.mAudioWriteCount - gVideoState.mAudioCallbackCount;
        if (gVideoState.mScreensOff != 0 || (queued <= gVideoState.mUnk_a0 * 3 && gVideoState.mUnk_a8 == 0 &&
                                             func_02039f10(gVideoState.mPlayer) != 0)) {
            gVideoState.mUnk_ac--;
            func_02039fdc(gVideoState.mPlayer);
            gVideoState.mUnk_a8 = 1;
        } else {
            gVideoState.mUnk_ac--;
            func_02039fb8(gVideoState.mPlayer, GetImageLayerAddr(gVideoState.mLayer), 0x100, 0);
            gVideoState.mUnk_a8 = 0;
        }
    }
    return 1;
}

// Does nothing unless the audio has started.
void StopVideoAudio(void) {
    if (gVideoState.mAudioStarted != 1) {
        return;
    }
    func_020400f4(1, 0, 1, 0);
    func_020401b8(1, 0);
    SND_FlushCommand(1);
}

// Ends the movie: stops audio and the alarm, frees the ring and the player, closes the file, restores the screen.
void StopVideo(void) {
    if (!(gSystemState.mFlags & 0x20)) {
        return;
    }
    StopVideoAudio();
    OS_CancelAlarm(&gVideoState.mAlarm);
    Free(&gVideoState.mAudioBuffer);
    func_02039ef0(gVideoState.mPlayer);
    func_020424d8(&gVideoState.mFile);
    FreeVramBanks(gVideoState.mLayer == 1 ? 2 : 6);
    TurnScreensOn();
    gSystemState.mFlags &= ~0x20;
}

// Follows the lid while a movie plays. The test reads bit 15 of the shared key state at 0x027fffa8 (LID_CLOSED of
// HW_SHARED_BUTTONS, written as literals here because those defines are local to system_init.c). With the lid closed
// and mScreensOff 0 it switches the displays off (func_0203c994), clears bit 16 of 0x04001000 and sets mScreensOff
// when func_020443ac(0) then returns 1; with the lid open and mScreensOff 1 it calls TurnScreensOn and then
// func_0204028c(0x7f).
void UpdateScreensForLid(void) {
    if (((s32) (*(vu16 *) 0x27fffa8 & 0x8000)) >> 15 == 1) {
        if (gVideoState.mScreensOff != 0) {
            return;
        }
        func_0203f6a4();
        func_0203c994();
        *(vu32 *) 0x4001000 &= ~0x10000;
        func_0203f6a4();
        if (func_020443ac(0) != 1) {
            return;
        }
        gVideoState.mScreensOff = 1;
    } else {
        if (gVideoState.mScreensOff != 1) {
            return;
        }
        TurnScreensOn();
        func_0204028c(0x7f);
    }
}

// Only when mScreensOff is 1 and func_020443ac(1) returns 1: switches the displays on, sets bit 16 of 0x04001000
// again and clears mScreensOff.
void TurnScreensOn(void) {
    if (gVideoState.mScreensOff != 1) {
        return;
    }
    if (func_020443ac(1) != 1) {
        return;
    }
    func_0203f6a4();
    GX_DispOn();
    *(vu32 *) 0x4001000 |= 0x10000;
    func_0203f6a4();
    gVideoState.mScreensOff = 0;
}

// Whether the screens are currently off.
u8 AreScreensOff(void) {
    return gVideoState.mScreensOff;
}

#pragma thumb on
