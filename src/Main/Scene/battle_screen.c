#include <Main/Scene/battle_screen.h>
#include <Main/Input/touch_buttons.h>
#include <Main/Save/save_blocks.h>
#include <Main/System/random_angle.h>
#include <Main/System/screen_fade.h>
#include <Main/System/game_math.h>
#include <Main/System/bg_image.h>
#include <Main/Scene/stage_progress.h>
#include <Main/Scene/stage_info_panel.h>
#include <Main/Scene/stage_map_menu.h>
#include <stdio.h>
#include <Main/System/game_util.h>
#include <Main/Scene/language_menu.h>
#include <Main/System/prompt_screen.h>
#include <Main/System/sound_control.h>
#include <Main/System/state_flow.h>
#include <Main/Graphics/text_draw.h>
#include <Main/Graphics/vram_cache.h>
#include <Main/Graphics/model.h>
#include <Main/Graphics/texture_blocks.h>
#include <Main/Sound/wave_player.h>
#include <Main/Graphics/sprite_script.h>
#include <Main/Map/map_entries.h>
#include <Main/Graphics/script_data.h>
#include <Main/System/system_init.h>
#include <Main/Graphics/gx_pool.h>
#include <Main/clib.h>
#include <nitro/fx.h>
#include <nitro/os.h>

// Cursor step that confirms the stage choice in UpdateBattleStagePick (the other steps are -1, 0 and 1).
#define SELECT_CONFIRM 0x55

// Values of data_020b6ee0.mBattleState, the state UpdateBattleScreen switches on. The first state is the table of kind
// values.
#define BATTLE_STATE_STAGE_PICK 0
#define BATTLE_STATE_ENTRY_PICK 1
#define BATTLE_STATE_COMPARE 10
#define BATTLE_STATE_KIND_VALUES 21

BattleView gBattleView;
StageRec gBattleSlotStages[3];

// Layout placeholders: no code reads these two tables and the link drops them.
// Matching: the compiler sorts this file's constant data by size together with the local initialisers in the functions
// below. With these two taking part, the initialisers land at the original's offsets.
const u8 battle_unused0[8] = {0};
const u8 battle_unused1[8] = {0};

void func_020071a8(void) {}

void func_020071ac(void) {
    AddTouchButtonWithSounds(0x190, 0, 0xa2, 0, 0, 0xd5, 0, 0x17d);
    DrawSprite((gTouchInput.mTrail.mTouchState.mButtonState == 0x40000190) + 0xd5, 0, 0xa2, 0x8e, 2);
}

void DrawConfirmButton(void) {
    AddTouchButtonWithSounds(0xc8, 0xff, 0xa2, 0, 0, 0x15, 0, 0x17d);
    DrawSprite((gTouchInput.mTrail.mTouchState.mButtonState == 0x400000c8) + 0x15, 0xff, 0xa2, 0x8e, 2);
}

void SetupStageModels(int a) {
    s32 v;
    StopBattleViewModel();
    StartModel(&data_020b6ee0.mModel[0], 0, 0, 0, data_0201aa60[a]);
    data_020b6ee0.mModel[0].mNodeIndex = 1;
    v = data_020b6ee0.mModel[0].mUnk_010;
    if (v != 0) {
        InitModel(&data_020b6ee0.mModel[1], v, v + 1, 4, 0);
    } else {
        data_020b6ee0.mModel[1].mId = 0;
    }
}

// Draws data_020b6ee0's two models at mModelPos (mModel[1] only while it is set) and plays their sounds. When
// mModel[0]'s mUnk_022 is not negative, starts gBattleView.mModel[1] with that value and clears it. Then runs
// UpdateBattleViewModel, which draws gBattleView.mModel[1] and ends it once its animation has ended.
void DrawBattleModels(void) {
    int flags = 0;
    u16 color = PackColor(0x1f, 0x15, 0xc);
    u16 modelId;
    SetLightFromAngles(flags, 0xf618, 14 << 12, color);
    modelId = data_020b6ee0.mModel[1].mId;
    flags |= 8;
    if (modelId != 0) {
        s32 half = (data_020b6ee0.mModel[0].mUnk_14c.mId - data_020b6ee0.mModel[0].mId) / 2;
        data_020b6ee0.mModel[1].mUnk_14c = data_020b6ee0.mModel[0].mUnk_14c;
        data_020b6ee0.mModel[1].mUnk_14c.mId = modelId + half + 1;
        data_020b6ee0.mModel[1].mUnk_152 = data_020b6ee0.mModel[1].mUnk_14c;
        data_020b6ee0.mModel[1].mUnk_172 |= 0x80;
        UpdateAndDrawModel(&data_020b6ee0.mModel[1], &data_020b6ee0.mModelPos, flags);
        PlayObjectSe(&data_020b6ee0.mModel[1]);
    }
    data_020b6ee0.mModel[0].mUnk_172 |= 0x80;
    UpdateAndDrawModel(&data_020b6ee0.mModel[0], &data_020b6ee0.mModelPos, flags);
    PlayObjectSe(&data_020b6ee0.mModel[0]);
    if (data_020b6ee0.mModel[0].mUnk_022 >= 0) {
        StartBattleViewModel(data_020b6ee0.mModel[0].mUnk_022);
        data_020b6ee0.mModel[0].mUnk_022 = -1;
    }
    UpdateBattleViewModel();
}

// Sets the camera goals to map entry ids 0 (look-at) and 1 (position) and eases the camera one step toward them.
void SetCameraFromMapEntries0And1(void) {
    func_02031fc4(0, &gMapState.mCamera.mLookAtGoal);
    func_02031fc4(1, &gMapState.mCamera.mPosGoal);
    EaseCameraToGoals();
}

// Sets the camera goals to map entry ids 0 (look-at) and 2 (position) and eases the camera one step toward them.
void SetCameraFromMapEntries0And2(void) {
    func_02031fc4(0, &gMapState.mCamera.mLookAtGoal);
    func_02031fc4(2, &gMapState.mCamera.mPosGoal);
    EaseCameraToGoals();
}

void SetCameraFromModel(void) {
    VecFx32 pos;
    s32 a, b, c;
    u16 v[4];
    v[0] = 0;
    {
        u8 table[17] = {0, 0, 1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0};
        SplitAnimFrameAndAdvance(data_020b6ee0.mModel[0].mUnk_004,
                                 table[data_020b6ee0.mModel[0].mEventKey & 0x1f] == 0
                                     ? &data_020b6ee0.mModel[0].mUnk_14c.mPlayFrames
                                     : &v[0],
                                 data_020b6ee0.mModel[0].mUnk_14c.mFramesPerAnimFrame, 0, &a, &b, &c, 1);
    }
    ReadAnimFileNode(data_020b6ee0.mModel[0].mUnk_004, NULL, &pos, 0, a, b, c, 2);
    gMapState.mCamera.mLookAtGoal.x = pos.x + data_020b6ee0.mModelPos.x;
    gMapState.mCamera.mLookAtGoal.z = pos.z + data_020b6ee0.mModelPos.z;
    gMapState.mCamera.mLookAtGoal.y = pos.y + data_020b6ee0.mModelPos.y;
    ReadAnimFileNode(data_020b6ee0.mModel[0].mUnk_004, &v[1], &pos, 0, a, b, c, 1);
    gMapState.mCamera.mPosGoal.x = pos.x + data_020b6ee0.mModelPos.x;
    gMapState.mCamera.mPosGoal.z = pos.z + data_020b6ee0.mModelPos.z;
    gMapState.mCamera.mPosGoal.y = pos.y + data_020b6ee0.mModelPos.y;
    EaseCameraToGoals();
    if ((v[1] >> 9) > 5 && (v[1] >> 9) < 0x78) {
        func_02033df4(v[1] >> 9);
    }
}

