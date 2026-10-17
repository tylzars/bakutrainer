#include <nitro/fx.h>

#define REG_DIVCNT (*(vu16 *) 0x04000280)
#define REG_DIV_NUMER (*(vs64 *) 0x04000290)
#define REG_DIV_DENOM (*(vs64 *) 0x04000298)
#define REG_DIV_RESULT (*(s64 *) 0x040002a0)

#define DIVCNT_BUSY 0x8000
#define DIVCNT_MODE_64_32 1

fx32 FX_Div(fx32 num, fx32 den) {
    FX_DivAsync(num, den);
    return FX_GetDivResult();
}

fx64 FX_GetDivResultFx64c(void) {
    while (REG_DIVCNT & DIVCNT_BUSY) {
    }
    return REG_DIV_RESULT;
}

fx32 FX_GetDivResult(void) {
    s64 r;
    while (REG_DIVCNT & DIVCNT_BUSY) {
    }
    r = REG_DIV_RESULT;
    return (fx32) ((r + 0x80000) >> 20);
}

void FX_InvAsync(fx32 x) {
    REG_DIVCNT = DIVCNT_MODE_64_32;
    REG_DIV_NUMER = 0x1000LL << 32;
    REG_DIV_DENOM = (u32) x;
}

void FX_DivAsync(fx32 num, fx32 den) {
    REG_DIVCNT = DIVCNT_MODE_64_32;
    REG_DIV_NUMER = (s64) num << 32;
    REG_DIV_DENOM = (u32) den;
}
