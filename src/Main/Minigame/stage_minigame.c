#include <Main/Minigame/stage_minigame.h>
#include <Main/Scene/battle_screen.h>
#include <Main/System/random_angle.h>
#include <Main/System/screen_fade.h>
#include <Main/System/game_math.h>
#include <Main/Scene/stage_progress.h>
#include <Main/System/game_util.h>
#include <Main/Scene/language_menu.h>
#include <Main/Scene/mode_select.h>
#include <Main/Scene/move_object.h>
#include <Main/System/prompt_screen.h>
#include <Main/System/sound_control.h>
#include <Main/Graphics/sprite_slots.h>
#include <Main/Scene/stage_info_panel.h>
#include <Main/System/state_flow.h>
#include <Main/Graphics/text_draw.h>
#include <Main/Graphics/model.h>
#include <Main/Graphics/texture_blocks.h>
#include <Main/Sound/wave_player.h>
#include <Main/Graphics/sprite_script.h>
#include <Main/Graphics/script_data.h>
#include <Main/System/system_init.h>
#include <Main/clib.h>
#include <nitro/fx.h>
#include <nitro/os.h>
#include <stdio.h>

Unk_02099770 data_02099770;
ParticleGame gParticleGame;

void func_0200a384(void) {
    Unk_02099774 *sel = &data_02099770.mIn;
    func_0200c5d8(1, 0, 0x1c8, 0x100, 16, 0, 130);
    func_02024208();
    func_0200a5cc(0);
    data_02099770.mIn.mUnk_14 = 0;
    data_02099770.mIn.mUnk_00 = func_02007758();
    data_02099770.mIn.mUnk_20 = -1;
    data_02099770.mIn.mUnk_24 = 0;
    SetupSharedModel(data_02099770.mIn.mUnk_00 ? 11 : 0, 0);
    RequestTrack(sel->mUnk_00 ? 8 : 7, 102);
}

void func_0200a3f4(void) {
    func_020102f8();
}

void func_0200a3fc(void) {
    Unk_02099774 *sel = &data_02099770.mIn;
    int done = 0;
    int id;
    DrawSharedModel();
    if (data_02099770.mIn.mUnk_08 == 1) {
        func_0200bb14(255, 161, 0x101, 21, 0, 0, 0x17d, 1);
    }
    if (sel->mUnk_0c == 1) {
        func_0200bb14(0, 161, 0x102, 14, 0, 0, 90, 1);
    }
    switch (sel->mState) {
        case 0:
            id = sel->mStateFrames + 0xfa8;
            DrawSprite(id, 127, 88, 255, 2);
            func_0200a5f0();
            if (id == 0xfb3) {
                func_0200a5cc(1);
                sel->mUnk_14 = 0;
            }
            break;
        case 1:
            DrawSprite(0xfb3, 127, 88, 255, 2);
            func_0200a5f0();
            if (TestButton(0x20000000, 0x101)) {
                if (data_020b6ee0.mNodeStageCount) {
                    func_0200a5cc(2);
                    sel->mUnk_14 = 0;
                } else {
                    done = 1;
                }
            }
            break;
        case 2:
            DrawSprite(0xfb3, 127, 88, 255, 2);
            func_0200a5f0();
            if (TestButton(0x20000000, 0x101)) {
                sel->mUnk_14++;
                if (sel->mUnk_14 < data_020b6ee0.mNodeStageCount) {
                    func_0200a5cc(2);
                } else {
                    done = 1;
                }
            }
            break;
        case 3:
        case 4:
            break;
    }
    if (done == 0) {
        if (IsFadeState(3, FADE_STATE_CLEAR) == 1) {
            u32 i;
            sel->mStateFrames++;
            for (i = 0; i < 3; i++) {
                if (sel->mLevels[i] != 0x4000) {
                    AddLevel(&sel->mLevels[i], 0x400);
                    break;
                }
            }
        }
    } else {
        if (func_02007758() == 0) {
            if (IsReachedNodeSelected() == 1) {
                int r = GetNodeValue(data_020b6ee0.mSelectedNodeIndex, 4);
                if (data_020b6ee0.mReachedNodeIndex == 24) {
                    data_020b6ee0.mLastNodeDone = 1;
                    data_020b6ee0.mUnk_a91b = 1;
                    func_0201033c(r, 24);
                } else {
                    data_020b6ee0.mReachedNodeIndex++;
                    func_0201033c(r, 10);
                }
            } else {
                PushState(10);
                PopState();
            }
        } else {
            PushState(10);
            PopState();
        }
        data_020b6ee0.mSaveRequested = 1;
    }
}

