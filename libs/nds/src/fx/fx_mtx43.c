#include <nitro/fx.h>
#include <nitro/mi.h>

#define MUL33(a, b) ((fx32) (((fx64) (a) * (b)) >> FX32_SHIFT))

// Scales the three rows of the 3x3 part by x, y and z and copies the translation.
void func_0203ba2c(const MtxFx43 *src, MtxFx43 *dst, fx32 x, fx32 y, fx32 z) {
    func_0203b8d8((const MtxFx33 *) src, (MtxFx33 *) dst, x, y, z);
    dst->_[9] = src->_[9];
    dst->_[10] = src->_[10];
    dst->_[11] = src->_[11];
}


#define CROSS64(a, d, b, c) ((fx64) (a) * (d) - (fx64) (b) * (c))
// Back from a 64-bit product sum to fx32: truncated, or rounded to the nearest 1/4096.
#define TRUNC12(x) ((fx32) ((x) >> 12))
#define ROUND12(x) ((fx32) (((x) + (fx64) 0x800) >> 12))

// Inverts the 3x3 part through its 2x2 minors and the determinant, then sets the translation row to
// minus the old translation times the new 3x3 part. Elements are numbered row by row (0-8 the 3x3 part, 9-11 the
// translation). Returns -1 and leaves dst alone when the determinant is zero, 0 otherwise. src may equal dst; the
// result then goes through a stack copy.
int func_0203ba60(const MtxFx43 *src, MtxFx43 *dst) {
    MtxFx43 tmp;
    MtxFx43 *out;
    fx32 det;                  // the determinant, later its reciprocal
    fx32 minor0, minor1, minor2;
    fx32 minor3, minor6, minor4, minor7;   // named after the element each one ends up in (transposed)

    if (src == dst) {
        out = &tmp;
    } else {
        out = dst;
    }

    // Minors of the first row (rounded), then the determinant by expansion along that row.
    minor0 = ROUND12(CROSS64(src->_[4], src->_[8], src->_[5], src->_[7]));
    minor1 = ROUND12(CROSS64(src->_[3], src->_[8], src->_[5], src->_[6]));
    minor2 = ROUND12(CROSS64(src->_[3], src->_[7], src->_[4], src->_[6]));

    det = ROUND12(CROSS64(src->_[0], minor0, src->_[1], minor1) + (fx64) src->_[2] * minor2);

    if (det == 0) {
        return -1;
    }

    // Start 1/det on the divider and fill the wait with four more minors (truncated).
    FX_InvAsync(det);

    minor3 = TRUNC12(CROSS64(src->_[1], src->_[8], src->_[7], src->_[2]));
    minor6 = TRUNC12(CROSS64(src->_[1], src->_[5], src->_[4], src->_[2]));
    minor4 = TRUNC12(CROSS64(src->_[0], src->_[8], src->_[6], src->_[2]));
    minor7 = TRUNC12(CROSS64(src->_[0], src->_[5], src->_[3], src->_[2]));

    // Adjugate times 1/det, with the checkerboard signs; the last two minors are formed in place.
    det = FX_GetDivResult();
    out->_[0] = MUL33(det, minor0);
    out->_[1] = -MUL33(det, minor3);
    out->_[2] = MUL33(det, minor6);

    out->_[3] = -MUL33(det, minor1);
    out->_[4] = MUL33(det, minor4);
    out->_[5] = -MUL33(det, minor7);

    out->_[6] = MUL33(det, minor2);
    out->_[7] = -MUL33(det, TRUNC12(CROSS64(src->_[0], src->_[7], src->_[6], src->_[1])));
    out->_[8] = MUL33(det, TRUNC12(CROSS64(src->_[0], src->_[4], src->_[3], src->_[1])));

    // New translation: -(old translation) * (new 3x3 part), one column at a time.
    out->_[9] = -TRUNC12((fx64) out->_[0] * src->_[9] + (fx64) out->_[3] * src->_[10] + (fx64) out->_[6] * src->_[11]);
    out->_[10] = -TRUNC12((fx64) out->_[1] * src->_[9] + (fx64) out->_[4] * src->_[10] + (fx64) out->_[7] * src->_[11]);
    out->_[11] = -TRUNC12((fx64) out->_[2] * src->_[9] + (fx64) out->_[5] * src->_[10] + (fx64) out->_[8] * src->_[11]);

    if (out == &tmp) {
        func_0203fdd8(&tmp, dst);
    }
    return 0;
}

