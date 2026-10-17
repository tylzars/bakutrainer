#pragma once

#include <stddef.h>

#ifndef __cplusplus
    #ifndef _WCHAR_T_DEFINED
        #define _WCHAR_T_DEFINED
typedef unsigned short wchar_t;
    #endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

int abs(int x);
int mbtowc(wchar_t *pwc, const char *s, size_t n);
int wctomb(char *s, wchar_t wc);
size_t wcstombs(char *dst, const wchar_t *src, size_t n);
void free(void *ptr);
void qsort(void *base, size_t count, size_t size, int (*cmp)(const void *, const void *));

#ifdef __cplusplus
}
#endif
