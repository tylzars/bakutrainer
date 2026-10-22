#include <Main/Input/touch_buttons.h>
#include <Main/System/random_angle.h>
#include <Main/System/screen_fade.h>
#include <Main/System/game_math.h>
#include <Main/System/system_init.h>
#include <Main/Scene/stage_info_panel.h>
#include <Main/System/sound_control.h>
#include <Main/Unknown/game_util.h>
#include <Main/Scene/mode_select.h>
#include <Main/Unknown/move_object.h>
#include <Main/Scene/stage_scene.h>
#include <Main/Graphics/sprite_slots.h>
#include <Main/Graphics/text_draw.h>
#include <Main/Graphics/model.h>
#include <Main/Sound/wave_player.h>
#include <Main/Graphics/sprite_script.h>
#include <Main/Map/map_entries.h>
#include <Main/Graphics/script_data.h>
#include <Main/System/state_flow.h>
#include <Main/Graphics/gx_pool.h>
#include <Main/clib.h>
#include <nitro/fx.h>
#include <nitro/gx.h>
#include <nitro/os.h>

// Values of data_020b10fc.mSceneState, the state UpdateScene switches on: start (adds the first popups), run (the
// timers count), popups only (set when a touched kind-3 popup brings mUnk_624 to 0: the timers stop, touches are
// ignored and the popups still update; that popup's slot, in POPUP_STATE_END_SCENE, moves the state on to end at the
// next update), end (waits for the model, then ends the mode).
#define SCENE_STATE_START 0
#define SCENE_STATE_RUN 1
#define SCENE_STATE_POPUPS_ONLY 2
#define SCENE_STATE_END 3

// Values of PopupSlot.mState, the state UpdatePopups switches on. 6 is a case of that switch but is never stored.
#define POPUP_STATE_WAIT 0
#define POPUP_STATE_APPEAR 1
#define POPUP_STATE_SHOWN 2
#define POPUP_STATE_DISAPPEAR 3
#define POPUP_STATE_TOUCHED 4
#define POPUP_STATE_END_SCENE 5
#define POPUP_STATE_FREE 7

const u16 data_0204e1c0[6] = {0xd10, 0xd14, 0xd18, 0xd1c, 0xd20, 0xd24};
const s32 data_0204e1cc[3][2] = {{28, 90}, {48, 90}, {68, 90}};

// The stage scene object; the code reaches it through gScene.
Scene data_020ab7e0;
Scene *gScene = &data_020ab7e0;

void GetMapEntryPos(int index, VecFx32 *out) {
    void *entry = GetMapEntry(index, 0);
    if (entry == 0) {
        OS_Terminate();
    }
    out->x = GetMapEntryX(entry);
    out->y = GetMapEntryY(entry);
    out->z = GetMapEntryZ(entry);
}

void StartShake(int ms) {
    gScene->mShakeTimer = (60 * (u32) ms) / 1000;
}

void SetupSceneCamera(void) {
    gMapState.mCamera.mUnk_2dc.mUnk_44 = 0x030d4000;
    gMapState.mCamera.mUnk_2dc.mUnk_54 = 0x640;
    func_02033e38(1, 1, 1);
    func_02033e38(8, 0, 1);
    gMapState.mCamera.mUnk_2dc.mUnk_30 = 1;
    GetMapEntryPos(1, &gScene->mUnk_5b4);
    GetMapEntryPos(0, &gScene->mUnk_5c0);
    GetMapEntryPos(2, &gScene->mUnk_5cc);
    GetMapEntryPos(4, &gScene->mUnk_5d8);
    memset(&gScene->mShakeTarget, 0, 12);
    memset(&gScene->mShakeOffset, 0, 12);
    memset(&gScene->mShakePerFrame, 0, 12);
    gScene->mShakeFrameIndex = 0;
    gScene->mShakeTimer = 0;
    gMapState.mCamera.mUnk_2dc.mUnk_38 = 0;
    gMapState.mCamera.mUnk_2dc.mUnk_40 = 0;
    gMapState.mCamera.mUnk_2dc.mUnk_3c = 0;
    gMapState.mCamera.mUnk_5b_1 = 0;
    func_02033df4(25);
    gMapState.mCamera.mUnk_2dc.mUnk_14 = gMapState.mCamera.mUnk_2dc.mUnk_18;
    gMapState.mCamera.mUnk_2dc.mUnk_10 = 0;
}

