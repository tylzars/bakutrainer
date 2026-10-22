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
    u32 mMagic;
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
    gGxState.mTexBlockCount = (gSystemState.mTexVramGroup.mSize16Kb << 14) / 512;
    gGxState.mPlttBlockCount = (gSystemState.mPlttVramGroup.mSize16Kb << 14) / 32;
    *(vu32 *) 0x04000060 |= 9;
    gGxState.mTexFile = texFile;
    gGxState.mPlttFile = plttFile;
    v = FindCachedFile(texFile);
    if (v < 0) {
        SeekBank(-v);
        ReadBank(&hdr, 8);
    } else {
        hdr.mMagic = ((Unk_TexHeader *) v)->mMagic;
        hdr.mUnk_4 = ((Unk_TexHeader *) v)->mUnk_4;
    }
    if (hdr.mMagic != TEX_MAGIC) {
        OS_Terminate();
    }
    gGxState.mTexFileKeyCount = (GetStoredFileSize(texFile) - hdr.mUnk_4) >> 2;
    v = FindCachedFile(plttFile);
    if (v < 0) {
        SeekBank(4 - v);
        ReadBank(&v, 4);
    } else {
        v = ((Unk_TexHeader *) v)->mUnk_4;
    }
    gGxState.mPlttFileKeyCount = (GetStoredFileSize(plttFile) - v) >> 2;
    gGxState.mStopNodeIndex = 0xff;
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
        Free(&gGxState.mTexSlots);
        Free(&gGxState.mTexNodes);
        Free(&gGxState.mTexBlocks);
    }
    gGxState.mTexSlots = Alloc(0x2000);
    gGxState.mTexNodes = Alloc(0x2be0);
    gGxState.mTexNodes[0].mPrev = 1;
    gGxState.mTexNodes[1].mNext = 0;
    gGxState.mTexNodes[1].mPrev = 2;
    gGxState.mTexEntryCount = 0;
    gSystemState.mUnk_2bc = 0;
    gSystemState.mUnk_2b8 = 0;
    for (i = gGxState.mTexNodes[1].mPrev; i < 0x2bd; i++) {
        gGxState.mTexNodes[i].mPrev = i + 1;
    }
    gGxState.mTexNodes[i].mPrev = gGxState.mTexNodes[1].mPrev;
    for (i = 0; i < 0x400; i++) {
        gGxState.mTexSlots[i].mKey = -1;
    }
    gGxState.mTexBlocks = Alloc((gGxState.mTexBlockCount + 2) * 8);
    gGxState.mTexBlocks[0].mUnk_0 = (gSystemState.mTexVramGroup.mSize16Kb << 14) / 512;
    gGxState.mTexBlocks[0].mUnk_6 = 0xffff;
    gGxState.mTexBlocks[0].mPrev = gGxState.mTexBlocks[0].mUnk_0;
    gGxState.mTexBlocks[0].mNext = gGxState.mTexBlocks[0].mUnk_0 + 1;
    gGxState.mTexBlocks[gGxState.mTexBlocks[0].mUnk_0].mUnk_0 = 0xffff;
    gGxState.mTexBlocks[gGxState.mTexBlocks[0].mUnk_0].mNext = 0;
    gGxState.mTexBlocks[gGxState.mTexBlocks[0].mUnk_0].mPrev = 0xffff;
    gGxState.mTexBlocks[gGxState.mTexBlocks[0].mUnk_0 + 1].mUnk_0 = 0;
    gGxState.mTexBlocks[gGxState.mTexBlocks[0].mUnk_0 + 1].mNext = 0xffff;
    gGxState.mTexBlocks[gGxState.mTexBlocks[0].mUnk_0 + 1].mPrev = 0;
    gGxState.mTexLookupCount = 0;
    gGxState.mTexProbeCount = 0;
}

// The same for the palette cache (32-byte VRAM blocks, no lookup counters); zeroes gSystemState.mUnk_2c4 and
// mUnk_2c0.
void ResetPlttCache(void) {
    int i;
    if (gSystemState.mFlags & POOL_FLAG) {
        Free(&gGxState.mPlttSlots);
        Free(&gGxState.mPlttNodes);
        Free(&gGxState.mPlttBlocks);
    }
    gGxState.mPlttSlots = Alloc(0x800);
    gGxState.mPlttNodes = Alloc(0xca0);
    gGxState.mPlttNodes[0].mPrev = 1;
    gGxState.mPlttNodes[1].mNext = 0;
    gGxState.mPlttNodes[1].mPrev = 2;
    gGxState.mPlttEntryCount = 0;
    gSystemState.mUnk_2c4 = 0;
    gSystemState.mUnk_2c0 = 0;
    for (i = gGxState.mPlttNodes[1].mPrev; i < 0xc9; i++) {
        gGxState.mPlttNodes[i].mPrev = i + 1;
    }
    gGxState.mPlttNodes[i].mPrev = gGxState.mPlttNodes[1].mPrev;
    for (i = 0; i < 0x100; i++) {
        gGxState.mPlttSlots[i].mKey = -1;
    }
    gGxState.mPlttBlocks = Alloc((gGxState.mPlttBlockCount + 2) * 8);
    gGxState.mPlttBlocks[0].mUnk_0 = (gSystemState.mPlttVramGroup.mSize16Kb << 14) / 32;
    gGxState.mPlttBlocks[0].mUnk_6 = 0xffff;
    gGxState.mPlttBlocks[0].mPrev = gGxState.mPlttBlocks[0].mUnk_0;
    gGxState.mPlttBlocks[0].mNext = gGxState.mPlttBlocks[0].mUnk_0 + 1;
    gGxState.mPlttBlocks[gGxState.mPlttBlocks[0].mUnk_0].mUnk_0 = 0xffff;
    gGxState.mPlttBlocks[gGxState.mPlttBlocks[0].mUnk_0].mNext = 0;
    gGxState.mPlttBlocks[gGxState.mPlttBlocks[0].mUnk_0].mPrev = 0xffff;
    gGxState.mPlttBlocks[gGxState.mPlttBlocks[0].mUnk_0 + 1].mUnk_0 = 0;
    gGxState.mPlttBlocks[gGxState.mPlttBlocks[0].mUnk_0 + 1].mNext = 0xffff;
    gGxState.mPlttBlocks[gGxState.mPlttBlocks[0].mUnk_0 + 1].mPrev = 0;
}

// Undoes InitGxPool when it ran: clears POOL_FLAG, frees both caches' tables and the VRAM banks of uses 10 and 11,
// sets the camera's mViewKind to VIEW_KIND_NONE and clears the bits InitGxPool set at 0x04000304 (not those at
// 0x04000060).
void ReleaseGxPool(void) {
    if (gSystemState.mFlags & POOL_FLAG) {
        gSystemState.mFlags &= ~POOL_FLAG;
        Free(&gGxState.mTexSlots);
        Free(&gGxState.mTexNodes);
        Free(&gGxState.mTexBlocks);
        Free(&gGxState.mPlttBlocks);
        Free(&gGxState.mPlttSlots);
        Free(&gGxState.mPlttNodes);
        FreeVramBanks(10);
        FreeVramBanks(11);
        gMapState.mCamera.mUnk_2dc.mViewKind = VIEW_KIND_NONE;
        *(vu16 *) 0x04000304 &= ~0xc;
    }
}
