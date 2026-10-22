#include <Main/Input/touch_buttons.h>
#include <Main/System/game_math.h>
#include <Main/System/bg_image.h>
#include <Main/System/game_util.h>
#include <Main/System/state_flow.h>
#include <Main/System/prompt_screen.h>
#include <Main/System/sound_control.h>
#include <Main/Scene/language_menu.h>
#include <Main/Graphics/text_draw.h>
#include <Main/Script/script_records.h>
#include <Main/Graphics/model.h>
#include <Main/Graphics/texture_blocks.h>
#include <Main/Sound/wave_player.h>
#include <Main/Graphics/script_data.h>
#include <Main/System/system_init.h>
#include <Main/clib.h>
#include <nitro/os.h>
#include <stdio.h>

// The prompts (this file's copy; the code reads from the second word of each entry).
static const Unk_0204e96c sPrompts[0x49] = {
    {0x6c, -1, 0, -1},      {0x71, -1, 0, -1},  {0x96, -1, 0, 0x2f},    {0xb1, -1, 0, -1},      {0xc2, -1, 0, -1},
    {0xef, -1, 0, -1},      {0x10b, -1, 0, -1}, {0x148, -1, 0, -1},     {0x16f, -1, 0, -1},     {0x174, -1, 0, -1},
    {0x17d, -1, 0, -1},     {0x186, -1, 0, -1}, {0x1b3, -1, 0, -1},     {0x1f0, -1, 0, -1},     {0x22d, -1, 0, -1},
    {0x264, -1, 0, -1},     {0x27a, -1, 0, -1}, {0x283, -1, 0, -1},     {0x28c, -1, 0, -1},     {0x2a1, -1, 0, -1},
    {0x2de, -1, 0, -1},     {0x2fa, -1, 0, -1}, {0x311, -1, 0, -1},     {0x31d, -1, 0, -1},     {0x326, -1, 0, -1},
    {0x32f, -1, 0, -1},     {0x34a, -1, 0, -1}, {0x375, -1, 0, -1},     {0x392, -1, 0, -1},     {0x3b6, -1, 0, -1},
    {0x6c, -1, 0, -1},      {0x3d4, -1, 0, -1}, {0x422, -1, 0, -1},     {0x456, -1, 0, -1},     {0x48f, -1, 0, -1},
    {0x4d2, -1, 0, -1},     {0x4e3, -1, 0, -1}, {0x4ea, -1, 0, -1},     {0x509, -1, 0, -1},     {0x517, -1, 0, -1},
    {0x535, -1, 0, -1},     {0x541, -1, 0, -1}, {0x551, -1, 0, -1},     {0x55d, -1, 0, -1},     {0x569, -1, 0, -1},
    {0x573, -1, 0, -1},     {0x57f, -1, 0, -1}, {0x587, 0x1e, 0, 0x3e}, {0x58d, 0x1f, 0, 0x31}, {0x593, 0x1c, 1, 0x32},
    {0x599, 0x1d, 1, 0x33}, {0x59f, -1, 0, -1}, {0x5c2, -1, 0, -1},     {0x5ca, -1, 0, -1},     {0x5d2, -1, 0, -1},
    {0x5da, -1, 0, -1},     {0x5e2, -1, 0, -1}, {0x5ea, -1, 0, -1},     {0x5f2, -1, 0, -1},     {0x5fa, -1, 0, -1},
    {0x602, -1, 0, -1},     {0x60a, -1, 0, -1}, {0x612, 0x1b, 0, 0x30}, {0x618, -1, 0, -1},     {0x620, -1, 0, -1},
    {0x628, -1, 0, -1},     {0x630, -1, 0, -1}, {0x638, -1, 0, -1},     {0x640, -1, 0, -1},     {0x648, -1, 0, -1},
    {0x650, -1, 0, -1},     {0x658, -1, 0, -1}, {0x660, -1, 0, -1},
};

