#include <string.h>

size_t strlen(const char *s) {
    int n = -1;
    do {
        n++;
    } while (*s++);
    return n;
}

char *strcpy(char *dst, const char *src) {
    unsigned char c;
    unsigned sa = (unsigned) src & 3;
    unsigned da = (unsigned) dst & 3;
    unsigned k;
    const unsigned char *s = (const unsigned char *) src;
    unsigned char *d = (unsigned char *) dst;
    if (da == sa) {
        if (sa != 0) {
            c = *s;
            *d = c;
            if (c == 0) {
                return dst;
            }
            k = 3 - sa;
            if (k != 0) {
                do {
                    c = *++s;
                    *++d = c;
                    if (c == 0) {
                        return dst;
                    }
                } while (--k);
            }
            d++;
            s++;
        }
        {
            unsigned w = *(const unsigned *) s;
            if (!(((w + 0xfefefeff) & ~w) & 0x80808080)) {
                d -= 4;
                do {
                    unsigned t;
                    *(unsigned *) (d += 4) = w;
                    w = *(const unsigned *) (s += 4);
                    t = w + 0xfefefeff;
                    t = t & ~w;
                    if (t & 0x80808080) {
                        break;
                    }
                } while (1);
                d += 4;
            }
        }
    }
    c = *s;
    *d = c;
    if (c == 0) {
        return dst;
    }
    do {
        c = *++s;
        *++d = c;
    } while (c != 0);
    return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
    char *d = dst;
    if (n == 0) {
        return dst;
    }
    do {
        if ((*d++ = *src++) == 0) {
            if (--n == 0) {
                return dst;
            }
            do {
                *d++ = 0;
            } while (--n);
            return dst;
        }
    } while (--n);
    return dst;
}

char *strchr(const char *s, int c) {
    char ch = c;
    char v = *s++;
    if (v) {
        do {
            if (v == ch) {
                return (char *) (s - 1);
            }
            v = *s++;
        } while (v);
    }
    if (ch != 0) {
        return 0;
    }
    return (char *) (s - 1);
}