void SetupBattleScreen(void) {
    u16 a[6] = {0xf87, 0xf49, 0xf98, 0xf76, 0xf65, 0xfa8};
    u16 b[6] = {0xf88, 0xf4a, 0xf99, 0xf77, 0xf66, 0xfa9};
    int sel;
    u32 i;
    ResetVramBanks();
    ReleaseTextureLayers();
    ReleaseGxPool();
    SetupTextureLayer(0x10cb, 0x10ca, 0x10cc, 0x80, 1);
    SetupTextureLayer(0x10cb, 0x10ca, 0x10cc, 0x82, 2);
    InitGxPool(0x1c7, 0x1c6, 0x100, 16);
    SetLcdSwap(1);
    {
        BOOL ok = 0;
        if (data_020b6ee0.mSelectedNodeIndex >= 0 && data_020b6ee0.mSelectedNodeIndex < MAP_NODE_COUNT) {
            ok = 1;
        }
        if (ok == 0) {
            OS_Terminate();
        }
    }
    sel = GetNodeValue(data_020b6ee0.mSelectedNodeIndex, 14);
    if (!((u32) sel < 6)) {
        OS_Terminate();
    }
    LoadImage(sel + 0xf5f, 2);
    func_02024208();
    RequestTrack(1, 0x4c);
    data_020b6ee0.mBattleState = BATTLE_STATE_KIND_VALUES;
    data_020b6ee0.mUnk_fe2 = 0;
    data_020b6ee0.mUnk_fe4 = 0;
    gBattleView.mState.mFirstSide = 2;
    gBattleView.mState.mUnk_0b = 0x7f;
    gBattleView.mState.mUnk_0c = gBattleView.mState.mUnk_0b;
    gBattleView.mState.mPickSide = 0;
    gBattleView.mState.mKindValueRow = GetNodeValue(data_020b6ee0.mSelectedNodeIndex, 18);
    gBattleView.mState.mPickIndices[1] = 0;
    gBattleView.mState.mPickIndices[0] = gBattleView.mState.mPickIndices[1];
    gBattleView.mState.mPickSide = 0;
    gBattleView.mModel[1].mId = 0;
    memset(&data_020b6ee0.mUnk_fee, 0, 0x12);
    FillEmptySlots();
    for (i = 0; i < 3; i++) {
        StageRec *rec = GetSlotStage(i);
        if (rec != NULL) {
            rec = GetSlotStage(i);
            memcpy(&gBattleSlotStages[i], rec, sizeof(StageRec));
            gBattleSlotStages[i].mInfo.mStageIndex = ((u32) rec - (u32) gStages) / sizeof(StageRec);
        } else {
            gBattleSlotStages[i].mInfo.mValue = -9999;
        }
        gStageInfos[i].mStageIndex = -1;
    }
    for (i = 0; i < 39; i++) {
        data_020b6ee0.mBattleSlotFlags[i] = BATTLE_SLOT_NOT_PICKED;
    }
    func_020220c8(0x10d0);
    SetupMap(a[sel], 0, b[sel], 0, 0, 0);
    gMapState.mCamera.mUnk_04 = func_020071a8;
    gMapState.mCamera.mUnk_2dc.mUnk_54 = 25 << 6;
    gMapState.mCamera.mUnk_2dc.mViewDistance = 0x128e000;
    gMapState.mCamera.mEaseShift = 1;
    memset(&data_020b6ee0.mModelPos, 0, 0x44);
    data_020b6ee0.mUnk_fc8.x = -1;
    data_020b6ee0.mUnk_fc8.y = -1;
    data_020b6ee0.mUnk_fc8.z = -1;
    func_02031fc4(0, &data_020b6ee0.mModelPos);
    data_020b6ee0.mUnk_fdf &= ~7;
    SetupStageModels(gBattleSlotStages[0].mInfo.mStageId);
    data_020b6ee0.mSideSprites[0].mUnk_de = 1;
    data_020b6ee0.mSideSprites[1].mUnk_de = 1;
}

void UpdateBattleScreen(void) {
    switch (data_020b6ee0.mBattleState) {
        case BATTLE_STATE_KIND_VALUES:
            UpdateBattleKindValues();
            break;
        case BATTLE_STATE_STAGE_PICK:
            UpdateBattleStagePick();
            break;
        case BATTLE_STATE_ENTRY_PICK:
            UpdateBattleEntryPick((gBattleView.mState.mEntryPickCount ^ gBattleView.mState.mFirstSide) & 1);
            break;
        case BATTLE_STATE_COMPARE:
            UpdateBattleCompare();
            break;
    }
}

void LeaveBattleScreen(void) {
    func_020102f8();
}

int IsStageInfosOutClear(void) {
    if ((data_020b6ee0.mBattleSlotFlags[0] & BATTLE_STAGE_INFOS_OUT) == 0) {
        return 1;
    }
    return 0;
}

// Stores column kind + 3 of the battle's row of table 0x1e7 as each record's mKindValue. Restarts mKindValueLevel,
// with which the stage pick counts the new values into the shown numbers.
void LoadKindValues(void) {
    int i;
    int base = gBattleView.mState.mKindValueRow * 7;
    for (i = 0; i < 3; i++) {
        gBattleSlotStages[i].mInfo.mKindValue = GetFileValue(0x1e7, base + (gBattleSlotStages[i].mInfo.mKind + 3));
        gStageInfos[i].mKindValue = GetFileValue(0x1e7, base + (gStageInfos[i].mKind + 3));
    }
    gBattleView.mState.mKindValueLevel = 0;
}

int GetPercentOfBattleTotal(StageInfo *info) {
    int percent = GetFileValue(0x1e8, 19);
    return GetBattleTotal(info) * percent / 100;
}

void SetupBattleSides(int slotIndex, int stageInfoIndex) {
    int i;
    memcpy(&gBattleView.mSide[0], &gBattleSlotStages[slotIndex].mInfo, sizeof(StageInfo));
    memcpy(&gBattleView.mSide[1], &gStageInfos[stageInfoIndex], sizeof(StageInfo));
    data_020b6ee0.mBattleSlotFlags[slotIndex] &= ~BATTLE_SLOT_NOT_PICKED;
    for (i = 0; i < 2; i++) {
        gBattleView.mSide[i].mSkipEntry = 0;
        gBattleView.mSide[i].mTriplePending = 0;
        gBattleView.mSide[i].mAdded = 0;
        gBattleView.mSide[i].mOthersHalfSum = 0;
        gBattleView.mSide[i].mOthersHalfSumAdded = 0;
        gBattleView.mSide[i].mPrevBattleTotal = 0;
    }
    for (i = 0; i < 3; i++) {
        if (i != slotIndex && gBattleSlotStages[i].mInfo.mValue > -5000) {
            gBattleView.mSide[0].mOthersHalfSum += GetValuePlusKindValue(&gBattleSlotStages[i].mInfo) / 2;
        }
        {
            StageInfo *p = &gStageInfos[i];
            if (i != stageInfoIndex && p->mValue > -5000) {
                gBattleView.mSide[1].mOthersHalfSum += GetValuePlusKindValue(p) / 2;
            }
        }
    }
    // i is 3 here, one past mSide[1]: the original writes this word too.
    gBattleView.mSide[i].mSubtracted = 0;
}

int func_020078f8(void) {
    int i;
    for (i = 0; i < 2; i++) {
        u8 v = GetFileValue(0x10d5, gBattleView.mSide[i].mKind * 2 + 2);
        if (v == gBattleView.mSide[i ^ 1].mKind) {
            return gBattleView.mSide[i ^ 1].mKind;
        }
    }
    return -1;
}

void SetSidesSubtracted(void) {
    int i;
    for (i = 0; i < 2; i++) {
        u8 v = GetFileValue(0x10d5, gBattleView.mSide[i].mKind * 2 + 2);
        if (v == gBattleView.mSide[i ^ 1].mKind) {
            gBattleView.mSide[i].mSubtracted = GetPercentOfBattleTotal(&gBattleView.mSide[i ^ 1]);
        } else {
            gBattleView.mSide[i].mSubtracted = 0;
        }
    }
}

// mValue + mKindValue - mSubtracted + mAdded, plus mOthersHalfSum while mOthersHalfSumAdded is 1; at least 0.
int GetBattleTotal(const StageInfo *info) {
    int sum = info->mAdded + (info->mValue + info->mKindValue - info->mSubtracted);
    if (info->mOthersHalfSumAdded == 1) {
        sum += info->mOthersHalfSum;
    }
    if (sum < 0) {
        sum = 0;
    }
    return sum;
}

// -9999 for a record that is out.
int GetValuePlusKindValue(const StageInfo *info) {
    if (info->mValue < -5000) {
        return -9999;
    }
    return info->mValue + info->mKindValue;
}

int FindTableEntryIndexUnbounded(int id) {
    int row = 0;
    if (id != GetFileValue(0x1d9, 2)) {
        int index = 2;
        do {
            index += 20;
            row++;
        } while (id != GetFileValue(0x1d9, index));
    }
    return row;
}

void StartBattleViewModel(int a) {
    switch (a) {
        case 0:
            InitModel(&gBattleView.mModel[1], 0x239, 0x238, 2, 0);
            gBattleView.mState.mUnk_0a = 0;
            break;
        case 1:
            InitModel(&gBattleView.mModel[1], 0x23f, 0x23e, 1, 0);
            gBattleView.mState.mUnk_0a = 1;
            break;
        case 2:
            InitModel(&gBattleView.mModel[1], 0x23d, 0x23c, 1, 0);
            gBattleView.mState.mUnk_0a = 0;
            break;
        case 3:
            InitModel(&gBattleView.mModel[1], 0x23b, 0x23a, 1, 0);
            gBattleView.mState.mUnk_0a = 0;
            break;
    }
}

void StopBattleViewModel(void) {
    gBattleView.mModel[1].mId = 0;
}

void UpdateBattleViewModel(void) {
    if (gBattleView.mModel[1].mId != 0) {
        if (gBattleView.mState.mUnk_0a == 0) {
            UpdateAndDrawModelOrtho(&gBattleView.mModel[1], 0x80, 0x60, 0x1000, 0, 0x100);
        } else {
            UpdateAndDrawModel(&gBattleView.mModel[1], &data_020b6ee0.mModelPos, 0);
        }
        if (gBattleView.mModel[1].mAnimEnded == 1) {
            gBattleView.mModel[1].mId = 0;
        }
    }
}