#undef CROSS64
#undef TRUNC12
#undef ROUND12

// Concatenation of two 4x3 matrices; ab may be b.
void func_0203bdec(const MtxFx43 *a, const MtxFx43 *b, MtxFx43 *ab) {
    MtxFx43 tmp;
    MtxFx43 *p;
    fx32 x, y, z;
    fx32 xx, yy, zz;

    if (ab == b) {
        p = &tmp;
    } else {
        p = ab;
    }
    x = a->_[0];
    y = a->_[1];
    z = a->_[2];
    p->_[0] = (fx32) (((fx64) x * b->_[0] + (fx64) y * b->_[3] + (fx64) z * b->_[6]) >> FX32_SHIFT);
    p->_[1] = (fx32) (((fx64) x * b->_[1] + (fx64) y * b->_[4] + (fx64) z * b->_[7]) >> FX32_SHIFT);
    xx = b->_[2];
    yy = b->_[5];
    zz = b->_[8];
    p->_[2] = (fx32) (((fx64) x * xx + (fx64) y * yy + (fx64) z * zz) >> FX32_SHIFT);
    x = a->_[3];
    y = a->_[4];
    z = a->_[5];
    p->_[5] = (fx32) (((fx64) x * xx + (fx64) y * yy + (fx64) z * zz) >> FX32_SHIFT);
    p->_[4] = (fx32) (((fx64) x * b->_[1] + (fx64) y * b->_[4] + (fx64) z * b->_[7]) >> FX32_SHIFT);
    xx = b->_[0];
    yy = b->_[3];
    zz = b->_[6];
    p->_[3] = (fx32) (((fx64) x * xx + (fx64) y * yy + (fx64) z * zz) >> FX32_SHIFT);
    x = a->_[6];
    y = a->_[7];
    z = a->_[8];
    p->_[6] = (fx32) (((fx64) x * xx + (fx64) y * yy + (fx64) z * zz) >> FX32_SHIFT);
    p->_[7] = (fx32) (((fx64) x * b->_[1] + (fx64) y * b->_[4] + (fx64) z * b->_[7]) >> FX32_SHIFT);
    xx = b->_[2];
    yy = b->_[5];
    zz = b->_[8];
    p->_[8] = (fx32) (((fx64) x * xx + (fx64) y * yy + (fx64) z * zz) >> FX32_SHIFT);
    x = a->_[9];
    y = a->_[10];
    z = a->_[11];
    p->_[11] = (fx32) ((((fx64) x * xx + (fx64) y * yy + (fx64) z * zz) >> FX32_SHIFT) + b->_[11]);
    p->_[10] = (fx32) ((((fx64) x * b->_[1] + (fx64) y * b->_[4] + (fx64) z * b->_[7]) >> FX32_SHIFT) + b->_[10]);
    xx = b->_[0];
    yy = b->_[3];
    zz = b->_[6];
    p->_[9] = (fx32) ((((fx64) x * xx + (fx64) y * yy + (fx64) z * zz) >> FX32_SHIFT) + b->_[9]);

    if (p == &tmp) {
        *ab = tmp;
    }
}

// View matrix from the camera position, the up vector and the target.
void func_0203c0c0(const VecFx32 *pos, const VecFx32 *up, const VecFx32 *target, MtxFx43 *mtx) {
    VecFx32 vz, vx, vy;
    vz.x = pos->x - target->x;
    vz.y = pos->y - target->y;
    vz.z = pos->z - target->z;
    VEC_Normalize(&vz, &vz);
    VEC_CrossProduct(up, &vz, &vx);
    VEC_Normalize(&vx, &vx);
    VEC_CrossProduct(&vz, &vx, &vy);
    mtx->_[0] = vx.x;
    mtx->_[1] = vy.x;
    mtx->_[2] = vz.x;
    mtx->_[3] = vx.y;
    mtx->_[4] = vy.y;
    mtx->_[5] = vz.y;
    mtx->_[6] = vx.z;
    mtx->_[7] = vy.z;
    mtx->_[8] = vz.z;
    mtx->_[9] = -VEC_DotProduct(pos, &vx);
    mtx->_[10] = -VEC_DotProduct(pos, &vy);
    mtx->_[11] = -VEC_DotProduct(pos, &vz);
}

