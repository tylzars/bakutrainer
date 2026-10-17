#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <runtime.h>

typedef struct FmtSpec {
    /* 00 */ unsigned char justify;   /* 0 left, 1 right, 2 right with zero fill */
    /* 01 */ unsigned char sign;      /* 0 none, 1 '+', 2 ' ' */
    /* 02 */ unsigned char prec_set;
    /* 03 */ unsigned char alt;
    /* 04 */ unsigned char len;       /* length modifier */
    /* 05 */ unsigned char conv;      /* conversion char, 0xff if invalid */
    /* 08 */ int width;
    /* 0c */ int prec;
} FmtSpec;

#define ISDIGIT(c) (((c) < 0 || (c) >= 128) ? 0 : (data_020637fc[c] & 8))
#define ISUPPER(c) (((c) < 0 || (c) >= 128) ? 0 : (data_020637fc[c] & 0x200))

/* parse one conversion spec after '%'; returns the char after it */
const char *func_02048d8c(const char *fmt, char **ap, FmtSpec *out) {
    FmtSpec s;
    const char *p = fmt + 1;
    int c = fmt[1];
    int more;
    int w;
    s.justify = 1;
    s.sign = 0;
    s.prec_set = 0;
    s.alt = 0;
    s.len = 0;
    s.width = 0;
    s.prec = 0;
    if (c == '%') {
        s.conv = c;
        *out = s;
        return p + 1;
    }
    do {
        more = 1;
        switch (c) {
            case '-':
                s.justify = 0;
                break;
            case '+':
                s.sign = 1;
                break;
            case ' ':
                if (s.sign != 1) {
                    s.sign = 2;
                }
                break;
            case '#':
                s.alt = 1;
                break;
            case '0':
                if (s.justify != 0) {
                    s.justify = 2;
                }
                break;
            default:
                more = 0;
                break;
        }
        if (!more) {
            break;
        }
        c = *++p;
    } while (1);
    if (c == '*') {
        w = *(int *) ((*ap += 4) - 4);
        s.width = w;
        if (w < 0) {
            s.justify = 0;
            s.width = -w;
        }
        c = *++p;
    } else {
        while (ISDIGIT(c)) {
            s.width = s.width * 10 + (c - '0');
            c = *++p;
        }
    }
    if (s.width > 509) {
        s.conv = 0xff;
        *out = s;
        return p + 1;
    }
    if (c == '.') {
        c = *++p;
        s.prec_set = 1;
        if (c == '*') {
            s.prec = *(int *) ((*ap += 4) - 4);
            c = *++p;
            if (s.prec < 0) {
                s.prec_set = 0;
            }
        } else {
            while (ISDIGIT(c)) {
                s.prec = s.prec * 10 + (c - '0');
                c = *++p;
            }
        }
    }
    more = 1;
    switch (c) {
        case 'h':
            s.len = 2;
            if (p[1] == 'h') {
                s.len = 1;
                c = *++p;
            }
            break;
        case 'l':
            s.len = 3;
            if (p[1] == 'l') {
                s.len = 4;
                c = *++p;
            }
            break;
        case 'L':
            s.len = 9;
            break;
        case 'j':
            s.len = 6;
            break;
        case 't':
            s.len = 8;
            break;
        case 'z':
            s.len = 7;
            break;
        default:
            more = 0;
            break;
    }
    if (more) {
        c = *++p;
    }
    s.conv = c;
    switch (c) {
        case 'd':
        case 'i':
        case 'o':
        case 'u':
        case 'x':
        case 'X':
            if (s.len == 9) {
                s.conv = 0xff;
            } else if (!s.prec_set) {
                s.prec = 1;
            } else if (s.justify == 2) {
                s.justify = 1;
            }
            break;
        case 'f':
        case 'F':
            if (s.len == 2 || s.len == 6 || s.len == 7 || s.len == 8 || s.len == 4) {
                s.conv = 0xff;
            } else if (!s.prec_set) {
                s.prec = 6;
            }
            break;
        case 'a':
        case 'A':
            if (!s.prec_set) {
                s.prec = 13;
            }
            if (s.len == 2 || s.len == 6 || s.len == 7 || s.len == 8 || s.len == 4 || s.len == 1) {
                s.conv = 0xff;
            }
            break;
        case 'g':
        case 'G':
            if (s.prec == 0) {
                s.prec = 1;
            }
            /* fall through */
        case 'e':
        case 'E':
            if (s.len == 2 || s.len == 6 || s.len == 7 || s.len == 8 || s.len == 4 || s.len == 1) {
                s.conv = 0xff;
            } else if (!s.prec_set) {
                s.prec = 6;
            }
            break;
        case 'p':
            s.conv = 'x';
            s.alt = 1;
            s.len = 3;
            s.prec = 8;
            break;
        case 'c':
            if (s.len == 3) {
                s.len = 5;
            } else if (s.prec_set != 0 || s.len != 0) {
                s.conv = 0xff;
            }
            break;
        case 's':
            if (s.len == 3) {
                s.len = 5;
            } else if (s.len != 0) {
                s.conv = 0xff;
            }
            break;
        case 'n':
            if (s.len == 9) {
                s.conv = 0xff;
            }
            break;
        default:
            s.conv = 0xff;
            break;
    }
    *out = s;
    return p + 1;
}

