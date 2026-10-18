#include <nitro/mi.h>
#include <nitro/os.h>
#include <Main/Unknown/bg_image.h>
#include <Main/Unknown/functions.h>
#include <Main/Unknown/globals.h>
#include <Main/Unknown/screen_fade.h>
#include <Main/Unknown/system_init.h>

void func_0201f450(int a, int b, u32 c, u32 d) {
    u32 cls = 0;
    u32 size = 0x80;
    u32 rows;
    if (c > 0x80 || d > 0x80) {
        cls = 1;
        size = 0x100;
        if (c > 0x100 || d > 0x100) {
            cls = 2;
            size = 0x200;
            if (c > 0x200 || d > 0x100) {
                cls = 3;
            }
        }
    }
    rows = (size * d) >> 10;
    if (a & 1) {
        *(vu32 *) 0x04000000 = (data_020c2bfc[0x13] << 31) + (data_020c2bfc[0xf] << 20) + 0x11833;
        data_020c18bc.mUnk_474[0] &= ~4;
        data_020c18bc.mUnk_2d4 = 0;
        data_020c18bc.mUnk_2d8 = 0;
        data_020c18bc.mUnk_2e4 = 0x100;
        data_020c18bc.mUnk_2e6 = 0x100;
        *(vu16 *) 0x04000030 = 0x100;
        *(vu16 *) 0x04000036 = 0x100;
        *(vu16 *) 0x04000032 = 0;
        *(vu16 *) 0x04000034 = 0;
        *(vu32 *) 0x04000038 = 0;
        *(vu32 *) 0x0400003c = 0;
        switch (b) {
            case 0:
                func_0201ebf4(2, rows, -1);
                *(vu16 *) 0x0400000e = (cls << 14) + 0x80;
                break;
            case 1:
                func_0201ebf4(2, rows * 2, -1);
                *(vu16 *) 0x0400000e = (cls << 14) + 0x84;
                break;
            default:
                OS_Terminate();
                break;
        }
        MIi_CpuClear16(0, (void *) 0x05000000, 0x200);
    }
    if (a & 2) {
        *(vu32 *) 0x04001000 = (data_020c317c.mUnk_00[0x13] << 31) + (data_020c317c.mUnk_00[0xf] << 20) + 0x11813;
        data_020c18bc.mUnk_474[0] &= ~8;
        data_020c18bc.mUnk_2dc = 0;
        data_020c18bc.mUnk_2e0 = 0;
        *(vu16 *) 0x04001030 = 0x100;
        *(vu16 *) 0x04001036 = 0x100;
        *(vu16 *) 0x04001032 = 0;
        *(vu16 *) 0x04001034 = 0;
        *(vu32 *) 0x04001038 = 0;
        *(vu32 *) 0x0400103c = 0;
        switch (b) {
            case 0:
                func_0201ebf4(6, rows, -1);
                *(vu16 *) 0x0400100e = (cls << 14) + 0x80;
                break;
            case 1:
                func_0201ebf4(6, rows * 2, -1);
                *(vu16 *) 0x0400100e = (cls << 14) + 0x84;
                break;
        }
        MIi_CpuClear16(0, (void *) 0x05000400, 0x200);
    }
}

u8 *func_0201f5bc(u8 *palette, u32 count, int a, int target) {
    u8 *start = palette;
    u32 i;
    u16 *dst;
    u8 r;
    u8 g;
    u8 b;

    if (target & 1) {
        dst = (u16 *) 0x05000000 + a;
        for (i = 0; i < count; i++) {
            r = palette[0] >> 3;
            g = palette[1] >> 3;
            b = palette[2] >> 3;
            *dst = r + (g << 5) + (b << 10);
            dst++;
            palette += 3;
        }
    }
    if (target & 2) {
        palette = start;
        dst = (u16 *) 0x05000400 + a;
        for (i = 0; i < count; i++) {
            r = palette[0] >> 3;
            g = palette[1] >> 3;
            b = palette[2] >> 3;
            *dst = r + (g << 5) + (b << 10);
            dst++;
            palette += 3;
        }
    }
    return palette;
}

void func_0201f648(void) {
    vu16 *regs[2] = {(vu16 *) 0x0400006c, (vu16 *) 0x0400106c};
    int i;

    for (i = 0; i < 2; i++) {
        if (data_020c18bc.mUnk_490[i] == 1) {
            u32 level = *regs[i] & 0x1f;
            if (level != 0) {
                *regs[i] = 0x8000 | (level - 1);
            } else {
                *regs[i] = 0;
                data_020c18bc.mUnk_490[i] = 3;
            }
        } else if (data_020c18bc.mUnk_490[i] == 2) {
            u32 level = *regs[i] & 0x1f;
            if (level < 0x10) {
                *regs[i] = 0x8000 | (level + 1);
            } else {
                *regs[i] = 0x8011;
                data_020c18bc.mUnk_490[i] = 0;
            }
        }
    }
}

