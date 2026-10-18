#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Number of nodes on the stage map, and the id of the first node's position record.
#define MAP_NODE_COUNT 25
#define MAP_NODE_POS_ID 0xa00

// Frame counter of the recap screen's animation (it starts negative).
extern s32 data_020c17fc;

void func_02019fa0(void);
void func_0201a3d4(void);
void func_0201a3e0(void);
void func_0201a4b0(void);
void func_0201a50c(void);
void func_0201a600(void);

#ifdef __cplusplus
}
#endif