void func_0200a5cc(int value) {
    u32 i;
    Unk_02099774 *sel;
    data_02099770.mIn.mState = value;
    data_02099770.mIn.mUnk_08 = 0;
    data_02099770.mIn.mUnk_0c = 0;
    data_02099770.mIn.mStateFrames = 0;
    sel = &data_02099770.mIn;
    for (i = 0; i < 3; i++) {
        sel->mLevels[i] = 0;
    }
}

// Draws the stage choice panels and moves the selection forward.
void func_0200a5f0(void) {
    Unk_02099774 *sel = &data_02099770.mIn;
    u32 i;
    int id;
    switch (data_02099770.mIn.mState) {
        case 0:
            sel->mUnk_08 = 0;
            return;
        case 1: {
            DrawText(127, 37, data_0201a64e, GetText(51), 2, 256, 16, 2);
            for (i = 0; i <= sel->mUnk_14 && i < 3; i++) {
                u32 t[3];
                u32 k;
                int x, y, xx, yy;
                StageRec *rec;
                // x holds the stage id until its record is found, then the x position
                x = data_020b6ee0.mSlotStages[i];
                if (i < sel->mUnk_14) {
                    for (k = 0; k < 3; k++) {
                        t[k] = 0x4000;
                    }
                } else {
                    for (k = 0; k < 3; k++) {
                        t[k] = sel->mLevels[k];
                    }
                    if (x == -1 ||
                        (!(data_020b6ee0.mBattleSlotFlags[i] & BATTLE_SLOT_GRADE_UP) && sel->mLevels[1] == 0x4000) ||
                        ((data_020b6ee0.mBattleSlotFlags[i] & BATTLE_SLOT_GRADE_UP) && sel->mLevels[2] == 0x4000)) {
                        sel->mUnk_14 = sel->mUnk_14 + 1;
                        func_0200a5cc(1);
                    }
                }
                if (x == -1) {
                    continue;
                }
                if (x == -1 || x >= 39) {
                    OS_Terminate();
                }
                rec = &gStages[x];
                x = 127;
                if (i == 0) {
                    x -= 76;
                } else if (i == 2) {
                    x += 76;
                }
                y = MapRangeFromZero(t[0], 0x4000, -32, 106);
                DrawSprite(rec->mInfo.mStageId + 0x1089, x, y, gSystemState.mUnk_308 - 4, 2);
                DrawSprite(0x1088, x, 106, gSystemState.mUnk_308 - 2, 2);
                if (i == 0) {
                    xx = MapRangeFromZero(t[1], 0x4000, -128, x);
                    yy = 121;
                } else if (i == 1) {
                    xx = x;
                    yy = MapRangeFromZero(t[1], 0x4000, 208, 121);
                } else {
                    xx = MapRangeFromZero(t[1], 0x4000, 384, x);
                    yy = 121;
                }
                if (!(data_020b6ee0.mBattleSlotFlags[i] & BATTLE_SLOT_NOT_PICKED)) {
                    if (data_020b6ee0.mBattleSlotFlags[i] & BATTLE_SLOT_KEPT) {
                        id = data_020b1084.mLanguage + 0xfd9;
                    } else {
                        id = data_020b1084.mLanguage + 0xfd0;
                    }
                } else {
                    id = 0xfd6;
                }
                DrawSprite(id, xx, yy, gSystemState.mUnk_308 - 4, 2);
                if (data_020b6ee0.mBattleSlotFlags[i] & BATTLE_SLOT_GRADE_UP) {
                    data_020b6ee0.mUnk_c6c[i] = -1;
                    if (i == 0) {
                        x = MapRangeFromZero(t[2], 0x4000, -128, x);
                        y = 48;
                    } else if (i == 1) {
                        y = MapRangeFromZero(t[2], 0x4000, -128, 48);
                    } else {
                        x = MapRangeFromZero(t[2], 0x4000, 384, x);
                        y = 48;
                    }
                    DrawSprite(0xfca, x, y, gSystemState.mUnk_308 - 8, 2);
                    DrawSprite(gSystemState.mElapsedFrames / 4 % 9 + 0x11ad, x + 14, y + 24, gSystemState.mUnk_308 - 6,
                               2);
                }
            }
            if (sel->mUnk_14 == 3) {
                sel->mUnk_08 = 1;
            }
            return;
        }
        case 2: {
            StageRec *rec;
            id = data_020b6ee0.mNodeStages[sel->mUnk_14];
            if (id == -1 || id >= 39) {
                OS_Terminate();
            }
            rec = &gStages[id];
            DrawText(127, 37, data_0201a64e, GetText(50), 2, 256, 16, 2);
            DrawSprite(rec->mInfo.mStageId + 0x1089, 127, MapRangeFromZero(sel->mLevels[0], 0x4000, -32, 106),
                       gSystemState.mUnk_308 - 4, 2);
            DrawSprite(0x1088, 127, 106, gSystemState.mUnk_308 - 2, 2);
            DrawText(127, MapRangeFromZero(sel->mLevels[0], 0x4000, 208, 135), data_0201a61c,
                     GetText(rec->mInfo.mStageId + 98), 2, 256, 16, 2);
            if (sel->mLevels[0] == 0x4000) {
                sel->mUnk_08 = 1;
            }
            return;
        }
        case 3:
        case 4:
            break;
    }
}

