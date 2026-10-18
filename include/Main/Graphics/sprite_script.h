#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Runs the script of the sprite object `obj` (a Sprite) for one frame: switches to a requested state, executes the
// script's commands and returns the frame to show.
int func_0202db1c(void *obj);

// Clears the sprite object `obj` and starts it on the script `p`.
void func_0202de60(void *obj, void *p);

// Queues the frame `a` of a sprite at (x, y) with priority `prio`; `e` is not used.
int func_0202de80(int a, int x, int y, int prio, int e, int mode);

#ifdef __cplusplus
}
#endif
