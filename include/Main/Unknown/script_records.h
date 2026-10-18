#pragma once

#include "types.h"
#include <Main/Unknown/globals.h>

#ifdef __cplusplus
extern "C" {
#endif

void func_02028e5c(const char *name);
void func_02028eb4(int index);
void func_02028f0c(int a, int b, int c, int d, int e, int f, void (*g)(u32 id, int x, int y, int w, int mode),
                   u8 (*h)(void), u32 (*i)(u32));
void func_02028fe8(void);
u8 func_0202903c(u8 c);
int func_02029050(u8 c);
int func_0202906c(u8 c);
int func_02029084(u8 c);
int func_02029094(u8 c);
int func_020290ac(const char *a, const char *b);
void func_020290cc(u8 **src, u8 **dst);
void func_02029118(s16 value, u8 **out);
s16 func_02029128(u8 **text);
const u8 *func_020291a4(const u8 *name);
void func_0202920c(const u8 *p, u16 *values, u32 *count);
void func_02029250(const s16 *src, s16 *dst, int count, int op);
int func_02029298(u8 **text);
void func_020292f4(const char *text);
char *func_0202938c(char *dst, const char *src);
int func_020293c4(s16 *out, u8 *text, u32 size);
int func_020293dc(s16 *out, u8 *text);
void func_020294c4(u8 *text, int mode);
void func_020297c8(u8 *text);
void func_02029898(u8 *text);
void func_020298c8(void);
void func_02029970(u8 *text);
void func_020299d4(u8 *text);
void func_02029a1c(const char *name);
void func_02029a74(u8 *text);
void func_02029aac(u8 *text, int flag);
void func_02029b28(u8 *text);
void func_02029b6c(u8 *text);
void func_02029b98(u8 *text);
void func_02029bd8(u8 *text);
void func_02029c18(u8 *text);
void func_02029c58(u8 *text);
void func_02029c98(u8 *text);
void func_02029d00(u8 *text);
void func_02029d8c(const char *name);
void func_02029da0(u8 *text);
void func_02029dd8(u8 *text);
void func_02029ed8(int id, int *a, int *b, int *c);
void func_02029f6c(u8 *text);
void func_0202a034(u8 *text);
void func_0202a078(int a, int b);
void func_0202a0dc(int index, int id);
void func_0202a10c(Unk_020c61ac_Cmd *rec);
void func_0202a138(void);
void func_0202a160(void);
u8 func_0202a188(void);
void func_0202a194(void);
Unk_EventRec *func_0202a1a4(void);
void func_0202a1cc(void);
void func_0202a1ec(int (*callback)(void));

#ifdef __cplusplus
}
#endif