int GetParticleKindValue(int a, int b) {
    return GetFileValue(0xd5f, b + a * 10);
}

int GetParticleGameValue(int a, int b) {
    return GetFileValue(0xd60, b + a * 20);
}

void func_0200a99c(DrawParams *p, u32 a, u32 b, u32 c, s32 d, u16 e, u8 f, u8 g, s32 h) {
    p->mUnk_41 = 0;
    p->mUnk_28 = d;
    p->mUnk_43_4 = 0;
    p->mUnk_43_3 = p->mUnk_43_4;
    p->mUnk_38 = 0;
    p->mUnk_3a = e;
    p->mUnk_3c = 0;
    p->mUnk_43 = f;
    p->mPos.x = a;
    p->mPos.z = c;
    p->mPos.y = b;
    if (h & 0x100) {
        p->mUnk_2c = -1;
        p->mUnk_34 = -1;
        p->mUnk_30 = -1;
    } else {
        p->mUnk_2c = 0;
        p->mUnk_34 = 0;
        p->mUnk_30 = 0;
    }
    p->mUnk_3e = g;
}

void func_0200aa2c(Model *obj, u32 a, u32 b, s32 c, u16 d, s32 e) {
    DrawParams params;
    func_0200a99c(&params, a, b, 0, c, d, 4, 255, e);
    UpdateModelScript(obj);
    DrawModelFileEx(obj->mUnk_170, &params, obj, e);
}

void func_0200aa68(u16 id, u32 a, u32 b, u32 c, s32 d, u16 e, u8 f, s32 g) {
    DrawParams params;
    func_0200a99c(&params, a, b, c, d, e, 4, f, g);
    DrawModelFileEx(id, &params, 0, g);
}

int PickParticleKind(void) {
    u8 i;
    u8 sum = 0;
    u8 r = RandomRange(1, 100);
    for (i = 0; i < 5; i++) {
        u32 w = gParticleGame.mWeights[0][i];
        sum += w;
        if (w != 0 && r <= sum) {
            return i;
        }
    }
    return -1;
}

