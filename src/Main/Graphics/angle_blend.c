#include "types.h"
#include <Main/System/random_angle.h>
#include <Main/Graphics/angle_blend.h>

#pragma thumb off

void BlendAngles(u32 t, u32 shift, u16 *cur, const u16 *target) {
    int d[3];
    int e[3];
    u16 alt[3];
    u32 i;
    int a;
    int b;
    int c;

    d[0] = AngleDelta(cur[0], target[0]);
    d[1] = AngleDelta(cur[1], target[1]);
    d[2] = AngleDelta(cur[2], target[2]);
    b = d[1];
    a = d[0];
    c = d[2];
    if (((a | b | c) & 0x7fff) == 0) {
        return;
    }
    if (a < 0) {
        a = -a;
    }
    if (b < 0) {
        b = -b;
    }
    if (c < 0) {
        c = -c;
    }
    if (a > 0x2800 || b > 0x2800 || c > 0x2800) {
        alt[0] = cur[0] + 0x8000;
        alt[1] = 0x8000 - cur[1];
        alt[2] = cur[2] + 0x8000;
        e[0] = AngleDelta(alt[0], target[0]);
        e[1] = AngleDelta(alt[1], target[1]);
        e[2] = AngleDelta(alt[2], target[2]);
        if ((e[0] < 0 ? -e[0] : e[0]) + (e[1] < 0 ? -e[1] : e[1]) + (e[2] < 0 ? -e[2] : e[2]) < a + b + c) {
            for (i = 0; i < 3; i++) {
                s16 q;
                q = (t * e[i]) >> shift;
                cur[i] = alt[i] + q;
            }
            return;
        }
    }
    for (i = 0; i < 3; i++) {
        s16 q;
        q = (t * d[i]) >> shift;
        cur[i] = cur[i] + q;
    }
}

void BlendVec3(int t, int shift, s32 *dst, const s32 *target) {
    dst[0] = dst[0] + ((t * (target[0] - dst[0])) >> shift);
    dst[1] = dst[1] + ((t * (target[1] - dst[1])) >> shift);
    dst[2] = dst[2] + ((t * (target[2] - dst[2])) >> shift);
}

#pragma thumb on