// Defined in reverse address order (see gSharedModel below).
const u16 data_0204e210[1] = {0};
const u16 data_0204e20c[1] = {0};
const u16 data_0204e208[1] = {0};
const u32 data_0204e214[1] = {0};
const PromptSpriteDef gPromptSpriteDefs[3] = {
    {0, 0xc0, 0x49, 0x131},
    {0xff, 0xc0, 0x49, 0x130},
    {0, 0xc0, 0x49, 0x132},
};
const PromptWave gPromptWaves[32] = {
    {0xdf, 0},  {0xde, 0}, {0x84, 1},  {0xa7, 1},  {0x6b, 1},  {0x19a, 1}, {0x193, 1}, {0x17e, 1},
    {0x18c, 1}, {0x99, 1}, {0xa0, 1},  {0xb5, 1},  {0x4f, 1},  {0x41, 1},  {0x3a, 1},  {0x33, 1},
    {0x48, 1},  {0xd1, 1}, {0x1b6, 1}, {0x1a8, 1}, {0x174, 1}, {0x1af, 1}, {0x1a1, 1}, {0xc3, 1},
    {0x7d, 1},  {0x32, 0}, {0x1e, 1},  {0x10, 1},  {9, 1},     {0x17, 1},  {0x16d, 1}, {0x59, 0},
};
const u16 data_0204e2c8[86] = {
    0xee0, 0xee7, 0xeef, 0xf11, 0xf15, 0xb,   0xf0a, 0xf0c, 0xf1e, 0xf21, 0xf26, 0xf29, 0xef7, 0xef9, 0xef7,
    0xef8, 0xefc, 0xefe, 0xefc, 0xefd, 0xf01, 0xf03, 0xf01, 0xf02, 0xf06, 0xf08, 0xf06, 0xf07, 0xf2f, 0xf31,
    0xf2f, 0xf30, 3,     4,     5,     0,     6,     2,     1,     7,     0xf1c, 0xa,   0xf0d, 0xef4, 0xf57,
    0xf2b, 0xee3, 0xeeb, 0xee4, 0xeec, 0xee5, 0xeed, 0xee8, 0xef0, 0xf14, 0xf18, 0xf13, 0xf17, 0xf12, 0xf16,
    0xf1e, 0xf20, 0xf1e, 0xf22, 0xf1e, 0xf23, 0xf1f, 0xf24, 0xf25, 0xf28, 0xf27, 0xf2a, 0xee9, 0xef1, 0xee6,
    0xeee, 0xef2, 0xef3, 9,     0xf10, 0xf2c, 0xc,   0xf19, 0xf1a, 0xeea, 8,
};

// The one model that the prompt, battle and result screens and others play (SetupSharedModel), and the flag stored with
// it (SetupSharedModel's b): with 0 the model holds its last frame and IsSharedModelDone reports it; with any other
// value IsSharedModelDone always returns 0.
typedef struct SharedModel {
    s32 mUnk_00;
    Model mModel;
} SharedModel;

const FontRange *data_02063988[8] = {
    (const FontRange *) data_0201a608, (const FontRange *) data_0201a608, (const FontRange *) data_0201a608,
    (const FontRange *) data_0201a608, (const FontRange *) data_0201a608, (const FontRange *) data_0201a608,
    (const FontRange *) data_0201a608, (const FontRange *) data_0201a608,
};

// Defined before gPromptSprite: with the font list above and the 2-byte tables defined in reverse, the compiler's
// size sort then keeps those tables in address order.
SharedModel gSharedModel;
PromptSprite gPromptSprite;
LangMenu gLangMenu;

void SetupPrompt(void) {
    s32 *p = &gPromptSprite.mSpriteIndex;
    u32 idx = GetFlowValue(0x14);
    u32 v;
    func_02010294(0x180, 1, 1);
    if (sPrompts[idx].mUnk_04 >= 0) {
        SetupSharedModel(sPrompts[idx].mUnk_04, sPrompts[idx].mUnk_08);
        gScriptWork.mUnk_280.mUnk_40 = 1;
    } else {
        gScriptWork.mUnk_280.mUnk_40 = 0;
    }
    LoadImage(0x237, 2);
    func_02024208();
    RequestTrack(5, 0x66);
    *p = -1;
    SetupScript(0xfbc, LookupLanguageFileId(data_020b1084.mLanguage), 0x244, 0x246, 2, 0, 0, 0, 0);
    gScriptWork.mCharFrameCount = 0;
    SetScriptRecord("LANGUAGE", data_020b1084.mLanguage);
    v = LookupLanguageFileId(data_020b1084.mLanguage);
    SetupScript(0xfbc, v, 0x244, 0x246, GetFlowValue(0x17), 0, 0, func_0200f588, 0);
    v = GetFlowValue(0x16);
    if (v != 0) {
        PushState(v);
        SetFlowValue(0x16, 0);
    }
}