int AddParticle(Particle *src, int kind) {
    int i;
    for (i = 0; i < PARTICLE_COUNT; i++) {
        Particle *p = &gParticleGame.mParts[i];
        if (p->mPresent == 0) {
            if (src != 0) {
                *p = *src;
                gParticleGame.mCount++;
                return 1;
            }
            p->mPresent = 1;
            p->mEntered = 0;
            p->mFacingHitTimer = 0;
            if (kind < 0) {
                p->mKind = PickParticleKind();
            } else {
                if (kind >= 5) {
                    OS_Terminate();
                }
                p->mKind = kind;
            }
            if (p->mKind >= 3) {
                u32 j;
                u32 near = 0;
                u8 mask = 0;
                u8 list[4];
                u8 n;
                u8 pick;
                for (j = 0; j < PARTICLE_COUNT; j++) {
                    if (gParticleGame.mParts[j].mPresent != 0 && gParticleGame.mParts[j].mKind == 3) {
                        near++;
                    }
                }
                if (near <= 3) {
                    u32 k;
                    if (gParticleGame.mX >> 8 > 128) {
                        mask |= 1;
                    }
                    if (gParticleGame.mY >> 8 > 96) {
                        mask |= 2;
                    }
                    n = 0;
                    for (k = 0; k < 4; k++) {
                        if (k != mask) {
                            list[n] = k;
                            n++;
                        }
                    }
                    pick = list[RandomRange(0, 2)];
                    p->mPos.x = RandomRange(0, 64) + 32;
                    p->mPos.y = RandomRange(0, 32) + 32;
                    if (pick & 1) {
                        p->mPos.x += 128;
                    }
                    if (pick & 2) {
                        p->mPos.y += 96;
                    }
                    p->mPos.x <<= 8;
                    p->mPos.y <<= 8;
                    p->mVel.x = 0;
                    p->mVel.y = 0;
                } else {
                    p->mKind = 0;
                }
            }
            if (p->mKind < 3) {
                int dx;
                int dy;
                int dist;
                p->mPos.x = RandomRange(0, 0x13f) - 32;
                p->mPos.y = RandomRange(0, 255) - 32;
                if (!(NextRandom() & 1)) {
                    p->mPos.y = (NextRandom() & 1) == 0 ? -32 : 223;
                } else {
                    p->mPos.x = (NextRandom() & 1) == 0 ? -32 : 287;
                }
                p->mPos.x <<= 8;
                p->mPos.y <<= 8;
                dx = (gParticleGame.mX - p->mPos.x) >> 8;
                dy = (gParticleGame.mY - p->mPos.y) >> 8;
                dist = SqrtU32(dx * dx + dy * dy);
                p->mVel.x =
                    ((int) RandomRange(gParticleGame.mWeights[1][p->mKind], gParticleGame.mWeights[2][p->mKind]) * dx /
                     (dist + 1))
                    << 8;
                p->mVel.y =
                    ((int) RandomRange(gParticleGame.mWeights[1][p->mKind], gParticleGame.mWeights[2][p->mKind]) * dy /
                     (dist + 1))
                    << 8;
            }
            StartSprite(&p->mSprite, data_0201abec[p->mKind + 6]);
            gParticleGame.mCount++;
            return 1;
        }
    }
    return 0;
}

void SetParticleDirection(Particle *p, int angle) {
    int len = SqrtU32(p->mVel.x * p->mVel.x + p->mVel.y * p->mVel.y);
    int len2;
    p->mVel.x = -FX_SinCosTable_[(angle >> 4) * 2];
    p->mVel.y = -FX_SinCosTable_[(angle >> 4) * 2 + 1];
    len2 = SqrtU32(p->mVel.x * p->mVel.x + p->mVel.y * p->mVel.y);
    p->mVel.x = p->mVel.x * (len + 1) / (len2 + 1);
    p->mVel.y = p->mVel.y * (len + 1) / (len2 + 1);
}

void RemoveParticle(Particle *p, int flag) {
    p->mPresent = 0;
    if (flag) {
        if (p->mKind == 2) {
            AddAnimObjSlotEx(12, 1 << 23, p->mPos.x >> 8, p->mPos.y >> 8, 0, 0, 30, 1);
        } else {
            AddAnimObjSlotEx(11, 1 << 23, p->mPos.x >> 8, p->mPos.y >> 8, 0, 0, 30, 1);
        }
    }
    gParticleGame.mCount--;
}

