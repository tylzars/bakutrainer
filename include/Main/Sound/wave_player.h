#pragma once

#include "types.h"
#include <nitro/fs.h>

#ifdef __cplusplus
extern "C" {
#endif

#define WAVE_CHANNEL_COUNT 16
#define WAVE_FORMAT_MAGIC 0x4453504d

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
    /* 00 */ u32 mUnk_00;
    /* 04 */ u32 mId;
    /* 08 */ const void *mWave;
    /* 0c */ u8 mUnk_0c;
    /* 0d */ u8 mActive : 1;
    u8 mUnk_0d_1 : 1;
    u8 mUnk_0d_2 : 1;
    u8 mUnk_0d_3 : 1;
    /* 0e */ u8 mUnk_0e[2];
    /* 10 */ u32 mTimer;
    /* 14 */ u32 mUnk_14;
} WaveChannel; /* 18 */

// A wave stream read from a file in four chunks (a ring of mChunk * 4 bytes) and played through a wave channel.
typedef struct WaveStream {
    /* 00 */ FSFile mFile;
    /* 48 */ u32 mChunk;
    /* 4c */ u32 mRate;
    /* 50 */ u8 *mBuffer;
    /* 54 */ s32 mCurrent;
    /* 58 */ u32 mPause;
    /* 5c */ u32 mFrame;
    /* 60 */ char mName[0x40];
} WaveStream;

typedef struct WavePlayer {
    /* 000 */ WaveChannel mUnk_000[WAVE_CHANNEL_COUNT];
    /* 180 */ u8 mUnk_180;
    /* 181 */ u8 mUnk_181;
    /* 182 */ u8 mUnk_182[2];
    /* 184 */ s32 mUnk_184;
    /* 188 */ s32 mUnk_188;
    /* 18c */ WaveStream *mUnk_18c;
} WavePlayer; /* 190 */

extern WavePlayer data_020c648c;

// Claims a channel from the set of channel bits `mask`: the first slot in the mask that is not in use, or, when
// `steal` is set and none is free, the first slot in the mask (releasing it first). Returns the slot number, or
// the number of slots when the mask holds no slot.
int func_0202ea50(int steal, u32 mask);
void func_0202eb18(u32 id, int x, int volume, int repeat, int pan, u32 mask, WaveLoop *loop, const void *data, int rate,
                   u32 size, int mode);
void func_0202eb64(int id, int volume, int repeat, int pan, u32 mask, WaveLoop *loop);
void func_0202eb84(u32 id, int x, int volume, int repeat, int pan, u32 mask, WaveLoop *loop);
void func_0202ebb8(const WaveInfo *info, const void *data, u32 id, int x, u32 volume, int repeat, int pan, u32 mask,
                   WaveLoop *loop);
void func_0202ed90(u32 mask);
int func_0202edd0(u32 id);
int func_0202ee1c(u32 id);
int func_0202ee28(u32 id, int *index);
void func_0202ee68(u32 id, u32 volume);
void func_0202eeec(void);
void func_0202ef24(void);
void func_0202efd4(void);
void func_0202efe4(u32 index);
void func_0202f00c(void);
void func_0202f05c(void);
void func_0202f084(void);
void func_0202f110(int a, int b, int c, int d, int e, int f);
void func_0202f200(void);
void func_0202f240(int id, int volume);
void func_0202f290(WaveStream *stream, const char *name, u32 rate);
void func_0202f2e8(WaveStream *stream);
void func_0202f31c(WaveStream *stream);
void func_0202f410(WaveStream *stream);
void func_0202f470(WaveStream *stream);

#ifdef __cplusplus
}
#endif
