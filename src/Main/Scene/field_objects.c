#include <nitro/os.h>
#include <nitro/gx.h>
#include <stdlib.h>
#include <Main/System/random_angle.h>
#include <Main/System/game_math.h>
#include <Main/System/system_init.h>
#include <Main/Scene/stage_info_panel.h>
#include <Main/Unknown/game_util.h>
#include <Main/clib.h>
#include <Main/Graphics/sprite_slots.h>
#include <Main/System/prompt_screen.h>
#include <Main/System/sound_control.h>
#include <Main/Scene/field_objects.h>
#include <stdio.h>
#include <Main/Scene/mode_select.h>
#include <Main/Graphics/text_draw.h>
#include <Main/Graphics/model.h>
#include <Main/Sound/wave_player.h>
#include <Main/Map/map_entries.h>
#include <Main/Graphics/script_data.h>
#include <Main/System/state_flow.h>

static const u16 data_0204e138[6] = {0xd67, 0xd6f, 0xd77, 0xd7f, 0xd87, 0xd8f};
static const u32 data_0204e144[5] = {0, 0xd97, 0xd9b, 0xd9d, 0xd9e};

FieldObjects gFieldObjects;

s32 GetFieldTableValue(int a, int b) {
    return GetFileValue(0xda0, b + a * 7);
}

s32 GetFieldPatternValue(int a, int b) {
    return GetFileValue(0xda1, b + a * 252);
}

s32 GetFieldPatternListValue(int a, int b) {
    return GetFileValue(0xd9f, b + a * 10);
}

void GetGridCell(s32 x, s32 z, s32 *outGroup, s32 *outSub, s32 *outRow) {
    s32 cellH = (gFieldObjects.mInfo.mGridEndZ - gFieldObjects.mInfo.mGridStartZ) / 8;
    s32 cellW = (gFieldObjects.mInfo.mFirstGroupEndX - gFieldObjects.mInfo.mGridStartX) / 8;
    *outSub = (x - gFieldObjects.mInfo.mGridStartX) / cellW;
    *outRow = (z - gFieldObjects.mInfo.mGridStartZ) / cellH;
    *outGroup = *outSub / 8;
    *outSub = *outSub % 8;
}

void DrawFieldObjects(void) {
    int g, i, j;
    FieldObject *p;
    for (g = 0; g < 4; g++) {
        for (i = 0; i < 8; i++) {
            for (j = 0; j < 8; j++) {
                p = &gFieldObjects.mGrid[g][i][j];
                switch (p->mKind) {
                    case 0:
                    case 2:
                        break;
                    case 1:
                        if (p->mType != 0) {
                            DrawModelFile(data_0204e144[p->mKind], &p->mPosX, 0);
                        } else {
                            UpdateAndDrawModel(p->mModel, &p->mPosX, 0);
                        }
                        break;
                    case 3:
                        UpdateAndDrawModel(p->mModel, &p->mPosX, 0);
                        break;
                    case 4:
                        if (p->mType != 0) {
                            DrawModelFile(data_0204e144[p->mKind], &p->mPosX, 0);
                        }
                        break;
                }
            }
        }
    }
    for (i = 0; i < 128; i++) {
        p = &gFieldObjects.mList[i];
        if (p->mKind != 0) {
            UpdateAndDrawModel(p->mModel, &p->mPosX, 0);
        }
    }
}

void EmptyGridGroup(u8 group) {
    int i, j;
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            gFieldObjects.mGrid[group][i][j].mKind = 0;
            gFieldObjects.mGrid[group][i][j].mListIndex = -1;
        }
    }
}

