#include <Main/Save/save_blocks.h>
#include <Main/System/save_data.h>
#include <Main/Input/mic_input.h>
#include <Main/System/random_angle.h>
#include <Main/System/screen_fade.h>
#include <Main/System/game_math.h>
#include <Main/System/bg_image.h>
#include <Main/Scene/stage_info_panel.h>
#include <Main/Scene/stage_map_menu.h>
#include <Main/System/state_flow.h>
#include <Main/System/prompt_screen.h>
#include <Main/System/sound_control.h>
#include <Main/Graphics/sprite_slots.h>
#include <Main/Scene/slide_transition.h>
#include <Main/Unknown/game_util.h>
#include <Main/Graphics/text_draw.h>
#include <Main/Graphics/model.h>
#include <Main/Graphics/texture_blocks.h>
#include <Main/Sound/wave_player.h>
#include <Main/Graphics/script_data.h>
#include <Main/Input/touch_buttons.h>
#include <Main/System/system_init.h>
#include <Main/Graphics/gx_pool.h>
#include <nitro/os.h>
#include <stdio.h>

// The record buffer AllocSaveRecord returns; only its first 0x1c bytes (as in SaveRecord) are declared here.
typedef struct SaveRecordHead {
    u8 mUnk_00[0x10];
    s32 mWritten;
    s32 mReachedNodeIndex;
    s32 mLastNodeDone;
} SaveRecordHead;

#define SLIDE_STEP 0x1e00
#define SLIDE_STEP_SMALL 0x780
#define SLIDE_FAR 0x18000
#define SLIDE_MID 0x8000
#define SLIDE_NEAR 0x4000

