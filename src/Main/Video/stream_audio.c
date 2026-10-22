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

Unk_MovieAudio data_020c661c;

#pragma thumb off

void *func_0202f500(u32 size) {
    return Alloc(size);
}

void func_0202f50c(void *p) {
    void *q = p;
    Free(&q);
}

void func_0202f520(void *arg) {
    data_020c661c.mUnk_2c++;
}

void func_0202f538(u32 a, u32 b) {
    u32 step = 0xffb0ff / a;
    u32 total = (b >> 6) * (step * 2);
    DC_StoreRange(data_020c661c.mUnk_34, (data_020c661c.mUnk_3c * b) << 1);
    func_02040198(1, 0);
    func_02040240(0, 1, (u32) data_020c661c.mUnk_34, 1, 0, (data_020c661c.mUnk_3c * b) >> 1, 0x7f, 0, step, 0x40);
    func_02040158(0, total, total, func_0202f520, 0);
    func_020400cc(1, 0, 1, 0);
    SND_FlushCommand(1);
}

void func_0202f614(void *arg) {
    data_020c661c.mUnk_40 = 1;
}

void *func_0202f628(u32 screen) {
    return (screen & 1) ? (void *) 0x6000000 : (void *) 0x6200000;
}

// Starts playing the movie file `path` on screen `b`: reads the player's header, allocates the decoded audio ring and
// starts the alarm that paces the audio.
void func_0202f638(const char *path, int b) {
    u32 a;
    u32 c;
    u32 lo;
    u32 x;
    u32 y;
    u32 z;
    if (gSystemState.mFlags & 0x20) {
        func_0202fb4c();
    }
    data_020c661c.mUnk_b6 = b;
    data_020c661c.mUnk_ac = 0;
    data_020c661c.mUnk_b4 = 0;
    data_020c661c.mUnk_b5 = 0;
    gSystemState.mFlags |= 0x20;
    if (func_0201db58(b) != 0x8000 || GetLayerRowSize(b) != 0x200) {
        SetupImageLayer(b, 1, 0x100, 0xc0);
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
    func_020421f8(&data_020c661c.mFile);
    if (func_02042490(&data_020c661c.mFile, path) == 0) {
        OS_Terminate();
    }
    data_020c661c.mUnk_98 = 6;
    data_020c661c.mPlayer = func_02039e08(&data_020c661c.mFile, 6);
    if (data_020c661c.mPlayer == 0) {
        OS_Terminate();
    }
    data_020c661c.mUnk_a4 = func_02039f40(data_020c661c.mPlayer);
    data_020c661c.mUnk_9c = func_02039f58(data_020c661c.mPlayer);
    if (data_020c661c.mUnk_9c != 0) {
        x = func_02039f10(data_020c661c.mPlayer);
        y = func_02039f28(data_020c661c.mPlayer);
        z = data_020c661c.mUnk_9c;
        data_020c661c.mUnk_a0 = (u32) (((u64) x << 24) / ((u64) z * y)) + 1;
        data_020c661c.mUnk_3c = data_020c661c.mUnk_a0 * (data_020c661c.mUnk_98 + 1);
        data_020c661c.mUnk_34 = Alloc(data_020c661c.mUnk_3c * data_020c661c.mUnk_9c * 2);
        data_020c661c.mUnk_2c = 0;
        data_020c661c.mUnk_38 = 0;
        data_020c661c.mUnk_30 = 0;
        OS_CreateAlarm(&data_020c661c.mAlarm);
        {
            OSTick tick = OS_GetTick();
            u32 d = func_02039f28(data_020c661c.mPlayer);
            u64 step = 0xf4240000000ULL / d;
            u32 period = (step * 0x82ea >> 6) / 1000;
            OS_SetPeriodicAlarm(&data_020c661c.mAlarm, tick + 5, period, func_0202f614, 0);
        }
    }
    data_020c661c.mUnk_44 = 0;
    data_020c661c.mUnk_a8 = 0;
}

// Decodes the next movie frame and its audio into the ring, starts the audio once enough frames are queued, and
// pauses or resumes the player to keep the audio and video in step. Returns 0 when the movie is over.
int func_0202f8d0(void) {
    u32 i;
    if (data_020c661c.mUnk_b5 == 0 || data_020c661c.mUnk_b4 == 0) {
        if (func_02039f70(data_020c661c.mPlayer) == 0) {
            return 0;
        }
        func_02039f94(data_020c661c.mPlayer);
        data_020c661c.mUnk_ac++;
    }
    data_020c661c.mUnk_b0 = func_0203a000(data_020c661c.mPlayer);
    for (i = 0; i < data_020c661c.mUnk_b0; i++) {
        if (data_020c661c.mUnk_b4 != 0 && data_020c661c.mUnk_2c < data_020c661c.mUnk_30) {
            while (data_020c661c.mUnk_3c - (data_020c661c.mUnk_30 - data_020c661c.mUnk_2c) == 0) {
            }
        }
        func_0203a018(data_020c661c.mPlayer, data_020c661c.mUnk_34 + data_020c661c.mUnk_38);
        DC_StoreRange(data_020c661c.mUnk_34 + data_020c661c.mUnk_38, data_020c661c.mUnk_9c * 2);
        data_020c661c.mUnk_38 += data_020c661c.mUnk_9c;
        if (data_020c661c.mUnk_38 == data_020c661c.mUnk_3c * data_020c661c.mUnk_9c) {
            data_020c661c.mUnk_38 = 0;
        }
        data_020c661c.mUnk_30++;
        if (data_020c661c.mUnk_b4 == 0 && data_020c661c.mUnk_ac >= data_020c661c.mUnk_98) {
            func_0202f538(func_02039f10(data_020c661c.mPlayer), data_020c661c.mUnk_9c);
            data_020c661c.mUnk_b4 = 1;
        }
    }
    if (data_020c661c.mUnk_b4 == 1 || func_02039f10(data_020c661c.mPlayer) == 0) {
        s32 queued = data_020c661c.mUnk_30 - data_020c661c.mUnk_2c;
        if (data_020c661c.mUnk_b5 != 0 || (queued <= data_020c661c.mUnk_a0 * 3 && data_020c661c.mUnk_a8 == 0 &&
                                           func_02039f10(data_020c661c.mPlayer) != 0)) {
            data_020c661c.mUnk_ac--;
            func_02039fdc(data_020c661c.mPlayer);
            data_020c661c.mUnk_a8 = 1;
        } else {
            data_020c661c.mUnk_ac--;
            func_02039fb8(data_020c661c.mPlayer, func_0202f628(data_020c661c.mUnk_b6), 0x100, 0);
            data_020c661c.mUnk_a8 = 0;
        }
    }
    return 1;
}

// Stops the audio once it has started.
void func_0202fb08(void) {
    if (data_020c661c.mUnk_b4 != 1) {
        return;
    }
    func_020400f4(1, 0, 1, 0);
    func_020401b8(1, 0);
    SND_FlushCommand(1);
}

// Ends the movie: stops audio and the alarm, frees the ring and the player, closes the file, restores the screen.
void func_0202fb4c(void) {
    if (!(gSystemState.mFlags & 0x20)) {
        return;
    }
    func_0202fb08();
    OS_CancelAlarm(&data_020c661c.mAlarm);
    Free(&data_020c661c.mUnk_34);
    func_02039ef0(data_020c661c.mPlayer);
    func_020424d8(&data_020c661c.mFile);
    FreeVramBanks(data_020c661c.mUnk_b6 == 1 ? 2 : 6);
    func_0202fc5c();
    gSystemState.mFlags &= ~0x20;
}

// Turns the screens off and on again with that bit (bit 15 of the shared key state).
void func_0202fbc8(void) {
    if (((s32) (*(vu16 *) 0x27fffa8 & 0x8000)) >> 15 == 1) {
        if (data_020c661c.mUnk_b5 != 0) {
            return;
        }
        func_0203f6a4();
        func_0203c994();
        *(vu32 *) 0x4001000 &= ~0x10000;
        func_0203f6a4();
        if (func_020443ac(0) != 1) {
            return;
        }
        data_020c661c.mUnk_b5 = 1;
    } else {
        if (data_020c661c.mUnk_b5 != 1) {
            return;
        }
        func_0202fc5c();
        func_0204028c(0x7f);
    }
}

void func_0202fc5c(void) {
    if (data_020c661c.mUnk_b5 != 1) {
        return;
    }
    if (func_020443ac(1) != 1) {
        return;
    }
    func_0203f6a4();
    GX_DispOn();
    *(vu32 *) 0x4001000 |= 0x10000;
    func_0203f6a4();
    data_020c661c.mUnk_b5 = 0;
}

// Whether the screens are currently off.
u8 func_0202fcb4(void) {
    return data_020c661c.mUnk_b5;
}

#pragma thumb on
