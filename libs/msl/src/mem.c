#include <string.h>

/* fills n bytes with the low byte of val, a word at a time when n is large */
extern void func_02048cdc(void *dst, int val, size_t n);

void *memcpy(void *dst, const void *s, size_t n) {
    char *d = dst;
    const char *src = s;
    if (n) {
        do {
            *d++ = *src++;
        } while (--n);
    }
    return dst;
}

void *memset(void *dst, int c, size_t n) {
    func_02048cdc(dst, c, n);
    return dst;
}

void *memchr(const void *p, int c, size_t n) {
    const unsigned char *s = p;
    unsigned char ch = c;
    if (n) {
        do {
            if (*s++ == ch) {
                return (void *) (s - 1);
            }
        } while (--n);
    }
    return 0;
}

int memcmp(const void *a, const void *b, size_t n) {
    const unsigned char *p = a, *q = b;
    if (n) {
        do {
            if (*p++ != *q++) {
                return (p[-1] < q[-1]) ? -1 : 1;
            }
        } while (--n);
    }
    return 0;
}

void func_02048cdc(void *dst, int val, size_t n) {
    unsigned char *p = dst;
    unsigned v = (unsigned char) val;
    if (n >= 32) {
        unsigned a = -(unsigned) p & 3;
        if (a) {
            n -= a;
            do {
                *p++ = (unsigned char) v;
            } while (--a);
        }
        if (v) {
            v = (v << 24) | (v << 16) | (v << 8) | v;
        }
        {
            unsigned blocks = n >> 5;
            unsigned *wp = (unsigned *) p;
            if (blocks) {
                do {
                    wp[0] = v;
                    wp[1] = v;
                    wp[2] = v;
                    wp[3] = v;
                    wp[4] = v;
                    wp[5] = v;
                    wp[6] = v;
                    wp[7] = v;
                    wp += 8;
                } while (--blocks);
            }
            blocks = (n & 0x1f) >> 2;
            if (blocks) {
                do {
                    *wp++ = v;
                } while (--blocks);
            }
            p = (unsigned char *) wp;
        }
        n &= 3;
    }
    if (n == 0) {
        return;
    }
    do {
        *p++ = (unsigned char) v;
    } while (--n);
}
