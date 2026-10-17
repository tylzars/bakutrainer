#include <runtime.h>
#include <math.h>

// Decides how to round the digit string when only the first n digits are kept: 1 rounds up, -1 rounds down.
// Exactly one half rounds to the even digit.
int func_0204b500(const DecDigits *d, int n) {
    const unsigned char *p = &d->text[n];
    const unsigned char *end;
    if (d->text[n] > 5) {
        return 1;
    }
    if (d->text[n] < 5) {
        return -1;
    }
    p++;
    end = d->text + d->len;
    while (p < end) {
        if (*p != 0) {
            return 1;
        }
        p++;
    }
    return (d->text[n - 1] & 1) ? 1 : -1;
}

// Adds one to the digit before position n, carrying to the left. A string of nines becomes a single 1
// and the exponent grows by one.
void func_0204b56c(DecDigits *d, int n) {
    unsigned char *b = d->text;
    unsigned char *i = b + n - 1;
    while (1) {
        if (*i < 9) {
            (*i)++;
            break;
        } else if (i == b) {
            *i = 1;
            d->exp++;
            break;
        } else {
            *i = 0;
            i--;
        }
    }
}

// Rounds the digit string to n digits (n must be below the current length).
void func_0204b5bc(DecDigits *d, int n) {
    if (n <= 0) {
        return;
    }
    if (n >= d->len) {
        return;
    }
    {
        int r = func_0204b500(d, n);
        d->len = n;
        if (r < 0) {
            return;
        }
        func_0204b56c(d, n);
    }
}

// Converts an unsigned 64-bit integer to its DecDigits digits.
void func_0204b5fc(DecDigits *d, unsigned long long v) {
    d->sign = 0;
    d->len = 0;
    if (v != 0) {
        do {
            int k = d->len++;
            d->text[k] = (unsigned char) (v % 10);
            v = v / 10;
        } while (v != 0);
    }
    {
        unsigned char *a = d->text;
        unsigned char *b = a + d->len - 1;
        while (a < b) {
            unsigned char t = *b;
            unsigned char u = *a;
            *a++ = t;
            *b-- = u;
        }
    }
    d->exp = d->len - 1;
}

// Multiplies two digit strings and keeps up to 32 digits, rounded to nearest even.
void func_0204b6c8(DecDigits *res, const DecDigits *a, const DecDigits *b) {
    unsigned char buf[64];
    unsigned int sum = 0;
    int n = a->len + b->len - 1;
    unsigned char *p = buf + n + 1;
    unsigned char *end = p;
    int k;

    res->sign = 0;
    while (n > 0) {
        int i = b->len - 1;
        int j = n - i - 1;
        int cnt;
        const unsigned char *pa, *pb;
        if (j < 0) {
            j = 0;
            i = n - 1;
        }
        pa = &a->text[j];
        pb = &b->text[i];
        cnt = i + 1;
        if (cnt > a->len - j) {
            cnt = a->len - j;
        }
        while (cnt > 0) {
            sum += *pa * *pb;
            pa++;
            pb--;
            cnt--;
        }
        n--;
        *--p = sum % 10;
        sum /= 10;
    }
    res->exp = a->exp + b->exp;
    if (sum != 0) {
        *--p = sum;
        res->exp++;
    }
    for (k = 0; k < 32 && p < end; k++) {
        res->text[k] = *p;
        p++;
    }
    res->len = k;
    if (p >= end) {
        return;
    }
    if (*p < 5) {
        return;
    }
    if (*p == 5) {
        const unsigned char *q = p + 1;
        while (q < end) {
            if (*q != 0) {
                goto up;
            }
            q++;
        }
        if (!(p[-1] & 1)) {
            return;
        }
    }
up:
    func_0204b56c(res, res->len);
}

// Fills a digit string from the characters of s (at most 32, rounded) with the given exponent.
void func_0204b84c(DecDigits *d, const char *s, short exp) {
    int i;
    d->exp = exp;
    d->sign = 0;
    for (i = 0; i < 32 && *s != 0; i++) {
        d->text[i] = *s++ - '0';
    }
    d->len = i;
    if (*s == 0) {
        return;
    }
    if (*s < 5) {
        return;
    }
    if (*s <= 5) {
        const char *p = s + 1;
        for (; *p != 0; p++) {
            if (*p != '0') {
                goto up;
            }
        }
        if (!(d->text[i - 1] & 1)) {
            return;
        }
    }
up:
    func_0204b56c(d, d->len);
}