/* integer to digits, written backwards from end; returns start (0 if too long) */
char *func_0204931c(int val, char *end, FmtSpec spec) {
    unsigned u, base;
    char *p;
    int digit, count;
    int neg = 0;
    u = val;
    neg = 0;
    p = end;
    *--p = 0;
    count = 0;
    if (val == 0 && spec.prec == 0 && !(spec.alt && spec.conv == 'o')) {
        return p;
    }
    switch (spec.conv) {
        case 'd':
        case 'i':
            base = 10;
            if (val < 0) {
                if (val != (int) 0x80000000) {
                    u = -val;
                }
                neg = 1;
            }
            break;
        case 'o':
            base = 8;
            spec.sign = 0;
            break;
        case 'u':
            base = 10;
            spec.sign = 0;
            break;
        case 'x':
        case 'X':
            base = 16;
            spec.sign = 0;
            break;
    }
    do {
        digit = u % base;
        u /= base;
        if (digit < 10) {
            digit += '0';
        } else {
            digit -= 10;
            if (spec.conv == 'x') {
                digit += 'a';
            } else {
                digit += 'A';
            }
        }
        *--p = digit;
        ++count;
    } while (u != 0);
    if (base == 8 && spec.alt && *p != '0') {
        *--p = '0';
        ++count;
    }
    if (spec.justify == 2) {
        spec.prec = spec.width;
        if (neg || spec.sign != 0) {
            --spec.prec;
        }
        if (base == 16 && spec.alt) {
            spec.prec -= 2;
        }
    }
    if (end - p + spec.prec > 509) {
        return 0;
    }
    while (count < spec.prec) {
        *--p = '0';
        ++count;
    }
    if (base == 16 && spec.alt) {
        *--p = spec.conv;
        *--p = '0';
    }
    if (neg) {
        *--p = '-';
    } else if (spec.sign == 1) {
        *--p = '+';
    } else if (spec.sign == 2) {
        *--p = ' ';
    }
    return p;
}

/* the same for 64-bit values */
char *func_02049568(long long val, char *end, FmtSpec spec) {
    unsigned long long u, base;
    char *p;
    int digit, count;
    int neg = 0;
    u = val;
    neg = 0;
    p = end;
    *--p = 0;
    count = 0;
    if (val == 0 && spec.prec == 0 && !(spec.alt && spec.conv == 'o')) {
        return p;
    }
    switch (spec.conv) {
        case 'd':
        case 'i':
            base = 10;
            if (val < 0) {
                if (val != (long long) 0x8000000000000000ULL) {
                    u = -val;
                }
                neg = 1;
            }
            break;
        case 'o':
            base = 8;
            spec.sign = 0;
            break;
        case 'u':
            base = 10;
            spec.sign = 0;
            break;
        case 'x':
        case 'X':
            base = 16;
            spec.sign = 0;
            break;
    }
    do {
        digit = u % base;
        u /= base;
        if (digit < 10) {
            digit += '0';
        } else {
            digit -= 10;
            if (spec.conv == 'x') {
                digit += 'a';
            } else {
                digit += 'A';
            }
        }
        *--p = digit;
        ++count;
    } while (u != 0);
    if (base == 8 && spec.alt && *p != '0') {
        *--p = '0';
        ++count;
    }
    if (spec.justify == 2) {
        spec.prec = spec.width;
        if (neg || spec.sign != 0) {
            --spec.prec;
        }
        if (base == 16 && spec.alt) {
            spec.prec -= 2;
        }
    }
    if (end - p + spec.prec > 509) {
        return 0;
    }
    while (count < spec.prec) {
        *--p = '0';
        ++count;
    }
    if (base == 16 && spec.alt) {
        *--p = spec.conv;
        *--p = '0';
    }
    if (neg) {
        *--p = '-';
    } else if (spec.sign == 1) {
        *--p = '+';
    } else if (spec.sign == 2) {
        *--p = ' ';
    }
    return p;
}

