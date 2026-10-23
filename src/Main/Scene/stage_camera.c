#include <nitro/os.h>
#include <stdio.h>
#include <Main/clib.h>
#include <Main/Save/save_blocks.h>
#include <Main/System/random_angle.h>
#include <Main/System/screen_fade.h>
#include <Main/System/game_math.h>
#include <Main/System/bg_image.h>
#include <Main/System/state_flow.h>
#include <Main/Scene/stage_info_panel.h>
#include <Main/Scene/mode_select.h>
#include <nitro/mi.h>
#include <Main/System/prompt_screen.h>
#include <Main/System/sound_control.h>
#include <Main/System/game_util.h>
#include <Main/Scene/stage_data.h>
#include <Main/Scene/stage_camera.h>
#include <Main/Scene/stage_map_menu.h>
#include <Main/Scene/move_object.h>
#include <Main/Minigame/three_in_row.h>
#include <Main/Graphics/sprite_slots.h>
#include <Main/Scene/stage_progress.h>
#include <Main/Graphics/text_draw.h>
#include <Main/Graphics/vram_cache.h>
#include <Main/Graphics/model.h>
#include <Main/Graphics/texture_blocks.h>
#include <Main/Sound/wave_player.h>
#include <Main/System/file_loader.h>
#include <Main/Map/map_entries.h>
#include <Main/Map/map_floor.h>
#include <Main/Graphics/script_data.h>
#include <Main/Input/touch_buttons.h>
#include <Main/System/system_init.h>
#include <Main/Graphics/gx_pool.h>

void ClearPanelState(void) {
    data_020b6ee0.mPanelState = 0;
}

void SetStageCameraFar(int stage) {
    StageRec *rec = &gStages[stage];
    int distancePerFrame = rec->mDistancePerFrame >> 4;
    int height;

    if (stage < 0 || stage >= STAGE_MAX) {
        OS_Terminate();
    }
    gMapState.mCamera.mLookAtGoal.x = rec->mLookAtPos.x;
    gMapState.mCamera.mLookAtGoal.z = rec->mLookAtPos.z;
    gMapState.mCamera.mLookAtGoal.y = rec->mLookAtPos.y;
    gMapState.mCamera.mPosGoal.x = rec->mPos.x - 400 * FX_SinCosTable_[(rec->mCameraAngle >> 4) * 2];
    gMapState.mCamera.mPosGoal.z = rec->mPos.z + 400 * FX_SinCosTable_[(rec->mCameraAngle >> 4) * 2 + 1];
    height = rec->mPos.y + MapRangeFromZero(distancePerFrame, 0xc00, 0x320000, 0x4b0000);
    gMapState.mCamera.mPosGoal.y = height;
    if (height > 0x5dc000) {
        gMapState.mCamera.mPosGoal.y = 0x5dc000;
    }
    EaseCameraToGoals();
}

void SetStageCameraNear(int stage) {
    StageRec *rec = &gStages[stage];

    if (stage < 0 || stage >= STAGE_MAX) {
        OS_Terminate();
    }
    gMapState.mCamera.mLookAtGoal.x = rec->mLookAtPos.x;
    gMapState.mCamera.mLookAtGoal.z = rec->mLookAtPos.z;
    gMapState.mCamera.mLookAtGoal.y = rec->mLookAtPos.y;
    gMapState.mCamera.mPosGoal.x = rec->mPos.x - 300 * FX_SinCosTable_[(rec->mCameraAngle >> 4) * 2];
    gMapState.mCamera.mPosGoal.z = rec->mPos.z + 300 * FX_SinCosTable_[(rec->mCameraAngle >> 4) * 2 + 1];
    gMapState.mCamera.mPosGoal.y = rec->mPos.y + 0x64000;
    EaseCameraToGoals();
}

void UpdateStageCamera(int stage) {
    if (stage < -1 || stage >= STAGE_MAX) {
        OS_Terminate();
    }
    if (stage != -1) {
        if (data_020b6ee0.mPanelState <= 1) {
            SetStageCameraFar(stage);
        } else {
            SetStageCameraNear(stage);
        }
        return;
    }
    data_020b6ee0.mUnk_a574.mUnk_00.x = data_020b6ee0.mUnk_a574.mUnk_0c.x - 600 * FX_SinCosTable_[0];
    data_020b6ee0.mUnk_a574.mUnk_00.z = data_020b6ee0.mUnk_a574.mUnk_0c.z + 600 * FX_SinCosTable_[1];
    data_020b6ee0.mUnk_a574.mUnk_00.y = data_020b6ee0.mUnk_a574.mUnk_0c.y + 0x3e8000;
    EaseVecToward(&gMapState.mCamera.mLookAtGoal, data_020b6ee0.mUnk_a574.mUnk_0c,
                  data_020b6ee0.mUnk_a574.mCameraEaseDivisor);
    EaseVecToward(&gMapState.mCamera.mPosGoal, data_020b6ee0.mUnk_a574.mUnk_00,
                  data_020b6ee0.mUnk_a574.mCameraEaseDivisor);
    EaseCameraToGoals();
}

void func_02015f34(void) {
    int i;
    DrawParams params;
    char text[12];
    s32 pos[2];
    Unk_020b6ee8 *obj;

    for (i = 0; i < data_020b6ee0.mUnk_0c48; i++) {
        obj = &data_020b6ee0.mUnk_0008[i];
        if (obj->mModel.mId != 0) {
            SetDrawParams(&params, obj->mPosX << 9, 0, obj->mPosZ << 9, 0x1000, obj->mUnk_185 << 8, 0, 0xff, 0x100);
            if (IsBoxInView(&params.mPos, 0x12c000, 0x190000, 0x190000) == 1) {
                UpdateAndDrawModel(&obj->mModel, &params, 0);
                if (obj->mKind == 10) {
                    sprintf(text, "%d", obj->mUnk_186);
                    ProjectToScreen(&params, pos);
                    DrawText(pos[0], pos[1], data_0201a61c, text, 2, 0x82, 0x10, 1);
                }
            }
        }
    }
}

void func_02016008(int stage) {
    StageRec *rec = &gStages[stage];
    s32 px = rec->mPos.x / 512;
    s32 pz = rec->mPos.z / 512;
    int moved = 0;
    Unk_020b7ef4 *f;
    Unk_020b7ef4 *b;
    int i;
    int d;
    int dx;
    int dz;
    DrawParams params;
    VecFx32 pos;

    if (stage < 0 || stage >= STAGE_MAX) {
        OS_Terminate();
    }
    f = data_020b6ee0.mUnk_1014;
    for (i = 0; i < data_020b6ee0.mUnk_5a54; i++, f++) {
        dx = f->mPosX - px;
        dz = f->mPosZ - pz;
        if (f->mKind == 0x19) {
            d = Length2d(dx, dz);
            if (d < 400) {
                break;
            }
        } else if (f->mKind == 0x14) {
            if (abs(dx) < 400 && abs(dz) < 800) {
                f->mNearFrames++;
                f->mModel.mSignals |= 1;
                if (f->mNearFrames > (f->mUnk_187 + 1) * 6) {
                    rec->mMoveAngle += 0x8000;
                    rec->mDistancePerFrame += 0x2000;
                    f->mNearFrames = 0;
                    AddMapObjectTimeOrScore(rec, 0x14, i);
                    f->mModel.mSignals |= 2;
                    PlayWaveKeepAlive(0x2b, 0x64, 0, 0x40, 0xffff, 0);
                }
            } else {
                f->mNearFrames = 0;
            }
        } else {
            d = abs(dx) + abs(dz);
            if (d < 400) {
                break;
            }
        }
    }
    if (i != data_020b6ee0.mUnk_5a54) {
        switch (f->mKind) {
            case 0xf:
                rec->mPos = rec->mPrevPos;
                rec->mMoveAngle += 0x8000;
                rec->mDistancePerFrame = 0x2000;
                AddMapObjectTimeOrScore(rec, 0xf, i);
                break;
            case 0x19: {
                int q = 0x32000 / d;
                int ax = px - f->mPosX;
                int az = pz - f->mPosZ;
                ax *= q;
                az *= q;
                rec->mPos.x = ax + (f->mPosX << 9);
                rec->mPos.z = az + (f->mPosZ << 9);
                rec->mMoveAngle = AngleOfVector(rec->mPos.x / 512 - f->mPosX, rec->mPos.z / 512 - f->mPosZ);
                rec->mDistancePerFrame += 0x4000;
                PlayWaveKeepAlive(0x28, 0x64, 0, 0x40, 0xffff, 0);
                break;
            }
            case 0x23:
                rec->mMoveAngle = ((f->mQuarterTurns + 1) & 3) << 14;
                rec->mDistancePerFrame = 0x14000;
                PlayWaveKeepAlive(1, 0x64, 0, 0x40, 0xffff, 0);
                break;
            case 0x28:
                moved = 1;
                if (rec->mUnk_22c == 0) {
                    u8 key = f->mTargetId;
                    Unk_020b7ef4 *p = data_020b6ee0.mUnk_1014;
                    int j;

                    SetFaded(moved);
                    PlayWaveKeepAlive(0x59, 0x64, 0, 0x40, 0xffff, 0);
                    rec->mDistancePerFrame = 0;
                    for (j = 0; j < data_020b6ee0.mUnk_5a54; j++, p++) {
                        if (p->mKind == 0x28 && p->mId == key) {
                            rec->mPos.x = p->mPosX << 9;
                            rec->mPos.z = p->mPosZ << 9;
                            break;
                        }
                    }
                }
                break;
        }
    } else {
        rec->mPrevPos = rec->mPos;
    }
    rec->mUnk_22c = moved;
    b = data_020b6ee0.mUnk_1014;
    i = 0;
    while (i < data_020b6ee0.mUnk_5a54) {
        pos.x = b->mPosX << 9;
        pos.z = b->mPosZ << 9;
        pos.y = 0;
        if (IsBoxInView(&pos, 0x190000, 0x190000, 0x190000) == 1) {
            switch (b->mKind) {
                case 0x19:
                    if (b == f) {
                        b->mModel.mSignals |= 1;
                    }
                    SetDrawParams(&params, pos.x, pos.y, pos.z, 0x1000, 0, 0, 0xff, 0x100);
                    UpdateAndDrawModel(&b->mModel, &params, 0);
                    break;
                case 0x14:
                    pos.y = 0;
                    SetDrawParams(&params, pos.x, pos.y, pos.z, 0x1000, (u16) (((b->mQuarterTurns + 1) & 3) << 14), 0,
                                  0xff, 0x100);
                    UpdateAndDrawModel(&b->mModel, &params, 0);
                    break;
                case 0x28:
                    pos.y = 0;
                    SetDrawParams(&params, pos.x, pos.y, pos.z, 0x1000, (u16) (((b->mQuarterTurns + 1) & 3) << 14), 0,
                                  0xff, 0x100);
                    UpdateAndDrawModel(&b->mModel, &params, 0);
                    break;
                case 0xf:
                    pos.y = 0;
                    SetDrawParams(&params, pos.x, pos.y, pos.z, 0x1000, (u16) (((b->mQuarterTurns + 1) & 3) << 14), 0,
                                  0xff, 0x100);
                    DrawModelFile(0xc87, &params, 0);
                    break;
                case 0x23:
                    pos.y = 0;
                    SetDrawParams(&params, pos.x, pos.y, pos.z, 0x1000, (u16) (b->mQuarterTurns << 14), 0, 0xff, 0x100);
                    DrawModelFile(0xc8a, &params, 0);
                    break;
            }
        }
        b++;
        i++;
    }
}

