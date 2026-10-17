#pragma once

#define FP_NAN 1
#define FP_INFINITE 2
#define FP_ZERO 3
#define FP_NORMAL 4
#define FP_SUBNORMAL 5

#ifdef __cplusplus
extern "C" {
#endif

double copysign(double x, double y);
double fabs(double x);
double frexp(double x, int *eptr);
double scalbn(double x, int n);

#ifdef __cplusplus
}
#endif
