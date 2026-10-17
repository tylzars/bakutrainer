#include <math.h>
#include <runtime.h>

#define __HI(x) *(1 + (int *) &x)
#define __LO(x) *(int *) &x

int func_0204bf28(double x) {
    return __HI(x) & 0x80000000;
}

int func_0204bf3c(double x) {
    int hx = __HI(x);
    int e = hx & 0x7ff00000;
    switch (e) {
        case 0x7ff00000:
            return ((hx & 0xfffff) != 0 || __LO(x) != 0) ? FP_NAN : FP_INFINITE;
        case 0:
            return ((hx & 0xfffff) != 0 || __LO(x) != 0) ? FP_SUBNORMAL : FP_ZERO;
    }
    return FP_NORMAL;
}
