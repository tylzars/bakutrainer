#pragma once

#include "types.h"
#include <nitro/fs.h>

#ifdef __cplusplus
extern "C" {
#endif

#define WAVE_CHANNEL_COUNT 16
#define WAVE_FORMAT_MAGIC 0x4453504d

// Flags of a wave id: the data is the caller's buffer, not a cached file (UpdateWaves skips its file check), and
// a busy channel of the mask may be taken over when none is free (ClaimWaveChannel's steal). No caller sets
// WAVE_ID_STEAL in this build.
#define WAVE_ID_NO_FILE 0x40000000
#define WAVE_ID_STEAL 0x80000000

// Header of a wave file; the samples follow it.
typedef struct WaveHeader {
    u32 mMagic;
    u16 mRate;
    u8 mUnk_06[2];
    u32 mSize;
    u16 mFormat;
    u8 mUnk_0e[2];
} WaveHeader; /* 10 */

// Wave description passed by the caller instead of a file header.
typedef struct WaveInfo {
    u16 mRate;
    u16 mUnk_02;
    u32 mSize;
    u16 mFormat;
    u16 mUnk_0a;
} WaveInfo; /* c */

// Loop range in bytes.
typedef struct WaveLoop {
    u32 mStart;
    u32 mEnd;
} WaveLoop;

typedef struct WaveChannel {
    /* 00 */ u32 mCommandTag;
    /* 04 */ u32 mId;
    /* 08 */ const void *mWave;
    // Keep-alive mark: 0 none; PlayWaveKeepAlive sets 2, UpdateKeepAliveWaves turns 2 into 1 each frame and stops
    // the channel at 1.
    /* 0c */ u8 mKeepAlive;
    /* 0d */ u8 mUsed : 1;
    u8 mUnk_0d_1 : 1;
    u8 mRepeat : 1;
    u8 mUnk_0d_3 : 1;
    /* 0e */ u8 mUnk_0e[2];
    /* 10 */ u32 mTimer;
    // Volume last sent; UpdateWaves' fade branch, which never runs in this build (mUnk_0d_3 is only ever 0), would
    // count it down.
    /* 14 */ u32 mVolume;
} WaveChannel; /* 18 */

// A wave stream read from a file in four chunks (a ring of mChunk * 4 bytes) and played through a wave channel.
typedef struct WaveStream {
    /* 00 */ FSFile mFile;
    /* 48 */ u32 mChunk;
    /* 4c */ u32 mRate;
    /* 50 */ u8 *mBuffer;
    /* 54 */ s32 mCurrent;
    /* 58 */ u32 mPause;
    /* 5c */ u32 mPlayFrames;
    /* 60 */ char mName[0x40];
} WaveStream;

typedef struct WavePlayer {
    /* 000 */ WaveChannel mChannels[WAVE_CHANNEL_COUNT];
    /* 180 */ u8 mChannelCount;
    // Channel count PlayTrack asks for (the tracker's first hardware channel); RefreshWaveChannelCount makes
    // mChannelCount equal to it.
    /* 181 */ u8 mRequestedChannelCount;
    /* 182 */ u8 mUnk_182[2];
    /* 184 */ s32 mStreamOpen;
    /* 188 */ s32 mStreamPaused;
    /* 18c */ WaveStream *mStream;
} WavePlayer; /* 190 */

extern WavePlayer gWavePlayer;

// Claims a channel from the set of channel bits `mask`: the first channel in the mask that is not in use, or, when
// `steal` is set and none is free, the first channel in the mask (stopping it first). Returns the channel number, or
// the number of channels when none can be claimed.
int ClaimWaveChannel(int steal, u32 mask);
void PlayWaveBuffer(u32 id, int x, int volume, int repeat, int pan, u32 mask, WaveLoop *loop, const void *data,
                    int rate, u32 size, int mode);
void PlayWave(int id, int volume, int repeat, int pan, u32 mask, WaveLoop *loop);
void PlayWaveEx(u32 id, int x, int volume, int repeat, int pan, u32 mask, WaveLoop *loop);
void PlayWaveData(const WaveInfo *info, const void *data, u32 id, int x, u32 volume, int repeat, int pan, u32 mask,
                  WaveLoop *loop);
void StopWaveChannels(u32 mask);
int StopWave(u32 id);
int IsWavePlaying(u32 id);
int FindWaveChannel(u32 id, int *index);
void SetWaveVolume(u32 id, u32 volume);
void StopAllWaves(void);
void UpdateWaves(void);
void DrainCommandReplies(void);
void ClearWaveChannel(u32 index);
void InitWavePlayer(void);
void StopWavePlayer(void);
void RefreshWaveChannelCount(void);
void PlayWaveKeepAlive(int id, int volume, int repeat, int pan, int mask, int loop);
void UpdateKeepAliveWaves(void);
void ScaleWaveTimer(int id, int scale);
void OpenWaveStream(WaveStream *stream, const char *name, u32 rate);
void CloseWaveStream(WaveStream *stream);
void UpdateWaveStream(WaveStream *stream);
void PauseWaveStream(WaveStream *stream);
void ResumeWaveStream(WaveStream *stream);

#ifdef __cplusplus
}
#endif
