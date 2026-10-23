#include <Main/clib.h>
#include <nitro/fs.h>
#include <nitro/os.h>
#include <nitro/snd.h>
#include <Main/Sound/wave_player.h>
#include <Main/System/file_loader.h>
#include <Main/System/system_init.h>

WavePlayer gWavePlayer;

#pragma thumb off

int ClaimWaveChannel(int steal, u32 mask) {
    int second = 0;
    u32 i;
    u32 bit;

    for (;;) {
        for (i = 0; i < gWavePlayer.mChannelCount; i++) {
            bit = 1 << i;

            if ((mask & bit) && (gWavePlayer.mChannels[i].mUsed == 0 || second == 1)) {
                WaveChannel *s = &gWavePlayer.mChannels[i];

                if (s->mUsed == 1) {
                    StopWaveChannels(bit);
                }
                s->mUsed = 1;
                func_02040198(bit, 0);
                return i;
            }
        }
        if (steal != 0) {
            second = 1;
            steal = 0;
            continue;
        }
        return i;
    }
}

#pragma thumb on

void PlayWaveBuffer(u32 id, int start, int volume, int repeat, int pan, u32 mask, WaveLoop *loop, const void *data,
                    int rate, u32 size, int mode) {
    WaveInfo info;
    info.mRate = rate;
    info.mUnk_02 = 1;
    info.mSize = size;
    info.mFormat = (mode == 1) ? 2 : 1;
    info.mUnk_0a = 0;
    PlayWaveData(&info, data, id | WAVE_ID_NO_FILE, start, volume, repeat, pan, mask, loop);
}

void PlayWave(int id, int volume, int repeat, int pan, u32 mask, WaveLoop *loop) {
    PlayWaveEx(id, 0, volume, repeat, pan, mask, loop);
}

void PlayWaveEx(u32 id, int start, int volume, int repeat, int pan, u32 mask, WaveLoop *loop) {
    PlayWaveData(0, FindOrLoadCachedFile((u16) id), id, start, volume, repeat, pan, mask, loop);
}

void PlayWaveData(const WaveInfo *info, const void *data, u32 id, int start, u32 volume, int repeat, int pan, u32 mask,
                  WaveLoop *loop) {
    WaveChannel *ch;
    u32 key;
    u32 size;
    u32 format;
    u32 mode;
    int index;
    u32 timer;
    u32 rate;
    u32 loopStart;
    const u8 *body;
    if ((u16) id == 0xffff) {
        OS_Terminate();
    }
    if (gSystemState.mUseWaves == 0) {
        return;
    }
    key = id & 0x4000ffff;
    index = ClaimWaveChannel((id & WAVE_ID_STEAL) ? 1 : 0, mask);
    if (index == gWavePlayer.mChannelCount) {
        return;
    }
    gWavePlayer.mChannels[index].mId = key;
    if (info == 0) {
        const WaveHeader *hdr = data;
        if (hdr->mMagic != WAVE_FORMAT_MAGIC) {
            OS_Terminate();
        }
        rate = hdr->mRate;
        size = hdr->mSize;
        format = hdr->mFormat & 3;
        body = (const u8 *) data + sizeof(WaveHeader);
    } else {
        rate = info->mRate;
        size = info->mSize;
        format = info->mFormat & 3;
        body = data;
    }
    if (format == 0) {
        size >>= 2;
    }
    gWavePlayer.mChannels[index].mWave = data;
    ch = &gWavePlayer.mChannels[index];
    ch->mRepeat = repeat;
    timer = 0xffb0ff / rate;
    loopStart = 0;
    ch->mUnk_0d_1 = 0;
    gWavePlayer.mChannels[index].mTimer = timer;
    gWavePlayer.mChannels[index].mVolume = volume;
    ch->mFading = 0;
    if (start != 0) {
        start = rate * start / 60;
    }
    switch (format) {
        case 0:
            ch->mUnk_0d_1 = 1;
            mode = 2;
            if (repeat == 1) {
                loopStart = 4;
            }
            break;
        case 1:
            mode = 1;
            start <<= 1;
            break;
        case 2:
            mode = 0;
            break;
    }
    if (size > start) {
        start &= ~3;
        size -= start;
        if (repeat == 1 && loop != 0) {
            loopStart = loop->mStart >> 2;
            size = loop->mEnd - loop->mStart;
        }
        func_02040240(index, mode, (u32) body + (start & ~1), (repeat == 1) ? 1 : 2, loopStart, size >> 2,
                      volume * 0x145 >> 8, 0, timer, pan);
        func_020400cc(1 << index, 0, 0, 0);
        ch->mCommandTag = SND_GetCurrentCommandTag();
    }
}