void ApplyParticleNearHit(Particle *p) {
    char buf[32];
    switch (p->mKind) {
        case 0:
        case 1:
        case 2: {
            u32 loss = 60000 * gParticleGame.mKindSeconds[p->mKind] / 1000;
            if (loss >= gParticleGame.mEndTimer) {
                gParticleGame.mEndTimer = 1;
            } else {
                gParticleGame.mEndTimer -= loss;
            }
            sprintf(buf, "-%d'", gParticleGame.mKindSeconds[p->mKind]);
            AddTextObjSlot(buf, 2, data_0201a61c, 30, p->mPos.x >> 8, p->mPos.y >> 8, 0, -128, 100, 1, 0);
            PlayWaveKeepAlive(47, 100, 0, 64, 0xffff, 0);
            RemoveParticle(p, 1);
            break;
        }
        case 3:
            AddCappedAt999Shifted(&gParticleGame.mScore, gParticleGame.mKindScores[p->mKind], 0);
            RemoveParticle(p, 0);
            PlayWaveKeepAlive(224, 100, 0, 64, 0xffff, 0);
            break;
        case 4:
            AddCappedAt5999(&gParticleGame.mEndTimer, 60000 * gParticleGame.mKindSeconds[p->mKind] / 1000);
            sprintf(buf, "+%d'", gParticleGame.mKindSeconds[p->mKind]);
            AddTextObjSlot(buf, 2, data_0201a61c, 30, p->mPos.x >> 8, p->mPos.y >> 8, 0, -128, 100, 1, 0);
            RemoveParticle(p, 0);
            PlayWaveKeepAlive(224, 100, 0, 64, 0xffff, 0);
            break;
    }
}

void ApplyParticleFacingHit(Particle *p, int angle) {
    Particle q;
    u16 base;
    switch (p->mKind) {
        case 3:
        case 4:
            ApplyParticleNearHit(p);
            return;
        case 1:
            base = AngleHalfway((u16) angle, gParticleGame.mAngle);
            q = *p;
            q.mKind = 0;
            q.mFacingHitTimer = 16;
            SetParticleDirection(&q, base);
            StartSprite(&q.mSprite, data_0201abec[q.mKind + 6]);
            AddParticle(&q, -1);
            q = *p;
            q.mKind = 0;
            q.mFacingHitTimer = 16;
            SetParticleDirection(&q, (u16) (base - 0x2000));
            StartSprite(&q.mSprite, data_0201abec[q.mKind + 6]);
            AddParticle(&q, -1);
            q = *p;
            q.mKind = 0;
            q.mFacingHitTimer = 16;
            SetParticleDirection(&q, (u16) (base + 0x2000));
            StartSprite(&q.mSprite, data_0201abec[q.mKind + 6]);
            AddParticle(&q, -1);
            AddCappedAt999Shifted(&gParticleGame.mScore, gParticleGame.mKindScores[p->mKind], 0);
            RemoveParticle(p, 1);
            PlayWaveKeepAlive(217, 100, 0, 64, 0xffff, 0);
            return;
        case 0:
        case 2:
            AddCappedAt999Shifted(&gParticleGame.mScore, gParticleGame.mKindScores[p->mKind], 0);
            RemoveParticle(p, 1);
            PlayWaveKeepAlive(217, 100, 0, 64, 0xffff, 0);
            return;
    }
}

void DrawParticles(void) {
    int i;
    for (i = 0; i < PARTICLE_COUNT; i++) {
        Particle *p = &gParticleGame.mParts[i];
        if (p->mPresent) {
            UpdateSprite(&p->mSprite);
            DrawSprite(p->mSprite.mUnk_04 + (1 << 23), p->mPos.x >> 8, p->mPos.y >> 8, 100, 1);
        }
    }
}