void FillGridGroup(u8 group, u8 pattern) {
    int a, b;
    s32 width = gFieldObjects.mInfo.mFirstGroupEndX - gFieldObjects.mInfo.mGridStartX;
    s32 height = gFieldObjects.mInfo.mGridEndZ - gFieldObjects.mInfo.mGridStartZ;
    s32 cellW = width / 8;
    s32 cellH = height / 8;
    int idx;
    FieldObject *obj;
    for (a = 0; a < 8; a++) {
        obj = gFieldObjects.mGrid[group][a];
        for (b = 0; b < 8; b++) {
            obj->mType = 1;
            obj->mPosX = gFieldObjects.mInfo.mGridStartX + group * width + a * cellW + cellW / 2;
            obj->mPosY = gFieldObjects.mInfo.mGridY;
            obj->mPosZ = gFieldObjects.mInfo.mGridStartZ + b * cellH + cellH / 2;
            obj->mMoveDirZ = -1;
            obj->mListIndex = -1;
            obj->mKind = gFieldObjects.mPatterns[pattern][a][b];
            switch (obj->mKind) {
                case 2:
                    InitModel(obj->mModel, 0, 0, 0, data_0201abbc[9]);
                    obj->mUnk_38 = -1;
                    obj->mUnk_34 = -1;
                    obj->mUnk_30 = -1;
                    break;
                case 3:
                    InitModel(obj->mModel, 0, 0, 0, data_0201abbc[10]);
                    obj->mUnk_38 = -1;
                    obj->mUnk_34 = -1;
                    obj->mUnk_30 = -1;
                    break;
            }
            if (obj->mKind == 2) {
                for (idx = 0; idx < 128; idx++) {
                    if (gFieldObjects.mList[idx].mKind == 0) {
                        obj->mListIndex = idx;
                        gFieldObjects.mList[idx] = *obj;
                        break;
                    }
                }
            }
            obj++;
        }
    }
}

void MoveListObjects(void) {
    int i = 0;
    FieldObject *o = gFieldObjects.mList;
    s32 half = ((gFieldObjects.mInfo.mGridEndZ - gFieldObjects.mInfo.mGridStartZ) / 8 - 1) / 2;
    for (; i < 128; i++, o++) {
        if (o->mKind != 2) {
            continue;
        }
        if (o->mType != 0) {
            s32 col, sub, row;
            GetGridCell(o->mPosX, o->mPosZ - half, &col, &sub, &row);
            if (gFieldObjects.mGrid[col][sub][row].mKind == 2) {
                gFieldObjects.mGrid[col][sub][row].mKind = 0;
                gFieldObjects.mGrid[col][sub][row].mListIndex = -1;
            }
            GetGridCell(o->mPosX, o->mPosZ + half, &col, &sub, &row);
            if (gFieldObjects.mGrid[col][sub][row].mKind == 2) {
                gFieldObjects.mGrid[col][sub][row].mKind = 0;
                gFieldObjects.mGrid[col][sub][row].mListIndex = -1;
            }
            o->mPosZ += o->mMoveDirZ * (50 << 10) / 2;
            if (o->mPosZ < gMapState.mCamera.mUnk_9c.mUnk_228.z - 0x177000) {
                o->mPosZ = gMapState.mCamera.mUnk_9c.mUnk_228.z - 0x177000;
                o->mMoveDirZ = -o->mMoveDirZ;
            } else if (o->mPosZ > gMapState.mCamera.mUnk_9c.mUnk_228.z + (150 << 12)) {
                o->mPosZ = gMapState.mCamera.mUnk_9c.mUnk_228.z + (150 << 12);
                o->mMoveDirZ = -o->mMoveDirZ;
            }
            GetGridCell(o->mPosX, o->mPosZ - half, &col, &sub, &row);
            if (gFieldObjects.mGrid[col][sub][row].mKind == 0) {
                gFieldObjects.mGrid[col][sub][row].mKind = 2;
                gFieldObjects.mGrid[col][sub][row].mListIndex = i;
            }
            GetGridCell(o->mPosX, o->mPosZ + half, &col, &sub, &row);
            if (gFieldObjects.mGrid[col][sub][row].mKind == 0) {
                gFieldObjects.mGrid[col][sub][row].mKind = 2;
                gFieldObjects.mGrid[col][sub][row].mListIndex = i;
            }
        } else {
            s32 col, sub, row;
            GetGridCell(o->mPosX, o->mPosZ - half, &col, &sub, &row);
            if (gFieldObjects.mGrid[col][sub][row].mKind == 2) {
                gFieldObjects.mGrid[col][sub][row].mKind = 0;
            }
            GetGridCell(o->mPosX, o->mPosZ + half, &col, &sub, &row);
            if (gFieldObjects.mGrid[col][sub][row].mKind == 2) {
                gFieldObjects.mGrid[col][sub][row].mKind = 0;
            }
        }
    }
}

