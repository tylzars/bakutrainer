#include <Main/Unknown/stage_minigame.h>
#include <Main/Unknown/battle_screen.h>
#include <Main/Unknown/functions.h>
#include <Main/Unknown/random_angle.h>
#include <Main/Unknown/screen_fade.h>
#include <Main/Unknown/game_math.h>
#include <Main/Unknown/stage_progress.h>
#include <Main/Unknown/stage_select.h>
#include <Main/Unknown/game_util.h>
#include <Main/Unknown/globals.h>
#include <Main/Unknown/language_menu.h>
#include <Main/Unknown/mode_select.h>
#include <Main/Unknown/move_object.h>
#include <Main/Unknown/prompt_screen.h>
#include <Main/Unknown/sound_control.h>
#include <Main/Unknown/sprite_slots.h>
#include <Main/Unknown/stage_info_panel.h>
#include <Main/Unknown/state_flow.h>
#include <Main/Unknown/text_draw.h>
#include <Main/Unknown/model.h>
#include <Main/Unknown/texture_blocks.h>
#include <Main/clib.h>
#include <nitro/fx.h>
#include <nitro/os.h>
#include <stdio.h>

Unk_02099770 data_02099770;
Unk_020997a4 data_020997a4;

void func_0200a384(void) {
    Unk_02099774 *sel = &data_02099770.mIn;
    func_0200c5d8(1, 0, 0x1c8, 0x100, 16, 0, 130);
    func_02024208();
    func_0200a5cc(0);
    data_02099770.mIn.mUnk_14 = 0;
    data_02099770.mIn.mUnk_00 = func_02007758();
    data_02099770.mIn.mUnk_20 = -1;
    data_02099770.mIn.mUnk_24 = 0;
    func_0200f848(data_02099770.mIn.mUnk_00 ? 11 : 0, 0);
    func_020104c0(sel->mUnk_00 ? 8 : 7, 102);
}

void func_0200a3f4(void) {
    func_020102f8();
}

