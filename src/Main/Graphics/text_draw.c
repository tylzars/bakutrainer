#include <Main/Graphics/text_draw.h>
#include <Main/Graphics/model.h>
#include <Main/Graphics/texture_blocks.h>
#include <Main/System/file_loader.h>
#include <Main/System/system_init.h>
#include <nitro/os.h>
#include <nitro/fx.h>

u16 ReadTextChar(const u8 **text) {
    u32 first;
    u32 lead;
    s32 mask;
    u32 value;
    u32 count;
    u32 num;
    const u8 *p;
    u16 acc;
    count = 0;
    if (gSystemState.mUseTextEscapes == 1 && **text == '$') {
        p = *text;
        num = 0;
        for (;;) {
            p++;
            if (*p < '0' || *p > '9') {
                break;
            }
            num = *p + num * 10 - '0';
        }
        first = gSystemState.mTextEscapeFunc(num);
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
                    value = gSystemState.mTextEscapeFunc(num);
                    if (value == 0) {
                        break;
                    }
                    acc2 += value & 0x3f;
                } while (first & (0x40 >> count));
            }
            return acc2 + ((first & (0x3f >> count)) << (6 * count));
        }
    }
    gSystemState.mUnk_316 = acc = 0;
    lead = **text;
    if ((lead & 0x80) == 0) {
        return *(*text)++;
    }
    if (lead & 0x40) {
        do {
            mask = 0x40;
            acc <<= 6;
            (*text)++;
            count++;
            acc += **text & 0x3f;
            mask >>= count;
        } while (lead & mask);
    }
    acc += (lead & (0x3f >> count)) << (6 * count);
    (*text)++;
    return acc;
}

