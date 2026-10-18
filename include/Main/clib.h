#pragma once

#include "types.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// C library functions the game calls. Game code takes these declarations from here, not from <string.h>: the compiler
// treats memset and memcpy declared in a header named string.h as built-ins and expands small calls inline, while the
// game's code calls them.
void *memcpy(void *dst, const void *src, size_t n);
void *memset(void *dst, int c, size_t n);
char *strcpy(char *dst, const char *src);
int abs(int x);
void qsort(void *base, size_t count, size_t size, int (*compare)(const void *, const void *));

#ifdef __cplusplus
}
#endif