void UpdateParticles(void) {
    int i;
    for (i = 0; i < PARTICLE_COUNT; i++) {
        Particle *p = &gParticleGame.mParts[i];
        Vec2i old;
        if (p->mPresent == 0) {
            continue;
        }
        old = p->mPos;
        p->mPos.x += p->mVel.x / 60;
        p->mPos.y += p->mVel.y / 60;
        if (p->mEntered == 0) {
            if (p->mPos.x > 0 && p->mPos.x <= 0xff00 && p->mPos.y > 0 && p->mPos.y <= 0xbf00) {
                p->mEntered = 1;
            }
        } else {
            int bounced = 0;
            int dx;
            int dy;
            int dist2;
            int flag;
            if (p->mPos.x < 0) {
                bounced = 1;
                p->mPos.x = 0;
                p->mVel.x = -p->mVel.x;
            } else if (p->mPos.x > 0xff00) {
                bounced = 1;
                p->mPos.x = 0xff00;
                p->mVel.x = -p->mVel.x;
            }
            if (p->mPos.y < 0) {
                bounced = 1;
                p->mPos.y = 0;
                p->mVel.y = -p->mVel.y;
            } else if (p->mPos.y > 0xc000) {
                bounced = 1;
                p->mPos.y = 0xc000;
                p->mVel.y = -p->mVel.y;
            }
            if (bounced) {
                PlayWaveKeepAlive(88, 100, 0, 64, 0xffff, 0);
            }
            dx = (p->mPos.x - gParticleGame.mX) >> 8;
            dy = (p->mPos.y - gParticleGame.mY) >> 8;
            dist2 = dx * dx + dy * dy;
            flag = 0;
            if (p->mFacingHitTimer == 0) {
                if (dist2 <= 576) {
                    u16 angle =
                        0xffff - AngleOfVector((old.x - gParticleGame.mX) >> 8, (old.y - gParticleGame.mY) >> 8);
                    if (AngleDistance(gParticleGame.mAngle, angle) <= 16000) {
                        flag = 1;
                        ApplyParticleFacingHit(p, angle);
                    }
                }
            } else {
                p->mFacingHitTimer--;
            }
            if (!flag && dist2 <= 196) {
                ApplyParticleNearHit(p);
            }
        }
    }
}

void SpawnParticles(void) {
    if (gParticleGame.mCount < gParticleGame.mParticleLimit) {
        if (gParticleGame.mSpawnTimer == 0) {
            AddParticle(0, -1);
            gParticleGame.mSpawnTimer = RandomRange(gParticleGame.mSpawnDelayMin, gParticleGame.mSpawnDelayLimit);
        } else {
            gParticleGame.mSpawnTimer--;
        }
    }
}

