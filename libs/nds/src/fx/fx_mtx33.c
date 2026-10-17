#include <nitro/fx.h>

#define MUL33(a, b) ((fx32) (((fx64) (a) * (b)) >> FX32_SHIFT))

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
asm void func_0203b998(MtxFx33 *m, fx32 sin, fx32 cos) {
    stmia r0!, {r2}
    mov r3, #0
    stmia r0!, {r1, r3}
    neg r1, r1
    stmia r0!, {r1, r2}
    mov r1, #1
    lsl r1, r1, #12
    str r3, [r0, #0]
    str r3, [r0, #4]
    str r3, [r0, #8]
    str r1, [r0, #0xc]
    bx lr
}
#pragma thumb off

// Vector times 3x3 matrix.
void func_0203b9b0(const VecFx32 *v, const MtxFx33 *m, VecFx32 *out) {
    fx32 x = v->x, y = v->y, z = v->z;
    out->x = (fx32) (((fx64) x * m->_[0] + (fx64) y * m->_[3] + (fx64) z * m->_[6]) >> FX32_SHIFT);
    out->y = (fx32) (((fx64) x * m->_[1] + (fx64) y * m->_[4] + (fx64) z * m->_[7]) >> FX32_SHIFT);
    out->z = (fx32) (((fx64) x * m->_[2] + (fx64) y * m->_[5] + (fx64) z * m->_[8]) >> FX32_SHIFT);
}
