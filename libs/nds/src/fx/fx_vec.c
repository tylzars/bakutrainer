#include <nitro/fx.h>

#define REG_DIVCNT (*(vu16 *) 0x04000280)
#define REG_DIV_NUMER (*(vs64 *) 0x04000290)
#define REG_DIV_DENOM (*(vs64 *) 0x04000298)
#define REG_DIV_RESULT (*(s64 *) 0x040002a0)
#define REG_SQRTCNT (*(vu16 *) 0x040002b0)
#define REG_SQRT_RESULT (*(s32 *) 0x040002b4)
#define REG_SQRT_PARAM (*(s64 *) 0x040002b8)

#define DIVCNT_BUSY 0x8000
#define DIVCNT_MODE_64_64 2
#define SQRTCNT_BUSY 0x8000
#define SQRTCNT_MODE_64 1

fx32 VEC_DotProduct(const VecFx32 *a, const VecFx32 *b) {
    return (fx32) (((fx64) a->x * b->x + (fx64) a->y * b->y + (fx64) a->z * b->z + 0x800LL) >> FX32_SHIFT);
}

void VEC_CrossProduct(const VecFx32 *a, const VecFx32 *b, VecFx32 *axb) {
    fx32 x = (fx32) (((fx64) a->y * b->z - (fx64) a->z * b->y + 0x800LL) >> FX32_SHIFT);
    fx32 y = (fx32) (((fx64) a->z * b->x - (fx64) a->x * b->z + 0x800LL) >> FX32_SHIFT);
    fx32 z = (fx32) (((fx64) a->x * b->y - (fx64) a->y * b->x + 0x800LL) >> FX32_SHIFT);
    axb->x = x;
    axb->y = y;
    axb->z = z;
}

// Unit vector in the direction of src, using the divider and square-root units.
void VEC_Normalize(const VecFx32 *src, VecFx32 *dst) {
    fx64 sum = (fx64) src->x * src->x;
    fx64 t;
    s32 s;
    sum += (fx64) src->y * src->y;
    sum += (fx64) src->z * src->z;
    REG_DIVCNT = DIVCNT_MODE_64_64;
    REG_DIV_NUMER = 0x1000000LL << 32;
    REG_DIV_DENOM = sum;
    REG_SQRTCNT = SQRTCNT_MODE_64;
    REG_SQRT_PARAM = sum << 2;
    while (REG_SQRTCNT & SQRTCNT_BUSY) {
    }
    s = REG_SQRT_RESULT;
    while (REG_DIVCNT & DIVCNT_BUSY) {
    }
    t = REG_DIV_RESULT * s;
    dst->x = (fx32) ((t * src->x + (0x1000LL << 32)) >> 45);
    dst->y = (fx32) ((t * src->y + (0x1000LL << 32)) >> 45);
    dst->z = (fx32) ((t * src->z + (0x1000LL << 32)) >> 45);
}
