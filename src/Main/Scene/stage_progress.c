#include <nitro/os.h>
#include <Main/System/random_angle.h>
#include <Main/Scene/stage_data.h>
#include <Main/System/state_flow.h>
#include <Main/System/prompt_screen.h>
#include <Main/Scene/battle_screen.h>
#include <Main/Scene/stage_info_panel.h>
#include <Main/Scene/stage_progress.h>
#include <Main/Scene/stage_map_menu.h>
#include <Main/Graphics/sprite_slots.h>
#include <Main/Graphics/text_draw.h>
#include <Main/Sound/wave_player.h>
#include <Main/clib.h>

const u8 data_020500d4[3] = {1, 0, 2};

void UpdateProgress(void) {
    StageRec *rec;
    int stage = data_020b6ee0.mSelectedStageIndex;
    int idx;

    if (stage != -1) {
        rec = &data_020b1990[stage];
    } else {
        rec = NULL;
    }
    switch ((u32) data_020b6ee0.mProgressState) {
        case 0:
            break;
        case 1:
            if (GetState() == 10 && data_020b6ee0.mPanelState <= 1) {
                data_020b6ee0.mProgressState = 2;
                func_0201033c(1, 10);
                SetTextAndImage(0x4c, 2, 0x1e0);
            }
            break;
        case 2:
            if (GetState() == 10 && data_020b6ee0.mSelectedStageIndex == 0) {
                data_020b6ee0.mProgressState = 3;
                SetTextAndImage(0x54, 1, 0x1e0);
                rec->mUnk_22e = 1;
            }
            break;
        case 3:
            if (GetState() == 10 && data_020b6ee0.mUnk_a570 != 0) {
                data_020b6ee0.mProgressState = 4;
                SetTextAndImage(0x55, 1, 0x1e0);
            }
            break;
        case 4:
            if (GetState() == 10 && data_020b1990[0].mPolygonGroupIndex == data_020b1990[0].mInfo.mUnk_10) {
                data_020b6ee0.mProgressState = 5;
                data_020b6ee0.mUnk_a56c = 2;
                SetTextAndImage(0x4d, 1, 0x1e0);
            }
            break;
        case 5:
            if (GetState() == 10) {
                u8 v = data_020b6ee0.mPanelState;
                if (v == 6 || v == 7 || v == 8 || v == 3) {
                    data_020b6ee0.mUnk_a56c = 1;
                    data_020b6ee0.mProgressState = 6;
                }
            }
            break;
        case 6:
            if (GetState() == 10 && data_020b6ee0.mPanelState <= 1) {
                data_020b6ee0.mProgressState = 7;
                SetTextAndImage(0x56, 1, 0x1e0);
            }
            break;
        case 7:
            if (GetState() == 10 && data_020b6ee0.mPanelState <= 2 && data_020b1990[0].mInfo.mUnk_12 == 100 &&
                data_020b6ee0.mTextId == -1) {
                data_020b6ee0.mProgressState = 8;
                data_020b6ee0.mUnk_a56d = 2;
                data_020b6ee0.mSelectedStageIndex = 0;
                data_020b6ee0.mPanelState = 1;
                SetTextAndImage(0x4f, 1, 0x1e0);
            }
            break;
        case 8:
            if (GetState() == 13) {
                data_020b6ee0.mProgressState = 9;
                func_0201033c(0x27, 13);
            }
            break;
        case 9:
            if (GetState() == 4) {
                data_020b6ee0.mProgressState = 10;
                data_020b6ee0.mUnk_a56d = 1;
            }
            break;
        case 10:
            if (GetState() == 10) {
                data_020b6ee0.mProgressState = 11;
                data_020b6ee0.mUnk_a56e = 2;
                func_0201033c(2, 10);
                SetTextAndImage(0x4e, 1, 0x1e0);
            }
            break;
        case 11:
            if (GetState() == 6) {
                data_020b6ee0.mProgressState = 12;
                func_0201033c(4, 6);
            }
            break;
        case 12:
            if (GetState() == 10 && data_020b6ee0.mPanelState <= 1 && data_020b6ee0.mReachedNodeIndex == 1) {
                data_020b6ee0.mProgressState = 13;
                data_020b6ee0.mUnk_a56e = 0;
                SetTextAndImage(0x57, 1, 0x1e0);
            }
            break;
        case 13:
            idx = FindStage(0, 0x12);
            if (idx == -1) {
                OS_Terminate();
            }
            if (GetState() == 10 && data_020b1990[idx].mPolygonGroupIndex == data_020b1990[idx].mInfo.mUnk_10) {
                data_020b6ee0.mProgressState = 14;
                SetTextAndImage(0x56, 1, 0x1e0);
            }
            break;
        case 14:
            idx = FindStage(0, 0x12);
            if (idx == -1) {
                OS_Terminate();
            }
            if (GetState() == 10 && data_020b6ee0.mPanelState <= 2 && data_020b1990[idx].mInfo.mUnk_12 == 100 &&
                data_020b6ee0.mTextId == -1) {
                data_020b6ee0.mProgressState = 15;
                data_020b6ee0.mUnk_a56d = 2;
                data_020b6ee0.mSelectedStageIndex = idx;
                data_020b6ee0.mPanelState = 1;
                SetTextAndImage(0x58, 1, 0x1e0);
            }
            break;
        case 15:
            idx = FindStage(0, 0x12);
            if (idx == -1) {
                OS_Terminate();
            }
            if ((GetState() == 4 || GetState() == 15) && data_020b6ee0.mSelectedStageIndex == idx) {
                data_020b6ee0.mProgressState = 16;
                data_020b6ee0.mUnk_a56d = 1;
                data_020b6ee0.mUnk_a56e = 2;
                SetTextAndImage(0x4e, 1, 0x1e0);
            }
            break;
        case 16:
            if (GetState() == 10 && data_020b6ee0.mPanelState <= 1 && data_020b6ee0.mReachedNodeIndex == 2) {
                data_020b6ee0.mProgressState = 17;
                data_020b6ee0.mUnk_a56e = 0;
                data_020b6ee0.mUnk_a56f = 2;
                SetTextAndImage(0x50, 1, 0x1e0);
            }
            break;
        case 17:
            if (GetState() == 9) {
                data_020b6ee0.mProgressState = 99;
                data_020b6ee0.mUnk_a56c = 1;
                data_020b6ee0.mUnk_a56d = 1;
                data_020b6ee0.mUnk_a56e = 1;
                data_020b6ee0.mUnk_a56f = 1;
                func_0201033c(0x2d, 9);
                SetTextAndImage(-1, 0, 0);
            }
            break;
        case 98:
            data_020b6ee0.mUnk_a56c = 1;
            data_020b6ee0.mUnk_a56d = 1;
            data_020b6ee0.mUnk_a56e = 1;
            data_020b6ee0.mUnk_a56f = 1;
            data_020b6ee0.mProgressState = 99;
            SetTextAndImage(-1, 0, 0);
            break;
        case 99:
            break;
    }
}

