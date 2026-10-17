#include <mobiclip.h>
#include <nitro/fs.h>
#include <nitro/init.h>
#include <nitro/mi.h>
#include <Main/Unknown/heap.h>

void *Player::operator new(unsigned long size) {
    return func_0202f500(size);
}

void Player::operator delete(void *p) {
    func_0202f50c(p);
}

Player::Player() {
    mReader = 0;
    mPlaneY = 0;
    mPlaneC = 0;
    mUnk_68 = 0;
    mUnk_6c = 0;
    mBuf[0] = 0;
    mBuf[1] = 0;
    mUnk_64 = 0;
    mAudio = 0;
    mUnk_94 = 0;
}

Player::~Player() {
    func_0203a65c(this);
}

// Reads and checks the file header, then allocates the decoder state, the n pictures and the stream buffers.
int func_0203a0b0(Player *p, Reader *r, u32 n) {
    p->mReader = r;
    p->mBase = r->mPos;
    if (!p->mReader->Read(p->mMagic, MOBICLIP_HEADER_SIZE)) {
        return 0;
    }
    if (p->mMagic[0] != 'M' || p->mMagic[1] != 'O' || p->mMagic[2] != 'D' || p->mMagic[3] != 'S' ||
        p->mMagic[4] != 'N' || (p->mMagic[5] != '2' && p->mMagic[5] != '3') || p->mVersion != MOBICLIP_VERSION) {
        return 0;
    }
    if (p->mMagic[4] == 'N' && p->mMagic[5] == '3') {
        char tag[2];
        u16 off;
        do {
            if (!p->mReader->Read(tag, 2)) {
                return 0;
            }
            if (!p->mReader->Read(&off, 2)) {
                return 0;
            }
            if (!p->mReader->Seek((void *) ((s32) (off << 2) + p->mReader->mPos))) {
                return 0;
            }
        } while (!(tag[0] == 'H' && tag[1] == 'E'));
    }
    if (p->mAudioType != 0 && p->mAudioType != 1 && p->mAudioType != 2 && p->mAudioType != 3 && p->mAudioType != 4) {
        return 0;
    }
    p->mCtx = (Ctx *) func_0202f500(sizeof(Ctx));
    MIi_CpuClearFast(0, p->mCtx, sizeof(Ctx));
    p->mUnk_38 = (int (*)(Ctx *)) func_0203b584();
    p->mCtx->mUnk_3c = func_0203b600();
    p->mCtx->mUnk_40 = data_020541d4;
    p->mCtx->mUnk_44 = (u8 *) func_0203b6e0() + 0x40;
    p->mUnk_4c = p->mWidth;
    p->mUnk_50 = p->mHeight;
    p->mUnk_54 = func_0203b670();
    p->mCtx->mWidth = p->mWidth;
    p->mCtx->mHeight = p->mHeight;
    if (p->mWidth > MOBICLIP_MAX_WIDTH) {
        return 0;
    }
    if (n < MOBICLIP_MIN_PLANES) {
        return 0;
    }
    p->mNumPlanes = n;
    p->mPlaneY = (u8 **) func_0202f500(n * 4);
    if (p->mPlaneY == 0) {
        return 0;
    }
    p->mPlaneC = (u8 **) func_0202f500(p->mNumPlanes * 4);
    if (p->mPlaneC == 0) {
        return 0;
    }
    for (u32 i = 0; i < p->mNumPlanes; i++) {
        p->mPlaneY[i] = (u8 *) func_0202f500(p->mHeight << 8);
        if (p->mPlaneY[i] == 0) {
            return 0;
        }
        p->mPlaneC[i] = (u8 *) func_0202f500((p->mHeight >> 1) << 8);
        if (p->mPlaneC[i] == 0) {
            return 0;
        }
        MIi_CpuClearFast(0, p->mPlaneY[i], p->mHeight << 8);
        MI_CpuFill8(p->mPlaneC[i], 0x80, (p->mHeight >> 1) << 8);
    }
    p->mUnk_64 = (void **) func_0202f500(p->mNumPlanes * 4);
    if (p->mUnk_64 == 0) {
        return 0;
    }
    p->mUnk_98 = 0;
    p->mUnk_ac[0] = 0;
    for (int i = 1; i < 6; i++) {
        p->mUnk_ac[i] = p->mNumPlanes - i;
    }
    p->mUnk_c4 = 0;
    p->mUnk_a0 = 0;
    p->mUnk_9c = 0;
    for (int i = 0; i < 2; i++) {
        p->mBuf[i] = (u8 *) func_0202f500(p->mUnk_24 + MOBICLIP_BUFFER_PAD);
        if (p->mBuf[i] == 0) {
            return 0;
        }
    }
    if (p->mAudioType != 0) {
        p->mAudio = (u8 *) func_0202f500(p->mChannels * MOBICLIP_AUDIO_STRIDE);
        if (p->mAudio == 0) {
            return 0;
        }
        MIi_CpuClearFast(0, p->mAudio, p->mChannels * MOBICLIP_AUDIO_STRIDE);
    }
    int saved = p->mReader->mPos;
    p->mReader->Seek((void *) (p->mUnk_2c + p->mBase));
    p->mUnk_94 = (u8 *) func_0202f500(p->mUnk_30 << 3);
    if (!p->mReader->Read(p->mUnk_94, p->mUnk_30 << 3)) {
        return 0;
    }
    p->mReader->Seek((void *) saved);
    if (p->mAudioType == 1) {
        int i;
        int saved2 = p->mReader->mPos;
        p->mReader->Seek((void *) (p->mUnk_28 + p->mBase));
        for (i = 0; i < p->mChannels; i++) {
            if (!p->mReader->Read(p->mAudio + i * MOBICLIP_AUDIO_STRIDE + 0x8c0, 0xc34)) {
                return 0;
            }
            *(u32 *) (p->mAudio + i * MOBICLIP_AUDIO_STRIDE + 0x14f4) = 2;
        }
        p->mReader->Seek((void *) saved2);
    } else if (p->mAudioType == 2) {
        for (int i = 0; i < p->mChannels; i++) {
            *(u32 *) (p->mAudio + i * MOBICLIP_AUDIO_STRIDE + 0x1cc) = 2;
        }
    }
    u32 w;
    p->mReader->Read(&w, 4);
    p->mUnk_78[0] = w >> 14;
    p->mUnk_88[0] = w & 0x3fff;
    p->mUnk_80[0] = 0;
    p->mReader->Read(p->mBuf[0], p->mUnk_78[0] + 4);
    p->mCur = 1;
    return 1;
}

