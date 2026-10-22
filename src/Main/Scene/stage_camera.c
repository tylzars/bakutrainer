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
#include <Main/Unknown/game_util.h>
#include <Main/Scene/stage_data.h>
#include <Main/Scene/stage_camera.h>
#include <Main/Scene/stage_map_menu.h>
#include <Main/Unknown/move_object.h>
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
    StageRec *rec = &data_020b1990[stage];
    int distancePerFrame = rec->mDistancePerFrame >> 4;
    int height;

    if (stage < 0 || stage >= STAGE_MAX) {
        OS_Terminate();
    }
    data_020c3e44.mUnk_2024.mUnk_8c.x = rec->mUnk_158.x;
    data_020c3e44.mUnk_2024.mUnk_8c.z = rec->mUnk_158.z;
    data_020c3e44.mUnk_2024.mUnk_8c.y = rec->mUnk_158.y;
    data_020c3e44.mUnk_2024.mUnk_80.x = rec->mUnk_1a0.x - 400 * FX_SinCosTable_[(rec->mUnk_224 >> 4) * 2];
    data_020c3e44.mUnk_2024.mUnk_80.z = rec->mUnk_1a0.z + 400 * FX_SinCosTable_[(rec->mUnk_224 >> 4) * 2 + 1];
    height = rec->mUnk_1a0.y + MapRangeFromZero(distancePerFrame, 0xc00, 0x320000, 0x4b0000);
    data_020c3e44.mUnk_2024.mUnk_80.y = height;
    if (height > 0x5dc000) {
        data_020c3e44.mUnk_2024.mUnk_80.y = 0x5dc000;
    }
    func_02033488();
}

void SetStageCameraNear(int stage) {
    StageRec *rec = &data_020b1990[stage];

    if (stage < 0 || stage >= STAGE_MAX) {
        OS_Terminate();
    }
    data_020c3e44.mUnk_2024.mUnk_8c.x = rec->mUnk_158.x;
    data_020c3e44.mUnk_2024.mUnk_8c.z = rec->mUnk_158.z;
    data_020c3e44.mUnk_2024.mUnk_8c.y = rec->mUnk_158.y;
    data_020c3e44.mUnk_2024.mUnk_80.x = rec->mUnk_1a0.x - 300 * FX_SinCosTable_[(rec->mUnk_224 >> 4) * 2];
    data_020c3e44.mUnk_2024.mUnk_80.z = rec->mUnk_1a0.z + 300 * FX_SinCosTable_[(rec->mUnk_224 >> 4) * 2 + 1];
    data_020c3e44.mUnk_2024.mUnk_80.y = rec->mUnk_1a0.y + 0x64000;
    func_02033488();
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
    func_0200b960(&data_020c3e44.mUnk_2024.mUnk_8c, data_020b6ee0.mUnk_a574.mUnk_0c,
                  data_020b6ee0.mUnk_a574.mCameraEaseDivisor);
    func_0200b960(&data_020c3e44.mUnk_2024.mUnk_80, data_020b6ee0.mUnk_a574.mUnk_00,
                  data_020b6ee0.mUnk_a574.mCameraEaseDivisor);
    func_02033488();
}

void func_02015f34(void) {
    int i;
    DrawParams params;
    char text[12];
    s32 pos[2];
    Unk_020b6ee8 *obj;

    for (i = 0; i < data_020b6ee0.mUnk_0c48; i++) {
        obj = &data_020b6ee0.mUnk_0008[i];
        if (obj->mModel.mUnk_170 != 0) {
            func_02023f58(&params, obj->mPosX << 9, 0, obj->mPosZ << 9, 0x1000, obj->mUnk_185 << 8, 0, 0xff,
                          0x100);
            if (func_02031d0c(&params.mPos, 0x12c000, 0x190000, 0x190000) == 1) {
                func_0202733c(&obj->mModel, &params, 0);
                if (obj->mKind == 10) {
                    sprintf(text, "%d", obj->mUnk_186);
                    func_02025db4(&params, pos);
                    func_020215fc(pos[0], pos[1], data_0201a61c, text, 2, 0x82, 0x10, 1);
                }
            }
        }
    }
}

void func_02016008(int stage) {
    StageRec *rec = &data_020b1990[stage];
    s32 px = rec->mUnk_1a0.x / 512;
    s32 pz = rec->mUnk_1a0.z / 512;
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
                f->mModel.mUnk_134 |= 1;
                if (f->mNearFrames > (f->mUnk_187 + 1) * 6) {
                    rec->mUnk_1d8 += 0x8000;
                    rec->mDistancePerFrame += 0x2000;
                    f->mNearFrames = 0;
                    func_02018840(rec, 0x14, i);
                    f->mModel.mUnk_134 |= 2;
                    func_0202f110(0x2b, 0x64, 0, 0x40, 0xffff, 0);
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
                rec->mUnk_1a0 = rec->mUnk_210;
                rec->mUnk_1d8 += 0x8000;
                rec->mDistancePerFrame = 0x2000;
                func_02018840(rec, 0xf, i);
                break;
            case 0x19: {
                int q = 0x32000 / d;
                int ax = px - f->mPosX;
                int az = pz - f->mPosZ;
                ax *= q;
                az *= q;
                rec->mUnk_1a0.x = ax + (f->mPosX << 9);
                rec->mUnk_1a0.z = az + (f->mPosZ << 9);
                rec->mUnk_1d8 = AngleOfVector(rec->mUnk_1a0.x / 512 - f->mPosX, rec->mUnk_1a0.z / 512 - f->mPosZ);
                rec->mDistancePerFrame += 0x4000;
                func_0202f110(0x28, 0x64, 0, 0x40, 0xffff, 0);
                break;
            }
            case 0x23:
                rec->mUnk_1d8 = ((f->mUnk_184 + 1) & 3) << 14;
                rec->mDistancePerFrame = 0x14000;
                func_0202f110(1, 0x64, 0, 0x40, 0xffff, 0);
                break;
            case 0x28:
                moved = 1;
                if (rec->mUnk_22c == 0) {
                    u8 key = f->mUnk_185;
                    Unk_020b7ef4 *p = data_020b6ee0.mUnk_1014;
                    int j;

                    SetFaded(moved);
                    func_0202f110(0x59, 0x64, 0, 0x40, 0xffff, 0);
                    rec->mDistancePerFrame = 0;
                    for (j = 0; j < data_020b6ee0.mUnk_5a54; j++, p++) {
                        if (p->mKind == 0x28 && p->mUnk_186 == key) {
                            rec->mUnk_1a0.x = p->mPosX << 9;
                            rec->mUnk_1a0.z = p->mPosZ << 9;
                            break;
                        }
                    }
                }
                break;
        }
    } else {
        rec->mUnk_210 = rec->mUnk_1a0;
    }
    rec->mUnk_22c = moved;
    b = data_020b6ee0.mUnk_1014;
    i = 0;
    while (i < data_020b6ee0.mUnk_5a54) {
        pos.x = b->mPosX << 9;
        pos.z = b->mPosZ << 9;
        pos.y = 0;
        if (func_02031d0c(&pos, 0x190000, 0x190000, 0x190000) == 1) {
            switch (b->mKind) {
                case 0x19:
                    if (b == f) {
                        b->mModel.mUnk_134 |= 1;
                    }
                    func_02023f58(&params, pos.x, pos.y, pos.z, 0x1000, 0, 0, 0xff, 0x100);
                    func_0202733c(&b->mModel, &params, 0);
                    break;
                case 0x14:
                    pos.y = 0;
                    func_02023f58(&params, pos.x, pos.y, pos.z, 0x1000, (u16) (((b->mUnk_184 + 1) & 3) << 14), 0, 0xff,
                                  0x100);
                    func_0202733c(&b->mModel, &params, 0);
                    break;
                case 0x28:
                    pos.y = 0;
                    func_02023f58(&params, pos.x, pos.y, pos.z, 0x1000, (u16) (((b->mUnk_184 + 1) & 3) << 14), 0, 0xff,
                                  0x100);
                    func_0202733c(&b->mModel, &params, 0);
                    break;
                case 0xf:
                    pos.y = 0;
                    func_02023f58(&params, pos.x, pos.y, pos.z, 0x1000, (u16) (((b->mUnk_184 + 1) & 3) << 14), 0, 0xff,
                                  0x100);
                    func_020248e8(0xc87, &params, 0);
                    break;
                case 0x23:
                    pos.y = 0;
                    func_02023f58(&params, pos.x, pos.y, pos.z, 0x1000, (u16) (b->mUnk_184 << 14), 0, 0xff, 0x100);
                    func_020248e8(0xc8a, &params, 0);
                    break;
            }
        }
        b++;
        i++;
    }
}

