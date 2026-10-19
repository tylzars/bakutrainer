#include <Main/Graphics/text_draw.h>
#include <Main/Graphics/texture_blocks.h>
#include <Main/System/file_loader.h>
#include <Main/System/system_init.h>
#include <nitro/os.h>
#include <nitro/fx.h>

// Declared without a prototype: this file passes func_020243c0's angle as an int, unconverted (model.h's u16
// prototype would truncate it).
void func_020243c0();

u16 func_02021498(const u8 **text) {
    u32 first;
    u32 lead;
    u32 value;
    u32 count;
    u32 num;
    const u8 *p;
    u16 acc;
    count = 0;
    if (data_020c18bc.mUnk_318 == 1 && **text == '$') {
        p = *text;
        num = 0;
        for (;;) {
            p++;
            if (*p < '0' || *p > '9') {
                break;
            }
            num = *p + num * 10 - '0';
        }
        first = data_020c18bc.mUnk_304(num);
        if (first == 0) {
            *text = p;
        } else if ((first & 0x80) == 0) {
            return first;
        } else {
            u32 acc2;
            acc2 = count = 0;
            if (first & 0x40) {
                do {
                    acc2 <<= 6;
                    count++;
                    value = data_020c18bc.mUnk_304(num);
                    if (value == 0) {
                        break;
                    }
                    acc2 += value & 0x3f;
                } while (first & (0x40 >> count));
            }
            return acc2 + ((first & (0x3f >> count)) << (6 * count));
        }
    }
    data_020c18bc.mUnk_316 = acc = 0;
    lead = **text;
    if ((lead & 0x80) == 0) {
        return *(*text)++;
    }
    if (lead & 0x40) {
        do {
            acc <<= 6;
            (*text)++;
            count++;
            acc += **text & 0x3f;
        } while (lead & (0x40 >> count));
    }
    acc += (lead & (0x3f >> count)) << (6 * count);
    (*text)++;
    return acc;
}

void func_0202158c(u8 **out, u32 code) {
    if (code >= 0x1000) {
        *(*out)++ = 0xe0 + (code >> 12);
        *(*out)++ = 0x80 + ((code >> 6) & 0x3f);
        *(*out)++ = 0x80 + (code & 0x3f);
    } else if (code >= 0x80) {
        *(*out)++ = 0xc0 + (code >> 6);
        *(*out)++ = 0x80 + (code & 0x3f);
    } else {
        *(*out)++ = code;
    }
}

u32 func_020215f0(u32 code) {
    if (code == 0x20 || code == 0xa0) {
        code = 0x20;
    }
    return code;
}

void func_020215fc(int x, int y, const void *font, const char *text, int align, int width, int height, int mode) {
    const void *f = font;
    func_02021624(x, y, (const FontRange *const *) &f, 0, (const u8 *) text, 0, align, width, height, mode);
}

