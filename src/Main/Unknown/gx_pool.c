#include <Main/Unknown/globals.h>
#include <Main/Unknown/functions.h>
#include <Main/Unknown/bg_image.h>
#include <Main/Unknown/system_init.h>
#include <Main/Unknown/file_stream.h>
#include <Main/Unknown/camera.h>
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
    if (data_020c18bc.mUnk_46c & POOL_FLAG) {
        return;
    }
    *(vu16 *) 0x04000304 |= 0xc;
    G3X_Init();
    G3X_InitMtxStack();
    func_0201ebf4(10, c, -1);
    func_0201ebf4(11, d, -1);
    data_020c321c.mUnk_22 = (data_020c18bc.mUnk_070.mUnk_2c << 14) / 512;
    data_020c321c.mUnk_24 = (data_020c18bc.mUnk_040.mUnk_2c << 14) / 32;
    *(vu32 *) 0x04000060 |= 9;
    data_020c321c.mUnk_14 = a;
    data_020c321c.mUnk_16 = b;
    v = func_0202e988(a);
    if (v < 0) {
        func_0201d9a8(-v);
        func_0201d9d4(&hdr, 8);
    } else {
        hdr.mUnk_0 = ((Unk_TexHeader *) v)->mUnk_0;
        hdr.mUnk_4 = ((Unk_TexHeader *) v)->mUnk_4;
    }
    if (hdr.mUnk_0 != TEX_MAGIC) {
        OS_Terminate();
    }
    data_020c321c.mUnk_18 = (func_0202e760(a) - hdr.mUnk_4) >> 2;
    v = func_0202e988(b);
    if (v < 0) {
        func_0201d9a8(4 - v);
        func_0201d9d4(&v, 4);
    } else {
        v = ((Unk_TexHeader *) v)->mUnk_4;
    }
    data_020c321c.mUnk_1a = (func_0202e760(b) - v) >> 2;
    data_020c321c.mUnk_06 = 0xff;
    func_02022120();
    func_0202224c();
    data_020c18bc.mUnk_46c |= POOL_FLAG;
}

void func_020220c8(int a) {
    u16 *p = func_0202ded8(a);
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
    int j;
    int i;
    if (data_020c18bc.mUnk_46c & POOL_FLAG) {
        func_0201c4f0(&data_020c31dc.mUnk_04);
        func_0201c4f0(&data_020c31dc.mUnk_00);
        func_0201c4f0(&data_020c31dc.mUnk_08);
    }
    data_020c31dc.mUnk_04 = func_0201c430(0x2000);
    data_020c31dc.mUnk_00 = func_0201c430(0x2be0);
    data_020c31dc.mUnk_00[0].mUnk_0a = 1;
    data_020c31dc.mUnk_00[1].mUnk_0c = 0;
    data_020c31dc.mUnk_00[1].mUnk_0a = 2;
    data_020c321c.mUnk_20 = 0;
    data_020c18bc.mUnk_2bc = 0;
    data_020c18bc.mUnk_2b8 = 0;
    for (i = data_020c31dc.mUnk_00[1].mUnk_0a; i < 0x2bd; i++) {
        data_020c31dc.mUnk_00[i].mUnk_0a = i + 1;
    }
    data_020c31dc.mUnk_00[i].mUnk_0a = data_020c31dc.mUnk_00[1].mUnk_0a;
    for (j = 0; j < 0x400; j++) {
        data_020c31dc.mUnk_04[j].mUnk_00 = -1;
    }
    data_020c31dc.mUnk_08 = func_0201c430((data_020c321c.mUnk_22 + 2) * 8);
    data_020c31dc.mUnk_08[0].mUnk_0 = (data_020c18bc.mUnk_070.mUnk_2c << 14) / 512;
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
    int j;
    int i;
    if (data_020c18bc.mUnk_46c & POOL_FLAG) {
        func_0201c4f0(&data_020c31dc.mUnk_10);
        func_0201c4f0(&data_020c31dc.mUnk_0c);
        func_0201c4f0(&data_020c31dc.mUnk_14);
    }
    data_020c31dc.mUnk_10 = func_0201c430(0x800);
    data_020c31dc.mUnk_0c = func_0201c430(0xca0);
    data_020c31dc.mUnk_0c[0].mUnk_0a = 1;
    data_020c31dc.mUnk_0c[1].mUnk_0c = 0;
    data_020c31dc.mUnk_0c[1].mUnk_0a = 2;
    data_020c321c.mUnk_1e = 0;
    data_020c18bc.mUnk_2c4 = 0;
    data_020c18bc.mUnk_2c0 = 0;
    for (i = data_020c31dc.mUnk_0c[1].mUnk_0a; i < 0xc9; i++) {
        data_020c31dc.mUnk_0c[i].mUnk_0a = i + 1;
    }
    data_020c31dc.mUnk_0c[i].mUnk_0a = data_020c31dc.mUnk_0c[1].mUnk_0a;
    for (j = 0; j < 0x100; j++) {
        data_020c31dc.mUnk_10[j].mUnk_00 = -1;
    }
    data_020c31dc.mUnk_14 = func_0201c430((data_020c321c.mUnk_24 + 2) * 8);
    data_020c31dc.mUnk_14[0].mUnk_0 = (data_020c18bc.mUnk_040.mUnk_2c << 14) / 32;
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
    if (data_020c18bc.mUnk_46c & POOL_FLAG) {
        data_020c18bc.mUnk_46c &= ~POOL_FLAG;
        func_0201c4f0(&data_020c31dc.mUnk_04);
        func_0201c4f0(&data_020c31dc.mUnk_00);
        func_0201c4f0(&data_020c31dc.mUnk_08);
        func_0201c4f0(&data_020c31dc.mUnk_14);
        func_0201c4f0(&data_020c31dc.mUnk_10);
        func_0201c4f0(&data_020c31dc.mUnk_0c);
        func_0201eb74(10);
        func_0201eb74(11);
        data_020c5e44.mUnk_300.mUnk_4a = 0xff;
        *(vu16 *) 0x04000304 &= ~0xc;
    }
}