void ShiftGridGroups(void) {
    int g, i, j, k;
    for (g = 0; g < 3; g++) {
        for (i = 0; i < 8; i++) {
            for (j = 0; j < 8; j++) {
                s16 idx = gFieldObjects.mGrid[g][i][j].mListIndex;
                if (idx >= 0) {
                    gFieldObjects.mList[idx].mKind = 0;
                }
            }
        }
    }
    for (i = 0; i < 128; i++) {
        if (gFieldObjects.mList[i].mKind != 0) {
            gFieldObjects.mList[i].mPosX -= gFieldObjects.mGridShiftDistance << 9;
        }
    }
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            gFieldObjects.mGrid[0][i][j] = gFieldObjects.mGrid[3][i][j];
            gFieldObjects.mGrid[0][i][j].mPosX -= gFieldObjects.mGridShiftDistance << 9;
        }
    }
    for (k = 1; k < 4; k++) {
        if (gFieldObjects.mEndTimer != 0) {
            {
                u8 r = RandomRange(0, gFieldObjects.mPatternListCount - 2);
                FillGridGroup(k, gFieldObjects.mPatternList[r]);
            }
        } else {
            EmptyGridGroup(k);
        }
    }
}

// Always 0 in this build: UpdateField's touch and key input for the x speed never runs, and the drive steps (its
// other branch) always run instead.
int IsXInputEnabled(void) {
    return 0;
}

int IsYInputEnabled(void) {
    if (gFieldObjects.mDrivePlusX != 0 || gFieldObjects.mDriveTimer == 0) {
        return 1;
    }
    return 0;
}

// Unlike StartDrivePlusX it does not wait for a running drive: it restarts over one of either direction and zeroes
// the x speed.
void StartDriveMinusX(s32 distance, u32 ms) {
    if (gFieldObjects.mBelowMinX == 0 && gFieldObjects.mEndTimer != 0) {
        gFieldObjects.mDrivePlusX = 0;
        gFieldObjects.mDriveStartOffset = gFieldObjects.mStageOffset;
        gFieldObjects.mDriveDistance = distance;
        gFieldObjects.mDriveTimer = ms * 60 / 1000;
        gFieldObjects.mOffsetSpeedX = 0;
    }
}

void StartDrivePlusX(s32 distance, u32 ms) {
    if (gFieldObjects.mDriveTimer == 0 && gFieldObjects.mAboveMaxX == 0 && gFieldObjects.mEndTimer != 0) {
        gFieldObjects.mDrivePlusX = 1;
        gFieldObjects.mDriveStartOffset = gFieldObjects.mStageOffset;
        gFieldObjects.mDriveDistance = distance;
        gFieldObjects.mDriveTimer = ms * 60 / 1000;
    }
}

void ApplyFieldObject(FieldObject *obj) {
    char text[32];
    s32 pos[2];
    switch (obj->mKind) {
        case 1:
        case 2:
            if (obj->mType == 0) {
                break;
            }
            obj->mType = 0;
            StartDriveMinusX(250 << 10, 250 << 1);
            PlayWaveKeepAlive(48, 100, 0, 64, 0xffff, 0);
            if (obj->mKind == 2) {
                obj = &gFieldObjects.mList[obj->mListIndex];
            }
            InitModel(obj->mModel, 0, 0, 0, data_0201abbc[11]);
            obj->mUnk_38 = -1;
            obj->mUnk_34 = -1;
            obj->mUnk_30 = -1;
            gFieldObjects.mNoScoreTimer = gFieldObjects.mNoScoreFrameCount;
            return;
        case 3:
            StartDrivePlusX(25 << 12, 0x15e);
            if (obj->mType == 0) {
                break;
            }
            obj->mType = 0;
            PlayWaveKeepAlive(1, 100, 0, 64, 0xffff, 0);
            return;
        case 4:
            if (obj->mType == 0) {
                break;
            }
            ProjectToScreen(&obj->mPosX, pos);
            obj->mType = 0;
            AddCappedAt5999(&gFieldObjects.mEndTimer, 60000 * gFieldObjects.mAddSeconds / 1000);
            sprintf(text, "+%d'", gFieldObjects.mAddSeconds);
            AddTextObjSlot(text, 2, data_0201a61c, 30, pos[0], pos[1], 0, -128, 100, 1, 0);
            PlayWaveKeepAlive(117, 100, 0, 64, 0xffff, 0);
            break;
    }
}

