#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// State of the page screen: its state (StartPageViewerState), the page shown and a countdown before button 0x601
// (next page) appears.
typedef struct PageViewState {
    s32 mState;
    s32 mPageIndex;
    s32 mNextPageTimer;
} PageViewState;

extern PageViewState gPageViewState;

void SetupPageViewer(void);
void DrawPageViewerButtons(void);
void UpdatePageViewer(void);
void StartPageViewerState(int state);
void LeavePageViewer(void);

#ifdef __cplusplus
}
#endif
