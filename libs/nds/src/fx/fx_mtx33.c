#include <nitro/fx.h>

#define MUL33(a, b) ((fx32) (((fx64) (a) * (b)) >> FX32_SHIFT))

// Writes the transpose of a 3x3 matrix.
// Not compiled: stores with `stmia r1!, {r2, r5, r8}` (base written back).
// The compiler writes back an ldm/stm base only in push/pop on sp and in struct copies and fills, whose
// registers are always r0-r3 (r0-r1 for a two-word fill); the pass that merges loads or stores into one ldm/stm
// never adds writeback.
// clang-format off
asm void func_0203b8bc(const MtxFx33 *src, MtxFx33 *dst) {
    stmfd sp!, {r4, r5, r6, r7, r8, r9}
    ldmia r0, {r2, r3, r4, r5, r6, r7, r8, r9, ip}
    stmia r1!, {r2, r5, r8}
    stmia r1!, {r3, r6, r9}
    stmia r1!, {r4, r7, ip}
    ldmfd sp!, {r4, r5, r6, r7, r8, r9}
    bx lr
}
// clang-format on

// Scales the three rows of a 3x3 matrix by x, y and z.
void func_0203b8d8(const MtxFx33 *src, MtxFx33 *dst, fx32 x, fx32 y, fx32 z) {
    dst->_[0] = MUL33(x, src->_[0]);
    dst->_[1] = MUL33(x, src->_[1]);
    dst->_[2] = MUL33(x, src->_[2]);
    dst->_[3] = MUL33(y, src->_[3]);
    dst->_[4] = MUL33(y, src->_[4]);
    dst->_[5] = MUL33(y, src->_[5]);
    dst->_[6] = MUL33(z, src->_[6]);
    dst->_[7] = MUL33(z, src->_[7]);
    dst->_[8] = MUL33(z, src->_[8]);
}

#pragma thumb on
// Writes the rotation matrix about the Z axis for the given sine and cosine.
void func_0203b998(MtxFx33 *m, fx32 sin, fx32 cos) {
    fx32 *p = m->_;
    p[0] = cos;
    p[1] = sin;
    p[2] = 0;
    p[3] = -sin;
    p[4] = cos;
    p[5] = 0;
    p[6] = 0;
    p[7] = 0;
    p[8] = 0x1000;
}
#pragma thumb off

// Vector times 3x3 matrix.
void func_0203b9b0(const VecFx32 *v, const MtxFx33 *m, VecFx32 *out) {
    fx32 x = v->x, y = v->y, z = v->z;
    out->x = (fx32) (((fx64) x * m->_[0] + (fx64) y * m->_[3] + (fx64) z * m->_[6]) >> FX32_SHIFT);
    out->y = (fx32) (((fx64) x * m->_[1] + (fx64) y * m->_[4] + (fx64) z * m->_[7]) >> FX32_SHIFT);
    out->z = (fx32) (((fx64) x * m->_[2] + (fx64) y * m->_[5] + (fx64) z * m->_[8]) >> FX32_SHIFT);
}