// Estimate, for FindBestSide1Entry, of side's total minus the other side's after the table entry in row `row`. It
// writes nothing. Unlike ApplyTableEntry, kinds 3 and 4 score a bonus while the other side's triple is pending and
// fewer than 2 turns were taken, kind 12 scores no change, and kinds 15 and 16 ignore mTriplePending.
int GetTableEntryTotalDiff(int side, int row) {
    StageInfo *mine = &gBattleView.mSide[side];
    StageInfo *other = &gBattleView.mSide[side ^ 1];
    int base = row * 20;
    int kind = GetFileValue(0x1d9, base + 3);
    int p1 = GetFileValue(0x1d9, base + 4);
    int p2 = GetFileValue(0x1d9, base + 5);
    int myPower = GetBattleTotal(mine);
    int result = myPower;
    int otherPower = GetBattleTotal(other);
    switch (kind) {
        case 0:
        case 1:
            if (mine->mOthersHalfSumAdded == 0 && myPower < otherPower) {
                int boosted = myPower + mine->mOthersHalfSum;
                if (boosted > otherPower) {
                    result = boosted;
                }
            }
            break;
        case 3:
        case 4:
            if (other->mTriplePending == 1 && gBattleView.mState.mEntryPickCount < 2) {
                result = myPower + MapRangeFromZero(data_020b6ee0.mSelectedNodeIndex, MAP_NODE_COUNT, 40, 150) * 3;
            }
            break;
        case 5:
            if (mine->mSubtracted > 0) {
                result = myPower + mine->mSubtracted * 2;
            }
            break;
        case 11:
            otherPower = 0;
            break;
        case 6:
            result = otherPower;
            otherPower = myPower;
            break;
        case 7:
            result = myPower - mine->mKindValue;
            otherPower = otherPower - other->mKindValue;
            break;
        case 8:
            result = myPower + mine->mKindValue;
            otherPower = otherPower + other->mKindValue;
            break;
        case 2:
            if (other->mPrevBattleTotal > 0) {
                otherPower = other->mPrevBattleTotal;
            }
            break;
        case 13:
            if (mine->mTriplePending == 1) {
                p1 *= 3;
            }
            result += p1;
            break;
        case 14:
            if (mine->mTriplePending == 1) {
                p1 *= 3;
            }
            result += p1;
            if (other->mKind == mine->mKind) {
                otherPower += p1;
            }
            break;
        case 15:
            if (other->mKind == p2) {
                result = myPower + p1;
            }
            break;
        case 16:
            result = myPower + p1;
            otherPower -= p1;
            break;
    }
    return result - otherPower;
}

void ApplyTableEntry(void) {
    int side = gBattleView.mApplySide;
    StageInfo *mine = &gBattleView.mSide[side];
    StageInfo *other = &gBattleView.mSide[side ^ 1];
    int base = gBattleView.mState.mTableEntryIndex * 20;
    int kind;
    int p1;
    int p2;
    int done = 0;
    kind = GetFileValue(0x1d9, base + 3);
    p1 = GetFileValue(0x1d9, base + 4);
    p2 = GetFileValue(0x1d9, base + 5);
    switch (kind) {
        case 0:
        case 1:
            mine->mOthersHalfSumAdded = 1;
            done = 1;
            break;
        case 3:
            gBattleView.mState.mEntryPickCount = 4;
            done = 1;
            break;
        case 4:
            other->mSkipEntry = 1;
            done = 1;
            break;
        case 5:
            if (mine->mSubtracted > 0) {
                mine->mSubtracted = -mine->mSubtracted;
                done = 1;
            }
            break;
        case 11:
            other->mAdded = -GetBattleTotal(other);
            done = 1;
            break;
        case 6: {
            int myPower = GetBattleTotal(mine);
            int otherPower = GetBattleTotal(other);
            mine->mAdded += otherPower - myPower;
            other->mAdded += myPower - otherPower;
            done = 1;
            break;
        }
        case 7:
            mine->mKindValue = 0;
            other->mKindValue = 0;
            done = 1;
            break;
        case 8:
            mine->mKindValue <<= 1;
            other->mKindValue <<= 1;
            done = 1;
            break;
        case 2:
            if (other->mPrevBattleTotal > 0) {
                other->mAdded += other->mPrevBattleTotal - GetBattleTotal(other);
            }
            done = 0;
            break;
        case 12:
            mine->mTriplePending = 1;
            done = 1;
            break;
        case 13:
            if (mine->mTriplePending == 1) {
                p1 *= 3;
                mine->mTriplePending = 0;
            }
            mine->mValue += p1;
            PlayWaveKeepAlive(p1 > 0 ? 0xe2 : 0xe1, 100, 0, 64, 0xffff, 0);
            done = 1;
            break;
        case 14:
            if (mine->mTriplePending == 1) {
                p1 *= 3;
                mine->mTriplePending = 0;
            }
            mine->mValue += p1;
            PlayWaveKeepAlive(p1 > 0 ? 0xe2 : 0xe1, 100, 0, 64, 0xffff, 0);
            if (other->mKind == mine->mKind) {
                other->mValue += p1;
            }
            done = 1;
            break;
        case 15:
            if (other->mKind == p2) {
                if (mine->mTriplePending == 1) {
                    p1 *= 3;
                    mine->mTriplePending = 0;
                }
                mine->mValue += p1;
                PlayWaveKeepAlive(p1 > 0 ? 0xe1 : 0xe2, 100, 0, 64, 0xffff, 0);
                done = 1;
            }
            break;
        case 16:
            if (mine->mTriplePending == 1) {
                p1 *= 3;
                mine->mTriplePending = 0;
            }
            if (mine->mStageIndex != -1) {
                PlayWaveKeepAlive(p1 > 0 ? 0xe2 : 0xe1, 100, 0, 64, 0xffff, 0);
            } else if (other->mStageIndex != -1) {
                PlayWaveKeepAlive(p1 > 0 ? 0xe1 : 0xe2, 100, 0, 64, 0xffff, 0);
            }
            other->mValue -= p1;
            mine->mValue += p1;
            done = 1;
            break;
    }
    gBattleView.mState.mUnk_13 = done;
}

int ComparePickIndicesWithBestPair(void) {
    int i;
    int j;
    int bestDiff;
    int bestOther;
    int bestOwn;
    StageInfo *own;
    i = 0;
    bestDiff = -5000;
    own = gStageInfos;
    for (; i < 3; own++, i++) {
        StageRec *rec = gBattleSlotStages;
        for (j = 0; j < 3; j++, rec++) {
            int a = GetValuePlusKindValue(own);
            int b = GetValuePlusKindValue(&rec->mInfo);
            if (a > -5000 && b > -5000 && a - b > bestDiff) {
                bestDiff = a - b;
                bestOther = j;
                bestOwn = i;
            }
        }
    }
    if (gBattleView.mState.mPickIndices[0] != bestOther) {
        return 1;
    }
    if (gBattleView.mState.mPickIndices[1] != bestOwn) {
        return 2;
    }
    return 0;
}

int FindBestSide1Entry(void) {
    StageInfo *side = &gBattleView.mSide[1];
    int best = -5000;
    int bestIndex = -5000 >> 13;
    int i;
    if (side->mSkipEntry != 1) {
        for (i = 0; i < 5; i++) {
            if (side->mTableEntryIds[i] >= 0) {
                int score = GetTableEntryTotalDiff(1, FindTableEntryIndexUnbounded(side->mTableEntryIds[i]));
                if (score > best) {
                    best = score;
                    bestIndex = i;
                }
            }
        }
    }
    return bestIndex;
}

// Only when flag is 1: each record still in of list `which` (0 gBattleSlotStages, 1 gStageInfos) loses half of its
// mValue + mKindValue.
void SubtractHalfFromList(int which, int flag) {
    int i;
    if (flag == 1) {
        for (i = 0; i < 3; i++) {
            StageInfo *p = (which == 0) ? &gBattleSlotStages[i].mInfo : &gStageInfos[i];
            if (p->mValue > -5000) {
                p->mValue -= (p->mValue + p->mKindValue) / 2;
            }
        }
    }
}

void func_02007ffc(u16 *a, u16 *b) {
    int x = MapRangeFromZero(*a, 0x4000, 150, 0);
    int y = MapRangeFromZero(*b, 0x4000, 150, 0);
    int z = GetNodeValue(data_020b6ee0.mSelectedNodeIndex, 7);
    DrawSprite(0x1195, x + 255, 44, 150, 2);
    DrawSprite(0x1196, x + 255, 55, 150, 2);
    y = -y;
    DrawSprite(z + 0x11b6, y, 44, 150, 2);
    DrawSprite(z + 0x11ba, y, 55, 150, 2);
}

void DrawBattleStageInfo(StageInfo *info) {
    char numBuf[8];
    char textBuf[8];
    GetValuePlusKindValue(info);
    DrawSprite(0x801082 + info->mKind, 0, 30, 150, 1);
    DrawText(5, 19, data_0201a61c, GetText(info->mStageId + 98), 0, 250, 0, 1);
    DrawSprite(0x80113f, 5, 191, 150, 1);
    DrawSprite(0xf8d, 0, 121, 250, 1);
    DrawText(37, 131, data_0201a61c, GetText(13), 2, 250, 0, 1);
    DrawSprite(0xf4a, 39, 163, 250, 1);
    DrawSprite(0xf4a + info->mGrade, 39, 163, 250, 1);
    DrawSprite(0xf38, 71, 139, 148, 1);
    DrawText(133, 131, data_0201a61c, GetText(24), 2, 250, 0, 1);
    DrawSprite(0xf42 + info->mKind, 116, 170, 250, 1);
    *FormatDecimal(GetValuePlusKindValue(info), numBuf) = 0;
    DrawText(116, 170, data_0201a694, numBuf, 0, 250, 10, 1);
    if (IsValueAtMax(info)) {
        DrawSprite(0xf57 + (gSystemState.mElapsedFrames % 60 <= 3), 130, 187, 140, 1);
    }
    sprintf(textBuf, "+%d", GetPercentOfBattleTotal(info));
    DrawText(190, 164, data_0201a68a, textBuf, 0, 250, 10, 1);
    DrawText(232, 179, data_0201a61c, GetText(15), 1, 250, 0, 1);
    DrawSprite(GetFileValue(0x10d5, info->mKind * 2 + 3) + 0xf32, 242, 175, 248, 1);
}