void func_020164e0(int stage) {
    StageRec *rec = &gStages[stage];
    s32 px = rec->mPos.x / 512;
    s32 pz = rec->mPos.z / 512;
    DrawParams params;
    VecFx32 pos;
    int i;
    Unk_020bc938 *obj = data_020b6ee0.mUnk_5a58;
    int found;
    int dx;
    int dz;

    if (stage < 0 || stage >= STAGE_MAX) {
        OS_Terminate();
    }
    SetDrawParams(&params, 0, 0, 0, 0x1000, 0, 2, 0xff, 0);
    i = 0;
    pos.y = 0;
    found = -1;
    while (i < data_020b6ee0.mUnk_8ad8) {
        if (obj->mPresent == 1) {
            pos.x = obj->mPosX << 9;
            pos.z = obj->mPosZ << 9;
            if (IsBoxInView(&pos, 0x190000, 0x190000, 0x190000) == 1) {
                params.mPos.x = pos.x;
                params.mPos.z = pos.z;
                if (obj->mKind == 0x1e) {
                    UpdateAndDrawModel(&obj->mModel, &params, 0);
                }
                if (found == -1) {
                    dx = obj->mPosX;
                    dz = obj->mPosZ;
                    dx -= px;
                    dz -= pz;
                    if (abs(dx) + abs(dz) < 400) {
                        found = i;
                    }
                }
            }
        }
        i++;
        obj++;
    }
    if (found != -1) {
        obj = &data_020b6ee0.mUnk_5a58[found];
        if (obj->mKind == 0x1e) {
            AddMapObjectTimeOrScore(rec, 0x1e, found);
            obj->mPresent = 0;
            PlayWaveKeepAlive(0xe0, 0x64, 0, 0x40, 0xffff, 0);
        }
    }
}

// Fills mUnk_0008, mUnk_1014, mUnk_5a58 and mFollowerEntries and their counts, by entry type, from the map entry list
// in chunk 0x5514. Also clears gFollowerGroup and sets its defaults (mSlowRange 600, mNearDistance 0x640,
// mUseRingTest 1). Called by state 4's set-up.
void SetupMapObjects(void) {
    int n5 = 0;
    int n34 = 0;
    int n7 = 0;
    int n30 = 0;
    u16 cnt;
    u16 *q;
    VecFx32 pos;
    u8 flags;

    memset(data_020b6ee0.mFollowerEntries, 0, 0x384);
    memset(&data_020b6ee0.mUnk_0c4c, 0, 0x10);
    memset(&gFollowerGroup, 0, 0x3f8);
    data_020b6ee0.mUnk_8ad9 = 0;
    q = GetMapChunk(0x5514);
    cnt = *q;
    q += 2;
    while (cnt-- != 0) {
        int extra = *q & 0xf;
        int type = (*q >> 4) & 0x7f;

        func_02032018(q[1], &pos, &flags);
        if (type == 0xf) {
            data_020b6ee0.mUnk_1014[n7].mKind = type;
            data_020b6ee0.mUnk_1014[n7].mPosX = pos.x / 512;
            data_020b6ee0.mUnk_1014[n7].mPosZ = pos.z / 512;
            data_020b6ee0.mUnk_1014[n7].mUnk_183 = ReadBits(q + 2, 0, 3);
            n7++;
        } else if (type == 0x14) {
            data_020b6ee0.mUnk_1014[n7].mKind = type;
            data_020b6ee0.mUnk_1014[n7].mPosX = pos.x / 512;
            data_020b6ee0.mUnk_1014[n7].mPosZ = pos.z / 512;
            data_020b6ee0.mUnk_1014[n7].mUnk_187 = ReadBits(q + 2, 0, 1);
            data_020b6ee0.mUnk_1014[n7].mNearFrames = 0;
            data_020b6ee0.mUnk_1014[n7].mQuarterTurns = (u32) flags >> 6;
            InitModel(&data_020b6ee0.mUnk_1014[n7].mModel, 0, 0, 0, data_0201aba0[6]);
            n7++;
        } else if (type == 0x19) {
            data_020b6ee0.mUnk_1014[n7].mKind = type;
            data_020b6ee0.mUnk_1014[n7].mPosX = pos.x / 512;
            data_020b6ee0.mUnk_1014[n7].mPosZ = pos.z / 512;
            InitModel(&data_020b6ee0.mUnk_1014[n7].mModel, 0, 0, 0, data_0201aba0[0]);
            n7++;
        } else if (type == 0x1e) {
            data_020b6ee0.mUnk_5a58[n30].mKind = type;
            data_020b6ee0.mUnk_5a58[n30].mPosX = pos.x / 512;
            data_020b6ee0.mUnk_5a58[n30].mPosZ = pos.z / 512;
            data_020b6ee0.mUnk_5a58[n30].mPresent = 1;
            data_020b6ee0.mUnk_5a58[n30].mUnk_181[1] = ReadBits(q + 2, 0, 0);
            data_020b6ee0.mUnk_5a58[n30].mUnk_181[0] = ReadBits(q + 2, 1, 7);
            if (data_020b6ee0.mUnk_5a58[n30].mUnk_181[1] == 0) {
                InitModel(&data_020b6ee0.mUnk_5a58[n30].mModel, 0xc89, 0xc88, 2, 0);
                data_020b6ee0.mUnk_8ad9++;
            } else {
                InitModel(&data_020b6ee0.mUnk_5a58[n30].mModel, 0xc8c, 0xc8b, 2, 0);
            }
            n30++;
        } else if (type == 0x23) {
            data_020b6ee0.mUnk_1014[n7].mKind = type;
            data_020b6ee0.mUnk_1014[n7].mPosX = pos.x / 512;
            data_020b6ee0.mUnk_1014[n7].mPosZ = pos.z / 512;
            data_020b6ee0.mUnk_1014[n7].mQuarterTurns = (u32) flags >> 6;
            n7++;
        } else if (type == 0x28) {
            data_020b6ee0.mUnk_1014[n7].mKind = type;
            data_020b6ee0.mUnk_1014[n7].mPosX = pos.x / 512;
            data_020b6ee0.mUnk_1014[n7].mPosZ = pos.z / 512;
            data_020b6ee0.mUnk_1014[n7].mTargetId = ReadBits(q + 2, 0, 3);
            data_020b6ee0.mUnk_1014[n7].mId = ReadBits(q + 2, 4, 7);
            InitModel(&data_020b6ee0.mUnk_1014[n7].mModel, 0xc8e, 0xc8d, 2, 0);
            n7++;
        } else if (type == 5) {
            data_020b6ee0.mFollowerEntries[n5].mPos = pos;
            data_020b6ee0.mFollowerEntries[n5].mKind = ReadBits(q + 2, 0, 0);
            data_020b6ee0.mFollowerEntries[n5].mPresent = 1;
            n5++;
        } else if (type == 0xa || type == 0x2d) {
            data_020b6ee0.mUnk_0008[n34].mPosX = pos.x / 512;
            data_020b6ee0.mUnk_0008[n34].mPosZ = pos.z / 512;
            data_020b6ee0.mUnk_0008[n34].mUnk_185 = flags;
            data_020b6ee0.mUnk_0008[n34].mKind = type;
            if (type == 0xa) {
                data_020b6ee0.mUnk_0008[n34].mUnk_186 = ReadBits(q + 2, 0, 2);
                InitModel(&data_020b6ee0.mUnk_0008[n34].mModel, 0, 0, 0, data_0201aba0[1]);
            } else if (type == 0x2d) {
                data_020b6ee0.mUnk_0008[n34].mUnk_187 = ReadBits(q + 2, 0, 2);
                data_020b6ee0.mUnk_0008[n34].mUnk_186 = 0x20;
                InitModel(&data_020b6ee0.mUnk_0008[n34].mModel, 0, 0, 0, data_0201aba0[2]);
            }
            n34++;
        }
        q += extra + 2;
    }
    if (n34 >= 8) {
        OS_Terminate();
    }
    data_020b6ee0.mUnk_0c48 = n34;
    if (n5 >= 0x20) {
        OS_Terminate();
    }
    data_020b6ee0.mFollowerEntryCount = n5;
    if (n7 >= 0x30) {
        OS_Terminate();
    }
    data_020b6ee0.mUnk_5a54 = n7;
    if (n30 >= 0x20) {
        OS_Terminate();
    }
    data_020b6ee0.mUnk_8ad8 = n30;
    gFollowerGroup.mFollowerCount = 1;
    gFollowerGroup.mSlowRange = 600;
    gFollowerGroup.mNearDistance = 0x640;
    gFollowerGroup.mUseRingTest = 1;
}

void UpdateFollowers(int stage) {
    StageRec *rec = &gStages[stage];
    int n;
    int i;

    if (stage < 0 || stage >= STAGE_MAX) {
        OS_Terminate();
    }
    n = 0;
    for (i = 0; i < data_020b6ee0.mFollowerEntryCount; i++) {
        if (i >= 0x20) {
            OS_Terminate();
        }
        if (n >= 0x11) {
            OS_Terminate();
        }
        if (data_020b6ee0.mFollowerEntries[i].mPresent != 0) {
            int dx = abs(data_020b6ee0.mFollowerEntries[i].mPos.x - rec->mPos.x);
            int dz = abs(data_020b6ee0.mFollowerEntries[i].mPos.z - rec->mPos.z);
            if (dx + dz < 0x4b0000) {
                gFollowerGroup.mFollowers[n].mPos.x = data_020b6ee0.mFollowerEntries[i].mPos.x / 512;
                gFollowerGroup.mFollowers[n].mPos.y = data_020b6ee0.mFollowerEntries[i].mPos.y / 512;
                gFollowerGroup.mFollowers[n].mPos.z = data_020b6ee0.mFollowerEntries[i].mPos.z / 512;
                gFollowerGroup.mFollowers[n].mDiameter = 300;
                gFollowerGroup.mFollowers[n].mMaxDistancePerFrame = 32.0f;
                gFollowerGroup.mFollowers[n].mFollowerEntryIndex = i;
                data_020b6ee0.mFollowerEntries[i].mInRange = 1;
                n++;
            } else {
                data_020b6ee0.mFollowerEntries[i].mInRange = 0;
            }
        }
    }
    if (data_020b6ee0.mFixedTargetTimer != 0) {
        data_020b6ee0.mFixedTargetTimer--;
        gFollowerGroup.mFollowers[n].mPos.x = data_020b6ee0.mFixedTargetX;
        gFollowerGroup.mFollowers[n].mPos.z = data_020b6ee0.mFixedTargetZ;
        gFollowerGroup.mFollowers[n].mDiameter = 0;
        gFollowerGroup.mTargetFixed = 1;
    } else {
        gFollowerGroup.mFollowers[n].mPos.x = rec->mPos.x / 512;
        gFollowerGroup.mFollowers[n].mPos.y = rec->mPos.y / 512;
        gFollowerGroup.mFollowers[n].mPos.z = rec->mPos.z / 512;
        gFollowerGroup.mFollowers[n].mDiameter = 900;
        gFollowerGroup.mFollowers[n].mMaxDistancePerFrame = 0;
        gFollowerGroup.mTargetFixed = 0;
    }
    gFollowerGroup.mFollowerCount = n;
    MoveFollowers(&gFollowerGroup);
    for (i = 0; i < n; i++) {
        int idx = gFollowerGroup.mFollowers[i].mFollowerEntryIndex;
        if (data_020b6ee0.mFollowerEntries[idx].mInRange == 1) {
            data_020b6ee0.mFollowerEntries[idx].mPos.x = 512.0f * gFollowerGroup.mFollowers[i].mPos.x;
            data_020b6ee0.mFollowerEntries[idx].mPos.y = 512.0f * gFollowerGroup.mFollowers[i].mPos.y;
            data_020b6ee0.mFollowerEntries[idx].mPos.z = 512.0f * gFollowerGroup.mFollowers[i].mPos.z;
        }
    }
    for (i = 0; i < data_020b6ee0.mFollowerEntryCount; i++) {
        if (data_020b6ee0.mFollowerEntries[i].mInRange == 1) {
            if (func_02031dc4(data_020b6ee0.mFollowerEntries[i].mPos.x,
                              data_020b6ee0.mFollowerEntries[i].mPos.z) == 1) {
                ProjectToScreen(&data_020b6ee0.mFollowerEntries[i], data_020b6ee0.mFollowerEntries[i].mScreenPos);
            }
        }
    }
    for (i = 0; i < data_020b6ee0.mFollowerEntryCount; i++) {
        if (data_020b6ee0.mFollowerEntries[i].mInRange == 1) {
            func_0200ba94(&data_020b6ee0.mFollowerEntries[i].mPos, 0x38400, 0x400);
            DrawSprite(data_020b6ee0.mFollowerEntries[i].mKind == 0 ? 0xe24 : 0xe23,
                       data_020b6ee0.mFollowerEntries[i].mScreenPos[0], data_020b6ee0.mFollowerEntries[i].mScreenPos[1],
                       0, 1);
        }
    }
}