void ApplyObjectsAtStage(void) {
    s32 col, sub, row;
    s32 quarter = ((gFieldObjects.mInfo.mGridEndZ - gFieldObjects.mInfo.mGridStartZ) / 8) / 4;
    GetGridCell(gFieldObjects.mStagePos.x, gFieldObjects.mStagePos.z - quarter, &col, &sub, &row);
    if (gFieldObjects.mGrid[col][sub][row].mKind != 0) {
        ApplyFieldObject(&gFieldObjects.mGrid[col][sub][row]);
    }
    GetGridCell(gFieldObjects.mStagePos.x, gFieldObjects.mStagePos.z + quarter, &col, &sub, &row);
    if (gFieldObjects.mGrid[col][sub][row].mKind != 0) {
        ApplyFieldObject(&gFieldObjects.mGrid[col][sub][row]);
    }
}

void SetupFieldCameraAndInfo(void) {
    void *entry;
    gMapState.mCamera.mUnk_2dc.mUnk_44 = 0x186a000;
    gMapState.mCamera.mUnk_2dc.mUnk_54 = 0x578;
    func_02033e38(1, 1, 1);
    func_02033e38(8, 0, 1);
    gMapState.mCamera.mUnk_2dc.mUnk_30 = 1;
    gMapState.mCamera.mUnk_9c.mUnk_204.x = gMapState.mCamera.mUnk_9c.mUnk_228.x;
    gFieldObjects.mScrollOrigin = gMapState.mCamera.mUnk_9c.mUnk_228;
    entry = GetMapEntry(2, 0);
    if (entry == NULL) {
        OS_Terminate();
    }
    gFieldObjects.mInfo.mUnk_00.x = GetMapEntryX(entry);
    gFieldObjects.mInfo.mUnk_00.y = GetMapEntryY(entry);
    gFieldObjects.mInfo.mUnk_00.z = GetMapEntryZ(entry);
    gFieldObjects.mInfo.mUnk_0c.x = gMapState.mCamera.mUnk_9c.mUnk_204.x - gMapState.mCamera.mUnk_9c.mUnk_228.x;
    gFieldObjects.mInfo.mUnk_0c.y = gMapState.mCamera.mUnk_9c.mUnk_204.y - gMapState.mCamera.mUnk_9c.mUnk_228.y;
    gFieldObjects.mInfo.mUnk_0c.z = gMapState.mCamera.mUnk_9c.mUnk_204.z - gMapState.mCamera.mUnk_9c.mUnk_228.z;
    gFieldObjects.mInfo.mScrolledDistance = 0;
    entry = GetMapEntry(3, 0);
    if (entry == NULL) {
        OS_Terminate();
    }
    gFieldObjects.mInfo.mGridStartX = GetMapEntryX(entry);
    gFieldObjects.mInfo.mGridY = GetMapEntryY(entry);
    gFieldObjects.mInfo.mGridStartZ = GetMapEntryZ(entry);
    entry = GetMapEntry(4, 0);
    if (entry == NULL) {
        OS_Terminate();
    }
    gFieldObjects.mInfo.mFirstGroupEndX = GetMapEntryX(entry);
    gFieldObjects.mInfo.mUnk_2c = GetMapEntryY(entry);
    gFieldObjects.mInfo.mGridEndZ = GetMapEntryZ(entry);
    gMapState.mCamera.mUnk_2dc.mUnk_38 = 0;
    gMapState.mCamera.mUnk_2dc.mUnk_40 = 0;
    gMapState.mCamera.mUnk_2dc.mUnk_3c = 0;
    gMapState.mCamera.mUnk_5b_1 = 0;
    func_02033df4(0x12);
    gMapState.mCamera.mUnk_2dc.mUnk_14 = gMapState.mCamera.mUnk_2dc.mUnk_18;
    gMapState.mCamera.mUnk_2dc.mUnk_10 = 0;
}

