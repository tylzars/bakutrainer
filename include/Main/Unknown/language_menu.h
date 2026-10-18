#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LANG_MENU_MAX 256
#define LANG_TEXT_SIZE 0x20

// One selectable language: its name, the id of its text row and the value stored as the selection.
typedef struct LangEntry {
    char mText[LANG_TEXT_SIZE];
    s32 mTextId;
    s32 mIndex;
} LangEntry;

// The language screen's list: the entries, how many there are and the selected one.
typedef struct LangMenu {
    LangEntry mEntries[LANG_MENU_MAX];
    u32 mCount;
    u32 mSelected;
} LangMenu;

// Animated value and its sentinel for the figure shown on the language screen.
typedef struct Unk_020ae6ec {
    s32 mUnk_00;
    u16 mUnk_04;
    u8 mUnk_06[2];
} Unk_020ae6ec;

// State of the shown figure: member 0x12 is a value read by other screens, 0x34 the current figure id.
typedef struct Unk_020ae834 {
    u8 mUnk_00[0x12];
    u16 mUnk_12;
    u8 mUnk_14[0x34 - 0x14];
    u16 mUnk_34;
    u16 mUnk_36_0 : 10;
    u16 mUnk_36_10 : 1;
    u16 mUnk_36_11 : 2;
    u16 mUnk_36_13 : 1;
    u16 mUnk_36_14 : 2;
} Unk_020ae834;

typedef struct Unk_0204e218 {
    s32 mUnk_00;
    s32 mUnk_04;
    s32 mUnk_08;
    s32 mUnk_0c;
} Unk_0204e218;

typedef struct Unk_0204e248 {
    u16 mUnk_00;
    u16 mUnk_02;
} Unk_0204e248;

extern Unk_020ae6ec data_020ae6ec;
extern s32 data_020ae6f4;
extern u8 data_020ae6f8[0x13c];
extern Unk_020ae834 data_020ae834;
extern LangMenu data_020ae870;
extern const Unk_0204e218 data_0204e218[];
extern const Unk_0204e248 data_0204e248[];

void func_0200f374(void);
u8 func_0200f468(void);
void func_0200f4d4(void);
void func_0200f580(void);
u8 func_0200f588(void);
void func_0200f5a4(const char *key, u32 value);
void func_0200f5c4(void);
void func_0200f660(void);
void func_0200f750(void);
void func_0200f758(void);
// Takes two arguments (it reads r0 and r1), but one caller passes only the first and leaves r1 as it is, so the
// declaration carries no prototype.
void func_0200f848();
void func_0200f898(void);
int func_0200f95c(void);
u16 func_0200f97c(void);
int func_0200f988(void);
int func_0200f9b8(void);

#ifdef __cplusplus
}
#endif
