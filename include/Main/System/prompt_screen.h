#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// State of the prompt screen (PromptScreen mState): the buttons and text slide in, stay in place, slide out. The
// buttons are touch buttons only in PROMPT_SCREEN_STATE_IN_PLACE.
#define PROMPT_SCREEN_STATE_SLIDE_IN 0
#define PROMPT_SCREEN_STATE_IN_PLACE 1
#define PROMPT_SCREEN_STATE_SLIDE_OUT 2
// Ids of the prompt screen's two buttons (drawn by DrawPromptScreen, tested with func_0200bd80).
#define PROMPT_SCREEN_BUTTON_FIRST 0x400
#define PROMPT_SCREEN_BUTTON_SECOND 0x401

// State of the two-button prompt screen: the saved display setup restored when it closes (mUnk_6c and mUnk_6e hold
// the sub screen's 0x04001050 and 0x04001052 registers, zeroed while it is open), and the position of the buttons and
// the text while they slide in and out.
typedef struct PromptScreen {
    // func_020101d0 also reads entry 1.
    u16 mSavedImageIds[2];
    u8 mUnk_04[0x0c - 0x04];
    s32 mSavedWindowBits;
    s32 mSavedSubWindowBits;
    s32 mSavedBg3X;
    s32 mSavedBg3Y;
    s32 mSavedSubBg3X;
    s32 mSavedSubBg3Y;
    u16 mSavedSubColor0;
    u16 mSavedSubColor1;
    s32 mTouchSamplingWasOn;
    s32 mSavedTouchEnabled;
    s32 mSavedTouchDown;
    s32 mSavedTouchReleased;
    s32 mSavedTouchPressed;
    s32 mSavedTouchReleaseTimer;
    s32 mSavedUseTouchReleaseTimer;
    // The buttons' x offsets from 128 and the text's y offset from 17, in 1/256 pixel.
    s32 mFirstButtonX;
    s32 mSecondButtonX;
    s32 mTextY;
    u8 mUnk_50[0x64 - 0x50];
    s32 mState;
    s32 mReleasedButtonId;
    u16 mUnk_6c;
    u16 mUnk_6e;
} PromptScreen;
extern PromptScreen gPromptScreen;

// A prompt (0x49 of them, indexed by the prompt number, state variable 0x14): the message shown, an id passed with
// the word after it to SetupSharedModel when it is not negative, and the prompt that follows (-1: none).
// language_menu.c and game_system.c each hold a copy of the table.
typedef struct Unk_0204e96c {
    /* 00 */ s32 mUnk_00;
    /* 04 */ s32 mUnk_04;
    /* 08 */ s32 mUnk_08;
    /* 0c */ s32 mUnk_0c;
} Unk_0204e96c;

int OpenPromptScreen(int *a, int *b);
void ClosePromptScreen(void);
void DrawPromptScreen(void);
void RunPromptScreen(void);
void func_020101d0(void);
void func_02010294(int a, int b, int c);
void func_020102f8(void);
void func_0201033c(int a, int b);
void func_02010364(int a, int b);

#ifdef __cplusplus
}
#endif