int func_02021624(int x, int y, const FontRange *const *fonts, int fontIndex, const u8 *text, int angle, u32 flags,
                  u32 width, int height, int mode) {
    u8 widths[0x400];
    u16 chars[0x400];
    u16 glyphs[0x400];
    s32 rect[4];
    int pal;
    int odd;
    u32 breakW;
    int lineX;
    unsigned long i;
    int spaces;
    int startY;
    u32 lineNo;
    int dirMode;
    int glyphHigh;
    const FontRange *font;
    int attr;
    u32 punct;
    u32 align;
    u32 nodraw;
    u32 window;
    const s16 *cosp;
    const s16 *sinp;
    u32 hilite;
    u32 alt;
    u32 rot;
    u32 limit;
    u32 bounds;
    u32 single;
    int skipped;
    u32 drawn;
    u32 lineH;
    u32 maxW;
    int result;
    u32 lineW;
    int n;
    u32 k;
    u16 *q;
    const FontRange *fr;
    BOOL ok;
    u32 c;

    result = 0;
    i = 0;
    spaces = 0;
    startY = y;
    lineNo = 0;
    dirMode = 0;
    glyphHigh = 0;

    ok = 1;
    rot = flags & TEXT_ROTATE;
    if (rot) {
        if ((data_020c18bc.mUnk_46c & 0x80) == 0) {
            ok = 0;
        }
    }
    if (ok == 0) {
        OS_Terminate();
    }
    if (mode != 1 && mode != 2) {
        OS_Terminate();
    }
    if (mode != 1 && rot) {
        OS_Terminate();
    }
    bounds = flags & TEXT_BOUNDS;
    if (bounds) {
        data_020c18bc.mUnk_2fe = 0x7fff;
        data_020c18bc.mUnk_302 = -0x7fff;
        data_020c18bc.mUnk_2fc = 0x7fff;
        data_020c18bc.mUnk_300 = -0x7fff;
    }
    if ((flags & (TEXT_VERTICAL | TEXT_VERTICAL_FLIP)) == (TEXT_VERTICAL | TEXT_VERTICAL_FLIP)) {
        OS_Terminate();
    }
    if (flags & TEXT_VERTICAL) {
        dirMode = 1;
    } else if (flags & TEXT_VERTICAL_FLIP) {
        dirMode = 2;
        glyphHigh = 3 << 16;
    }
    if (dirMode != 0) {
        if (dirMode == 1) {
            int t = x;
            x = y;
            y = 0x100 - t;
        } else {
            int t = x;
            x = y;
            y = t;
        }
    }
    data_020c18bc.mUnk_318 = (flags & TEXT_ESCAPES) ? 1 : 0;

    k = 0;
    do {
        chars[k++] = func_02021498(&text);
    } while (chars[k - 1] != 0);
    data_020c18bc.mUnk_317_0 = 0;

    font = fonts[fontIndex];
    func_0202d828(font->mGlyph, 0, rect);
    lineH = dirMode ? rect[3] : rect[2];

    n = 0;
    for (;;) {
        k = 0;
        q = &chars[n];
        if (*q == 0) {
            break;
        }
        if (data_020c18bc.mUnk_311[0] == *q) {
            if (q[1] >= '0' && q[1] <= '9') {
                k = 1;
            }
            if (k == 0) {
                OS_Terminate();
            }
            font = fonts[q[1] - '0'];
            func_0202d828(font->mGlyph, 0, rect);
            if (dirMode) {
                if (lineH < rect[3]) {
                    lineH = rect[3];
                }
            } else {
                if (lineH < rect[2]) {
                    lineH = rect[2];
                }
            }
            k = 2;
        }
        if (k != 0) {
            int src = n + k;
            int dst = n;
            while (chars[src] != 0) {
                chars[dst++] = chars[src++];
            }
            chars[dst] = 0;
            continue;
        }
        fr = font;
        if (func_020215f0(*q) != 0x20) {
            c = *q;
            for (;;) {
                if (c >= fr->mFirst && c <= fr->mLast) {
                    glyphs[n] = fr->mGlyph + (*q - fr->mFirst);
                    break;
                }
                if (fr->mLast == 0 || fr->mFirst > c) {
                    if (data_020c18bc.mUnk_30e == 0x20) {
                        chars[n] = 0x20;
                        glyphs[n] = fr->mGlyph + (chars[n] - fr->mFirst);
                    } else {
                        *q = data_020c18bc.mUnk_30e;
                        n--;
                    }
                    break;
                }
                fr++;
            }
        }
        n++;
    }

    single = flags & TEXT_SINGLE_LINE;
    if (single) {
        maxW = 0x100;
    } else {
        maxW = 0x1000;
    }
    limit = flags & TEXT_LIMIT;
    while (chars[k] != 0 && maxW > result) {
        if (func_020215f0(chars[k]) > 0x20) {
            func_0202d828(glyphs[k], 0, rect);
            if (dirMode) {
                widths[k] = rect[3];
            } else {
                widths[k] = rect[2];
            }
        } else {
            spaces++;
            widths[k] = lineH;
        }
        if (!limit || k < data_020c18bc.mUnk_30c + spaces) {
            result += widths[k];
        }
        k++;
    }

    skipped = 0;
    if (chars[0] != 0) {
        punct = flags & TEXT_NO_BREAK_PUNCT;
        align = flags & TEXT_ALIGN_MASK;
        nodraw = flags & TEXT_NO_DRAW;
        window = flags & TEXT_LINE_WINDOW;
        cosp = &FX_SinCosTable_[(angle >> 4) * 2 + 1];
        sinp = &FX_SinCosTable_[(angle >> 4) * 2];
        hilite = flags & TEXT_HILITE;
        alt = flags & TEXT_ALT_DRAW;
        do {
            u8 nl;
            u32 cnt;
            int brk;
            lineW = 0;
            breakW = 0;
            cnt = 0;
            nl = data_020c18bc.mUnk_310;
            while ((c = chars[i + cnt]) > 1 && lineW < width) {
                brk = 0;
                if (c == 0x20) {
                    if (punct) {
                        u32 next = chars[i + cnt + 1];
                        if (next != '?' && next != '!' && next != ':' && next != ';') {
                            brk = 1;
                        }
                    } else {
                        brk = 1;
                    }
                }
                if (nl == c) {
                    break;
                }
                if (brk == 1 || (cnt != 0 && chars[i + cnt - 1] == '-')) {
                    breakW = lineW;
                    lineW += widths[i + cnt];
                } else {
                    lineW += widths[i + cnt];
                }
                cnt++;
            }
            if (lineW < width) {
                breakW = lineW;
            }
            switch (align) {
                case 1:
                    lineX = x - breakW;
                    break;
                case 2:
                    lineX = x - (breakW >> 1);
                    break;
                default:
                    lineX = x;
                    break;
            }
            if (breakW == 0) {
                breakW = width;
            }
            drawn = 0;
            while (drawn < breakW) {
                int px;
                int py;
                if (chars[i] == 0) {
                    break;
                }
                if (func_020215f0(chars[i]) != 0x20 && data_020c18bc.mUnk_310 != chars[i] && !nodraw) {
                    if (!(window && (lineNo < data_020c18bc.mUnk_313 ||
                                     lineNo >= data_020c18bc.mUnk_313 + data_020c18bc.mUnk_314))) {
                        if (dirMode != 0) {
                            if (dirMode == 1) {
                                px = y;
                                py = lineX;
                            } else {
                                px = y;
                                py = 0xc0 - lineX;
                            }
                        } else {
                            px = lineX;
                            py = y;
                        }
                        pal = data_020c18bc.mUnk_308;
                        odd = i & 1;
                        if (!(limit && i >= data_020c18bc.mUnk_30c + skipped)) {
                            if (rot) {
                                int dx = px - x;
                                int dy = py - y;
                                int cs = *cosp;
                                int ns = -*sinp;
                                attr = data_020c18bc.mUnk_30a[1];
                                px = x + ((dx * cs - dy * ns) >> 12);
                                py = y + ((dx * ns + dy * cs) >> 12);
                                if (hilite) {
                                    attr = 0x1f;
                                }
                                func_020243c0(glyphHigh + glyphs[i], px, py, -data_020c18bc.mUnk_30a[0] << 6, angle,
                                              0x1000, 0x1000, attr);
                            } else if (alt) {
                                if (dirMode != 0) {
                                    OS_Terminate();
                                }
                                func_0202cf08((glyphHigh + 0x100000) | glyphs[i], px, py, 0, mode);
                            } else {
                                func_0202cf08(glyphHigh + (glyphs[i] + (hilite ? 0x40000 : 0)), px, py, pal + odd,
                                              mode);
                            }
                            if (bounds) {
                                func_0202d828(glyphHigh + glyphs[i], 0, rect);
                                if (px + rect[0] < data_020c18bc.mUnk_2fc) {
                                    data_020c18bc.mUnk_2fc = px + rect[0];
                                }
                                if (px + rect[2] > data_020c18bc.mUnk_300) {
                                    data_020c18bc.mUnk_300 = px + rect[2];
                                }
                                if (py + rect[1] < data_020c18bc.mUnk_2fe) {
                                    data_020c18bc.mUnk_2fe = py + rect[1];
                                }
                                if (py + rect[3] > data_020c18bc.mUnk_302) {
                                    data_020c18bc.mUnk_302 = py + rect[3];
                                }
                            }
                        } else {
                            data_020c18bc.mUnk_317_0 = 1;
                        }
                    }
                } else {
                    skipped++;
                }
                lineX += widths[i];
                drawn += widths[i];
                i++;
            }
            while (func_020215f0(chars[i]) == 0x20 || chars[i] == 1 || chars[i] == data_020c18bc.mUnk_310) {
                i++;
                skipped++;
            }
            if (!window ||
                (lineNo >= data_020c18bc.mUnk_313 && lineNo < data_020c18bc.mUnk_313 + data_020c18bc.mUnk_314)) {
                if (dirMode != 0) {
                    if (dirMode == 1) {
                        y -= height;
                    } else {
                        y += height;
                    }
                } else {
                    y += height;
                }
            }
            lineNo++;
        } while (!single && chars[i] != 0);
    }
    data_020c18bc.mUnk_315 = lineNo;
    if (flags & TEXT_RETURN_HEIGHT) {
        return y - height - startY;
    }
    if (flags & TEXT_NO_WIDTH) {
        result = 0;
    }
    return result;
}