void func_0201f6d0(int a, int b, int c, int d, u16 e) {
    int phase = 0;
    int touched = 0;
    u32 frames = 0;
    u32 step = 0;

    func_0201f880(3);
    func_0201dde0(d, 3);
    func_0202c1c4();
    func_0202bdd8(b, c, a, 0, 1);
    func_02020854(1);
    while (func_0201f910(&frames, &touched, 0x3c, 0x78, 3) == 0) {
        if (phase == 0) {
            if ((data_020c18bc.mUnk_45c & 3) == 0) {
                step++;
                if (step == 10) {
                    phase = 1;
                }
            }
        } else {
            step++;
            if (step >= 0x19) {
                step = 10;
            }
        }
        func_0201bb30();
        func_0202cf08(e + step, 0x2c, 0x5d, 0, 1);
    }
    data_020c18bc.mUnk_46c |= 0x40000;
    func_0201f858(3);
    data_020c18bc.mUnk_46c &= ~0x40000;
}

int func_0201f794(int a, int b) {
    if ((a & 1) && data_020c18bc.mUnk_490[0] != b) {
        return 0;
    }
    if ((a & 2) && data_020c18bc.mUnk_490[1] != b) {
        return 0;
    }
    return 1;
}

int func_0201f7c0(int a, int b) {
    if (a & b) {
        return 1;
    }
    return 0;
}

void func_0201f7cc(int a) {
    if (func_0201f7c0(a, 1) == 1) {
        data_020c18bc.mUnk_490[0] = 2;
    }
    if (func_0201f7c0(a, 2) == 1) {
        data_020c18bc.mUnk_490[1] = 2;
    }
}

void func_0201f7f8(int a) {
    if (func_0201f7c0(a, 1) == 1) {
        if (data_020c18bc.mUnk_490[0] != 3) {
            data_020c18bc.mUnk_490[0] = 1;
        }
    }
    if (func_0201f7c0(a, 2) == 1) {
        if (data_020c18bc.mUnk_490[1] != 3) {
            data_020c18bc.mUnk_490[1] = 1;
        }
    }
}

void func_0201f830(int a) {
    func_0201f7f8(a);
    while (func_0201f794(a, 3) != 1) {
        func_0201bb30();
    }
}

void func_0201f858(int a) {
    while (func_0201f794(a, 0) != 1) {
        func_0201f7cc(a);
        func_0201bb30();
    }
}

void func_0201f880(int a) {
    if (func_0201f7c0(a, 1) == 1) {
        data_020c18bc.mUnk_490[0] = 0;
        *(vu16 *) 0x0400006c = 0x8011;
        func_0202ae34(1);
    }
    if (func_0201f7c0(a, 2) == 1) {
        data_020c18bc.mUnk_490[1] = 0;
        *(vu16 *) 0x0400106c = 0x8011;
        func_0202ae34(2);
    }
}

void func_0201f8d0(int a) {
    if (func_0201f7c0(a, 1) == 1) {
        data_020c18bc.mUnk_490[0] = 3;
        *(vu16 *) 0x0400006c = 0;
    }
    if (func_0201f7c0(a, 2) == 1) {
        data_020c18bc.mUnk_490[1] = 3;
        *(vu16 *) 0x0400106c = 0;
    }
}

int func_0201f910(u32 *frames, int *touched, u32 minFrames, u32 maxFrames, int layers) {
    if ((data_020c18bc.mUnk_10 & 9) || func_02020e3c(0, 0) == 1) {
        *touched = 1;
    }
    if (!(data_020c18bc.mUnk_10 & 9) && func_02020e3c(0, 0) == 0 && *frames > minFrames && *touched == 1) {
        return 1;
    }
    if (*frames >= maxFrames) {
        return 1;
    }
    if (func_0201f794(layers, 3) == 1) {
        (*frames)++;
    }
    return 0;
}

void func_0201f980(int a, int b, int c, int d, int e) {
    u32 frames = 0;
    int touched = 0;

    func_0201f858(e);
    func_0201dde0(a, e);
    func_0201f7f8(e);
    while (func_0201f910(&frames, &touched, b, c, e) == 0) {
        func_0201bb30();
    }
    if (d != 0) {
        if (d == 2) {
            func_0201f858(e);
        }
    } else {
        func_0201f880(e);
    }
    if (e & 1) {
        func_0201eb74(2);
    }
    if (e & 2) {
        func_0201eb74(6);
    }
}

#pragma thumb off
// Moves an entry to the end of the list.
void func_0201fa08(IndexNode16 *list, u32 index) {
    IndexNode16 *node = &list[index];
    list[node->mNext].mPrev = node->mPrev;
    list[node->mPrev].mNext = node->mNext;
    node->mNext = 0;
    node->mPrev = list[0].mPrev;
    list[0].mPrev = index;
    list[node->mPrev].mNext = index;
}
#pragma thumb on

#pragma thumb off
// Moves an entry to the end of the list.
void func_0201fa5c(IndexNode *list, u32 index) {
    IndexNode *node = &list[index];
    list[node->mNext].mPrev = node->mPrev;
    list[node->mPrev].mNext = node->mNext;
    node->mNext = 0;
    node->mPrev = list[0].mPrev;
    list[0].mPrev = index;
    list[node->mPrev].mNext = index;
}
#pragma thumb on