// One follower's step, run by MoveFollowers (called from UpdateFollowers) for each follower whose
// mMaxDistancePerFrame is not 0: the ring test (a hit puts it back at mPrevPos), the sector distances to the other
// followers and the speed from the follower found ahead. angle is unused.
void UpdateFollower(FollowerGroup *followers, int index, u16 angle) {
    int bestIdx = 0;
    Follower *f = &followers->mFollowers[index];
    StageRec *rec = &gStages[data_020b6ee0.mSelectedStageIndex];
    int bestDist;
    int j;
    Follower *o;

    if (data_020b6ee0.mSelectedStageIndex < 0 || data_020b6ee0.mSelectedStageIndex >= STAGE_MAX) {
        OS_Terminate();
    }
    if (f->mFollowerEntryIndex >= 0x20) {
        OS_Terminate();
    }
    bestDist = followers->mNearDistance;
    for (j = 0; j < 8; j++) {
        f->mSectorDistances[j] = 30000;
    }
    if (followers->mUseRingTest == 1) {
        VecFx32 pos;
        pos.x = 512.0f * f->mPos.x;
        pos.y = 512.0f * f->mPos.y;
        pos.z = 512.0f * f->mPos.z;
        if (FindNearestRingSegment(&pos, f->mDiameter, 0x6e, 0, 0x13, 0)) {
            if ((gMapState.mCamera.mNearestRingKey & 0x10) && data_020b6ee0.mFixedTargetTimer != 0 &&
                data_020b6ee0.mFollowerEntries[f->mFollowerEntryIndex].mKind == 0 &&
                data_020b6ee0.mUnk_0c4c == gMapState.mCamera.mNearestRing &&
                data_020b6ee0.mUnk_0008[data_020b6ee0.mFixedTargetObjectIndex].mUnk_186 > 0) {
                data_020b6ee0.mUnk_0008[data_020b6ee0.mFixedTargetObjectIndex].mModel.mSignals |= 1;
                data_020b6ee0.mUnk_0008[data_020b6ee0.mFixedTargetObjectIndex].mUnk_186--;
                if (data_020b6ee0.mUnk_0008[data_020b6ee0.mFixedTargetObjectIndex].mUnk_186 == 0) {
                    SetLinkedRingKeys(data_020b6ee0.mUnk_0c4c, 0, 0);
                    data_020b6ee0.mUnk_0008[data_020b6ee0.mFixedTargetObjectIndex].mModel.mSignals |= 2;
                    PlayWaveKeepAlive(0xdb, 0x64, 0, 0x40, 0xffff, 0);
                } else {
                    PlayWaveKeepAlive(0xdd, 0x64, 0, 0x40, 0xffff, 0);
                }
                data_020b6ee0.mFollowerEntries[f->mFollowerEntryIndex].mPresent = 0;
                data_020b6ee0.mFollowerEntries[f->mFollowerEntryIndex].mInRange = 0;
            }
            {
                u32 cam = GetNearestSegmentAngle();
                int cross = Cross2d(gMapState.mCamera.mNearestSegmentStart.x, gMapState.mCamera.mNearestSegmentStart.z,
                                    gMapState.mCamera.mNearestSegmentEnd.x, gMapState.mCamera.mNearestSegmentEnd.z,
                                    f->mPos.x, f->mPos.z);
                f->mPos = f->mPrevPos;
                f->mAngle = cam + (((cross != 0) ? ((cross >> 31) | 1) : 0) << 14);
                f->mTargetAngle = f->mAngle;
            }
        }
    }
    j = 0;
    if (j < followers->mFollowerCount + 1) {
        o = followers->mFollowers;
        do {
            int dx;
            int dz;
            int dist;

            if (j != index) {
                dx = o->mPos.x - f->mPos.x;
                dz = o->mPos.z - f->mPos.z;
                dist = Length2d(dx, dz);
                if (dist < followers->mNearDistance) {
                    u32 ang = AngleOfVector(dx, dz);
                    const s16 *s1 = &FX_SinCosTable_[((u16) (ang - 0x4000) >> 4) * 2];
                    int ax = f->mPos.x + (float) (f->mDiameter * s1[0] / 2 / 0x1000);
                    int az = f->mPos.z - (float) (f->mDiameter * s1[1] / 2 / 0x1000);
                    const s16 *s2 = &FX_SinCosTable_[((u16) (ang + 0x4000) >> 4) * 2];
                    int bx = o->mPos.x + (float) (o->mDiameter * s2[0] / 2 / 0x1000);
                    int bz = o->mPos.z - (float) (o->mDiameter * s2[1] / 2 / 0x1000);
                    u32 span = AngleDistance(ang, AngleOfVector(bx - ax, bz - az));
                    u16 cur = ang - (u16) span;
                    u32 end = ang + span;

                    while (IsAngleIncreasing(end, cur) == 1) {
                        u16 sector = ((cur + 0x1000) / 0x2000) & 7;
                        if (f->mSectorDistances[sector] > dist) {
                            f->mSectorDistances[sector] = dist;
                        }
                        cur += 0x200;
                    }
                    if (f->mSectorDistances[(u32) f->mAngle >> 13] < bestDist) {
                        bestDist = dist;
                        bestIdx = j;
                    }
                }
            }
            o++;
            j++;
        } while (j < followers->mFollowerCount + 1);
    }
    if (bestDist < followers->mNearDistance) {
        int avg = (f->mDiameter + followers->mFollowers[bestIdx].mDiameter) / 2;
        int limit = followers->mSlowRange + avg;

        bestDist = (bestDist >= limit) ? limit : bestDist;
        f->mDistancePerFrame = f->mMaxDistancePerFrame * (float) MapRange(bestDist, avg, limit, 0, 0x400) / 1024.0f;
        f->mNeighbourAhead = 1;
        if (f->mDistancePerFrame < 0.1) {
            if (data_020b6ee0.mFollowerEntries[f->mFollowerEntryIndex].mKind == 1 &&
                bestIdx == followers->mFollowerCount && gFollowerGroup.mTargetFixed == 0) {
                data_020b6ee0.mFollowerEntries[f->mFollowerEntryIndex].mPresent = 0;
                data_020b6ee0.mFollowerEntries[f->mFollowerEntryIndex].mInRange = 0;
                AddMapObjectTimeOrScore(rec, 5, -1);
            }
        }
    } else {
        f->mNeighbourAhead = 0;
        if (f->mDistancePerFrame != f->mMaxDistancePerFrame) {
            f->mDistancePerFrame = f->mDistancePerFrame + (f->mMaxDistancePerFrame - f->mDistancePerFrame) / 16.0f;
        }
    }
}

void MoveFollowers(FollowerGroup *followers) {
    Follower *target = &followers->mFollowers[followers->mFollowerCount];
    int i;

    if (followers->mFollowerCount >= 0x11) {
        OS_Terminate();
    }
    for (i = 0; i < followers->mFollowerCount; i++) {
        Follower *f = &followers->mFollowers[i];
        const s16 *s1;
        const s16 *s2;
        float fa;
        float fb;
        float fc;
        float fd;
        float px;
        float pz;
        u32 ang;
        int diff;
        int bi;
        u32 sec;
        int best;
        int j;

        s1 = &FX_SinCosTable_[(f->mAngle >> 4) * 2];
        fa = (f->mDiameter * s1[0] * 2) / 0x1000;
        fb = (f->mDiameter * -s1[1] * 2) / 0x1000;
        s2 = &FX_SinCosTable_[(f->mTargetAngle >> 4) * 2];
        fc = (f->mDiameter * s2[0] * 3) / 0x1000;
        fd = (f->mDiameter * -s2[1] * 3) / 0x1000;
        px = fa + f->mPos.x;
        pz = fb + f->mPos.z;
        px = fc + px;
        pz = fd + pz;
        diff = AngleDelta(f->mAngle, (u16) AngleOfVector(px - f->mPos.x, pz - f->mPos.z));
        if (abs(diff) > 0x100) {
            f->mAngle += diff / 8;
        }
        if (0 != f->mMaxDistancePerFrame) {
            UpdateFollower(followers, i, f->mAngle);
        }
        f->mPrevPos = f->mPos;
        f->mPos.x = f->mPos.x + f->mDistancePerFrame * (float) FX_SinCosTable_[(f->mAngle >> 4) * 2] / 4096.0f;
        f->mPos.z = f->mPos.z - f->mDistancePerFrame * (float) FX_SinCosTable_[(f->mAngle >> 4) * 2 + 1] / 4096.0f;
        ang = AngleOfVector(target->mPos.x - f->mPos.x, target->mPos.z - f->mPos.z);
        if (f->mNeighbourAhead == 1 && 2.0f * f->mDistancePerFrame < f->mMaxDistancePerFrame) {
            best = 0;
            sec = ang >> 13;

            for (j = 0; j < 4; j++) {
                int off = (j & 1) ? -(j / 2 + 1) : (j / 2 + 1);
                int s = (sec + off) & 7;
                if (f->mSectorDistances[s] > best) {
                    best = f->mSectorDistances[s];
                    bi = s;
                }
            }
            if (best > f->mDiameter) {
                ang = (u16) ((bi << 13) + 0x1000);
            }
        }
        diff = AngleDelta(f->mTargetAngle, ang);
        if (abs(diff) > 0x100) {
            f->mTargetAngle = f->mTargetAngle + diff / 8;
        } else {
            f->mTargetAngle = ang;
        }
    }
}

#pragma thumb off
int SelectStageOnTap(int stage, int *out) {
    StageRec *rec = &gStages[stage];

    if (gSystemState.mTouchReleased && rec->mVisible == 1) {
        if ((int) Length2d(gSystemState.mTouchX - rec->mScreenX, gSystemState.mTouchY - rec->mScreenY) < 0x10) {
            if ((int) Length2d(gSystemState.mTouchX - data_020b6ee0.mUnk_a574.mTouchPressX,
                               gSystemState.mTouchY - data_020b6ee0.mUnk_a574.mTouchPressY) < 6) {
                *out = stage;
                PlayWaveKeepAlive(0x167, 0x64, 0, 0x40, 0xffff, 0);
                return 1;
            }
        }
    }
    return 0;
}
#pragma thumb on

static inline s32 sinIdx(int a) {
    return FX_SinCosTable_[(a >> 4) * 2];
}

static inline s32 cosIdx(int a) {
    return FX_SinCosTable_[(a >> 4) * 2 + 1];
}

#pragma thumb off