const u8 *func_02021dd8(u16 block, int id) {
    const u8 *table = (const u8 *) func_0202ded8(block);
    if ((u32) id < 2) {
        OS_Terminate();
    }
    return table + (id - 2) * 4 + 0x10;
}

const u8 *func_02021df4(u32 base, const u8 *entry) {
    return (const u8 *) func_0202ded8(base + entry[1]) + *(const u16 *) (entry + 2);
}

int func_02021e08(u16 id, int index) {
    const u8 *entry = func_02021dd8(id, index);
    if (entry[0] > 4) {
        return 0;
    }
    return entry[0];
}

const u8 *func_02021e18(u16 block, int id) {
    const u8 *entry = func_02021dd8(block, id);
    BOOL ok = 1;
    if (entry[0] != 4 && entry[0] != 1) {
        ok = 0;
    }
    if (ok == 0) {
        OS_Terminate();
    }
    return func_02021df4(block, entry);
}

int func_02021e44(u16 id, int index) {
    const u8 *entry = func_02021dd8(id, index);
    const u8 *p;
    BOOL ok = 1;
    if (entry[0] != 2 && entry[0] != 3) {
        ok = 0;
    }
    if (ok == 0) {
        OS_Terminate();
    }
    p = func_02021df4(id, entry);
    if (entry[0] == 2) {
        return (s16) (p[0] + (p[1] << 8));
    }
    return p[0] + ((p[1] << 8) + ((p[3] << 24) + (p[2] << 16)));
}