// Closes the reader and frees everything the player allocated.
void func_0203a65c(Player *p) {
    if (p->mReader != 0) {
        p->mReader->Close();
        if (p->mReader != 0) {
            delete p->mReader;
        }
    }
    func_0202f50c(p->mCtx);
    if (p->mPlaneY != 0) {
        for (u32 i = 0; i < p->mNumPlanes; i++) {
            func_0202f50c(p->mPlaneY[i]);
        }
        func_0202f50c(p->mPlaneY);
    }
    if (p->mPlaneC != 0) {
        for (u32 i = 0; i < p->mNumPlanes; i++) {
            func_0202f50c(p->mPlaneC[i]);
        }
        func_0202f50c(p->mPlaneC);
    }
    if (p->mUnk_64 != 0) {
        func_0202f50c(p->mUnk_64);
    }
    if (p->mUnk_68 != 0) {
        func_0202f50c(p->mUnk_68);
    }
    if (p->mUnk_6c != 0) {
        func_0202f50c(p->mUnk_6c);
    }
    for (int i = 0; i < 2; i++) {
        if (p->mBuf[i] != 0) {
            func_0202f50c(p->mBuf[i]);
        }
        p->mBuf[i] = 0;
    }
    if (p->mUnk_94 != 0) {
        func_0202f50c(p->mUnk_94);
    }
    if (p->mAudio != 0) {
        func_0202f50c(p->mAudio);
    }
    p->mReader = 0;
    p->mPlaneY = 0;
    p->mPlaneC = 0;
    p->mUnk_68 = 0;
    p->mUnk_6c = 0;
    p->mUnk_64 = 0;
    p->mAudio = 0;
    p->mUnk_94 = 0;
}

int func_0203a7c4(Player *p) {
    return p->mUnk_20;
}

int func_0203a7cc(Player *p) {
    return p->mRate;
}