// Per-frame step of stage index. Clamps mDistancePerFrame to -0x20000..0x20000 and, on a touch press, records the
// touch point in mUnk_a574.
// While the stage's mEventKey is not 1, or mPanelState is 3, 4, 6, 7 or 8, it then only turns the selected stage by
// the decaying spin mUnk_a8e8, or lets a tap select another stage (panel states 0..2). The touch drag that would set
// the spin here needs panel state 3 or 4 with flags bit 0 clear, which no call reaches in this build: state 10 sets
// bit 0 in panel states 3 to 5, and state 4 keeps panel state 0.
// Otherwise it moves the stage along mMoveAngle, keeps it on the ground, and handles its hits with other stages and
// the map (FindNearestRingSegment).
void UpdateStageMotion(int index, int *sel, int flags) {
    StageRec *rec = &gStages[index];
    int hit = 0;
    int i;
    s32 x;
    s32 z;
    s32 ground;
    VecFx32 old;
    VecFx32 push;
    u32 base;
    int res;
    s32 turn;
    s32 ex;
    s32 ez;
    u16 angle;
    s32 spd;
    s32 k;

    if (index < 0 || index >= STAGE_MAX) {
        OS_Terminate();
    }
    rec->mDistancePerFrame = (rec->mDistancePerFrame < 0x20000 ? rec->mDistancePerFrame : 0x20000) < -0x20000
                                 ? -0x20000
                                 : (rec->mDistancePerFrame < 0x20000 ? rec->mDistancePerFrame : 0x20000);
    if (gSystemState.mTouchPressed) {
        data_020b6ee0.mUnk_a574.mTouchPressX = gSystemState.mTouchX;
        data_020b6ee0.mUnk_a574.mTouchPressY = gSystemState.mTouchY;
    }
    if (rec->mEventKey != 1 || data_020b6ee0.mPanelState == 3 || data_020b6ee0.mPanelState == 4 ||
        data_020b6ee0.mPanelState == 6 || data_020b6ee0.mPanelState == 7 || data_020b6ee0.mPanelState == 8) {
        if (index == *sel) {
            if (!(flags & 1) && (data_020b6ee0.mPanelState == 3 || data_020b6ee0.mPanelState == 4)) {
                if (gSystemState.mTouchPressed) {
                    data_020b6ee0.mPrevTouchX = gSystemState.mTouchX;
                    data_020b6ee0.mUnk_0c5d = gSystemState.mTouchY;
                    data_020b6ee0.mUnk_a8e8 = 0;
                } else if (gSystemState.mTouchDown) {
                    data_020b6ee0.mUnk_a8e8 += (gSystemState.mTouchX - data_020b6ee0.mPrevTouchX) << 7;
                    data_020b6ee0.mPrevTouchX = gSystemState.mTouchX;
                }
            }
            rec->mMoveAngle -= data_020b6ee0.mUnk_a8e8;
            data_020b6ee0.mUnk_a8e8 = data_020b6ee0.mUnk_a8e8 * 31 / 32;
            return;
        }
        if (data_020b6ee0.mPanelState > 2) {
            return;
        }
        if (SelectStageOnTap(index, sel) != 1) {
            return;
        }
        rec->mSignals |= 2;
        return;
    }
    if (!(flags & 1)) {
        if (index == *sel) {
            if (gSystemState.mTouchReleased) {
                int n = gTouchInput.mTrail.mTouchState.mTrailCount - 1;
                if (n < 30 && n > 1) {
                    s32 dx = (gTouchInput.mTrail.mPoints[n].mX - gTouchInput.mTrail.mPoints[0].mX) / 4;
                    s32 dy = (gTouchInput.mTrail.mPoints[n].mY - gTouchInput.mTrail.mPoints[0].mY) / 4;
                    int len = Length2d(dx, dy);
                    if (len > 8) {
                        dx = (dx << 3) / len;
                        dy = (dy << 3) / len;
                        Length2d(dx, dy);
                    }
                    dx = (dx << 12) + rec->mDistancePerFrame * sinIdx(rec->mMoveAngle) / 4096;
                    dy = (dy << 12) + rec->mDistancePerFrame * -cosIdx(rec->mMoveAngle) / 4096;
                    rec->mMoveAngle = AngleOfVector(dx, dy);
                    len = Length2d(dx, dy);
                    rec->mDistancePerFrame = len < 0x14000 ? len : 0x14000;
                }
            } else if (gSystemState.mTouchDown) {
                int n = gTouchInput.mTrail.mTouchState.mTrailCount - 1;
                if (n > 4) {
                    if ((int) Length2d(gTouchInput.mTrail.mPoints[n].mX - (gTouchInput.mTrail.mPoints + n - 4)->mX,
                                       gTouchInput.mTrail.mPoints[n].mY - (gTouchInput.mTrail.mPoints + n - 4)->mY) <
                        4) {
                        rec->mDistancePerFrame -= 3276.8;
                    }
                }
            }
        } else {
            SelectStageOnTap(index, sel);
        }
    }
    spd = rec->mDistancePerFrame - 204.8;
    rec->mDistancePerFrame = spd;
    if (spd <= 0) {
        rec->mDistancePerFrame = 0;
        return;
    }
    // Matching: k is loaded before spd is scaled and stored after the position copy. Adding in one statement (or with
    // +=) gives the load and the multiply other registers.
    k = rec->mUnk_1dc;
    spd = spd * 0xc0 >> 12;
    k = k + spd;
    old = rec->mPos;
    rec->mUnk_1dc = k;
    x = rec->mDistancePerFrame * sinIdx(rec->mMoveAngle) >> 12;
    z = rec->mDistancePerFrame * -cosIdx(rec->mMoveAngle) >> 12;
    rec->mPos.x += x;
    rec->mPos.z += z;
    ground = GetFloorHeightEx(rec->mPos.x, rec->mPos.z, rec->mPos.y + 0xc800, 0, 0, NULL, &push) + 0x32000;
    if (rec->mDistancePerFrame != 0) {
        PlayWaveKeepAlive(0x1c4, MapRangeFromZero(abs(rec->mDistancePerFrame >> 12), 0x20, 0, 0x4b), 1, 0x40, 0xffff,
                          0);
    }
    if (push.y != 0x1000) {
        rec->mPos.x += push.x;
        rec->mPos.z += push.z;
        rec->mUnk_1dc -= 0x50;
    }
    if (ground >= rec->mPos.y) {
        rec->mPos.y = ground;
        rec->mPosYPerFrame = 0.0f;
    } else {
        rec->mPos.y = rec->mPos.y - rec->mPosYPerFrame;
        if (rec->mPosYPerFrame < 51200.0f) {
            rec->mPosYPerFrame += 5120.0f;
        }
    }
    if (!(flags & 2) && rec->mDistancePerFrame != 0) {
        for (i = 0; i < data_020b6ee0.mStageCount; i++) {
            StageRec *other;
            u32 dir;
            int diff;
            s32 along;
            s32 across;
            s32 *c = data_020b6ee0.mUnk_9ddc;

            if (i == index) {
                continue;
            }
            other = &gStages[i];
            ex = (other->mPos.x - rec->mPos.x) / 512;
            ez = (other->mPos.z - rec->mPos.z) / 512;
            if (LengthSquared2d64(ex, ez) >= 0x60e64) {
                continue;
            }
            dir = AngleOfVector(ex, ez);
            diff = AngleDelta(rec->mMoveAngle, dir);
            if (abs(diff) >= 0x3000) {
                continue;
            }
            along = rec->mDistancePerFrame * cosIdx(abs(diff)) / 4096;
            x = along * sinIdx((u16) (dir + 0x8000)) / 4096;
            z = along * -cosIdx((u16) (dir + 0x8000)) / 4096;
            across = rec->mDistancePerFrame * sinIdx(abs(diff)) / 4096;
            x += across * sinIdx((u16) (rec->mMoveAngle - diff)) / 4096;
            z += across * -cosIdx((u16) (rec->mMoveAngle - diff)) / 4096;
            if (c[2] < 3 && data_020b6ee0.mPanelState != 2) {
                other->mMoveAngle = AngleOfVector(-x, -z);
                other->mDistancePerFrame = Length2d(x, z) >> 1;
                if (other->mEventKey != 1) {
                    other->mDistancePerFrame = 0;
                }
            }
            PlayWaveKeepAlive(0x30, 100, 0, 0x40, 0xffff, 0);
            if ((u32) data_020b6ee0.mProgressState >= 99) {
                AddPercent(rec, -4, 0, 0);
                AddPercent(other, -4, 0, 0);
            }
            hit = 1;
        }
    }
    if (hit == 1) {
        rec->mMoveAngle = AngleOfVector(x, z);
        rec->mDistancePerFrame = Length2d(x, z);
    }
    {
        int mask = 1;
        if (FindNearestRingSegment(&rec->mPos, 400, 200, 0, 0x400, 0)) {
            s32 oz = old.z / 512;
            s32 ox = old.x / 512;
            if (Cross2d(gMapState.mCamera.mNearestSegmentStart.x, gMapState.mCamera.mNearestSegmentStart.z,
                        gMapState.mCamera.mNearestSegmentEnd.x, gMapState.mCamera.mNearestSegmentEnd.z, ox, oz) > 0) {
                if (AngleDistance(
                        AngleOfVector(gMapState.mCamera.mNearestPoint.x - ox, gMapState.mCamera.mNearestPoint.z - oz),
                        rec->mMoveAngle) < 0x4000) {
                    mask |= 0x400;
                }
            }
        }
        res = FindNearestRingSegment(&rec->mPos, 400, 200, 0, mask, 0);
    }
    if (res == 0) {
        return;
    }
    base = GetNearestSegmentAngle();
    PlayWaveKeepAlive(0x30, 100, 0, 0x40, 0xffff, 0);
    turn = Cross2d(gMapState.mCamera.mNearestSegmentStart.x, gMapState.mCamera.mNearestSegmentStart.z,
                   gMapState.mCamera.mNearestSegmentEnd.x, gMapState.mCamera.mNearestSegmentEnd.z, old.x / 512,
                   old.z / 512);
    turn = turn != 0 ? (turn >> 31) | 1 : 0;
    turn = -(turn * 0x4000);
    if (!(res & 0x10000000)) {
        angle = base - turn + AngleDelta(rec->mMoveAngle, (u16) (base + turn));
    } else {
        angle = base - turn;
    }
    if (gMapState.mCamera.mNearestRingKey & 0x10) {
        s32 cx = gMapState.mCamera.mNearestPoint.x;
        s32 cz = gMapState.mCamera.mNearestPoint.z;
        int j;
        u32 best;
        int near;

        data_020b6ee0.mFixedTargetTimer = 600;
        best = -1;
        for (j = 0; j < data_020b6ee0.mUnk_0c48; j++) {
            u32 dist = abs(data_020b6ee0.mUnk_0008[j].mPosX - cx) + abs(data_020b6ee0.mUnk_0008[j].mPosZ - cz);
            if (dist < best) {
                best = dist;
                near = j;
            }
        }
        data_020b6ee0.mFixedTargetX = data_020b6ee0.mUnk_0008[near].mPosX;
        data_020b6ee0.mFixedTargetZ = data_020b6ee0.mUnk_0008[near].mPosZ;
        data_020b6ee0.mFixedTargetObjectIndex = near;
        data_020b6ee0.mUnk_0c4c = gMapState.mCamera.mNearestRing;
    }
    if (gMapState.mCamera.mNearestRingKey & 2) {
        void *key = GetFirstLinkedRing(gMapState.mCamera.mNearestRing);
        int count = data_020b6ee0.mUnk_0c48;
        s32 dmg;

        for (i = 0; i < count; i++) {
            Unk_020b6ee8 *g = &data_020b6ee0.mUnk_0008[i];
            if (g->mKind == 0x2d && g->mUnk_180 == key) {
                break;
            }
        }
        if (i == count) {
            int j;
            u32 best = -1;
            int near;
            for (j = 0; j < data_020b6ee0.mUnk_0c48; j++) {
                Unk_020b6ee8 *g = &data_020b6ee0.mUnk_0008[j];
                s32 dx = g->mPosX - rec->mPos.x / 512;
                s32 dz = g->mPosZ - rec->mPos.z / 512;
                u32 dist = abs(dx) + abs(dz);
                if (dist < best) {
                    best = dist;
                    near = j;
                }
            }
            data_020b6ee0.mUnk_0008[near].mUnk_180 = key;
            i = near;
        }
        dmg = abs(rec->mDistancePerFrame * cosIdx((u16) AngleDistance(angle, rec->mMoveAngle)) / 4096 >> 12);
        data_020b6ee0.mUnk_0008[i].mUnk_186 -= dmg;
        if (data_020b6ee0.mUnk_0008[i].mUnk_186 < 0) {
            SetLinkedRingKeys(gMapState.mCamera.mNearestRing, 0, 0);
            data_020b6ee0.mUnk_0008[i].mModel.mSignals |= 2;
            PlayWaveKeepAlive(0xda, 100, 0, 0x40, 0xffff, 0);
        } else {
            data_020b6ee0.mUnk_0008[i].mModel.mSignals |= 1;
        }
    }
    rec->mDistancePerFrame -= rec->mDistancePerFrame / 2 < 1 ? 1 : rec->mDistancePerFrame / 2;
    rec->mPos = old;
    rec->mMoveAngle = angle;
    data_020b6ee0.mUnk_8e60.mModel.mSignals |= 1;
}
#pragma thumb on

