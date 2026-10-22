#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LANG_MENU_MAX 256
#define LANG_TEXT_SIZE 0x20

// One row of the list: a non-empty string of table 0x246 before "LAST" (its text, and its index in the table, which
// UpdateLangMenu passes to func_02010364) and mIndex, its list position minus 1, which becomes the prompt number
// (flow value 0x14) when the row is chosen.
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

// The prompt's sprite: its index in gPromptSpriteDefs (-1: none; script event 6 sets it) and its slide level, which
// DrawPromptSprite eases to full.
typedef struct PromptSprite {
    s32 mSpriteIndex;
    u16 mSlideLevel;
    u8 mUnk_06[2];
} PromptSprite;

typedef struct PromptSpriteDef {
    s32 mX;
    s32 mY;
    // The y offset the sprite and the script text slide in from.
    s32 mSlideY;
    s32 mSpriteId;
} PromptSpriteDef;

typedef struct PromptWave {
    u16 mWaveId;
    u16 mAddLanguage;
} PromptWave;

// Font list of the script text (eight entries, all the first font record).
extern const struct FontRange *data_02063988[8];
extern PromptSprite gPromptSprite;
extern LangMenu gLangMenu;
extern const PromptSpriteDef gPromptSpriteDefs[];
extern const PromptWave gPromptWaves[];
extern const u16 data_0204e208[1];
extern const u16 data_0204e20c[1];
extern const u16 data_0204e210[1];
extern const u32 data_0204e214[1];
extern const u16 data_0204e2c8[86];

void SetupPrompt(void);
int DrawPromptSprite(void);
void UpdatePrompt(void);
void LeavePrompt(void);
u8 func_0200f588(void);
void SetScriptRecord(const char *key, u32 value);
void HandlePromptEvents(void);
void SetupLangMenu(void);
void LeaveLangMenu(void);
void UpdateLangMenu(void);
void SetupSharedModel(u32 a, int b);
void DrawSharedModel(void);
int IsSharedModelDone(void);
u16 GetSharedModelFrameIndex(void);
int CountSharedModels(void);
int GetDefaultLanguage(void);

#ifdef __cplusplus
}
#endif