void UpdateFieldScrollAndScore(void) {
    s32 step = (((((gFieldObjects.mScrollSpeed >> 8) * 100000) / 3600) << 9) / 60);
    s32 remain;
    gFieldObjects.mInfo.mScrolledDistance += step;
    remain = gFieldObjects.mInfo.mScrolledDistance;
    while (remain > 0) {
        gMapState.mCamera.mUnk_9c.mUnk_228.x = gFieldObjects.mScrollOrigin.x + remain;
        remain = gFieldObjects.mInfo.mScrolledDistance - (gFieldObjects.mGridShiftDistance << 9);
        if (remain >= 0) {
            gFieldObjects.mInfo.mScrolledDistance = remain;
            ShiftGridGroups();
        }
    }
    gMapState.mCamera.mUnk_9c.mUnk_204.x = gMapState.mCamera.mUnk_9c.mUnk_228.x + gFieldObjects.mInfo.mUnk_0c.x;
    gMapState.mCamera.mUnk_9c.mUnk_204.y = gMapState.mCamera.mUnk_9c.mUnk_228.y + gFieldObjects.mInfo.mUnk_0c.y;
    gMapState.mCamera.mUnk_9c.mUnk_204.z = gMapState.mCamera.mUnk_9c.mUnk_228.z + gFieldObjects.mInfo.mUnk_0c.z;
    if (gFieldObjects.mNoScoreTimer != 0) {
        gFieldObjects.mNoScoreTimer--;
    }
    if (gFieldObjects.mEndTimer != 0 && gFieldObjects.mNoScoreTimer == 0) {
        s32 cellW = (gFieldObjects.mInfo.mFirstGroupEndX - gFieldObjects.mInfo.mGridStartX) / 8;
        AddCappedAt999Shifted(&gFieldObjects.mScore,
                              MapRange(gFieldObjects.mScrollSpeed >> 8, gFieldObjects.mMinScrollSpeed >> 8,
                                       gFieldObjects.mMaxScrollSpeed >> 8, 8, 256) *
                                  step / cellW,
                              8);
    }
    UpdateCamera();
}