void UpdateSceneCamera(void) {
    Unk_020c5f04 *cam = &gMapState.mCamera.mUnk_9c;
    VecFx32 a;
    VecFx32 b;
    a.x = MapRange(gScene->mEndTimer, 0, gScene->mEndTimerMax, gScene->mUnk_5cc.x >> 8, gScene->mUnk_5b4.x >> 8) << 8;
    a.y = MapRange(gScene->mEndTimer, 0, gScene->mEndTimerMax, gScene->mUnk_5cc.y >> 8, gScene->mUnk_5b4.y >> 8) << 8;
    a.z = MapRange(gScene->mEndTimer, 0, gScene->mEndTimerMax, gScene->mUnk_5cc.z >> 8, gScene->mUnk_5b4.z >> 8) << 8;
    b.x = MapRange(gScene->mEndTimer, 0, gScene->mEndTimerMax, gScene->mUnk_5d8.x >> 8, gScene->mUnk_5c0.x >> 8) << 8;
    b.y = MapRange(gScene->mEndTimer, 0, gScene->mEndTimerMax, gScene->mUnk_5d8.y >> 8, gScene->mUnk_5c0.y >> 8) << 8;
    b.z = MapRange(gScene->mEndTimer, 0, gScene->mEndTimerMax, gScene->mUnk_5d8.z >> 8, gScene->mUnk_5c0.z >> 8) << 8;
    EaseVecToward(&cam->mUnk_204, a, 4);
    EaseVecToward(&cam->mUnk_228, b, 4);
    if (gScene->mShakeTimer != 0) {
        gScene->mShakeTimer--;
        gScene->mShakeFrameIndex = (gScene->mShakeFrameIndex + 1) % 3;
        if (gScene->mShakeFrameIndex == 0) {
            gScene->mShakeTarget.x = ((NextRandom() & 7) - 5) << 12;
            gScene->mShakeTarget.y = ((NextRandom() & 7) - 5) << 12;
            gScene->mShakeTarget.z = ((NextRandom() & 7) - 5) << 12;
            gScene->mShakePerFrame.x = (gScene->mShakeTarget.x - gScene->mShakeOffset.x) / 2;
            gScene->mShakePerFrame.y = (gScene->mShakeTarget.y - gScene->mShakeOffset.y) / 2;
            gScene->mShakePerFrame.z = (gScene->mShakeTarget.z - gScene->mShakeOffset.z) / 2;
        }
        gScene->mShakeOffset.x += gScene->mShakePerFrame.x;
        gScene->mShakeOffset.y += gScene->mShakePerFrame.y;
        gScene->mShakeOffset.z += gScene->mShakePerFrame.z;
        cam->mUnk_228.x += gScene->mShakeOffset.x;
        cam->mUnk_228.y += gScene->mShakeOffset.y;
        cam->mUnk_228.z += gScene->mShakeOffset.z;
    }
    UpdateCamera();
}

