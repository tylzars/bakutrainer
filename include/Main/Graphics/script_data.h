#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// The data of script_data.s in the code area.
// Font records (a range of character codes, its first glyph and the terminator), read by the text routines.
extern const u8 data_0201a608[];
extern const u8 data_0201a612[];
extern const u8 data_0201a61c[];
extern const u8 data_0201a626[];
extern const u8 data_0201a630[];
extern const u8 data_0201a63a[];
extern const u8 data_0201a644[];
extern const u8 data_0201a64e[];
extern const u8 data_0201a662[];
extern const u8 data_0201a66c[];
extern const u8 data_0201a676[];
extern const u8 data_0201a680[];
extern const u8 data_0201a68a[];
extern const u8 data_0201a694[];
extern const u8 data_0201a6b2[];
extern const u8 data_0201a6bc[];
extern const u8 data_0201a6c6[];
extern const u8 data_0201a6d0[];

// Model scripts (run by ExecModelScript) and sprite scripts (run by UpdateSprite), by table.
extern const u32 data_0201aa60[];
extern const u32 data_0201ab04[];
extern u32 data_0201aba0[7];
extern u32 data_0201abbc[];
extern void *data_0201abec[];

#ifdef __cplusplus
}
#endif