// Draw callback of the prompt's script text: while a sprite is chosen, eases its slide level, sets the text's y offset
// (gScriptWork.mTextOffsetY) to the sprite's slide offset, draws the sprite and returns 0x7e; otherwise returns 0.
int DrawPromptSprite(void) {
    PromptSprite *state = &gPromptSprite;
    if (gPromptSprite.mSpriteIndex != -1) {
        int i = state->mSpriteIndex;
        const PromptSpriteDef *e = &gPromptSpriteDefs[i];
        EaseLevel(&state->mSlideLevel, 8);
        gScriptWork.mTextOffsetY = MapRangeFromZero(state->mSlideLevel, 0x4000, e->mSlideY, 0);
        func_0202a1cc();
        DrawSprite(e->mSpriteId + 0x800000, gPromptSpriteDefs[i].mX, e->mY + gScriptWork.mTextOffsetY, 0x80, 1);
        return 0x7e;
    }
    return 0;
}

void UpdatePrompt(void) {
    int id;
    u32 idx;
    if (gTouchInput.mTrail.mTouchState.mButtonState == 0x40000400) {
        id = 0xf8f;
    } else {
        id = 0xf8e;
    }
    idx = GetFlowValue(0x14);
    if (sPrompts[idx].mUnk_04 >= 0) {
        DrawSharedModel();
    }
    UpdateScript(DrawPromptSprite);
    HandlePromptEvents();
    if (gScriptWork.mEofFlags & 4) {
        if (sPrompts[idx].mUnk_0c >= 0) {
            func_0201033c(sPrompts[idx].mUnk_0c, 0);
            ReenterState();
        } else {
            PopState();
        }
    }
    if (GetScriptState() != SCRIPT_STATE_END) {
        AddTouchButtonWithSounds(0x400, 0x80, 0x60, 0, 0, id, 0, 0x168);
        DrawSprite(id, 0x80, 0x60, 0, 2);
    }
}

void LeavePrompt(void) {
    func_020102f8();
}

u8 func_0200f588(void) {
    u8 buf[0x20];
    u8 i;
    buf[0] = 0;
    i = gSystemState.mUnk_316++;
    return buf[i];
}

void SetScriptRecord(const char *key, u32 value) {
    char buf[0x20];
    sprintf(buf, "%s=%i", key, value);
    SetScriptRecordFromText(buf);
}

void HandlePromptEvents(void) {
    ScriptEvent *rec;
    PromptSprite *state = &gPromptSprite;
    int ok;
    while ((rec = NextScriptEvent()) != 0) {
        switch (rec->mKind) {
            case 6:
                ok = rec->mParam >= 0 && rec->mParam <= 3;
                if (!ok) {
                    OS_Terminate();
                }
                state->mSpriteIndex = rec->mParam - 1;
                state->mSlideLevel = 0;
                break;
            case 7:
                if (!gScriptWork.mSkipToEnd) {
                    const PromptWave *e = &gPromptWaves[rec->mParam];
                    u32 base = e->mAddLanguage == 1 ? data_020b1084.mLanguage : 0;
                    PlayWaveKeepAlive(base + e->mWaveId, 100, 0, 0x40, 0xffff, 0);
                }
                break;
        }
    }
}

void SetupLangMenu(void) {
    LangMenu *menu = &gLangMenu;
    int i;
    char buf[0x20];
    func_02010294(0x100, 1, 0);
    LoadImage(0xdaa, 3);
    func_02024208();
    RequestTrack(0, 0x80);
    i = 2;
    gLangMenu.mCount = 0;
    for (;; i++) {
        if (GetFileValueType(0x246, i) == 4) {
            const u8 *s = GetFileString(0x246, i);
            if (s[0] != 0) {
                CopyUpperWord(buf, s);
                if (StringsEqual("LAST", buf) == 1) {
                    break;
                }
                if (menu->mCount >= LANG_MENU_MAX) {
                    OS_Terminate();
                }
                menu->mEntries[menu->mCount].mTextId = i;
                menu->mEntries[menu->mCount].mIndex = menu->mCount - 1;
                strcpy(menu->mEntries[menu->mCount].mText, buf);
                menu->mCount++;
            }
        }
    }
    menu->mSelected = GetFlowValue(0x15);
    if (menu->mCount <= 1) {
        OS_Terminate();
    }
    if (menu->mSelected >= menu->mCount) {
        OS_Terminate();
    }
}