void SetStateFlagTo1(void) {
    SetStateFlag(1);
}

void UpdateBattleCompare(void) {
    static s32 sWavePlayed;
    char text[8];
    s32 cur[2];
    s32 hp[2];
    int sideA = gBattleView.mState.mPickIndices[0];
    int sideB = gBattleView.mState.mPickIndices[1];
    int i;
    int x;
    int y;
    StageInfo *info;
    StageInfo *pa = &gBattleView.mSide[0];
    StageInfo *pb = &gBattleView.mSide[1];
    hp[0] = GetBattleTotal(pa);
    hp[1] = GetBattleTotal(pb);
    func_02007ffc(&data_020b6ee0.mUnk_fe2, &data_020b6ee0.mUnk_fe4);
    if (data_020b6ee0.mUnk_fee == 0x4000) {
        AddLevel(&data_020b6ee0.mUnk_ff0, 400);
    } else if (data_020b6ee0.mUnk_fee != 0) {
        if (hp[0] > hp[1]) {
            if (gStageInfos[sideB].mValue > -5000) {
                StartSprite(&data_020b6ee0.mSideSprites[1], gSpriteScripts[pb->mKind + 33]);
                gBattleSlotStages[sideA].mInfo.mValue = hp[0] - hp[1] - gBattleSlotStages[sideA].mInfo.mKindValue;
                memcpy(gBattleSlotStages[sideA].mInfo.mTableEntryIds, gBattleView.mSide[0].mTableEntryIds, 10);
                gStageInfos[sideB].mValue = -9999;
                gBattleView.mState.mKeptSide = 0;
                data_020b6ee0.mBattleSlotFlags[sideA] |= BATTLE_SLOT_KEPT;
                SubtractHalfFromList(1, pb->mOthersHalfSumAdded);
                pb->mOthersHalfSumAdded = 0;
            }
        } else if (gBattleSlotStages[sideA].mInfo.mValue > -5000) {
            gStageInfos[sideB].mValue = hp[1] - hp[0] - gStageInfos[sideB].mKindValue;
            memcpy(gStageInfos[sideB].mTableEntryIds, gBattleView.mSide[1].mTableEntryIds, 10);
            gBattleSlotStages[sideA].mInfo.mValue = -9999;
            gBattleView.mState.mKeptSide = 1;
            data_020b6ee0.mBattleSlotFlags[sideA] &= ~BATTLE_SLOT_KEPT;
            SubtractHalfFromList(0, pa->mOthersHalfSumAdded);
            pa->mOthersHalfSumAdded = 0;
            StartSprite(&data_020b6ee0.mSideSprites[0], gSpriteScripts[pa->mKind + 33]);
            if (hp[1] == 0) {
                gStageInfos[sideB].mValue = -9999;
                StartSprite(&data_020b6ee0.mSideSprites[1], gSpriteScripts[pb->mKind + 33]);
            }
        }
        if (hp[0] >= hp[1]) {
            if (gBattleView.mSide[0].mGrade < gBattleView.mSide[1].mGrade) {
                data_020b6ee0.mBattleSlotFlags[sideA] |= BATTLE_SLOT_GRADE_UP;
            }
        }
    }
    i = 0;
    {
        u8 startX[2] = {0x9d, 0x61};
        u8 startY[2] = {0x5d, 0x5d};
        info = &gBattleView.mSide[0];
        for (; i < 2; info++, i++) {
            if (hp[i] > hp[i ^ 1] || data_020b6ee0.mUnk_fee < 10) {
                x = MapRangeFromZero(data_020b6ee0.mUnk_ff0, 0x4000, startX[i], info->mBattleX);
                y = MapRangeFromZero(data_020b6ee0.mUnk_ff0, 0x4000, startY[i], info->mBattleY);
                DrawSprite(info->mStageId + ((i == 1 ? 0x10000 : 0) + 0x1089), x, y, 250, 2);
                DrawSprite(0x41088, x, y, 252, 2);
                if (data_020b6ee0.mSideSprites[i].mUnk_de == 0) {
                    DrawSprite(UpdateSprite(&data_020b6ee0.mSideSprites[i]), x, y, 240, 2);
                }
                cur[i] = GetBattleTotal(info);
                *FormatDecimal(cur[i], text) = 0;
                DrawText(x - 10, y - 40, data_0201a6bc, text, 0, 96, 0, 2);
                DrawSprite(info->mKind + 0x11a1, x - 10, y - 40, 240, 2);
                if (gBattleView.mState.mChangeFrames[i] < 18) {
                    DrawSprite(gBattleView.mState.mChangeFrames[i]++ / 3 + 0x11a7, x - 10, y - 40, 100, 2);
                }
            } else {
                int r = MapRangeFromZero(data_020b6ee0.mUnk_fee, 0x4000, 0, 20);
                if (data_020b6ee0.mUnk_fee < 0x4000) {
                    DrawSprite(r + 0xfec + info->mKind * 21, startX[i], startY[i], 100, 2);
                    if (r == 1 && sWavePlayed == 0) {
                        sWavePlayed = 1;
                        PlayWaveKeepAlive(42, 100, 0, 64, 0xffff, 0);
                    } else if (r > 1) {
                        sWavePlayed = 0;
                    }
                }
            }
        }
    }
    if (data_020b6ee0.mUnk_ff0 == 0 && data_020b6ee0.mUnk_fee == 0) {
        if (cur[0] > 0 && cur[1] > 0) {
            u16 tick = data_020b6ee0.mUnk_ff6++;
            if (tick & 1) {
                s32 a = cur[0];
                s32 b = cur[1];
                if (a > 10) {
                    a = 10;
                }
                if (b > 10) {
                    b = 10;
                }
                if (a >= b) {
                    a = b;
                }
                gBattleView.mSide[0].mSubtracted += a;
                gBattleView.mSide[1].mSubtracted += a;
            }
        } else {
            gBattleView.mState.mChangeFrames[1] = 0;
            gBattleView.mState.mChangeFrames[0] = gBattleView.mState.mChangeFrames[1];
            *(vu16 *) 0x04001050 = 0x2848;
            *(vu16 *) 0x04001052 = 0;
        }
    }
    if (data_020b6ee0.mUnk_ff0 < 0x4000) {
        if (!(cur[0] > 0 && cur[1] > 0) && data_020b6ee0.mUnk_fee < 10) {
            s16 phase = data_020b6ee0.mModel[0].mUnk_00c;
            if (phase == 0) {
                SetupStageModels(cur[0] <= 0 ? gBattleView.mSide[1].mStageId : gBattleView.mSide[0].mStageId);
                data_020b6ee0.mModel[0].mSignals |= 0x10;
                data_020b6ee0.mUnk_fee = 1;
            } else if (phase == 1 && (data_020b6ee0.mModel[0].mAnimEnded == 1 ||
                                      gTouchInput.mTrail.mTouchState.mButtonState == 0x20000190)) {
                SetupStageModels(cur[0] <= 0 ? gBattleView.mSide[0].mStageId : gBattleView.mSide[1].mStageId);
                data_020b6ee0.mModel[0].mSignals |= 2;
            } else if (phase == 2 && (data_020b6ee0.mModel[0].mAnimEnded == 1 ||
                                      gTouchInput.mTrail.mTouchState.mButtonState == 0x20000190)) {
                int id = GetNodeValue(data_020b6ee0.mSelectedNodeIndex, 7) + 23;
                AddLevel(&data_020b6ee0.mUnk_fee, 480);
                SetFaded(1);
                if (cur[0] <= 0) {
                    id = 11;
                }
                // Sets mAnimEnded, so that the test below switches to the shared model at once.
                data_020b6ee0.mModel[0].mUnk_172 |= 0x400;
                SetupSharedModel(id, 0);
            }
        }
        if (data_020b6ee0.mModel[0].mUnk_00c != 2 || data_020b6ee0.mModel[0].mAnimEnded != 1) {
            SetCameraFromModel();
            DrawMap(gMapState.mCamera.mPosGoal.x, gMapState.mCamera.mPosGoal.z);
            func_020071ac();
            DrawBattleModels();
        } else {
            DrawSharedModel();
            AddLevel(&data_020b6ee0.mUnk_fee, 480);
        }
    }
    if (data_020b6ee0.mUnk_fee == 0x4000 && data_020b6ee0.mUnk_ff0 == 0x4000) {
        if (gBattleSlotStages[0].mInfo.mValue < -5000 && gBattleSlotStages[1].mInfo.mValue < -5000 &&
            gBattleSlotStages[2].mInfo.mValue < -5000) {
            data_020b6ee0.mBattleSlotFlags[0] &= ~BATTLE_STAGE_INFOS_OUT;
            SetStateFlagTo1();
            return;
        }
        if (gStageInfos[0].mValue < -5000 && gStageInfos[1].mValue < -5000 && gStageInfos[2].mValue < -5000) {
            data_020b6ee0.mBattleSlotFlags[0] |= BATTLE_STAGE_INFOS_OUT;
            SetStateFlagTo1();
            return;
        }
        gBattleView.mState.mFirstSide ^= 1;
        data_020b6ee0.mBattleState = BATTLE_STATE_STAGE_PICK;
        gBattleView.mState.mUnk_1b = -1;
        data_020b6ee0.mUnk_ff0 = 0;
        SetFaded(1);
    }
}

