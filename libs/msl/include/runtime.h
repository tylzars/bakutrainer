#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Decimal digits of a double: sign, exponent, digit count and up to 32 digits (values 0-9 while computing, ASCII
// once formatted; text[0] is 'I' or 'N' for infinity and NaN).
typedef struct {
    /* 00 */ char sign;
    /* 01 */ char mUnk_01;
    /* 02 */ short exp;
    /* 04 */ unsigned char len;
    /* 05 */ unsigned char text[32];
    /* 25 */ unsigned char mUnk_25;
} DecDigits; /* 26 */

// How many digits the conversion keeps.
typedef struct {
    /* 00 */ unsigned char mode;
    /* 02 */ short ndigits;
} DecForm;

// Multibyte conversion functions of the active locale.
typedef struct {
    /* 00 */ int (*mbtowc)(unsigned short *, const char *, size_t);
    /* 04 */ int (*wctomb)(char *, unsigned short);
} CharConv;

typedef struct {
    /* 00 */ void *mUnk_00;
    /* 04 */ void *mUnk_04;
    /* 08 */ CharConv *conv;
} Locale;

extern Locale data_02064520;
// Character-class bits for the 128 ASCII codes (8: digit, 0x200: upper case).
extern const unsigned short data_020637fc[];
// Collation order of the characters 0x20-0x7f.
extern unsigned short data_02064628[];

// Double to decimal digits.
int func_0204b500(const DecDigits *d, int n);
void func_0204b56c(DecDigits *d, int n);
void func_0204b5bc(DecDigits *d, int n);
void func_0204b5fc(DecDigits *d, unsigned long long v);
void func_0204b6c8(DecDigits *res, const DecDigits *a, const DecDigits *b);
void func_0204b84c(DecDigits *d, const char *s, short exp);
void func_0204b8e8(DecDigits *d, int n);
void func_0204bc6c(DecDigits *d, double x);
void func_0204bdec(const DecForm *form, double x, DecDigits *d);
int func_0204be94(unsigned long long x);

// Sign bit and class (FP_* in math.h) of a double.
int func_0204bf28(double x);
int func_0204bf3c(double x);

// Frees a block unless the pointer is null (operator delete calls it).
void func_02048b3c(void *ptr);

// Start-up hooks of the C runtime: an empty one and the walk over the static-initializer table.
void func_0204ded0(void);
void func_0204dee8(void);

#ifdef __cplusplus
}
#endif