void LeaveLangMenu(void) {
    func_020102f8();
}

void UpdateLangMenu(void) {
    LangMenu *menu = &gLangMenu;
    func_0200b920(0x10, "CS (U/D): %i", gLangMenu.mSelected - 1);
    func_0200b920(0x20, "Language (LB/RB): %s", GetLanguageText(data_020b1084.mLanguage + 2, data_020b1084.mLanguage));
    if ((gSystemState.mKeysPressed & 0x80) && menu->mSelected > 1) {
        menu->mSelected--;
    } else if (gSystemState.mKeysPressed & 0x40) {
        if (menu->mSelected < menu->mCount - 1) {
            menu->mSelected++;
        }
    }
    if ((gSystemState.mKeysPressed & 0x200) && data_020b1084.mLanguage != 0) {
        data_020b1084.mLanguage--;
    }
    DrawText(0x80, 0x60, data_0201a6c6, menu->mEntries[menu->mSelected].mText, 2, 0xe0, 0x10, 1);
    if (gSystemState.mKeysPressed & 1) {
        SetFlowValue(0x14, menu->mEntries[menu->mSelected].mIndex);
        SetFlowValue(0x15, menu->mSelected);
        func_02010364(menu->mEntries[menu->mSelected].mTextId, 0xc);
    }
}

void SetupSharedModel(u32 a, int b) {
    if (a >= CountSharedModels()) {
        OS_Terminate();
    }
    StartModel(&gSharedModel.mModel, a * 2 + 0x1f5, a * 2 + 0x1f4, 2, 0);
    gSharedModel.mUnk_00 = b;
    if (b == 0) {
        gSharedModel.mModel.mUnk_172_13 = 1;
    }
}

void DrawSharedModel(void) {
    u8 params[0x44];
    int idx = (gSharedModel.mModel.mUnk_170 - 0x1f5) / 2;
    int a, x, y, z;
    func_020242a8(params, 0xff, 0, 0, 0x1000, 0x8000, -1, 0x100);
    params[0x43] |= 8;
    UpdateModelScript(&gSharedModel.mModel);
    DrawModelFileEx(gSharedModel.mModel.mUnk_170, params, &gSharedModel.mModel, 0);
    a = GetFileValue(0x235, idx * 4 + 2);
    if (gSharedModel.mModel.mUnk_14c.mUnk_2 >= a) {
        x = GetFileValue(0x235, idx * 4 + 4);
        y = GetFileValue(0x235, idx * 4 + 5);
        z = GetFileValue(0x235, idx * 4 + 3);
        if (gSharedModel.mModel.mUnk_14c.mUnk_2 < a + 4) {
            x = MapRange(gSharedModel.mModel.mUnk_14c.mUnk_2, a, a + 4, x - 0x78, x);
        }
        DrawSprite(z * 6 + 0xee, x, y, 0x64, 1);
    }
}

int IsSharedModelDone(void) {
    if (gSharedModel.mUnk_00 != 0) {
        return 0;
    }
    return gSharedModel.mModel.mUnk_172_10;
}

u16 GetSharedModelFrameIndex(void) {
    return gSharedModel.mModel.mUnk_14c.mUnk_2;
}

// Number of rows of table 0x235 (column 2 up to -1): the model indices SetupSharedModel accepts.
int CountSharedModels(void) {
    int n = 0;
    while (GetFileValue(0x235, 2 + n * 4) != -1) {
        n++;
    }
    return n;
}

// Always 0 in this build; the owner info it reads is not used.
int GetDefaultLanguage(void) {
    OSOwnerInfo info;
    OS_GetOwnerInfo(&info);
    return 0;
}
