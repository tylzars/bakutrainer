#include <nitro/mi.h>
#include <nitro/os.h>
#include <Main/Unknown/bg_image.h>
#include <Main/Unknown/functions.h>
#include <Main/Unknown/system_init.h>
#include <Main/Unknown/globals.h>

u32 func_0201db58(int layer) {
    u16 control;

    if (layer != 1 && layer != 2) {
        OS_Terminate();
    }
    if (layer & 1) {
        control = *(vu16 *) 0x0400000e;
    } else {
        control = *(vu16 *) 0x0400100e;
    }
    if ((control & 0x84) == 0x84) {
        return 0x8000;
    }
    return 0x100;
}

int func_0201db94(int layer) {
    if (layer != 1 && layer != 2) {
        OS_Terminate();
    }
    if (layer & 1) {
        return data_02050200[*(vu16 *) 0x0400000e >> 14];
    }
    return data_02050200[*(vu16 *) 0x0400100e >> 14];
}

int func_0201dbd4(int layer) {
    int n = func_0201db94(layer);

    if (func_0201db58(layer) == 0x8000) {
        n <<= 1;
    }
    return n;
}

#pragma thumb off
void func_0201dbf4(u16 *src, u32 dst, u32 width, u32 stride, u8 *lut) {
    u32 row = 16;
    int i;
    u16 *out;

    if (lut == NULL) {
        do {
            MIi_CpuCopy16(src, (void *) dst, width * 2);
            src += 0x200;
            dst += stride * 2;
        } while (--row != 0);
        return;
    }
    out = (u16 *) ((dst & 0xfff00000) + ((dst & 0xfffff) >> 1));
    do {
        for (i = 0; i < width / 2; i++) {
            u8 lo = lut[src[0] - 0x8000];
            u8 hi = lut[src[1] - 0x8000];
            src += 2;
            *out++ = lo | (hi << 8);
        }
        src += 0x200 - width;
        out += stride - width;
    } while (--row != 0);
}
#pragma thumb on

void func_0201dcc0(u16 *src, u32 layers, u32 row, u32 width, u32 total, u8 *lut) {
    u32 half;
    int n;

    switch (layers & 3) {
        case 1:
            n = func_0201db94(layers);
            func_0201dbf4(src, 0x06000000 + row * n * 2, width, n, lut);
            break;
        case 2:
            n = func_0201db94(layers);
            func_0201dbf4(src, 0x06200000 + row * n * 2, width, n, lut);
            break;
        case 3:
            half = total >> 1;
            if (row < half) {
                n = func_0201db94(1);
                func_0201dbf4(src, 0x06000000 + row * n * 2, width, n, lut);
            } else {
                n = func_0201db94(2);
                func_0201dbf4(src, 0x06200000 + (row - half) * n * 2, width, n, lut);
            }
            break;
    }
}

void func_0201dd74(u8 *src, u32 dst, u32 width, u32 rows) {
    int layer;
    int step;

    if (dst == 0x06000000) {
        layer = 1;
    } else {
        layer = 2;
    }
    if (func_0201db58(layer) == 0x8000) {
        width += width;
    }
    if (func_0204873c(width) == 1) {
        MIi_CpuCopy16(src, (void *) dst, width * rows);
        return;
    }
    step = func_0201dbd4(layer);
    while (rows != 0) {
        MIi_CpuCopy16(src, (void *) dst, width);
        src += width;
        dst += step;
        rows--;
    }
}

void func_0201dde0(int id, int target) {
    func_0201ddec(id, target, 0, 0);
}

void func_0201ddec(int id, int target, int x, int flags) {
    int found;
    void *buf;

    found = func_0202e988(id);
    if (found > 0) {
        func_0201de48((u8 *) found, target, x, flags);
    } else {
        buf = (void *) func_0202e86c(id);
        func_0201de48(buf, target, x, flags);
        func_0201c4f0(&buf);
    }
    if (target == 3) {
        data_020c18bc.mUnk_49c[1] = id;
        data_020c18bc.mUnk_49c[0] = data_020c18bc.mUnk_49c[1];
    } else if (target == 1) {
        data_020c18bc.mUnk_49c[0] = id;
    } else if (target == 2) {
        data_020c18bc.mUnk_49c[1] = id;
    }
}