static const u16 data_02050194[24] = {
    2528, 2628, 2739, 2827, 2935, 3061, 608,  3216, 708,  792,  909,  1048,
    1188, 1288, 1397, 1481, 1596, 1732, 1866, 1966, 2075, 2161, 2269, 2395,
};

static const u16 sMapIndices[4][6] = {
    {0, 1, 2, 3, 4, 5},
    {6, 7, 8, 9, 10, 11},
    {12, 13, 14, 15, 16, 17},
    {18, 19, 20, 21, 22, 23},
};

static const u16 sMapFileIds[24] = {
    2527, 2627, 2738, 2826, 2934, 3060, 607,  3215, 707,  791,  908,  1047,
    1187, 1287, 1396, 1480, 1595, 1731, 1865, 1965, 2074, 2160, 2268, 2394,
};

int func_020182b8(int row, int column) {
    return GetFileValue(0xd61, column + row * 2);
}

void func_020182cc(void) {
    int stage;
    Unk_020bfd40 *obj;
    StageRec *rec;
    u8 resumed;
    int row;

    stage = data_020b6ee0.mSelectedStageIndex;
    obj = &data_020b6ee0.mUnk_8e60;
    resumed = stage != -1;
    data_020b6ee0.mSelectedStageIndex = stage == -1 ? 0 : stage;
    {
        BOOL ok = 0;
        if (data_020b6ee0.mSelectedStageIndex >= 0 && data_020b6ee0.mSelectedStageIndex < STAGE_MAX) {
            ok = 1;
        }
        if (ok == 0) {
            OS_Terminate();
        }
    }
    rec = &gStages[data_020b6ee0.mSelectedStageIndex];
    if (resumed == 1) {
        if (rec->mInfo.mGrade > 12) {
            OS_Terminate();
        }
        if (rec->mInfo.mKind >= 6) {
            OS_Terminate();
        }
        row = (rec->mInfo.mGrade - 1) / 3;
        if (row > 4) {
            OS_Terminate();
        }
        data_020b6ee0.mMapIndex = sMapIndices[row][rec->mInfo.mKind];
        if (data_020b6ee0.mMapIndex >= 24) {
            OS_Terminate();
        }
    }
    ResetVramBanks();
    ReleaseTextureLayers();
    LoadImage(rec->mInfo.mStageId + 0xdb3, 2);
    SetupTextureLayer(0x10cb, 0x10ca, 0x10cc, 0, 1);
    SetupTextureLayer(0x10cb, 0x10ca, 0x10cc, 0, 2);
    SetLcdSwap(0);
    InitGxPool(0x1c7, 0x1c6, 0x180, 0x10);
    SetupMap(sMapFileIds[data_020b6ee0.mMapIndex], 0, data_02050194[data_020b6ee0.mMapIndex], 0, 0, 0);
    gMapState.mCamera.mUnk_04 = func_02015d48;
    gMapState.mCamera.mUnk_2dc.mUnk_54 = 0x640;
    gMapState.mCamera.mUnk_2dc.mViewDistance = 0x186a000;
    RequestTrack(3, 0x4c);
    MI_CpuFill8(&rec->mDraw, 0, 0x44);
    func_02031fc4(0, &rec->mPos);
    rec->mDraw.mUnk_43 = 2;
    rec->mDraw.mUnk_28 = 2 << 19;
    rec->mDistancePerFrame = 0;
    rec->mCameraAngle = 0;
    InitModel(rec, 0, 0, 0, data_0201ab04[rec->mInfo.mStageId]);
    rec->mLookAtPos = rec->mPos;
    InitModel(&data_020b6ee0.mUnk_8e60, 0, 0, 0, data_0201aba0[5]);
    data_020b6ee0.mUnk_8e60.mDraw = rec->mDraw;
    data_020b6ee0.mUnk_8e60.mDraw.mUnk_2c = -1;
    data_020b6ee0.mUnk_8e60.mDraw.mUnk_30 = -1;
    data_020b6ee0.mUnk_8e60.mDraw.mUnk_34 = -1;
    data_020b6ee0.mUnk_8e60.mDraw.mUnk_3c = 0;
    data_020b6ee0.mUnk_8e60.mDraw.mUnk_3a = data_020b6ee0.mUnk_8e60.mDraw.mUnk_3c;
    gMapState.mCamera.mEaseShift = 4;
    func_02031fc4(0, &gMapState.mCamera.mLookAt);
    gMapState.mCamera.mPos = gMapState.mCamera.mLookAt;
    gMapState.mCamera.mPos.y += 0x6d6000;
    ClearPanelState();
    SetupMapObjects();
    obj->mEndTimer = func_020182b8(rec->mInfo.mGrade - 1, 2) * 60000 / 1000;
    obj->mScore = 0;
    StartModeSlide(1, 0);
}

void func_02018558(void) {
    int stage = data_020b6ee0.mSelectedStageIndex;
    Unk_020bfd40 *obj = &data_020b6ee0.mUnk_8e60;
    StageRec *rec = &gStages[stage];
    VecFx32 pos;
    u8 slideState;
    int busy;

    if (stage >= STAGE_MAX) {
        OS_Terminate();
    }
    DrawModeScoreAndTime(data_020b6ee0.mSelectedStageIndex, obj->mScore, gModeSelectState.mScoreThreshold,
                         obj->mEndTimer, 2);
    SetStageCameraFar(stage);
    rec->mLookAtPos = rec->mPos;
    func_0200ba94(&rec->mPos, 0x70800, 0x800);
    UpdateAndDrawModel(rec, &rec->mPos, 0);
    PlayObjectSe(rec);
    ProjectToScreen(&rec->mPos, &rec->mScreenX);
    UpdateModelScript(&data_020b6ee0.mUnk_8e60.mModel);
    if (data_020b6ee0.mUnk_8e60.mModel.mEventKey != 1) {
        data_020b6ee0.mUnk_8e60.mDraw.mPos = rec->mPos;
        DrawModelFileEx(data_020b6ee0.mUnk_8e60.mModel.mId, &data_020b6ee0.mUnk_8e60.mDraw, &data_020b6ee0.mUnk_8e60,
                        0);
    } else {
        ClearAnimTracks();
    }
    pos.x = gMapState.mCamera.mLookAt.x;
    pos.z = gMapState.mCamera.mLookAt.z;
    pos.y = -0x19000;
    DrawModelFileAtPos(data_020b6ee0.mMapIndex + 0xfbe, &pos, (gSystemState.mElapsedFrames >> 1) % 100, 0, 0x3000, 0);
    DrawMap(gMapState.mCamera.mLookAt.x, gMapState.mCamera.mLookAt.z);
    UpdateFollowers(stage);
    func_02015f34();
    func_02016008(stage);
    func_020164e0(stage);
    slideState = GetModeSlideState();
    if (slideState != MODE_SLIDE_STATE_NONE) {
        busy = UpdateModeSlide(1);
        if (slideState == MODE_SLIDE_STATE_END && busy == 0) {
            StartResultScreen(GetScoreThresholdMet(), obj->mScore);
        }
        return;
    }
    UpdateStageMotion(stage, &data_020b6ee0.mSelectedStageIndex, 2);
    func_020186fc(stage);
}

void func_020186cc(void) {
    func_020102f8();
    RemoveCachedFile(sMapFileIds[data_020b6ee0.mMapIndex]);
    ResetVramBanks();
}

int func_020186f0(u8 tableRow) {
    return func_020182b8(tableRow, 3);
}

void func_020186fc(int stage) {
    Unk_020bfd40 *obj = &data_020b6ee0.mUnk_8e60;
    StageRec *rec = &gStages[stage];

    if (stage < 0 || stage >= STAGE_MAX) {
        OS_Terminate();
    }
    obj->mEndTimer--;
    func_020059cc(obj->mScore);
    if (obj->mEndTimer <= 0 || data_020b6ee0.mUnk_8ad9 == 0) {
        StartModeSlide(0, obj->mScore >= func_020182b8(rec->mInfo.mGrade - 1, 3));
    }
}

void AddStageAmountObjSlots(StageRec *rec, int kind, int amount) {
    char buf[32];
    ObjSlot *slot;

    if (kind == 0xf49) {
        FormatSignedSeconds(amount, buf);
    } else if (amount > 0) {
        sprintf(buf, "+%d", amount < 0 ? -amount : amount);
    } else {
        sprintf(buf, "-%d", amount < 0 ? -amount : amount);
    }
    slot = FindObjSlot(AddSpriteObjSlot(kind, 0, 30, 0, -15, 0, -256, 110, 1, 0));
    slot->mPos = rec->mPos;
    slot->mUsePos = 1;
    slot = FindObjSlot(AddTextObjSlot(buf, 2, data_0201a61c, 30, 0, 0, 0, -256, 100, 1, 0));
    slot->mPos = rec->mPos;
    slot->mUsePos = 1;
}

// What meeting a map object of type `kind` gives the mode:
//   - types 0xf (mUnk_183 * 100 of the object `index`), 0x14 and 5 (100 each) take time off mEndTimer and signal
//     data_020b6ee0.mUnk_8e60's model (bit 2 for 0x14, else bit 0);
//   - type 0x1e adds mUnk_181[0] * 300 time or, for a score object (mUnk_181[1] 0), mUnk_181[0] * 100 score, and
//     counts down mUnk_8ad9.
// The amount is shown at the stage (AddStageAmountObjSlots).
void AddMapObjectTimeOrScore(StageRec *rec, int kind, int index) {
    Unk_020bfd40 *obj = &data_020b6ee0.mUnk_8e60;
    int time, score;
    time = score = 0;

    switch ((u32) kind) {
        case 0xf:
            if (index < 0 || index >= data_020b6ee0.mUnk_5a54) {
                OS_Terminate();
            }
            time = -data_020b6ee0.mUnk_1014[index].mUnk_183 * 100;
            data_020b6ee0.mUnk_8e60.mModel.mSignals |= 1;
            break;
        case 0x14:
            time = -100;
            data_020b6ee0.mUnk_8e60.mModel.mSignals |= 4;
            break;
        case 5:
            time = -100;
            data_020b6ee0.mUnk_8e60.mModel.mSignals |= 1;
            break;
        case 0x1e:
            if (index < 0 || index >= data_020b6ee0.mUnk_8ad8) {
                OS_Terminate();
            }
            if (data_020b6ee0.mUnk_5a58[index].mUnk_181[1] == 0) {
                data_020b6ee0.mUnk_8ad9--;
                score = data_020b6ee0.mUnk_5a58[index].mUnk_181[0] * 100;
            } else {
                time = data_020b6ee0.mUnk_5a58[index].mUnk_181[0] * 300;
            }
            break;
    }
    if (time != 0 || score != 0) {
        ProjectToScreen(&rec->mPos, &rec->mScreenX);
    }
    if (time != 0) {
        obj->mEndTimer += time;
        AddStageAmountObjSlots(rec, 0xf49, time);
    } else if (score != 0) {
        if (score <= 0) {
            OS_Terminate();
        }
        AddCappedAt999Shifted(&obj->mScore, score, 0);
        AddStageAmountObjSlots(rec, 0xf48, score);
    }
}

