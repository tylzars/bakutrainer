#pragma once

#include "types.h"
#include <nitro/fs.h>
#include <nitro/os.h>

#ifdef __cplusplus
extern "C" {
#endif

// The movie playback state: the alarm that paces the audio, the decoded audio ring, the player and the file it reads
// (the movie being played), the screen flags and the layer the frames go to.
typedef struct VideoState {
    /* 00 */ OSAlarm mAlarm;
    /* 2c */ vu32 mAudioCallbackCount;
    /* 30 */ u32 mAudioWriteCount;
    /* 34 */ s16 *mAudioBuffer;
    // In halfwords from mAudioBuffer; wraps to 0 at mAudioEntryCount * mAudioEntryLength.
    /* 38 */ u32 mAudioWritePos;
    /* 3c */ u32 mAudioEntryCount;
    // Set to 1 by VideoAlarmCallback; nothing reads it.
    /* 40 */ vu32 mUnk_40;
    /* 44 */ u32 mUnk_44;
    u8 mUnk_48[4];
    /* 4c */ struct Player *mPlayer;
    /* 50 */ FSFile mFile;
    /* 98 */ u32 mUnk_98;
    // The length of one audio entry, in halfwords.
    /* 9c */ u32 mAudioEntryLength;
    /* a0 */ s32 mUnk_a0;
    /* a4 */ int mUnk_a4;
    /* a8 */ u32 mUnk_a8;
    /* ac */ u32 mUnk_ac;
    /* b0 */ u32 mUnk_b0;
    /* b4 */ u8 mAudioStarted;
    // 1 once UpdateScreensForLid has turned the screens off for a closed lid, 0 again once TurnScreensOn has turned
    // them on (StartVideo also clears it). While it and mAudioStarted are set, UpdateVideo reads no new frame and holds
    // the player.
    /* b5 */ u8 mScreensOff;
    /* b6 */ u8 mLayer;
    u8 mUnk_b7;
} VideoState; /* b8 */
extern VideoState gVideoState;

void *MobiclipAlloc(u32 size);
void MobiclipFree(void *p);
void VideoAudioCallback(void *arg);
void StartVideoAudio(u32 a, u32 b);
void VideoAlarmCallback(void *arg);
void *GetImageLayerAddr(u32 layer);
void StartVideo(const char *path, int layer);
int UpdateVideo(void);
void StopVideoAudio(void);
void StopVideo(void);
void UpdateScreensForLid(void);
void TurnScreensOn(void);
u8 AreScreensOff(void);

#ifdef __cplusplus
}
#endif