// 1 when the node under the cursor (mSelectedNodeIndex) is the furthest reached one; 0 once mLastNodeDone is set.
int IsReachedNodeSelected(void) {
    if (data_020b6ee0.mSelectedNodeIndex == -1) {
        OS_Terminate();
    }
    if (data_020b6ee0.mSelectedNodeIndex == data_020b6ee0.mReachedNodeIndex && data_020b6ee0.mLastNodeDone == 0) {
        return 1;
    }
    return 0;
}

void UpdateNodeState(void) {
    switch (data_020b6ee0.mNodeState) {
        case 0:
            break;
        case 1:
            if (GetStateFlag() == 1) {
                if (IsReachedNodeSelected() == 1) {
                    int sel;
                    data_020b6ee0.mNodeState = 2;
                    sel = GetNodeValue(data_020b6ee0.mSelectedNodeIndex, 3);
                    SetupNodeStageInfos(data_020b6ee0.mSelectedNodeIndex, 0);
                    func_0201033c(sel, 0x13);
                } else {
                    int sel;
                    data_020b6ee0.mNodeState = 2;
                    sel = GetNodeValue(data_020b6ee0.mSelectedNodeIndex, 5);
                    SetupNodeStageInfos(data_020b6ee0.mSelectedNodeIndex, 1);
                    func_0201033c(sel, 0x13);
                }
            }
            break;
        case 2:
            if (GetStateFlag() == 1) {
                if (IsReachedNodeSelected() == 1) {
                    data_020b6ee0.mNodeState = 0;
                    if (func_02007758() == 0) {
                        func_02014bf0();
                        func_02014afc(data_020b6ee0.mSelectedNodeIndex, 3);
                        PushState(0x14);
                        PopState();
                    } else {
                        func_02014bf0();
                        func_02014afc(data_020b6ee0.mSelectedNodeIndex, 1);
                        PushState(0x14);
                        PopState();
                    }
                } else {
                    data_020b6ee0.mNodeState = 0;
                    func_02007758();
                    func_02014bf0();
                    func_02014afc(data_020b6ee0.mSelectedNodeIndex, 1);
                    PushState(0x14);
                    PopState();
                }
            }
            break;
    }
}

