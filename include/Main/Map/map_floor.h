#pragma once

#include "types.h"
#include <nitro/fx.h>

#ifdef __cplusplus
extern "C" {
#endif

// The stage data chunk MAP_FLOOR_CELLS holds one halfword per map cell: the start of the cell's list in chunk
// MAP_FLOOR_LISTS. A list entry names a region of chunk MAP_FLOOR_REGIONS (MAP_FLOOR_INDEX, in 16-byte units);
// MAP_FLOOR_LAST marks the list's last entry.
#define MAP_FLOOR_CELLS 0x5528
#define MAP_FLOOR_LISTS 0x552a
#define MAP_FLOOR_REGIONS 0x552b
#define MAP_FLOOR_INDEX 0x3fff
#define MAP_FLOOR_LAST 0x8000

// mUnk_14 of a region: with MAP_FLOOR_SINGLE set, the low byte is the region's attribute and its height data follows
// mUnk_14 directly; otherwise the low 15 bits are the offset of the height data from mUnk_14 and mUnk_16 outlines
// (MapFloorShape) follow it, each with its own attribute.
#define MAP_FLOOR_SINGLE 0x8000
// The first halfword of the height data: the number of samples per side in the low byte, MAP_FLOOR_WIDE when the
// samples are halfwords instead of bytes.
#define MAP_FLOOR_WIDE 0x100
// An outline whose attribute is MAP_FLOOR_NONE never matches.
#define MAP_FLOOR_NONE 0xff

// One point of an outline, relative to the region's corner.
typedef struct MapFloorPoint {
    s16 mUnk_00;
    s16 mUnk_02;
} MapFloorPoint;

// An outline: mUnk_00 points, attribute mUnk_01.
typedef struct MapFloorShape {
    u8 mUnk_00;
    u8 mUnk_01;
    MapFloorPoint mUnk_02[1];
} MapFloorShape;

// A floor region: its x/z bounds in units of 512 (mUnk_00..mUnk_0c), the heights of its lowest and highest sample
// in the same units (mUnk_10, mUnk_12), then mUnk_14/mUnk_16 as described above.
typedef struct MapFloorRegion {
    u32 mUnk_00;
    u32 mUnk_04;
    u32 mUnk_08;
    u32 mUnk_0c;
    s16 mUnk_10;
    s16 mUnk_12;
    u16 mUnk_14;
    u8 mUnk_16;
    u8 mUnk_17;
} MapFloorRegion;

// The floor height under (x, z) for a position at height y: the highest floor of the map cell that y lies above (or
// within a of below); 0 when (x, z) is outside the map. Outlines whose attribute bit is set in mask are skipped.
// *out receives the floor's attribute (0xff when none was found) and *vec its normal; either may be NULL.
int func_02030830(fx32 x, fx32 z, fx32 y, int a, u32 mask, u8 *out, VecFx32 *vec);
// func_02030830 without the normal.
int func_02031270(fx32 x, fx32 z, fx32 y, int a, u32 mask, u8 *out);
// func_0201feb0 of the camera's point mUnk_14 relative to its point mUnk_20.
u32 func_0203129c(void);

#ifdef __cplusplus
}
#endif