int UpdateTitleSlide(void) {
    s32 x;
    int i;
    s32 sum;
    switch (gTitleScreen.mSlideState) {
        case TITLE_SLIDE_STATE_IN:
            switch (gTitleScreen.mState) {
                case 1:
                    if (gTitleScreen.mSlideDirection > 0) {
                        x = gTitleScreen.mSlideOffsets[0] + SLIDE_STEP;
                        if (x >= 0) {
                            x = 0;
                        }
                        gTitleScreen.mSlideOffsets[0] = x;
                        x = gTitleScreen.mSlideOffsets[1] + SLIDE_STEP;
                        if (x >= 0) {
                            x = 0;
                        }
                        gTitleScreen.mSlideOffsets[1] = x;
                        x = gTitleScreen.mSlideOffsets[2] + SLIDE_STEP;
                        if (x >= 0) {
                            x = 0;
                        }
                        gTitleScreen.mSlideOffsets[2] = x;
                    } else {
                        x = gTitleScreen.mSlideOffsets[0] - SLIDE_STEP;
                        if (x < 0) {
                            x = 0;
                        }
                        gTitleScreen.mSlideOffsets[0] = x;
                        x = gTitleScreen.mSlideOffsets[1] - SLIDE_STEP;
                        if (x < 0) {
                            x = 0;
                        }
                        gTitleScreen.mSlideOffsets[1] = x;
                        x = gTitleScreen.mSlideOffsets[2] - SLIDE_STEP;
                        if (x < 0) {
                            x = 0;
                        }
                        gTitleScreen.mSlideOffsets[2] = x;
                    }
                    if (gTitleScreen.mSlideOffsets[0] == 0 && gTitleScreen.mSlideOffsets[1] == 0 &&
                        gTitleScreen.mSlideOffsets[2] == 0) {
                        gTitleScreen.mSlideState = TITLE_SLIDE_STATE_IN_PLACE;
                    }
                    break;
                case 2:
                case 5:
                    if (gTitleScreen.mSlideDirection > 0) {
                        x = gTitleScreen.mSlideOffsets[0] + SLIDE_STEP;
                        if (x >= 0) {
                            x = 0;
                        }
                        gTitleScreen.mSlideOffsets[0] = x;
                        x = gTitleScreen.mSlideOffsets[1] + SLIDE_STEP;
                        if (x >= 0) {
                            x = 0;
                        }
                        gTitleScreen.mSlideOffsets[1] = x;
                        x = gTitleScreen.mSlideOffsets[2] + SLIDE_STEP;
                        if (x >= 0) {
                            x = 0;
                        }
                        gTitleScreen.mSlideOffsets[2] = x;
                    } else {
                        x = gTitleScreen.mSlideOffsets[0] - SLIDE_STEP;
                        if (x < 0) {
                            x = 0;
                        }
                        gTitleScreen.mSlideOffsets[0] = x;
                        x = gTitleScreen.mSlideOffsets[1] - SLIDE_STEP;
                        if (x < 0) {
                            x = 0;
                        }
                        gTitleScreen.mSlideOffsets[1] = x;
                        x = gTitleScreen.mSlideOffsets[2] - SLIDE_STEP;
                        if (x < 0) {
                            x = 0;
                        }
                        gTitleScreen.mSlideOffsets[2] = x;
                    }
                    x = gTitleScreen.mSlideOffsets[3] + SLIDE_STEP_SMALL;
                    if (x >= 0) {
                        x = 0;
                    }
                    gTitleScreen.mSlideOffsets[3] = x;
                    x = gTitleScreen.mSlideOffsets[4] - SLIDE_STEP_SMALL;
                    if (x < 0) {
                        x = 0;
                    }
                    gTitleScreen.mSlideOffsets[4] = x;
                    if (gTitleScreen.mSlideOffsets[0] == 0 && gTitleScreen.mSlideOffsets[1] == 0 &&
                        gTitleScreen.mSlideOffsets[2] == 0 && gTitleScreen.mSlideOffsets[3] == 0 && x == 0) {
                        gTitleScreen.mSlideState = TITLE_SLIDE_STATE_IN_PLACE;
                    }
                    break;
                case 4:
                    x = gTitleScreen.mSlideOffsets[0] + SLIDE_STEP_SMALL;
                    if (x >= 0) {
                        x = 0;
                    }
                    gTitleScreen.mSlideOffsets[0] = x;
                    x = gTitleScreen.mSlideOffsets[1] - SLIDE_STEP_SMALL;
                    if (x < 0) {
                        x = 0;
                    }
                    gTitleScreen.mSlideOffsets[1] = x;
                    for (i = 0; i < 1; i++) {
                        x = gTitleScreen.mSlideOffsets[i + 2] - SLIDE_STEP;
                        if (x < 0) {
                            x = 0;
                        }
                        gTitleScreen.mSlideOffsets[i + 2] = x;
                    }
                    sum = 0;
                    for (i = 0; i < 8; i++) {
                        sum += gTitleScreen.mSlideOffsets[i];
                    }
                    if (sum == 0) {
                        gTitleScreen.mSlideState = TITLE_SLIDE_STATE_IN_PLACE;
                    }
                    break;
                case 3:
                    x = gTitleScreen.mSlideOffsets[0] - SLIDE_STEP;
                    if (x < 0) {
                        x = 0;
                    }
                    gTitleScreen.mSlideOffsets[0] = x;
                    x = gTitleScreen.mSlideOffsets[4] - SLIDE_STEP_SMALL;
                    if (x < 0) {
                        x = 0;
                    }
                    gTitleScreen.mSlideOffsets[4] = x;
                    if (gTitleScreen.mSlideOffsets[0] == 0 && x == 0) {
                        gTitleScreen.mSlideState = TITLE_SLIDE_STATE_IN_PLACE;
                    }
                    break;
                case 6:
                    x = gTitleScreen.mSlideOffsets[0] - SLIDE_STEP;
                    if (x < 0) {
                        x = 0;
                    }
                    gTitleScreen.mSlideOffsets[0] = x;
                    x = gTitleScreen.mSlideOffsets[1] - SLIDE_STEP;
                    if (x < 0) {
                        x = 0;
                    }
                    gTitleScreen.mSlideOffsets[1] = x;
                    x = gTitleScreen.mSlideOffsets[3] + SLIDE_STEP_SMALL;
                    if (x >= 0) {
                        x = 0;
                    }
                    gTitleScreen.mSlideOffsets[3] = x;
                    x = gTitleScreen.mSlideOffsets[4] - SLIDE_STEP_SMALL;
                    if (x < 0) {
                        x = 0;
                    }
                    gTitleScreen.mSlideOffsets[4] = x;
                    if (gTitleScreen.mSlideOffsets[0] == 0 && gTitleScreen.mSlideOffsets[1] == 0 &&
                        gTitleScreen.mSlideOffsets[3] == 0 && x == 0) {
                        gTitleScreen.mSlideState = TITLE_SLIDE_STATE_IN_PLACE;
                    }
                    break;
            }
            break;
        case TITLE_SLIDE_STATE_OUT:
            switch (gTitleScreen.mState) {
                case 1:
                    x = gTitleScreen.mSlideOffsets[0] - SLIDE_STEP;
                    if (x < -SLIDE_FAR) {
                        x = -SLIDE_FAR;
                    }
                    gTitleScreen.mSlideOffsets[0] = x;
                    if (x <= -SLIDE_MID) {
                        x = gTitleScreen.mSlideOffsets[1] - SLIDE_STEP;
                        if (x < -SLIDE_FAR) {
                            x = -SLIDE_FAR;
                        }
                        gTitleScreen.mSlideOffsets[1] = x;
                    }
                    if (gTitleScreen.mSlideOffsets[1] <= -2 * SLIDE_MID) {
                        x = gTitleScreen.mSlideOffsets[2] - SLIDE_STEP;
                        if (x < -SLIDE_FAR) {
                            x = -SLIDE_FAR;
                        }
                        gTitleScreen.mSlideOffsets[2] = x;
                    }
                    if (gTitleScreen.mSlideOffsets[0] == -SLIDE_FAR && gTitleScreen.mSlideOffsets[1] == -SLIDE_FAR &&
                        gTitleScreen.mSlideOffsets[2] == -SLIDE_FAR) {
                        return 1;
                    }
                    break;
                case 2:
                case 5:
                    if (gTitleScreen.mSlideDirection > 0) {
                        x = gTitleScreen.mSlideOffsets[0] + SLIDE_STEP;
                        if (x >= SLIDE_FAR) {
                            x = SLIDE_FAR;
                        }
                        gTitleScreen.mSlideOffsets[0] = x;
                        if (x >= 2 * SLIDE_NEAR) {
                            x = gTitleScreen.mSlideOffsets[1] + SLIDE_STEP;
                            if (x >= SLIDE_FAR) {
                                x = SLIDE_FAR;
                            }
                            gTitleScreen.mSlideOffsets[1] = x;
                        }
                        if (gTitleScreen.mSlideOffsets[1] >= 4 * SLIDE_NEAR) {
                            x = gTitleScreen.mSlideOffsets[2] + SLIDE_STEP;
                            if (x >= SLIDE_FAR) {
                                x = SLIDE_FAR;
                            }
                            gTitleScreen.mSlideOffsets[2] = x;
                        }
                    } else {
                        x = gTitleScreen.mSlideOffsets[0] - SLIDE_STEP;
                        if (x < -SLIDE_FAR) {
                            x = -SLIDE_FAR;
                        }
                        gTitleScreen.mSlideOffsets[0] = x;
                        if (x <= -2 * SLIDE_NEAR) {
                            x = gTitleScreen.mSlideOffsets[1] - SLIDE_STEP;
                            if (x < -SLIDE_FAR) {
                                x = -SLIDE_FAR;
                            }
                            gTitleScreen.mSlideOffsets[1] = x;
                        }
                        if (gTitleScreen.mSlideOffsets[1] <= -4 * SLIDE_NEAR) {
                            x = gTitleScreen.mSlideOffsets[2] - SLIDE_STEP;
                            if (x < -SLIDE_FAR) {
                                x = -SLIDE_FAR;
                            }
                            gTitleScreen.mSlideOffsets[2] = x;
                        }
                    }
                    x = gTitleScreen.mSlideOffsets[3] - SLIDE_STEP_SMALL;
                    if (x < -SLIDE_NEAR) {
                        x = -SLIDE_NEAR;
                    }
                    gTitleScreen.mSlideOffsets[3] = x;
                    x = gTitleScreen.mSlideOffsets[4] + SLIDE_STEP_SMALL;
                    if (x >= SLIDE_NEAR) {
                        x = SLIDE_NEAR;
                    }
                    gTitleScreen.mSlideOffsets[4] = x;
                    sum = gTitleScreen.mSlideDirection * SLIDE_FAR;
                    if (gTitleScreen.mSlideOffsets[0] == sum && gTitleScreen.mSlideOffsets[1] == sum &&
                        gTitleScreen.mSlideOffsets[2] == sum && gTitleScreen.mSlideOffsets[3] == -SLIDE_NEAR &&
                        x == SLIDE_NEAR) {
                        return 1;
                    }
                    break;
                case 4:
                    x = gTitleScreen.mSlideOffsets[0] - SLIDE_STEP_SMALL;
                    if (x < -SLIDE_NEAR) {
                        x = -SLIDE_NEAR;
                    }
                    gTitleScreen.mSlideOffsets[0] = x;
                    x = gTitleScreen.mSlideOffsets[1] + SLIDE_STEP_SMALL;
                    if (x >= SLIDE_NEAR) {
                        x = SLIDE_NEAR;
                    }
                    gTitleScreen.mSlideOffsets[1] = x;
                    for (i = 0; i < 1; i++) {
                        x = gTitleScreen.mSlideOffsets[i + 2] + SLIDE_STEP;
                        if (x >= SLIDE_FAR) {
                            x = SLIDE_FAR;
                        }
                        gTitleScreen.mSlideOffsets[i + 2] = x;
                    }
                    if (gTitleScreen.mSlideOffsets[0] == -SLIDE_NEAR && gTitleScreen.mSlideOffsets[1] == SLIDE_NEAR) {
                        for (i = 0; i < 1; i++) {
                            if (gTitleScreen.mSlideOffsets[i + 2] < SLIDE_FAR) {
                                return 0;
                            }
                        }
                        return 1;
                    }
                    break;
                case 3:
                    x = gTitleScreen.mSlideOffsets[0] + SLIDE_STEP;
                    if (x >= SLIDE_FAR) {
                        x = SLIDE_FAR;
                    }
                    gTitleScreen.mSlideOffsets[0] = x;
                    x = gTitleScreen.mSlideOffsets[4] + SLIDE_STEP_SMALL;
                    if (x >= SLIDE_NEAR) {
                        x = SLIDE_NEAR;
                    }
                    gTitleScreen.mSlideOffsets[4] = x;
                    if (gTitleScreen.mSlideOffsets[0] == SLIDE_FAR && x == SLIDE_NEAR) {
                        return 1;
                    }
                    break;
                case 6:
                    x = gTitleScreen.mSlideOffsets[0] + SLIDE_STEP;
                    if (x >= SLIDE_FAR) {
                        x = SLIDE_FAR;
                    }
                    gTitleScreen.mSlideOffsets[0] = x;
                    if (x >= 4 * SLIDE_NEAR) {
                        x = gTitleScreen.mSlideOffsets[1] + SLIDE_STEP;
                        if (x >= SLIDE_FAR) {
                            x = SLIDE_FAR;
                        }
                        gTitleScreen.mSlideOffsets[1] = x;
                    }
                    x = gTitleScreen.mSlideOffsets[3] - SLIDE_STEP_SMALL;
                    if (x < -SLIDE_NEAR) {
                        x = -SLIDE_NEAR;
                    }
                    gTitleScreen.mSlideOffsets[3] = x;
                    x = gTitleScreen.mSlideOffsets[4] + SLIDE_STEP_SMALL;
                    if (x >= SLIDE_NEAR) {
                        x = SLIDE_NEAR;
                    }
                    gTitleScreen.mSlideOffsets[4] = x;
                    if (gTitleScreen.mSlideOffsets[0] == SLIDE_FAR && gTitleScreen.mSlideOffsets[1] == SLIDE_FAR &&
                        gTitleScreen.mSlideOffsets[3] == -SLIDE_NEAR && x == SLIDE_NEAR) {
                        return 1;
                    }
                    break;
            }
            break;
    }
    return 0;
}