void func_0201896c(void) {
    Unk_020c1454 *panel = &data_020b6ee0.mUnk_a574;
    DrawParams params;
    VecFx32 offset;
    int i;
    StageRec *rec;
    int dx;
    int dz;

    ResetVramBanks();
    MI_CpuFill8(&params, 0, 0x44);
    ReleaseTextureLayers();
    SetupTextureLayer(0x10cb, 0x10ca, 0x10cc, 0, 1);
    SetupTextureLayer(0x10cb, 0x10ca, 0x10cc, 2, 2);
    SetLcdSwap(0);
    InitGxPool(0x1c7, 0x1c6, 0x180, 0x10);
    SetupMap(0x1db, 0, 0x1dc, 0, 0, 0);
    gMapState.mCamera.mUnk_04 = func_02015d48;
    gMapState.mCamera.mUnk_2dc.mUnk_54 = 0x640;
    RequestTrackDelayed(6, 0x4c, 3);
    gMapState.mCamera.mUnk_2dc.mViewDistance = 0x186a000;
    func_02031fc4(0, &offset);
    func_02033df4(0x18);
    gMapState.mCamera.mEaseShift = 0;
    if (data_020b6ee0.mUnk_a147 == 0) {
        func_02031fc4(0, &gMapState.mCamera.mLookAtGoal);
        func_02031fc4(0, &data_020b6ee0.mUnk_a574.mUnk_0c);
        gMapState.mCamera.mPosGoal = gMapState.mCamera.mLookAtGoal;
        gMapState.mCamera.mPosGoal.y += 0xcb2000;
        gMapState.mCamera.mPosGoal.z += 0x32000;
        data_020b6ee0.mUnk_a574.mCameraEaseDivisor = 0x80;
    } else {
        data_020b6ee0.mUnk_a147 = 0;
    }
    params.mUnk_43 = 2;
    params.mUnk_28 = 2 << 19;
    for (i = 0; i < data_020b6ee0.mStageCount; i++) {
        rec = &gStages[i];
        InitModel(rec, 0, 0, 0, data_0201ab04[rec->mInfo.mStageId]);
        rec->mDraw = params;
        rec->mMoveAngle = NextRandom();
        rec->mDistancePerFrame = 0;
        rec->mCameraAngle = 0;
        if (rec->mNoStoredOffset == -1) {
            func_02031fc4(0, &rec->mPos);
            rec->mDistancePerFrame = 0x6000;
        } else {
            rec->mPos.x = offset.x + rec->mStoredOffsetX;
            rec->mPos.z = offset.z + rec->mStoredOffsetZ;
        }
        rec->mPos.y = offset.y + 0x32000;
        rec->mPos.y = GetFloorHeightEx(rec->mPos.x, rec->mPos.z, rec->mPos.y + 0xc800, 0, 0, 0, 0) + 0x32000;
        rec->mLookAtPos = rec->mPos;
    }
    if (data_020b6ee0.mSelectedStageIndex == -1) {
        data_020b6ee0.mPanelState = 0;
    } else {
        data_020b6ee0.mPanelState = 1;
        {
            BOOL ok = 0;
            if (data_020b6ee0.mSelectedStageIndex >= 0 && data_020b6ee0.mSelectedStageIndex < STAGE_MAX) {
                ok = 1;
            }
            if (ok == 0) {
                OS_Terminate();
            }
        }
        rec = &gStages[data_020b6ee0.mSelectedStageIndex];
        dx = rec->mPos.x - gMapState.mCamera.mLookAtGoal.x;
        dz = rec->mPos.z - gMapState.mCamera.mLookAtGoal.z;
        gMapState.mCamera.mLookAtGoal.x += dx;
        gMapState.mCamera.mLookAtGoal.z += dz;
        gMapState.mCamera.mPosGoal.x += dx;
        gMapState.mCamera.mPosGoal.z += dz;
    }
    FillEmptySlots();
    func_020196b4();
    func_020198f0();
    panel->mPanelButtonOffsetX = 0x28;
    data_020b6ee0.mPrevSelectedStageIndex = -1;
    data_020b6ee0.mButtonsEnabled = 1;
    InitModel(&data_020b6ee0.mUnk_9c64, 0, 0, 2, data_0201aba0[3]);
    data_020b6ee0.mUnk_a10c = -1;
    LoadStageOrTextImage(data_020b6ee0.mSelectedStageIndex, 2);
    data_020b6ee0.mUnk_a14b = 0;
    InitModel(&data_020b6ee0.mUnk_8e60, 0, 0, 0, data_0201aba0[5]);
    SetDrawParams(&data_020b6ee0.mUnk_8e60.mDraw, 0, 0, 0, 6 << 18, 0, 2, 0, 0x100);
    if (data_020b6ee0.mUnk_a14a == 1) {
        data_020b6ee0.mUnk_a14a = 0;
        data_020b6ee0.mUnk_9c64.mSignals |= 1;
        if (data_020b6ee0.mSelectedStageIndex != -1) {
            data_020b6ee0.mUnk_8e60.mModel.mSignals |= 4;
        }
    }
    if (data_020b6ee0.mSaveRequested == 1) {
        RunSaveScreen(0);
        data_020b6ee0.mSaveRequested = 0;
    }
    StopMoveObj(&data_020b6ee0.mMoveObj);
}

int CompareKeysDescending(const u16 *a, const u16 *b) {
    return *b - *a;
}

typedef struct SortEntry {
    u16 mKey;
    u16 mStage;
} SortEntry;

void func_02018cd4(void) {
    s32 *c = data_020b6ee0.mUnk_9ddc;
    DrawParams view;
    SortEntry order[64];
    DrawParams params;
    VecFx32 camPos;
    VecFx32 marker;
    s32 dist;
    Unk_020c1454 *fade = &data_020b6ee0.mUnk_a574;
    int flags;
    int i;
    int j;
    StageRec *rec;

    UpdateStageInput();
    memset(order, 0, data_020b6ee0.mStageCount * 2);
    memset(c, 0, 0xc);
    for (i = 0; i < data_020b6ee0.mStageCount; i++) {
        StageRec *rec = &gStages[i];
        if (i == data_020b6ee0.mSelectedStageIndex) {
            order[i].mKey = 0xffff;
        } else if (rec->mEventKey != 1) {
            order[i].mKey = 0xfffe;
        } else if (rec->mDistancePerFrame > 0x1000) {
            order[i].mKey = (rec->mDistancePerFrame >> 12) + 1;
        }
        order[i].mStage = i;
        if (rec->mUnk_22e == 1) {
            c[0]++;
        }
        if (rec->mDistancePerFrame != 0) {
            c[2]++;
        }
        if (rec->mDistancePerFrame != 0 || rec->mEventKey != 1) {
            c[1]++;
        }
    }
    qsort(order, data_020b6ee0.mStageCount, 4, (int (*)(const void *, const void *)) CompareKeysDescending);
    func_02031fc4(0, &camPos);
    UpdateStageCamera(data_020b6ee0.mSelectedStageIndex);
    UpdateRandomStage();
    for (j = 0; j < data_020b6ee0.mStageCount; j++) {
        int idx = order[j].mStage;
        int lim;
        rec = &gStages[idx];
        params = rec->mDraw;
        rec->mLookAtPos = params.mPos;
        rec->mVisible = IsBoxInView(&rec->mPos, 0x64000, 0x64000, 0x64000);
        if (rec->mVisible == 1 || idx == data_020b6ee0.mSelectedStageIndex) {
            ProjectToScreen(&rec->mPos, &rec->mScreenX);
            lim = (data_020b6ee0.mPanelState == 0 || data_020b6ee0.mPanelState == 1) ? 2 : 1;
            if (order[j].mKey != 0 && j < lim) {
                rec->mModelId = rec->mInfo.mStageId * 6 + 0xfd8;
            } else {
                rec->mModelId = rec->mInfo.mStageId * 6 + 0xfdb;
            }
            if (rec->mEventKey != 1 || rec->mSignals != 0 || idx == data_020b6ee0.mSelectedStageIndex) {
                if (rec->mEventKey != 1 && data_020b6ee0.mUnk_ca8 != 0 && idx == data_020b6ee0.mSelectedStageIndex) {
                    SplitAnimFrameAndAdvance(data_020b6ee0.mUnk_ca8, &data_020b6ee0.mUnk_ca6, 2, 1,
                                             &data_020b6ee0.mUnk_c94, &data_020b6ee0.mUnk_c98, &data_020b6ee0.mUnk_c9c,
                                             1);
                    ReadAnimFileNode(data_020b6ee0.mUnk_ca8, data_020b6ee0.mUnk_ca0, &data_020b6ee0.mUnk_c88,
                                     &data_020b6ee0.mUnk_c7c, data_020b6ee0.mUnk_c94, data_020b6ee0.mUnk_c94,
                                     data_020b6ee0.mUnk_c9c, 0);
                    params.mUnk_3a += data_020b6ee0.mUnk_ca0[0];
                    params.mUnk_38 += data_020b6ee0.mUnk_ca0[1];
                    params.mUnk_3c += data_020b6ee0.mUnk_ca0[2];
                    params.mPos.x += data_020b6ee0.mUnk_c88.x * data_020b6ee0.mUnk_c7c.x;
                    params.mPos.y += data_020b6ee0.mUnk_c88.y * data_020b6ee0.mUnk_c7c.y;
                    params.mPos.z += data_020b6ee0.mUnk_c88.z * data_020b6ee0.mUnk_c7c.z;
                    rec->mLookAtPos = params.mPos;
                }
                if (data_020b6ee0.mUnk_a10c != -1 && idx == data_020b6ee0.mSelectedStageIndex) {
                    SetModelFrameIndex((Model *) rec, data_020b6ee0.mUnk_a10c);
                }
                UpdateAndDrawModel(rec, &params, 0x10);
                PlayObjectSe(rec);
            } else {
                DrawModelFileEx(rec->mModelId, &params, rec, 0x10);
            }
            if (idx == data_020b6ee0.mSelectedStageIndex) {
                UpdateModelScript(&data_020b6ee0.mUnk_8e60.mModel);
                if (data_020b6ee0.mUnk_8e60.mModel.mEventKey != 1) {
                    data_020b6ee0.mUnk_8e60.mDraw.mPos = rec->mPos;
                    DrawModelFileEx(data_020b6ee0.mUnk_8e60.mModel.mId, &data_020b6ee0.mUnk_8e60.mDraw.mPos,
                                    &data_020b6ee0.mUnk_8e60.mModel, 0);
                } else {
                    ClearAnimTracks();
                }
                marker = params.mPos;
                marker.y -= 0x19000;
                func_0200ba94(&params.mPos, 0x70800, 0x800);
            }
            if (data_020b6ee0.mPanelState <= 1) {
                if (idx == data_020b6ee0.mSelectedStageIndex) {
                    DrawModelFileAtPos(0x10c8, &marker, 0, gSystemState.mElapsedFrames << 10, 0x100000, 0);
                    func_02019800(rec);
                }
                if (rec->mInSlot == 1 && (u32) data_020b6ee0.mProgressState >= 0x11) {
                    DrawStageSprite(rec, 0, 0x76, 0, 0x15, 0xc8);
                }
                DrawStageSprite(rec, 1, 0x77, 0xc, 0x11, 0xc8);
            } else if (data_020b6ee0.mPanelState == 2 || data_020b6ee0.mPanelState == 5) {
                if (idx == data_020b6ee0.mSelectedStageIndex && data_020b6ee0.mUnk_a148 == 0) {
                    func_02019800(rec);
                    if (rec->mInSlot == 1 && (u32) data_020b6ee0.mProgressState >= 0x11) {
                        DrawStageSprite(rec, 0, 0x76, 0, 0x15, 0xc8);
                    }
                    DrawStageSprite(rec, 1, rec->mInfo.mUnk_1a + 0x42, 0xc, 0x11, 0xc8);
                }
            }
        }
        flags = 4;
        if (data_020b6ee0.mButtonsEnabled == 1 || (u8) (data_020b6ee0.mPanelState + 253) <= 2) {
            flags |= 1;
        }
        UpdateStageMotion(idx, &data_020b6ee0.mSelectedStageIndex, flags);
    }
    UpdateStageMoveObj();
    func_02031fc4(0, &view.mPos);
    SetDrawParams(&view, view.mPos.x, view.mPos.y, view.mPos.z, 0x1000, 0, 0, 0, 0x100);
    UpdateAndDrawModel(&data_020b6ee0.mUnk_9c64, &view, 0);
    func_02019788();
    DrawMap(gMapState.mCamera.mLookAt.x, gMapState.mCamera.mLookAt.z);
    if (data_020b6ee0.mPrevSelectedStageIndex != data_020b6ee0.mSelectedStageIndex) {
        if (data_020b6ee0.mSelectedStageIndex == -1) {
            data_020b6ee0.mPanelState = 0;
        } else if (data_020b6ee0.mPrevSelectedStageIndex == -1) {
            data_020b6ee0.mPanelState = 1;
        }
        data_020b6ee0.mUnk_a14b = 1;
        data_020b6ee0.mPanelLevel = 0;
        data_020b6ee0.mPrevSelectedStageIndex = data_020b6ee0.mSelectedStageIndex;
        fade->mPanelButtonOffsetX = 0x28;
        StopStageMoveObjAndClearObjSlots();
    }
    UpdateTextAndStageInfo();
    if (data_020b6ee0.mUnk_a14b == 1) {
        LoadStageOrTextImage(data_020b6ee0.mSelectedStageIndex, 2);
        data_020b6ee0.mUnk_a14b = 0;
    }
    if (data_020b6ee0.mPanelState == 0 && data_020b6ee0.mButtonsEnabled == 0) {
        if (gSystemState.mTouchPressed) {
            data_020b6ee0.mUnk_a574.mCameraEaseDivisor = 4;
            data_020b6ee0.mUnk_a574.mTouchPressX = gSystemState.mTouchX;
            data_020b6ee0.mUnk_a574.mTouchPressY = gSystemState.mTouchY;
            data_020b6ee0.mUnk_a574.mDragStartPos = data_020b6ee0.mUnk_a574.mUnk_0c;
        }
        if (gSystemState.mTouchDown) {
            data_020b6ee0.mUnk_a574.mUnk_0c.x =
                data_020b6ee0.mUnk_a574.mDragStartPos.x +
                (data_020b6ee0.mUnk_a574.mTouchPressX - gSystemState.mTouchX) * (0xe << 10);
            data_020b6ee0.mUnk_a574.mUnk_0c.z =
                data_020b6ee0.mUnk_a574.mDragStartPos.z +
                (data_020b6ee0.mUnk_a574.mTouchPressY - gSystemState.mTouchY) * (0xe << 10);
            dist = Length2d((data_020b6ee0.mUnk_a574.mUnk_0c.x - camPos.x) / 512,
                            (data_020b6ee0.mUnk_a574.mUnk_0c.z - camPos.z) / 512);
            if (dist > 0x3200) {
                s32 scale = (0x3200 << 9) / dist;
                s32 dx = (data_020b6ee0.mUnk_a574.mUnk_0c.x - camPos.x) / 512;
                s32 dz = (data_020b6ee0.mUnk_a574.mUnk_0c.z - camPos.z) / 512;
                dx *= scale;
                dz *= scale;
                data_020b6ee0.mUnk_a574.mUnk_0c.x = camPos.x + dx;
                data_020b6ee0.mUnk_a574.mUnk_0c.z = camPos.z + dz;
            }
        }
    }
    UpdatePanel(data_020b6ee0.mSelectedStageIndex);
    gMapState.mCamera.mEaseShift = 4;
}