void SetupScene(void) {
    u16 area[5];
    int i;
    int stage = data_020b6ee0.mSelectedStageIndex;
    int ok;
    if (stage < 0) {
        stage = 0;
    }
    gScene->mStage = gStages[stage].mInfo;
    data_020b10fc.mTableRow = gScene->mStage.mGrade - 1;
    ok = 0;
    if (data_020b10fc.mTableRow >= 0 && data_020b10fc.mTableRow < 12) {
        ok = 1;
    }
    if (!ok) {
        OS_Terminate();
    }
    func_0200c5d8(0, 0, gScene->mStage.mStageId + 0xdb3, 0x180, 16, 0, 2);
    func_02024208();
    RequestTrack(4, 0x4c);
    func_0200dc08(data_0204e1c0[gScene->mStage.mKind], &area[4], &area[3], &area[2], &area[1], &area[0], 0);
    SetupMap(area[4], area[2], area[3], 0, area[1], area[0]);
    UpdateSystemAndWait();
    SetupSceneCamera();
    G3X_SetClearColor(0, 8, 0x7fff, 0, 0);
    memset(&gScene->mUnk_3a8, 0, sizeof(DrawParams));
    func_02031fc4(3, &gScene->mUnk_3a8.mPos);
    gScene->mUnk_3a8.mUnk_43 = 0;
    InitModel(&gScene->mUnk_230, 0, 0, 0, data_0201abbc[gScene->mStage.mKind + 1]);
    gScene->mBasePos = gScene->mUnk_3a8.mPos;
    InitModel(&gScene->mUnk_3ec, 0, 0, 0, data_0201abbc[0]);
    data_020b10fc.mSceneState = SCENE_STATE_START;
    gScene->mPopupRow = data_020b10fc.mTableRow - 1;
    gScene->mScore = 0;
    gScene->mRowTimer = 0;
    LoadSceneValues();
    func_0200ce24();
    ClearNumber();
    for (i = 0; i < POPUP_SLOT_COUNT; i++) {
        gScene->mSlots[i].mState = POPUP_STATE_FREE;
    }
    StartModeSlide(1, 0);
}

void UpdateScene(void) {
    u8 slideState;
    int busy;
    func_020059cc(gScene->mScore);
    slideState = GetModeSlideState();
    if (slideState != MODE_SLIDE_STATE_NONE) {
        UpdateSceneCamera();
        DrawScene();
        busy = UpdateModeSlide(1);
        if (slideState == MODE_SLIDE_STATE_END && busy == 0) {
            StartResultScreen(GetScoreThresholdMet(), gScene->mScore);
        }
        return;
    }
    UpdateSceneCamera();
    DrawScene();
    EaseVecToward(&gScene->mUnk_3a8.mPos, gScene->mBasePos, 12);
    switch (data_020b10fc.mSceneState) {
        case SCENE_STATE_START:
            AddPopups();
            data_020b10fc.mSceneState = SCENE_STATE_RUN;
        case SCENE_STATE_RUN:
            gScene->mRowTimer--;
            if (gScene->mRowTimer <= 0) {
                func_0200ce24();
            }
            gScene->mPopupTimer--;
            if (gScene->mPopupTimer <= 0) {
                AddPopups();
            }
            gScene->mEndTimer--;
            if (gScene->mEndTimer <= 0) {
                data_020b10fc.mSceneState = SCENE_STATE_END;
                gScene->mUnk_230.mUnk_134 |= 2;
            }
        case SCENE_STATE_POPUPS_ONLY:
            UpdatePopups();
            break;
        case SCENE_STATE_END:
            if (gScene->mUnk_230.mUnk_172_9) {
                StartModeSlide(0, gScene->mScore >= gScene->mScoreThreshold);
            }
            break;
    }
}

void LeaveScene(void) {
    SetFaded(3);
    ReleaseGxPool();
}

void DrawScene(void) {
    func_0201168c(data_020b6ee0.mSelectedStageIndex, gScene->mScore, gModeSelectState.mScoreThreshold,
                  gScene->mEndTimer, 2);
    func_0200cdc4();
    func_02033614(gMapState.mCamera.mUnk_9c.mUnk_228.x, gMapState.mCamera.mUnk_9c.mUnk_228.z);
    UpdateAndDrawModel(&gScene->mUnk_230, &gScene->mUnk_3a8, 0xb0000002);
    PlayObjectSe(&gScene->mUnk_230);
    func_02024368(&gScene->mUnk_3ec, 255, 0, 0x1000, 0, 0);
    if (data_020b10fc.mSceneState < SCENE_STATE_END) {
        DrawNumber();
    }
}