// Also starts the slide-out, in direction b; UpdateTitleScreen moves to state a once it is done.
void RequestTitleState(int a, int b) {
    gTitleScreen.mNextState = a;
    gTitleScreen.mSlideState = TITLE_SLIDE_STATE_OUT;
    gTitleScreen.mSlideDirection = b;
}

// Copies of data_020b1084.mUnk_04 and mUnk_08 that state 6's buttons change (0x709 / 0x70a and 0x70b / 0x70c);
// CloseMicEdit(1) writes them back when they differ.
typedef struct MicEdit {
    u32 mUnk_00;
    u32 mUnk_04;
    // Falling peak of state 6's mic value: raised to each larger value, drawn as sprite 0xe01 + mPeak while not 0 and
    // lowered by one per frame; 0 while gSystemState.mUnk_492 is set.
    u32 mPeak;
} MicEdit;

static MicEdit sMicEdit;
WaveStream gTitleStream;
TitleScreen gTitleScreen;

// Also stops the title stream; CloseMicEdit starts it again.
void OpenMicEdit(void) {
    CloseWaveStream(&gTitleStream);
    sMicEdit.mUnk_00 = data_020b1084.mUnk_04;
    sMicEdit.mUnk_04 = data_020b1084.mUnk_08;
    InitMic(0x400, data_020b1084.mUnk_04);
    StartMicSampling(0x82e, 0);
}