void SetupField(void) {
    s32 stageIndex = data_020b6ee0.mSelectedStageIndex;
    u16 v[5];
    s32 i, j, k;
    s8 value;
    BOOL ok;
    if (stageIndex < 0) {
        stageIndex = 0;
    }
    gFieldObjects.mStage = gStages[stageIndex].mInfo;
    data_020b10fc.mTableRow = gFieldObjects.mStage.mGrade - 1;
    ok = 0;
    if (data_020b10fc.mTableRow >= 0 && data_020b10fc.mTableRow < 12) {
        ok = 1;
    }
    if (ok == 0) {
        OS_Terminate();
    }
    func_0200c5d8(0, 0, 0xdb3 + gFieldObjects.mStage.mStageId, 0x100, 16, 1, 0x82);
    func_02024208();
    RequestTrack(2, 0x4c);
    func_0200dc08(data_0204e138[gFieldObjects.mStage.mKind], &v[4], &v[3], &v[2], &v[1], &v[0], NULL);
    SetupMap(v[4], v[2], v[3], 0, v[1], v[0]);
    UpdateSystemAndWait();
    SetupFieldCameraAndInfo();
    G3X_SetClearColor(0x7fff, 8, 0x7fff, 0, 0);
    {
        s64 dx = (gFieldObjects.mInfo.mUnk_00.x - gFieldObjects.mScrollOrigin.x) / 512;
        s64 dz = (gFieldObjects.mInfo.mUnk_00.z - gFieldObjects.mScrollOrigin.z) / 512;
        gFieldObjects.mGridShiftDistance = SqrtU64(dx * dx + dz * dz);
    }
    InitModel(&gFieldObjects.mStageModel, 0, 0, 0, data_0201ab04[gFieldObjects.mStage.mStageId]);
    gFieldObjects.mUnk_1e8 = 3 << 14;
    gFieldObjects.mStagePos = gMapState.mCamera.mUnk_9c.mUnk_228;
    gFieldObjects.mUnk_1f3 = (gFieldObjects.mUnk_1f3 & ~7) | 2;
    gFieldObjects.mUnk_1d8 = 2 << 18;
    gFieldObjects.mUnk_144 |= 1;
    gFieldObjects.mStageOffset.mX = -0x106800;
    gFieldObjects.mStageOffset.mY = -0x70800;
    gFieldObjects.mOffsetSpeedX = 0;
    gFieldObjects.mOffsetSpeedY = 0;
    gFieldObjects.mStagePos.x += gFieldObjects.mStageOffset.mX;
    gFieldObjects.mStagePos.z += gFieldObjects.mStageOffset.mY;
    gFieldObjects.mDriveDistance = 0;
    gFieldObjects.mDriveTimer = 0;
    gFieldObjects.mBelowMinX = 0;
    gFieldObjects.mAboveMaxX = 0;
    gFieldObjects.mMinScrollSpeed = GetFieldTableValue(data_020b10fc.mTableRow, 2) << 8;
    gFieldObjects.mMaxScrollSpeed = GetFieldTableValue(data_020b10fc.mTableRow, 3) << 8;
    gFieldObjects.mPatternListColumn = GetFieldTableValue(data_020b10fc.mTableRow, 4) - 1;
    gFieldObjects.mEndFrameCount = GetFieldTableValue(data_020b10fc.mTableRow, 5) * 60000 / 1000;
    gFieldObjects.mScoreThreshold = GetFieldTableValue(data_020b10fc.mTableRow, 6);
    gFieldObjects.mAddSeconds = GetFieldTableValue(data_020b10fc.mTableRow, 7);
    gFieldObjects.mNoScoreFrameCount = GetFieldTableValue(data_020b10fc.mTableRow, 8) * 60 / 1000;
    memset(gFieldObjects.mPatterns, 0, 0x1000);
    for (i = 0; i < 42; i++) {
        for (j = 0; j < 6; j++) {
            for (k = 0; k < 8; k++) {
                gFieldObjects.mPatterns[i][k][j + 1] = GetFieldPatternValue(k, j + (i * 6 + 2));
            }
        }
    }
    memset(gFieldObjects.mPatternList, 0, 0x40);
    if (!(gFieldObjects.mPatternListColumn < 10)) {
        OS_Terminate();
    }
    gFieldObjects.mPatternListCount = 0;
    do {
        value = GetFieldPatternListValue(gFieldObjects.mPatternListCount, gFieldObjects.mPatternListColumn + 2);
        gFieldObjects.mPatternList[gFieldObjects.mPatternListCount] = value - 1;
        gFieldObjects.mPatternListCount++;
    } while (value >= 0);
    for (i = 0; i < 4; i++) {
        EmptyGridGroup(i);
    }
    for (i = 0; i < 128; i++) {
        gFieldObjects.mList[i].mKind = 0;
    }
    for (i = 2; i < 4; i++) {
        FillGridGroup(i, gFieldObjects.mPatternList[RandomRange(0, gFieldObjects.mPatternListCount - 2)]);
    }
    gFieldObjects.mUnk_000[0] = 0;
    gFieldObjects.mEndTimer = gFieldObjects.mEndFrameCount;
    gFieldObjects.mScore = 0;
    gFieldObjects.mNoScoreTimer = 0;
    gFieldObjects.mScrollSpeed = gFieldObjects.mMinScrollSpeed;
    StartModeSlide(1, 0);
}

void DrawField(void) {
    func_0201168c(data_020b6ee0.mSelectedStageIndex, gFieldObjects.mScore >> 8, gModeSelectState.mScoreThreshold,
                  gFieldObjects.mEndTimer, 2);
    func_02033614(gMapState.mCamera.mUnk_9c.mUnk_228.x, gMapState.mCamera.mUnk_9c.mUnk_228.z);
    UpdateAndDrawModel(&gFieldObjects.mStageModel, &gFieldObjects.mStagePos, 0);
    DrawFieldObjects();
}