void SetupParticleGame(void) {
    s32 level = data_020b6ee0.mSelectedStageIndex;
    int i;
    if (level < 0) {
        level = 0;
    }
    gParticleGame.mInfo = gStages[level].mInfo;
    data_020b10fc.mTableRow = gParticleGame.mInfo.mGrade - 1;
    {
        BOOL ok = 0;
        if (data_020b10fc.mTableRow >= 0 && data_020b10fc.mTableRow < 12) {
            ok = 1;
        }
        if (ok == 0) {
            OS_Terminate();
        }
    }
    func_0200c5d8(0, 0, gParticleGame.mInfo.mStageId + 0xdb3, 0x100, 16, 1, 130);
    func_02024208();
    RequestTrack(2, 0x4c);
    gParticleGame.mAngle = 0;
    gParticleGame.mX = 0x8000;
    gParticleGame.mY = 0x6000;
    gParticleGame.mVelX = 0;
    gParticleGame.mVelY = 0;
    InitModel(&gParticleGame.mModel, 0, 0, 0, data_0201abbc[8]);
    for (i = 0; i < PARTICLE_COUNT; i++) {
        gParticleGame.mParts[i].mPresent = 0;
    }
    gParticleGame.mCount = 0;
    gParticleGame.mKindScores[0] = GetParticleKindValue(0, 2);
    gParticleGame.mKindSeconds[0] = GetParticleKindValue(0, 3);
    gParticleGame.mKindScores[1] = GetParticleKindValue(0, 4);
    gParticleGame.mKindSeconds[1] = GetParticleKindValue(0, 5);
    gParticleGame.mKindScores[2] = GetParticleKindValue(0, 6);
    gParticleGame.mKindSeconds[2] = GetParticleKindValue(0, 7);
    gParticleGame.mKindScores[3] = GetParticleKindValue(0, 8);
    gParticleGame.mKindSeconds[3] = GetParticleKindValue(0, 9);
    gParticleGame.mKindScores[4] = GetParticleKindValue(0, 10);
    gParticleGame.mKindSeconds[4] = GetParticleKindValue(0, 11);
    gParticleGame.mParticleLimit = GetParticleGameValue(data_020b10fc.mTableRow, 2);
    gParticleGame.mEndFrameCount = GetParticleGameValue(data_020b10fc.mTableRow, 3) * 60000 / 1000;
    gParticleGame.mScoreThreshold = GetParticleGameValue(data_020b10fc.mTableRow, 4);
    gParticleGame.mSpawnDelayMin = GetParticleGameValue(data_020b10fc.mTableRow, 5) * 60 / 1000;
    gParticleGame.mSpawnDelayLimit = GetParticleGameValue(data_020b10fc.mTableRow, 6) * 60 / 1000;
    gParticleGame.mWeights[0][0] = GetParticleGameValue(data_020b10fc.mTableRow, 7);
    gParticleGame.mWeights[1][0] = GetParticleGameValue(data_020b10fc.mTableRow, 8);
    gParticleGame.mWeights[2][0] = GetParticleGameValue(data_020b10fc.mTableRow, 9);
    gParticleGame.mWeights[0][1] = GetParticleGameValue(data_020b10fc.mTableRow, 10);
    gParticleGame.mWeights[1][1] = GetParticleGameValue(data_020b10fc.mTableRow, 11);
    gParticleGame.mWeights[2][1] = GetParticleGameValue(data_020b10fc.mTableRow, 12);
    gParticleGame.mWeights[0][2] = GetParticleGameValue(data_020b10fc.mTableRow, 13);
    gParticleGame.mWeights[1][2] = GetParticleGameValue(data_020b10fc.mTableRow, 14);
    gParticleGame.mWeights[2][2] = GetParticleGameValue(data_020b10fc.mTableRow, 15);
    gParticleGame.mWeights[0][3] = GetParticleGameValue(data_020b10fc.mTableRow, 16);
    gParticleGame.mWeights[1][3] = GetParticleGameValue(data_020b10fc.mTableRow, 17);
    gParticleGame.mWeights[2][3] = GetParticleGameValue(data_020b10fc.mTableRow, 18);
    gParticleGame.mWeights[0][4] = GetParticleGameValue(data_020b10fc.mTableRow, 19);
    gParticleGame.mWeights[1][4] = GetParticleGameValue(data_020b10fc.mTableRow, 20);
    gParticleGame.mWeights[2][4] = GetParticleGameValue(data_020b10fc.mTableRow, 21);
    gParticleGame.mUnk_0000 = 0;
    gParticleGame.mEndTimer = gParticleGame.mEndFrameCount;
    gParticleGame.mScore = 0;
    gParticleGame.mSpawnTimer = 180;
    StartModeSlide(1, 0);
}

void DrawParticleGame(void) {
    func_0201168c(data_020b6ee0.mSelectedStageIndex, gParticleGame.mScore, gModeSelectState.mScoreThreshold,
                  gParticleGame.mEndTimer, 2);
    func_0200aa68(gParticleGame.mInfo.mKind + 0xd28, 256, 0, 0x1000, 0x1000, 0, 255, 256);
    func_0200aa68(gParticleGame.mInfo.mStageId + 0xd31, gParticleGame.mX >> 8, gParticleGame.mY >> 8, 0, 0x6000,
                  gParticleGame.mAngle, 255, 0);
    func_0200aa68(0xd2e, gParticleGame.mX >> 8, gParticleGame.mY >> 8, 0, 0x6000, 0, 255, 0);
    func_0200aa2c(&gParticleGame.mModel, gParticleGame.mX >> 8, gParticleGame.mY >> 8, 0x6000, gParticleGame.mAngle,
                  256);
    DrawParticles();
}