// With a = 1 (button 0x101) writes the copies back and saves when they changed; a = 0 (0x102) drops them. Starts the
// title stream again.
void CloseMicEdit(int a) {
    ReleaseMic();
    if (a != 0) {
        if (sMicEdit.mUnk_04 != data_020b1084.mUnk_08 || sMicEdit.mUnk_00 != data_020b1084.mUnk_04) {
            data_020b1084.mUnk_04 = sMicEdit.mUnk_00;
            data_020b1084.mUnk_08 = sMicEdit.mUnk_04;
            RunSaveScreen(1);
        }
    }
    OpenWaveStream(&gTitleStream, "BAK_strm_Title.raw", 0x5654);
}

void ResetTitleReturnState(void) {
    gTitleScreen.mReturnState = -1;
}

void SetupTitleImageAndModel(void) {
    func_0200c5d8(0, 0, 0x25e, 0x100, 0x10, 1, 2);
    func_02024208();
    StartModel(&gTitleScreen.mModel, 0x25d, 0x25c, 2, 0);
}

void SetupTitleScreen(void) {
    TitleScreen *s = &gTitleScreen;
    int i;
    SetupTitleImageAndModel();
    s->mState = gTitleScreen.mReturnState < 0 ? 1 : s->mReturnState;
    s->mNextState = -1;
    s->mReadSelected = 0;
    s->mAnySaveWritten = 0;
    s->mIntroTimer = (data_020b6ee0.mUnk_a91a != 0 && s->mReturnState < 0) ? 1 : 0x4b0;
    s->mSlideDirection = -1;
    func_0200dd90();
    if (s->mReturnState < 0) {
        s->mUnk_1e4 = 0;
        s->mUnk_1e0 = 0x10000;
    } else {
        s->mUnk_1e4 = 1;
        s->mUnk_1e0 = 0;
    }
    s->mReturnState = -1;
    data_020b6ee0.mUnk_a91a = 0;
    data_020b6ee0.mUnk_a91b = 0;
    RequestTrack(0, 0x80);
    OpenWaveStream(&gTitleStream, "BAK_strm_Title.raw", 0x5654);
    for (i = 0; i < 3; i++) {
        SaveRecordHead *buf = AllocSaveRecord();
        ReadSaveBlock(i, buf, SAVE_BLOCK_ID);
        s->mSaveSlots[i].mWritten = buf->mWritten;
        if (s->mSaveSlots[i].mWritten == 1) {
            s->mAnySaveWritten = 1;
            s->mSaveSlots[i].mReachedNodeIndex = buf->mReachedNodeIndex;
            s->mSaveSlots[i].mLastNodeDone = buf->mLastNodeDone;
        }
        FreeSaveRecord(&buf);
    }
}