// mode is the side. Side 0 is done on key bit 1; side 1 once data_020b6ee0.mEntryFrames, which this counts up, passes
// 90. Either side is done when it skips (mSkipEntry) or on touch state 0x200000c8 (button 200).
int IsEntryPickDone(StageInfo *info, int mode) {
    if (info->mSkipEntry == 1 || (mode == 0 && (gSystemState.mKeysPressed & 1)) ||
        gTouchInput.mTrail.mTouchState.mButtonState == 0x200000c8 || (mode == 1 && data_020b6ee0.mEntryFrames++ > 90)) {
        return 1;
    }
    return 0;
}

int GetEntryStartSharedModelIndex(int order) {
    if (gBattleView.mState.mFirstSide ^ (order == 0)) {
        return 1;
    }
    return GetNodeValue(data_020b6ee0.mSelectedNodeIndex, 7) + 2;
}

int GetEntryStartWaveId(int order) {
    static const s32 sEntryStartWaveIds[4] = {2, 0x1a1, 0x33, 0x17e};
    if (gBattleView.mState.mFirstSide ^ (order == 0)) {
        return data_020b1084.mLanguage + 0x6b;
    }
    return data_020b1084.mLanguage + sEntryStartWaveIds[GetNodeValue(data_020b6ee0.mSelectedNodeIndex, 7)];
}