// Refills the StageInfo list (data_020b1524) with the three stage ids of the selected node (GetSelectedNodeStageId);
// node is only range-checked and mode is unused.
void SetupNodeStageInfos(u32 node, int mode) {
    int i;

    if (node >= MAP_NODE_COUNT) {
        OS_Terminate();
    }
    data_020b6ee0.mStageInfoCount = 0;
    for (i = 0; i < 3; i++) {
        AddStage(1, GetSelectedNodeStageId(data_020500d4[i]));
    }
}

void func_02014afc(int node, int flags) {
    int i;

    if (flags & 1) {
        for (i = 0; i < 3; i++) {
            StageRec *rec = GetSlotStage(i);
            if (rec != NULL) {
                if (data_020b6ee0.mUnk_a8f3[i] & 1) {
                    func_020150a0(rec, 1);
                }
                if (!(data_020b6ee0.mUnk_a8f3[i] & 2)) {
                    if (data_020b6ee0.mUnk_a8f3[i] & 4) {
                        AddPercent(rec, -10, 0, 0);
                    } else {
                        AddPercent(rec, -30, 0, 0);
                    }
                }
            }
        }
    }
    data_020b6ee0.mNodeStageCount = 0;
    if (flags & 2) {
        for (i = 0; i < 3; i++) {
            int sel = GetNodeValue(node, i + 15);
            if (sel != -1) {
                int found = FindStage(0, sel);
                if (found == -1) {
                    int v = AddStage(0, sel);
                    data_020b6ee0.mNodeStages[data_020b6ee0.mNodeStageCount++] = v;
                    if (data_020b6ee0.mSelectedStageIndex == -1) {
                        data_020b6ee0.mSelectedStageIndex = v;
                        data_020b6ee0.mUnk_a14a = 1;
                    }
                } else {
                    data_020b6ee0.mNodeStages[data_020b6ee0.mNodeStageCount++] = found;
                }
            }
        }
    }
}

void func_02014bf0(void) {
    int i;
    StageRec *rec;

    for (i = 0; i < data_020b6ee0.mStageCount; i++) {
        rec = &data_020b1990[i];
        func_02014ccc(&rec->mInfo);
        if (rec->mInSlot == 0) {
            AddPercent(rec, 10, 0, 0);
        }
    }
}

