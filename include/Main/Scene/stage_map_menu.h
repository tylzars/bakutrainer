#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Number of nodes on the stage map, and the id of the first node's position record.
#define MAP_NODE_COUNT 25
#define MAP_NODE_POS_ID 0xa00

// Animation frame of the recap screen: -8 at set-up (a delay). While the reached node has a column-0xf value, raised by
// 1 on frames whose counter % 3 is 2. Frames 0..8 are drawn, then sprite 0x1089 plus that value.
extern s32 gRecapFrameIndex;

void UpdateMapMenu(void);
void LeaveMapMenu(void);
void UpdateMapMenuInput(void);
void SetupRecap(void);
void UpdateRecap(void);
void LeaveRecap(void);

#ifdef __cplusplus
}
#endif
