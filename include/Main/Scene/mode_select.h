#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Values of ModeSelectState.mSlideState (GetModeSlideState).
#define MODE_SLIDE_STATE_NONE 0
// The slide every mode starts with; it moves out and ends.
#define MODE_SLIDE_STATE_START 1
// The slide a mode ends with; it stays, and the caller then goes to the result screen.
#define MODE_SLIDE_STATE_END 2

// State of the mode selection screen, and of the slide every mode shows at its start and end.
typedef struct ModeSelectState {
    s32 mScoreThreshold;
    s32 mBestScore;
    u8 mSlideState;
    u8 mPad_09[3];
    s32 mSlideX;
    s32 mSlideHoldTimer;
    // StartModeSlide's b. Every start slide passes 0, so it is read only for the end slide. The modes pass it on to
    // StartResultScreen (data_020b10fc.mUnk_0c).
    s32 mScoreThresholdMet;
} ModeSelectState;

extern s32 gKeepSelectedMode;
extern ModeSelectState gModeSelectState;
extern u32 gPrevTimeRemaining;

void SelectMode(int mode);
void SetupModeSelect(void);
void DrawModeSelect(void);
void UpdateModeSelect(void);
void LeaveModeSelect(void);
void StartResultScreen(int a, u32 b);
void func_020059cc(u32 value);
void AddCappedAt999Shifted(u32 *value, u32 add, int shift);
void AddCappedAt5999(u32 *value, u32 add);
int UpdateTimeRemaining(u32 value);
void StartModeSlide(int a, int b);
int UpdateModeSlide(int layer);
u8 GetModeSlideState(void);
s32 GetScoreThresholdMet(void);

#ifdef __cplusplus
}
#endif
