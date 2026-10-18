#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Flags of func_02021624 and func_020215fc.
#define TEXT_ALIGN_MASK 0x3
#define TEXT_HILITE 0x8
#define TEXT_NO_DRAW 0x10
#define TEXT_RETURN_HEIGHT 0x20
#define TEXT_NO_WIDTH 0x40
#define TEXT_SINGLE_LINE 0x80
#define TEXT_ROTATE 0x100
#define TEXT_ALT_DRAW 0x200
#define TEXT_LIMIT 0x400
#define TEXT_BOUNDS 0x800
#define TEXT_LINE_WINDOW 0x1000
#define TEXT_VERTICAL 0x2000
#define TEXT_ESCAPES 0x4000
#define TEXT_NO_BREAK_PUNCT 0x8000
#define TEXT_VERTICAL_FLIP 0x10000

// A font maps character codes to glyph ids through a list of ranges that ends with a range whose last code is 0.
typedef struct FontRange {
    u16 mFirst;
    u16 mLast;
    u16 mGlyph;
} FontRange;

// A font list in .data (eight FontRange lists).
extern const FontRange *data_02063988[];

u16 func_02021498(const u8 **text);
void func_0202158c(u8 **out, u32 code);
u32 func_020215f0(u32 code);
void func_020215fc(int x, int y, const void *font, const char *text, int align, int width, int height, int mode);
int func_02021624(int x, int y, const FontRange *const *fonts, int fontIndex, const u8 *text, int angle, u32 flags,
                  u32 width, int height, int mode);
const u8 *func_02021dd8(u16 block, int id);
const u8 *func_02021df4(u32 base, const u8 *entry);
int func_02021e08(u16 id, int index);
const u8 *func_02021e18(u16 block, int id);
int func_02021e44(u16 id, int index);
void func_02021e94(int block, int id, u8 *dst, int flags, ...);
int func_02021f0c(u16 block, int id, int flags);
void func_02021fa8(const char *fmt, ...);

#ifdef __cplusplus
}
#endif
