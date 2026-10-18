#pragma once

#include "types.h"
#include <Main/Unknown/globals.h>
#include <Main/Unknown/stage_data.h>

#ifdef __cplusplus
extern "C" {
#endif

// Declarations of game functions used by other files.
void func_0200b898(int x, int y, const char *fmt, ...);
void func_0200b8dc(int x, int y, const char *fmt, ...);
void func_0200e474(void);
void func_0201c1e0(void);
void func_0201c6d4(void);
void func_0201c7b8(int a);
void func_02021fb0(int a, int b, int c, int d);
void func_020220c8(int a);
void func_02022120(void);
void func_0202224c(void);
void func_02022370(void);
void func_02035108(u16 a, int b, u8 c, int d, int e, int f);
void func_02038578(u8 volume, int mode);
u8 func_02038694(void);
int func_0203946c(void);
void func_0203947c(int a, u16 b, u16 c);
int func_02039598(int mode, const void *data);
u8 *func_02039698(u32 n);
BOOL func_020396b8(void *buf);
void func_0203970c(void);
u16 func_0203981c(void);
u32 func_02039a64(void);
void func_02039a94(void);
void func_02039b70(void);
void func_02039ba8(void);
u8 *func_02039cd4(u32 size);
void func_02039d24(void);
void func_02039d74();
void func_0204028c(u32 a);

#ifdef __cplusplus
}
#endif
