#include <Main/Save/save_blocks.h>
#include <Main/System/screen_fade.h>
#include <Main/System/bg_image.h>
#include <Main/System/system_init.h>
#include <Main/Scene/stage_info_panel.h>
#include <Main/Unknown/game_util.h>
#include <Main/System/sound_control.h>
#include <Main/System/state_flow.h>
#include <Main/Scene/page_viewer.h>
#include <Main/Graphics/texture_blocks.h>

PageViewState data_02098d74;

void func_02006fc0(void) {
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
    data_02098d74.mUnk_04 = 0;
    func_02007168(1);
    if (data_020b6ee0.mSaveRequested == 1) {
        RunSaveScreen(0);
        data_020b6ee0.mSaveRequested = 0;
    }
}

void func_02007040(void) {
    PageViewState *p = &data_02098d74;
    switch (data_02098d74.mState) {
        case 0:
            func_0200bb14(0x80, 0x60, 0x600, 0xf8e, 0, 0, 0, 1);
            break;
        case 1:
            func_0200bb14(0, 0xa1, 0x600, 14, 0, 0, 90, 1);
            if (p->mUnk_08 == 0) {
                func_0200bb14(0xff, 0xa1, 0x601, 21, 0, 0, 0x17d, 1);
            }
            break;
    }
}

void func_020070c4(void) {
    PageViewState *p = &data_02098d74;
    func_02007040();
    switch (data_02098d74.mState) {
        case 0:
            if (func_0200bd80(0x20000000, 0x600) != 0) {
                p->mUnk_04 = 0;
                func_02007168(1);
            }
            break;
        case 1:
            if (func_0200bd80(0x20000000, 0x600) != 0) {
                if (data_020b6ee0.mUnk_a91b != 0) {
                    data_020b6ee0.mUnk_a91b = 0;
                    PushState(10);
                    PopState();
                } else {
                    PushState(1);
                    PopState();
                }
            }
            if (p->mUnk_08 == 0) {
                if (func_0200bd80(0x20000000, 0x601) != 0) {
                    p->mUnk_04++;
                    if (p->mUnk_04 == 11) {
                        p->mUnk_04 = 0;
                    }
                    func_02007168(1);
                }
            } else {
                p->mUnk_08--;
            }
            break;
    }
}

void func_02007168(int state) {
    PageViewState *p = &data_02098d74;
    SetFaded(3);
    switch (state) {
        case 0:
            break;
        case 1:
            LoadImageEx(p->mUnk_04 + 0x1e9, 3, 0, 2);
            p->mUnk_08 = 120;
            break;
    }
    p->mState = state;
}

void func_0200719c(void) {
    SetFaded(3);
}