void LoadStageInfo(StageInfo *info) {
    int base = info->mUnk_13 * 27;

    info->mUnk_10 = func_02021e44(0xfd6, base + 2);
    info->mUnk_12 = func_02021e44(0xfd6, base + 3);
    info->mUnk_11 = func_02021e44(0xfd6, base + 4);
    info->mUnk_04 = func_02021e44(0xfd6, base + 5);
    if (info->mUnk_10 >= 6) {
        OS_Terminate();
    }
    info->mUnk_14 = 0;
    info->mUnk_00 = 10;
    info->mUnk_02 = 11;
    info->mUnk_1c[0] = func_02021e44(0xfd6, base + 0x17);
    info->mUnk_1c[1] = func_02021e44(0xfd6, base + 0x18);
    info->mUnk_1c[2] = func_02021e44(0xfd6, base + 0x19);
    info->mUnk_1c[3] = func_02021e44(0xfd6, base + 0x1a);
    info->mUnk_1c[4] = func_02021e44(0xfd6, base + 0x1b);
    func_02014ccc(info);
}

void func_02014ccc(StageInfo *info) {
    info->mUnk_1a = NextRandom() & 3;
}

void func_02014cdc(StageInfo *info) {
    u32 r = NextRandom() % 3;
    info->mUnk_1a = (info->mUnk_1a + 1 + r) & 3;
}

// Each of the five mUnk_1c values that is -1 gets an entry id from PickWeightedTableEntry; -2 becomes -1.
void PickTableEntries(StageInfo *info) {
    int i;

    for (i = 0; i < 5; i++) {
        if (info->mUnk_1c[i] == -1) {
            info->mUnk_1c[i] = PickWeightedTableEntry(info);
        } else if (info->mUnk_1c[i] == -2) {
            info->mUnk_1c[i] = -1;
        }
    }
}

int PickWeightedTableEntry(StageInfo *info) {
    int i;
    int ok = 0;
    int row;
    int off;
    u32 roll;

    if (info->mUnk_11 >= 1 && info->mUnk_11 <= 12) {
        ok = 1;
    }
    if (!ok) {
        OS_Terminate();
    }
    row = info->mUnk_11 + 9;
    roll = NextRandom() % 10000;
    i = 0;
    off = 0;
    do {
        if (func_02021e44(0x1d9, row + off) >= roll) {
            break;
        }
        i++;
        off += 20;
    } while (i < 100);
    if (i == 100) {
        OS_Terminate();
    }
    return func_02021e44(0x1d9, i * 20 + 2);
}

static void InitStage(int index, int stage);
static void InitStageInfo(int index, int stage);

// Clears both stage lists, adds stage ids 3, 4 and 5 to the StageInfo list and, only when a is 0, stage id 0x19 to
// the StageRec list.
void ResetStages(int a) {
    memset(data_020b1990, 0, 0x5550);
    memset(data_020b1524, 0, 0x72);
    data_020b6ee0.mStageCount = 0;
    data_020b6ee0.mStageInfoCount = data_020b6ee0.mStageCount;
    AddStage(1, 3);
    AddStage(1, 4);
    AddStage(1, 5);
    if (a == 0) {
        AddStage(0, 0x19);
    }
}

// Appends stage id to a list and returns its index: list 0 a StageRec in data_020b1990 (mStageCount), list 1 a
// StageInfo in data_020b1524 (mStageInfoCount, at most 3).
int AddStage(int list, int id) {
    int slot;

    if (id >= STAGE_MAX) {
        OS_Terminate();
    }
    switch (list) {
        case 0:
            if (data_020b6ee0.mStageCount >= STAGE_MAX) {
                OS_Terminate();
            }
            slot = data_020b6ee0.mStageCount++;
            InitStage(slot, id);
            break;
        case 1:
            if (data_020b6ee0.mStageInfoCount >= 3) {
                OS_Terminate();
            }
            slot = data_020b6ee0.mStageInfoCount++;
            InitStageInfo(slot, id);
            break;
        default:
            if (list != 0 && list != 1) {
                OS_Terminate();
            }
            break;
    }
    return slot;
}

