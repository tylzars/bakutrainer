#pragma once

#include "types.h"
#include <Main/Scene/move_object.h>

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
    // The sprite id (OBJ_KIND_SPRITE) or DrawText's align (OBJ_KIND_TEXT).
    s32 mUnk_44;
    u8 mUnk_48[0x10a - 0x48];
    // Frames left for a text or a timed sprite; the slot is freed when it counts down to 0. A text added with 0 wraps
    // to 0xffff on its first update. A sprite added with a negative duration gets 0 here instead and falls (mVy grows
    // by mVyPerFrame) until it leaves the 0x100 x 0xc0 screen area. For OBJ_KIND_ANIM this halfword lies inside the
    // sprite script state at mUnk_30 and is not a timer.
    u16 mTimer;
    u8 mUnk_10c[2];
    u8 mUnk_10e;
    u8 mUnk_10f[0x134 - 0x10f];
    // While mUsePos is set, the slot is drawn at mPos projected to the screen (ProjectToScreen) plus (mX, mY) >> 8.
    VecFx32 mPos;
    u8 mUsePos;
    u8 mUnk_141[3];
    // The position in 1/256 pixels, moved by mVx and mVy each update. Drawn at (mX, mY) >> 8, or that far from the
    // projected mPos while mUsePos is set.
    s32 mX;
    s32 mY;
    s32 mVx;
    s32 mVy;
    s32 mVyPerFrame;
    // DrawSprite's w; a text slot's glyphs get it through gSystemState.mUnk_308. The low byte is the sort key and
    // w >> 8 would be the priority, but every caller passes a value below 0x100.
    s32 mDrawOrder;
    s32 mLayer;
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

extern SlotCounters gSlotCounters;
extern AnimSlot gAnimSlots[ANIM_SLOT_COUNT];
extern ObjSlot gObjSlots[OBJ_SLOT_COUNT];

void ClearObjSlots(void);
ObjSlot *AddObjSlot(int kind, int a, int x, int y, int vx, int vy, int drawOrder, int layer);
int AddAnimObjSlot(int script, int x, int y, int drawOrder, int layer);
int AddAnimObjSlotEx(int script, int a, int x, int y, int vx, int vy, int drawOrder, int layer);
int AddSpriteObjSlot(int a, int b, int duration, int x, int y, int vx, int vy, int drawOrder, int layer, int i);
int AddTextObjSlot(const char *text, int align, const void *font, int duration, int x, int y, int vx, int vy,
                   int drawOrder, int layer, int g);
ObjSlot *FindObjSlot(int id);
void UpdateObjSlots(void);
void ClearAnimSlots(void);
void func_0200daec(void);
void UpdateAnimSlots(void);
void func_0200dbac(void);
void GetSetupMapArgs(int id, u16 *outId, u16 *outVal, u16 *outOpt1, u16 *outZ1, u16 *outZ2, u16 *outOpt2);
void RunVideo(const char *path, int layer);
void StartTitleSlideIn(void);

#ifdef __cplusplus
}
#endif
