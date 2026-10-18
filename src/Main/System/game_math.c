#include <Main/clib.h>
#include <Main/System/game_math.h>
#include <Main/System/engine_asm.h>
#include <nitro/os.h>

#define MATH_BUSY 0x8000

#define ANGLE_FULL 0x4000

// Quadratic Bezier with the weights in 1/1024 steps: a at t = 0, b at t = 1024, c as the control point.
int func_0201fab0(int a, int b, int c, int t) {
    int u = 1024 - t;
    int x = a * u + c * 2 * t;
    int y = u * x + b * t * t;
    return y / 1024 / 1024;
}

u32 func_0201fae4(int x, int y) {
    return x * x + y * y;
}

u32 func_0201faf0(int x, int y) {
    return func_0201fccc(func_0201fae4(x, y));
}

s64 func_0201fafc(int x, int y) {
    return (s64) x * x + (s64) y * y;
}

// Moves *value toward 0 (speed < 0) or ANGLE_FULL (speed > 0) by 1/|speed| of the distance when it is farther than
// 2 * |speed| away; otherwise it lands on the step directly.
void func_0201fb24(u16 *value, int speed) {
    int target;
    int diff;
    if (speed > 0) {
        target = ANGLE_FULL;
    } else {
        target = 0;
        speed = -speed;
    }
    diff = target - *value;
    if (abs(diff) > speed * 2) {
        diff = diff / speed;
    }
    *value += diff;
}

void func_0201fb5c(u16 *p, int add) {
    if (*p < ANGLE_FULL) {
        int sum = *p + add;
        if (sum < ANGLE_FULL) {
            *p = sum;
        } else {
            *p = ANGLE_FULL;
        }
    }
}

// Linear map of a from [0, b] to [c, d].
int func_0201fb74(int a, int b, int c, int d) {
    int r;
    int bitsRange;
    int bitsA;
    if (a == 0) {
        return c;
    }
    if (a == b) {
        return d;
    }
    bitsRange = 32 - func_02028250(abs(d - c));
    bitsA = 32 - func_02028250(a);
    if (bitsRange + bitsA > 30) {
        r = (s64) a * ((s64) d - c) / b;
    } else {
        r = func_0201fca8((d - c) * a, b);
    }
    return c + r;
}

// Linear map of v from [a, b] to [c, d].
int func_0201fbe4(int v, int a, int b, int c, int d) {
    if (a == b) {
        OS_Terminate();
    }
    return c + func_0201fca8((v - a) * (d - c), b - a);
}

char *func_0201fc08(int value, char *buf) {
    return func_0201fc14(value, buf, 0);
}

// Writes value in decimal to buf, with separator after the digit of each thousands place unless it is 0.
char *func_0201fc14(int value, char *buf, char separator) {
    int pow10[11] = {1000000000, 100000000, 10000000, 1000000, 100000, 10000, 1000, 100, 10, 1, 0};
    char *start = buf;
    int *p = pow10;
    u32 v = value;
    if (value < 0) {
        *buf++ = '-';
        v = -value;
    }
    while (*p > v) {
        p++;
    }
    if (*p == 0) {
        *start = '0';
        return start + 1;
    }
    do {
        int c = '0';
        while (v >= *p) {
            v -= *p;
            c++;
        }
        *buf++ = c;
        if (separator != 0 && (*p == 1000 || *p == 1000000 || *p == 1000000000)) {
            *buf++ = separator;
        }
        p++;
    } while (*p != 0);
    return buf;
}

// a / b rounded to nearest, saturated to 0x0fffffff (negated when a < 0) when b is 0.
int func_0201fca8(int a, int b) {
    int r;
    if (b == 0) {
        return 0x0fffffff ^ (a >> 31);
    }
    r = func_0201fd40(a * 2, b);
    return (r + (r >> 31) + 1) >> 1;
}

u32 func_0201fccc(u32 value) {
    *(volatile u16 *) 0x040002b0 = 0;
    *(volatile u32 *) 0x040002b8 = value;
    while (*(volatile u16 *) 0x040002b0 & MATH_BUSY) {
    }
    return *(volatile u32 *) 0x040002b4;
}

u32 func_0201fcec(u64 value) {
    *(volatile u16 *) 0x040002b0 = 1;
    *(volatile u64 *) 0x040002b8 = value;
    while (*(volatile u16 *) 0x040002b0 & MATH_BUSY) {
    }
    return *(volatile u32 *) 0x040002b4;
}

u32 func_0201fd10(u32 numerator, u32 denominator, u32 *remainder) {
    *(volatile u16 *) 0x04000280 = 0;
    *(volatile u32 *) 0x04000290 = numerator;
    *(volatile u64 *) 0x04000298 = denominator;
    while (*(volatile u16 *) 0x04000280 & MATH_BUSY) {
    }
    *remainder = *(volatile u32 *) 0x040002a8;
    return *(volatile u32 *) 0x040002a0;
}

s32 func_0201fd40(s32 numerator, s32 denominator) {
    *(volatile u16 *) 0x04000280 = 0;
    *(volatile u32 *) 0x04000290 = numerator;
    *(volatile u64 *) 0x04000298 = (u32) denominator;
    while (*(volatile u16 *) 0x04000280 & MATH_BUSY) {
    }
    return *(volatile u32 *) 0x040002a0;
}
