// Side header of system_init.c: its touch buttons and touch trail.
#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// A point of the touch trail (two bytes) that ReadTouchAndAppendTrail appends (see Unk_020c2560).
typedef struct Unk_TouchPoint {
    u8 mUnk_0;
    u8 mUnk_1;
} Unk_TouchPoint;

typedef struct Unk_TouchButton {
    s16 mUnk_0;
    s16 mUnk_2;
    s16 mUnk_4;
    s16 mUnk_6;
    u16 mUnk_8;
    u16 mUnk_a;
    u16 mUnk_c;
    u16 mUnk_e;
} Unk_TouchButton;

// The touch state that follows the trail.
typedef struct Unk_020c2660 {
    u8 mUnk_00[4];
    u32 mUnk_04;
    u32 mUnk_08;
    u32 mUnk_0c;
    u16 mUnk_10;
    u16 mUnk_12;
    u8 mUnk_14;
    u8 mUnk_15;
    u16 mUnk_16;
    u8 mUnk_18;
} Unk_020c2660;

// The touch trail (the points ReadTouchAndAppendTrail appends; their count is mUnk_100.mUnk_10, wrapping at 0x80)
// and the touch state that follows it form one object.
typedef struct Unk_020c2560 {
    Unk_TouchPoint mUnk_000[0x80];
    Unk_020c2660 mUnk_100;
} Unk_020c2560;

// The touch buttons, followed by the touch trail and its state, form one object.
typedef struct Unk_020c1d60 {
    Unk_TouchButton mUnk_000[0x80];
    Unk_020c2560 mUnk_800;
} Unk_020c1d60;
extern Unk_020c1d60 data_020c1d60;

int IsLcdSwapped(void);
int func_02020890(void);
void SetTouchButtonSoundsAndKeys(int flags, u16 a, u16 b, u16 c);
void AddTouchButtonEx(int a, int b, int c, int d, int e, int f, u16 g, u16 h, u16 i);
void AddTouchButtonWithSounds(int a, int b, int c, int d, int e, int f, u16 g, u16 h);
void AddTouchButton(int id, int x, int y, int a, int b, int c);
int IsTouchInShape(int x, int y, int shape);
int IsPointInShape(int x, int y, int shape, int px, int py);
void ClearTouchButtons(void);
u32 UpdateTouchButtons(u32 mask);
void ResetTouchTrail(void);
void ResetTouchButtons(u16 a, int b);
int ReadTouchAndAppendTrail(u8 *x, u8 *y);

#ifdef __cplusplus
}
#endif
