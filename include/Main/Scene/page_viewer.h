#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// State of the page screen: its mode, the page shown and a countdown before the next input is read.
typedef struct PageViewState {
    s32 mState;
    s32 mUnk_04;
    s32 mUnk_08;
} PageViewState;

extern PageViewState data_02098d74;

void func_02006fc0(void);
void func_02007040(void);
void func_020070c4(void);
void func_02007168(int state);
void func_0200719c(void);

#ifdef __cplusplus
}
#endif
