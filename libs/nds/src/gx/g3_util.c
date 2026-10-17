#include <nitro/fx.h>
#include <nitro/gx.h>

#define G3X_MTXMODE_PROJECTION 0
#define G3X_MTXMODE_POSITION_VECTOR 2

// Builds a perspective projection (into a temporary when mtx is null) and optionally loads it as the projection matrix.
void func_0203d030(fx32 fovySin, fx32 fovyCos, fx32 aspect, fx32 n, fx32 f, fx32 scale, BOOL load, MtxFx44 *mtx) {
    MtxFx44 tmp;
    if (mtx == 0) {
        mtx = &tmp;
    }
    func_0203c1c8(fovySin, fovyCos, aspect, n, f, scale, mtx);
    if (load) {
        reg_G3X_MTX_MODE = G3X_MTXMODE_PROJECTION;
        func_0203ca80(mtx);
    }
}

// Builds an orthographic projection and optionally loads it as the projection matrix.
void func_0203d08c(fx32 t, fx32 b, fx32 l, fx32 r, fx32 n, fx32 f, fx32 scale, BOOL load, MtxFx44 *mtx) {
    MtxFx44 tmp;
    if (mtx == 0) {
        mtx = &tmp;
    }
    func_0203c314(t, b, l, r, n, f, scale, mtx);
    if (load) {
        reg_G3X_MTX_MODE = G3X_MTXMODE_PROJECTION;
        func_0203ca80(mtx);
    }
}

// Builds a view matrix and optionally loads it as the position matrix.
void func_0203d0f0(const VecFx32 *pos, const VecFx32 *up, const VecFx32 *target, BOOL load, MtxFx43 *mtx) {
    MtxFx43 tmp;
    if (mtx == 0) {
        mtx = &tmp;
    }
    func_0203c0c0(pos, up, target, mtx);
    if (load) {
        reg_G3X_MTX_MODE = G3X_MTXMODE_POSITION_VECTOR;
        G3_LoadMtx43(mtx);
    }
}

// Multiplies the current matrix by a rotation about the Z axis.
void func_0203d13c(fx32 sin, fx32 cos) {
    vu32 *fifo = (vu32 *) 0x04000468;
    *fifo = cos;
    *fifo = sin;
    *fifo = 0;
    *fifo = -sin;
    *fifo = cos;
    *fifo = 0;
    *fifo = 0;
    *fifo = 0;
    *fifo = 0x1000;
}