int func_0203a7d4(Player *p) {
    return p->mFrames;
}

int func_0203a7dc(Player *p) {
    if (p->mUnk_20 == 0) {
        return 0;
    }
    return p->mUnk_28 != 0 ? 0x80 : 0x100;
}

// Moves on to the next frame: swaps the two stream buffers, sets the decoder's read position to the frame in the
// new one, starts a read of the following frame into the other one and binds the pictures the decoder uses.
int func_0203a800(Player *p) {
    if (p->mUnk_98 == p->mFrames) {
        return 0;
    }
    p->mUnk_98++;
    int old = p->mCur;
    p->mCur = (old + 1) % 2;
    p->mReader->v5();
    u32 w = *(u32 *) (p->mBuf[p->mCur] + (p->mUnk_80[p->mCur] + p->mUnk_78[p->mCur]));
    p->mUnk_78[old] = w >> 14;
    p->mUnk_88[old] = w & 0x3fff;
    p->mCtx->ptr = p->mBuf[p->mCur] + p->mUnk_80[p->mCur];
    if (p->mUnk_98 < p->mFrames) {
        p->mUnk_80[old] = p->mReader->v4(p->mBuf[old], p->mUnk_78[old] + 4);
    }
    for (int i = 0; i < 6; i++) {
        p->mCtx->mUnk_c[i] = p->mPlaneY[p->mUnk_ac[i]];
        p->mCtx->mUnk_24[i] = p->mPlaneC[p->mUnk_ac[i]];
    }
    p->mUnk_c8 = p->mChannels * p->mUnk_88[p->mCur];
    return 1;
}

// Decodes the picture of the current frame.
int func_0203a938(Player *p) {
    if (p->mUnk_9c > p->mUnk_a0) {
        return 0;
    }
    int k;
    if (*(u16 *) p->mCtx->ptr & 0x8000) {
        p->mUnk_a4 = 1;
        k = 4;
    } else {
        k = 0;
        p->mUnk_a4 = k;
    }
    p->mCtx->ptr += p->mUnk_38(p->mCtx);
    if (p->mMagic[4] == 'N' && p->mMagic[5] == '3') {
        p->mCtx->ptr += k;
    }
    p->mUnk_64[p->mUnk_ac[0]] = p->mCtx->mUnk_3b4;
    p->mUnk_cc = 0;
    p->mUnk_d0 = 0;
    for (int i = 0; i < 6; i++) {
        p->mUnk_ac[i]++;
        if (p->mNumPlanes == p->mUnk_ac[i]) {
            p->mUnk_ac[i] = 0;
        }
    }
    p->mUnk_a0++;
    return 1;
}

// Converts the next picture to 16-bit pixels at a (b is the row length in pixels); a nonzero c first passes the
// picture through a decoder routine into temporary pictures.
int func_0203aa1c(Player *p, u32 a, u32 b, int c) {
    if (p->mUnk_9c >= p->mUnk_a0) {
        return 0;
    }
    if (c != 0) {
        if (p->mUnk_68 == 0) {
            p->mUnk_68 = func_0202f500(p->mHeight << 8);
            if (p->mUnk_68 == 0) {
                return 0;
            }
        }
        if (p->mUnk_6c == 0) {
            p->mUnk_6c = func_0202f500((p->mHeight >> 1) << 8);
            if (p->mUnk_6c == 0) {
                return 0;
            }
        }
        void *s[9];
        s[0] = p->mPlaneY[p->mUnk_c4];
        s[1] = p->mPlaneC[p->mUnk_c4];
        s[2] = p->mUnk_68;
        s[3] = p->mUnk_6c;
        s[4] = (void *) p->mWidth;
        s[5] = (void *) p->mHeight;
        s[6] = p->mUnk_64[p->mUnk_c4];
        s[7] = func_0203b6e0();
        if (c == 2) {
            s[8] = 0;
        } else {
            s[8] = (void *) 1;
        }
        func_0205e970(s);
        p->mUnk_3c = p->mUnk_68;
        p->mUnk_40 = p->mUnk_6c;
    } else {
        p->mUnk_3c = p->mPlaneY[p->mUnk_c4];
        p->mUnk_40 = p->mPlaneC[p->mUnk_c4];
    }
    p->mUnk_44 = (void *) a;
    p->mUnk_48 = b << 1;
    func_0203af68(&p->mUnk_3c);
    p->mUnk_9c++;
    p->mUnk_c4++;
    if (p->mUnk_c4 == p->mNumPlanes) {
        p->mUnk_c4 = 0;
    }
    return 1;
}

