#include <nitro/math.h>
#include <nitro/fx.h>

void func_0204882c(fx16 *sinTable, u32 nShift) {
    u32 n = 1 << nShift;
    u32 i;
    u32 count;
    s32 angle;

    count = (n >> 1) + (n >> 2);
    for (i = 0, angle = 0; i < count; i++, angle += 0x10000u >> nShift) {
        sinTable[i] = FX_SinCosTable_[(angle >> 4) * 2];
    }
}

void func_02048880(fx32 *data, u32 nShift, const fx16 *sinTable) {
    u32 rev;
    u32 quarter;
    u32 shift;
    u32 i;
    u32 step;
    u32 k;
    u32 n2;
    u32 angle;
    s32 top;
    u32 m;
    u32 n;

    n = 1 << nShift;
    n2 = n << 1;
    quarter = n >> 2;
    step = n;
    rev = 0;
    top = 0x80000000;
    shift = 31 - nShift;

    for (i = 0; i < n2; i += 2) {
        u32 r = rev >> shift;
        if (i < r) {
            fx32 t = data[i];
            data[i] = data[r];
            data[r] = t;
            t = data[i + 1];
            data[i + 1] = data[r + 1];
            data[r + 1] = t;
        }
        rev ^= top >> CountLeadingZeros(~rev);
    }

    for (m = 2; m < n2; m <<= 1) {
        step >>= 1;
        for (i = 0; i < n2; i += m << 1) {
            u32 j = i + m;
            fx32 c = data[i + 1];
            fx32 a = data[i];
            fx32 b = data[j];
            fx32 d = data[j + 1];
            data[i] = a + b;
            data[i + 1] = c + d;
            data[j] = a - b;
            data[j + 1] = c - d;
        }
        angle = step;
        for (k = 2; k < m; k += 2) {
            if (k != (m >> 1)) {
                fx32 cs = sinTable[angle + quarter];
                fx32 sn = sinTable[angle];
                fx32 csMinusSn, csPlusSn;
                angle += step;
                csMinusSn = cs + -sn;
                csPlusSn = cs - -sn;
                for (i = k; i < n2; i += m << 1) {
                    u32 j = i + m;
                    fx32 x = data[j];
                    fx32 y = data[j + 1];
                    fx32 re = data[i];
                    fx32 p = FX_Mul(cs, x + y);
                    fx32 im = data[i + 1];
                    fx32 tr = p - FX_Mul(csMinusSn, y);
                    fx32 ti = p - FX_Mul(csPlusSn, x);
                    data[i] = re + tr;
                    data[i + 1] = im + ti;
                    data[j] = re - tr;
                    data[j + 1] = im - ti;
                }
            } else {
                angle += step;
                for (i = k; i < n2; i += m << 1) {
                    u32 j = i + m;
                    fx32 a = data[i];
                    fx32 b = data[i + 1];
                    fx32 c = data[j];
                    fx32 d = data[j + 1];
                    data[i] = a + d;
                    data[i + 1] = b - c;
                    data[j] = a - d;
                    data[j + 1] = b + c;
                }
            }
        }
    }
}

/* runs the transform, then divides every element by 2^nShift */
void func_02048af8(fx32 *data, u32 nShift, const fx16 *sinTable) {
    u32 n2 = (1 << nShift) << 1;
    u32 i;

    func_02048880(data, nShift, sinTable);
    for (i = 0; i < n2; i++) {
        data[i] >>= nShift;
    }
}