/* double to "0x1.xxxp+e" (hex float), inf and nan; digits written backwards from end */
#define XB ((unsigned char *) &x)
char *func_02049864(double x, char *end, FmtSpec spec) {
    char *p = end;
    DecForm f;
    FmtSpec e;
    DecDigits d;
    unsigned char *xb;
    int i, n, bit, ex;
    double ld;
    unsigned snum;
    char *q;
    ld = x;
    if (spec.prec > 509) {
        return 0;
    }
    f.mode = 0;
    f.ndigits = 32;
    func_0204bdec(&f, ld, &d);
    if (0.0 == fabs(x)) {
        p = end - 6;
        __memcpy(p, "0x0p0", 6);
        return p;
    }
    if (d.text[0] == 'I') {
        if (d.sign) {
            p = end - 5;
            if (spec.conv == 'A') {
                __memcpy(p, "-INF", 5);
            } else {
                __memcpy(p, "-inf", 5);
            }
        } else {
            p = end - 4;
            if (spec.conv == 'A') {
                __memcpy(p, "INF", 4);
            } else {
                __memcpy(p, "inf", 4);
            }
        }
        return p;
    }
    if (d.text[0] == 'N') {
        if (d.sign) {
            p = end - 5;
            if (spec.conv == 'A') {
                __memcpy(p, "-NAN", 5);
            } else {
                __memcpy(p, "-nan", 5);
            }
        } else {
            p = end - 4;
            if (spec.conv == 'A') {
                __memcpy(p, "NAN", 4);
            } else {
                __memcpy(p, "nan", 4);
            }
        }
        return p;
    }
    e.justify = 1;
    e.sign = 1;
    e.prec_set = 0;
    e.alt = 0;
    e.len = 0;
    e.width = 0;
    e.prec = 1;
    e.conv = 'd';
    q = (char *) &x;
    for (i = 0; i < 4; i++) {
        char t = q[i];
        q[i] = q[7 - i];
        q[7 - i] = t;
    }
    snum = XB[0] << 25;
    snum |= XB[1] << 17;
    snum = (snum >> 21) & 0x7ff;
    ex = snum - 1023;
    p = func_0204931c(ex, p, e);
    if (spec.conv == 'a') {
        *--p = 'p';
    } else {
        *--p = 'P';
    }
    xb = (unsigned char *) &x;
    bit = spec.prec * 4 + 11;
    for (n = spec.prec; n >= 1; n--) {
        unsigned char ch;
        if (bit < 64) {
            unsigned char v = xb[bit >> 3] >> (7 - (bit & 7));
            if ((bit & ~7) != ((bit - 4) & ~7)) {
                v = (v | (xb[(bit >> 3) - 1] << 8) >> (7 - (bit & 7))) & 0xff;
            }
            v &= 0xf;
            if (v < 10) {
                ch = v + '0';
            } else if (spec.conv == 'a') {
                ch = v + 'a' - 10;
            } else {
                ch = v + 'A' - 10;
            }
        } else {
            ch = '0';
        }
        *--p = ch;
        bit -= 4;
    }
    if (spec.prec != 0 || spec.alt != 0) {
        *--p = '.';
    }
    *--p = '1';
    if (spec.conv == 'a') {
        *--p = 'x';
    } else {
        *--p = 'X';
    }
    *--p = '0';
    if (d.sign) {
        *--p = '-';
    } else if (spec.sign == 1) {
        *--p = '+';
    } else if (spec.sign == 2) {
        *--p = ' ';
    }
    return p;
}