static void InitStage(int index, int id) {
    StageRec *rec;

    if (index >= STAGE_MAX) {
        OS_Terminate();
    }
    if (index >= data_020b6ee0.mStageCount) {
        OS_Terminate();
    }
    rec = &data_020b1990[index];
    memset(rec, 0, sizeof(StageRec));
    rec->mPolygonGroupIndex = 99;
    rec->mInfo.mUnk_13 = id;
    LoadStageInfo(&rec->mInfo);
    PickTableEntries(&rec->mInfo);
    rec->mStoredOffsetX = rec->mUnk_1e8 = rec->mStoredOffsetZ = -1;
}

static void InitStageInfo(int index, int id) {
    StageInfo *info;

    if (index >= 3) {
        OS_Terminate();
    }
    if (index >= data_020b6ee0.mStageInfoCount) {
        OS_Terminate();
    }
    info = &data_020b1524[index];
    // The original clears 0x230 bytes, a StageRec's size, from this StageInfo.
    memset(info, 0, sizeof(StageRec));
    if (id != -1) {
        info->mUnk_13 = id;
        LoadStageInfo(info);
        PickTableEntries(info);
    } else {
        info->mUnk_04 = -9999;
    }
}

int FindStage(int list, int id) {
    int i;
    u8 n;
    int v;

    if ((u32) id >= STAGE_MAX) {
        OS_Terminate();
    }
    switch (list) {
        case 0:
            n = data_020b6ee0.mStageCount;
            for (i = 0; i < n; i++) {
                v = data_020b1990[i].mInfo.mUnk_13;
                if (v == id) {
                    break;
                }
            }
            if (i == n) {
                i = -1;
            }
            break;
        case 1:
            n = data_020b6ee0.mStageInfoCount;
            for (i = 0; i < n; i++) {
                v = data_020b1524[i].mUnk_13;
                if (v == id) {
                    break;
                }
            }
            if (i == n) {
                i = -1;
            }
            break;
        default:
            if (list != 0 && list != 1) {
                OS_Terminate();
            }
            break;
    }
    return i;
}

int func_02014f98(StageRec *rec, int delta) {
    StageInfo *info = &rec->mInfo;
    int low;
    int high;

    low = func_02021e44(0xfd6, info->mUnk_11 + (info->mUnk_13 * 27 + 4));
    high = func_02021e44(0xfd6, info->mUnk_11 + (info->mUnk_13 * 27 + 5));
    info->mUnk_04 += delta;
    if (delta > 0 && info->mUnk_04 > high) {
        info->mUnk_04 = high;
        return 1;
    }
    if (delta < 0 && info->mUnk_04 < low) {
        info->mUnk_04 = low;
        return 1;
    }
    return 0;
}

// Adds amount to the stage's percent (mInfo.mUnk_12), clamped to 0..100; returns 1 when it was clamped. When amount
// is positive, a set plays sound 0xe8 (func_0202f110) and b set, while a stage is selected, adds an ObjSlot
// (func_0200d78c) at the gauge position of the old value (PercentToX). Reaching 100 clears mUnk_22e.
int AddPercent(StageRec *rec, int amount, int a, int b) {
    StageInfo *info = &rec->mInfo;
    int value = info->mUnk_12;

    if (a != 0 && amount > 0) {
        func_0202f110(0xe8, 100, 0, 0x40, 0xffff, 0);
    }
    if (b != 0 && amount > 0 && data_020b6ee0.mSelectedStageIndex != -1) {
        func_0200d78c(0x1e, 0, PercentToX(value), 0x5e, 0, 0, 0x3c, 2);
    }
    value += amount;
    rec->mUnk_22e = value >= 100 ? 0 : rec->mUnk_22e;
    if (value > 100) {
        info->mUnk_12 = 100;
        return 1;
    }
    if (value < 0) {
        info->mUnk_12 = 0;
        return 1;
    }
    info->mUnk_12 = value;
    return 0;
}

int func_020150a0(StageRec *rec, int delta) {
    StageInfo *info = &rec->mInfo;

    info->mUnk_11 += delta;
    if (info->mUnk_11 > 12) {
        info->mUnk_11 = 12;
        return 1;
    }
    return 0;
}