void UpdateBattleEntryPick(int side) {
    u8 curX[2] = {157, 97};
    u8 curY[2] = {93, 93};
    char numText[8];
    u8 otherX0[4] = {138, 188, 19, 69};
    u8 otherY0[4] = {187, 162, 79, 54};
    u8 otherX1[4] = {177, 207, 47, 77};
    u8 otherY1[4] = {118, 93, 93, 68};
    char valueText[8];
    int iconA;
    int spriteA;
    int spriteB;
    int iconB;
    int spriteC;
    int baseId;
    int kind;
    int kindId;
    int active;
    int i, j, k;
    int x, y;
    int flags;
    StageInfo *info;
    StageInfo *other;

    AddLevel(&data_020b6ee0.mUnk_fee, 400);
    active = 0;
    if (data_020b6ee0.mUnk_fee == 0x4000) {
        func_02007ffc(&data_020b6ee0.mUnk_fe2, &data_020b6ee0.mUnk_fe4);
        if (data_020b6ee0.mUnk_ff6 == 0x4000) {
            AddLevel(&data_020b6ee0.mUnk_ff0, 400);
            if (data_020b6ee0.mUnk_ff0 < 0x4000) {
                *(vu16 *) 0x04001052 = MapRangeFromZero(data_020b6ee0.mUnk_ff0, 0x4000, 0, 16);
            } else if (data_020b6ee0.mUnk_ff8 == 0x4000) {
                int a;
                EaseLevel(&data_020b6ee0.mUnk_ff2, 4);
                a = MapRangeFromZero(data_020b6ee0.mUnk_ff2, 0x4000, 0, 8);
                *(vu16 *) 0x04001050 = 0x2840;
                *(vu16 *) 0x04001052 = (a << 8) + 16 - a;
            } else if (data_020b6ee0.mUnk_ff8 == 0) {
                j = func_020078f8();
                if (j != -1) {
                    SetFaded(1);
                    ResetTexCache();
                    ResetPlttCache();
                    SetupSharedModel(j + 12, 0);
                    data_020b6ee0.mUnk_ff8 = 1;
                    gBattleView.mState.mSubtractedSide = gBattleView.mSide[0].mKind == j;
                    gBattleView.mState.mSubtractedTarget =
                        GetPercentOfBattleTotal(&gBattleView.mSide[(gBattleView.mState.mSubtractedSide & 0x7f) ^ 1]);
                } else {
                    data_020b6ee0.mUnk_ff8 = 0x4000;
                }
            } else if (data_020b6ee0.mUnk_ff8 == 1) {
                DrawSharedModel();
                if (GetSharedModelPlayFrames() > 50) {
                    int t =
                        MapRange(GetSharedModelPlayFrames() < 100 ? GetSharedModelPlayFrames() : 100, 50, 100, 100, 0);
                    *FormatDecimal(GetBattleTotal(&gBattleView.mSide[gBattleView.mState.mSubtractedSide & 0x7f]),
                                   valueText) = 0;
                    DrawSprite(gBattleView.mSide[gBattleView.mState.mSubtractedSide & 0x7f].mKind + 0xf42, 106, t + 168,
                               200, 1);
                    DrawText(106, t + 168, data_0201a694, valueText, 0, 96, 0, 1);
                    if (GetSharedModelPlayFrames() >= 100) {
                        if (GetSharedModelPlayFrames() <= 250) {
                            gBattleView.mSide[gBattleView.mState.mSubtractedSide & 0x7f].mSubtracted =
                                MapRange(GetSharedModelPlayFrames(), 100, 250, 0, gBattleView.mState.mSubtractedTarget);
                        } else {
                            if (!(gBattleView.mState.mSubtractedSide & 0x80)) {
                                SetSidesSubtracted();
                                gBattleView.mState.mSubtractedSide |= 0x80;
                            }
                            if (IsSharedModelDone() == 1) {
                                SetFaded(1);
                                data_020b6ee0.mUnk_ff8 = 0x4000;
                            }
                        }
                    }
                }
            }
        } else {
            switch (data_020b6ee0.mEntryStartStep) {
                case 0:
                    SetupSharedModel(GetEntryStartSharedModelIndex(0), 0);
                    data_020b6ee0.mEntryStartStep++;
                    SetFaded(1);
                    PlayWaveKeepAlive(GetEntryStartWaveId(0), 100, 0, 64, 0xffff, 0);
                    break;
                case 1:
                    DrawSharedModel();
                    if (IsSharedModelDone() == 1) {
                        data_020b6ee0.mEntryStartStep++;
                        SetupStageModels(gBattleView.mSide[gBattleView.mState.mFirstSide].mStageId);
                        SetFaded(1);
                    }
                    break;
                case 2:
                    func_020071ac();
                    active = 1;
                    data_020b6ee0.mModel[0].mSignals |= 1;
                    DrawBattleModels();
                    if (data_020b6ee0.mModel[0].mAnimEnded == 1 ||
                        gTouchInput.mTrail.mTouchState.mButtonState == 0x20000190) {
                        SetupSharedModel(GetEntryStartSharedModelIndex(1), 0);
                        data_020b6ee0.mEntryStartStep++;
                        SetFaded(1);
                        PlayWaveKeepAlive(GetEntryStartWaveId(1), 100, 0, 64, 0xffff, 0);
                    }
                    break;
                case 3:
                    DrawSharedModel();
                    if (IsSharedModelDone() == 1) {
                        data_020b6ee0.mEntryStartStep++;
                        SetupStageModels(gBattleView.mSide[gBattleView.mState.mFirstSide ^ 1].mStageId);
                        SetFaded(1);
                    }
                    break;
                case 4:
                    func_020071ac();
                    active = 1;
                    data_020b6ee0.mModel[0].mSignals |= 1;
                    DrawBattleModels();
                    if (data_020b6ee0.mModel[0].mAnimEnded == 1 ||
                        gTouchInput.mTrail.mTouchState.mButtonState == 0x20000190) {
                        data_020b6ee0.mEntryStartStep = 6;
                        SetFaded(1);
                        if (data_020b6ee0.mModel[0].mAnimEnded == 1 ||
                            gTouchInput.mTrail.mTouchState.mButtonState == 0x20000190) {
                            data_020b6ee0.mUnk_ff6 = 0x4000;
                        }
                    }
                    break;
            }
        }
    } else {
        *(vu16 *) 0x04001050 = 0x2848;
        *(vu16 *) 0x04001052 = active;
    }

    k = 0;
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 3; j++) {
            flags = 0;
            if (j == gBattleView.mState.mPickIndices[i]) {
                info = &gBattleView.mSide[i];
                x = MapRangeFromZero(data_020b6ee0.mUnk_fee, 0x4000, info->mBattleX, curX[i]);
                y = MapRangeFromZero(data_020b6ee0.mUnk_fee, 0x4000, info->mBattleY, curY[i]);
                if (i == gBattleView.mState.mSubtractedSide ||
                    (gBattleView.mState.mChangeFrames[i] < 18 && gBattleView.mState.mTotalFell[i] == 1)) {
                    x += RandomRange(-2, 2);
                }
            } else {
                info = (i == 0) ? &gBattleSlotStages[j].mInfo : &gStageInfos[j];
                if (gBattleView.mSide[i].mOthersHalfSumAdded == 0) {
                    x = otherX0[k];
                    y = otherY0[k];
                    flags = (data_020b6ee0.mUnk_ff6 == 0x4000) ? 0x40000 : -1;
                } else {
                    x = otherX1[k];
                    y = otherY1[k];
                    flags = 0;
                }
                k++;
            }
            if (info->mValue > -5000) {
                DrawSprite(flags | (((i == 1) ? 0x10000 : 0) + 0x1089 + info->mStageId), x, y, 250, 2);
                DrawSprite(0x41088, x, y, 252, 2);
            }
            if (j == gBattleView.mState.mPickIndices[i]) {
                if (i == side && data_020b6ee0.mUnk_ff6 == 0x4000) {
                    int frame = (gSystemState.mElapsedFrames >> 2) & 7;
                    DrawSprite(frame + 0x11be + (i == 1) * 8, x, y, 251, 2);
                }
                if (data_020b6ee0.mSideSprites[i].mUnk_de == 0) {
                    DrawSprite(UpdateSprite(&data_020b6ee0.mSideSprites[i]), x, y, 240, 2);
                }
                *FormatDecimal(GetBattleTotal(info), numText) = 0;
                DrawText(x - 10, y - 40, data_0201a6bc, numText, 0, 96, 0, 2);
                DrawSprite(info->mKind + 0x11a1, x - 10, y - 40, 240, 2);
                if (gBattleView.mState.mChangeFrames[i] < 18) {
                    DrawSprite(gBattleView.mState.mChangeFrames[i] / 3 + 0x11a7, x - 10, y - 40, 100, 2);
                }
            }
        }
        if (gBattleView.mState.mChangeFrames[i] < 18) {
            gBattleView.mState.mChangeFrames[i]++;
        }
    }

    info = &gBattleView.mSide[side];
    other = &gBattleView.mSide[side ^ 1];
    if (data_020b6ee0.mUnk_ff8 == 0x4000) {
        int baseX = (side == 0) ? 100 : 0;
        DrawSprite(0xfeb, baseX + 78, 152, 150, 2);
        for (j = 0; j < 5; j++) {
            if (info->mTableEntryIds[j] >= 0) {
                int slotX = j * 26 + 24;
                GetTableEntrySpriteIds(info, info->mTableEntryIds[j], &iconA, &iconB);
                GetTableEntryInfoIds(info, info->mTableEntryIds[j], &baseId, &spriteA, &spriteB, &spriteC, &kind,
                                     &kindId);
                AddTouchButtonWithSounds(j, baseX + slotX, 141, 0, 0, 0x10b4, 0, (side == 0) ? 0x167 : 0);
                if (side == 0 || (gBattleView.mState.mEntryCursor == j && data_020b6ee0.mEntryFrames > 10)) {
                    DrawSprite(iconA, baseX + slotX, 141, 148, 2);
                    DrawSprite(iconB, baseX + slotX + 8, 137, 146, 2);
                } else {
                    DrawSprite(0x1194, baseX + slotX, 141, 148, 2);
                }
                if (gBattleView.mState.mEntryCursor == j && info->mSkipEntry == 0) {
                    int panelX = MapRangeFromZero(data_020b6ee0.mEntryPanelXLevel, 0x4000, -50, 53);
                    int panelY = MapRangeFromZero(data_020b6ee0.mEntryPanelYLevel, 0x4000, 80, 0);
                    int textId0, textId1;
                    EaseLevel(&data_020b6ee0.mEntryPanelXLevel, 4);
                    EaseLevel(&data_020b6ee0.mEntryPanelYLevel, 4);
                    DrawSprite(UpdateSprite(&data_020b6ee0.mSelectionSprite), baseX + slotX, 141, 147, 2);
                    DrawSprite(spriteA, panelX, 133, 150, 1);
                    DrawSprite(spriteB, panelX, 133, 140, 1);
                    panelX += 25;
                    DrawSprite(spriteC, panelX, 123, 130, 1);
                    DrawSprite(0x80113f, 5, panelY + 191, 250, 1);
                    if (kind == 14) {
                        textId0 = info->mKind + 184;
                        textId1 = info->mKind + 172;
                    } else if (kind == 15) {
                        textId0 = kindId + 190;
                        textId1 = kindId + 178;
                    } else {
                        textId0 = baseId + 138;
                        textId1 = baseId + 155;
                    }
                    DrawText(95, panelY + 147, data_0201a61c, GetText(textId0), 0, 240, 16, 1);
                    panelY += 164;
                    DrawText(18, panelY, data_0201a612, GetText(textId1), 0, 232, 10, 1);
                }
            }
        }
        if (gBattleView.mState.mUnk_1b != side) {
            gBattleView.mState.mUnk_1b = side;
            SetupStageModels(gBattleView.mSide[side].mStageId);
        }
        data_020b6ee0.mModel[0].mSignals |= 4;
        DrawBattleModels();
        if (GetBattleTotal(info) <= 0 || GetBattleTotal(other) <= 0) {
            data_020b6ee0.mBattleState = BATTLE_STATE_COMPARE;
            memset(&data_020b6ee0.mUnk_fee, 0, 18);
        } else if (gBattleView.mState.mEntryTimer == 60) {
            if (side == 0) {
                int prev = gBattleView.mState.mEntryCursor;
                u32 touch = gTouchInput.mTrail.mTouchState.mButtonState;
                int pos = (u16) touch;
                if ((touch & 0x80000000) && pos != 0xffff && pos < 200) {
                    gBattleView.mState.mEntryCursor = pos;
                }
                if (gSystemState.mKeysPressed & 0x10) {
                    do {
                        gBattleView.mState.mEntryCursor++;
                        if (gBattleView.mState.mEntryCursor == 5) {
                            gBattleView.mState.mEntryCursor = 0;
                        }
                    } while (info->mTableEntryIds[gBattleView.mState.mEntryCursor] == -1);
                } else if (gSystemState.mKeysPressed & 0x20) {
                    do {
                        gBattleView.mState.mEntryCursor--;
                        if (gBattleView.mState.mEntryCursor < 0) {
                            gBattleView.mState.mEntryCursor = 4;
                        }
                    } while (info->mTableEntryIds[gBattleView.mState.mEntryCursor] == -1);
                }
                if (prev != gBattleView.mState.mEntryCursor) {
                    data_020b6ee0.mEntryPanelXLevel = 0;
                }
                if (gSystemState.mKeysPressed & 2) {
                    info->mSkipEntry = 1;
                }
            } else {
                gBattleView.mState.mEntryCursor = FindBestSide1Entry();
                if (gBattleView.mState.mEntryCursor < 0) {
                    info->mSkipEntry = 1;
                }
            }
            if (info->mTableEntryIds[gBattleView.mState.mEntryCursor] == -1) {
                gBattleView.mState.mEntryCursor++;
                if (gBattleView.mState.mEntryCursor == 5) {
                    gBattleView.mState.mEntryCursor = 0;
                }
            }
            if (IsEntryPickDone(info, side) == 1) {
                gBattleView.mState.mUnk_13 = 0;
                if (info->mTableEntryIds[gBattleView.mState.mEntryCursor] >= 0 && info->mSkipEntry == 0) {
                    int oldA = GetBattleTotal(info);
                    int oldB = GetBattleTotal(other);
                    int newA, newB;
                    gBattleView.mApplySide = side;
                    gBattleView.mState.mTableEntryIndex =
                        FindTableEntryIndexUnbounded(info->mTableEntryIds[gBattleView.mState.mEntryCursor]);
                    ApplyTableEntry();
                    if (info->mStageIndex != -1 &&
                        GetTableEntryValue(FindTableEntryIndex(info->mTableEntryIds[gBattleView.mState.mEntryCursor]),
                                           9) == 1) {
                        gStages[info->mStageIndex].mInfo.mTableEntryIds[gBattleView.mState.mEntryCursor] = -1;
                    }
                    info->mTableEntryIds[gBattleView.mState.mEntryCursor] = -1;
                    newA = GetBattleTotal(info);
                    newB = GetBattleTotal(other);
                    info->mPrevBattleTotal = oldA;
                    other->mPrevBattleTotal = oldB;
                    if (oldA != newA) {
                        gBattleView.mState.mChangeFrames[side] = 0;
                        if (oldA < newA) {
                            data_020b6ee0.mModel[0].mSignals |= 8;
                            gBattleView.mState.mTotalFell[side] = 0;
                            StartSprite(&data_020b6ee0.mSideSprites[side], gSpriteScripts[32]);
                        } else {
                            gBattleView.mState.mTotalFell[side] = 1;
                            StartSprite(&data_020b6ee0.mSideSprites[side], gSpriteScripts[info->mKind + 33]);
                        }
                    }
                    if (oldB != newB) {
                        gBattleView.mState.mChangeFrames[side ^ 1] = 0;
                        if (oldB < newB) {
                            gBattleView.mState.mTotalFell[side ^ 1] = 0;
                            StartSprite(&data_020b6ee0.mSideSprites[side ^ 1], gSpriteScripts[32]);
                        } else {
                            gBattleView.mState.mTotalFell[side ^ 1] = 1;
                            StartSprite(&data_020b6ee0.mSideSprites[side ^ 1], gSpriteScripts[other->mKind + 33]);
                        }
                    }
                }
                data_020b6ee0.mEntryFrames = 0;
                gBattleView.mState.mEntryTimer = 59;
                info->mSkipEntry = 0;
            }
        } else if (gBattleView.mState.mEntryTimer == 0) {
            gBattleView.mState.mEntryPickCount++;
            if (gBattleView.mState.mEntryPickCount > 3) {
                data_020b6ee0.mBattleState = BATTLE_STATE_COMPARE;
                memset(&data_020b6ee0.mUnk_fee, 0, 18);
            }
            gBattleView.mState.mEntryCursor = 0;
            gBattleView.mState.mEntryTimer = 60;
        } else if (gBattleView.mModel[1].mId == 0) {
            gBattleView.mState.mEntryTimer--;
        }
        if (!((gBattleView.mState.mEntryPickCount ^ gBattleView.mState.mFirstSide) & 1) &&
            gBattleView.mState.mEntryTimer == 60 && gBattleView.mState.mEntryPickCount <= 3) {
            if (info->mTableEntryIds[gBattleView.mState.mEntryCursor] >= 0) {
                DrawConfirmButton();
            }
            AddTouchButtonWithSounds(201, 128, 179, 0, 0, 0x10b2, 0, 0x167);
            DrawSprite((gTouchInput.mTrail.mTouchState.mButtonState == 0x400000c9) + 0x10b2, 128, 179, 142, 2);
            DrawText(128, 179, data_0201a61c, GetText(61), 2, 250, 0, 2);
            if (gTouchInput.mTrail.mTouchState.mButtonState == 0x200000c9) {
                info->mSkipEntry = 1;
            }
        }
    }
    if (!(data_020b6ee0.mEntryStartStep & 1) && (data_020b6ee0.mUnk_ff8 == 0 || data_020b6ee0.mUnk_ff8 == 0x4000)) {
        if (gBattleView.mState.mUnk_1b == -1 && active == 0) {
            SetCameraFromMapEntries0And1();
        } else {
            SetCameraFromModel();
        }
        DrawMap(gMapState.mCamera.mPosGoal.x, gMapState.mCamera.mPosGoal.z);
    }
}

