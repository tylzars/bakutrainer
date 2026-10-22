#include <Main/System/bg_image.h>
#include <Main/System/system_init.h>
#include <Main/System/file_stream.h>
#include <Main/Map/map_entries.h>
#include <Main/System/file_loader.h>
#include <Main/Graphics/gx_pool.h>
#include <Main/Graphics/model.h>
#include <nitro/gx.h>
#include <nitro/mi.h>
#include <nitro/os.h>

#define POOL_FLAG 0x80
#define TEX_MAGIC 0x32584554

typedef struct Unk_TexHeader {
    u32 mUnk_0;
    u32 mUnk_4;
} Unk_TexHeader;

void InitGxPool(int texFile, int plttFile, int c, int d) {
    int v;
    Unk_TexHeader hdr;
    if (gSystemState.mFlags & POOL_FLAG) {
        return;
    }
    *(vu16 *) 0x04000304 |= 0xc;
    G3X_Init();
    G3X_InitMtxStack();
    ReallocVramBanks(10, c, -1);
    ReallocVramBanks(11, d, -1);
    data_020c31dc.mTexBlockCount = (gSystemState.mTexVramGroup.mSize16Kb << 14) / 512;
    data_020c31dc.mPlttBlockCount = (gSystemState.mPlttVramGroup.mSize16Kb << 14) / 32;
    *(vu32 *) 0x04000060 |= 9;
    data_020c31dc.mTexFile = texFile;
    data_020c31dc.mPlttFile = plttFile;
    v = FindCachedFile(texFile);
    if (v < 0) {
        SeekBank(-v);
        ReadBank(&hdr, 8);
    } else {
        hdr.mUnk_0 = ((Unk_TexHeader *) v)->mUnk_0;
        hdr.mUnk_4 = ((Unk_TexHeader *) v)->mUnk_4;
    }
    if (hdr.mUnk_0 != TEX_MAGIC) {
        OS_Terminate();
    }
    data_020c31dc.mTexFileKeyCount = (GetStoredFileSize(texFile) - hdr.mUnk_4) >> 2;
    v = FindCachedFile(plttFile);
    if (v < 0) {
        SeekBank(4 - v);
        ReadBank(&v, 4);
    } else {
        v = ((Unk_TexHeader *) v)->mUnk_4;
    }
    data_020c31dc.mPlttFileKeyCount = (GetStoredFileSize(plttFile) - v) >> 2;
    data_020c31dc.mStopNodeIndex = 0xff;
    ResetTexCache();
    ResetPlttCache();
    gSystemState.mFlags |= POOL_FLAG;
}

void func_020220c8(int a) {
    u16 *p = FindOrLoadCachedFile(a);
    if (p[0] != 0x5645) {
        OS_Terminate();
    }
    if (p[1] != 0x3030) {
        OS_Terminate();
    }
    p += 8;
    if (*p != 0x4400) {
        do {
            if ((*p >> 8) != 0x44) {
                return;
            }
            p = (u16 *) ((u8 *) p + p[1] * 16);
        } while (*p != 0x4400);
    }
    MIi_CpuCopy16(p + 2, (void *) 0x04000380, 0x40);
}

// Frees (once the pool is set up) and reallocates the texture cache's slots, nodes and block heap and empties them:
// all slots unused, every node but the two list ends on the free chain, one free run over the whole heap of
// 512-byte VRAM blocks, and the lookup counters zeroed. Also zeroes gSystemState.mUnk_2bc and mUnk_2b8, which
// FindOrLoadTex raises by the bytes it queues.
void ResetTexCache(void) {
    int i;
    if (gSystemState.mFlags & POOL_FLAG) {
        Free(&data_020c31dc.mTexSlots);
        Free(&data_020c31dc.mTexNodes);
        Free(&data_020c31dc.mTexBlocks);
    }
    data_020c31dc.mTexSlots = Alloc(0x2000);
    data_020c31dc.mTexNodes = Alloc(0x2be0);
    data_020c31dc.mTexNodes[0].mPrev = 1;
    data_020c31dc.mTexNodes[1].mNext = 0;
    data_020c31dc.mTexNodes[1].mPrev = 2;
    data_020c31dc.mTexEntryCount = 0;
    gSystemState.mUnk_2bc = 0;
    gSystemState.mUnk_2b8 = 0;
    for (i = data_020c31dc.mTexNodes[1].mPrev; i < 0x2bd; i++) {
        data_020c31dc.mTexNodes[i].mPrev = i + 1;
    }
    data_020c31dc.mTexNodes[i].mPrev = data_020c31dc.mTexNodes[1].mPrev;
    for (i = 0; i < 0x400; i++) {
        data_020c31dc.mTexSlots[i].mKey = -1;
    }
    data_020c31dc.mTexBlocks = Alloc((data_020c31dc.mTexBlockCount + 2) * 8);
    data_020c31dc.mTexBlocks[0].mUnk_0 = (gSystemState.mTexVramGroup.mSize16Kb << 14) / 512;
    data_020c31dc.mTexBlocks[0].mUnk_6 = 0xffff;
    data_020c31dc.mTexBlocks[0].mPrev = data_020c31dc.mTexBlocks[0].mUnk_0;
    data_020c31dc.mTexBlocks[0].mNext = data_020c31dc.mTexBlocks[0].mUnk_0 + 1;
    data_020c31dc.mTexBlocks[data_020c31dc.mTexBlocks[0].mUnk_0].mUnk_0 = 0xffff;
    data_020c31dc.mTexBlocks[data_020c31dc.mTexBlocks[0].mUnk_0].mNext = 0;
    data_020c31dc.mTexBlocks[data_020c31dc.mTexBlocks[0].mUnk_0].mPrev = 0xffff;
    data_020c31dc.mTexBlocks[data_020c31dc.mTexBlocks[0].mUnk_0 + 1].mUnk_0 = 0;
    data_020c31dc.mTexBlocks[data_020c31dc.mTexBlocks[0].mUnk_0 + 1].mNext = 0xffff;
    data_020c31dc.mTexBlocks[data_020c31dc.mTexBlocks[0].mUnk_0 + 1].mPrev = 0;
    data_020c31dc.mTexLookupCount = 0;
    data_020c31dc.mTexProbeCount = 0;
}