void func_0201de48(u8 *data, int target, int x, int flags) {
    void *tmp = NULL;
    void *rgb = NULL;
    u32 w;
    u32 h;
    u32 h2;
    u32 magic;
    u32 mode;
    u32 size;
    u32 count;
    u32 n;
    u32 i;
    u32 j;
    int indexed = 0;
    u8 *pix;
    u8 *src;
    u8 *pal;
    u16 *out;
    u16 *top;
    u16 *bottom;
    u16 color;

    magic = *(u32 *) data;
    if (magic == 0xe0ffd8ff) {
        func_0201e518(data, &w, &h);
        data_020c18bc.mUnk_2cc = w;
        data_020c18bc.mUnk_2ce = h;
        data_020c18bc.mUnk_2d0 = 0x8000;
        h2 = h;
        if ((target & 3) == 3 && h >= 0x180) {
            h2 >>= 1;
        }
        if (x == 0) {
            if (!(flags & 2)) {
                func_0201f450(target, 1, w, h2);
            }
        } else {
            if (!(flags & 2)) {
                func_0201f450(target, 0, w, h2);
            }
            tmp = func_0201eee4(x, target);
        }
        if (target & 1) {
            func_0201e56c(data, target, tmp);
        }
        if (target & 2) {
            func_0201e56c(data, 2, tmp);
        }
        func_0201c4f0(&tmp);
    } else {
        if (magic == 0x6177686d) {
            w = (data[8] << 8) + data[9];
            h = (data[10] << 8) + data[11];
            h2 = h;
            count = (data[12] << 8) + data[13];
        } else {
            w = *(u16 *) (data + 0xc);
            h = *(u16 *) (data + 0xe);
            h2 = h;
            if (*(u16 *) (data + 0x10) != 0xf) {
                OS_Terminate();
            }
            count = 0;
            indexed = 1;
        }
        data_020c18bc.mUnk_2cc = w;
        data_020c18bc.mUnk_2ce = h;
        mode = target & 3;
        if (mode == 3 && h >= 0x180) {
            h2 = h >> 1;
        }
        if (count != 0) {
            if (!(flags & 1)) {
                data_020c18bc.mUnk_2d0 = 0x100;
                if (!(flags & 2)) {
                    func_0201f450(target, 0, w, h2);
                }
                pix = func_0201f5bc(data + 0x20, count, 0, target);
                DC_StoreRange(pix, w * h + 0x320);
                size = h2 * w;
            } else {
                pal = data + 0x20;
                src = pal + count * 3;
                data_020c18bc.mUnk_2d0 = 0x8000;
                if (!(flags & 2)) {
                    func_0201f450(target, 1, w, h2);
                }
                DC_StoreRange(data, w * h + 0x320);
                size = h2 * w * 2;
                rgb = func_0201c430(h * w * 2);
                out = rgb;
                for (n = h * w; n != 0; n--) {
                    u8 *c = pal + *src * 3;

                    color = (c[0] >> 3) | ((c[1] >> 3) << 5) | ((c[2] >> 3) << 10);
                    *out++ = color + 0x8000;
                    src++;
                }
                pix = rgb;
            }
        } else {
            top = (u16 *) (data + 0x12);
            pal = data + 0x20;
            data_020c18bc.mUnk_2d0 = 0x8000;
            if (!(flags & 2)) {
                func_0201f450(target, 1, w, h2);
            }
            DC_StoreRange(data, w * h * 3 + 0x20);
            pix = pal;
            if (indexed == 0) {
                out = (u16 *) pal;
                for (n = h * w; n != 0; n--) {
                    color = (pal[0] >> 3) | ((pal[1] >> 3) << 5) | ((pal[2] >> 3) << 10);
                    *out++ = color + 0x8000;
                    pal += 3;
                }
            } else {
                bottom = top + (h - 1) * w;
                pix = (u8 *) top;
                for (i = 0; i < h / 2; i++) {
                    for (j = 0; j < w; j++) {
                        u16 a = bottom[j];
                        u16 b = top[j];

                        top[j] = a + 0x8000;
                        bottom[j] = b + 0x8000;
                    }
                    top += w;
                    bottom -= w;
                }
            }
            size = h2 * w * 2;
        }
        switch (mode) {
            case 1:
                func_0201dd74(pix, 0x06000000, w, h);
                break;
            case 2:
                func_0201dd74(pix, 0x06200000, w, h);
                break;
            case 3:
                func_0201dd74(pix, 0x06000000, w, h2);
                if (h < 0x180) {
                    size = 0;
                }
                func_0201dd74(pix + size, 0x06200000, w, h2);
                break;
        }
    }
    func_0201c4f0(&rgb);
}
