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
void func_0202c2f0(void *dst, const void *src, u32 w, u32 h, BOOL mode);
void func_0202c3e0(Unk_LoadEntry *entry, const void *src, u32 w);
void func_0202cf08(int id, int x, int y, int w, int mode);
int func_0202d7a0(u32 flags, int index, s32 *out);
int func_0202d828(int obj, int a, s32 *rect);
void func_0202d908(int a);
int func_0202db1c(void *obj);
void func_0202de60(void *obj, void *p);
int func_0202de80(int a, int x, int y, int prio, int e, int mode);
void func_0202de94(void);
void func_0202dec4(int id);
void *func_0202ded8(int id);
void func_0202e494(void);
void func_0202e4fc(u32 file, void *dst, u32 size, u32 offset);
u32 func_0202e760(u32 index);
u8 *func_0202e86c(int id);
void func_0202e8e4(void);
int func_0202e988(int id);
void func_0202eb64(int a, int b, int c, int d, int e, int f);
void func_0202edd0(u32 id);
void func_0202eeec(void);
void func_0202ef24(void);
void func_0202efd4(void);
void func_0202f00c(void);
void func_0202f110(int a, int b, int c, int d, int e, int f);
void func_0202f200(void);
void func_0202f240(int id, int volume);
void func_0202f290(void *obj, const char *name, u32 n);
void func_0202f2e8(void *obj);
void func_0202f31c(void *obj);
void func_0202f410(void *obj);
void func_0202f470(void *obj);
void *func_0202f500(u32 size);
void func_0202f50c(void *p);
void func_0202f638(const char *path, int b);
int func_0202f8d0(void);
void func_0202fb4c(void);
void func_0202fbc8(void);
u8 func_0202fcb4(void);
int func_02030830(fx32 x, fx32 z, fx32 y, int a, u32 mask, u8 *out, VecFx32 *vec);
int func_02031270(int a, int b, int c, int d, int e, int f);
u32 func_0203129c(void);
u32 func_02031824(VecFx32 *pos, int radius, int a, int b, u16 c, u16 d);
void *func_02031c70(void *p);
int func_02031c98(u16 *entry, u16 mask, u16 bits);
int func_02031d0c(VecFx32 *pos, int a, int b, int c);
int func_02031dc4(s32 x, s32 z);
u16 *func_02031eb8(u32 id);
s32 func_02031f98(void *entry);
s32 func_02031fa4(void *entry);
s32 func_02031fb0(void *entry);
void func_02031fc4(u32 id, VecFx32 *out);
void func_02032018(u32 index, VecFx32 *pos, u8 *flags);
void *func_0203204c(int a, int b);
int func_020320c0(int a, int b, VecFx32 *pos);
void func_020325b0(u16 a, u16 b, u16 c, u16 d, u16 e, u16 f);
void func_02033488(void);
void func_020334f8(int a);
void func_02033520(void);
void func_02033614(s32 x, s32 z);
void func_02033b8c(void);
void func_02033df4(int a);
void func_02033e38(int a, int b, int c);
void func_02033fc8(void);
void func_02034768(int a, int b, int c);
void func_020348ac(int r, int g, int b, int z);
u32 func_02034918(u32 x);
void *func_0203492c(void *p);
void func_02034954(void);
void func_02034a08(void);
void func_02034a44(void);
void func_02034a60(int a);
void func_02034be4(void);
void func_02034c38(void);
void func_02034ce0(u32 value, void *dst);
u32 func_02034cf0(void *src);
void func_02034d1c(void);
int func_02035058(void);
void func_020350a0(void);
int func_020350f8(void);
void func_02035108(u16 a, int b, u8 c, int d, int e, int f);
void func_020359d0(int a);
void func_020360f4(void);
void func_02036194(void);
void func_02038578(u8 volume, int mode);
u8 func_02038694(void);
int func_0203946c(void);
void func_0203947c(int a, u16 b, u16 c);
u32 func_02039a64(void);
void func_02039a94(void);
void func_02039b70(void);
void func_02039ba8(void);
void func_02039d24(void);
void func_0204028c(u32 a);

#ifdef __cplusplus
}
#endif