void func_020164e0(int stage) {
    StageRec *rec = &data_020b1990[stage];
    s32 px = rec->mUnk_1a0.x / 512;
    s32 pz = rec->mUnk_1a0.z / 512;
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
    func_02023f58(&params, 0, 0, 0, 0x1000, 0, 2, 0xff, 0);
    i = 0;
    pos.y = 0;
    found = -1;
    while (i < data_020b6ee0.mUnk_8ad8) {
        if (obj->mPresent == 1) {
            pos.x = obj->mPosX << 9;
            pos.z = obj->mPosZ << 9;
            if (func_02031d0c(&pos, 0x190000, 0x190000, 0x190000) == 1) {
                params.mPos.x = pos.x;
                params.mPos.z = pos.z;
                if (obj->mKind == 0x1e) {
                    func_0202733c(&obj->mModel, &params, 0);
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
            func_02018840(rec, 0x1e, found);
            obj->mPresent = 0;
            func_0202f110(0xe0, 0x64, 0, 0x40, 0xffff, 0);
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
    q = func_02031eb8(0x5514);
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
            data_020b6ee0.mUnk_1014[n7].mUnk_184 = (u32) flags >> 6;
            func_020271ec(&data_020b6ee0.mUnk_1014[n7].mModel, 0, 0, 0, data_0201aba0[6]);
            n7++;
        } else if (type == 0x19) {
            data_020b6ee0.mUnk_1014[n7].mKind = type;
            data_020b6ee0.mUnk_1014[n7].mPosX = pos.x / 512;
            data_020b6ee0.mUnk_1014[n7].mPosZ = pos.z / 512;
            func_020271ec(&data_020b6ee0.mUnk_1014[n7].mModel, 0, 0, 0, data_0201aba0[0]);
            n7++;
        } else if (type == 0x1e) {
            data_020b6ee0.mUnk_5a58[n30].mKind = type;
            data_020b6ee0.mUnk_5a58[n30].mPosX = pos.x / 512;
            data_020b6ee0.mUnk_5a58[n30].mPosZ = pos.z / 512;
            data_020b6ee0.mUnk_5a58[n30].mPresent = 1;
            data_020b6ee0.mUnk_5a58[n30].mUnk_181[1] = ReadBits(q + 2, 0, 0);
            data_020b6ee0.mUnk_5a58[n30].mUnk_181[0] = ReadBits(q + 2, 1, 7);
            if (data_020b6ee0.mUnk_5a58[n30].mUnk_181[1] == 0) {
                func_020271ec(&data_020b6ee0.mUnk_5a58[n30].mModel, 0xc89, 0xc88, 2, 0);
                data_020b6ee0.mUnk_8ad9++;
            } else {
                func_020271ec(&data_020b6ee0.mUnk_5a58[n30].mModel, 0xc8c, 0xc8b, 2, 0);
            }
            n30++;
        } else if (type == 0x23) {
            data_020b6ee0.mUnk_1014[n7].mKind = type;
            data_020b6ee0.mUnk_1014[n7].mPosX = pos.x / 512;
            data_020b6ee0.mUnk_1014[n7].mPosZ = pos.z / 512;
            data_020b6ee0.mUnk_1014[n7].mUnk_184 = (u32) flags >> 6;
            n7++;
        } else if (type == 0x28) {
            data_020b6ee0.mUnk_1014[n7].mKind = type;
            data_020b6ee0.mUnk_1014[n7].mPosX = pos.x / 512;
            data_020b6ee0.mUnk_1014[n7].mPosZ = pos.z / 512;
            data_020b6ee0.mUnk_1014[n7].mUnk_185 = ReadBits(q + 2, 0, 3);
            data_020b6ee0.mUnk_1014[n7].mUnk_186 = ReadBits(q + 2, 4, 7);
            func_020271ec(&data_020b6ee0.mUnk_1014[n7].mModel, 0xc8e, 0xc8d, 2, 0);
            n7++;
        } else if (type == 5) {
            data_020b6ee0.mFollowerEntries[n5].mPos = pos;
            data_020b6ee0.mFollowerEntries[n5].mUnk_18 = ReadBits(q + 2, 0, 0);
            data_020b6ee0.mFollowerEntries[n5].mPresent = 1;
            n5++;
        } else if (type == 0xa || type == 0x2d) {
            data_020b6ee0.mUnk_0008[n34].mPosX = pos.x / 512;
            data_020b6ee0.mUnk_0008[n34].mPosZ = pos.z / 512;
            data_020b6ee0.mUnk_0008[n34].mUnk_185 = flags;
            data_020b6ee0.mUnk_0008[n34].mKind = type;
            if (type == 0xa) {
                data_020b6ee0.mUnk_0008[n34].mUnk_186 = ReadBits(q + 2, 0, 2);
                func_020271ec(&data_020b6ee0.mUnk_0008[n34].mModel, 0, 0, 0, data_0201aba0[1]);
            } else if (type == 0x2d) {
                data_020b6ee0.mUnk_0008[n34].mUnk_187 = ReadBits(q + 2, 0, 2);
                data_020b6ee0.mUnk_0008[n34].mUnk_186 = 0x20;
                func_020271ec(&data_020b6ee0.mUnk_0008[n34].mModel, 0, 0, 0, data_0201aba0[2]);
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
    StageRec *rec = &data_020b1990[stage];
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
            int dx = abs(data_020b6ee0.mFollowerEntries[i].mPos.x - rec->mUnk_1a0.x);
            int dz = abs(data_020b6ee0.mFollowerEntries[i].mPos.z - rec->mUnk_1a0.z);
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
        gFollowerGroup.mFollowers[n].mPos.x = rec->mUnk_1a0.x / 512;
        gFollowerGroup.mFollowers[n].mPos.y = rec->mUnk_1a0.y / 512;
        gFollowerGroup.mFollowers[n].mPos.z = rec->mUnk_1a0.z / 512;
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
                func_02025db4(&data_020b6ee0.mFollowerEntries[i], data_020b6ee0.mFollowerEntries[i].mScreenPos);
            }
        }
    }
    for (i = 0; i < data_020b6ee0.mFollowerEntryCount; i++) {
        if (data_020b6ee0.mFollowerEntries[i].mInRange == 1) {
            func_0200ba94(&data_020b6ee0.mFollowerEntries[i].mPos, 0x38400, 0x400);
            func_0202cf08(data_020b6ee0.mFollowerEntries[i].mUnk_18 == 0 ? 0xe24 : 0xe23,
                          data_020b6ee0.mFollowerEntries[i].mScreenPos[0],
                          data_020b6ee0.mFollowerEntries[i].mScreenPos[1], 0, 1);
        }
    }
}

// One follower's step, run by MoveFollowers (called from UpdateFollowers) for each follower whose
// mMaxDistancePerFrame is not 0: the ring test (a hit puts it back at mPrevPos), the sector distances to the other
// followers and the speed from the follower found ahead. angle is unused.
void UpdateFollower(FollowerGroup *followers, int index, u16 angle) {
    int bestIdx = 0;
    Follower *f = &followers->mFollowers[index];
    StageRec *rec = &data_020b1990[data_020b6ee0.mSelectedStageIndex];
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
        if (func_02031824(&pos, f->mDiameter, 0x6e, 0, 0x13, 0)) {
            if ((data_020c3e44.mUnk_2024.mUnk_3a & 0x10) && data_020b6ee0.mFixedTargetTimer != 0 &&
                data_020b6ee0.mFollowerEntries[f->mFollowerEntryIndex].mUnk_18 == 0 &&
                data_020b6ee0.mUnk_0c4c == data_020c3e44.mUnk_2024.mUnk_10 &&
                data_020b6ee0.mUnk_0008[data_020b6ee0.mFixedTargetObjectIndex].mUnk_186 > 0) {
                data_020b6ee0.mUnk_0008[data_020b6ee0.mFixedTargetObjectIndex].mModel.mUnk_134 |= 1;
                data_020b6ee0.mUnk_0008[data_020b6ee0.mFixedTargetObjectIndex].mUnk_186--;
                if (data_020b6ee0.mUnk_0008[data_020b6ee0.mFixedTargetObjectIndex].mUnk_186 == 0) {
                    func_02031c98(data_020b6ee0.mUnk_0c4c, 0, 0);
                    data_020b6ee0.mUnk_0008[data_020b6ee0.mFixedTargetObjectIndex].mModel.mUnk_134 |= 2;
                    func_0202f110(0xdb, 0x64, 0, 0x40, 0xffff, 0);
                } else {
                    func_0202f110(0xdd, 0x64, 0, 0x40, 0xffff, 0);
                }
                data_020b6ee0.mFollowerEntries[f->mFollowerEntryIndex].mPresent = 0;
                data_020b6ee0.mFollowerEntries[f->mFollowerEntryIndex].mInRange = 0;
            }
            {
                u32 cam = func_0203129c();
                int cross = Cross2d(data_020c3e44.mUnk_2024.mUnk_14.x, data_020c3e44.mUnk_2024.mUnk_14.z,
                                    data_020c3e44.mUnk_2024.mUnk_20.x, data_020c3e44.mUnk_2024.mUnk_20.z, f->mPos.x,
                                    f->mPos.z);
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
            if (data_020b6ee0.mFollowerEntries[f->mFollowerEntryIndex].mUnk_18 == 1 &&
                bestIdx == followers->mFollowerCount && gFollowerGroup.mTargetFixed == 0) {
                data_020b6ee0.mFollowerEntries[f->mFollowerEntryIndex].mPresent = 0;
                data_020b6ee0.mFollowerEntries[f->mFollowerEntryIndex].mInRange = 0;
                func_02018840(rec, 5, -1);
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
    StageRec *rec = &data_020b1990[stage];

    if (gSystemState.mTouchReleased && rec->mVisible == 1) {
        if ((int) Length2d(gSystemState.mTouchX - rec->mScreenX, gSystemState.mTouchY - rec->mScreenY) < 0x10) {
            if ((int) Length2d(gSystemState.mTouchX - data_020b6ee0.mUnk_a574.mTouchPressX,
                               gSystemState.mTouchY - data_020b6ee0.mUnk_a574.mTouchPressY) < 6) {
                *out = stage;
                func_0202f110(0x167, 0x64, 0, 0x40, 0xffff, 0);
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

// Per-frame step of stage index: clamps mDistancePerFrame to -0x20000..0x20000 and, on a touch press, records the
// touch point in mUnk_a574. While the stage's mUnk_142 is not 1 or mPanelState is 3, 4, 6, 7 or 8 it then only turns
// the selected stage by the decaying spin mUnk_a8e8 or lets a tap select another stage (panel states 0..2). The touch
// drag that would set the spin here needs panel state 3 or 4 with flags bit 0 clear, which no call reaches in this
// build (state 10 sets bit 0 in panel states 3 to 5, and state 4 keeps panel state 0). Otherwise it moves the stage
// along mUnk_1d8, keeps it on the ground and handles its hits with other stages and the map (func_02031824).
void UpdateStageMotion(int index, int *sel, int flags) {
    StageRec *rec = &data_020b1990[index];
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
    if (rec->mUnk_142 != 1 || data_020b6ee0.mPanelState == 3 || data_020b6ee0.mPanelState == 4 ||
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
            rec->mUnk_1d8 -= data_020b6ee0.mUnk_a8e8;
            data_020b6ee0.mUnk_a8e8 = data_020b6ee0.mUnk_a8e8 * 31 / 32;
            return;
        }
        if (data_020b6ee0.mPanelState > 2) {
            return;
        }
        if (SelectStageOnTap(index, sel) != 1) {
            return;
        }
        rec->mUnk_134 |= 2;
        return;
    }
    if (!(flags & 1)) {
        if (index == *sel) {
            if (gSystemState.mTouchReleased) {
                int n = data_020c1d60.mUnk_800.mUnk_100.mUnk_10 - 1;
                if (n < 30 && n > 1) {
                    s32 dx =
                        (data_020c1d60.mUnk_800.mUnk_000[n].mUnk_0 - data_020c1d60.mUnk_800.mUnk_000[0].mUnk_0) / 4;
                    s32 dy =
                        (data_020c1d60.mUnk_800.mUnk_000[n].mUnk_1 - data_020c1d60.mUnk_800.mUnk_000[0].mUnk_1) / 4;
                    int len = Length2d(dx, dy);
                    if (len > 8) {
                        dx = (dx << 3) / len;
                        dy = (dy << 3) / len;
                        Length2d(dx, dy);
                    }
                    dx = (dx << 12) + rec->mDistancePerFrame * sinIdx(rec->mUnk_1d8) / 4096;
                    dy = (dy << 12) + rec->mDistancePerFrame * -cosIdx(rec->mUnk_1d8) / 4096;
                    rec->mUnk_1d8 = AngleOfVector(dx, dy);
                    len = Length2d(dx, dy);
                    rec->mDistancePerFrame = len < 0x14000 ? len : 0x14000;
                }
            } else if (gSystemState.mTouchDown) {
                int n = data_020c1d60.mUnk_800.mUnk_100.mUnk_10 - 1;
                if (n > 4) {
                    if ((int) Length2d(data_020c1d60.mUnk_800.mUnk_000[n].mUnk_0 -
                                           (data_020c1d60.mUnk_800.mUnk_000 + n - 4)->mUnk_0,
                                       data_020c1d60.mUnk_800.mUnk_000[n].mUnk_1 -
                                           (data_020c1d60.mUnk_800.mUnk_000 + n - 4)->mUnk_1) < 4) {
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
    {
        k = rec->mUnk_1dc;
        k = k + (spd = spd * 0xc0 >> 12);
        old = rec->mUnk_1a0;
        rec->mUnk_1dc = k;
        x = rec->mDistancePerFrame * sinIdx(rec->mUnk_1d8) >> 12;
        z = rec->mDistancePerFrame * -cosIdx(rec->mUnk_1d8) >> 12;
        rec->mUnk_1a0.x += x;
        rec->mUnk_1a0.z += z;
        ground = func_02030830(rec->mUnk_1a0.x, rec->mUnk_1a0.z, rec->mUnk_1a0.y + 0xc800, 0, 0, NULL, &push) + 0x32000;
    }
    if (rec->mDistancePerFrame != 0) {
        func_0202f110(0x1c4, MapRangeFromZero(abs(rec->mDistancePerFrame >> 12), 0x20, 0, 0x4b), 1, 0x40, 0xffff, 0);
    }
    if (push.y != 0x1000) {
        rec->mUnk_1a0.x += push.x;
        rec->mUnk_1a0.z += push.z;
        rec->mUnk_1dc -= 0x50;
    }
    if (ground >= rec->mUnk_1a0.y) {
        rec->mUnk_1a0.y = ground;
        rec->mUnk_21c = 0.0f;
    } else {
        rec->mUnk_1a0.y = rec->mUnk_1a0.y - rec->mUnk_21c;
        if (rec->mUnk_21c < 51200.0f) {
            rec->mUnk_21c += 5120.0f;
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
            other = &data_020b1990[i];
            ex = (other->mUnk_1a0.x - rec->mUnk_1a0.x) / 512;
            ez = (other->mUnk_1a0.z - rec->mUnk_1a0.z) / 512;
            if (LengthSquared2d64(ex, ez) >= 0x60e64) {
                continue;
            }
            dir = AngleOfVector(ex, ez);
            diff = AngleDelta(rec->mUnk_1d8, dir);
            if (abs(diff) >= 0x3000) {
                continue;
            }
            along = rec->mDistancePerFrame * cosIdx(abs(diff)) / 4096;
            x = along * sinIdx((u16) (dir + 0x8000)) / 4096;
            z = along * -cosIdx((u16) (dir + 0x8000)) / 4096;
            across = rec->mDistancePerFrame * sinIdx(abs(diff)) / 4096;
            x += across * sinIdx((u16) (rec->mUnk_1d8 - diff)) / 4096;
            z += across * -cosIdx((u16) (rec->mUnk_1d8 - diff)) / 4096;
            if (c[2] < 3 && data_020b6ee0.mPanelState != 2) {
                other->mUnk_1d8 = AngleOfVector(-x, -z);
                other->mDistancePerFrame = Length2d(x, z) >> 1;
                if (other->mUnk_142 != 1) {
                    other->mDistancePerFrame = 0;
                }
            }
            func_0202f110(0x30, 100, 0, 0x40, 0xffff, 0);
            if ((u32) data_020b6ee0.mProgressState >= 99) {
                AddPercent(rec, -4, 0, 0);
                AddPercent(other, -4, 0, 0);
            }
            hit = 1;
        }
    }
    if (hit == 1) {
        rec->mUnk_1d8 = AngleOfVector(x, z);
        rec->mDistancePerFrame = Length2d(x, z);
    }
    {
        int mask = 1;
        if (func_02031824(&rec->mUnk_1a0, 400, 200, 0, 0x400, 0)) {
            s32 oz = old.z / 512;
            s32 ox = old.x / 512;
            if (Cross2d(data_020c3e44.mUnk_2024.mUnk_14.x, data_020c3e44.mUnk_2024.mUnk_14.z,
                        data_020c3e44.mUnk_2024.mUnk_20.x, data_020c3e44.mUnk_2024.mUnk_20.z, ox, oz) > 0) {
                if (AngleDistance(
                        AngleOfVector(data_020c3e44.mUnk_2024.mUnk_2c.x - ox, data_020c3e44.mUnk_2024.mUnk_2c.z - oz),
                        rec->mUnk_1d8) < 0x4000) {
                    mask |= 0x400;
                }
            }
        }
        res = func_02031824(&rec->mUnk_1a0, 400, 200, 0, mask, 0);
    }
    if (res == 0) {
        return;
    }
    base = func_0203129c();
    func_0202f110(0x30, 100, 0, 0x40, 0xffff, 0);
    turn = Cross2d(data_020c3e44.mUnk_2024.mUnk_14.x, data_020c3e44.mUnk_2024.mUnk_14.z,
                   data_020c3e44.mUnk_2024.mUnk_20.x, data_020c3e44.mUnk_2024.mUnk_20.z, old.x / 512, old.z / 512);
    turn = turn != 0 ? (turn >> 31) | 1 : 0;
    turn = -(turn * 0x4000);
    if (!(res & 0x10000000)) {
        angle = base - turn + AngleDelta(rec->mUnk_1d8, (u16) (base + turn));
    } else {
        angle = base - turn;
    }
    if (data_020c3e44.mUnk_2024.mUnk_3a & 0x10) {
        s32 cx = data_020c3e44.mUnk_2024.mUnk_2c.x;
        s32 cz = data_020c3e44.mUnk_2024.mUnk_2c.z;
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
        data_020b6ee0.mUnk_0c4c = data_020c3e44.mUnk_2024.mUnk_10;
    }
    if (data_020c3e44.mUnk_2024.mUnk_3a & 2) {
        void *key = func_02031c70(data_020c3e44.mUnk_2024.mUnk_10);
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
                s32 dx = g->mPosX - rec->mUnk_1a0.x / 512;
                s32 dz = g->mPosZ - rec->mUnk_1a0.z / 512;
                u32 dist = abs(dx) + abs(dz);
                if (dist < best) {
                    best = dist;
                    near = j;
                }
            }
            data_020b6ee0.mUnk_0008[near].mUnk_180 = key;
            i = near;
        }
        dmg = abs(rec->mDistancePerFrame * cosIdx((u16) AngleDistance(angle, rec->mUnk_1d8)) / 4096 >> 12);
        data_020b6ee0.mUnk_0008[i].mUnk_186 -= dmg;
        if (data_020b6ee0.mUnk_0008[i].mUnk_186 < 0) {
            func_02031c98(data_020c3e44.mUnk_2024.mUnk_10, 0, 0);
            data_020b6ee0.mUnk_0008[i].mModel.mUnk_134 |= 2;
            func_0202f110(0xda, 100, 0, 0x40, 0xffff, 0);
        } else {
            data_020b6ee0.mUnk_0008[i].mModel.mUnk_134 |= 1;
        }
    }
    rec->mDistancePerFrame -= rec->mDistancePerFrame / 2 < 1 ? 1 : rec->mDistancePerFrame / 2;
    rec->mUnk_1a0 = old;
    rec->mUnk_1d8 = angle;
    data_020b6ee0.mUnk_8e60.mModel.mUnk_134 |= 1;
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
    return func_02021e44(0xd61, column + row * 2);
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
    rec = &data_020b1990[data_020b6ee0.mSelectedStageIndex];
    if (resumed == 1) {
        if (rec->mInfo.mUnk_11 > 12) {
            OS_Terminate();
        }
        if (rec->mInfo.mUnk_10 >= 6) {
            OS_Terminate();
        }
        row = (rec->mInfo.mUnk_11 - 1) / 3;
        if (row > 4) {
            OS_Terminate();
        }
        data_020b6ee0.mMapIndex = sMapIndices[row][rec->mInfo.mUnk_10];
        if (data_020b6ee0.mMapIndex >= 24) {
            OS_Terminate();
        }
    }
    ResetVramBanks();
    func_0202c1c4();
    LoadImage(rec->mInfo.mUnk_13 + 0xdb3, 2);
    func_0202bdd8(0x10cb, 0x10ca, 0x10cc, 0, 1);
    func_0202bdd8(0x10cb, 0x10ca, 0x10cc, 0, 2);
    SetLcdSwap(0);
    func_02021fb0(0x1c7, 0x1c6, 0x180, 0x10);
    func_020325b0(sMapFileIds[data_020b6ee0.mMapIndex], 0, data_02050194[data_020b6ee0.mMapIndex], 0, 0, 0);
    data_020c3e44.mUnk_2024.mUnk_04 = func_02015d48;
    data_020c3e44.mUnk_2024.mUnk_2dc.mUnk_54 = 0x640;
    data_020c3e44.mUnk_2024.mUnk_2dc.mUnk_44 = 0x186a000;
    RequestTrack(3, 0x4c);
    MI_CpuFill8(&rec->mDraw, 0, 0x44);
    func_02031fc4(0, &rec->mUnk_1a0);
    rec->mDraw.mUnk_43 = 2;
    rec->mDraw.mUnk_28 = 2 << 19;
    rec->mDistancePerFrame = 0;
    rec->mUnk_224 = 0;
    func_020271ec(rec, 0, 0, 0, data_0201ab04[rec->mInfo.mUnk_13]);
    rec->mUnk_158 = rec->mUnk_1a0;
    func_020271ec(&data_020b6ee0.mUnk_8e60, 0, 0, 0, data_0201aba0[5]);
    data_020b6ee0.mUnk_8e60.mDraw = rec->mDraw;
    data_020b6ee0.mUnk_8e60.mDraw.mUnk_2c = -1;
    data_020b6ee0.mUnk_8e60.mDraw.mUnk_30 = -1;
    data_020b6ee0.mUnk_8e60.mDraw.mUnk_34 = -1;
    data_020b6ee0.mUnk_8e60.mDraw.mUnk_3c = 0;
    data_020b6ee0.mUnk_8e60.mDraw.mUnk_3a = data_020b6ee0.mUnk_8e60.mDraw.mUnk_3c;
    data_020c3e44.mUnk_2024.mUnk_98 = 4;
    func_02031fc4(0, &data_020c3e44.mUnk_2024.mUnk_74);
    data_020c3e44.mUnk_2024.mUnk_5c = data_020c3e44.mUnk_2024.mUnk_74;
    data_020c3e44.mUnk_2024.mUnk_5c.y += 0x6d6000;
    ClearPanelState();
    SetupMapObjects();
    obj->mEndTimer = func_020182b8(rec->mInfo.mUnk_11 - 1, 2) * 60000 / 1000;
    obj->mScore = 0;
    func_02005a8c(1, 0);
}

void func_02018558(void) {
    int stage = data_020b6ee0.mSelectedStageIndex;
    Unk_020bfd40 *obj = &data_020b6ee0.mUnk_8e60;
    StageRec *rec = &data_020b1990[stage];
    VecFx32 pos;
    u8 mode;
    int busy;

    if (stage >= STAGE_MAX) {
        OS_Terminate();
    }
    func_0201168c(data_020b6ee0.mSelectedStageIndex, obj->mScore, data_0206d448.mUnk_00, obj->mEndTimer, 2);
    SetStageCameraFar(stage);
    rec->mUnk_158 = rec->mUnk_1a0;
    func_0200ba94(&rec->mUnk_1a0, 0x70800, 0x800);
    func_0202733c(rec, &rec->mUnk_1a0, 0);
    PlayObjectSe(rec);
    func_02025db4(&rec->mUnk_1a0, &rec->mScreenX);
    func_020262a0(&data_020b6ee0.mUnk_8e60.mModel);
    if (data_020b6ee0.mUnk_8e60.mModel.mUnk_142 != 1) {
        data_020b6ee0.mUnk_8e60.mDraw.mPos = rec->mUnk_1a0;
        func_02024b5c(data_020b6ee0.mUnk_8e60.mModel.mUnk_170, &data_020b6ee0.mUnk_8e60.mDraw, &data_020b6ee0.mUnk_8e60,
                      0);
    } else {
        func_020271c8();
    }
    pos.x = data_020c3e44.mUnk_2024.mUnk_74.x;
    pos.z = data_020c3e44.mUnk_2024.mUnk_74.z;
    pos.y = -0x19000;
    func_0200ba08(data_020b6ee0.mMapIndex + 0xfbe, &pos, (gSystemState.mElapsedFrames >> 1) % 100, 0, 0x3000, 0);
    func_02033614(data_020c3e44.mUnk_2024.mUnk_74.x, data_020c3e44.mUnk_2024.mUnk_74.z);
    UpdateFollowers(stage);
    func_02015f34();
    func_02016008(stage);
    func_020164e0(stage);
    mode = func_02005bf8();
    if (mode != 0) {
        busy = func_02005b1c(1);
        if (mode == 2 && busy == 0) {
            func_020059b4(func_02005c04(), obj->mScore);
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

int func_020186f0(u8 stage) {
    return func_020182b8(stage, 3);
}

void func_020186fc(int stage) {
    Unk_020bfd40 *obj = &data_020b6ee0.mUnk_8e60;
    StageRec *rec = &data_020b1990[stage];

    if (stage < 0 || stage >= STAGE_MAX) {
        OS_Terminate();
    }
    obj->mEndTimer--;
    func_020059cc(obj->mScore);
    if (obj->mEndTimer <= 0 || data_020b6ee0.mUnk_8ad9 == 0) {
        func_02005a8c(0, obj->mScore >= func_020182b8(rec->mInfo.mUnk_11 - 1, 3));
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
    slot = func_0200d88c(func_0200d7cc(kind, 0, 30, 0, -15, 0, -256, 110, 1, 0));
    slot->mUnk_134 = rec->mUnk_1a0;
    slot->mUnk_140 = 1;
    slot = func_0200d88c(func_0200d838(buf, 2, data_0201a61c, 30, 0, 0, 0, -256, 100, 1, 0));
    slot->mUnk_134 = rec->mUnk_1a0;
    slot->mUnk_140 = 1;
}

void func_02018840(StageRec *rec, int kind, int index) {
    Unk_020bfd40 *obj = &data_020b6ee0.mUnk_8e60;
    int time, score;
    time = score = 0;

    switch ((u32) kind) {
        case 0xf:
            if (index < 0 || index >= data_020b6ee0.mUnk_5a54) {
                OS_Terminate();
            }
            time = -data_020b6ee0.mUnk_1014[index].mUnk_183 * 100;
            data_020b6ee0.mUnk_8e60.mModel.mUnk_134 |= 1;
            break;
        case 0x14:
            time = -100;
            data_020b6ee0.mUnk_8e60.mModel.mUnk_134 |= 4;
            break;
        case 5:
            time = -100;
            data_020b6ee0.mUnk_8e60.mModel.mUnk_134 |= 1;
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
        func_02025db4(&rec->mUnk_1a0, &rec->mScreenX);
    }
    if (time != 0) {
        obj->mEndTimer += time;
        AddStageAmountObjSlots(rec, 0xf49, time);
    } else if (score != 0) {
        if (score <= 0) {
            OS_Terminate();
        }
        func_020059d0(&obj->mScore, score, 0);
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
    func_0202c1c4();
    func_0202bdd8(0x10cb, 0x10ca, 0x10cc, 0, 1);
    func_0202bdd8(0x10cb, 0x10ca, 0x10cc, 2, 2);
    SetLcdSwap(0);
    func_02021fb0(0x1c7, 0x1c6, 0x180, 0x10);
    func_020325b0(0x1db, 0, 0x1dc, 0, 0, 0);
    data_020c3e44.mUnk_2024.mUnk_04 = func_02015d48;
    data_020c3e44.mUnk_2024.mUnk_2dc.mUnk_54 = 0x640;
    RequestTrackDelayed(6, 0x4c, 3);
    data_020c3e44.mUnk_2024.mUnk_2dc.mUnk_44 = 0x186a000;
    func_02031fc4(0, &offset);
    func_02033df4(0x18);
    data_020c3e44.mUnk_2024.mUnk_98 = 0;
    if (data_020b6ee0.mUnk_a147 == 0) {
        func_02031fc4(0, &data_020c3e44.mUnk_2024.mUnk_8c);
        func_02031fc4(0, &data_020b6ee0.mUnk_a574.mUnk_0c);
        data_020c3e44.mUnk_2024.mUnk_80 = data_020c3e44.mUnk_2024.mUnk_8c;
        data_020c3e44.mUnk_2024.mUnk_80.y += 0xcb2000;
        data_020c3e44.mUnk_2024.mUnk_80.z += 0x32000;
        data_020b6ee0.mUnk_a574.mCameraEaseDivisor = 0x80;
    } else {
        data_020b6ee0.mUnk_a147 = 0;
    }
    params.mUnk_43 = 2;
    params.mUnk_28 = 2 << 19;
    for (i = 0; i < data_020b6ee0.mStageCount; i++) {
        rec = &data_020b1990[i];
        func_020271ec(rec, 0, 0, 0, data_0201ab04[rec->mInfo.mUnk_13]);
        rec->mDraw = params;
        rec->mUnk_1d8 = NextRandom();
        rec->mDistancePerFrame = 0;
        rec->mUnk_224 = 0;
        if (rec->mUnk_1e8 == -1) {
            func_02031fc4(0, &rec->mUnk_1a0);
            rec->mDistancePerFrame = 0x6000;
        } else {
            rec->mUnk_1a0.x = offset.x + rec->mStoredOffsetX;
            rec->mUnk_1a0.z = offset.z + rec->mStoredOffsetZ;
        }
        rec->mUnk_1a0.y = offset.y + 0x32000;
        rec->mUnk_1a0.y =
            func_02030830(rec->mUnk_1a0.x, rec->mUnk_1a0.z, rec->mUnk_1a0.y + 0xc800, 0, 0, 0, 0) + 0x32000;
        rec->mUnk_158 = rec->mUnk_1a0;
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
        rec = &data_020b1990[data_020b6ee0.mSelectedStageIndex];
        dx = rec->mUnk_1a0.x - data_020c3e44.mUnk_2024.mUnk_8c.x;
        dz = rec->mUnk_1a0.z - data_020c3e44.mUnk_2024.mUnk_8c.z;
        data_020c3e44.mUnk_2024.mUnk_8c.x += dx;
        data_020c3e44.mUnk_2024.mUnk_8c.z += dz;
        data_020c3e44.mUnk_2024.mUnk_80.x += dx;
        data_020c3e44.mUnk_2024.mUnk_80.z += dz;
    }
    FillEmptySlots();
    func_020196b4();
    func_020198f0();
    panel->mPanelButtonOffsetX = 0x28;
    data_020b6ee0.mPrevSelectedStageIndex = -1;
    data_020b6ee0.mButtonsEnabled = 1;
    func_020271ec(&data_020b6ee0.mUnk_9c64, 0, 0, 2, data_0201aba0[3]);
    data_020b6ee0.mUnk_a10c = -1;
    LoadStageOrTextImage(data_020b6ee0.mSelectedStageIndex, 2);
    data_020b6ee0.mUnk_a14b = 0;
    func_020271ec(&data_020b6ee0.mUnk_8e60, 0, 0, 0, data_0201aba0[5]);
    func_02023f58(&data_020b6ee0.mUnk_8e60.mDraw, 0, 0, 0, 6 << 18, 0, 2, 0, 0x100);
    if (data_020b6ee0.mUnk_a14a == 1) {
        data_020b6ee0.mUnk_a14a = 0;
        data_020b6ee0.mUnk_9c64.mUnk_134 |= 1;
        if (data_020b6ee0.mSelectedStageIndex != -1) {
            data_020b6ee0.mUnk_8e60.mModel.mUnk_134 |= 4;
        }
    }
    if (data_020b6ee0.mSaveRequested == 1) {
        RunSaveScreen(0);
        data_020b6ee0.mSaveRequested = 0;
    }
    func_0200c164(&data_020b6ee0.mMoveObj);
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
        StageRec *rec = &data_020b1990[i];
        if (i == data_020b6ee0.mSelectedStageIndex) {
            order[i].mKey = 0xffff;
        } else if (rec->mUnk_142 != 1) {
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
        if (rec->mDistancePerFrame != 0 || rec->mUnk_142 != 1) {
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
        rec = &data_020b1990[idx];
        params = rec->mDraw;
        rec->mUnk_158 = params.mPos;
        rec->mVisible = func_02031d0c(&rec->mUnk_1a0, 0x64000, 0x64000, 0x64000);
        if (rec->mVisible == 1 || idx == data_020b6ee0.mSelectedStageIndex) {
            func_02025db4(&rec->mUnk_1a0, &rec->mScreenX);
            lim = (data_020b6ee0.mPanelState == 0 || data_020b6ee0.mPanelState == 1) ? 2 : 1;
            if (order[j].mKey != 0 && j < lim) {
                rec->mUnk_170 = rec->mInfo.mUnk_13 * 6 + 0xfd8;
            } else {
                rec->mUnk_170 = rec->mInfo.mUnk_13 * 6 + 0xfdb;
            }
            if (rec->mUnk_142 != 1 || rec->mUnk_134 != 0 || idx == data_020b6ee0.mSelectedStageIndex) {
                if (rec->mUnk_142 != 1 && data_020b6ee0.mUnk_ca8 != 0 && idx == data_020b6ee0.mSelectedStageIndex) {
                    func_0202601c(data_020b6ee0.mUnk_ca8, &data_020b6ee0.mUnk_ca6, 2, 1, &data_020b6ee0.mUnk_c94,
                                  &data_020b6ee0.mUnk_c98, &data_020b6ee0.mUnk_c9c, 1);
                    func_02023478(data_020b6ee0.mUnk_ca8, data_020b6ee0.mUnk_ca0, &data_020b6ee0.mUnk_c88,
                                  &data_020b6ee0.mUnk_c7c, data_020b6ee0.mUnk_c94, data_020b6ee0.mUnk_c94,
                                  data_020b6ee0.mUnk_c9c, 0);
                    params.mUnk_3a += data_020b6ee0.mUnk_ca0[0];
                    params.mUnk_38 += data_020b6ee0.mUnk_ca0[1];
                    params.mUnk_3c += data_020b6ee0.mUnk_ca0[2];
                    params.mPos.x += data_020b6ee0.mUnk_c88.x * data_020b6ee0.mUnk_c7c.x;
                    params.mPos.y += data_020b6ee0.mUnk_c88.y * data_020b6ee0.mUnk_c7c.y;
                    params.mPos.z += data_020b6ee0.mUnk_c88.z * data_020b6ee0.mUnk_c7c.z;
                    rec->mUnk_158 = params.mPos;
                }
                if (data_020b6ee0.mUnk_a10c != -1 && idx == data_020b6ee0.mSelectedStageIndex) {
                    func_02025ff4((Model *) rec, data_020b6ee0.mUnk_a10c);
                }
                func_0202733c(rec, &params, 0x10);
                PlayObjectSe(rec);
            } else {
                func_02024b5c(rec->mUnk_170, &params, rec, 0x10);
            }
            if (idx == data_020b6ee0.mSelectedStageIndex) {
                func_020262a0(&data_020b6ee0.mUnk_8e60.mModel);
                if (data_020b6ee0.mUnk_8e60.mModel.mUnk_142 != 1) {
                    data_020b6ee0.mUnk_8e60.mDraw.mPos = rec->mUnk_1a0;
                    func_02024b5c(data_020b6ee0.mUnk_8e60.mModel.mUnk_170, &data_020b6ee0.mUnk_8e60.mDraw.mPos,
                                  &data_020b6ee0.mUnk_8e60.mModel, 0);
                } else {
                    func_020271c8();
                }
                marker = params.mPos;
                marker.y -= 0x19000;
                func_0200ba94(&params.mPos, 0x70800, 0x800);
            }
            if (data_020b6ee0.mPanelState <= 1) {
                if (idx == data_020b6ee0.mSelectedStageIndex) {
                    func_0200ba08(0x10c8, &marker, 0, gSystemState.mElapsedFrames << 10, 0x100000, 0);
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
    UpdateStageMoveObject();
    func_02031fc4(0, &view.mPos);
    func_02023f58(&view, view.mPos.x, view.mPos.y, view.mPos.z, 0x1000, 0, 0, 0, 0x100);
    func_0202733c(&data_020b6ee0.mUnk_9c64, &view, 0);
    func_02019788();
    func_02033614(data_020c3e44.mUnk_2024.mUnk_74.x, data_020c3e44.mUnk_2024.mUnk_74.z);
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
        func_02019ad8();
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
    data_020c3e44.mUnk_2024.mUnk_98 = 4;
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
            if (!func_0200bd80(0x80000000, 0x5a) && !func_0200bd80(0x80000000, 0xa) &&
                !func_0200bd80(0x80000000, 0xb) && !func_0200bd80(0x80000000, 0xc) && !func_0200bd80(0x80000000, 0xd) &&
                !func_0200bd80(0x80000000, 0xe) && !func_0200bd80(0x80000000, 0xf) &&
                !func_0200bd80(0x80000000, 0x10) && !func_0200bd80(0x80000000, 0x11) &&
                !func_0200bd80(0x80000000, 0x21) && !func_0200bd80(0x80000000, 0x111) &&
                !func_0200bd80(0x80000000, 0x112)) {
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
            rec = &data_020b1990[i];
            rec->mStoredOffsetX = rec->mUnk_1a0.x - pos.x;
            rec->mUnk_1e8 = 0;
            rec->mStoredOffsetZ = rec->mUnk_1a0.z - pos.z;
        }
    }
}

// Stores in mPolygonGroupIndex the first polygon group of 10..15 that holds the stage's position (func_020320c0),
// minus 10, or 99 when none does.
void FindStagePolygonGroup(int stage) {
    StageRec *rec = &data_020b1990[stage];
    int i;

    if (stage < 0 || stage >= STAGE_MAX) {
        OS_Terminate();
    }
    for (i = 10; i < 16; i++) {
        if (func_020320c0(i, 0, &rec->mUnk_1a0) == 1) {
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

// Called every frame of state 10; does nothing while mPanelState > 2. Picks a random stage and, unless it is the
// selected one, moving, out of view or the panel is in state 2, may start a random turn or move or change its
// mUnk_22e. About one call in 0x40 / mStageCount + 8 then updates that stage's polygon group and adds 2 percent when it
// equals mInfo.mUnk_10, or -2 when it is another group and mProgressState is 99 or more. While mUnk_a149 < 4, one call
// in 512 also runs func_020198f0.
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
    rec = &data_020b1990[idx];
    if (data_020b6ee0.mUnk_a149 < 4 && !(NextRandom() & 0x1ff)) {
        func_020198f0();
    }
    if (idx != data_020b6ee0.mSelectedStageIndex && rec->mDistancePerFrame == 0 && rec->mVisible != 0 &&
        data_020b6ee0.mPanelState != 2) {
        switch ((u8) NextRandom()) {
            case 0:
                if (c[1] == 0) {
                    rec->mUnk_1d8 = NextRandom();
                    rec->mUnk_134 |= 4;
                }
                break;
            case 1:
                if (c[1] == 0) {
                    rec->mUnk_1d8 = NextRandom();
                    rec->mDistancePerFrame = 0x4000;
                }
                break;
            case 2:
                if (rec->mInfo.mUnk_1a >= 4) {
                    OS_Terminate();
                }
                if (rec->mInfo.mUnk_12 < 0x64) {
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
    if (rec->mPolygonGroupIndex == rec->mInfo.mUnk_10) {
        result = 2;
    } else if (rec->mPolygonGroupIndex != 99 && (u32) data_020b6ee0.mProgressState >= 0x63) {
        result = -2;
    }
    if (idx == data_020b6ee0.mSelectedStageIndex) {
        if (result == 2) {
            data_020b6ee0.mUnk_8e60.mModel.mUnk_134 |= 1;
        } else if (result == -2) {
            data_020b6ee0.mUnk_8e60.mModel.mUnk_134 |= 4;
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
                func_020271ec(&e->mModel, 0, 0, 2, data_0201aba0[4]);
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
            func_02023f58(&params, e[i].mPos.x, e[i].mPos.y, e[i].mPos.z, 0x100000, 0, 2, 0, 0x100);
            func_0202733c(&e[i], &params, 0);
        }
    }
}

// Finds the mUnk_9024 entry within range of the record's position (func_02019874). If it is kind 1, removes it, starts
// the move object from the record's screen point (UpdateStageMoveObject adds 10 percent to the record when it ends),
// plays sound 0xe0 (func_0202f110) and sets mUnk_a570. Called for the selected stage in panel states 0..2 and 5.
void func_02019800(StageRec *rec) {
    int i = func_02019874(&rec->mUnk_1a0);

    if (i != -1) {
        Unk_020bff04 *e = &data_020b6ee0.mUnk_9024[i];

        if (e->mKind == 1) {
            func_02019750(i);
            StartStageMoveObject(rec, rec->mScreenX, rec->mScreenY, 0x3e, 0x5e, 1);
            func_0202f110(0xe0, 0x64, 0, 0x40, 0xffff, 0);
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
// 0, 0x280 in state 1, else 0x500) and draws sprite there; nothing while mVisible is 0. Sprite 0 is not drawn, and
// index 1 only while mUnk_22e is set, on alternate 32-frame halves.
void DrawStageSprite(StageRec *rec, int index, int sprite, int mulX, int mulY, int width) {
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
        func_0200b928(&e->x, mulX, 8);
        func_0200b928(&e->y, mulY, 8);
        if (sprite != 0) {
            func_0202cf08(sprite, rec->mScreenX + e->x / 512, rec->mScreenY - e->y / 512, width, 1);
        }
    }
}

// Starts the move object carrying the record: with mode 1 from (x, y) to the record's percent position (PercentToX of
// mInfo.mUnk_12, 0x5e - 252), otherwise the reverse. c and d are unused.
void StartStageMoveObject(StageRec *rec, int x, int y, int c, int d, int mode) {
    MoveObj *o = &data_020b6ee0.mMoveObj;
    Vec2i pos;
    int r;

    o->mUnk_310 = rec;
    r = PercentToX(rec->mInfo.mUnk_12);
    pos.x = mode == 1 ? x : r;
    pos.y = mode == 1 ? y : 0x5e;
    pos.y += mode == 1 ? 0 : -252;
    func_0200c034(o, 0x1f, -1, &pos, 0x3c, 1, 1);
    func_0200c124(o, 1, 7);
    pos.x = mode == 1 ? r : x;
    pos.y = mode == 1 ? 0x5e : y;
    pos.y += mode == 1 ? -252 : 0;
    func_0200c0dc(o, 0x800, &pos, 0);
}

void func_02019ad8(void) {
    func_0200c164(&data_020b6ee0.mMoveObj);
    func_0200d6c8();
}

// Runs the move object; when it has ended, adds 10 percent to the record it carries (AddPercent(rec, 10, 1, 1)).
void UpdateStageMoveObject(void) {
    MoveObj *o = &data_020b6ee0.mMoveObj;

    if (o->mActive == 1) {
        func_0200c174(o, 0);
        if (o->mActive == 1) {
            func_0200c470(o);
            return;
        }
        if (o->mUnk_310 == 0) {
            OS_Terminate();
        }
        AddPercent(o->mUnk_310, 10, 1, 1);
    }
}

void func_02019b38(void) {}

// Per-frame update of the map menu's cursor and camera: sets the camera's two points from a and b, the second blended
// toward the first by mBlend (eased toward mTargetBlend), eases the camera toward them, draws the cursor sprites once
// mDelayTimer is 0, moves the cursor by the touch drag (within 0x320 of map entry 0) and moves a and b toward the
// cursor.
void UpdateMapCursorAndCamera(VecFx32 *a, VecFx32 *b) {
    MapMenu *p = &data_020b6ee0.mMapMenu;
    VecFx32 v;
    s32 out1[2];
    s32 out0[2];

    func_02031fc4(0, &v);
    data_020b6ee0.mUnk_a574.mUnk_0c.x = a->x;
    data_020b6ee0.mUnk_a574.mUnk_0c.y = a->y;
    data_020b6ee0.mUnk_a574.mUnk_0c.z = a->z;
    data_020b6ee0.mUnk_a574.mUnk_00.x = b->x;
    data_020b6ee0.mUnk_a574.mUnk_00.y = b->y;
    data_020b6ee0.mUnk_a574.mUnk_00.z = b->z;
    func_0200b928(&p->mBlend, p->mTargetBlend, 0x20);
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
    func_0200b960(&data_020c3e44.mUnk_2024.mUnk_5c, data_020b6ee0.mUnk_a574.mUnk_00,
                  data_020b6ee0.mUnk_a574.mCameraEaseDivisor);
    func_0200b960(&data_020c3e44.mUnk_2024.mUnk_74, data_020b6ee0.mUnk_a574.mUnk_0c,
                  data_020b6ee0.mUnk_a574.mCameraEaseDivisor);
    func_02033b8c();
    func_02033520();
    func_02025db4(p, out1);
    if (p->mDelayTimer == 0) {
        func_0202cf08(0xe25, 0x7f, out1[1], 0xdc, 1);
        func_0202cf08(0xe26, out1[0], 0x5f, 0xd2, 1);
        func_0202cf08(0xe29, out1[0], out1[1], 0xc8, 1);
        func_0202cf08(0xe2e, out1[0], out1[1], 0xbe, 1);
    } else {
        p->mDelayTimer--;
    }
    if (gSystemState.mTouchDown == 1 && func_0200bd80(0x40000000, 0x101) == 0 &&
        func_0200bd80(0x40000000, 0x102) == 0 && data_020b6ee0.mButtonsEnabled == 0 && p->mNodeInfoHidden == 1) {
        s32 dx;
        s32 dy;

        func_02025db4(p, out0);
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
            func_02031270(p->mCursor.mPos.x, p->mCursor.mPos.z, p->mCursor.mPos.y + 0x32000, 0, 0, 0) + 0x1900;
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
    func_0200b928(&p->mCursorXPerFrame, 0, 0x10);
    func_0200b928(&p->mCursorZPerFrame, 0, 0x10);
    {
        s32 d = (p->mCursor.mPos.z - a->z) / 16;

        b->z += d;
        a->z += d;
    }
    {
        s32 d = (p->mCursor.mPos.x - a->x) / 16;

        b->x += d;
        a->x += d;
    }
}

void SetupMapMenu(void) {
    MapMenu *p = &data_020b6ee0.mMapMenu;

    ResetVramBanks();
    func_0202c1c4();
    func_0202bdd8(0x10cb, 0x10ca, 0x10cc, 0x82, 2);
    func_0202bdd8(0x10cb, 0x10ca, 0x10cc, 0, 1);
    LoadImage(0xda3, 2);
    SetLcdSwap(0);
    func_0200bdcc(2, 0x10);
    func_02021fb0(0x1c7, 0x1c6, 0x100, 0x10);
    func_020325b0(0xdda, 0, 0xddb, 0, 0, 0);
    data_020c3e44.mUnk_2024.mUnk_04 = func_02019b38;
    data_020c3e44.mUnk_2024.mUnk_2dc.mUnk_54 = 0x640;
    data_020c3e44.mUnk_2024.mUnk_2dc.mUnk_44 = 0x9c4000;
    func_02031fc4(0, &data_020c180c);
    func_02031fc4(1, &data_020c1800);
    func_02031fc4(0, &data_020c3e44.mUnk_2024.mUnk_5c);
    func_02031fc4(0, &data_020c3e44.mUnk_2024.mUnk_74);
    data_020c3e44.mUnk_2024.mUnk_5c.z += 0x352000;
    data_020c3e44.mUnk_2024.mUnk_74.y += 0x32000;
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
    func_02027230(&p->mReachedNodeModel, 0xde5, 0xde4, 2, 0);
    if ((u32) data_020b6ee0.mProgressState > 0xc) {
        func_0202f110(0x59, 0x64, 0, 0x40, 0xffff, 0);
    }
}
