#include <stdlib.h>
#include <string.h>
#include <runtime.h>

int mbtowc(wchar_t *pwc, const char *s, size_t n) {
    if (s == 0) {
        return 0;
    }
    if (n == 0) {
        return -1;
    }
    if (pwc) {
        *pwc = *(unsigned char *) s;
    }
    if (*s == 0) {
        return 0;
    }
    return 1;
}

int wctomb(char *s, wchar_t wc) {
    if (s == 0) {
        return 0;
    }
    *s = wc;
    return 1;
}

/* wide char to multibyte through the active locale's converter */
int func_02048ba8(char *s, wchar_t wc) {
    return data_02064520.conv->wctomb(s, wc);
}

size_t wcstombs(char *dst, const wchar_t *src, size_t n) {
    char tmp[4];
    size_t len = 0;
    size_t cl;
    if (dst == 0 || src == 0) {
        return 0;
    }
    while (1) {
        if (*src == 0) {
            dst[len] = 0;
            break;
        }
        cl = func_02048ba8(tmp, *src++);
        if (len + cl > n) {
            break;
        }
        strncpy(dst + len, tmp, cl);
        len += cl;
        if (len > n) {
            break;
        }
    }
    return len;
}
