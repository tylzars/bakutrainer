#pragma once

#include "types.h"
#include <nitro/wm.h>

#ifdef __cplusplus
extern "C" {
#endif

void func_0201b7e0(u32 a);
void func_0201b7ec(u32 a, int b, int c);
void func_0201bb30(void);
void func_0201bb40(int flags);
void func_0201c1d0(void);
void func_0201c1e0(void);
void func_0201c32c(void);
void func_0201c41c(void);
void *func_0201c430(u32 size);
void *func_0201c43c(u32 size, u32 flags);
int func_0201c4f0(void *p);
u16 func_0201c538(void);
u16 func_0201c544(void);
u32 func_0201c550(void);
u32 func_0201c564(void);

// Objects of the per-frame system code.
typedef struct Unk_020c321c {
    u8 mUnk_00[0x18];
    u16 mUnk_18;
    u16 mUnk_1a;
} Unk_020c321c;
extern Unk_020c321c data_020c321c;

typedef struct Unk_020c267c {
    u8 mUnk_00[0x18];
    struct {
        u8 mUnk_000[0x57c];
        u8 mUnk_57c;
        u8 mUnk_57d[3];
    } mUnk_18[2];
} Unk_020c267c;
extern Unk_020c267c data_020c267c;

typedef struct Unk_020c660c_Obj {
    u8 mUnk_00[0x4c];
    u32 mUnk_4c;
    u8 mUnk_50[0x10];
    char mUnk_60[1];
} Unk_020c660c_Obj;

typedef struct Unk_020c660c {
    u32 mUnk_00;
    s32 mUnk_04;
    s32 mUnk_08;
    Unk_020c660c_Obj *mUnk_0c;
} Unk_020c660c;
extern Unk_020c660c data_020c660c;

extern u16 data_020c9e04;
extern void *data_020c9e10;
extern u16 *data_020c9e14;
extern u8 *data_020c9e1c;
extern WMDataSharing data_020c9e60;
#ifdef __cplusplus
}
#endif
