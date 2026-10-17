#include <nitro/fx.h>

#define REG_DIV_NUMER (*(u64 *) 0x04000290)
#define REG_DIV_DENOM (*(u64 *) 0x04000298)

// Rounded product of a 32-bit value and a 64-bit reciprocal (32 fractional bits), as 20.12 fixed point.
static inline fx32 FX_Mul32x64c(fx32 a, fx64 b) {
    return (fx32) ((b * (fx64) a + 0x80000000LL) >> 32);
}

// Perspective projection from the sine and cosine of the field of view; scale is the w scale (0x1000 = 1.0).
void func_0203c1c8(fx32 sinFov, fx32 cosFov, fx32 aspect, fx32 n, fx32 f, fx32 scale, MtxFx44 *mtx) {
    fx32 cot;
    fx64 val;
    fx32 tmp;
    cot = FX_Div(cosFov, sinFov);
    REG_DIV_NUMER = (u64) 0x1000 << 32;
    REG_DIV_DENOM = (u32) (n - f);
    if (scale != 0x1000) {
        cot = (cot * scale) / 0x1000;
    }
    mtx->_[1] = 0;
    mtx->_[2] = 0;
    mtx->_[3] = 0;
    mtx->_[4] = 0;
    mtx->_[5] = cot;
    mtx->_[6] = 0;
    mtx->_[7] = 0;
    mtx->_[8] = 0;
    mtx->_[9] = 0;
    mtx->_[11] = -scale;
    mtx->_[12] = 0;
    mtx->_[13] = 0;
    mtx->_[15] = 0;
    val = FX_GetDivResultFx64c();
    REG_DIV_NUMER = (u64) (u32) cot << 32;
    REG_DIV_DENOM = (u32) aspect;
    if (scale != 0x1000) {
        val = (val * scale) / 0x1000;
    }
    mtx->_[10] = FX_Mul32x64c(f + n, val);
    tmp = FX_Mul(n * 2, f);
    mtx->_[14] = FX_Mul32x64c(tmp, val);
    mtx->_[0] = FX_GetDivResult();
}

// Orthographic projection from the six clip planes; scale is the w scale (0x1000 = 1.0).
void func_0203c314(fx32 t, fx32 b, fx32 l, fx32 r, fx32 n, fx32 f, fx32 scale, MtxFx44 *mtx) {
    fx64 widthInv;
    fx64 heightInv;
    fx64 depthInv;
    FX_InvAsync(r - l);
    mtx->_[1] = 0;
    mtx->_[2] = 0;
    mtx->_[3] = 0;
    mtx->_[4] = 0;
    mtx->_[6] = 0;
    mtx->_[7] = 0;
    mtx->_[8] = 0;
    mtx->_[9] = 0;
    mtx->_[11] = 0;
    mtx->_[15] = scale;
    widthInv = FX_GetDivResultFx64c();
    REG_DIV_NUMER = (u64) 0x1000 << 32;
    REG_DIV_DENOM = (u32) (t - b);
    if (scale != 0x1000) {
        widthInv = (widthInv * scale) / 0x1000;
    }
    mtx->_[0] = FX_Mul32x64c(0x2000, widthInv);
    heightInv = FX_GetDivResultFx64c();
    REG_DIV_NUMER = (u64) 0x1000 << 32;
    REG_DIV_DENOM = (u32) (n - f);
    if (scale != 0x1000) {
        heightInv = (heightInv * scale) / 0x1000;
    }
    mtx->_[5] = FX_Mul32x64c(0x2000, heightInv);
    depthInv = FX_GetDivResultFx64c();
    if (scale != 0x1000) {
        depthInv = (depthInv * scale) / 0x1000;
    }
    mtx->_[10] = FX_Mul32x64c(0x2000, depthInv);
    mtx->_[12] = FX_Mul32x64c(-(r + l), widthInv);
    mtx->_[13] = FX_Mul32x64c(-(t + b), heightInv);
    mtx->_[14] = FX_Mul32x64c(f + n, depthInv);
}
