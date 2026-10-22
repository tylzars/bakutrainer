#pragma once

#include "types.h"
#include <nitro/fs.h>
#include <nitro/os.h>

#ifdef __cplusplus
extern "C" {
#endif

// The stream audio object: the alarm that paces the audio, the decoded audio ring, the player and the file it
// reads (the movie being played).
typedef struct Unk_MovieAudio {
    /* 00 */ OSAlarm mAlarm;
    /* 2c */ vu32 mUnk_2c;
    /* 30 */ u32 mUnk_30;
    /* 34 */ s16 *mUnk_34;
    /* 38 */ u32 mUnk_38;
    /* 3c */ u32 mUnk_3c;
    /* 40 */ vu32 mUnk_40;
    /* 44 */ u32 mUnk_44;
    u8 mUnk_48[4];
    /* 4c */ struct Player *mPlayer;
    /* 50 */ FSFile mFile;
    /* 98 */ u32 mUnk_98;
    /* 9c */ u32 mUnk_9c;
    /* a0 */ s32 mUnk_a0;
    /* a4 */ int mUnk_a4;
    /* a8 */ u32 mUnk_a8;
    /* ac */ u32 mUnk_ac;
    /* b0 */ u32 mUnk_b0;
    /* b4 */ u8 mAudioStarted;
    // 1 once func_0202fbc8 has turned the screens off for a closed lid, 0 again once func_0202fc5c has turned them on
    // (StartVideo also clears it). While it and mAudioStarted are set, UpdateVideo reads no new frame and holds the
    // player.
    /* b5 */ u8 mScreensOff;
    /* b6 */ u8 mUnk_b6;
    u8 mUnk_b7;
} Unk_MovieAudio; /* b8 */
extern Unk_MovieAudio data_020c661c;

void *func_0202f500(u32 size);
void func_0202f50c(void *p);
void func_0202f520(void *arg);
void StartVideoAudio(u32 a, u32 b);
void func_0202f614(void *arg);
void *func_0202f628(u32 screen);
void StartVideo(const char *path, int b);
int UpdateVideo(void);
void StopVideoAudio(void);
void StopVideo(void);
void func_0202fbc8(void);
void func_0202fc5c(void);
u8 AreScreensOff(void);

#ifdef __cplusplus
}
#endif