void StopWaveChannels(u32 mask) {
    u32 i;
    if (mask != 0) {
        func_020400f4(mask, 0, 0, 0);
        func_020401b8(mask, 0);
        for (i = 0; i < gWavePlayer.mChannelCount; i++) {
            if (mask & (1 << i)) {
                ClearWaveChannel(i);
            }
        }
    }
}

int StopWave(u32 id) {
    u32 mask = 0;
    u32 i;
    for (i = 0; i < gWavePlayer.mChannelCount; i++) {
        if (gWavePlayer.mChannels[i].mUsed == 1 && id == gWavePlayer.mChannels[i].mId) {
            mask |= 1 << i;
        }
    }
    StopWaveChannels(mask);
    if (mask == 0) {
        return 0;
    }
    return 1;
}

int IsWavePlaying(u32 id) {
    return FindWaveChannel(id, 0);
}

int FindWaveChannel(u32 id, int *index) {
    u32 i;
    for (i = 0; i < gWavePlayer.mChannelCount; i++) {
        if (gWavePlayer.mChannels[i].mUsed == 1 && id == gWavePlayer.mChannels[i].mId) {
            if (index != 0) {
                *index = i;
            }
            return 1;
        }
    }
    return 0;
}

// Sets the volume of the channels playing id. Its only caller is UpdateWaves' fade branch.
void SetWaveVolume(u32 id, u32 volume) {
    u32 i;
    u32 mask = 0;
    for (i = 0; i < gWavePlayer.mChannelCount; i++) {
        if (gWavePlayer.mChannels[i].mUsed == 1 && id == gWavePlayer.mChannels[i].mId) {
            mask |= 1 << i;
        }
    }
    if (mask != 0) {
        u32 out = volume;
        for (i = 0; i < gWavePlayer.mChannelCount; i++) {
            // Tests the channel number itself against the mask, not its bit.
            if ((mask & i) && gWavePlayer.mChannels[i].mUnk_0d_1 == 1) {
                out = volume * 0x30 >> 8;
                gWavePlayer.mChannels[i].mVolume = out;
            }
        }
        func_020401f8(mask, out * 0x145 >> 8, 0);
    }
}

void StopAllWaves(void) {
    u32 mask = 0;
    u32 i;
    for (i = 0; i < gWavePlayer.mChannelCount; i++) {
        if (gWavePlayer.mChannels[i].mUsed == 1) {
            mask |= 1 << i;
        }
    }
    StopWaveChannels(mask);
}

void UpdateWaves(void) {
    u32 mask = 0;
    u32 busy = func_02040a90();
    u32 i;
    WaveChannel *ch;
    for (i = 0; i < gWavePlayer.mChannelCount; i++) {
        ch = &gWavePlayer.mChannels[i];
        // Never runs: mFading is only ever 0.
        if (ch->mFading && ch->mVolume != 0) {
            u32 v = ch->mVolume;
            ch->mVolume = v - 1;
            SetWaveVolume(ch->mId, v);
        }
        if (ch->mUsed == 1) {
            if ((ch->mRepeat == 0 || gSystemState.mUseWaves == 0) && (busy & (1 << i)) == 0 &&
                SND_IsFinishedCommandTag(ch->mCommandTag) == 1) {
                mask |= 1 << i;
            } else if ((ch->mId & WAVE_ID_NO_FILE) == 0) {
                if (FindOrLoadCachedFile(ch->mId) != ch->mWave) {
                    mask |= 1 << i;
                }
            }
        }
    }
    if (mask != 0) {
        StopWaveChannels(mask);
    }
    while (SND_RecvCommandReply(0) != 0) {
    }
    SND_FlushCommand(2);
}

void DrainCommandReplies(void) {
    while (SND_RecvCommandReply(0) != 0) {
    }
}

// Frees a channel: clears mUsed, mKeepAlive and mWave; the other fields keep their values.
void ClearWaveChannel(u32 index) {
    WaveChannel *ch = &gWavePlayer.mChannels[index];
    ch->mUsed = 0;
    gWavePlayer.mChannels[index].mKeepAlive = 0;
    gWavePlayer.mChannels[index].mWave = 0;
}

// Sets the channel count (16, or mRequestedChannelCount while a song is loaded), sets flag 0x2000 and frees every
// channel. In this build it runs once, at boot, before any song and before flag 0x2000 is set, so the
// mRequestedChannelCount branch never runs.
void InitWavePlayer(void) {
    u32 i;
    StopWavePlayer();
    if ((gSystemState.mFlags & 0x1000000) == 0) {
        gWavePlayer.mChannelCount = 0x10;
    } else {
        gWavePlayer.mChannelCount = gWavePlayer.mRequestedChannelCount;
    }
    gSystemState.mFlags |= 0x2000;
    func_020402f0();
    for (i = 0; i < gWavePlayer.mChannelCount; i++) {
        ClearWaveChannel(i);
    }
}

