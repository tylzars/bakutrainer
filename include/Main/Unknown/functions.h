#pragma once

#include "types.h"
#include <Main/Unknown/stage_data.h>

#ifdef __cplusplus
extern "C" {
#endif

// Declarations of game functions used by other files.
int func_02006fb4(u8 stage);
int func_0200b88c(u8 stage);
void func_0200bb14(int a, int b, int c, int d, int e, int f, int g, int h);
void func_0200bb58(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j, int k, int l, int m, int n,
                   int o);
void func_0200bc50(int a, int b, int c, int d, int e, int f, int g, const char *h, const void *i, int j, int k, int l,
                   int m, int n, int o, int p);
int func_0200bd80(u32 a, u32 b);
int func_0200bdac(u32 a, u32 b, u32 c);
void func_0200bdcc(int a, int b);

void func_0200c5d8(int a, int b, int c, int d, int e, int f, int g);
int func_0200d6b0(u8 stage);
void func_0200d6c8(void);
void func_0200d838(const char *text, int a, const void *font, int b, int x, int y, int c, int d, int e, int f, int g);
void func_0200d8bc(void);
void func_0200dacc(void);
void func_0200daf8(void);
void func_0200dbac(void);
void func_0200e474(void);
void func_0200f848(u32 value);
void func_0200f898(void);
int func_0200f95c(void);
const char *func_0200f9f0(int id);
void func_0200fa24(void);
void func_0200fa80(int a);
void func_0200fab0(void);
void func_0201004c(void);
void func_020102f8(void);
void func_0201033c(int a, int b);
void func_02010388(int a, int b);
void func_020103c0(void);
void func_020104c0(int a, int b);
void func_02010648(u32 a);
void func_02010c94(void);
void func_02010d34(void);
void func_0201168c(int level, u32 a, int b, int c, int d);
void func_02011890(int level, int mode);
void func_02013d48(StageInfo *info, s16 slot, u8 flag, int a, int b, int c, int d, int e);
void func_02013dc8(StageInfo *info, s16 slot, int a, int b);
void func_02013f74(int level, void *obj, int a, int b, int c, int d);
void func_020143f4(void);
void func_02014430(int a);
void func_020145bc(void);
void func_020149e0(void);
void func_02014bf0(void);
int func_02014d2c(StageInfo *info);
int func_02014f98(StageRec *rec);
int func_020186f0(u8 stage);
void func_0201b7e0(u32 a);
void func_0201bb30(void);
void func_0201bb40(int n);
void func_0201c4f0(void *p);
void func_0201eb20(void);
int func_0201f794(int a, int b);
void func_0201f7cc(void);
void func_0201f7f8(int a);
void func_0201f880(int a);
u32 func_0201fd68(void);
u32 func_0201fd84(u32 lo, u32 hi);
void func_0202002c(u16 a, int b, u16 c, int d, void (*fn)(u32), int e);
void func_02020348(void);
void func_020215fc(int x, int y, const void *font, const char *text, int align, int width, int height, int mode);
int func_02021e44(u16 id, int index);
void func_02022370(void);
void func_02024208(void);
void func_020248e8(u32 id, void *p, int a);
void func_02025db4(void *a, s32 *out);
void func_020271ec(void *obj, int a, int b, int c, u32 res);
void func_0202733c(void *a, void *b, int c);
void func_0202cf08(int id, int x, int y, int w, int mode);
int func_0202db1c(void *obj);
void func_0202de60(void *obj, void *p);
int func_0202de80(int a, int x, int y, int prio, int e, int mode);
void func_0202f00c(void);
void func_0202f110(int a, int b, int c, int d, int e, int f);
void func_0202f200(void);
void *func_0202f500(u32 size);
void func_0202f50c(void *p);
void func_02034954(void);
void func_02034d1c(void);
void func_020350a0(void);
int func_020350f8(void);
int func_0203946c(void);
void func_0203947c(int a, u16 b, u16 c);
void func_02039ba8(void);

#ifdef __cplusplus
}
#endif