void UpdateField(void) {
    s32 pos[2];
    u8 slideState;
    func_020059cc(gFieldObjects.mScore >> 8);
    slideState = GetModeSlideState();
    if (slideState != MODE_SLIDE_STATE_NONE) {
        int busy;
        UpdateCamera();
        DrawField();
        busy = UpdateModeSlide(1);
        if (slideState == MODE_SLIDE_STATE_END && busy == 0) {
            StartResultScreen(GetScoreThresholdMet(), gFieldObjects.mScore >> 8);
        }
        return;
    }
    UpdateFieldScrollAndScore();
    gFieldObjects.mOffsetSpeedX =
        (gFieldObjects.mOffsetSpeedX < 0x2ee000 ? gFieldObjects.mOffsetSpeedX : 0x2ee000) < -0x2ee000
            ? -0x2ee000
            : (gFieldObjects.mOffsetSpeedX < 0x2ee000 ? gFieldObjects.mOffsetSpeedX : 0x2ee000);
    gFieldObjects.mOffsetSpeedY =
        (gFieldObjects.mOffsetSpeedY < 0x2ee000 ? gFieldObjects.mOffsetSpeedY : 0x2ee000) < -0x2ee000
            ? -0x2ee000
            : (gFieldObjects.mOffsetSpeedY < 0x2ee000 ? gFieldObjects.mOffsetSpeedY : 0x2ee000);
    gFieldObjects.mStageOffset.mX += gFieldObjects.mOffsetSpeedX / 60;
    gFieldObjects.mStageOffset.mY += gFieldObjects.mOffsetSpeedY / 60;
    if (gFieldObjects.mDriveTimer == 0 && gFieldObjects.mEndTimer != 0) {
        if (gFieldObjects.mStageOffset.mX < -0x106800 ||
            (gFieldObjects.mStageOffset.mX > (50 << 10) && gFieldObjects.mAboveMaxX == 0)) {
            gFieldObjects.mOffsetSpeedX = 0;
        }
        if (gFieldObjects.mBelowMinX != 0) {
            if (gFieldObjects.mStageOffset.mX < -0x106800) {
                gFieldObjects.mStageOffset.mX += 25 << 10;
            } else {
                gFieldObjects.mBelowMinX = 0;
            }
            if (gFieldObjects.mStageOffset.mX > (50 << 10)) {
                gFieldObjects.mStageOffset.mX = 50 << 10;
            }
        } else if (gFieldObjects.mAboveMaxX != 0) {
            if (gFieldObjects.mStageOffset.mX < (50 << 10)) {
                gFieldObjects.mAboveMaxX = 0;
            }
        } else {
            gFieldObjects.mStageOffset.mX =
                (gFieldObjects.mStageOffset.mX < (50 << 10) ? gFieldObjects.mStageOffset.mX : 50 << 10) < -0x106800
                    ? -0x106800
                    : (gFieldObjects.mStageOffset.mX < (50 << 10) ? gFieldObjects.mStageOffset.mX : 50 << 10);
        }
    }
    if (gFieldObjects.mStageOffset.mY < -0x177000 || gFieldObjects.mStageOffset.mY > (150 << 12)) {
        gFieldObjects.mOffsetSpeedY = 0;
    }
    gFieldObjects.mStageOffset.mY =
        (gFieldObjects.mStageOffset.mY < (150 << 12) ? gFieldObjects.mStageOffset.mY : 150 << 12) < -0x177000
            ? -0x177000
            : (gFieldObjects.mStageOffset.mY < (150 << 12) ? gFieldObjects.mStageOffset.mY : 150 << 12);
    ProjectToScreen(&gFieldObjects.mStagePos, pos);
    if (gFieldObjects.mEndTimer != 0) {
        if (IsYInputEnabled()) {
            if (gSystemState.mTouchDown) {
                s32 diff = gSystemState.mTouchY - pos[1];
                if (abs(diff) > 5) {
                    gFieldObjects.mOffsetSpeedY += (diff != 0 ? (diff >> 31) | 1 : 0) * (50 << 12);
                } else {
                    gFieldObjects.mOffsetSpeedY >>= 1;
                }
            } else if (gSystemState.mKeysDown & 0x40) {
                gFieldObjects.mOffsetSpeedY -= 50 << 12;
            } else if (gSystemState.mKeysDown & 0x80) {
                gFieldObjects.mOffsetSpeedY += 50 << 12;
            } else {
                MoveTowardZero(&gFieldObjects.mOffsetSpeedY, 50 << 12);
            }
        }
        if (IsXInputEnabled()) {
            if (gSystemState.mTouchDown) {
                s32 diff = gSystemState.mTouchX - pos[0];
                if (abs(diff) > 5) {
                    gFieldObjects.mOffsetSpeedX += (diff != 0 ? (diff >> 31) | 1 : 0) * (25 << 12);
                } else {
                    gFieldObjects.mOffsetSpeedX >>= 1;
                }
            } else if (gSystemState.mKeysDown & 0x20) {
                gFieldObjects.mOffsetSpeedX -= 25 << 12;
            } else if (gSystemState.mKeysDown & 0x10) {
                gFieldObjects.mOffsetSpeedX += 25 << 12;
            } else {
                MoveTowardZero(&gFieldObjects.mOffsetSpeedX, 25 << 12);
            }
        } else {
            switch (gFieldObjects.mDrivePlusX) {
                case 0:
                    if (gFieldObjects.mDriveTimer != 0) {
                        MoveTowardZero(&gFieldObjects.mOffsetSpeedY, 50 << 12);
                        gFieldObjects.mDriveTimer--;
                        if (gFieldObjects.mStageOffset.mX >
                            gFieldObjects.mDriveStartOffset.mX - gFieldObjects.mDriveDistance) {
                            gFieldObjects.mOffsetSpeedX -= 50 << 12;
                        } else {
                            MoveTowardZero(&gFieldObjects.mOffsetSpeedX, 25 << 12);
                        }
                    } else {
                        gFieldObjects.mOffsetSpeedX = 0;
                        gFieldObjects.mDriveDistance = 0;
                    }
                    if (gFieldObjects.mStageOffset.mX < -0x106800) {
                        gFieldObjects.mBelowMinX = 1;
                    }
                    break;
                case 1:
                    if (gFieldObjects.mDriveTimer != 0) {
                        gFieldObjects.mDriveTimer--;
                        if (gFieldObjects.mStageOffset.mX <
                            gFieldObjects.mDriveStartOffset.mX + gFieldObjects.mDriveDistance) {
                            gFieldObjects.mOffsetSpeedX += 50 << 12;
                        } else {
                            MoveTowardZero(&gFieldObjects.mOffsetSpeedX, 25 << 12);
                        }
                    } else if (gFieldObjects.mStageOffset.mX > gFieldObjects.mDriveStartOffset.mX) {
                        gFieldObjects.mOffsetSpeedX -= 25 << 12;
                    } else {
                        gFieldObjects.mOffsetSpeedX = 0;
                        gFieldObjects.mDriveDistance = 0;
                    }
                    if (gFieldObjects.mStageOffset.mX > (50 << 10)) {
                        gFieldObjects.mAboveMaxX = 1;
                    }
                    break;
            }
        }
    }
    gFieldObjects.mStagePos = gMapState.mCamera.mUnk_9c.mUnk_228;
    gFieldObjects.mStagePos.x += gFieldObjects.mStageOffset.mX;
    gFieldObjects.mStagePos.z += gFieldObjects.mStageOffset.mY;
    EaseToward(&gFieldObjects.mScrollSpeed,
               MapRange(gFieldObjects.mStageOffset.mX / (50 << 10), -31, 1, gFieldObjects.mMinScrollSpeed >> 8,
                        gFieldObjects.mMaxScrollSpeed >> 8)
                   << 8,
               2);
    MoveListObjects();
    ApplyObjectsAtStage();
    if (gFieldObjects.mEndTimer != 0) {
        gFieldObjects.mEndTimer--;
        if (gFieldObjects.mEndTimer == 0) {
            gFieldObjects.mDriveTimer = 0;
        }
    } else {
        gFieldObjects.mOffsetSpeedX -= 25 << 12;
        if (gFieldObjects.mScrollSpeed <= 0) {
            int flag = (gFieldObjects.mScore >> 8) >= gFieldObjects.mScoreThreshold;
            gFieldObjects.mScrollSpeed = 0;
            StartModeSlide(0, flag);
        }
    }
    DrawField();
}

void LeaveField(void) {
    func_020102f8();
}

int GetFieldScoreThreshold(u8 tableRow) {
    return GetFieldTableValue(tableRow, 6);
}