void func_0200a3fc(void) {
    Unk_02099774 *sel = &data_02099770.mIn;
    int done = 0;
    int id;
    func_0200f898();
    if (data_02099770.mIn.mUnk_08 == 1) {
        func_0200bb14(255, 161, 0x101, 21, 0, 0, 0x17d, 1);
    }
    if (sel->mUnk_0c == 1) {
        func_0200bb14(0, 161, 0x102, 14, 0, 0, 90, 1);
    }
    switch (sel->mUnk_04) {
        case 0:
            id = sel->mUnk_10 + 0xfa8;
            func_0202cf08(id, 127, 88, 255, 2);
            func_0200a5f0();
            if (id == 0xfb3) {
                func_0200a5cc(1);
                sel->mUnk_14 = 0;
            }
            break;
        case 1:
            func_0202cf08(0xfb3, 127, 88, 255, 2);
            func_0200a5f0();
            if (func_0200bd80(0x20000000, 0x101)) {
                if (data_020b6ee0.mUnk_a53c) {
                    func_0200a5cc(2);
                    sel->mUnk_14 = 0;
                } else {
                    done = 1;
                }
            }
            break;
        case 2:
            func_0202cf08(0xfb3, 127, 88, 255, 2);
            func_0200a5f0();
            if (func_0200bd80(0x20000000, 0x101)) {
                sel->mUnk_14++;
                if (sel->mUnk_14 < data_020b6ee0.mUnk_a53c) {
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
        if (func_0201f794(3, 3) == 1) {
            u32 i;
            sel->mUnk_10++;
            for (i = 0; i < 3; i++) {
                if (sel->mUnk_18[i] != 0x4000) {
                    func_0201fb5c(&sel->mUnk_18[i], 0x400);
                    break;
                }
            }
        }
    } else {
        if (func_02007758() == 0) {
            if (func_020149b0() == 1) {
                int r = func_02014538(data_020b6ee0.mUnk_a534, 4);
                if (data_020b6ee0.mUnk_a530 == 24) {
                    data_020b6ee0.mUnk_a560 = 1;
                    data_020b6ee0.mUnk_a91b = 1;
                    func_0201033c(r, 24);
                } else {
                    data_020b6ee0.mUnk_a530++;
                    func_0201033c(r, 10);
                }
            } else {
                func_0200fa80(10);
                func_0200fab0();
            }
        } else {
            func_0200fa80(10);
            func_0200fab0();
        }
        data_020b6ee0.mUnk_a14c = 1;
    }
}

void func_0200a5cc(int value) {
    u32 i;
    Unk_02099774 *sel;
    data_02099770.mIn.mUnk_04 = value;
    data_02099770.mIn.mUnk_08 = 0;
    data_02099770.mIn.mUnk_0c = 0;
    data_02099770.mIn.mUnk_10 = 0;
    sel = &data_02099770.mIn;
    for (i = 0; i < 3; i++) {
        sel->mUnk_18[i] = 0;
    }
}

// Draws the stage choice panels and moves the selection forward.
void func_0200a5f0(void) {
    Unk_02099774 *sel = &data_02099770.mIn;
    u32 i;
    int id;
    switch (data_02099770.mIn.mUnk_04) {
        case 0:
            sel->mUnk_08 = 0;
            return;
        case 1: {
            func_020215fc(127, 37, data_0201a64e, func_0200f9f0(51), 2, 256, 16, 2);
            for (i = 0; i <= sel->mUnk_14 && i < 3; i++) {
                u32 t[3];
                u32 k;
                int x, y, xx, yy;
                StageRec *rec;
                id = data_020b6ee0.mUnk_c60[i];
                if (i < sel->mUnk_14) {
                    for (k = 0; k < 3; k++) {
                        t[k] = 0x4000;
                    }
                } else {
                    for (k = 0; k < 3; k++) {
                        t[k] = sel->mUnk_18[k];
                    }
                    if (id == -1 || (!(data_020b6ee0.mUnk_a8f3[i] & 1) && sel->mUnk_18[1] == 0x4000) ||
                        ((data_020b6ee0.mUnk_a8f3[i] & 1) && sel->mUnk_18[2] == 0x4000)) {
                        sel->mUnk_14 = sel->mUnk_14 + 1;
                        func_0200a5cc(1);
                    }
                }
                if (id == -1) {
                    continue;
                }
                if (id == -1 || id >= 39) {
                    OS_Terminate();
                }
                rec = &data_020b1990[id];
                x = 127;
                if (i == 0) {
                    x -= 76;
                } else if (i == 2) {
                    x += 76;
                }
                y = func_0201fb74(t[0], 0x4000, -32, 106);
                func_0202cf08(rec->mInfo.mUnk_13 + 0x1089, x, y, data_020c18bc.mUnk_308 - 4, 2);
                func_0202cf08(0x1088, x, 106, data_020c18bc.mUnk_308 - 2, 2);
                if (i == 0) {
                    xx = func_0201fb74(t[1], 0x4000, -128, x);
                    yy = 121;
                } else if (i == 1) {
                    xx = x;
                    yy = func_0201fb74(t[1], 0x4000, 208, 121);
                } else {
                    xx = func_0201fb74(t[1], 0x4000, 384, x);
                    yy = 121;
                }
                if (!(data_020b6ee0.mUnk_a8f3[i] & 2)) {
                    if (data_020b6ee0.mUnk_a8f3[i] & 4) {
                        id = data_020b1084.mLanguage + 0xfd9;
                    } else {
                        id = data_020b1084.mLanguage + 0xfd0;
                    }
                } else {
                    id = 0xfd6;
                }
                func_0202cf08(id, xx, yy, data_020c18bc.mUnk_308 - 4, 2);
                if (data_020b6ee0.mUnk_a8f3[i] & 1) {
                    data_020b6ee0.mUnk_c6c[i] = -1;
                    if (i == 0) {
                        x = func_0201fb74(t[2], 0x4000, -128, x);
                        y = 48;
                    } else if (i == 1) {
                        y = func_0201fb74(t[2], 0x4000, -128, 48);
                    } else {
                        x = func_0201fb74(t[2], 0x4000, 384, x);
                        y = 48;
                    }
                    func_0202cf08(0xfca, x, y, data_020c18bc.mUnk_308 - 8, 2);
                    func_0202cf08(data_020c18bc.mUnk_45c / 4 % 9 + 0x11ad, x + 14, y + 24, data_020c18bc.mUnk_308 - 6,
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
            id = data_020b6ee0.mUnk_a540[sel->mUnk_14];
            if (id == -1 || id >= 39) {
                OS_Terminate();
            }
            rec = &data_020b1990[id];
            func_020215fc(127, 37, data_0201a64e, func_0200f9f0(50), 2, 256, 16, 2);
            func_0202cf08(rec->mInfo.mUnk_13 + 0x1089, 127, func_0201fb74(sel->mUnk_18[0], 0x4000, -32, 106),
                          data_020c18bc.mUnk_308 - 4, 2);
            func_0202cf08(0x1088, 127, 106, data_020c18bc.mUnk_308 - 2, 2);
            func_020215fc(127, func_0201fb74(sel->mUnk_18[0], 0x4000, 208, 135), data_0201a61c,
                          func_0200f9f0(rec->mInfo.mUnk_13 + 98), 2, 256, 16, 2);
            if (sel->mUnk_18[0] == 0x4000) {
                sel->mUnk_08 = 1;
            }
            return;
        }
        case 3:
        case 4:
            break;
    }
}

int func_0200a970(int a, int b) {
    return func_02021e44(0xd5f, b + a * 10);
}

int func_0200a988(int a, int b) {
    return func_02021e44(0xd60, b + a * 20);
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
    func_020262a0(obj);
    func_02024b5c(obj->mUnk_170, &params, obj, e);
}

void func_0200aa68(u16 id, u32 a, u32 b, u32 c, s32 d, u16 e, u8 f, s32 g) {
    DrawParams params;
    func_0200a99c(&params, a, b, c, d, e, 4, f, g);
    func_02024b5c(id, &params, 0, g);
}

int func_0200aa9c(void) {
    u8 i;
    u8 sum = 0;
    u8 r = func_0201fd84(1, 100);
    for (i = 0; i < 5; i++) {
        u32 w = data_020997a4.mWeights[0][i];
        sum += w;
        if (w != 0 && r <= sum) {
            return i;
        }
    }
    return -1;
}

int func_0200aadc(Particle *src, int kind) {
    int i;
    for (i = 0; i < PARTICLE_COUNT; i++) {
        Particle *p = &data_020997a4.mParts[i];
        if (p->mUnk_14 == 0) {
            if (src != 0) {
                *p = *src;
                data_020997a4.mCount++;
                return 1;
            }
            p->mUnk_14 = 1;
            p->mUnk_15 = 0;
            p->mUnk_16 = 0;
            if (kind < 0) {
                p->mUnk_00 = func_0200aa9c();
            } else {
                if (kind >= 5) {
                    OS_Terminate();
                }
                p->mUnk_00 = kind;
            }
            if (p->mUnk_00 >= 3) {
                u32 j;
                u32 near = 0;
                u8 mask = 0;
                u8 list[4];
                u8 n;
                u8 pick;
                for (j = 0; j < PARTICLE_COUNT; j++) {
                    if (data_020997a4.mParts[j].mUnk_14 != 0 && data_020997a4.mParts[j].mUnk_00 == 3) {
                        near++;
                    }
                }
                if (near <= 3) {
                    u32 k;
                    if (data_020997a4.mUnk_01ac >> 8 > 128) {
                        mask |= 1;
                    }
                    if (data_020997a4.mUnk_01b0 >> 8 > 96) {
                        mask |= 2;
                    }
                    n = 0;
                    for (k = 0; k < 4; k++) {
                        if (k != mask) {
                            list[n] = k;
                            n++;
                        }
                    }
                    pick = list[func_0201fd84(0, 2)];
                    p->mPos.x = func_0201fd84(0, 64) + 32;
                    p->mPos.y = func_0201fd84(0, 32) + 32;
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
                    p->mUnk_00 = 0;
                }
            }
            if (p->mUnk_00 < 3) {
                int dx;
                int dy;
                int dist;
                p->mPos.x = func_0201fd84(0, 0x13f) - 32;
                p->mPos.y = func_0201fd84(0, 255) - 32;
                if (!(func_0201fd68() & 1)) {
                    p->mPos.y = (func_0201fd68() & 1) == 0 ? -32 : 223;
                } else {
                    p->mPos.x = (func_0201fd68() & 1) == 0 ? -32 : 287;
                }
                p->mPos.x <<= 8;
                p->mPos.y <<= 8;
                dx = (data_020997a4.mUnk_01ac - p->mPos.x) >> 8;
                dy = (data_020997a4.mUnk_01b0 - p->mPos.y) >> 8;
                dist = func_0201fccc(dx * dx + dy * dy);
                p->mVel.x =
                    ((int) func_0201fd84(data_020997a4.mWeights[1][p->mUnk_00], data_020997a4.mWeights[2][p->mUnk_00]) *
                     dx / (dist + 1))
                    << 8;
                p->mVel.y =
                    ((int) func_0201fd84(data_020997a4.mWeights[1][p->mUnk_00], data_020997a4.mWeights[2][p->mUnk_00]) *
                     dy / (dist + 1))
                    << 8;
            }
            func_0202de60(&p->mSprite, data_0201abec[p->mUnk_00 + 6]);
            data_020997a4.mCount++;
            return 1;
        }
    }
    return 0;
}

void func_0200acfc(Particle *p, int angle) {
    int len = func_0201fccc(p->mVel.x * p->mVel.x + p->mVel.y * p->mVel.y);
    int len2;
    p->mVel.x = -FX_SinCosTable_[(angle >> 4) * 2];
    p->mVel.y = -FX_SinCosTable_[(angle >> 4) * 2 + 1];
    len2 = func_0201fccc(p->mVel.x * p->mVel.x + p->mVel.y * p->mVel.y);
    p->mVel.x = p->mVel.x * (len + 1) / (len2 + 1);
    p->mVel.y = p->mVel.y * (len + 1) / (len2 + 1);
}

void func_0200ad64(Particle *p, int flag) {
    p->mUnk_14 = 0;
    if (flag) {
        if (p->mUnk_00 == 2) {
            func_0200d78c(12, 1 << 23, p->mPos.x >> 8, p->mPos.y >> 8, 0, 0, 30, 1);
        } else {
            func_0200d78c(11, 1 << 23, p->mPos.x >> 8, p->mPos.y >> 8, 0, 0, 30, 1);
        }
    }
    data_020997a4.mCount--;
}

void func_0200adc0(Particle *p) {
    char buf[32];
    switch (p->mUnk_00) {
        case 0:
        case 1:
        case 2: {
            u32 loss = 60000 * data_020997a4.mUnk_11fd8[p->mUnk_00] / 1000;
            if (loss >= data_020997a4.mUnk_0004) {
                data_020997a4.mUnk_0004 = 1;
            } else {
                data_020997a4.mUnk_0004 -= loss;
            }
            sprintf(buf, "-%d'", data_020997a4.mUnk_11fd8[p->mUnk_00]);
            func_0200d838(buf, 2, data_0201a61c, 30, p->mPos.x >> 8, p->mPos.y >> 8, 0, -128, 100, 1, 0);
            func_0202f110(47, 100, 0, 64, 0xffff, 0);
            func_0200ad64(p, 1);
            break;
        }
        case 3:
            func_020059d0(&data_020997a4.mUnk_0008, data_020997a4.mUnk_11fc4[p->mUnk_00], 0);
            func_0200ad64(p, 0);
            func_0202f110(224, 100, 0, 64, 0xffff, 0);
            break;
        case 4:
            func_020059e8(&data_020997a4.mUnk_0004, 60000 * data_020997a4.mUnk_11fd8[p->mUnk_00] / 1000);
            sprintf(buf, "+%d'", data_020997a4.mUnk_11fd8[p->mUnk_00]);
            func_0200d838(buf, 2, data_0201a61c, 30, p->mPos.x >> 8, p->mPos.y >> 8, 0, -128, 100, 1, 0);
            func_0200ad64(p, 0);
            func_0202f110(224, 100, 0, 64, 0xffff, 0);
            break;
    }
}

void func_0200af28(Particle *p, int angle) {
    Particle q;
    u16 base;
    switch (p->mUnk_00) {
        case 3:
        case 4:
            func_0200adc0(p);
            return;
        case 1:
            base = func_0201fe8c((u16) angle, data_020997a4.mUnk_0230);
            q = *p;
            q.mUnk_00 = 0;
            q.mUnk_16 = 16;
            func_0200acfc(&q, base);
            func_0202de60(&q.mSprite, data_0201abec[q.mUnk_00 + 6]);
            func_0200aadc(&q, -1);
            q = *p;
            q.mUnk_00 = 0;
            q.mUnk_16 = 16;
            func_0200acfc(&q, (u16) (base - 0x2000));
            func_0202de60(&q.mSprite, data_0201abec[q.mUnk_00 + 6]);
            func_0200aadc(&q, -1);
            q = *p;
            q.mUnk_00 = 0;
            q.mUnk_16 = 16;
            func_0200acfc(&q, (u16) (base + 0x2000));
            func_0202de60(&q.mSprite, data_0201abec[q.mUnk_00 + 6]);
            func_0200aadc(&q, -1);
            func_020059d0(&data_020997a4.mUnk_0008, data_020997a4.mUnk_11fc4[p->mUnk_00], 0);
            func_0200ad64(p, 1);
            func_0202f110(217, 100, 0, 64, 0xffff, 0);
            return;
        case 0:
        case 2:
            func_020059d0(&data_020997a4.mUnk_0008, data_020997a4.mUnk_11fc4[p->mUnk_00], 0);
            func_0200ad64(p, 1);
            func_0202f110(217, 100, 0, 64, 0xffff, 0);
            return;
    }
}

void func_0200b0a0(void) {
    int i;
    for (i = 0; i < PARTICLE_COUNT; i++) {
        Particle *p = &data_020997a4.mParts[i];
        if (p->mUnk_14) {
            func_0202db1c(&p->mSprite);
            func_0202cf08(p->mSprite.mUnk_04 + (1 << 23), p->mPos.x >> 8, p->mPos.y >> 8, 100, 1);
        }
    }
}

void func_0200b0e0(void) {
    int i;
    for (i = 0; i < PARTICLE_COUNT; i++) {
        Particle *p = &data_020997a4.mParts[i];
        Vec2i old;
        if (p->mUnk_14 == 0) {
            continue;
        }
        old = p->mPos;
        p->mPos.x += p->mVel.x / 60;
        p->mPos.y += p->mVel.y / 60;
        if (p->mUnk_15 == 0) {
            if (p->mPos.x > 0 && p->mPos.x <= 0xff00 && p->mPos.y > 0 && p->mPos.y <= 0xbf00) {
                p->mUnk_15 = 1;
            }
        } else {
            int bounced = 0;
            int dx;
            int dy;
            int dist2;
            int flag;
            if (p->mPos.x < 0) {
                p->mPos.x = 0;
                p->mVel.x = -p->mVel.x;
                bounced = 1;
            } else if (p->mPos.x > 0xff00) {
                p->mPos.x = 0xff00;
                p->mVel.x = -p->mVel.x;
                bounced = 1;
            }
            if (p->mPos.y < 0) {
                p->mPos.y = 0;
                p->mVel.y = -p->mVel.y;
                bounced = 1;
            } else if (p->mPos.y > 0xc000) {
                p->mPos.y = 0xc000;
                p->mVel.y = -p->mVel.y;
                bounced = 1;
            }
            if (bounced) {
                func_0202f110(88, 100, 0, 64, 0xffff, 0);
            }
            dx = (p->mPos.x - data_020997a4.mUnk_01ac) >> 8;
            dy = (p->mPos.y - data_020997a4.mUnk_01b0) >> 8;
            dist2 = dx * dx + dy * dy;
            flag = 0;
            if (p->mUnk_16 == 0) {
                if (dist2 <= 576) {
                    u16 angle = 0xffff - func_0201feb0((old.x - data_020997a4.mUnk_01ac) >> 8,
                                                       (old.y - data_020997a4.mUnk_01b0) >> 8);
                    if (func_0201fe14(data_020997a4.mUnk_0230, angle) <= 16000) {
                        flag = 1;
                        func_0200af28(p, angle);
                    }
                }
            } else {
                p->mUnk_16--;
            }
            if (!flag && dist2 <= 196) {
                func_0200adc0(p);
            }
        }
    }
}

void func_0200b244(void) {
    if (data_020997a4.mCount < data_020997a4.mUnk_11fec) {
        if (data_020997a4.mUnk_11fc0 == 0) {
            func_0200aadc(0, -1);
            data_020997a4.mUnk_11fc0 = func_0201fd84(data_020997a4.mUnk_11ff8, data_020997a4.mUnk_11ffc);
        } else {
            data_020997a4.mUnk_11fc0--;
        }
    }
}

void func_0200b278(void) {
    s32 level = data_020b6ee0.mUnk_a8e4;
    int i;
    if (level < 0) {
        level = 0;
    }
    data_020997a4.mInfo = data_020b1990[level].mInfo;
    data_020b10fc.mUnk_08 = data_020997a4.mInfo.mUnk_11 - 1;
    {
        BOOL ok = 0;
        if (data_020b10fc.mUnk_08 >= 0 && data_020b10fc.mUnk_08 < 12) {
            ok = 1;
        }
        if (ok == 0) {
            OS_Terminate();
        }
    }
    func_0200c5d8(0, 0, data_020997a4.mInfo.mUnk_13 + 0xdb3, 0x100, 16, 1, 130);
    func_02024208();
    func_020104c0(2, 0x4c);
    data_020997a4.mUnk_0230 = 0;
    data_020997a4.mUnk_01ac = 0x8000;
    data_020997a4.mUnk_01b0 = 0x6000;
    data_020997a4.mUnk_023c = 0;
    data_020997a4.mUnk_0240 = 0;
    func_020271ec(&data_020997a4.mModel, 0, 0, 0, data_0201abbc[8]);
    for (i = 0; i < PARTICLE_COUNT; i++) {
        data_020997a4.mParts[i].mUnk_14 = 0;
    }
    data_020997a4.mCount = 0;
    data_020997a4.mUnk_11fc4[0] = func_0200a970(0, 2);
    data_020997a4.mUnk_11fd8[0] = func_0200a970(0, 3);
    data_020997a4.mUnk_11fc4[1] = func_0200a970(0, 4);
    data_020997a4.mUnk_11fd8[1] = func_0200a970(0, 5);
    data_020997a4.mUnk_11fc4[2] = func_0200a970(0, 6);
    data_020997a4.mUnk_11fd8[2] = func_0200a970(0, 7);
    data_020997a4.mUnk_11fc4[3] = func_0200a970(0, 8);
    data_020997a4.mUnk_11fd8[3] = func_0200a970(0, 9);
    data_020997a4.mUnk_11fc4[4] = func_0200a970(0, 10);
    data_020997a4.mUnk_11fd8[4] = func_0200a970(0, 11);
    data_020997a4.mUnk_11fec = func_0200a988(data_020b10fc.mUnk_08, 2);
    data_020997a4.mUnk_11ff0 = func_0200a988(data_020b10fc.mUnk_08, 3) * 60000 / 1000;
    data_020997a4.mUnk_11ff4 = func_0200a988(data_020b10fc.mUnk_08, 4);
    data_020997a4.mUnk_11ff8 = func_0200a988(data_020b10fc.mUnk_08, 5) * 60 / 1000;
    data_020997a4.mUnk_11ffc = func_0200a988(data_020b10fc.mUnk_08, 6) * 60 / 1000;
    data_020997a4.mWeights[0][0] = func_0200a988(data_020b10fc.mUnk_08, 7);
    data_020997a4.mWeights[1][0] = func_0200a988(data_020b10fc.mUnk_08, 8);
    data_020997a4.mWeights[2][0] = func_0200a988(data_020b10fc.mUnk_08, 9);
    data_020997a4.mWeights[0][1] = func_0200a988(data_020b10fc.mUnk_08, 10);
    data_020997a4.mWeights[1][1] = func_0200a988(data_020b10fc.mUnk_08, 11);
    data_020997a4.mWeights[2][1] = func_0200a988(data_020b10fc.mUnk_08, 12);
    data_020997a4.mWeights[0][2] = func_0200a988(data_020b10fc.mUnk_08, 13);
    data_020997a4.mWeights[1][2] = func_0200a988(data_020b10fc.mUnk_08, 14);
    data_020997a4.mWeights[2][2] = func_0200a988(data_020b10fc.mUnk_08, 15);
    data_020997a4.mWeights[0][3] = func_0200a988(data_020b10fc.mUnk_08, 16);
    data_020997a4.mWeights[1][3] = func_0200a988(data_020b10fc.mUnk_08, 17);
    data_020997a4.mWeights[2][3] = func_0200a988(data_020b10fc.mUnk_08, 18);
    data_020997a4.mWeights[0][4] = func_0200a988(data_020b10fc.mUnk_08, 19);
    data_020997a4.mWeights[1][4] = func_0200a988(data_020b10fc.mUnk_08, 20);
    data_020997a4.mWeights[2][4] = func_0200a988(data_020b10fc.mUnk_08, 21);
    data_020997a4.mUnk_0000 = 0;
    data_020997a4.mUnk_0004 = data_020997a4.mUnk_11ff0;
    data_020997a4.mUnk_0008 = 0;
    data_020997a4.mUnk_11fc0 = 180;
    func_02005a8c(1, 0);
}

void func_0200b53c(void) {
    func_0201168c(data_020b6ee0.mUnk_a8e4, data_020997a4.mUnk_0008, data_0206d448, data_020997a4.mUnk_0004, 2);
    func_0200aa68(data_020997a4.mInfo.mUnk_10 + 0xd28, 256, 0, 0x1000, 0x1000, 0, 255, 256);
    func_0200aa68(data_020997a4.mInfo.mUnk_13 + 0xd31, data_020997a4.mUnk_01ac >> 8, data_020997a4.mUnk_01b0 >> 8, 0,
                  0x6000, data_020997a4.mUnk_0230, 255, 0);
    func_0200aa68(0xd2e, data_020997a4.mUnk_01ac >> 8, data_020997a4.mUnk_01b0 >> 8, 0, 0x6000, 0, 255, 0);
    func_0200aa2c(&data_020997a4.mModel, data_020997a4.mUnk_01ac >> 8, data_020997a4.mUnk_01b0 >> 8, 0x6000,
                  data_020997a4.mUnk_0230, 256);
    func_0200b0a0();
}

void func_0200b610(void) {
    u8 state;
    func_020059cc(data_020997a4.mUnk_0008);
    state = func_02005bf8();
    if (state != 0) {
        int result;
        func_0200b53c();
        result = func_02005b1c(1);
        if (state == 2 && result == 0) {
            func_020059b4(func_02005c04(), data_020997a4.mUnk_0008);
        }
        return;
    }
    {
        s32 lim = 51200;
        s32 v = data_020997a4.mUnk_023c;
        if (v < lim) {
            lim = v;
        }
        if (lim < -51200) {
            v = -51200;
        } else if (v >= 51200) {
            v = 51200;
        }
        data_020997a4.mUnk_023c = v;
    }
    {
        s32 lim = 51200;
        s32 v = data_020997a4.mUnk_0240;
        if (v < lim) {
            lim = v;
        }
        if (lim < -51200) {
            v = -51200;
        } else if (v >= 51200) {
            v = 51200;
        }
        data_020997a4.mUnk_0240 = v;
    }
    data_020997a4.mUnk_01ac += data_020997a4.mUnk_023c / 60;
    data_020997a4.mUnk_01b0 += data_020997a4.mUnk_0240 / 60;
    {
        s32 lim = 0xff00;
        s32 v = data_020997a4.mUnk_01ac;
        if (v < lim) {
            lim = v;
        }
        if (lim < 0) {
            v = 0;
        } else if (v >= 0xff00) {
            v = 0xff00;
        }
        data_020997a4.mUnk_01ac = v;
    }
    {
        s32 v = data_020997a4.mUnk_01b0;
        s32 lim = 0xbf00;
        if (v < lim) {
            lim = v;
        }
        if (lim < 0) {
            v = 0;
        } else if (v >= 0xbf00) {
            v = 0xbf00;
        }
        data_020997a4.mUnk_01b0 = v;
    }
    if (data_020c18bc.mUnk_10 & 0x10) {
        data_020997a4.mUnk_023c += 0x1000;
    } else if (data_020c18bc.mUnk_10 & 0x20) {
        data_020997a4.mUnk_023c -= 0x1000;
    } else {
        func_0200b9cc(&data_020997a4.mUnk_023c, 0x300);
    }
    if (data_020c18bc.mUnk_10 & 0x80) {
        data_020997a4.mUnk_0240 += 0x1000;
    } else if (data_020c18bc.mUnk_10 & 0x40) {
        data_020997a4.mUnk_0240 -= 0x1000;
    } else {
        func_0200b9cc(&data_020997a4.mUnk_0240, 0x300);
    }
    if (data_020c18bc.mUnk_2ac_1) {
        u16 target = 0xffff - func_0201feb0(data_020c18bc.mUnk_2ae - (data_020997a4.mUnk_01ac >> 8),
                                            data_020c18bc.mUnk_2af - (data_020997a4.mUnk_01b0 >> 8));
        s32 angle = data_020997a4.mUnk_0230;
        func_0200b994(&angle, target, 2);
        data_020997a4.mUnk_0230 = angle;
    } else {
        u16 current = data_020997a4.mUnk_0230;
        s32 target = current;
        s32 angle;
        u16 keys = data_020c18bc.mUnk_10;
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
        func_0200b994(&angle, target, 2);
        data_020997a4.mUnk_0230 = angle;
    }
    func_0200b0e0();
    if (--data_020997a4.mUnk_0004 == 0) {
        func_02005a8c(0, data_020997a4.mUnk_0008 >= data_020997a4.mUnk_11ff4);
    }
    func_0200b244();
    func_0200b53c();
}

void func_0200b884(void) {
    func_020102f8();
}

int func_0200b88c(u8 stage) {
    return func_0200a988(stage, 4);
}
