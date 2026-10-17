#pragma once

#include "types.h"
#include <nitro/fs.h>

// Bytes of the file header, and the format version the player accepts.
#define MOBICLIP_HEADER_SIZE 0x30
#define MOBICLIP_VERSION 10
// Widest picture accepted, and the fewest pictures the player must be given to work with.
#define MOBICLIP_MAX_WIDTH 0x100
#define MOBICLIP_MIN_PLANES 6
// Bytes of decoder state per audio channel, and the slack after each stream buffer.
#define MOBICLIP_AUDIO_STRIDE 0x14f8
#define MOBICLIP_BUFFER_PAD 0x400

#ifdef __cplusplus

// Source of the video data. Every slot but the destructor is pure here.
struct ReaderBase {
    ReaderBase() {
        mUnk_4 = 0;
        mPos = 0;
    }
    virtual ~ReaderBase() {}
    virtual int Seek(void *addr) = 0;
    virtual int Read(void *dst, u32 size) = 0;
    virtual int v4(u8 *dst, u32 size) = 0;
    virtual void v5() = 0;
    virtual void Close() = 0;
    s32 mUnk_4;
    s32 mPos;
};

// The video data read from an open file.
struct Reader : ReaderBase {
    static void *operator new(unsigned long size);
    static void operator delete(void *p);
    virtual ~Reader() {}
    virtual int Seek(void *addr);
    virtual int Read(void *dst, u32 size);
    virtual int v4(u8 *dst, u32 size);
    virtual void v5();
    virtual void Close();
    FSFile *file;
    u8 flag;
};

// Decoder state; its first word is the read position in the frame being decoded.
struct Ctx {
    u8 *ptr;
    u32 mWidth;
    u32 mHeight;
    void *mUnk_c[6];
    void *mUnk_24[6];
    void *mUnk_3c;
    void *mUnk_40;
    void *mUnk_44;
    u8 mPad48[0x3b4 - 0x48];
    void *mUnk_3b4;
    u8 mPad3b8[0x454 - 0x3b8];
};

// Predictor and step index of one channel of 4-bit audio.
struct AdSt {
    s32 pred;
    s32 idx;
};

// Decoder state of one audio channel; the first two words depend on the audio type.
struct Chan {
    u8 *mUnk_0;
    u32 mUnk_4;
    u8 mPad8[MOBICLIP_AUDIO_STRIDE - 8];
};

struct Player {
    Reader *mReader;       // 00
    char mMagic[6];        // 04
    u16 mVersion;          // 0a
    u32 mFrames;           // 0c
    u32 mWidth;            // 10
    u32 mHeight;           // 14
    u32 mRate;             // 18
    u16 mAudioType;        // 1c
    u16 mChannels;         // 1e
    u32 mUnk_20;           // 20
    u32 mUnk_24;           // 24
    u32 mUnk_28;           // 28
    u32 mUnk_2c;           // 2c
    u32 mUnk_30;           // 30
    Ctx *mCtx;             // 34
    int (*mUnk_38)(Ctx *); // 38
    void *mUnk_3c;         // 3c
    void *mUnk_40;         // 40
    void *mUnk_44;         // 44
    u32 mUnk_48;           // 48
    u32 mUnk_4c;           // 4c
    u32 mUnk_50;           // 50
    void *mUnk_54;         // 54
    u8 *mAudio;            // 58
    u8 **mPlaneY;          // 5c
    u8 **mPlaneC;          // 60
    void **mUnk_64;        // 64
    void *mUnk_68;         // 68
    void *mUnk_6c;         // 6c
    u8 *mBuf[2];           // 70
    u32 mUnk_78[2];        // 78
    u32 mUnk_80[2];        // 80
    u32 mUnk_88[2];        // 88
    s32 mCur;              // 90
    u8 *mUnk_94;           // 94
    u32 mUnk_98;           // 98
    u32 mUnk_9c;           // 9c
    u32 mUnk_a0;           // a0
    u32 mUnk_a4;           // a4
    u32 mNumPlanes;        // a8
    u32 mUnk_ac[6];        // ac
    u32 mUnk_c4;           // c4
    u32 mUnk_c8;           // c8
    u32 mUnk_cc;           // cc
    u32 mUnk_d0;           // d0
    u32 mBase;             // d4
    static void *operator new(unsigned long size);
    static void operator delete(void *p);
    Player();
    ~Player();
};

extern "C" {

#else

typedef struct Player Player;
typedef struct Reader Reader;
typedef struct AdSt AdSt;
typedef struct Chan Chan;

#endif

int func_02039dd8(Player *p, Reader *r, u32 n);
Player *func_02039e08(FSFile *src, u32 n);
void func_02039ef0(Player *p);
int func_02039f10(Player *p);
int func_02039f28(Player *p);
int func_02039f40(Player *p);
int func_02039f58(Player *p);
int func_02039f70(Player *p);
int func_02039f94(Player *p);
int func_02039fb8(Player *p, u32 a, u32 b, int c);
int func_02039fdc(Player *p);
int func_0203a000(Player *p);
int func_0203a018(Player *p, void *buffer);
int func_0203a0b0(Player *p, Reader *r, u32 n);
void func_0203a65c(Player *p);
int func_0203a7c4(Player *p);
int func_0203a7cc(Player *p);
int func_0203a7d4(Player *p);
int func_0203a7dc(Player *p);
int func_0203a800(Player *p);
int func_0203a938(Player *p);
int func_0203aa1c(Player *p, u32 a, u32 b, int c);
int func_0203aba0(Player *p);
int func_0203abe0(Player *p);
int func_0203abf0(Player *p, void *buffer);
int func_0203adb8(Reader *r, FSFile *file);
void func_0203af68(void *args);
u32 func_0203b528(void);
u32 func_0203b534(void);
void func_0203b53c(u8 *p, u32 n);
void func_0203b55c(u8 *p, u32 n);
void *func_0203b584(void);
void *func_0203b600(void);
void *func_0203b670(void);
void *func_0203b6e0(void);
void func_0203b750(AdSt *st, u8 *p);
void func_0203b764(AdSt *st, u8 *in, int n, u32 *out);

// One of the decoder's tables.
extern u8 data_020541d4[];

// Decoder entry points: picture filter (nine argument words), and the two audio channel decoders.
void func_0205e970(void *args);
int func_02052468(Chan *ch);
void func_02052e28(Chan *ch);

#ifdef __cplusplus
}
#endif