void UpdateStageInput(void) {
    u16 keys = gSystemState.mKeysPressed;

    if (keys & 0x10) {
        if (data_020b6ee0.mPanelState <= 1) {
            int next;
            if (data_020b6ee0.mSelectedStageIndex < data_020b6ee0.mStageCount - 1) {
                next = data_020b6ee0.mSelectedStageIndex + 1;
            } else {
                next = 0;
            }
            data_020b6ee0.mSelectedStageIndex = next;
        }
    }
    if (keys & 0x20) {
        if (data_020b6ee0.mPanelState <= 1) {
            int prev;
            if (data_020b6ee0.mSelectedStageIndex > 0) {
                prev = data_020b6ee0.mSelectedStageIndex - 1;
            } else {
                prev = data_020b6ee0.mStageCount - 1;
            }
            data_020b6ee0.mSelectedStageIndex = prev;
        }
    }
    if (data_020b6ee0.mPanelState == 0 || data_020b6ee0.mPanelState == 1 || data_020b6ee0.mPanelState == 2 ||
        (u8) (data_020b6ee0.mPanelState + 250) <= 2) {
        if (gSystemState.mTouchPressed == 1) {
            if (!TestButton(0x80000000, 0x5a) && !TestButton(0x80000000, 0xa) && !TestButton(0x80000000, 0xb) &&
                !TestButton(0x80000000, 0xc) && !TestButton(0x80000000, 0xd) && !TestButton(0x80000000, 0xe) &&
                !TestButton(0x80000000, 0xf) && !TestButton(0x80000000, 0x10) && !TestButton(0x80000000, 0x11) &&
                !TestButton(0x80000000, 0x21) && !TestButton(0x80000000, 0x111) && !TestButton(0x80000000, 0x112)) {
                data_020b6ee0.mButtonsEnabled = 0;
            }
        }
        if (!gSystemState.mTouchDown && !gSystemState.mTouchReleased) {
            data_020b6ee0.mButtonsEnabled = 1;
        }
    } else {
        data_020b6ee0.mButtonsEnabled = 1;
    }
}

void func_02019438(void) {
    GetNextState();
    StoreStageOffsets();
    func_020102f8();
    ResetVramBanks();
}

void StoreStageOffsets(void) {
    if (GetState() == 10) {
        VecFx32 pos;
        StageRec *rec;
        int i;

        func_02031fc4(0, &pos);
        for (i = 0; i < data_020b6ee0.mStageCount; i++) {
            rec = &gStages[i];
            rec->mStoredOffsetX = rec->mPos.x - pos.x;
            rec->mNoStoredOffset = 0;
            rec->mStoredOffsetZ = rec->mPos.z - pos.z;
        }
    }
}

// Stores in mPolygonGroupIndex the first polygon group of 10..15 that holds the stage's position (IsPosInMapPoly),
// minus 10, or 99 when none does.
void FindStagePolygonGroup(int stage) {
    StageRec *rec = &gStages[stage];
    int i;

    if (stage < 0 || stage >= STAGE_MAX) {
        OS_Terminate();
    }
    for (i = 10; i < 16; i++) {
        if (IsPosInMapPoly(i, 0, &rec->mPos) == 1) {
            if (rec->mPolygonGroupIndex != i - 10) {
                rec->mPolygonGroupIndex = i - 10;
            }
            break;
        }
    }
    if (i == 16) {
        rec->mPolygonGroupIndex = 99;
    }
}

// Called every frame of state 10; does nothing while mPanelState > 2. Picks a random stage. Unless that stage is the
// selected one, moving or out of view, or the panel is in state 2, it may start a random turn or move or change its
// mUnk_22e. About one call in 0x40 / mStageCount + 8 then updates that stage's polygon group. It adds 2 percent when
// the group equals mInfo.mKind, or -2 when it is another group and mProgressState is 99 or more. While
// mUnk_a149 < 4, one call in 512 also runs func_020198f0.
void UpdateRandomStage(void) {
    s32 *c = data_020b6ee0.mUnk_9ddc;
    int result = 0;
    int idx;
    StageRec *rec;
    int interval;

    if (data_020b6ee0.mPanelState > 2) {
        return;
    }
    idx = RandomRange(0, data_020b6ee0.mStageCount - 1);
    if (!((idx >= 0) & (idx < data_020b6ee0.mStageCount))) {
        OS_Terminate();
    }
    rec = &gStages[idx];
    if (data_020b6ee0.mUnk_a149 < 4 && !(NextRandom() & 0x1ff)) {
        func_020198f0();
    }
    if (idx != data_020b6ee0.mSelectedStageIndex && rec->mDistancePerFrame == 0 && rec->mVisible != 0 &&
        data_020b6ee0.mPanelState != 2) {
        switch ((u8) NextRandom()) {
            case 0:
                if (c[1] == 0) {
                    rec->mMoveAngle = NextRandom();
                    rec->mSignals |= 4;
                }
                break;
            case 1:
                if (c[1] == 0) {
                    rec->mMoveAngle = NextRandom();
                    rec->mDistancePerFrame = 0x4000;
                }
                break;
            case 2:
                if (rec->mInfo.mUnk_1a >= 4) {
                    OS_Terminate();
                }
                if (rec->mInfo.mPercent < 0x64) {
                    rec->mUnk_22e = c[0] < 3 ? 1 : rec->mUnk_22e;
                } else {
                    rec->mUnk_22e = 0;
                }
                break;
        }
    }
    interval = 0x40 / data_020b6ee0.mStageCount + 8;
    if (NextRandom() % interval != 0) {
        return;
    }
    FindStagePolygonGroup(idx);
    if (rec->mPolygonGroupIndex == rec->mInfo.mKind) {
        result = 2;
    } else if (rec->mPolygonGroupIndex != 99 && (u32) data_020b6ee0.mProgressState >= 0x63) {
        result = -2;
    }
    if (idx == data_020b6ee0.mSelectedStageIndex) {
        if (result == 2) {
            data_020b6ee0.mUnk_8e60.mModel.mSignals |= 1;
        } else if (result == -2) {
            data_020b6ee0.mUnk_8e60.mModel.mSignals |= 4;
        }
    }
    AddPercent(rec, result, 0, idx == data_020b6ee0.mSelectedStageIndex ? 1 : 0);
}

void func_020196b4(void) {
    int i;

    data_020b6ee0.mUnk_a149 = 0;
    for (i = 0; i < 8; i++) {
        data_020b6ee0.mUnk_9024[i].mKind = 0;
    }
}

int func_020196e0(int value) {
    int i;

    if (value == 0) {
        OS_Terminate();
    }
    for (i = 0; i < 8; i++) {
        if (data_020b6ee0.mUnk_9024[i].mKind == 0) {
            Unk_020bff04 *e = &data_020b6ee0.mUnk_9024[i];
            e->mKind = value;
            if (e->mKind == 1) {
                InitModel(&e->mModel, 0, 0, 2, data_0201aba0[4]);
            }
            data_020b6ee0.mUnk_a149++;
            return i;
        }
    }
    return -1;
}

void func_02019750(u32 index) {
    if (index >= 8) {
        OS_Terminate();
    }
    if (data_020b6ee0.mUnk_9024[index].mKind == 0) {
        OS_Terminate();
    }
    data_020b6ee0.mUnk_9024[index].mKind = 0;
    data_020b6ee0.mUnk_a149--;
}

void func_02019788(void) {
    DrawParams params;
    Unk_020bff04 *e = data_020b6ee0.mUnk_9024;
    int i;

    for (i = 0; i < 8; i++) {
        if (data_020b6ee0.mUnk_9024[i].mKind != 0 && e[i].mKind == 1) {
            SetDrawParams(&params, e[i].mPos.x, e[i].mPos.y, e[i].mPos.z, 0x100000, 0, 2, 0, 0x100);
            UpdateAndDrawModel(&e[i], &params, 0);
        }
    }
}

// Finds the mUnk_9024 entry within range of the record's position (func_02019874). If it is kind 1: removes it, starts
// the move object from the record's screen point, plays sound 0xe0 (PlayWaveKeepAlive) and sets mUnk_a570.
// UpdateStageMoveObj adds 10 percent to the record when the move ends. Called for the selected stage in panel states
// 0..2 and 5.
void func_02019800(StageRec *rec) {
    int i = func_02019874(&rec->mPos);

    if (i != -1) {
        Unk_020bff04 *e = &data_020b6ee0.mUnk_9024[i];

        if (e->mKind == 1) {
            func_02019750(i);
            StartStageMoveObj(rec, rec->mScreenX, rec->mScreenY, 0x3e, 0x5e, 1);
            PlayWaveKeepAlive(0xe0, 0x64, 0, 0x40, 0xffff, 0);
            data_020b6ee0.mUnk_a570 = 1;
        }
    }
}

// Index of the first used (kind not 0) mUnk_9024 entry with (|dx| + |dz|) / 0xc800 < 8 from pos, or -1.
int func_02019874(VecFx32 *pos) {
    Unk_020bff04 *e = data_020b6ee0.mUnk_9024;
    int i;

    for (i = 0; i < 8; i++) {
        if (data_020b6ee0.mUnk_9024[i].mKind != 0) {
            s32 dx = e[i].mPos.x - pos->x;
            s32 dz = e[i].mPos.z - pos->z;
            u8 inRange = (abs(dx) + abs(dz)) / 0xc800 < 8;

            if (inRange == 1) {
                return i;
            }
        }
    }
    return -1;
}

