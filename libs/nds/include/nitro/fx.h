#pragma once

#include "types.h"

#define FX32_SHIFT 12

static inline fx32 FX_Mul(fx32 v1, fx32 v2) {
    return (fx32) (((fx64) v1 * v2 + 0x800LL) >> FX32_SHIFT);
}

// 3D vector of fixed-point values.
typedef struct {
    fx32 x;
    fx32 y;
    fx32 z;
} VecFx32;

// 3x3 matrix (rotation and scale only), row-major.
typedef struct {
    fx32 _[9];
} MtxFx33;

// 4x3 matrix (three basis rows plus translation), row-major.
typedef struct {
    fx32 _[12];
} MtxFx43;

// 4x4 matrix, row-major.
typedef struct {
    fx32 _[16];
} MtxFx44;

#ifdef __cplusplus
extern "C" {
#endif

fx32 FX_Div(fx32 num, fx32 den);
fx64 FX_GetDivResultFx64c(void);
fx32 FX_GetDivResult(void);
void FX_InvAsync(fx32 x);
void FX_DivAsync(fx32 num, fx32 den);
u16 FX_Atan2Idx(fx32 y, fx32 x);

void func_0203b8d8(const MtxFx33 *src, MtxFx33 *dst, fx32 x, fx32 y, fx32 z);
void func_0203b998(MtxFx33 *m, fx32 sin, fx32 cos);
void func_0203b9b0(const VecFx32 *v, const MtxFx33 *m, VecFx32 *out);
void func_0203ba2c(const MtxFx43 *src, MtxFx43 *dst, fx32 x, fx32 y, fx32 z);
int func_0203ba60(const MtxFx43 *src, MtxFx43 *dst);
void func_0203bdec(const MtxFx43 *a, const MtxFx43 *b, MtxFx43 *ab);
void func_0203c0c0(const VecFx32 *pos, const VecFx32 *up, const VecFx32 *target, MtxFx43 *mtx);
void func_0203c1c8(fx32 sinFov, fx32 cosFov, fx32 aspect, fx32 n, fx32 f, fx32 scale, MtxFx44 *mtx);
void func_0203c314(fx32 t, fx32 b, fx32 l, fx32 r, fx32 n, fx32 f, fx32 scale, MtxFx44 *mtx);

fx32 VEC_DotProduct(const VecFx32 *a, const VecFx32 *b);
void VEC_CrossProduct(const VecFx32 *a, const VecFx32 *b, VecFx32 *axb);
void VEC_Normalize(const VecFx32 *src, VecFx32 *dst);

#ifdef __cplusplus
}
#endif
