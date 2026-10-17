#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int vsnprintf(char *buf, size_t size, const char *format, char *args);
int vsprintf(char *buf, const char *format, char *args);
int sprintf(char *buf, const char *format, ...);

#ifdef __cplusplus
}
#endif
