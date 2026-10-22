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
// mUnk_14 directly; otherwise the low 15 bits are the offset of the height data from mUnk_14 and mShapeCount outlines
// (MapFloorShape) follow it, each with its own attribute.
#define MAP_FLOOR_SINGLE 0x8000
// The first halfword of the height data: the number of samples per side in the low byte, MAP_FLOOR_WIDE when the
// samples are halfwords instead of bytes.
#define MAP_FLOOR_WIDE 0x100
// An outline whose attribute is MAP_FLOOR_NONE never matches.
#define MAP_FLOOR_NONE 0xff

// One point of an outline, relative to the region's corner.
typedef struct MapFloorPoint {
    s16 mX;
    s16 mZ;
} MapFloorPoint;

// An outline of a region, with its own attribute.
typedef struct MapFloorShape {
    u8 mPointCount;
    u8 mAttr;
    MapFloorPoint mPoints[1];
} MapFloorShape;

// A floor region: its x/z bounds and the heights of its lowest and highest sample, all in units of 512 (mMinX and
// mMinZ are also the corner the outline points and the height samples are placed from), then mUnk_14 and the outline
// count as described above.
typedef struct MapFloorRegion {
    u32 mMinX;
    u32 mMinZ;
    u32 mMaxX;
    u32 mMaxZ;
    s16 mMinY;
    s16 mMaxY;
    u16 mUnk_14;
    u8 mShapeCount;
    u8 mUnk_17;
} MapFloorRegion;

// The floor height under (x, z) for a position at height y: the highest floor of the map cell that y lies above (or
// within a of below); 0 when (x, z) is outside the map. Outlines whose attribute bit is set in mask are skipped.
// *out receives the floor's attribute (0xff when none was found) and *vec its normal; either may be NULL.
int GetFloorHeightEx(fx32 x, fx32 z, fx32 y, int a, u32 mask, u8 *out, VecFx32 *vec);
// GetFloorHeightEx without the normal.
int GetFloorHeight(fx32 x, fx32 z, fx32 y, int a, u32 mask, u8 *out);
// AngleOfVector of the vector from the end of the nearest ring segment to its start (mNearestSegmentStart relative to
// mNearestSegmentEnd), as FindNearestRingSegment last recorded them.
u32 GetNearestSegmentAngle(void);

#ifdef __cplusplus
}
#endif
