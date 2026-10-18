#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Entry of a circular doubly linked list of 16-bit indices; entry 0 is the list head.
typedef struct IndexNode16 {
    u8 mUnk_0[0xa];
    u16 mPrev;
    u16 mNext;
    u8 mUnk_e[2];
} IndexNode16;

void func_0201f450(int a, int b, u32 c, u32 d);
u8 *func_0201f5bc(u8 *palette, u32 count, int a, int target);
void func_0201f648(void);
void func_0201f6d0(int a, int b, int c, int d, u16 e);
int func_0201f794(int a, int b);
int func_0201f7c0(int a, int b);
void func_0201f7cc(int a);
void func_0201f7f8(int a);
void func_0201f830(int a);
void func_0201f858(int a);
void func_0201f880(int a);
void func_0201f8d0(int a);
int func_0201f910(u32 *frames, int *touched, u32 minFrames, u32 maxFrames, int layers);
void func_0201f980(int a, int b, int c, int d, int e);
void func_0201fa08(IndexNode16 *list, u32 index);

typedef struct IndexNode {
    u16 mUnk_0;
    u16 mPrev;
    u16 mNext;
    u16 mUnk_6;
} IndexNode;
void func_0201fa5c(IndexNode *list, u32 index);
#ifdef __cplusplus
}
#endif