// Stops every wave and clears flag 0x2000. Never does anything in this build: InitWavePlayer, its only caller, runs
// before the flag is set.
void StopWavePlayer(void) {
    if (gSystemState.mFlags & 0x2000) {
        StopAllWaves();
        gSystemState.mFlags &= ~0x2000;
    }
}

// Makes mChannelCount equal to mRequestedChannelCount, freeing the added channels or stopping the removed ones. Its
// only caller PlayTrack sets flag 0x1000000 first, and flag 0x2000 stays set from boot, so in this build the first
// two branches never run.
void RefreshWaveChannelCount(void) {
    u32 i;
    u32 cur;
    u32 want;
    u32 mask;
    if ((gSystemState.mFlags & 0x2000) == 0) {
        InitWavePlayer();
        return;
    }
    if ((gSystemState.mFlags & 0x1000000) == 0) {
        gWavePlayer.mRequestedChannelCount = 0x10;
        for (i = gWavePlayer.mChannelCount; i < gWavePlayer.mRequestedChannelCount; i++) {
            ClearWaveChannel(i);
        }
        gWavePlayer.mChannelCount = gWavePlayer.mRequestedChannelCount;
        return;
    }
    want = gWavePlayer.mRequestedChannelCount;
    cur = gWavePlayer.mChannelCount;
    if (cur == want) {
        return;
    }
    if (cur < want) {
        for (; cur < gWavePlayer.mRequestedChannelCount; cur++) {
            ClearWaveChannel(cur);
        }
        gWavePlayer.mChannelCount = gWavePlayer.mRequestedChannelCount;
    } else {
        mask = 0;
        for (i = want; i < cur; i++) {
            mask |= 1 << i;
        }
        StopWaveChannels(mask);
        gWavePlayer.mChannelCount = gWavePlayer.mRequestedChannelCount;
    }
}

#define WAVE_STREAM_ID 0x4d2
#define WAVE_STREAM_KEY (WAVE_ID_NO_FILE | WAVE_STREAM_ID)

// With repeat 0 the same as PlayWave. Otherwise, if id is not playing, plays it and marks its repeating channels. If
// it plays, renews the mark of its first repeating channel and sends the pan and a changed volume. A caller keeps the
// wave by calling this every frame: UpdateKeepAliveWaves stops a channel whose mark was not renewed.
void PlayWaveKeepAlive(int id, int volume, int repeat, int pan, int mask, int loop) {
    u32 i;
    u32 bit;
    u32 index;
    if (repeat == 0) {
        PlayWave(id, volume, repeat, pan, mask, (WaveLoop *) loop);
        return;
    }
    if (IsWavePlaying(id) == 0) {
        PlayWave(id, volume, repeat, pan, mask, (WaveLoop *) loop);
        for (i = 0; i < gWavePlayer.mChannelCount; i++) {
            WaveChannel *ch = &gWavePlayer.mChannels[i];
            if (id == ch->mId && ch->mRepeat == 1) {
                ch->mKeepAlive = 2;
            }
        }
        return;
    }
    for (i = 0; i < gWavePlayer.mChannelCount; i++) {
        WaveChannel *ch = &gWavePlayer.mChannels[i];
        if (id == ch->mId && ch->mRepeat == 1) {
            index = i;
            gWavePlayer.mChannels[index].mKeepAlive = 2;
            bit = 1 << i;
            func_02040220(bit, pan);
            if (volume != gWavePlayer.mChannels[index].mVolume) {
                if (gWavePlayer.mChannels[index].mUnk_0d_1 == 1) {
                    volume = ((u32) volume * 0x30) >> 8;
                }
                func_020401f8(bit, ((u32) volume * 0x145) >> 8, 0);
                gWavePlayer.mChannels[index].mVolume = volume;
            }
            return;
        }
    }
}

void UpdateKeepAliveWaves(void) {
    u32 mask = 0;
    u32 i;
    for (i = 0; i < gWavePlayer.mChannelCount; i++) {
        WaveChannel *ch = &gWavePlayer.mChannels[i];
        if (ch->mKeepAlive == 1) {
            mask |= 1 << i;
        } else if (ch->mKeepAlive == 2) {
            ch->mKeepAlive = 1;
        }
    }
    StopWaveChannels(mask);
}