static const s16 data_0204e1e4[1] = {0};
static const u32 data_0204e1ec[1] = {0xdd1};
static const u32 data_0204e1e8[1] = {0xdce};
static const s16 data_0204e1f0[4] = {140, 70, -22, 0};

typedef struct MicRange {
    u32 mMin;
    u32 mMax;
} MicRange;

// Only [1] is read (the range of sMicEdit.mUnk_04).
static const MicRange sMicRanges[2] = {{0, 3}, {0, 7}};

void UpdateTitleScreen(void) {
    static s32 sShakeTimer;
    static s32 sShakeX;
    static s32 sShakeY;
    TitleScreen *s = &gTitleScreen;
    DrawParams params;
    char text[0x40];
    char num[8];
    int flag;
    int i;
    u8 pct;
    s16 idx;
    const s16 *pos;
    const u32 *id;
    u32 v;
    u32 *level;
    const MicRange *range;

    if (s->mIntroTimer == 0) {
        SetFaded(3);
        ReleaseGxPool();
        ReleaseTextureLayers();
        LoadImage(0xdaa, 1);
        PauseWaveStream(&gTitleStream);
        RunVideo("BAK_Intro.mods", 2);
        s->mIntroTimer = 0x4b0;
        SetupImageLayer(1, 0, 0x100, 0x100);
        SetupTitleImageAndModel();
        ResumeWaveStream(&gTitleStream);
        return;
    }
    if (s->mState != 6 && s->mNextState != 6) {
        UpdateWaveStream(&gTitleStream);
    }
    if (s->mState == 1 && s->mNextState < 0) {
        if (gSystemState.mKeysDown != 0 || gSystemState.mTouchDown) {
            s->mIntroTimer = 0x4b0;
        } else {
            s->mIntroTimer--;
        }
    }
    func_020242a8(&params, 0xff, 0, 0, 0x1000, 0x8000, -1, 0x100);
    params.mUnk_43_3 = 1;
    UpdateModelScript(&s->mModel);
    DrawModelFileEx(s->mModel.mUnk_170, &params, &s->mModel, 0);
    if (s->mUnk_1e4 == 0) {
        if (s->mUnk_1e0 >= 0) {
            s32 x = s->mUnk_1e0 - 0x1900;
            if (x < 0) {
                x = 0;
            }
            s->mUnk_1e0 = x;
            DrawSprite(0xdfa, 0x59 - (s->mUnk_1e0 >> 8), 0x35, 0x96, 2);
            DrawSprite(0xdfb, (s->mUnk_1e0 >> 8) + 0x59, 0x35, 0x8c, 2);
            if (s->mUnk_1e0 == 0) {
                PlayWaveKeepAlive(0xe7, 100, 0, 0x40, 0xffff, 0);
                s->mUnk_1e0 = -1;
                sShakeTimer = 12;
                return;
            }
        } else {
            int sign;
            s32 d;
            sign = (NextRandom() & 1) ? -1 : 1;
            d = RandomRange(0, 2);
            d *= sign;
            sShakeX = d;
            sign = (NextRandom() & 1) ? -1 : 1;
            d = RandomRange(0, 2);
            d *= sign;
            sShakeY = d;
            DrawSprite(0xdfa, sShakeX + 0x59, d + 0x35, 0x96, 2);
            DrawSprite(0xdfb, sShakeX + 0x59, sShakeY + 0x35, 0x8c, 2);
            if (--sShakeTimer == 0) {
                s->mUnk_1e4 = 1;
                return;
            }
        }
        return;
    }
    DrawSprite(0xdfa, 0x59, 0x35, 0x96, 2);
    DrawSprite(0xdfb, 0x59, 0x35, 0x8c, 2);
    if (UpdateTitleSlide() != 0 && s->mNextState >= 0) {
        s->mState = s->mNextState;
        s->mNextState = -1;
        func_0200dd90();
    }
    flag = gTitleScreen.mSlideState == TITLE_SLIDE_STATE_IN_PLACE ? 1 : 0;
    switch (s->mState) {
        case 1:
            func_0200bc50((gTitleScreen.mSlideOffsets[0] >> 8) + 0x80, 0x3e, 0, 0, 0x8c, 0x701, 0xdda, GetText(0xc4),
                          data_0201a61c, 2, 0, 0x167, flag, 0, 0, 0);
            if (s->mAnySaveWritten != 0) {
                func_0200bc50((gTitleScreen.mSlideOffsets[1] >> 8) + 0x80, 0x66, 0, 0, 0x8c, 0x700, 0xdda,
                              GetText(0xc5), data_0201a61c, 2, 0, 0x167, flag, 0, 0, 0);
            } else {
                func_0200bc50((gTitleScreen.mSlideOffsets[1] >> 8) + 0x80, 0x66, 0, 0, 0x8c, 0x700, 0xddc,
                              GetText(0xc5), data_0201a626, 2, 0, 0x167, 0, 0, 0, 0);
            }
            func_0200bc50((gTitleScreen.mSlideOffsets[2] >> 8) + 0x80, 0x8e, 0, 0, 0x8c, 0x706, 0xdda, GetText(0xc6),
                          data_0201a61c, 2, 0, 0x167, flag, 0, 0, 0);
            if (TestButton(0x20000000, 0x700)) {
                RequestTitleState(2, -1);
                s->mReadSelected = 1;
            }
            if (TestButton(0x20000000, 0x701)) {
                RequestTitleState(2, -1);
                s->mReadSelected = 0;
            }
            if (TestButton(0x20000000, 0x706)) {
                RequestTitleState(5, -1);
                return;
            }
            break;
        case 2:
            DrawSprite(0xe22, 0x2f, (gTitleScreen.mSlideOffsets[3] >> 8) + 0x11, 0x8c, 1);
            if (s->mReadSelected == 0) {
                DrawText(0x2f, (gTitleScreen.mSlideOffsets[3] >> 8) + 0x11, data_0201a61c, GetText(0xc4), 0, 200, 16,
                         1);
            } else {
                DrawText(0x2f, (gTitleScreen.mSlideOffsets[3] >> 8) + 0x11, data_0201a61c, GetText(0xc5), 0, 200, 16,
                         1);
            }
            for (i = 0; i < 3; i++) {
                if (s->mSaveSlots[i].mWritten == 1) {
                    pct = s->mSaveSlots[i].mLastNodeDone != 0
                              ? 100
                              : s->mSaveSlots[i].mReachedNodeIndex * 100 / MAP_NODE_COUNT;
                    func_0200bc50((gTitleScreen.mSlideOffsets[i] >> 8) + 0x39, i * 0x2c + 0x36, 0, 0, 0x8c, 0x704 + i,
                                  0xdf6, 0, 0, 2, 0, 0x167, flag, 0, 0, 0);
                    sprintf(text, "%s %d", GetText(0xcd), i + 1);
                    DrawText((gTitleScreen.mSlideOffsets[i] >> 8) + 0x39, i * 0x2c + 0x36, data_0201a61c, text, 0, 0xf0,
                             0x10, 1);
                    if (pct != 0) {
                        u8 n = MapRange(pct, 0, 100, 0, 0x18);
                        DrawSprite(0xddd + n, (gTitleScreen.mSlideOffsets[i] >> 8) + 0xa0, i * 0x2c + 0x36, 0x82, 1);
                    }
                    sprintf(text, "%d%", pct);
                    DrawText((gTitleScreen.mSlideOffsets[i] >> 8) + 0xa0, i * 0x2c + 0x36, data_0201a662, text, 2, 0xf0,
                             0x10, 1);
                } else if (s->mReadSelected == 0) {
                    func_0200bc50((gTitleScreen.mSlideOffsets[i] >> 8) + 0x39, i * 0x2c + 0x36, 0, 0, 0x8c, 0x704 + i,
                                  0xdf8, 0, 0, 2, 0, 0, flag, 0, 0, 0);
                    DrawText((gTitleScreen.mSlideOffsets[i] >> 8) + 0x39, i * 0x2c + 0x36, data_0201a61c, GetText(0xcc),
                             0, 0xf0, 0x10, 1);
                }
            }
            func_0200bb14(0, (gTitleScreen.mSlideOffsets[4] >> 8) + 0xa1, 0x102, 0xe, 0, 0, 0x5a, flag);
            if (TestButtonRange(0x20000000, 0x704, 0x706)) {
                data_020b1084.mSaveSlot = (u16) gTouchInput.mTrail.mTouchState.mButtonState - 0x704;
                if (s->mReadSelected == 0) {
                    if (s->mSaveSlots[data_020b1084.mSaveSlot].mWritten == 1) {
                        RequestTitleState(3, -1);
                    } else {
                        ResetAllProgress();
                        PushState(10);
                        PopState();
                    }
                } else {
                    ResetAndReadSaveSlot();
                    PushState(10);
                    PopState();
                }
            }
            if (TestButton(0x20000000, 0x102)) {
                RequestTitleState(1, 1);
                return;
            }
            break;
        case 3:
            DrawSprite(0xe21, (gTitleScreen.mSlideOffsets[0] >> 8) + 0x7f, 0x50, 0x8c, 1);
            DrawText((gTitleScreen.mSlideOffsets[0] >> 8) + 0x7f, 0x50, data_0201a61c, GetText(0xcf), 2, 0xc0, 0xc, 1);
            func_0200bb14(0xff, (gTitleScreen.mSlideOffsets[4] >> 8) + 0xa1, 0x101, 0x15, 0, 0, 0x17d, flag);
            func_0200bb14(0, (gTitleScreen.mSlideOffsets[4] >> 8) + 0xa1, 0x102, 0xe, 0, 0, 0x5a, flag);
            if (TestButton(0x20000000, 0x101)) {
                ResetAllProgress();
                RunSaveScreen(0);
                PushState(10);
                PopState();
            }
            if (TestButton(0x20000000, 0x102)) {
                RequestTitleState(2, 1);
                s->mReadSelected = 0;
                return;
            }
            break;
        case 5:
            DrawSprite(0xe22, 0x2f, (gTitleScreen.mSlideOffsets[3] >> 8) + 0x11, 0x8c, 1);
            DrawText(0x2f, (gTitleScreen.mSlideOffsets[3] >> 8) + 0x11, data_0201a61c, GetText(0xc6), 0, 200, 16, 1);
            func_0200bc50((gTitleScreen.mSlideOffsets[1] >> 8) + 0x80, 0x48, 0, 0, 0x8c, 0x703, 0xdda, GetText(0xc8),
                          data_0201a61c, 2, 0, 0x167, flag, 0, 0, 0);
            func_0200bc50((gTitleScreen.mSlideOffsets[2] >> 8) + 0x80, 0x70, 0, 0, 0x8c, 0x708, 0xdda, GetText(0xc9),
                          data_0201a61c, 2, 0, 0x167, flag, 0, 0, 0);
            func_0200bb14(0, (gTitleScreen.mSlideOffsets[4] >> 8) + 0xa1, 0x102, 0xe, 0, 0, 0x5a, flag);
            if (TestButton(0x20000000, 0x702)) {
                gTitleScreen.mSelectedLanguage = data_020b1084.mLanguage;
                RequestTitleState(4, -1);
            }
            if (TestButton(0x20000000, 0x703)) {
                s->mReturnState = s->mState;
                PushState(0x18);
                PopState();
            }
            if (TestButton(0x20000000, 0x708)) {
                RequestTitleState(6, -1);
                OpenMicEdit();
            }
            if (TestButton(0x20000000, 0x102)) {
                RequestTitleState(1, 1);
                return;
            }
            break;
        case 4:
            DrawSprite(0xe22, 0x2f, (gTitleScreen.mSlideOffsets[0] >> 8) + 0x11, 0x8c, 1);
            DrawText(0x2f, (gTitleScreen.mSlideOffsets[0] >> 8) + 0x11, data_0201a61c, GetText(0xc7), 0, 200, 16, 1);
            pos = data_0204e1f0;
            id = data_0204e1ec;
            for (i = 0; i < 1; i++) {
                idx = data_0204e1e4[i] + 2;
                func_0200bb14((gTitleScreen.mSlideOffsets[idx] >> 8) + data_0204e1f0[0], data_0204e1f0[1], 0x705 + i,
                              data_0204e1e8[i], 0, 0, 0x167, 1);
                if (gTitleScreen.mSelectedLanguage == i) {
                    DrawSprite(0xdd0, pos[0] + (gTitleScreen.mSlideOffsets[idx] >> 8), pos[1], 0xfa, 1);
                }
                DrawSprite(*id, (gTitleScreen.mSlideOffsets[idx] >> 8) + (pos[0] + pos[2]), pos[1], 0xff, 1);
            }
            func_0200bb14(0xff, (gTitleScreen.mSlideOffsets[1] >> 8) + 0xa1, 0x101, 0x15, 0, 0, 0x17d, flag);
            func_0200bb14(0, (gTitleScreen.mSlideOffsets[1] >> 8) + 0xa1, 0x102, 0xe, 0, 0, 0x5a, flag);
            for (i = 0; i < 1; i++) {
                if (TestButton(0x80000000, 0x705 + i)) {
                    gTitleScreen.mSelectedLanguage = i;
                    break;
                }
            }
            if (TestButton(0x20000000, 0x102)) {
                RequestTitleState(5, 1);
            }
            if (TestButton(0x20000000, 0x101)) {
                if (gTitleScreen.mSelectedLanguage != data_020b1084.mLanguage) {
                    data_020b1084.mLanguage = gTitleScreen.mSelectedLanguage;
                    CloseWaveStream(&gTitleStream);
                    RunSaveScreen(1);
                    OpenWaveStream(&gTitleStream, "BAK_strm_Title.raw", 0x5654);
                }
                RequestTitleState(5, 1);
                return;
            }
            break;
        case 6:
            DrawSprite(0xe22, 0x2f, (gTitleScreen.mSlideOffsets[3] >> 8) + 0x11, 0x8c, 1);
            DrawText(0x2f, (gTitleScreen.mSlideOffsets[3] >> 8) + 0x11, data_0201a61c, GetText(0xc9), 0, 200, 16, 1);
            DrawSprite(0xe21, (gTitleScreen.mSlideOffsets[0] >> 8) + 0x7f, 0x28, 0x8c, 1);
            DrawSprite(0xe00, (gTitleScreen.mSlideOffsets[1] >> 8) + 0x7f, 0x8f, 0x8c, 1);
            DrawText((gTitleScreen.mSlideOffsets[1] >> 8) + 0x7f, 0x9e, data_0201a61c, GetText(0xd2), 2, 0xc0, 0xc, 1);
            level = &sMicEdit.mUnk_04;
            range = &sMicRanges[1];
            sprintf(num, "%d", *level + 1);
            DrawText((gTitleScreen.mSlideOffsets[1] >> 8) + 0x82, 0xb4, data_0201a676, num, 0, 0xf0, 1, 1);
            if (*level > range->mMin) {
                func_0200bb14((gTitleScreen.mSlideOffsets[1] >> 8) + 0x50, 0x85, 0x70b, 0xdfc, 0, 0, 0x17d,
                              gSystemState.mUnk_492 == 0 ? 1 : 0);
            }
            if (*level < range->mMax) {
                func_0200bb14((gTitleScreen.mSlideOffsets[1] >> 8) + 0xaf, 0x85, 0x70c, 0xdfe, 0, 0, 0x17d,
                              gSystemState.mUnk_492 == 0 ? 1 : 0);
            }
            if (gSystemState.mUnk_492 == 0) {
                DrawText((gTitleScreen.mSlideOffsets[0] >> 8) + 0x7f, 0x28, data_0201a61c, GetText(0xd1), 2, 0xc0, 0xc,
                         1);
                if (s->mNextState < 0) {
                    if (!(gSystemState.mFlags & 0x800)) {
                        OS_Terminate();
                    }
                    v = MapRangeFromZero(func_020213e8((sMicRanges[1].mMax - sMicEdit.mUnk_04 + 1) * 25), 7, 0, 0x1f);
                } else {
                    v = 0;
                }
            } else {
                DrawText((gTitleScreen.mSlideOffsets[0] >> 8) + 0x7f, 0x28, data_0201a61c, GetText(0xd3), 2, 0xc0, 0xc,
                         1);
                sMicEdit.mPeak = 0;
                v = 0;
            }
            if (v > sMicEdit.mPeak) {
                sMicEdit.mPeak = v;
            }
            if (sMicEdit.mPeak != 0) {
                DrawSprite(sMicEdit.mPeak + 0xe01, (gTitleScreen.mSlideOffsets[1] >> 8) + 0x7f, 0x8f, 0x8c, 1);
                sMicEdit.mPeak--;
            }
            func_0200bb14(0xff, (gTitleScreen.mSlideOffsets[4] >> 8) + 0xa1, 0x101, 0x15, 0, 0, 0x17d, flag);
            func_0200bb14(0, (gTitleScreen.mSlideOffsets[4] >> 8) + 0xa1, 0x102, 0xe, 0, 0, 0x5a, flag);
            if (TestButton(0x20000000, 0x709)) {
                func_02021054(--sMicEdit.mUnk_00);
            } else if (TestButton(0x20000000, 0x70a)) {
                func_02021054(++sMicEdit.mUnk_00);
            } else if (TestButton(0x20000000, 0x70b)) {
                sMicEdit.mUnk_04--;
            } else if (TestButton(0x20000000, 0x70c)) {
                sMicEdit.mUnk_04++;
            }
            if (TestButton(0x20000000, 0x102)) {
                RequestTitleState(5, 1);
                CloseMicEdit(0);
            }
            if (TestButton(0x20000000, 0x101)) {
                RequestTitleState(5, 1);
                CloseMicEdit(1);
            }
            break;
    }
}

void LeaveTitleScreen(void) {
    CloseWaveStream(&gTitleStream);
    func_020102f8();
}
