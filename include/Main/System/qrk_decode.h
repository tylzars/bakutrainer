#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

void UnpackQrk(const u8 *src, u8 *dst, void *work);
u32 GetQrkUnpackedSize(const u8 *src);
u32 GetQrkWorkSize(void);

#ifdef __cplusplus
}
#endif
