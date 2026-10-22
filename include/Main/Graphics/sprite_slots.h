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
    // The script index (OBJ_KIND_ANIM) or the sprite id (OBJ_KIND_SPRITE); nothing reads it in this build.
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
    VecFx32 mUnk_134;
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

extern SlotCounters data_020abfc0;
extern AnimSlot data_020abfcc[ANIM_SLOT_COUNT];
extern ObjSlot data_020ace4c[OBJ_SLOT_COUNT];

void ClearObjSlots(void);
ObjSlot *AddObjSlot(int kind, int a, int x, int y, int vx, int vy, int life, int h);
int AddAnimObjSlot(int script, int a, int b, int c, int d);
int AddAnimObjSlotEx(int script, int a, int b, int c, int d, int e, int f, int g);
int AddSpriteObjSlot(int a, int b, int duration, int c, int d, int e, int f, int g, int h, int i);
int AddTextObjSlot(const char *text, int a, const void *font, int b, int x, int y, int c, int d, int e, int f, int g);
ObjSlot *FindObjSlot(int id);
void UpdateObjSlots(void);
void ClearAnimSlots(void);
void func_0200daec(void);
void UpdateAnimSlots(void);
void func_0200dbac(void);
void func_0200dc08(int id, u16 *outId, u16 *outVal, u16 *outOpt1, u16 *outZ1, u16 *outZ2, u16 *outOpt2);
void RunVideo(const char *path, int b);
void func_0200dd90(void);

#ifdef __cplusplus
}
#endif