void func_02021e94(int block, int id, u8 *dst, int flags, ...) {
    const u8 *src = func_02021e18(block, id);
    if (flags & 1) {
        u8 saved = data_020c18bc.mUnk_318;
        data_020c18bc.mUnk_318 = 1;
        while (*src != 0) {
            func_0202158c(&dst, func_02021498(&src));
        }
        data_020c18bc.mUnk_318 = saved;
    } else {
        while (*src != 0) {
            *dst++ = *src++;
        }
    }
    *dst = 0;
}

int func_02021f0c(u16 block, int id, int flags) {
    u8 buf[0x80];
    const u8 *p;
    int count = 0;
    int mode = func_02021e08(block, id);
    if (mode == 4 || mode == 1) {
        if (flags & 2) {
            p = buf;
            func_02021e94(block, id, buf, 1);
        } else {
            p = func_02021e18(block, id);
        }
        if (mode == 4) {
            u32 ch = *p++;
            if (ch != 0) {
                do {
                    if ((flags & 1) == 0 || func_020215f0(ch) != 0x20) {
                        count++;
                    }
                    ch = *p++;
                } while (ch != 0);
            }
        } else {
            u16 ch = func_02021498(&p);
            if (ch != 0) {
                do {
                    if ((flags & 1) == 0 || func_020215f0(ch) != 0x20) {
                        count++;
                    }
                    ch = func_02021498(&p);
                } while (ch != 0);
            }
        }
    }
    return (u16) count;
}

void func_02021fa8(const char *fmt, ...) {}
