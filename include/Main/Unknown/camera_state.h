#pragma once

#include "types.h"
#include <Main/Unknown/globals.h>

#ifdef __cplusplus
extern "C" {
#endif

// The battle and stage camera code declare the camera object's state (its first 0xc0 bytes) and its view settings
// (at 0x300) as two objects. Their accesses to mUnk_98..mUnk_b0 go through the object + 0x80, and the compiler
// checks 0x80 + the member offset against the declared size: up to 0x120 bytes that counts as past the end, which
// keeps them ordered against the stack accesses around them (func_020073b0, func_02015d58, func_02015e08,
// func_02018558); with the whole object declared they move. A file including this header never reaches past 0xc0
// through data_020c5e44.
extern CameraState data_020c5e44;
extern Unk_020c6144 data_020c6144;

#ifdef __cplusplus
}
#endif