// Builds 2^n as a digit string: table values for small n, otherwise squaring of 2^(n/2).
void func_0204b8e8(DecDigits *d, int n) {
    DecDigits b, a;
    switch (n) {
        case -64:
            func_0204b84c(d, "542101086242752217003726400434970855712890625", -20);
            return;
        case -53:
            func_0204b84c(d, "11102230246251565404236316680908203125", -16);
            return;
        case -32:
            func_0204b84c(d, "23283064365386962890625", -10);
            return;
        case -16:
            func_0204b84c(d, "152587890625", -5);
            return;
        case -8:
            func_0204b84c(d, "390625", -3);
            return;
        case -7:
            func_0204b84c(d, "78125", -3);
            return;
        case -6:
            func_0204b84c(d, "15625", -2);
            return;
        case -5:
            func_0204b84c(d, "3125", -2);
            return;
        case -4:
            func_0204b84c(d, "625", -2);
            return;
        case -3:
            func_0204b84c(d, "125", -1);
            return;
        case -2:
            func_0204b84c(d, "25", -1);
            return;
        case -1:
            func_0204b84c(d, "5", -1);
            return;
        case 0:
            func_0204b84c(d, "1", 0);
            return;
        case 1:
            func_0204b84c(d, "2", 0);
            return;
        case 2:
            func_0204b84c(d, "4", 0);
            return;
        case 3:
            func_0204b84c(d, "8", 0);
            return;
        case 4:
            func_0204b84c(d, "16", 1);
            return;
        case 5:
            func_0204b84c(d, "32", 1);
            return;
        case 6:
            func_0204b84c(d, "64", 1);
            return;
        case 7:
            func_0204b84c(d, "128", 2);
            return;
        case 8:
            func_0204b84c(d, "256", 2);
            return;
    }
    func_0204b8e8(&b, (n + (int) ((n & 0x80000000) >> 31)) >> 1);
    func_0204b6c8(d, &b, &b);
    if (n & 1) {
        a = *d;
        if (n > 0) {
            func_0204b84c(&b, "2", 0);
        } else {
            func_0204b84c(&b, "5", -1);
        }
        func_0204b6c8(d, &a, &b);
    }
}

// Converts a double to its exact DecDigits digits; zero, infinity and NaN are marked with text 0, 'I' and 'N'.
// Converts a double to digits as asked by the format: rounded to form->ndigits, then as ASCII characters.
void func_0204bc6c(DecDigits *d, double x) {
    char sign = func_0204bf28(x) != 0;
    DecDigits iv, p2;
    int e, n;
    double f, t;
    unsigned long long bits;

    if (x == 0.0) {
        d->sign = sign;
        d->exp = 0;
        d->len = 1;
        d->text[0] = 0;
        return;
    }
    if (func_0204bf3c(x) <= FP_INFINITE) {
        d->sign = sign;
        d->exp = 0;
        d->len = 1;
        d->text[0] = (func_0204bf3c(x) == FP_NAN) ? 'N' : 'I';
        return;
    }
    if (sign) {
        x = -x;
    }
    f = frexp(x, &e);
    t = f;
    bits = *(unsigned long long *) &t | 0x0010000000000000ULL;
    n = 53 - func_0204be94((bits & -bits) - 1);
    func_0204b8e8(&p2, e - n);
    func_0204b5fc(&iv, (unsigned long long) scalbn(f, n));
    func_0204b6c8(d, &iv, &p2);
    d->sign = sign;
}

// Converts a double to digits as asked by the format: rounded to form->ndigits, then as ASCII characters.
void func_0204bdec(const DecForm *form, double x, DecDigits *d) {
    int digits = form->ndigits;
    int i;
    func_0204bc6c(d, x);
    if (d->text[0] > 9) {
        return;
    }
    if (digits > 32) {
        digits = 32;
    }
    func_0204b5bc(d, digits);
    while (d->len < digits) {
        d->text[d->len++] = 0;
    }
    d->exp -= d->len - 1;
    for (i = 0; i < d->len; i++) {
        d->text[i] += '0';
    }
}

// Population count of a 64-bit value.
int func_0204be94(unsigned long long x) {
    x = x - ((x >> 1) & 0x5555555555555555ULL);
    x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
    x = (x + (x >> 4)) & 0x0f0f0f0f0f0f0f0fULL;
    x = x + (x >> 8);
    x = x + (x >> 16);
    x = x + (x >> 32);
    return (int) (x & 0xff);
}
