#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Fade state of a screen (SystemState mFadeState, one per screen; IsFadeState compares it). The level register is at
// its top value (0x8011), stepping down to 0, stepping up to the top, or at 0.
#define FADE_STATE_FADED 0
#define FADE_STATE_TO_CLEAR 1
#define FADE_STATE_TO_FADED 2
#define FADE_STATE_CLEAR 3

// Entry of a circular doubly linked list of 16-bit indices; entry 0 is the list head.
typedef struct IndexNode16 {
    u8 mUnk_0[0xa];
    u16 mPrev;
    u16 mNext;
    u8 mUnk_e[2];
} IndexNode16;

void SetupImageLayer(int a, int b, u32 c, u32 d);
u8 *UploadPalette(u8 *palette, u32 count, int a, int layers);
void UpdateFade(void);
void RunImageWithSprite(int a, int b, int c, u16 image, u16 sprite);
int IsFadeState(int layers, int state);
int TestBits(int a, int b);
void StartFade(int a);
void StartFadeClear(int a);
void FadeClearAndWait(int a);
void FadeAndWait(int a);
void SetFaded(int a);
void SetFadeClear(int a);
int UpdateWaitForReleaseOrTimeout(u32 *frames, int *touched, u32 minFrames, u32 maxFrames, int layers);
void RunImage(int a, int b, int c, int d, int e);
void MoveIndexNode16ToEnd(IndexNode16 *list, u32 index);

typedef struct IndexNode {
    u16 mUnk_0;
    u16 mPrev;
    u16 mNext;
    u16 mUnk_6;
} IndexNode;
void MoveIndexNodeToEnd(IndexNode *list, u32 index);
#ifdef __cplusplus
}
#endif