void UpdateParticleGame(void) {
    u8 slideState;
    func_020059cc(gParticleGame.mScore);
    slideState = GetModeSlideState();
    if (slideState != MODE_SLIDE_STATE_NONE) {
        int busy;
        DrawParticleGame();
        busy = UpdateModeSlide(1);
        if (slideState == MODE_SLIDE_STATE_END && busy == 0) {
            StartResultScreen(GetScoreThresholdMet(), gParticleGame.mScore);
        }
        return;
    }
    {
        s32 lim = 51200;
        s32 v = gParticleGame.mVelX;
        if (v < lim) {
            lim = v;
        }
        if (lim < -51200) {
            v = -51200;
        } else if (v >= 51200) {
            v = 51200;
        }
        gParticleGame.mVelX = v;
    }
    {
        s32 lim = 51200;
        s32 v = gParticleGame.mVelY;
        if (v < lim) {
            lim = v;
        }
        if (lim < -51200) {
            v = -51200;
        } else if (v >= 51200) {
            v = 51200;
        }
        gParticleGame.mVelY = v;
    }
    gParticleGame.mX += gParticleGame.mVelX / 60;
    gParticleGame.mY += gParticleGame.mVelY / 60;
    {
        s32 lim = 0xff00;
        s32 v = gParticleGame.mX;
        if (v < lim) {
            lim = v;
        }
        if (lim < 0) {
            v = 0;
        } else if (v >= 0xff00) {
            v = 0xff00;
        }
        gParticleGame.mX = v;
    }
    {
        s32 v = gParticleGame.mY;
        s32 lim = 0xbf00;
        if (v < lim) {
            lim = v;
        }
        if (lim < 0) {
            v = 0;
        } else if (v >= 0xbf00) {
            v = 0xbf00;
        }
        gParticleGame.mY = v;
    }
    if (gSystemState.mKeysDown & 0x10) {
        gParticleGame.mVelX += 0x1000;
    } else if (gSystemState.mKeysDown & 0x20) {
        gParticleGame.mVelX -= 0x1000;
    } else {
        MoveTowardZero(&gParticleGame.mVelX, 0x300);
    }
    if (gSystemState.mKeysDown & 0x80) {
        gParticleGame.mVelY += 0x1000;
    } else if (gSystemState.mKeysDown & 0x40) {
        gParticleGame.mVelY -= 0x1000;
    } else {
        MoveTowardZero(&gParticleGame.mVelY, 0x300);
    }
    if (gSystemState.mTouchDown) {
        u16 target = 0xffff - AngleOfVector(gSystemState.mTouchX - (gParticleGame.mX >> 8),
                                            gSystemState.mTouchY - (gParticleGame.mY >> 8));
        s32 angle = gParticleGame.mAngle;
        EaseAngleToward(&angle, target, 2);
        gParticleGame.mAngle = angle;
    } else {
        u16 current = gParticleGame.mAngle;
        s32 target = current;
        s32 angle;
        u16 keys = gSystemState.mKeysDown;
        if ((keys & 1) && (keys & 2)) {
            target = 0xa000;
        } else if ((keys & 0x800) && (keys & 0x400)) {
            target = 0x2000;
        } else if ((keys & 1) && (keys & 0x400)) {
            target = 0xe000;
        } else if ((keys & 0x800) && (keys & 2)) {
            target = 0x6000;
        } else if (keys & 1) {
            target = 0xc000;
        } else if (keys & 2) {
            target = 0x8000;
        } else if (keys & 0x800) {
            target = 0x4000;
        } else if (keys & 0x400) {
            target = 0;
        }
        angle = current;
        EaseAngleToward(&angle, target, 2);
        gParticleGame.mAngle = angle;
    }
    UpdateParticles();
    if (--gParticleGame.mEndTimer == 0) {
        StartModeSlide(0, gParticleGame.mScore >= gParticleGame.mScoreThreshold);
    }
    SpawnParticles();
    DrawParticleGame();
}

void LeaveParticleGame(void) {
    func_020102f8();
}

int GetParticleGameScoreThreshold(u8 tableRow) {
    return GetParticleGameValue(tableRow, 4);
}