/* round a DecDigits digit string to n digits (half to even) */
void func_02049d40(DecDigits *d, int n) {
    char *t;
    int len;
    char *p;
    int carry;
    int c;
    if (n < 0) {
    zero:
        d->exp = 0;
        d->len = 1;
        d->text[0] = '0';
        return;
    }
    len = d->len;
    if (n >= len) {
        return;
    }
    t = d->text;
    p = t + n + 1;
    c = (char) (*--p - '0');
    if (c == 5) {
        char *q = t + len;
        do {
            q--;
        } while (q > p && *q == '0');
        if (q == p) {
            carry = p[-1] & 1;
        } else {
            carry = 1;
        }
    } else {
        carry = c > 5;
    }
    while (n != 0) {
        c = (char) (*--p - '0' + carry);
        carry = c > 9;
        if (carry != 0 || c == 0) {
            n--;
        } else {
            *p = c + '0';
            break;
        }
    }
    if (carry) {
        d->exp++;
        d->len = 1;
        d->text[0] = '1';
        return;
    }
    if (n == 0) {
        goto zero;
    }
    d->len = n;
}

/* double to DecDigits digits for %e %f %g, written backwards from end; returns start (0 if too long) */
char *func_02049e64(double x, char *end, FmtSpec spec) {
    int sign;
    int alt;
    int prec;
    int conv;
    char *p;
    char *q;
    DecForm f;
    DecDigits d;
    int ex, n, i, k;
    char esign;
    int whole;   /* digits before the point (%f) */
    int frac;    /* digits after the point (%f) */
    alt = spec.alt;
    sign = spec.sign;
    prec = spec.prec;
    conv = spec.conv;
    if (prec > 509) {
        return 0;
    }
    f.mode = 0;
    f.ndigits = 32;
    func_0204bdec(&f, x, &d);
    q = d.text + d.len;
    while (d.len > 1 && *--q == '0') {
        d.len--;
        d.exp++;
    }
    switch (d.text[0]) {
        case '0':
            d.exp = 0;
            break;
        case 'I':
            if (x < 0.0) {
                p = end - 5;
                if (ISUPPER(conv)) {
                    __memcpy(p, "-INF", 5);
                } else {
                    __memcpy(p, "-inf", 5);
                }
            } else {
                p = end - 4;
                if (ISUPPER(conv)) {
                    __memcpy(p, "INF", 4);
                } else {
                    __memcpy(p, "inf", 4);
                }
            }
            return p;
        case 'N':
            if (d.sign) {
                p = end - 5;
                if (ISUPPER(conv)) {
                    __memcpy(p, "-NAN", 5);
                } else {
                    __memcpy(p, "-nan", 5);
                }
            } else {
                p = end - 4;
                if (ISUPPER(conv)) {
                    __memcpy(p, "NAN", 4);
                } else {
                    __memcpy(p, "nan", 4);
                }
            }
            return p;
    }
    p = end;
    d.exp += d.len - 1;
    *--p = 0;
    switch (conv) {
        case 'g':
        case 'G':
            if (d.len > prec) {
                func_02049d40(&d, prec);
            }
            if (d.exp < -4 || d.exp >= prec) {
                if (alt) {
                    prec--;
                } else {
                    prec = d.len - 1;
                }
                if (spec.conv == 'g') {
                    spec.conv = 'e';
                } else {
                    spec.conv = 'E';
                }
                goto do_e;
            } else {
                if (alt) {
                    prec -= d.exp + 1;
                } else {
                    prec = d.len - (d.exp + 1);
                    if (prec < 0) {
                        prec = 0;
                    }
                }
                goto do_f;
            }
        case 'e':
        case 'E':
        do_e:
            if (d.len > prec + 1) {
                func_02049d40(&d, prec + 1);
            }
            ex = d.exp;
            esign = '+';
            n = 0;
            if (ex < 0) {
                ex = -ex;
                esign = '-';
            }
            while (ex != 0 || n < 2) {
                *--p = ex % 10 + '0';
                ex /= 10;
                n++;
            }
            *--p = esign;
            *--p = spec.conv;
            if ((end - p) + prec > 509) {
                return 0;
            }
            if (d.len < prec + 1) {
                for (i = prec + 2 - d.len; --i != 0;) {
                    *--p = '0';
                }
            }
            q = d.text + d.len;
            for (i = d.len - 1; i != 0; i--) {
                *--p = *--q;
            }
            if (prec != 0 || alt != 0) {
                *--p = '.';
            }
            *--p = d.text[0];
            if (d.sign) {
                *--p = '-';
            } else if (sign == 1) {
                *--p = '+';
            } else if (sign == 2) {
                *--p = ' ';
            }
            break;
        case 'f':
        case 'F':
        do_f:
            frac = d.len - d.exp - 1;
            if (frac < 0) {
                frac = 0;
            }
            if (frac > prec) {
                func_02049d40(&d, d.len - (frac - prec));
                frac = d.len - d.exp - 1;
                if (frac < 0) {
                    frac = 0;
                }
            }
            whole = d.exp + 1;
            if (whole < 0) {
                whole = 0;
            }
            if (whole + frac > 509) {
                return 0;
            }
            q = d.text + d.len;
            for (i = 0; i < prec - frac; i++) {
                *--p = '0';
            }
            for (i = 0; i < frac && i < d.len; i++) {
                *--p = *--q;
            }
            for (; i < frac; i++) {
                *--p = '0';
            }
            if (prec != 0 || alt != 0) {
                *--p = '.';
            }
            if (whole != 0) {
                for (k = 0; k < whole - d.len; k++) {
                    *--p = '0';
                }
                for (; k < whole; k++) {
                    *--p = *--q;
                }
            } else {
                *--p = '0';
            }
            if (d.sign) {
                *--p = '-';
            } else if (sign == 1) {
                *--p = '+';
            } else if (sign == 2) {
                *--p = ' ';
            }
            break;
    }
    return p;
}

