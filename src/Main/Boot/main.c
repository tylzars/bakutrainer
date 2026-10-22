#include <nitro/os.h>
#include <Main/System/bg_image.h>
#include <Main/Save/save_blocks.h>
#include <Main/System/screen_fade.h>
#include <Main/System/system_init.h>
#include <Main/Scene/stage_progress.h>
#include <Main/Scene/stage_info_panel.h>
#include <Main/System/prompt_screen.h>
#include <Main/Graphics/sprite_slots.h>
#include <Main/System/save_data.h>
#include <Main/System/sound_control.h>
#include <Main/System/state_flow.h>
#include <Main/Sound/wave_player.h>
#include <Main/Graphics/sprite_script.h>
#include <Main/Graphics/script_data.h>
#include <Main/Scene/slide_transition.h>
#include <nitro/init.h>

void main(void) {
    GameFlow *flow = &gGameFlow;
    int i;

    gSystemState.mFlags = 0;
    if (*(vu16 *) 0x027ffc40 != 2) {
        gSystemState.mUnk_2a7 = 0;
        InitSystem(0xfa);
        ResetVramBanks();
    }
    gSystemState.mFlags |= 0x10820000;
    SetFaded(3);
    func_02010d34();
    if (*(vu16 *) 0x027ffc40 != 2) {
        InitSaveStore(SAVE_BLOCK_ID, SAVE_STORE_LARGE, 2, 0x1c6, func_02010648, -1);
        func_02010c94();
        ClearBackupTimer();
    }
    gSystemState.mUnk_310 = 0x40;
    gSystemState.mUnk_493 = 1;
    gSystemState.mUnk_494 = 1;
    InitWavePlayer();
    InitSoundCtl(0, 0x80);
    InitGameFlow();
    ClearObjSlots();
    ClearAnimSlots();
    ResetAllProgress();
    ResetTitleReturnState();
    StartSprite(&data_020b6ee0.mSelectionSprite, data_0201abec[2]);
    data_020b6ee0.mUnk_a91b = 0;
    func_0200dbac();
    data_020b6ee0.mUnk_a91a = 1;
    PushState(1);
    PopState();

    while (1) {
        while (flow->mState != flow->mNextState || flow->mReenterRequested == 1) {
            flow->mReenterRequested = 0;
            if (gStateLeaveFuncs[flow->mState] != NULL) {
                gStateLeaveFuncs[flow->mState]();
            }
            if (flow->mNextState >= STATE_COUNT) {
                OS_Terminate();
            }
            flow->mPrevState = flow->mState;
            flow->mState = flow->mNextState;
            flow->mStateFlag = 0;
            if (gStateEnterFuncs[flow->mState] != NULL) {
                gStateEnterFuncs[flow->mState]();
            }
        }
        if (gStateUpdateFuncs[flow->mState] == NULL) {
            OS_Terminate();
        }
        gStateUpdateFuncs[flow->mState]();
        if (gStatePromptScreenEnabled[flow->mState] != 0 && (gSystemState.mKeysPressed & 8)) {
            RunPromptScreen();
        }
        UpdateNodeState();
        UpdateProgress();
        UpdateObjSlots();
        UpdateAnimSlots();
        flow->mFrame++;
        UpdateKeepAliveWaves();
        UpdateSoundCtl();
        UpdateSystemAndWait();
    }
}
