#include <stdlib.h>

#define SWAP_BYTES(x, y, sz)                                                                                           \
    do {                                                                                                               \
        size_t k = (sz);                                                                                               \
        char *sy = (y);                                                                                                \
        char *sx = (x);                                                                                                \
        for (; k != 0; k--) {                                                                                          \
            char t = *sx;                                                                                              \
            *sx++ = *sy;                                                                                               \
            *sy++ = t;                                                                                                 \
        }                                                                                                              \
    } while (0)

/* heap sort */
void qsort(void *base, size_t count, size_t size, int (*cmp)(const void *, const void *)) {
    char *a = base;
    size_t l, n = count, j;
    char *p, *c, *lp, *rp;

    if (n < 2) {
        return;
    }
    l = (n >> 1) + 1;
    lp = a + size * (l - 1);
    rp = a + size * (n - 1);
    for (;;) {
        if (l > 1) {
            l--;
            lp -= size;
        } else {
            SWAP_BYTES(lp, rp, size);
            n--;
            if (n == 1) {
                return;
            }
            rp -= size;
        }
        j = l;
        c = a + size * (j - 1);
        while (2 * j <= n) {
            p = c;
            j *= 2;
            c = a + size * (j - 1);
            if (j < n && cmp(c, c + size) < 0) {
                j++;
                c += size;
            }
            if (cmp(p, c) >= 0) {
                break;
            }
            SWAP_BYTES(p, c, size);
        }
    }
}