void func_0200cdc4(void) {
    int n = 3 - gScene->mUnk_624;
    int i;
    func_0202de80(0xe3a, 8, 84, 100, -1, 2);
    for (i = 0; i < n; i++) {
        func_0202de80(0xe39, data_0204e1cc[i][0], data_0204e1cc[i][1], 100, -1, 2);
    }
}

void func_0200ce24(void) {
    gScene->mPopupRow++;
    gScene->mRowTimer = gScene->mRowSeconds * 60;
    if (gScene->mPopupIntervalMs >= gScene->mPopupIntervalMinMs + gScene->mUnk_63c) {
        gScene->mPopupIntervalMs -= gScene->mUnk_63c;
    } else {
        gScene->mPopupIntervalMs = gScene->mPopupIntervalMinMs;
    }
    if (gScene->mPopupShowMs >= gScene->mPopupShowMinMs + gScene->mUnk_648) {
        gScene->mPopupShowMs -= gScene->mUnk_648;
    } else {
        gScene->mPopupShowMs = gScene->mPopupShowMinMs;
    }
}

// Restarts mPopupTimer. If a cell is free, steps mPopupRow back while that row of table 0xd64 has a popup count of 0,
// then starts up to that many popups in free cells: the first of kind 1 when mUnk_62c > mUnk_628 (which resets
// mUnk_62c), the rest of a kind picked by the row's four weights.
void AddPopups(void) {
    u32 mask;
    s32 cum[4];
    s32 total;
    int i;
    int n;
    int k;
    int slot;
    int roll;
    Scene *p;

    total = -1;
    mask = func_0200bf54(POPUP_SLOT_COUNT);
    p = gScene;
    p->mPopupTimer = p->mPopupIntervalMs * 60 / 1000;
    for (i = 0; i < POPUP_SLOT_COUNT; i++) {
        if (gScene->mSlots[i].mState != POPUP_STATE_FREE) {
            mask |= 1 << gScene->mSlots[i].mCellIndex;
        }
    }
    if (mask != -1) {
        do {
            int t = gScene->mPopupRow * 5;
            int cellType = GetFileValueType(0xd64, t + 6);
            if (cellType != 2 && cellType != 3) {
                OS_Terminate();
            }
            gScene->mRowPopupCount = GetFileValue(0xd64, t + 6);
            if (gScene->mRowPopupCount == 0) {
                gScene->mPopupRow--;
            }
        } while (gScene->mRowPopupCount == 0);
        for (i = 0; i < 4; i++) {
            total += GetFileValue(0xd64, gScene->mPopupRow * 5 + 2 + i);
            cum[i] = total;
        }
        n = 0;
        if (gScene->mUnk_62c > gScene->mUnk_628) {
            gScene->mUnk_62c = 0;
            slot = TakeRandomClearBit(&mask);
            StartPopup(slot, 1, slot);
            n++;
        }
        for (; n < gScene->mRowPopupCount; n++) {
            if (mask == -1) {
                break;
            }
            roll = RandomRange(0, total - 1);
            k = 0;
            while (cum[k] < roll) {
                k++;
            }
            slot = TakeRandomClearBit(&mask);
            StartPopup(slot, k, slot);
        }
    }
}

void StartPopup(int slot, int type, int index) {
    PopupSlot *slots = gScene->mSlots;
    PopupSlot *e = &slots[slot];
    e->mState = POPUP_STATE_WAIT;
    e->mCellIndex = index;
    slots[slot].mKind = type;
    e->mAppearFrameCount = 0x10;
    e->mAppearTimer = 0x10;
    e->mDisappearFrameCount = 0x10;
    e->mDisappearTimer = 0x10;
    e->mShowTimer = gScene->mPopupShowMs * 60 / 1000;
    e->mWaitTimer = 0;
    if (slots[slot].mKind == 2) {
        e->mUnk_1c = gScene->mUnk_658;
    } else if (slots[slot].mKind == 1) {
        e->mUnk_1c = gScene->mUnk_65c;
    } else if (slots[slot].mKind != 3) {
        e->mUnk_1c = gScene->mUnk_660;
    }
}