// Adds a kind-1 mUnk_9024 entry at a random point around map entry 0 (x and z moved by -0x3f..0x40 steps of 0xc800,
// y raised by 0x19000), unless an entry already lies near that point (func_02019874) or all 8 are used.
void func_020198f0(void) {
    VecFx32 v;
    int j;

    func_02031fc4(0, &v);
    v.x += (0x40 - (NextRandom() & 0x7f)) * 0xc800;
    v.z += (0x40 - (NextRandom() & 0x7f)) * 0xc800;
    v.y += 0x19000;
    if (func_02019874(&v) == -1) {
        j = func_020196e0(1);
        if (j != -1) {
            Unk_020bff04 *e = &data_020b6ee0.mUnk_9024[j];

            e->mPos = v;
        }
    }
}

// Eases the record's sprite offset index (0..2) toward mulX and mulY times a scale set by mPanelState (0x200 in state
// 0, 0x280 in state 1, else 0x500), and draws sprite there. Does nothing while mVisible is 0. Sprite 0 is not drawn,
// and index 1 only while mUnk_22e is set, on alternate 32-frame halves.
void DrawStageSprite(StageRec *rec, int index, int sprite, int mulX, int mulY, int prio) {
    int scale;
    StageOffset *e;

    if (rec->mVisible != 0) {
        if ((u32) index >= 3) {
            OS_Terminate();
        }
        e = &rec->mSpriteOffsets[index];
        switch (data_020b6ee0.mPanelState) {
            case 0:
                scale = 0x200;
                break;
            case 1:
                scale = 0x280;
                break;
            default:
                scale = 0x500;
                break;
        }
        if (index == 1 && rec->mUnk_22e == 0) {
            sprite = 0;
        }
        if (index == 1 && !((gSystemState.mElapsedFrames >> 5) & 1)) {
            sprite = 0;
        }
        mulX = mulX * scale;
        mulY = mulY * scale;
        EaseToward(&e->x, mulX, 8);
        EaseToward(&e->y, mulY, 8);
        if (sprite != 0) {
            DrawSprite(sprite, rec->mScreenX + e->x / 512, rec->mScreenY - e->y / 512, prio, 1);
        }
    }
}

// Starts the move object carrying the record. With mode 1 it moves from (x, y) to the record's percent position
// (PercentToX of mInfo.mPercent, 0x5e - 252); otherwise the reverse. c and d are unused.
void StartStageMoveObj(StageRec *rec, int x, int y, int c, int d, int mode) {
    MoveObj *o = &data_020b6ee0.mMoveObj;
    Vec2i pos;
    int r;

    o->mStageRec = rec;
    r = PercentToX(rec->mInfo.mPercent);
    pos.x = mode == 1 ? x : r;
    pos.y = mode == 1 ? y : 0x5e;
    pos.y += mode == 1 ? 0 : -252;
    SetupMoveObj(o, 0x1f, -1, &pos, 0x3c, 1, 1);
    SetMoveObjTrail(o, 1, 7);
    pos.x = mode == 1 ? r : x;
    pos.y = mode == 1 ? 0x5e : y;
    pos.y += mode == 1 ? -252 : 0;
    StartMoveObj(o, 0x800, &pos, 0);
}

void StopStageMoveObjAndClearObjSlots(void) {
    StopMoveObj(&data_020b6ee0.mMoveObj);
    ClearObjSlots();
}

// Runs the move object; when it has ended, adds 10 percent to the record it carries (AddPercent(rec, 10, 1, 1)).
void UpdateStageMoveObj(void) {
    MoveObj *o = &data_020b6ee0.mMoveObj;

    if (o->mPresent == 1) {
        UpdateMoveObj(o, 0);
        if (o->mPresent == 1) {
            DrawMoveObj(o);
            return;
        }
        if (o->mStageRec == 0) {
            OS_Terminate();
        }
        AddPercent(o->mStageRec, 10, 1, 1);
    }
}

void func_02019b38(void) {}

// Per-frame update of the map menu's cursor and camera. Eases the camera's look-at toward lookAt, and its position
// toward cameraPos blended toward lookAt by mBlend (eased toward mTargetBlend; 0x200 = cameraPos itself). Draws the
// cursor sprites once mDelayTimer is 0. Moves the cursor by the touch drag (within 0x320 of map entry 0), and lookAt
// and cameraPos a sixteenth of the way toward the cursor in x and z.
void UpdateMapCursorAndCamera(VecFx32 *lookAt, VecFx32 *cameraPos) {
    MapMenu *p = &data_020b6ee0.mMapMenu;
    VecFx32 v;
    s32 out1[2];
    s32 out0[2];

    func_02031fc4(0, &v);
    data_020b6ee0.mUnk_a574.mUnk_0c.x = lookAt->x;
    data_020b6ee0.mUnk_a574.mUnk_0c.y = lookAt->y;
    data_020b6ee0.mUnk_a574.mUnk_0c.z = lookAt->z;
    data_020b6ee0.mUnk_a574.mUnk_00.x = cameraPos->x;
    data_020b6ee0.mUnk_a574.mUnk_00.y = cameraPos->y;
    data_020b6ee0.mUnk_a574.mUnk_00.z = cameraPos->z;
    EaseToward(&p->mBlend, p->mTargetBlend, 0x20);
    if (p->mBlend != 0x200) {
        data_020b6ee0.mUnk_a574.mUnk_00.x =
            (data_020b6ee0.mUnk_a574.mUnk_0c.x - data_020b6ee0.mUnk_a574.mUnk_00.x) * p->mBlend / 512;
        data_020b6ee0.mUnk_a574.mUnk_00.y =
            (data_020b6ee0.mUnk_a574.mUnk_0c.y - data_020b6ee0.mUnk_a574.mUnk_00.y) * p->mBlend / 512;
        data_020b6ee0.mUnk_a574.mUnk_00.z =
            (data_020b6ee0.mUnk_a574.mUnk_0c.z - data_020b6ee0.mUnk_a574.mUnk_00.z) * p->mBlend / 512;
        data_020b6ee0.mUnk_a574.mUnk_00.x = data_020b6ee0.mUnk_a574.mUnk_0c.x - data_020b6ee0.mUnk_a574.mUnk_00.x;
        data_020b6ee0.mUnk_a574.mUnk_00.y = data_020b6ee0.mUnk_a574.mUnk_0c.y - data_020b6ee0.mUnk_a574.mUnk_00.y;
        data_020b6ee0.mUnk_a574.mUnk_00.z = data_020b6ee0.mUnk_a574.mUnk_0c.z - data_020b6ee0.mUnk_a574.mUnk_00.z;
    }
    EaseVecToward(&gMapState.mCamera.mPos, data_020b6ee0.mUnk_a574.mUnk_00, data_020b6ee0.mUnk_a574.mCameraEaseDivisor);
    EaseVecToward(&gMapState.mCamera.mLookAt, data_020b6ee0.mUnk_a574.mUnk_0c,
                  data_020b6ee0.mUnk_a574.mCameraEaseDivisor);
    func_02033b8c();
    SetupCameraView();
    ProjectToScreen(p, out1);
    if (p->mDelayTimer == 0) {
        DrawSprite(0xe25, 0x7f, out1[1], 0xdc, 1);
        DrawSprite(0xe26, out1[0], 0x5f, 0xd2, 1);
        DrawSprite(0xe29, out1[0], out1[1], 0xc8, 1);
        DrawSprite(0xe2e, out1[0], out1[1], 0xbe, 1);
    } else {
        p->mDelayTimer--;
    }
    if (gSystemState.mTouchDown == 1 && TestButton(0x40000000, 0x101) == 0 && TestButton(0x40000000, 0x102) == 0 &&
        data_020b6ee0.mButtonsEnabled == 0 && p->mNodeInfoHidden == 1) {
        s32 dx;
        s32 dy;

        ProjectToScreen(p, out0);
        dy = gSystemState.mTouchY - out0[1];
        dx = gSystemState.mTouchX - out0[0];
        p->mCursorXPerFrame = (dx << 9) / 16;
        p->mCursorZPerFrame = (dy << 9) / 16;
    }
    if (p->mCursorXPerFrame != 0 || p->mCursorZPerFrame != 0) {
        s32 d;

        p->mCursor.mPos.x += p->mCursorXPerFrame;
        p->mCursor.mPos.z += p->mCursorZPerFrame;
        p->mCursor.mPos.y =
            GetFloorHeight(p->mCursor.mPos.x, p->mCursor.mPos.z, p->mCursor.mPos.y + 0x32000, 0, 0, 0) + 0x1900;
        d = Length2d((p->mCursor.mPos.x - v.x) / 512, (p->mCursor.mPos.z - v.z) / 512);
        if (d > 0x320) {
            s32 r = 0x64000 / d;
            s32 dx = (p->mCursor.mPos.x - v.x) / 512;
            s32 dz = (p->mCursor.mPos.z - v.z) / 512;

            dx *= r;
            dz *= r;
            p->mCursor.mPos.x = v.x + dx;
            p->mCursor.mPos.z = v.z + dz;
        }
    }
    EaseToward(&p->mCursorXPerFrame, 0, 0x10);
    EaseToward(&p->mCursorZPerFrame, 0, 0x10);
    {
        s32 d = (p->mCursor.mPos.z - lookAt->z) / 16;

        cameraPos->z += d;
        lookAt->z += d;
    }
    {
        s32 d = (p->mCursor.mPos.x - lookAt->x) / 16;

        cameraPos->x += d;
        lookAt->x += d;
    }
}

void SetupMapMenu(void) {
    MapMenu *p = &data_020b6ee0.mMapMenu;

    ResetVramBanks();
    ReleaseTextureLayers();
    SetupTextureLayer(0x10cb, 0x10ca, 0x10cc, 0x82, 2);
    SetupTextureLayer(0x10cb, 0x10ca, 0x10cc, 0, 1);
    LoadImage(0xda3, 2);
    SetLcdSwap(0);
    func_0200bdcc(2, 0x10);
    InitGxPool(0x1c7, 0x1c6, 0x100, 0x10);
    SetupMap(0xdda, 0, 0xddb, 0, 0, 0);
    gMapState.mCamera.mUnk_04 = func_02019b38;
    gMapState.mCamera.mUnk_2dc.mUnk_54 = 0x640;
    gMapState.mCamera.mUnk_2dc.mViewDistance = 0x9c4000;
    func_02031fc4(0, &gMapMenuLookAt);
    func_02031fc4(1, &gMapMenuCameraPos);
    func_02031fc4(0, &gMapState.mCamera.mPos);
    func_02031fc4(0, &gMapState.mCamera.mLookAt);
    gMapState.mCamera.mPos.z += 0x352000;
    gMapState.mCamera.mLookAt.y += 0x32000;
    data_020b6ee0.mUnk_a574.mCameraEaseDivisor = 0x80;
    func_0200bf38(8);
    memset(p, 0, 0x44);
    p->mCursor.mUnk_43 = 2;
    p->mCursor.mUnk_28 = 0x8000;
    func_02031fc4(data_020b6ee0.mReachedNodeIndex + MAP_NODE_POS_ID, &p->mCursor.mPos);
    p->mCursor.mPos.y += 0x1900;
    p->mCursorSnapped = 0;
    p->mNodeInfoHidden = 1;
    data_020b6ee0.mButtonsEnabled = 1;
    p->mTargetBlend = 0x200;
    p->mBlend = p->mTargetBlend;
    data_020b6ee0.mUnk_a574.mMapButtonOffsetX = 0x28;
    p->mCursorXPerFrame = 0;
    p->mCursorZPerFrame = 0;
    p->mDelayTimer = 0xb4;
    StartModel(&p->mReachedNodeModel, 0xde5, 0xde4, 2, 0);
    if ((u32) data_020b6ee0.mProgressState > 0xc) {
        PlayWaveKeepAlive(0x59, 0x64, 0, 0x40, 0xffff, 0);
    }
}