// One frame of the stage choice before a round. Brings in both sides' three stage panels and their values, and moves
// the cursor (picked panel, keys, or ComparePickIndicesWithBestPair's choice after a delay). Starts the round once
// both chosen stages are still in play and the choice is confirmed.
void UpdateBattleStagePick(void) {
    u8 xs[6] = {118, 168, 218, 29, 79, 129};
    u8 ys[6] = {177, 152, 127, 129, 104, 79};
    char text[8];
    int a;
    int selecting = 0;
    int i;
    int j;
    int move;
    StageInfo *info;

    if (data_020b6ee0.mUnk_fee == 0) {
        memset(data_020b6ee0.mSideListLevels, 0, 8);
        gBattleView.mState.mPickIndices[1] = 0;
        gBattleView.mState.mPickIndices[0] = gBattleView.mState.mPickIndices[1];
        data_020b6ee0.mModel[1].mId = 0;
        data_020b6ee0.mModel[0].mUnk_172 |= 0x10;
        gBattleView.mState.mUnk_1b = -2;
    }
    AddLevel(&data_020b6ee0.mUnk_fee, 400);
    if (data_020b6ee0.mUnk_fee == 0x4000) {
        AddLevel(&data_020b6ee0.mUnk_ff0, 400);
        if (data_020b6ee0.mUnk_ff0 < 0x4000) {
            *(vu16 *) 0x04001052 = MapRangeFromZero(data_020b6ee0.mUnk_ff0, 0x4000, 0, 16);
        } else {
            int level;
            EaseLevel(&data_020b6ee0.mUnk_ff2, 4);
            level = MapRangeFromZero(data_020b6ee0.mUnk_ff2, 0x4000, 0, 8);
            *(vu16 *) 0x04001050 = 0x2840;
            *(vu16 *) 0x04001052 = (level << 8) + 16 - level;
            AddLevel(&data_020b6ee0.mUnk_fe2, 800);
        }
    }
    if (data_020b6ee0.mSideListLevels[0] == 0x4000) {
        AddLevel(&data_020b6ee0.mUnk_fe4, 800);
        if (data_020b6ee0.mSideListLevels[1] == 0x4000) {
            AddLevel(&data_020b6ee0.mUnk_fea, 800);
        }
    }
    func_02007ffc(&data_020b6ee0.mUnk_fe2, &data_020b6ee0.mUnk_fe4);
    if (data_020b6ee0.mUnk_fe2 == 0x4000) {
        for (i = 0; i < 2; i++) {
            if (i != 0 && data_020b6ee0.mUnk_fe4 != 0x4000) {
                continue;
            }
            AddLevel(&data_020b6ee0.mSideListLevels[i], 170);
            for (j = 0; j < 3; j++) {
                info = i == 0 ? &gBattleSlotStages[j].mInfo : &gStageInfos[j];
                if (GetValuePlusKindValue(info) > -5000) {
                    int x = xs[i * 3 + j];
                    int y = ys[i * 3 + j];
                    int d = (2 - j) * 1920;
                    if (data_020b6ee0.mSideListLevels[0] >= j * 1920) {
                        int hit;
                        a = MapRangeFromZero(data_020b6ee0.mSideListLevels[i] < 0x4000 - d
                                                 ? data_020b6ee0.mSideListLevels[i]
                                                 : 0x4000 - d,
                                             0x4000 - d, 0, 23);
                        if (data_020b6ee0.mSideListLevels[i] < 0x4000) {
                            DrawSprite(a + 0x106a, x, y, 248, 2);
                        }
                        if (a >= 16) {
                            a = (data_020b6ee0.mUnk_ff0 == 0x4000 ||
                                 (gBattleView.mState.mKeptSide == i && j == gBattleView.mState.mPickIndices[i]))
                                    ? 0
                                    : 0x40000;
                            AddTouchButtonWithSounds(j + 16 + i * 4, x, y, 0, 0, 0x1089, 0, 0x167);
                            DrawSprite(a | (info->mStageId + ((i == 1 ? 0x10000 : 0) + 0x1089)), x, y, 250, 2);
                            DrawSprite(0x41088, x, y, 252, 2);
                            info->mBattleX = x;
                            info->mBattleY = y;
                            hit = 0;
                            if (i == 0) {
                                if (data_020b6ee0.mBattleSlotFlags[j] & BATTLE_SLOT_GRADE_UP) {
                                    hit = 1;
                                }
                            } else {
                                int s = gBattleView.mState.mPickIndices[0];
                                if (!(data_020b6ee0.mBattleSlotFlags[s] & BATTLE_SLOT_GRADE_UP) &&
                                    gBattleSlotStages[s].mInfo.mGrade < gStageInfos[j].mGrade) {
                                    hit = 1;
                                }
                            }
                            if (hit == 1) {
                                DrawSprite(MapRangeFromZero(gSystemState.mElapsedFrames & 31, 31, 0, 8) + 0x11ad,
                                           x + 22, y - 5, 230, 2);
                            }
                        }
                    }
                }
            }
        }
    }
    if (gBattleView.mState.mKindValueLevel > 0x3f6a && gBattleView.mState.mKindValueLevel < 0x4000) {
        gBattleView.mState.mChangeFrames[0] = 0;
    }
    AddLevel(&gBattleView.mState.mKindValueLevel, 150);
    for (j = 0; j < 2; j++) {
        for (i = 0; i < 3; i++) {
            int v;
            info = j == 0 ? &gBattleSlotStages[i].mInfo : &gStageInfos[i];
            v = GetValuePlusKindValue(info);
            v -= MapRangeFromZero(gBattleView.mState.mKindValueLevel, 0x4000, info->mKindValue, 0);
            if (v > -5000) {
                int x = MapRangeFromZero(data_020b6ee0.mUnk_fee, 0x4000, info->mBattleX, xs[j * 3 + i]) - 10;
                int y = MapRangeFromZero(data_020b6ee0.mUnk_fee, 0x4000, info->mBattleY, ys[j * 3 + i]) - 40;
                *FormatDecimal(v, text) = 0;
                DrawText(x, y, data_0201a6bc, text, 0, 96, 0, 2);
                DrawSprite(info->mKind + 0x11a1, x, y, 240, 2);
                if (gBattleView.mState.mChangeFrames[0] < 18 && info->mKindValue != 0) {
                    DrawSprite(gBattleView.mState.mChangeFrames[0] / 3 + 0x11a7, x, y, 100, 2);
                }
            }
        }
    }
    if (gBattleView.mState.mChangeFrames[0] < 18) {
        gBattleView.mState.mChangeFrames[0]++;
    }
    move = 0;
    if (data_020b6ee0.mSideListLevels[1] == 0x4000) {
        DrawBattleStageInfo(gBattleView.mState.mPickSide == 0
                                ? &gBattleSlotStages[gBattleView.mState.mPickIndices[0]].mInfo
                                : &gStageInfos[gBattleView.mState.mPickIndices[1]]);
        selecting = 1;
        if (gBattleView.mState.mFirstSide == 0) {
            u32 pick = gTouchInput.mTrail.mTouchState.mButtonState;
            u16 keys;
            if ((pick & 0x40000000) && (u16) pick != 0xffff) {
                int n = (u16) pick - 16;
                int col = n % 4;
                int row = n / 4;
                if (row < 2) {
                    gBattleView.mState.mPickSide = row;
                    gBattleView.mState.mPickIndices[gBattleView.mState.mPickSide] = col;
                }
            }
            keys = gSystemState.mKeysPressed;
            if (keys & 0x40) {
                if (gBattleView.mState.mPickSide == 0) {
                    gBattleView.mState.mPickSide = 1;
                }
            } else if (keys & 0x80) {
                if (gBattleView.mState.mPickSide == 1) {
                    gBattleView.mState.mPickSide = 0;
                }
            }
            if (keys & 0x10) {
                move = 1;
            } else if (keys & 0x20) {
                move = -1;
            } else if (keys & 1) {
                move = SELECT_CONFIRM;
            }
        } else if (data_020b6ee0.mUnk_ff8 < 60) {
            data_020b6ee0.mUnk_ff8++;
        } else {
            int r = ComparePickIndicesWithBestPair();
            move = 1;
            if (r & 1) {
                gBattleView.mState.mPickSide = 0;
            } else if (r & 2) {
                gBattleView.mState.mPickSide = 1;
            } else {
                move = SELECT_CONFIRM;
            }
            data_020b6ee0.mUnk_ff8 = 0;
        }
    }
    if (move != 0 && move != SELECT_CONFIRM) {
        do {
            int s;
            j = gBattleView.mState.mPickSide;
            gBattleView.mState.mPickIndices[j] = move + gBattleView.mState.mPickIndices[j];
            if (gBattleView.mState.mPickIndices[j] > 2) {
                gBattleView.mState.mPickIndices[j] = 0;
            } else if (gBattleView.mState.mPickIndices[j] < 0) {
                gBattleView.mState.mPickIndices[j] = 2;
            }
            s = gBattleView.mState.mPickIndices[j];
            info = j == 0 ? &gBattleSlotStages[s].mInfo : &gStageInfos[s];
        } while (GetValuePlusKindValue(info) <= -5000);
    }
    if (data_020b6ee0.mSideListLevels[1] == 0x4000) {
        for (j = 0; j < 2; j++) {
            int s;
            int yi;
            int hi;
            for (;;) {
                s = gBattleView.mState.mPickIndices[j];
                if (GetValuePlusKindValue(j == 0 ? &gBattleSlotStages[s].mInfo : &gStageInfos[s]) > -5000) {
                    break;
                }
                gBattleView.mState.mPickIndices[j]++;
                if (gBattleView.mState.mPickIndices[j] > 2) {
                    gBattleView.mState.mPickIndices[j] = 0;
                }
            }
            a = j == gBattleView.mState.mPickSide ? (gSystemState.mElapsedFrames >> 2) & 7 : 0;
            yi = j == 0 ? s : s + 3;
            hi = j == 1 ? 1 : 0;
            DrawSprite(a + (hi * 8 + 0x11be), xs[j == 0 ? s : s + 3], ys[yi], 251, 2);
        }
        if (move == SELECT_CONFIRM || gTouchInput.mTrail.mTouchState.mButtonState == 0x200000c8) {
            if (gBattleSlotStages[gBattleView.mState.mPickIndices[0]].mInfo.mValue > -5000 &&
                gStageInfos[gBattleView.mState.mPickIndices[1]].mValue > -5000) {
                data_020b6ee0.mBattleState = BATTLE_STATE_ENTRY_PICK;
                gBattleView.mState.mChangeFrames[1] = 0xff;
                gBattleView.mState.mChangeFrames[0] = gBattleView.mState.mChangeFrames[1];
                gBattleView.mState.mEntryCursor = 0;
                gBattleView.mState.mEntryPickCount = 0;
                gBattleView.mState.mEntryTimer = 60;
                gBattleView.mState.mSubtractedSide = -1;
                gBattleView.mState.mUnk_1b = -1;
                gBattleView.mState.mTotalFell[1] = 0;
                gBattleView.mState.mTotalFell[0] = gBattleView.mState.mTotalFell[1];
                SetupBattleSides(gBattleView.mState.mPickIndices[0], gBattleView.mState.mPickIndices[1]);
                memset(&data_020b6ee0.mUnk_fee, 0, 18);
                FreeVramBanks(2);
                InitGxPool(0x1c7, 0x1c6, 0x100, 16);
                return;
            }
        }
        if (gBattleView.mState.mFirstSide == 0) {
            DrawConfirmButton();
        }
        data_020b6ee0.mModel[0].mSignals |= 4;
    } else {
        data_020b6ee0.mModel[0].mSignals |= 1;
    }
    {
        int cur = gBattleView.mState.mPickSide;
        int s = gBattleView.mState.mPickIndices[cur];
        u8 id = cur == 1 ? gStageInfos[s].mStageId : gBattleSlotStages[s].mInfo.mStageId;
        if (gBattleView.mState.mUnk_1b != id) {
            SetupStageModels(id);
            data_020b6ee0.mModel[0].mUnk_172 |= 0x80;
            data_020b6ee0.mModel[0].mUnk_172 |= 0x20;
            if (gBattleView.mState.mUnk_1b == -2) {
                StartBattleViewModel(1);
            }
            gBattleView.mState.mUnk_1b = id;
        }
    }
    if (selecting == 0) {
        SetCameraFromMapEntries0And2();
        gMapState.mCamera.mEaseShift = 4;
        UpdateBattleViewModel();
    } else {
        SetCameraFromModel();
        DrawBattleModels();
    }
    DrawMap(gMapState.mCamera.mPosGoal.x, gMapState.mCamera.mPosGoal.z);
}

