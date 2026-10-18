#pragma once

#include "types.h"
#include <Main/Unknown/move_object.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OBJ_SLOT_COUNT 16
#define ANIM_SLOT_COUNT 8

// Kinds of an ObjSlot (0 = free).
#define OBJ_KIND_FREE 0
#define OBJ_KIND_TEXT_PLUS 1
#define OBJ_KIND_SPRITE 2
#define OBJ_KIND_ANIM 3
#define OBJ_KIND_TEXT 4

// A short-lived object moving across the screen: a sprite, an animation or a text.
typedef struct ObjSlot {
    s32 mKind;
    s32 mId;
    s32 mUnk_08;
    s32 mUnk_0c;
    char mText[0x20];
    const void *mUnk_30;
    u8 mUnk_34[0xc];
    s32 mUnk_40;
    s32 mUnk_44;
    u8 mUnk_48[0x10a - 0x48];
    u16 mUnk_10a;
    u8 mUnk_10c[2];
    u8 mUnk_10e;
    u8 mUnk_10f[0x134 - 0x10f];
    u8 mUnk_134[0xc];
    u8 mUnk_140;
    u8 mUnk_141[3];
    s32 mX;
    s32 mY;
    s32 mVx;
    s32 mVy;
    s32 mUnk_154;
    s32 mUnk_158;
    s32 mUnk_15c;
} ObjSlot;

// A running animation object.
typedef struct AnimSlot {
    s32 mKind;
    u8 mUnk_04[8];
    u8 mUnk_0c[0x158 - 0xc];
    u16 mUnk_158;
    u8 mUnk_15a[2];
    u8 mUnk_15c;
    u8 mUnk_15d;
    u8 mUnk_15e[0x17e - 0x15e];
    u16 mUnk_17e;
    u8 mUnk_180[0x188 - 0x180];
    s32 mUnk_188;
    u8 mUnk_18c[0x1d0 - 0x18c];
} AnimSlot;

typedef struct SlotCounters {
    s32 mLastId;
    s32 mUnk_04;
    s32 mUnk_08;
} SlotCounters;

// Three words read from a record (func_0200e4c4).
typedef struct Unk_020ae504_Rec {
    s32 mUnk_00;
    u32 mUnk_04;
    s32 mUnk_08;
} Unk_020ae504_Rec;

// State block; only the members the code touches are named.
typedef struct Unk_020ae504 {
    Unk_020ae504_Rec mUnk_00[3];
    u32 mUnk_24;
    s32 mUnk_28;
    s32 mUnk_2c;
    s32 mUnk_30;
    s32 mUnk_34;
    s8 mUnk_38;
    u8 mUnk_39[3];
    Model mUnk_3c;
    u32 mUnk_1b4;
    u8 mUnk_1b8;
    u8 mUnk_1b9[3];
    s32 mUnk_1bc[8];
    s32 mUnk_1dc;
    s32 mUnk_1e0;
    s32 mUnk_1e4;
} Unk_020ae504;

extern SlotCounters data_020abfc0;
extern AnimSlot data_020abfcc[ANIM_SLOT_COUNT];
extern ObjSlot data_020ace4c[OBJ_SLOT_COUNT];
extern Unk_020ae504 data_020ae504;

void func_0200d6c8(void);
ObjSlot *func_0200d6ec(int kind, int a, int x, int y, int vx, int vy, int life, int h);
int func_0200d76c(int kind, int a, int b, int c, int d);
int func_0200d78c(int kind, int a, int b, int c, int d, int e, int f, int g);
int func_0200d7cc(int a, int b, int duration, int c, int d, int e, int f, int g, int h);
int func_0200d838(const char *text, int a, const void *font, int b, int x, int y, int c, int d, int e, int f, int g);
ObjSlot *func_0200d88c(int id);
void func_0200d8bc(void);
void func_0200dacc(void);
void func_0200daec(void);
void func_0200daf8(void);
void func_0200dbac(void);
void func_0200dc08(int id, u16 *outId, u16 *outVal, u16 *outOpt1, u16 *outZ1, u16 *outZ2, u16 *outOpt2);
void func_0200dd5c(const char *path, int b);
void func_0200dd90(void);

#ifdef __cplusplus
}
#endif
