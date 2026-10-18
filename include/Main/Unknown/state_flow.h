#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Secondary state record kept beside the main loop state.
typedef struct StateFlowAux {
    u8 mUnk_00[0x14];
    s32 mUnk_14;
} StateFlowAux;

// Text block the language-dependent string lookups index into.
typedef struct TextBase {
    u8 mUnk_00[0x0c];
    s32 mLanguage;
} TextBase;

extern StateFlowAux data_020b1214;
extern TextBase data_020b1078;

u16 func_0200f9c8(int index);
const char *func_0200f9f0(int id);
const char *func_0200fa10(int id, int index);
void func_0200fa24(void);
void func_0200fa3c(void);
void func_0200fa50(void);
void func_0200fa80(int state);
void func_0200fab0(void);
void func_0200fabc(u32 flag);
u32 func_0200fb00(void);
u32 func_0200fb0c(void);
void func_0200fb18(u32 index, u32 value);
u32 func_0200fb34(u32 index);
void func_0200fb50(s32 value);
s32 func_0200fb5c(void);

#ifdef __cplusplus
}
#endif