void UpdateBattleKindValues(void) {
    char text[8];
    int i;
    int j;
    int side;
    int from;
    int base = gBattleView.mState.mKindValueRow * 7;
    int k;
    int o;
    int value;
    func_02007ffc(&data_020b6ee0.mUnk_fe2, &data_020b6ee0.mUnk_fe4);
    if (data_020b6ee0.mUnk_fe2 == 0) {
        gBattleView.mState.mChangeFrames[0] = 0xff;
    }
    if (data_020b6ee0.mUnk_fee == 0) {
        DrawSprite(0x8011d0, 128, 36, 200, 1);
        DrawText(128, 36, data_0201a61c, GetText(68), 2, 240, 16, 1);
        for (j = 0; j < 3; j++) {
            for (i = 0; i < 2; i++) {
                k = j * 2 + i;
                value = GetFileValue(0x1e7, k + 3 + base);
                DrawSprite(0x11ce, i * 110 + 67, j * 34 + 76, 210, 1);
                DrawSprite(k + 0xebe, i * 110 + 46, j * 34 + 71, 200, 1);
                *FormatDecimal(value, text) = 0;
                DrawText(i * 110 + 67, j * 34 + 76, data_0201a64e, text, 0, 250, 100, 1);
            }
        }
        SetCameraFromMapEntries0And1();
        DrawMap(gMapState.mCamera.mPosGoal.x, gMapState.mCamera.mPosGoal.z);
        DrawSprite(0xec4, 128, 45, 100, 2);
        DrawSprite(0x11cf, 127, 153, 200, 2);
        DrawText(127, 153, data_0201a61c, GetText(69), 2, 240, 16, 2);
        DrawConfirmButton();
        EaseLevel(&data_020b6ee0.mUnk_fe2, 4);
        EaseLevel(&data_020b6ee0.mUnk_fe4, 4);
        EaseLevel(&data_020b6ee0.mUnk_ff0, 4);
    } else {
        DrawSharedModel();
        EaseLevel(&data_020b6ee0.mUnk_fe2, -8);
        EaseLevel(&data_020b6ee0.mUnk_fe4, -8);
    }
    side = 0;
    {
        s16 start[2] = {-5000, 5256};
        u8 end[2] = {39, 196};
        s8 step[2] = {10, -10};
        for (; side < 2; side++) {
            o = 1 - side;
            from = start[o];
            for (i = 0; i < 3; i++) {
                StageInfo *info = (side == 0) ? &gBattleSlotStages[2 - i].mInfo : &gStageInfos[2 - i];
                if (GetValuePlusKindValue(info) > -5000) {
                    int x = MapRange(data_020b6ee0.mUnk_ff0, 0, 0x4000, from, end[o] + i * step[o]);
                    int y = i * 20 + 70;
                    info->mBattleX = x + 10;
                    info->mBattleY = y + 40;
                    *FormatDecimal(GetValuePlusKindValue(info), text) = 0;
                    DrawSprite(info->mKind + 0x11a1, x, y, 150, 2);
                    DrawText(x, y, data_0201a6bc, text, 0, 96, 0, 2);
                }
            }
        }
    }
    if (data_020b6ee0.mUnk_fee == 0) {
        if ((gSystemState.mKeysPressed & 1) || gTouchInput.mTrail.mTouchState.mButtonState == 0x200000c8) {
            SetFaded(1);
            ResetTexCache();
            SetupSharedModel(GetNodeValue(data_020b6ee0.mSelectedNodeIndex, 7) + 19, 0);
            PlayWaveKeepAlive(91, 100, 0, 64, 0xffff, 0);
            data_020b6ee0.mUnk_fee = 1;
        }
    } else if (IsSharedModelDone() == 1) {
        int id;
        SetFaded(1);
        data_020b6ee0.mBattleState = BATTLE_STATE_STAGE_PICK;
        data_020b6ee0.mUnk_fe2 = 0;
        data_020b6ee0.mUnk_fe4 = 0;
        gBattleView.mState.mPickSide = 0;
        gBattleView.mState.mFirstSide = 0;
        gBattleView.mState.mUnk_1b = -1;
        id = GetNodeValue(data_020b6ee0.mSelectedNodeIndex, 14);
        memset(&data_020b6ee0.mUnk_fee, 0, 18);
        *(vu16 *) 0x04001050 = 0x2848;
        *(vu16 *) 0x04001052 = 0;
        *(vu16 *) 0x05000400 = 0;
        LoadImage(id + 0xf59, 2);
        LoadKindValues();
    }
}