void WriteTextChar(u8 **out, u32 code) {
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

u32 NormalizeSpace(u32 code) {
    if (code == 0x20 || code == 0xa0) {
        code = 0x20;
    }
    return code;
}

void DrawText(int x, int y, const void *font, const char *text, int align, int width, int height, int layer) {
    const void *f = font;
    DrawTextEx(x, y, (const FontRange *const *) &f, 0, (const u8 *) text, 0, align, width, height, layer);
}

// Matching:
//   - The `const u16 *` read into c keeps the glyph's own *q load; without the cast the compiler merges it with the
//     load into c.
//   - maxW doubles as the line loop's character count (zeroed first, so lineW and breakW copy it); spaces and lineW
//     are reused for the skipped count and the drawn width; spaceW is reloaded inside the width loop. These keep the
//     stack slots in the original order.
//   - i is an unsigned long: as u32 it changes the saved registers and the stack frame.
int DrawTextEx(int x, int y, const FontRange *const *fonts, u8 fontIndex, const u8 *text, u16 angle, u32 flags,
               u32 width, int height, int layer) {
    u8 widths[0x400];
    u16 chars[0x400];
    u16 glyphs[0x400];
    s32 rect[4];
    int odd;
    u8 spaceW;
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
    if (flags & TEXT_ROTATE) {
        if ((gSystemState.mFlags & 0x80) == 0) {
            ok = 0;
        }
    }
    if (ok == 0) {
        OS_Terminate();
    }
    if (layer != 1 && layer != 2) {
        OS_Terminate();
    }
    if (layer != 1 && (flags & TEXT_ROTATE)) {
        OS_Terminate();
    }
    if (flags & TEXT_BOUNDS) {
        gSystemState.mTextBoundsMinY = 0x7fff;
        gSystemState.mTextBoundsMaxY = -0x7fff;
        gSystemState.mTextBoundsMinX = 0x7fff;
        gSystemState.mTextBoundsMaxX = -0x7fff;
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
    gSystemState.mUseTextEscapes = (flags & TEXT_ESCAPES) ? 1 : 0;

    k = 0;
    do {
        chars[k++] = ReadTextChar(&text);
    } while (chars[k - 1] != 0);
    gSystemState.mTextIncomplete = 0;

    font = fonts[fontIndex];
    GetSpriteRect(font->mGlyph, 0, rect);
    lineH = dirMode ? rect[3] : rect[2];

    n = 0;
    for (;;) {
        k = 0;
        q = &chars[n];
        if (*q == 0) {
            break;
        }
        if (gSystemState.mUnk_311[0] == *q) {
            if (q[1] >= '0' && q[1] <= '9') {
                k = 1;
            }
            if (k == 0) {
                OS_Terminate();
            }
            font = fonts[q[1] - '0'];
            GetSpriteRect(font->mGlyph, 0, rect);
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
        if (NormalizeSpace(*q) != 0x20) {
            c = *(const u16 *) q;
            for (;;) {
                if (c >= fr->mFirst && c <= fr->mLast) {
                    glyphs[n] = fr->mGlyph + (*q - fr->mFirst);
                    break;
                }
                if (fr->mLast == 0 || fr->mFirst > c) {
                    if (gSystemState.mTextReplacementChar == 0x20) {
                        chars[n] = 0x20;
                        glyphs[n] = fr->mGlyph + (chars[n] - fr->mFirst);
                    } else {
                        *q = gSystemState.mTextReplacementChar;
                        n--;
                    }
                    break;
                }
                fr++;
            }
        }
        n++;
    }

    if (flags & TEXT_SINGLE_LINE) {
        maxW = 0x100;
    } else {
        maxW = 0x1000;
    }
    while (chars[k] != 0 && maxW > result) {
        spaceW = lineH;
        if (NormalizeSpace(chars[k]) > 0x20) {
            GetSpriteRect(glyphs[k], 0, rect);
            if (dirMode) {
                widths[k] = rect[3];
            } else {
                widths[k] = rect[2];
            }
        } else {
            spaces++;
            widths[k] = spaceW;
        }
        if (!(flags & TEXT_LIMIT) || k < gSystemState.mTextCharLimit + spaces) {
            result += widths[k];
        }
        k++;
    }

    spaces = 0;
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
            int brk;
            maxW = 0;
            lineW = maxW;
            breakW = maxW;
            nl = gSystemState.mTextLineBreakChar;
            while ((c = chars[i + maxW]) > 1 && lineW < width) {
                brk = 0;
                if (c == 0x20) {
                    if (punct) {
                        u32 next = chars[i + maxW + 1];
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
                if (brk == 1 || (maxW != 0 && chars[i + maxW - 1] == '-')) {
                    breakW = lineW;
                    lineW += widths[i + maxW];
                } else {
                    lineW += widths[i + maxW];
                }
                maxW++;
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
            lineW = 0;
            while (lineW < breakW) {
                int px;
                int py;
                int pal;
                if (chars[i] == 0) {
                    break;
                }
                if (NormalizeSpace(chars[i]) != 0x20 && gSystemState.mTextLineBreakChar != chars[i] && !nodraw) {
                    if (!(window &&
                          (lineNo < gSystemState.mTextWindowLineIndex ||
                           lineNo >= gSystemState.mTextWindowLineIndex + gSystemState.mTextWindowLineCount))) {
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
                        pal = gSystemState.mUnk_308;
                        odd = i & 1;
                        if (!((flags & TEXT_LIMIT) && i >= gSystemState.mTextCharLimit + spaces)) {
                            if (flags & TEXT_ROTATE) {
                                int dx = px - x;
                                int dy = py - y;
                                int cs = *cosp;
                                int ns = -*sinp;
                                attr = gSystemState.mUnk_30a[1];
                                px = x + ((dx * cs - dy * ns) >> 12);
                                py = y + ((dx * ns + dy * cs) >> 12);
                                if (hilite) {
                                    attr = 0x1f;
                                }
                                DrawSpritePolygons(glyphHigh + glyphs[i], px, py, -gSystemState.mUnk_30a[0] << 6, angle,
                                                   0x1000, 0x1000, attr);
                            } else if (alt) {
                                if (dirMode != 0) {
                                    OS_Terminate();
                                }
                                DrawSprite((glyphHigh + 0x100000) | glyphs[i], px, py, 0, layer);
                            } else {
                                DrawSprite(glyphHigh + (glyphs[i] + (hilite ? 0x40000 : 0)), px, py, pal + odd, layer);
                            }
                            if (flags & TEXT_BOUNDS) {
                                GetSpriteRect(glyphHigh + glyphs[i], 0, rect);
                                if (px + rect[0] < gSystemState.mTextBoundsMinX) {
                                    gSystemState.mTextBoundsMinX = px + rect[0];
                                }
                                if (px + rect[2] > gSystemState.mTextBoundsMaxX) {
                                    gSystemState.mTextBoundsMaxX = px + rect[2];
                                }
                                if (py + rect[1] < gSystemState.mTextBoundsMinY) {
                                    gSystemState.mTextBoundsMinY = py + rect[1];
                                }
                                if (py + rect[3] > gSystemState.mTextBoundsMaxY) {
                                    gSystemState.mTextBoundsMaxY = py + rect[3];
                                }
                            }
                        } else {
                            gSystemState.mTextIncomplete = 1;
                        }
                    }
                } else {
                    spaces++;
                }
                lineX += widths[i];
                lineW += widths[i];
                i++;
            }
            while (NormalizeSpace(chars[i]) == 0x20 || chars[i] == 1 || chars[i] == gSystemState.mTextLineBreakChar) {
                i++;
                spaces++;
            }
            if (!window || (lineNo >= gSystemState.mTextWindowLineIndex &&
                            lineNo < gSystemState.mTextWindowLineIndex + gSystemState.mTextWindowLineCount)) {
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
        } while (!(flags & TEXT_SINGLE_LINE) && chars[i] != 0);
    }
    gSystemState.mTextLineCount = lineNo;
    if (flags & TEXT_RETURN_HEIGHT) {
        return y - height - startY;
    }
    return (flags & TEXT_NO_WIDTH) ? 0 : result;
}

// The 4-byte entry of value `index` (from 2) in the table of indexed file `id`, which starts at 0x10; an index of 0
// or 1 stops the system.
const u8 *GetFileValueEntry(u16 id, int index) {
    const u8 *table = (const u8 *) FindOrLoadCachedFile(id);
    if ((u32) index < 2) {
        OS_Terminate();
    }
    return table + (index - 2) * 4 + 0x10;
}

// The bytes of an entry's value: in file id + entry[1], at the offset in the entry's halfword at 2.
const u8 *GetFileValueData(u32 id, const u8 *entry) {
    return (const u8 *) FindOrLoadCachedFile(id + entry[1]) + *(const u16 *) (entry + 2);
}

// The type of value `index` of indexed file `id`. 1 and 4 are strings (GetFileString; CountFileStringChars counts
// type 1 in encoded characters, type 4 in bytes). 2 is a 16-bit and 3 a 32-bit number (GetFileValue). 0 for any type
// above 4.
int GetFileValueType(u16 id, int index) {
    const u8 *entry = GetFileValueEntry(id, index);
    if (entry[0] > 4) {
        return 0;
    }
    return entry[0];
}

const u8 *GetFileString(u16 id, int index) {
    const u8 *entry = GetFileValueEntry(id, index);
    BOOL ok = 1;
    if (entry[0] != 4 && entry[0] != 1) {
        ok = 0;
    }
    if (ok == 0) {
        OS_Terminate();
    }
    return GetFileValueData(id, entry);
}

int GetFileValue(u16 id, int index) {
    const u8 *entry = GetFileValueEntry(id, index);
    const u8 *p;
    BOOL ok = 1;
    if (entry[0] != 2 && entry[0] != 3) {
        ok = 0;
    }
    if (ok == 0) {
        OS_Terminate();
    }
    p = GetFileValueData(id, entry);
    if (entry[0] == 2) {
        return (s16) (p[0] + (p[1] << 8));
    }
    return p[0] + ((p[1] << 8) + ((p[3] << 24) + (p[2] << 16)));
}

void CopyFileString(int id, int index, u8 *dst, int flags, ...) {
    const u8 *src = GetFileString(id, index);
    if (flags & 1) {
        u8 saved = gSystemState.mUseTextEscapes;
        gSystemState.mUseTextEscapes = 1;
        while (*src != 0) {
            WriteTextChar(&dst, ReadTextChar(&src));
        }
        gSystemState.mUseTextEscapes = saved;
    } else {
        while (*src != 0) {
            *dst++ = *src++;
        }
    }
    *dst = 0;
}

int CountFileStringChars(u16 id, int index, int flags) {
    u8 buf[0x80];
    const u8 *p;
    int count = 0;
    int type = GetFileValueType(id, index);
    if (type == 4 || type == 1) {
        if (flags & 2) {
            p = buf;
            CopyFileString(id, index, buf, 1);
        } else {
            p = GetFileString(id, index);
        }
        if (type == 4) {
            u32 ch = *p++;
            if (ch != 0) {
                do {
                    if ((flags & 1) == 0 || NormalizeSpace(ch) != 0x20) {
                        count++;
                    }
                    ch = *p++;
                } while (ch != 0);
            }
        } else {
            u16 ch = ReadTextChar(&p);
            if (ch != 0) {
                do {
                    if ((flags & 1) == 0 || NormalizeSpace(ch) != 0x20) {
                        count++;
                    }
                    ch = ReadTextChar(&p);
                } while (ch != 0);
            }
        }
    }
    return (u16) count;
}

void func_02021fa8(const char *fmt, ...) {}