void UpdatePopups(void) {
    int i;
    Scene *p;
    PopupSlot *e;
    int x;
    int y;
    int redraw;

    for (i = 0; i < POPUP_SLOT_COUNT; i++) {
        p = gScene;
        e = &p->mSlots[i];
        redraw = 0;
        x = e->mCellIndex % 3 * 64 + 63;
        y = e->mCellIndex / 3 * 48 + 64;
        switch (e->mState) {
            case POPUP_STATE_WAIT:
                e->mWaitTimer--;
                if (e->mWaitTimer <= 0) {
                    e->mState = POPUP_STATE_APPEAR;
                }
                break;
            case POPUP_STATE_APPEAR:
                e->mAppearTimer--;
                func_0202de80(MapRangeClamped(e->mAppearTimer, e->mAppearFrameCount, 0, 0, 6) + 0xe85, x, y, 0xbe, -1,
                              1);
                if (e->mAppearTimer <= 2) {
                    redraw = 1;
                }
                if (e->mAppearTimer <= 0) {
                    e->mState = POPUP_STATE_SHOWN;
                }
                break;
            case POPUP_STATE_SHOWN:
                e->mShowTimer--;
                redraw = 1;
                if (e->mShowTimer <= 0) {
                    e->mState = POPUP_STATE_DISAPPEAR;
                }
                break;
            case POPUP_STATE_DISAPPEAR:
                e->mDisappearTimer--;
                func_0202de80(MapRangeClamped(e->mDisappearTimer, e->mDisappearFrameCount, 0, 6, 0) + 0xe85, x, y, 0xbe,
                              -1, 1);
                if (e->mDisappearTimer <= 0) {
                    e->mState = POPUP_STATE_FREE;
                }
                break;
            case POPUP_STATE_TOUCHED:
                e->mDisappearTimer--;
                func_020242e8(0xd0f, x, y, 0x1000, 0x1000, 0, 0xff, 0);
                if (e->mDisappearTimer <= 0) {
                    e->mState = POPUP_STATE_FREE;
                }
                break;
            case POPUP_STATE_END_SCENE:
                data_020b10fc.mSceneState = SCENE_STATE_END;
                p->mUnk_230.mUnk_134 |= 2;
                break;
            case 6:
            case POPUP_STATE_FREE:
                break;
        }
        if (redraw == 1) {
            func_020242e8(e->mKind + 0xd0b, x, y, 0x1000, 0x1000, 0, 0xff, 0);
            AddTouchButton(i + 0x100, x, y, 0, 0, 0xe83);
        }
    }
    if (data_020b10fc.mSceneState < SCENE_STATE_POPUPS_ONLY && gSystemState.mTouchPressed) {
        if (TestButtonRange(0x80000000, 0x100, 0x108)) {
            HandlePopupTouch(&gScene->mSlots[(u16) gTouchInput.mTrail.mTouchState.mButtonState - 0x100]);
        }
    }
}

