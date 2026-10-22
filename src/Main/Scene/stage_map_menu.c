#include <nitro/os.h>
#include <nitro/fs.h>
#include <nitro/gx.h>
#include <nitro/mi.h>
#include <nitro/pm.h>
#include <nitro/rtc.h>
#include <nitro/snd.h>
#include <nitro/tp.h>
#include <stdio.h>
#include <Main/Save/save_blocks.h>
#include <Main/System/screen_fade.h>
#include <Main/System/bg_image.h>
#include <Main/System/system_init.h>
#include <Main/Unknown/game_util.h>
#include <Main/System/prompt_screen.h>
#include <Main/Scene/stage_camera.h>
#include <Main/Scene/stage_info_panel.h>
#include <Main/System/state_flow.h>
#include <Main/Scene/stage_map_menu.h>
#include <Main/Graphics/text_draw.h>
#include <Main/Graphics/model.h>
#include <Main/Graphics/texture_blocks.h>
#include <Main/Sound/wave_player.h>
#include <Main/Graphics/sprite_script.h>
#include <Main/Map/map_entries.h>
#include <Main/Map/map_floor.h>
#include <Main/Graphics/script_data.h>

// data_020c180c before data_020c1800: the compiler's size sort then leaves the two in address order.
VecFx32 data_020c180c;
VecFx32 data_020c1800;
s32 gRecapFrameIndex;

void UpdateMapMenu(void) {
    static int prevSel = -1;
    DrawParams dp;
    VecFx32 pos;
    char buf[0x20];
    u32 i;
    u32 j;
    MapMenu *p = &data_020b6ee0.mMapMenu;
    Unk_020c1454 *q = &data_020b6ee0.mUnk_a574;

    UpdateMapCursorAndCamera(&data_020c180c, &data_020c1800);
    func_02033614(data_020c3e44.mUnk_2024.mUnk_74.x, data_020c3e44.mUnk_2024.mUnk_74.z);
    prevSel = data_020b6ee0.mSelectedNodeIndex;
    data_020b6ee0.mSelectedNodeIndex = -1;
    for (i = 0; i < MAP_NODE_COUNT; i++) {
        if (i < data_020b6ee0.mReachedNodeIndex) {
            func_0200ba68(0xde6, i + MAP_NODE_POS_ID, 0, 0, 0x1000, 0);
        } else if (i == data_020b6ee0.mReachedNodeIndex) {
            func_02031fc4(i + MAP_NODE_POS_ID, &dp.mPos);
            func_02023f58(&dp, dp.mPos.x, dp.mPos.y, dp.mPos.z, 0x1000, 0, 0, gSystemState.mElapsedFrames % 400, 0x100);
            func_0202733c(&p->mReachedNodeModel, &dp, 0);
        } else {
            func_0200ba68(0xde3, i + MAP_NODE_POS_ID, 0, 0, 0x1000, 0);
        }
        if (i < data_020b6ee0.mReachedNodeIndex || i == data_020b6ee0.mReachedNodeIndex) {
            func_02031fc4(i + MAP_NODE_POS_ID, &pos);
            if (func_0200baf8(&p->mCursor.mPos, &pos) < 0x3200) {
                data_020b6ee0.mSelectedNodeIndex = i;
            }
        }
    }
    if (data_020b6ee0.mSelectedNodeIndex != -1 && p->mDelayTimer == 0) {
        if (data_020b6ee0.mSelectedNodeIndex != prevSel) {
            func_0202f110(0x167, 0x64, 0, 0x40, 0xffff, 0);
        }
        if (p->mCursorSnapped == 0) {
            func_02031fc4(data_020b6ee0.mSelectedNodeIndex + MAP_NODE_POS_ID, &pos);
            p->mCursor.mPos.x = pos.x;
            p->mCursor.mPos.z = pos.z;
            p->mCursor.mPos.y =
                func_02031270(p->mCursor.mPos.x, p->mCursor.mPos.z, p->mCursor.mPos.y + 0x32000, 0, 0, 0) + 0x1900;
            p->mCursorSnapped = 1;
            p->mNodeInfoHidden = 0;
            p->mTargetBlend = 0x100;
            p->mCursorXPerFrame = 0;
            p->mCursorZPerFrame = 0;
            q->mMapButtonOffsetX = 0x28;
            func_0202de60(&p->mUnk_1c8, data_0201abec[0]);
            func_0202de60(&p->mUnk_2cc, data_0201abec[1]);
        }
        if (p->mNodeInfoHidden == 0) {
            int a = GetNodeValue(data_020b6ee0.mSelectedNodeIndex, 0xe);
            int b = GetNodeValue(data_020b6ee0.mSelectedNodeIndex, 7);

            if (p->mUnk_1c8.mUnk_de == 0) {
                func_0202cf08(func_0202db1c(&p->mUnk_1c8), 0, 7, 0xfa, 2);
            } else {
                func_0202cf08(a + 0xed6, 0, 7, 0xfa, 2);
                func_0202cf08(b + 0xedc, 0, 7, 0xfa, 2);
                func_020215fc(0x72, 0x1c, data_0201a630, GetText(0x46), 2, 0xf0, 0x10, 2);
                sprintf(buf, "%d", data_020b6ee0.mSelectedNodeIndex + 1);
                func_020215fc(0x72, 0x3b, data_0201a6b2, buf, 2, 0xf0, 0x10, 2);
            }
            for (j = 0; j < 3; j++) {
                int k = GetSelectedNodeStageId(j);

                if (k >= 0) {
                    int v1 = func_02021e44(0xfd6, k * 0x1b + 2);
                    int v2 = func_02021e44(0xfd6, k * 0x1b + 5);

                    func_0202cf08(k + 0x1089, j * 0x37 + 0x22, 0x85, 0xfa, 2);
                    func_0202cf08(0x41088, j * 0x37 + 0x22, 0x85, 0xfc, 2);
                    func_0202cf08(v1 + 0x11a1, j * 0x37 + 0x13, 0x56, 0xfa, 2);
                    sprintf(buf, "%d", v2);
                    func_020215fc(j * 0x37 + 0x13, 0x56, data_0201a6bc, buf, 0, 0x60, 0, 2);
                }
            }
            func_0200b928(&q->mMapButtonOffsetX, 0, 4);
            func_0200bb14(q->mMapButtonOffsetX + 0xff, 0xa1, 0x101, 0x15, 0, 0, 0x17d, 1);
            func_0200bb14(-q->mMapButtonOffsetX, 0xa1, 0x102, 0xe, 0, 0, 0x5a, 1);
        } else {
            func_020215fc(0x5a, 0x5a, data_0201a64e, GetText(0x47), 2, 0xc8, 0x10, 2);
        }
    } else {
        p->mCursorSnapped = 0;
        func_020215fc(0x5a, 0x5a, data_0201a64e, GetText(0x47), 2, 0xc8, 0x10, 2);
    }
    if (p->mNodeInfoHidden == 1) {
        func_0200b928(&q->mMapButtonOffsetX, 0, 4);
        func_0200bb14(-q->mMapButtonOffsetX, 0xa1, 0x102, 0xe, 0, 0, 0x5a, 1);
    }
    UpdateMapMenuInput();
}