int func_0203aba0(Player *p) {
    if (p->mUnk_9c >= p->mUnk_a0) {
        return 0;
    }
    p->mUnk_9c++;
    p->mUnk_c4++;
    if (p->mUnk_c4 == p->mNumPlanes) {
        p->mUnk_c4 = 0;
    }
    return 1;
}

int func_0203abe0(Player *p) {
    return p->mUnk_88[p->mCur];
}

#define CHANNEL(p) ((Chan *) ((p)->mAudio + (p)->mUnk_d0 * MOBICLIP_AUDIO_STRIDE))

// Decodes one block of audio of the current frame into buffer.
int func_0203abf0(Player *p, void *buffer) {
    if (p->mUnk_cc == p->mUnk_c8) {
        return 0;
    }
    u32 t = p->mAudioType;
    if (t == 0) {
        return 0;
    }
    if (t == 1) {
        CHANNEL(p)->mUnk_0 = p->mCtx->ptr;
        CHANNEL(p)->mUnk_4 = (u32) buffer;
        p->mCtx->ptr += func_02052468(CHANNEL(p));
    } else if (t == 2) {
        CHANNEL(p)->mUnk_0 = p->mCtx->ptr;
        CHANNEL(p)->mUnk_4 = (u32) buffer;
        func_02052e28(CHANNEL(p));
        p->mCtx->ptr += 0x28;
    } else if (t == 3) {
        if (p->mUnk_a4 == 1) {
            func_0203b750((AdSt *) CHANNEL(p), p->mCtx->ptr);
            p->mCtx->ptr += 4;
        }
        func_0203b764((AdSt *) CHANNEL(p), p->mCtx->ptr, 0x80, (u32 *) buffer);
        p->mCtx->ptr += 0x80;
    } else {
        MIi_CpuCopy16(p->mCtx->ptr, buffer, 0x200);
        p->mCtx->ptr += 0x200;
    }
    p->mUnk_d0++;
    if (p->mUnk_d0 == p->mChannels) {
        p->mUnk_d0 = 0;
        p->mUnk_a4 = 0;
    }
    p->mUnk_cc++;
    return 1;
}

void *Reader::operator new(unsigned long size) {
    return func_0202f500(size);
}

void Reader::operator delete(void *p) {
    func_0202f50c(p);
}

// Binds the reader to an open file; the position is kept relative to the start of the file.
int func_0203adb8(Reader *r, FSFile *file) {
    r->file = file;
    r->mUnk_4 = file->prop.file.bottom - file->prop.file.top;
    r->mPos = r->file->prop.file.pos - r->file->prop.file.top;
    r->flag = 0;
    return 1;
}

int Reader::Seek(void *addr) {
    if (flag == 1) {
        func_02042514(file);
        flag = 0;
    }
    if (func_020425f4(file, (u32) addr, 0) == 0) {
        return 0;
    }
    mPos = (u32) addr;
    return 1;
}

int Reader::Read(void *dst, u32 size) {
    if (flag == 1) {
        func_02042514(file);
        func_020425f4(file, mPos, 0);
        flag = 0;
    }
    if (func_020425e4(file, dst, size) == -1) {
        return 0;
    }
    mPos += size;
    return 1;
}

// Starts a read of whole 0x200-byte sectors that covers size bytes at the current position; returns the
// offset of the requested bytes in dst.
int Reader::v4(u8 *dst, u32 size) {
    u32 aligned = mPos & -0x200;
    u32 off = mPos - aligned;
    u32 len = size + off;
    if (len & 0x1ff) {
        len = (len & -0x200) + 0x200;
    }
    func_020425f4(file, aligned, 0);
    if (func_020425d4(file, dst, len) == -1) {
        return 0;
    }
    mPos += size;
    flag = 1;
    return off;
}

// Waits for the read started by v4.
void Reader::v5() {
    if (flag == 1) {
        func_02042514(file);
    }
    flag = 0;
}

void Reader::Close() {
    v5();
}