void HandlePopupTouch(PopupSlot *slot) {
    PopupPos pos;

    pos.x = slot->mCellIndex % 3 * 64 + 63;
    pos.y = slot->mCellIndex / 3 * 48 + 64;
    AddAnimObjSlot(5, pos.x, pos.y, 30, 1);
    slot->mState = POPUP_STATE_TOUCHED;
    slot->mDisappearTimer = 30;
    gScene->mUnk_62c++;
    if (slot->mKind == 3) {
        gScene->mUnk_624--;
        gScene->mUnk_62c = 0;
        if (gScene->mUnk_624 <= 0) {
            slot->mShowTimer = 0;
            slot->mState = POPUP_STATE_END_SCENE;
            data_020b10fc.mSceneState = SCENE_STATE_POPUPS_ONLY;
        }
        StartShake(200);
        PlayWaveKeepAlive(0x72, 100, 0, 0x40, 0xffff, 0);
    } else if (slot->mKind == 1) {
        gScene->mEndTimer += gScene->mUnk_65c;
        gScene->mEndTimer = gScene->mEndTimer < gScene->mEndTimerMax ? gScene->mEndTimer : gScene->mEndTimerMax;
        PlayWaveKeepAlive(0x75, 100, 0, 0x40, 0xffff, 0);
    } else if (slot->mKind == 2) {
        AddCappedAt999Shifted(&gScene->mScore, gScene->mUnk_658, 0);
        ShowNumber(gScene->mUnk_658, &pos);
        gScene->mUnk_3a8.mPos.z = gScene->mBasePos.z - 0xc800;
        PlayWaveKeepAlive(0x74, 100, 0, 0x40, 0xffff, 0);
    } else {
        AddCappedAt999Shifted(&gScene->mScore, gScene->mUnk_660, 0);
        ShowNumber(gScene->mUnk_660, &pos);
        gScene->mUnk_3a8.mPos.z = gScene->mBasePos.z - 0xc800;
        PlayWaveKeepAlive(0x73, 100, 0, 0x40, 0xffff, 0);
    }
}

void LoadSceneValues(void) {
    int base;
    base = data_020b10fc.mTableRow * 3;
    gScene->mEndTimer = GetFileValue(0xd63, base + 2) * 60;
    gScene->mEndTimerMax = gScene->mEndTimer;
    base = (data_020b10fc.mTableRow + 1) * 3;
    gScene->mScoreThreshold = GetFileValue(0xd63, base);
    base = data_020b10fc.mTableRow * 3;
    gScene->mUnk_624 = GetFileValue(0xd63, base + 4);
    base = data_020b10fc.mTableRow * 11;
    gScene->mPopupIntervalMs = GetFileValue(0xd62, base + 2);
    base = data_020b10fc.mTableRow * 11;
    gScene->mUnk_63c = GetFileValue(0xd62, base + 3);
    base = data_020b10fc.mTableRow * 11;
    gScene->mPopupIntervalMinMs = GetFileValue(0xd62, base + 4);
    base = data_020b10fc.mTableRow * 11;
    gScene->mRowSeconds = GetFileValue(0xd62, base + 5);
    base = data_020b10fc.mTableRow * 11;
    gScene->mPopupShowMs = GetFileValue(0xd62, base + 6);
    base = data_020b10fc.mTableRow * 11;
    gScene->mUnk_648 = GetFileValue(0xd62, base + 7);
    base = data_020b10fc.mTableRow * 11;
    gScene->mPopupShowMinMs = GetFileValue(0xd62, base + 8);
    base = (data_020b10fc.mTableRow + 1) * 11;
    gScene->mUnk_660 = GetFileValue(0xd62, base);
    base = data_020b10fc.mTableRow * 11;
    gScene->mUnk_65c = GetFileValue(0xd62, base + 10) * 60;
    base = data_020b10fc.mTableRow * 11;
    gScene->mUnk_658 = GetFileValue(0xd62, base + 9);
    base = data_020b10fc.mTableRow * 11;
    gScene->mUnk_628 = GetFileValue(0xd62, base + 12);
}

void ClearNumber(void) {
    gScene->mNumberLevel = 0;
    gScene->mNumber = 0;
}

void ShowNumber(int value, const PopupPos *pos) {
    gScene->mNumberLevel = 0;
    gScene->mPos = *pos;
    gScene->mNumber = value;
}

void DrawNumber(void) {
    Scene *p = gScene;
    int x;
    int y;

    if (p->mNumberLevel < 0x4000 && p->mNumber != 0) {
        x = p->mPos.x;
        EaseLevel(&p->mNumberLevel, 12);
        p = gScene;
        y = p->mPos.y - MapRange(p->mNumberLevel, 0, 0x4000, 0, 30);
        if (y <= 20) {
            y = 20;
        }
        DrawDecimal(x, y, p->mNumber, data_0201a680, 2, 1);
    }
}

int GetSceneScoreThreshold(u8 tableRow) {
    return GetFileValue(0xd63, (tableRow + 1) * 3);
}
