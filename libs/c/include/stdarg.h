#pragma once

// Variable arguments: the caller's register arguments are stored next to the stacked ones, so the list starts at the
// first word after the last named parameter.
typedef char *va_list;

#define va_start(ap, parm) ((ap) = (va_list) (((unsigned) &(parm) & ~3) + 4))
