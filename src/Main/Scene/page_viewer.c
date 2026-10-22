#include <Main/Save/save_blocks.h>
#include <Main/System/screen_fade.h>
#include <Main/System/bg_image.h>
#include <Main/System/system_init.h>
#include <Main/Scene/stage_info_panel.h>
#include <Main/System/game_util.h>
#include <Main/System/sound_control.h>
#include <Main/System/state_flow.h>
#include <Main/Scene/page_viewer.h>
#include <Main/Graphics/texture_blocks.h>

// Number of pages: images 0x1e9 to 0x1f3, shown in turn by button 0x601.
#define PAGE_VIEWER_PAGE_COUNT 11

PageViewState gPageViewState;

void SetupPageViewer(void) {
    UpdateSystemAndWait();
    SetFaded(3);
    ResetKeys();
    ResetVramBanks();
    ReleaseTextureLayers();
    SetLcdSwap(1);
    LoadImage(0xdaa, 3);
    SetupTextureLayer(0x10cb, 0x10ca, 0x10cc, 2, 1);
    SetupTextureLayer(0x10cb, 0x10ca, 0x10cc, 2, 2);
    RequestTrack(1, 0x80);
    gPageViewState.mPageIndex = 0;
    StartPageViewerState(1);
    if (data_020b6ee0.mSaveRequested == 1) {
        RunSaveScreen(0);
        data_020b6ee0.mSaveRequested = 0;
    }
}

// The state 0 branch (button 0x600 at 0x80, 0x60) never runs in this build: every StartPageViewerState call passes 1.
void DrawPageViewerButtons(void) {
    PageViewState *p = &gPageViewState;
    switch (gPageViewState.mState) {
        case 0:
            func_0200bb14(0x80, 0x60, 0x600, 0xf8e, 0, 0, 0, 1);
            break;
        case 1:
            func_0200bb14(0, 0xa1, 0x600, 14, 0, 0, 90, 1);
            if (p->mNextPageTimer == 0) {
                func_0200bb14(0xff, 0xa1, 0x601, 21, 0, 0, 0x17d, 1);
            }
            break;
    }
}

// Button 0x600 leaves for game state 10 when data_020b6ee0.mUnk_a91b is set (clearing it), else for the title screen
// (state 1); 0x601 shows the next page. State 0 is never entered in this build.
void UpdatePageViewer(void) {
    PageViewState *p = &gPageViewState;
    DrawPageViewerButtons();
    switch (gPageViewState.mState) {
        case 0:
            if (TestButton(0x20000000, 0x600) != 0) {
                p->mPageIndex = 0;
                StartPageViewerState(1);
            }
            break;
        case 1:
            if (TestButton(0x20000000, 0x600) != 0) {
                if (data_020b6ee0.mUnk_a91b != 0) {
                    data_020b6ee0.mUnk_a91b = 0;
                    PushState(10);
                    PopState();
                } else {
                    PushState(1);
                    PopState();
                }
            }
            if (p->mNextPageTimer == 0) {
                if (TestButton(0x20000000, 0x601) != 0) {
                    p->mPageIndex++;
                    if (p->mPageIndex == PAGE_VIEWER_PAGE_COUNT) {
                        p->mPageIndex = 0;
                    }
                    StartPageViewerState(1);
                }
            } else {
                p->mNextPageTimer--;
            }
            break;
    }
}

// Every call passes 1 (load the page's image and start the next-page countdown).
void StartPageViewerState(int state) {
    PageViewState *p = &gPageViewState;
    SetFaded(3);
    switch (state) {
        case 0:
            break;
        case 1:
            LoadImageEx(p->mPageIndex + 0x1e9, 3, 0, 2);
            p->mNextPageTimer = 120;
            break;
    }
    p->mState = state;
}

void LeavePageViewer(void) {
    SetFaded(3);
}
