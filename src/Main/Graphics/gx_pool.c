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

void func_02021fb0(int a, int b, int c, int d) {
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
    data_020c31dc.mUnk_62 = (gSystemState.mTexVramGroup.mSize16Kb << 14) / 512;
    data_020c31dc.mUnk_64 = (gSystemState.mPlttVramGroup.mSize16Kb << 14) / 32;
    *(vu32 *) 0x04000060 |= 9;
    data_020c31dc.mUnk_54 = a;
    data_020c31dc.mUnk_56 = b;
    v = FindCachedFile(a);
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
    data_020c31dc.mUnk_58 = (GetStoredFileSize(a) - hdr.mUnk_4) >> 2;
    v = FindCachedFile(b);
    if (v < 0) {
        SeekBank(4 - v);
        ReadBank(&v, 4);
    } else {
        v = ((Unk_TexHeader *) v)->mUnk_4;
    }
    data_020c31dc.mUnk_5a = (GetStoredFileSize(b) - v) >> 2;
    data_020c31dc.mUnk_46 = 0xff;
    func_02022120();
    func_0202224c();
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

void func_02022120(void) {
    int i;
    if (gSystemState.mFlags & POOL_FLAG) {
        Free(&data_020c31dc.mUnk_04);
        Free(&data_020c31dc.mUnk_00);
        Free(&data_020c31dc.mUnk_08);
    }
    data_020c31dc.mUnk_04 = Alloc(0x2000);
    data_020c31dc.mUnk_00 = Alloc(0x2be0);
    data_020c31dc.mUnk_00[0].mUnk_0a = 1;
    data_020c31dc.mUnk_00[1].mUnk_0c = 0;
    data_020c31dc.mUnk_00[1].mUnk_0a = 2;
    data_020c31dc.mUnk_60 = 0;
    gSystemState.mUnk_2bc = 0;
    gSystemState.mUnk_2b8 = 0;
    for (i = data_020c31dc.mUnk_00[1].mUnk_0a; i < 0x2bd; i++) {
        data_020c31dc.mUnk_00[i].mUnk_0a = i + 1;
    }
    data_020c31dc.mUnk_00[i].mUnk_0a = data_020c31dc.mUnk_00[1].mUnk_0a;
    for (i = 0; i < 0x400; i++) {
        data_020c31dc.mUnk_04[i].mUnk_00 = -1;
    }
    data_020c31dc.mUnk_08 = Alloc((data_020c31dc.mUnk_62 + 2) * 8);
    data_020c31dc.mUnk_08[0].mUnk_0 = (gSystemState.mTexVramGroup.mSize16Kb << 14) / 512;
    data_020c31dc.mUnk_08[0].mUnk_6 = 0xffff;
    data_020c31dc.mUnk_08[0].mUnk_2 = data_020c31dc.mUnk_08[0].mUnk_0;
    data_020c31dc.mUnk_08[0].mUnk_4 = data_020c31dc.mUnk_08[0].mUnk_0 + 1;
    data_020c31dc.mUnk_08[data_020c31dc.mUnk_08[0].mUnk_0].mUnk_0 = 0xffff;
    data_020c31dc.mUnk_08[data_020c31dc.mUnk_08[0].mUnk_0].mUnk_4 = 0;
    data_020c31dc.mUnk_08[data_020c31dc.mUnk_08[0].mUnk_0].mUnk_2 = 0xffff;
    data_020c31dc.mUnk_08[data_020c31dc.mUnk_08[0].mUnk_0 + 1].mUnk_0 = 0;
    data_020c31dc.mUnk_08[data_020c31dc.mUnk_08[0].mUnk_0 + 1].mUnk_4 = 0xffff;
    data_020c31dc.mUnk_08[data_020c31dc.mUnk_08[0].mUnk_0 + 1].mUnk_2 = 0;
    data_020c31dc.mUnk_4c = 0;
    data_020c31dc.mUnk_50 = 0;
}

void func_0202224c(void) {
    int i;
    if (gSystemState.mFlags & POOL_FLAG) {
        Free(&data_020c31dc.mUnk_10);
        Free(&data_020c31dc.mUnk_0c);
        Free(&data_020c31dc.mUnk_14);
    }
    data_020c31dc.mUnk_10 = Alloc(0x800);
    data_020c31dc.mUnk_0c = Alloc(0xca0);
    data_020c31dc.mUnk_0c[0].mUnk_0a = 1;
    data_020c31dc.mUnk_0c[1].mUnk_0c = 0;
    data_020c31dc.mUnk_0c[1].mUnk_0a = 2;
    data_020c31dc.mUnk_5e = 0;
    gSystemState.mUnk_2c4 = 0;
    gSystemState.mUnk_2c0 = 0;
    for (i = data_020c31dc.mUnk_0c[1].mUnk_0a; i < 0xc9; i++) {
        data_020c31dc.mUnk_0c[i].mUnk_0a = i + 1;
    }
    data_020c31dc.mUnk_0c[i].mUnk_0a = data_020c31dc.mUnk_0c[1].mUnk_0a;
    for (i = 0; i < 0x100; i++) {
        data_020c31dc.mUnk_10[i].mUnk_00 = -1;
    }
    data_020c31dc.mUnk_14 = Alloc((data_020c31dc.mUnk_64 + 2) * 8);
    data_020c31dc.mUnk_14[0].mUnk_0 = (gSystemState.mPlttVramGroup.mSize16Kb << 14) / 32;
    data_020c31dc.mUnk_14[0].mUnk_6 = 0xffff;
    data_020c31dc.mUnk_14[0].mUnk_2 = data_020c31dc.mUnk_14[0].mUnk_0;
    data_020c31dc.mUnk_14[0].mUnk_4 = data_020c31dc.mUnk_14[0].mUnk_0 + 1;
    data_020c31dc.mUnk_14[data_020c31dc.mUnk_14[0].mUnk_0].mUnk_0 = 0xffff;
    data_020c31dc.mUnk_14[data_020c31dc.mUnk_14[0].mUnk_0].mUnk_4 = 0;
    data_020c31dc.mUnk_14[data_020c31dc.mUnk_14[0].mUnk_0].mUnk_2 = 0xffff;
    data_020c31dc.mUnk_14[data_020c31dc.mUnk_14[0].mUnk_0 + 1].mUnk_0 = 0;
    data_020c31dc.mUnk_14[data_020c31dc.mUnk_14[0].mUnk_0 + 1].mUnk_4 = 0xffff;
    data_020c31dc.mUnk_14[data_020c31dc.mUnk_14[0].mUnk_0 + 1].mUnk_2 = 0;
}

void func_02022370(void) {
    if (gSystemState.mFlags & POOL_FLAG) {
        gSystemState.mFlags &= ~POOL_FLAG;
        Free(&data_020c31dc.mUnk_04);
        Free(&data_020c31dc.mUnk_00);
        Free(&data_020c31dc.mUnk_08);
        Free(&data_020c31dc.mUnk_14);
        Free(&data_020c31dc.mUnk_10);
        Free(&data_020c31dc.mUnk_0c);
        FreeVramBanks(10);
        FreeVramBanks(11);
        data_020c3e44.mUnk_2024.mUnk_2dc.mUnk_4a = 0xff;
        *(vu16 *) 0x04000304 &= ~0xc;
    }
}