// Sends the timer of the last active channel playing id, scaled by scale / 0x100, to all of them (mTimer is not
// changed). Never called in this build: PlaySeEx calls it only for a scale other than 0x100, and PlaySe, the only
// caller of PlaySeEx, passes 0x100.
void ScaleWaveTimer(int id, int scale) {
    int i;
    u32 mask = 0;
    u32 timer = 0;
    for (i = 0; i < (u32) gWavePlayer.mChannelCount; i++) {
        WaveChannel *ch = &gWavePlayer.mChannels[i];
        if (ch->mUsed == 1 && id == ch->mId) {
            mask |= 1 << i;
            timer = ch->mTimer;
        }
    }
    func_020401d8(mask, (timer * (u32) scale) >> 8);
}

void OpenWaveStream(WaveStream *stream, const char *name, u32 rate) {
    if (gWavePlayer.mStreamOpen != 0) {
        OS_Terminate();
    }
    gWavePlayer.mStreamOpen = 1;
    gWavePlayer.mStream = stream;
    if (func_02042490(&stream->mFile, name) != 1) {
        OS_Terminate();
    }
    strcpy(stream->mName, name);
    stream->mRate = rate;
    stream->mChunk = stream->mRate * 2;
    stream->mBuffer = Alloc(stream->mChunk * 4);
    stream->mCurrent = -1;
    stream->mPause = 0;
    stream->mPlayFrames = 0;
}

void CloseWaveStream(WaveStream *stream) {
    if (gWavePlayer.mStreamOpen != 1) {
        OS_Terminate();
    }
    StopWave(WAVE_STREAM_KEY);
    func_020424d8(&stream->mFile);
    Free(&stream->mBuffer);
    gWavePlayer.mStreamOpen = 0;
}

void UpdateWaveStream(WaveStream *stream) {
    if (gWavePlayer.mStreamOpen != 1) {
        OS_Terminate();
    }
    if (stream->mPause != 1) {
        if (stream->mPause == 0) {
            if (stream->mCurrent == -1) {
                stream->mCurrent = 0;
                func_020425f4(&stream->mFile, 0, 0);
                func_020425e4(&stream->mFile, stream->mBuffer + stream->mCurrent * stream->mChunk, stream->mChunk);
                PlayWaveBuffer(WAVE_STREAM_ID, 0, 100, 1, 0x40, 0xfffe, 0, stream->mBuffer, stream->mRate,
                               stream->mChunk * 4, 1);
            } else {
                s32 next = (stream->mCurrent + 1) % 4;
                u32 cur = (((stream->mRate << 8) / 60 * stream->mPlayFrames >> 8) / stream->mChunk) & 3;
                if (next != cur) {
                    u32 got;
                    stream->mCurrent = next;
                    got = func_020425e4(&stream->mFile, stream->mBuffer + stream->mCurrent * stream->mChunk,
                                        stream->mChunk);
                    if (got < stream->mChunk) {
                        func_020425f4(&stream->mFile, 0, 0);
                        func_020425e4(&stream->mFile, stream->mBuffer + got + stream->mCurrent * stream->mChunk,
                                      stream->mChunk - got);
                    }
                }
            }
            stream->mPlayFrames++;
        } else {
            ResumeWaveStream(stream);
        }
    }
}

void PauseWaveStream(WaveStream *stream) {
    int index;
    u32 i;
    if (gWavePlayer.mStreamOpen != 1) {
        OS_Terminate();
    }
    if (stream->mPause == 0) {
        FindWaveChannel(WAVE_STREAM_KEY, &index);
        stream->mPause = 1;
        func_020401d8(1 << index, 0);
        func_020401f8(1 << index, 0, 0);
        i = 0;
        while (SND_RecvCommandReply(i) != 0) {
        }
        SND_FlushCommand(2);
        gWavePlayer.mStreamPaused = 1;
    }
}

void ResumeWaveStream(WaveStream *stream) {
    int index;
    u32 i;
    if (gWavePlayer.mStreamOpen != 1) {
        OS_Terminate();
    }
    if (stream->mPause == 0) {
        return;
    }
    if (stream->mPause < 0x3c) {
        stream->mPause++;
        return;
    }
    FindWaveChannel(WAVE_STREAM_KEY, &index);
    stream->mPause = 0;
    func_020401d8(1 << index, gWavePlayer.mChannels[index].mTimer);
    func_020401f8(1 << index, (gWavePlayer.mChannels[index].mVolume * 0x145) >> 8, 0);
    i = 0;
    while (SND_RecvCommandReply(i) != 0) {
    }
    SND_FlushCommand(2);
    gWavePlayer.mStreamPaused = 0;
}