// The same for the palette cache (32-byte VRAM blocks, no lookup counters); zeroes gSystemState.mUnk_2c4 and
// mUnk_2c0.
void ResetPlttCache(void) {
    int i;
    if (gSystemState.mFlags & POOL_FLAG) {
        Free(&data_020c31dc.mPlttSlots);
        Free(&data_020c31dc.mPlttNodes);
        Free(&data_020c31dc.mPlttBlocks);
    }
    data_020c31dc.mPlttSlots = Alloc(0x800);
    data_020c31dc.mPlttNodes = Alloc(0xca0);
    data_020c31dc.mPlttNodes[0].mPrev = 1;
    data_020c31dc.mPlttNodes[1].mNext = 0;
    data_020c31dc.mPlttNodes[1].mPrev = 2;
    data_020c31dc.mPlttEntryCount = 0;
    gSystemState.mUnk_2c4 = 0;
    gSystemState.mUnk_2c0 = 0;
    for (i = data_020c31dc.mPlttNodes[1].mPrev; i < 0xc9; i++) {
        data_020c31dc.mPlttNodes[i].mPrev = i + 1;
    }
    data_020c31dc.mPlttNodes[i].mPrev = data_020c31dc.mPlttNodes[1].mPrev;
    for (i = 0; i < 0x100; i++) {
        data_020c31dc.mPlttSlots[i].mKey = -1;
    }
    data_020c31dc.mPlttBlocks = Alloc((data_020c31dc.mPlttBlockCount + 2) * 8);
    data_020c31dc.mPlttBlocks[0].mUnk_0 = (gSystemState.mPlttVramGroup.mSize16Kb << 14) / 32;
    data_020c31dc.mPlttBlocks[0].mUnk_6 = 0xffff;
    data_020c31dc.mPlttBlocks[0].mPrev = data_020c31dc.mPlttBlocks[0].mUnk_0;
    data_020c31dc.mPlttBlocks[0].mNext = data_020c31dc.mPlttBlocks[0].mUnk_0 + 1;
    data_020c31dc.mPlttBlocks[data_020c31dc.mPlttBlocks[0].mUnk_0].mUnk_0 = 0xffff;
    data_020c31dc.mPlttBlocks[data_020c31dc.mPlttBlocks[0].mUnk_0].mNext = 0;
    data_020c31dc.mPlttBlocks[data_020c31dc.mPlttBlocks[0].mUnk_0].mPrev = 0xffff;
    data_020c31dc.mPlttBlocks[data_020c31dc.mPlttBlocks[0].mUnk_0 + 1].mUnk_0 = 0;
    data_020c31dc.mPlttBlocks[data_020c31dc.mPlttBlocks[0].mUnk_0 + 1].mNext = 0xffff;
    data_020c31dc.mPlttBlocks[data_020c31dc.mPlttBlocks[0].mUnk_0 + 1].mPrev = 0;
}

// Undoes InitGxPool when it ran: clears POOL_FLAG, frees both caches' tables and the VRAM banks of uses 10 and 11,
// sets the camera's mUnk_2dc.mUnk_4a to 0xff and clears the bits InitGxPool set at 0x04000304 (not those at
// 0x04000060).
void ReleaseGxPool(void) {
    if (gSystemState.mFlags & POOL_FLAG) {
        gSystemState.mFlags &= ~POOL_FLAG;
        Free(&data_020c31dc.mTexSlots);
        Free(&data_020c31dc.mTexNodes);
        Free(&data_020c31dc.mTexBlocks);
        Free(&data_020c31dc.mPlttBlocks);
        Free(&data_020c31dc.mPlttSlots);
        Free(&data_020c31dc.mPlttNodes);
        FreeVramBanks(10);
        FreeVramBanks(11);
        gMapState.mCamera.mUnk_2dc.mUnk_4a = 0xff;
        *(vu16 *) 0x04000304 &= ~0xc;
    }
}