/* format engine: hands each piece of output to cb(arg, text, length) and returns the total length, -1 on failure */
size_t func_0204a5c8(int (*cb)(void *, const char *, size_t), void *arg, const char *fmt, char *ap) {
    int count, v, len, n;
    FmtSpec spec;
    long long ll;
    double d;
    char *s;
    const char *pct;
    char fill = ' ';
    char space;
    char buf[512];
    int total = 0;
    while (*fmt) {
        pct = strchr(fmt, '%');
        if (pct == 0) {
            n = strlen(fmt);
            total += n;
            if (n == 0) {
                break;
            }
            if (cb(arg, fmt, n)) {
                break;
            }
            return -1;
        }
        n = pct - fmt;
        total += n;
        if (n) {
            if (!cb(arg, fmt, n)) {
                return -1;
            }
        }
        fmt = func_02048d8c(pct, &ap, &spec);
        switch (spec.conv) {
            case 'd':
            case 'i':
                if (spec.len == 3) {
                    v = *(int *) ((ap += 4) - 4);
                } else if (spec.len == 4) {
                    ll = *(long long *) ((ap += 8) - 8);
                } else if (spec.len == 6) {
                    ll = *(long long *) ((ap += 8) - 8);
                } else if (spec.len == 7) {
                    v = *(unsigned *) ((ap += 4) - 4);
                } else if (spec.len == 8) {
                    v = *(long *) ((ap += 4) - 4);
                } else {
                    v = *(int *) ((ap += 4) - 4);
                }
                if (spec.len == 2) {
                    v = (short) v;
                }
                if (spec.len == 1) {
                    v = (signed char) v;
                }
                if (spec.len == 4 || spec.len == 6) {
                    s = func_02049568(ll, buf + 512, spec);
                    if (s == 0) {
                        goto bad;
                    }
                } else {
                    s = func_0204931c(v, buf + 512, spec);
                    if (s == 0) {
                        goto bad;
                    }
                }
                len = (buf + 511) - s;
                break;
            case 'o':
            case 'u':
            case 'x':
            case 'X':
                if (spec.len == 3) {
                    v = *(int *) ((ap += 4) - 4);
                } else if (spec.len == 4) {
                    ll = *(long long *) ((ap += 8) - 8);
                } else if (spec.len == 6) {
                    ll = *(long long *) ((ap += 8) - 8);
                } else if (spec.len == 7) {
                    v = *(unsigned *) ((ap += 4) - 4);
                } else if (spec.len == 8) {
                    v = *(long *) ((ap += 4) - 4);
                } else {
                    v = *(int *) ((ap += 4) - 4);
                }
                if (spec.len == 2) {
                    v = (unsigned short) v;
                }
                if (spec.len == 1) {
                    v = (unsigned char) v;
                }
                if (spec.len == 4 || spec.len == 6) {
                    s = func_02049568(ll, buf + 512, spec);
                    if (s == 0) {
                        goto bad;
                    }
                } else {
                    s = func_0204931c(v, buf + 512, spec);
                    if (s == 0) {
                        goto bad;
                    }
                }
                len = (buf + 511) - s;
                break;
            case 'e':
            case 'E':
            case 'f':
            case 'F':
            case 'g':
            case 'G':
                if (spec.len == 9) {
                    ap += 8;
                    d = *(long double *) (ap - 8);
                } else {
                    ap += 8;
                    d = *(double *) (ap - 8);
                }
                s = func_02049e64(d, buf + 512, spec);
                if (s == 0) {
                    goto bad;
                }
                len = (buf + 511) - s;
                break;
            case 'a':
            case 'A':
                if (spec.len == 9) {
                    ap += 8;
                    d = *(long double *) (ap - 8);
                } else {
                    ap += 8;
                    d = *(double *) (ap - 8);
                }
                s = func_02049864(d, buf + 512, spec);
                if (s == 0) {
                    goto bad;
                }
                len = (buf + 511) - s;
                break;
            case 's':
                if (spec.len == 5) {
                    const void *w = *(const void **) ((ap += 4) - 4);
                    if (w == 0) {
                        w = (wchar_t *) L"";
                    }
                    if ((int) wcstombs(buf, w, 512) < 0) {
                        goto bad;
                    }
                    s = buf;
                } else {
                    s = *(char **) ((ap += 4) - 4);
                }
                if (s == 0) {
                    s = "";
                }
                if (spec.alt) {
                    len = (unsigned char) *s++;
                    if (spec.prec_set) {
                        if (len > spec.prec) {
                            len = spec.prec;
                        }
                    }
                } else if (spec.prec_set) {
                    char *e;
                    len = spec.prec;
                    e = memchr(s, 0, len);
                    if (e) {
                        len = e - s;
                    }
                } else {
                    len = strlen(s);
                }
                break;
            case 'n': {
                void *p = *(void **) ((ap += 4) - 4);
                switch (spec.len) {
                    case 0:
                        *(int *) p = total;
                        break;
                    case 2:
                        *(short *) p = total;
                        break;
                    case 3:
                        *(int *) p = total;
                        break;
                    case 6:
                        *(long long *) p = total;
                        break;
                    case 7:
                        *(int *) p = total;
                        break;
                    case 8:
                        *(int *) p = total;
                        break;
                    case 4:
                        *(long long *) p = total;
                        break;
                }
                continue;
            }
            case 'c':
                s = buf;
                buf[0] = *(int *) ((ap += 4) - 4);
                len = 1;
                break;
            case '%':
                buf[0] = '%';
                s = buf;
                len = 1;
                break;
            case 0xff:
            default:
            bad:
                n = strlen(pct);
                if (n) {
                    if (!cb(arg, pct, n)) {
                        return -1;
                    }
                }
                return total + n;
        }
        count = len;
        if (spec.justify != 0) {
            fill = (spec.justify == 2) ? '0' : ' ';
            if ((*s == '+' || *s == '-' || *s == ' ') && fill == '0') {
                if (!cb(arg, s, 1)) {
                    return -1;
                }
                s++;
                len--;
            }
            if (count < spec.width) {
                do {
                    if (!cb(arg, &fill, 1)) {
                        return -1;
                    }
                } while (++count < spec.width);
            }
        }
        if (len) {
            if (!cb(arg, s, len)) {
                return -1;
            }
        }
        if (spec.justify == 0 && count < spec.width) {
            do {
                space = ' ';
                if (!cb(arg, &space, 1)) {
                    return -1;
                }
            } while (++count < spec.width);
        }
        total += count;
    }
    return total;
}
typedef struct StrWriteCtx {
    char *buf;
    size_t size;
    size_t pos;
} StrWriteCtx;

/* formatter output callback: appends to a bounded buffer */
int func_0204adec(void *arg, const char *src, size_t n) {
    StrWriteCtx *c = arg;
    size_t pos = c->pos;
    size_t size = c->size;
    if (pos + n > size) {
        n = size - pos;
    }
    memcpy(c->buf + pos, src, n);
    c->pos += n;
    return 1;
}

int vsnprintf(char *buf, size_t size, const char *format, char *args) {
    StrWriteCtx c;
    size_t n;
    c.buf = buf;
    c.size = size;
    c.pos = 0;
    n = func_0204a5c8(func_0204adec, &c, format, args);
    if (buf == 0) {
        return n;
    }
    if (n < size) {
        buf[n] = 0;
        return n;
    }
    if (size) {
        (buf + size)[-1] = 0;
    }
    return n;
}

int vsprintf(char *buf, const char *format, char *args) {
    return vsnprintf(buf, -1, format, args);
}

int sprintf(char *buf, const char *format, ...) {
    char *ap = (char *) ((unsigned) &format & ~3) + 4;
    return vsnprintf(buf, -1, format, ap);
}