int func_020150c0(StageInfo *info) {
    if (info->mUnk_04 >= func_02021e44(0xfd6, info->mUnk_11 + (info->mUnk_13 * 27 + 5))) {
        return 1;
    }
    return 0;
}

void ResetSlots(int a) {
    int i;

    for (i = 0; i < 3; i++) {
        data_020b6ee0.mSlotStages[i] = -1;
    }
    data_020b6ee0.mUnslottedVisible = 0;
    data_020b6ee0.mSelectedSlotIndex = 0;
}

// Puts stage index stage (-1 for none) into slot and clears mInSlot of the stage it replaces; returns that stage's
// index (-1 for none).
int SetSlotStage(int slot, int stage) {
    int old;
    StageRec *rec;

    if ((u32) slot >= 3) {
        OS_Terminate();
    }
    if (stage >= STAGE_MAX) {
        OS_Terminate();
    }
    old = data_020b6ee0.mSlotStages[slot];
    if (old != -1) {
        if (old >= STAGE_MAX) {
            OS_Terminate();
        }
        rec = &data_020b1990[old];
        rec->mInSlot = 0;
    }
    if (stage != -1) {
        if (stage >= data_020b6ee0.mStageCount) {
            OS_Terminate();
        }
        data_020b6ee0.mSlotStages[slot] = stage;
        rec = &data_020b1990[stage];
        rec->mInSlot = 1;
    } else {
        data_020b6ee0.mSlotStages[slot] = -1;
    }
    return old;
}

StageRec *GetSlotStage(int slot) {
    int stage;

    if ((u32) slot >= 3) {
        OS_Terminate();
    }
    stage = data_020b6ee0.mSlotStages[slot];
    if (stage >= STAGE_MAX) {
        OS_Terminate();
    }
    if (stage == -1) {
        return 0;
    }
    return &data_020b1990[stage];
}

int FindStageSlot(int stage) {
    int i;

    if (stage < 0) {
        OS_Terminate();
    }
    for (i = 0; i < 3; i++) {
        if (stage == data_020b6ee0.mSlotStages[i]) {
            return i;
        }
    }
    return -1;
}

// Fills each empty slot with a stage that is in no slot, if there is one. Each SetSlotStage call frees the stage it
// replaces, so the slot ends with the last such stage in index order.
void FillEmptySlots(void) {
    int i;
    int j;
    u8 stage;

    for (i = 0; i < 3; i++) {
        stage = data_020500d4[i];
        if (GetSlotStage(stage) == 0) {
            for (j = 0; j < data_020b6ee0.mStageCount; j++) {
                if (data_020b1990[j].mInSlot == 0) {
                    SetSlotStage(stage, j);
                }
            }
        }
    }
}

int FindFirstUnslottedStage(void) {
    int i;

    for (i = 0; i < data_020b6ee0.mStageCount; i++) {
        if (data_020b1990[i].mInSlot == 0) {
            return i;
        }
    }
    return -1;
}

int FindNextUnslottedStage(int stage) {
    int i;

    if (stage < 0) {
        OS_Terminate();
    }
    for (i = stage + 1; i < data_020b6ee0.mStageCount; i++) {
        if (data_020b1990[i].mInSlot == 0) {
            return i;
        }
    }
    return FindFirstUnslottedStage();
}

int FindLastUnslottedStage(void) {
    int i;

    for (i = data_020b6ee0.mStageCount - 1; i >= 0; i--) {
        if (data_020b1990[i].mInSlot == 0) {
            return i;
        }
    }
    return -1;
}

int FindPrevUnslottedStage(int stage) {
    int i;

    if (stage < 0) {
        OS_Terminate();
    }
    for (i = stage - 1; i >= 0; i--) {
        if (data_020b1990[i].mInSlot == 0) {
            return i;
        }
    }
    return FindLastUnslottedStage();
}