void LeaveMapMenu(void) {
    func_020102f8();
    ResetVramBanks();
}

void UpdateMapMenuInput(void) {
    MapMenu *p = &data_020b6ee0.mMapMenu;
    Unk_020c1454 *q = &data_020b6ee0.mUnk_a574;
    if (func_0200bd80(0x20000000, 0x101)) {
        p->mNodeInfoHidden = 1;
        p->mTargetBlend = 0x200;
        data_020b6ee0.mNodeState = 1;
        SetStateFlag(1);
    }
    if (func_0200bd80(0x20000000, 0x102)) {
        if (p->mNodeInfoHidden == 0) {
            p->mNodeInfoHidden = 1;
            p->mTargetBlend = 0x200;
            q->mMapButtonOffsetX = 0x28;
        } else {
            PushState(10);
            PopState();
        }
    }
    if (gSystemState.mTouchPressed == 1) {
        if (func_0200bd80(0x80000000, 0x101) == 0 && func_0200bd80(0x80000000, 0x102) == 0) {
            data_020b6ee0.mUnk_a574.mCameraEaseDivisor = 0x10;
            data_020b6ee0.mButtonsEnabled = 0;
            p->mDelayTimer = 0;
        }
    }
    if (gSystemState.mTouchDown == 0 && gSystemState.mTouchReleased == 0) {
        data_020b6ee0.mButtonsEnabled = 1;
    }
}

void SetupRecap(void) {
    SetFaded(3);
    UpdateSystemAndWait();
    ResetKeys();
    ResetVramBanks();
    func_0202c1c4();
    SetLcdSwap(0);
    func_0202bdd8(0x10cb, 0x10ca, 0x10cc, 0x82, 1);
    func_0202bdd8(0x10cb, 0x10ca, 0x10cc, 0x82, 2);
    LoadImage(0x237, 3);
    gRecapFrameIndex = -8;
}

void UpdateRecap(void) {
    int sel;

    func_020215fc(0x7f, 0x3a, data_0201a61c, "Recap", 2, 0xf0, 0x10, 1);
    sel = GetNodeValue(data_020b6ee0.mReachedNodeIndex, 0xf);
    if (sel != -1) {
        gRecapFrameIndex += gSystemState.mElapsedFrames % 3 / 2;
        if (gRecapFrameIndex >= 0) {
            if (gRecapFrameIndex <= 8) {
                func_0202cf08(0xfa7 - gRecapFrameIndex, 0x7f, 0x62, 0x64, 1);
            } else {
                func_0202cf08(sel + 0x1089, 0x7f, 0x62, 0x64, 1);
            }
        }
        func_0202cf08(0x1088, 0x7f, 0x62, 0x66, 1);
    } else {
        func_0202cf08(0x1088, 0x7f, 0x62, 0x66, 1);
    }
    func_0200bb14(0xff, 0xa1, 0x101, 0x15, 0, 0, 0x17d, 1);
    if (func_0200bd80(0x20000000, 0x101)) {
        SetStateFlag(1);
    }
}

void LeaveRecap(void) {
    func_020102f8();
}
