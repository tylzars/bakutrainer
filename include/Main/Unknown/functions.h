#pragma once

#include "types.h"
#include <Main/Unknown/globals.h>
#include <Main/Unknown/stage_data.h>

#ifdef __cplusplus
extern "C" {
#endif

// Declarations of game functions used by other files.
void func_02008288(void);
void func_02008994(int a);
void func_02009728(void);
void func_02009fe8(void);
int func_0200b88c(u8 stage);
void func_0200b928(s32 *p, s32 target, int steps);
s32 func_0200b9cc(s32 *p, s32 step);
void func_0200bb14(int a, int b, int c, int d, int e, int f, int g, int h);
void func_0200bb58(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j, int k, int l, int m, int n,
                   int o);
void func_0200bc50(int a, int b, int c, int d, int e, int f, int g, const char *h, const void *i, int j, int k, int l,
                   int m, int n, int o, int p);
int func_0200bd80(u32 a, u32 b);
int func_0200bdac(u32 a, u32 b, u32 c);
void func_0200bdcc(int a, int b);
u16 func_0200be10(int r, int g, int b);
void func_0200be20(int a, int b, int c, u16 color);
u32 func_0200bf54(int n);
int func_0200bf68(u32 *mask);
int func_0200bfc4(int v, int a, int b, int c, int d);
void func_0200bffc(int x, int y, int value, const void *font, int align, int mode);
void func_0200c5d8(int a, int b, int c, int d, int e, int f, int g);
void func_0200c688(int n);
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
void func_020105f4(void *obj);
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
int func_02014538(int a, int b);
void func_020145bc(void);
void func_020149e0(void);
void func_02014bf0(void);
int func_02014d2c(StageInfo *info);
int func_02014f98(StageRec *rec);
StageRec *func_02015190(int index);
void func_020151f8(void);
int func_020186f0(u8 stage);
void func_0201b7e0(u32 a);
void func_0201bb30(void);
void func_0201bb40(int n);
void func_0201c41c(void);
void func_0201c4f0(void *p);
void func_0201dde0(int a, int b);
void func_0201ddec(int a, int b, int c, int d);
void func_0201eb20(void);
void func_0201f6d0(int a, int b, int c, int d, int e);
int func_0201f794(int a, int b);
void func_0201f7cc(void);
void func_0201f7f8(int a);
void func_0201f880(int a);
void func_0201f980(int a, int b, int c, int d, int e);
void func_0201fb24(u16 *value, int speed);
int func_0201fb74(int a, int b, int c, int d);
int func_0201fbe4(int v, int a, int b, int c, int d);
char *func_0201fc08(int value, char *buf);
u32 func_0201fcec(u64 value);
u32 func_0201fd68(void);
u32 func_0201fd84(u32 lo, u32 hi);
void func_0202002c(u16 a, int b, u16 c, int d, void (*fn)(u32), int e);
void func_02020348(void);
void func_02020854(int a);
void func_02020944(int a, int b, int c, int d, int e, int f, int g, int h);
int func_02020978(int id, int x, int y, int a, int b, int c);
void func_020215fc(int x, int y, const void *font, const char *text, int align, int width, int height, int mode);
int func_02021e08(u16 id, int index);
int func_02021e44(u16 id, int index);
void func_02021fb0(int a, int b, int c, int d);
void func_020220c8(int a);
void func_02022370(void);
void func_02023478(u16 a, u16 *p, Pos3 *pos, int d, int e, int f, int g, int h);
void func_02024208(void);
void func_020242e8(u16 id, int x, int y, int a, int b, u16 c, u8 d, int e);
void func_02024338(void *a, void *b, int c);
void func_02024368(void *obj, int a, int b, int c, int d, int e);
void func_020248e8(u32 id, void *p, int a);
void func_02025db4(void *a, s32 *out);
void func_0202601c(u16 a, u16 *p, int c, int d, s32 *e, s32 *f, s32 *g, int h);
void func_020271ec(void *obj, int a, int b, int c, u32 res);
void func_02027230(void *obj, int a, int b, int c, u32 res);
void func_0202733c(void *a, void *b, int c);
void func_02027fc2(const char *msg);
void func_0202bdd8(int a, int b, int c, int d, int e);
void func_0202c1c4(void);
void func_0202cf08(int id, int x, int y, int w, int mode);
void func_0202d828(int obj, int a, s32 *rect);
int func_0202db1c(void *obj);
void func_0202de60(void *obj, void *p);
int func_0202de80(int a, int x, int y, int prio, int e, int mode);
void func_0202f00c(void);
void func_0202f110(int a, int b, int c, int d, int e, int f);
void func_0202f200(void);
void *func_0202f500(u32 size);
void func_0202f50c(void *p);
void func_0202f638(int a, int b);
int func_0202f8d0(void);
void func_0202fb4c(void);
s32 func_02031f98(void *entry);
s32 func_02031fa4(void *entry);
s32 func_02031fb0(void *entry);
void func_02031fc4(u32 id, Pos3 *out);
void *func_0203204c(int a, int b);
void func_020325b0(u16 a, u16 b, u16 c, u16 d, u16 e, u16 f);
void func_02033488(void);
void func_02033614(s32 x, s32 z);
void func_02033df4(int a);
void func_02033e38(int a, int b, int c);
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
