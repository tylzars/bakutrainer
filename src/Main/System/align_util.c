#include <Main/System/align_util.h>

#pragma thumb off

u32 AlignUp32(u32 x) {
    return x + (31 - ((x - 1) & 31));
}

void *AlignPtrUp4(void *p) {
    return (void *) ((u32) p + (3 - (((u32) p - 1) & 3)));
}

void *AlignPtrUp2(void *p) {
    return (void *) ((u32) p + (1 - (((u32) p - 1) & 1)));
}

#pragma thumb on
