#include <Main/clib.h>
#include <nitro/fs.h>
#include <nitro/os.h>
#include <nitro/snd.h>
#include <Main/Unknown/functions.h>
#include <Main/Unknown/globals.h>
#include <Main/Unknown/wave_player.h>
#include <Main/Unknown/file_loader.h>

WavePlayer data_020c648c;

#pragma thumb off

int func_0202ea50(int steal, u32 mask) {
    int second = 0;
    u32 i;
    u32 bit;

    for (;;) {
        for (i = 0; i < data_020c648c.mUnk_180; i++) {
            bit = 1 << i;

            if ((mask & bit) && (data_020c648c.mUnk_000[i].mActive == 0 || second == 1)) {
                WaveChannel *s = &data_020c648c.mUnk_000[i];

                if (s->mActive == 1) {
                    func_0202ed90(bit);
                }
                s->mActive = 1;
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

void func_0202eb18(u32 id, int x, int volume, int repeat, int pan, u32 mask, WaveLoop *loop, const void *data, int rate,
                   u32 size, int mode) {
    WaveInfo info;
    info.mRate = rate;
    info.mUnk_02 = 1;
    info.mSize = size;
    info.mFormat = (mode == 1) ? 2 : 1;
    info.mUnk_0a = 0;
    func_0202ebb8(&info, data, id | 0x40000000, x, volume, repeat, pan, mask, loop);
}

void func_0202eb64(int id, int volume, int repeat, int pan, u32 mask, WaveLoop *loop) {
    func_0202eb84(id, 0, volume, repeat, pan, mask, loop);
}

void func_0202eb84(u32 id, int x, int volume, int repeat, int pan, u32 mask, WaveLoop *loop) {
    func_0202ebb8(0, func_0202ded8((u16) id), id, x, volume, repeat, pan, mask, loop);
}

void func_0202ebb8(const WaveInfo *info, const void *data, u32 id, int x, u32 volume, int repeat, int pan, u32 mask,
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
    if (data_020c18bc.mUnk_494 == 0) {
        return;
    }
    key = id & 0x4000ffff;
    index = func_0202ea50((id & 0x80000000) ? 1 : 0, mask);
    if (index == data_020c648c.mUnk_180) {
        return;
    }
    data_020c648c.mUnk_000[index].mId = key;
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
    data_020c648c.mUnk_000[index].mWave = data;
    ch = &data_020c648c.mUnk_000[index];
    ch->mUnk_0d_2 = repeat;
    timer = 0xffb0ff / rate;
    loopStart = 0;
    ch->mUnk_0d_1 = 0;
    data_020c648c.mUnk_000[index].mTimer = timer;
    data_020c648c.mUnk_000[index].mUnk_14 = volume;
    ch->mUnk_0d_3 = 0;
    if (x != 0) {
        x = rate * x / 60;
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
            x <<= 1;
            break;
        case 2:
            mode = 0;
            break;
    }
    if (size > x) {
        x &= ~3;
        size -= x;
        if (repeat == 1 && loop != 0) {
            loopStart = loop->mStart >> 2;
            size = loop->mEnd - loop->mStart;
        }
        func_02040240(index, mode, (u32) body + (x & ~1), (repeat == 1) ? 1 : 2, loopStart, size >> 2,
                      volume * 0x145 >> 8, 0, timer, pan);
        func_020400cc(1 << index, 0, 0, 0);
        ch->mUnk_00 = SND_GetCurrentCommandTag();
    }
}

void func_0202ed90(u32 mask) {
    u32 i;
    if (mask != 0) {
        func_020400f4(mask, 0, 0, 0);
        func_020401b8(mask, 0);
        for (i = 0; i < data_020c648c.mUnk_180; i++) {
            if (mask & (1 << i)) {
                func_0202efe4(i);
            }
        }
    }
}

int func_0202edd0(u32 id) {
    u32 mask = 0;
    u32 i;
    for (i = 0; i < data_020c648c.mUnk_180; i++) {
        if (data_020c648c.mUnk_000[i].mActive == 1 && id == data_020c648c.mUnk_000[i].mId) {
            mask |= 1 << i;
        }
    }
    func_0202ed90(mask);
    if (mask == 0) {
        return 0;
    }
    return 1;
}

int func_0202ee1c(u32 id) {
    return func_0202ee28(id, 0);
}

int func_0202ee28(u32 id, int *index) {
    u32 i;
    for (i = 0; i < data_020c648c.mUnk_180; i++) {
        if (data_020c648c.mUnk_000[i].mActive == 1 && id == data_020c648c.mUnk_000[i].mId) {
            if (index != 0) {
                *index = i;
            }
            return 1;
        }
    }
    return 0;
}

void func_0202ee68(u32 id, u32 volume) {
    u32 i;
    u32 mask = 0;
    for (i = 0; i < data_020c648c.mUnk_180; i++) {
        if (data_020c648c.mUnk_000[i].mActive == 1 && id == data_020c648c.mUnk_000[i].mId) {
            mask |= 1 << i;
        }
    }
    if (mask != 0) {
        u32 out = volume;
        for (i = 0; i < data_020c648c.mUnk_180; i++) {
            // Tests the channel number itself against the mask, not its bit.
            if ((mask & i) && data_020c648c.mUnk_000[i].mUnk_0d_1 == 1) {
                out = volume * 0x30 >> 8;
                data_020c648c.mUnk_000[i].mUnk_14 = out;
            }
        }
        func_020401f8(mask, out * 0x145 >> 8, 0);
    }
}

void func_0202eeec(void) {
    u32 mask = 0;
    u32 i;
    for (i = 0; i < data_020c648c.mUnk_180; i++) {
        if (data_020c648c.mUnk_000[i].mActive == 1) {
            mask |= 1 << i;
        }
    }
    func_0202ed90(mask);
}

void func_0202ef24(void) {
    u32 mask = 0;
    u32 busy = func_02040a90();
    u32 i;
    WaveChannel *ch;
    for (i = 0; i < data_020c648c.mUnk_180; i++) {
        ch = &data_020c648c.mUnk_000[i];
        if (ch->mUnk_0d_3 && ch->mUnk_14 != 0) {
            u32 v = ch->mUnk_14;
            ch->mUnk_14 = v - 1;
            func_0202ee68(ch->mId, v);
        }
        if (ch->mActive == 1) {
            if ((ch->mUnk_0d_2 == 0 || data_020c18bc.mUnk_494 == 0) && (busy & (1 << i)) == 0 &&
                SND_IsFinishedCommandTag(ch->mUnk_00) == 1) {
                mask |= 1 << i;
            } else if ((ch->mId & 0x40000000) == 0) {
                if (func_0202ded8(ch->mId) != ch->mWave) {
                    mask |= 1 << i;
                }
            }
        }
    }
    if (mask != 0) {
        func_0202ed90(mask);
    }
    while (SND_RecvCommandReply(0) != 0) {
    }
    SND_FlushCommand(2);
}

void func_0202efd4(void) {
    while (SND_RecvCommandReply(0) != 0) {
    }
}

void func_0202efe4(u32 index) {
    WaveChannel *ch = &data_020c648c.mUnk_000[index];
    ch->mActive = 0;
    data_020c648c.mUnk_000[index].mUnk_0c = 0;
    data_020c648c.mUnk_000[index].mWave = 0;
}

void func_0202f00c(void) {
    u32 i;
    func_0202f05c();
    if ((data_020c18bc.mUnk_46c & 0x1000000) == 0) {
        data_020c648c.mUnk_180 = 0x10;
    } else {
        data_020c648c.mUnk_180 = data_020c648c.mUnk_181;
    }
    data_020c18bc.mUnk_46c |= 0x2000;
    func_020402f0();
    for (i = 0; i < data_020c648c.mUnk_180; i++) {
        func_0202efe4(i);
    }
}

void func_0202f05c(void) {
    if (data_020c18bc.mUnk_46c & 0x2000) {
        func_0202eeec();
        data_020c18bc.mUnk_46c &= ~0x2000;
    }
}

void func_0202f084(void) {
    u32 i;
    u32 cur;
    u32 want;
    u32 mask;
    if ((data_020c18bc.mUnk_46c & 0x2000) == 0) {
        func_0202f00c();
        return;
    }
    if ((data_020c18bc.mUnk_46c & 0x1000000) == 0) {
        data_020c648c.mUnk_181 = 0x10;
        for (i = data_020c648c.mUnk_180; i < data_020c648c.mUnk_181; i++) {
            func_0202efe4(i);
        }
        data_020c648c.mUnk_180 = data_020c648c.mUnk_181;
        return;
    }
    want = data_020c648c.mUnk_181;
    cur = data_020c648c.mUnk_180;
    if (cur == want) {
        return;
    }
    if (cur < want) {
        for (; cur < data_020c648c.mUnk_181; cur++) {
            func_0202efe4(cur);
        }
        data_020c648c.mUnk_180 = data_020c648c.mUnk_181;
    } else {
        mask = 0;
        for (i = want; i < cur; i++) {
            mask |= 1 << i;
        }
        func_0202ed90(mask);
        data_020c648c.mUnk_180 = data_020c648c.mUnk_181;
    }
}

#define WAVE_STREAM_ID 0x4d2
#define WAVE_STREAM_KEY 0x400004d2

void func_0202f110(int a, int b, int c, int d, int e, int f) {
    u32 i;
    u32 bit;
    u32 index;
    if (c == 0) {
        func_0202eb64(a, b, c, d, e, (WaveLoop *) f);
        return;
    }
    if (func_0202ee1c(a) == 0) {
        func_0202eb64(a, b, c, d, e, (WaveLoop *) f);
        for (i = 0; i < data_020c648c.mUnk_180; i++) {
            WaveChannel *ch = &data_020c648c.mUnk_000[i];
            if (a == ch->mId && ch->mUnk_0d_2 == 1) {
                ch->mUnk_0c = 2;
            }
        }
        return;
    }
    for (i = 0; i < data_020c648c.mUnk_180; i++) {
        WaveChannel *ch = &data_020c648c.mUnk_000[i];
        if (a == ch->mId && ch->mUnk_0d_2 == 1) {
            index = i;
            data_020c648c.mUnk_000[index].mUnk_0c = 2;
            bit = 1 << i;
            func_02040220(bit, d);
            if (b != data_020c648c.mUnk_000[index].mUnk_14) {
                if (data_020c648c.mUnk_000[index].mUnk_0d_1 == 1) {
                    b = ((u32) b * 0x30) >> 8;
                }
                func_020401f8(bit, ((u32) b * 0x145) >> 8, 0);
                data_020c648c.mUnk_000[index].mUnk_14 = b;
            }
            return;
        }
    }
}

void func_0202f200(void) {
    u32 mask = 0;
    u32 i;
    for (i = 0; i < data_020c648c.mUnk_180; i++) {
        WaveChannel *ch = &data_020c648c.mUnk_000[i];
        if (ch->mUnk_0c == 1) {
            mask |= 1 << i;
        } else if (ch->mUnk_0c == 2) {
            ch->mUnk_0c = 1;
        }
    }
    func_0202ed90(mask);
}

void func_0202f240(int id, int volume) {
    int i;
    u32 mask = 0;
    u32 timer = 0;
    for (i = 0; i < (u32) data_020c648c.mUnk_180; i++) {
        WaveChannel *ch = &data_020c648c.mUnk_000[i];
        if (ch->mActive == 1 && id == ch->mId) {
            mask |= 1 << i;
            timer = ch->mTimer;
        }
    }
    func_020401d8(mask, (timer * (u32) volume) >> 8);
}

void func_0202f290(WaveStream *stream, const char *name, u32 rate) {
    if (data_020c648c.mUnk_184 != 0) {
        OS_Terminate();
    }
    data_020c648c.mUnk_184 = 1;
    data_020c648c.mUnk_18c = stream;
    if (func_02042490(&stream->mFile, name) != 1) {
        OS_Terminate();
    }
    strcpy(stream->mName, name);
    stream->mRate = rate;
    stream->mChunk = stream->mRate * 2;
    stream->mBuffer = func_0201c430(stream->mChunk * 4);
    stream->mCurrent = -1;
    stream->mPause = 0;
    stream->mFrame = 0;
}

void func_0202f2e8(WaveStream *stream) {
    if (data_020c648c.mUnk_184 != 1) {
        OS_Terminate();
    }
    func_0202edd0(WAVE_STREAM_KEY);
    func_020424d8(&stream->mFile);
    func_0201c4f0(&stream->mBuffer);
    data_020c648c.mUnk_184 = 0;
}

void func_0202f31c(WaveStream *stream) {
    if (data_020c648c.mUnk_184 != 1) {
        OS_Terminate();
    }
    if (stream->mPause != 1) {
        if (stream->mPause == 0) {
            if (stream->mCurrent == -1) {
                stream->mCurrent = 0;
                func_020425f4(&stream->mFile, 0, 0);
                func_020425e4(&stream->mFile, stream->mBuffer + stream->mCurrent * stream->mChunk, stream->mChunk);
                func_0202eb18(WAVE_STREAM_ID, 0, 100, 1, 0x40, 0xfffe, 0, stream->mBuffer, stream->mRate,
                              stream->mChunk * 4, 1);
            } else {
                s32 next = (stream->mCurrent + 1) % 4;
                u32 cur = (((stream->mRate << 8) / 60 * stream->mFrame >> 8) / stream->mChunk) & 3;
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
            stream->mFrame++;
        } else {
            func_0202f470(stream);
        }
    }
}

void func_0202f410(WaveStream *stream) {
    int index;
    u32 i;
    if (data_020c648c.mUnk_184 != 1) {
        OS_Terminate();
    }
    if (stream->mPause == 0) {
        func_0202ee28(WAVE_STREAM_KEY, &index);
        stream->mPause = 1;
        func_020401d8(1 << index, 0);
        func_020401f8(1 << index, 0, 0);
        i = 0;
        while (SND_RecvCommandReply(i) != 0) {
        }
        SND_FlushCommand(2);
        data_020c648c.mUnk_188 = 1;
    }
}

void func_0202f470(WaveStream *stream) {
    int index;
    u32 i;
    if (data_020c648c.mUnk_184 != 1) {
        OS_Terminate();
    }
    if (stream->mPause == 0) {
        return;
    }
    if (stream->mPause < 0x3c) {
        stream->mPause++;
        return;
    }
    func_0202ee28(WAVE_STREAM_KEY, &index);
    stream->mPause = 0;
    func_020401d8(1 << index, data_020c648c.mUnk_000[index].mTimer);
    func_020401f8(1 << index, (data_020c648c.mUnk_000[index].mUnk_14 * 0x145) >> 8, 0);
    i = 0;
    while (SND_RecvCommandReply(i) != 0) {
    }
    SND_FlushCommand(2);
    data_020c648c.mUnk_188 = 0;
}
