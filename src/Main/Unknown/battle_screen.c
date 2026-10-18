#include <Main/Unknown/battle_screen.h>
#include <Main/Unknown/functions.h>
#include <Main/Unknown/stage_progress.h>
#include <Main/Unknown/stage_select.h>
#include <stdio.h>
#include <Main/Unknown/game_util.h>
#include <Main/Unknown/globals.h>
#include <Main/Unknown/language_menu.h>
#include <Main/Unknown/prompt_screen.h>
#include <Main/Unknown/sound_control.h>
#include <Main/Unknown/state_flow.h>
#include <Main/clib.h>
#include <nitro/fx.h>
#include <nitro/os.h>

// Cursor step that confirms the stage choice in func_02009728 (the other steps are -1, 0 and 1).
#define SELECT_CONFIRM 0x55

s32 data_02098d80;
BattleView data_02098d84;
StageRec data_020990e0[3];

void func_020071a8(void) {}

void func_020071ac(void) {
    func_02020944(0x190, 0, 0xa2, 0, 0, 0xd5, 0, 0x17d);
    func_0202cf08((data_020c2660.mUnk_04 == 0x40000190) + 0xd5, 0, 0xa2, 0x8e, 2);
}

void func_020071f8(void) {
    func_02020944(0xc8, 0xff, 0xa2, 0, 0, 0x15, 0, 0x17d);
    func_0202cf08((data_020c2660.mUnk_04 == 0x400000c8) + 0x15, 0xff, 0xa2, 0x8e, 2);
}

void func_02007244(int a) {
    s32 v;
    func_02007ab0();
    func_02027230(&data_020b6ee0.mModel[0], 0, 0, 0, data_0201aa60[a]);
    data_020b6ee0.mModel[0].mUnk_175 = 1;
    v = data_020b6ee0.mModel[0].mUnk_010;
    if (v != 0) {
        func_020271ec(&data_020b6ee0.mModel[1], v, v + 1, 4, 0);
    } else {
        data_020b6ee0.mModel[1].mUnk_170 = 0;
    }
}

void func_020072a8(void) {
    int flags = 0;
    u16 color = func_0200be10(0x1f, 0x15, 0xc);
    u16 count;
    func_0200be20(flags, 0xf618, 14 << 12, color);
    count = data_020b6ee0.mModel[1].mUnk_170;
    flags |= 8;
    if (count != 0) {
        s32 half = (data_020b6ee0.mModel[0].mUnk_14c.mUnk_0 - data_020b6ee0.mModel[0].mUnk_170) / 2;
        data_020b6ee0.mModel[1].mUnk_14c = data_020b6ee0.mModel[0].mUnk_14c;
        data_020b6ee0.mModel[1].mUnk_14c.mUnk_0 = count + half + 1;
        data_020b6ee0.mModel[1].mUnk_152 = data_020b6ee0.mModel[1].mUnk_14c;
        data_020b6ee0.mModel[1].mUnk_172 |= 0x80;
        func_0202733c(&data_020b6ee0.mModel[1], &data_020b6ee0.mUnk_f9c, flags);
        func_020105f4(&data_020b6ee0.mModel[1]);
    }
    data_020b6ee0.mModel[0].mUnk_172 |= 0x80;
    func_0202733c(&data_020b6ee0.mModel[0], &data_020b6ee0.mUnk_f9c, flags);
    func_020105f4(&data_020b6ee0.mModel[0]);
    if (data_020b6ee0.mModel[0].mUnk_022 >= 0) {
        func_02007a1c(data_020b6ee0.mModel[0].mUnk_022);
        data_020b6ee0.mModel[0].mUnk_022 = -1;
    }
    func_02007abc();
}

void func_02007370(void) {
    func_02031fc4(0, &data_020c5e44.mUnk_b0);
    func_02031fc4(1, &data_020c5e44.mUnk_a4);
    func_02033488();
}

void func_02007390(void) {
    func_02031fc4(0, &data_020c5e44.mUnk_b0);
    func_02031fc4(2, &data_020c5e44.mUnk_a4);
    func_02033488();
}

void func_020073b0(void) {
    VecFx32 pos;
    s32 a, b, c;
    u16 v[4];
    v[0] = 0;
    {
        u8 table[17] = {0, 0, 1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0};
        func_0202601c(data_020b6ee0.mModel[0].mUnk_004,
                      table[data_020b6ee0.mModel[0].mUnk_142 & 0x1f] == 0 ? &data_020b6ee0.mModel[0].mUnk_14c.mUnk_2
                                                                          : &v[0],
                      data_020b6ee0.mModel[0].mUnk_14c.mUnk_4, 0, &a, &b, &c, 1);
    }
    func_02023478(data_020b6ee0.mModel[0].mUnk_004, NULL, &pos, 0, a, b, c, 2);
    data_020c5e44.mUnk_b0.x = pos.x + data_020b6ee0.mUnk_f9c.x;
    data_020c5e44.mUnk_b0.z = pos.z + data_020b6ee0.mUnk_f9c.z;
    data_020c5e44.mUnk_b0.y = pos.y + data_020b6ee0.mUnk_f9c.y;
    func_02023478(data_020b6ee0.mModel[0].mUnk_004, &v[1], &pos, 0, a, b, c, 1);
    data_020c5e44.mUnk_a4.x = pos.x + data_020b6ee0.mUnk_f9c.x;
    data_020c5e44.mUnk_a4.z = pos.z + data_020b6ee0.mUnk_f9c.z;
    data_020c5e44.mUnk_a4.y = pos.y + data_020b6ee0.mUnk_f9c.y;
    func_02033488();
    if ((v[1] >> 9) > 5 && (v[1] >> 9) < 0x78) {
        func_02033df4(v[1] >> 9);
    }
}

void func_020074ac(void) {
    u16 a[6] = {0xf87, 0xf49, 0xf98, 0xf76, 0xf65, 0xfa8};
    u16 b[6] = {0xf88, 0xf4a, 0xf99, 0xf77, 0xf66, 0xfa9};
    int sel;
    u32 i;
    func_0201eb20();
    func_0202c1c4();
    func_02022370();
    func_0202bdd8(0x10cb, 0x10ca, 0x10cc, 0x80, 1);
    func_0202bdd8(0x10cb, 0x10ca, 0x10cc, 0x82, 2);
    func_02021fb0(0x1c7, 0x1c6, 0x100, 16);
    func_02020854(1);
    {
        BOOL ok = 0;
        if (data_020b6ee0.mUnk_a534 >= 0 && data_020b6ee0.mUnk_a534 < 25) {
            ok = 1;
        }
        if (ok == 0) {
            OS_Terminate();
        }
    }
    sel = func_02014538(data_020b6ee0.mUnk_a534, 14);
    if (!((u32) sel < 6)) {
        OS_Terminate();
    }
    func_0201dde0(sel + 0xf5f, 2);
    func_02024208();
    func_020104c0(1, 0x4c);
    data_020b6ee0.mUnk_fe0 = 21;
    data_020b6ee0.mUnk_fe2 = 0;
    data_020b6ee0.mUnk_fe4 = 0;
    data_02098d84.mState.mUnk_14 = 2;
    data_02098d84.mState.mUnk_0b = 0x7f;
    data_02098d84.mState.mUnk_0c = data_02098d84.mState.mUnk_0b;
    data_02098d84.mState.mUnk_06 = 0;
    data_02098d84.mState.mUnk_0d = func_02014538(data_020b6ee0.mUnk_a534, 18);
    data_02098d84.mState.mUnk_0e[1] = 0;
    data_02098d84.mState.mUnk_0e[0] = data_02098d84.mState.mUnk_0e[1];
    data_02098d84.mState.mUnk_06 = 0;
    data_02098d84.mModel[1].mUnk_170 = 0;
    memset(&data_020b6ee0.mUnk_fee, 0, 0x12);
    func_020151f8();
    for (i = 0; i < 3; i++) {
        StageRec *rec = func_02015190(i);
        if (rec != NULL) {
            rec = func_02015190(i);
            memcpy(&data_020990e0[i], rec, sizeof(StageRec));
            data_020990e0[i].mInfo.mUnk_1b = ((u32) rec - (u32) data_020b1990) / sizeof(StageRec);
        } else {
            data_020990e0[i].mInfo.mUnk_04 = -9999;
        }
        data_020b1524[i].mUnk_1b = -1;
    }
    for (i = 0; i < 39; i++) {
        data_020b6ee0.mUnk_a8f3[i] = 2;
    }
    func_020220c8(0x10d0);
    func_020325b0(a[sel], 0, b[sel], 0, 0, 0);
    data_020c5e44.mUnk_28 = func_020071a8;
    data_020c6144.mUnk_54 = 25 << 6;
    data_020c6144.mUnk_44 = 0x128e000;
    data_020c5e44.mUnk_bc = 1;
    memset(&data_020b6ee0.mUnk_f9c, 0, 0x44);
    data_020b6ee0.mUnk_fc8.x = -1;
    data_020b6ee0.mUnk_fc8.y = -1;
    data_020b6ee0.mUnk_fc8.z = -1;
    func_02031fc4(0, &data_020b6ee0.mUnk_f9c);
    data_020b6ee0.mUnk_fdf &= ~7;
    func_02007244(data_020990e0[0].mInfo.mUnk_13);
    data_020b6ee0.mUnk_a6d8[0].mUnk_de = 1;
    data_020b6ee0.mUnk_a6d8[1].mUnk_de = 1;
}

void func_02007704(void) {
    switch (data_020b6ee0.mUnk_fe0) {
        case 21:
            func_02009fe8();
            break;
        case 0:
            func_02009728();
            break;
        case 1:
            func_02008994((data_02098d84.mState.mUnk_15 ^ data_02098d84.mState.mUnk_14) & 1);
            break;
        case 10:
            func_02008288();
            break;
    }
}

void func_02007750(void) {
    func_020102f8();
}

int func_02007758(void) {
    if ((data_020b6ee0.mUnk_a8f3[0] & 0x80) == 0) {
        return 1;
    }
    return 0;
}

void func_02007770(void) {
    int i;
    int base = data_02098d84.mState.mUnk_0d * 7;
    for (i = 0; i < 3; i++) {
        data_020990e0[i].mInfo.mUnk_06 = func_02021e44(0x1e7, base + (data_020990e0[i].mInfo.mUnk_10 + 3));
        data_020b1524[i].mUnk_06 = func_02021e44(0x1e7, base + (data_020b1524[i].mUnk_10 + 3));
    }
    data_02098d84.mState.mUnk_04 = 0;
}

int func_020077d0(StageInfo *info) {
    int percent = func_02021e44(0x1e8, 19);
    return func_020079a0(info) * percent / 100;
}

void func_020077f0(int rec, int slot) {
    int i;
    memcpy(&data_02098d84.mSide[0], &data_020990e0[rec].mInfo, sizeof(StageInfo));
    memcpy(&data_02098d84.mSide[1], &data_020b1524[slot], sizeof(StageInfo));
    data_020b6ee0.mUnk_a8f3[rec] &= ~2;
    for (i = 0; i < 2; i++) {
        data_02098d84.mSide[i].mUnk_16 = 0;
        data_02098d84.mSide[i].mUnk_15 = 0;
        data_02098d84.mSide[i].mUnk_0a = 0;
        data_02098d84.mSide[i].mUnk_0c = 0;
        data_02098d84.mSide[i].mUnk_19 = 0;
        data_02098d84.mSide[i].mUnk_0e = 0;
    }
    for (i = 0; i < 3; i++) {
        if (i != rec && data_020990e0[i].mInfo.mUnk_04 > -5000) {
            data_02098d84.mSide[0].mUnk_0c += func_020079cc(&data_020990e0[i].mInfo) / 2;
        }
        {
            StageInfo *p = &data_020b1524[i];
            if (i != slot && p->mUnk_04 > -5000) {
                data_02098d84.mSide[1].mUnk_0c += func_020079cc(p) / 2;
            }
        }
    }
    // i is 3 here, one past mSide[1]: the original writes this word too.
    data_02098d84.mSide[i].mUnk_08 = 0;
}

int func_020078f8(void) {
    int i;
    for (i = 0; i < 2; i++) {
        u8 v = func_02021e44(0x10d5, data_02098d84.mSide[i].mUnk_10 * 2 + 2);
        if (v == data_02098d84.mSide[i ^ 1].mUnk_10) {
            return data_02098d84.mSide[i ^ 1].mUnk_10;
        }
    }
    return -1;
}

void func_02007940(void) {
    int i;
    for (i = 0; i < 2; i++) {
        u8 v = func_02021e44(0x10d5, data_02098d84.mSide[i].mUnk_10 * 2 + 2);
        if (v == data_02098d84.mSide[i ^ 1].mUnk_10) {
            data_02098d84.mSide[i].mUnk_08 = func_020077d0(&data_02098d84.mSide[i ^ 1]);
        } else {
            data_02098d84.mSide[i].mUnk_08 = 0;
        }
    }
}

int func_020079a0(const StageInfo *info) {
    int sum = info->mUnk_0a + (info->mUnk_04 + info->mUnk_06 - info->mUnk_08);
    if (info->mUnk_19 == 1) {
        sum += info->mUnk_0c;
    }
    if (sum < 0) {
        sum = 0;
    }
    return sum;
}

int func_020079cc(const StageInfo *info) {
    if (info->mUnk_04 < -5000) {
        return -9999;
    }
    return info->mUnk_04 + info->mUnk_06;
}

int func_020079ec(int id) {
    int row = 0;
    if (id != func_02021e44(0x1d9, 2)) {
        int index = 2;
        do {
            index += 20;
            row++;
        } while (id != func_02021e44(0x1d9, index));
    }
    return row;
}

void func_02007a1c(int a) {
    switch (a) {
        case 0:
            func_020271ec(&data_02098d84.mModel[1], 0x239, 0x238, 2, 0);
            data_02098d84.mState.mUnk_0a = 0;
            break;
        case 1:
            func_020271ec(&data_02098d84.mModel[1], 0x23f, 0x23e, 1, 0);
            data_02098d84.mState.mUnk_0a = 1;
            break;
        case 2:
            func_020271ec(&data_02098d84.mModel[1], 0x23d, 0x23c, 1, 0);
            data_02098d84.mState.mUnk_0a = 0;
            break;
        case 3:
            func_020271ec(&data_02098d84.mModel[1], 0x23b, 0x23a, 1, 0);
            data_02098d84.mState.mUnk_0a = 0;
            break;
    }
}

void func_02007ab0(void) {
    data_02098d84.mModel[1].mUnk_170 = 0;
}

void func_02007abc(void) {
    if (data_02098d84.mModel[1].mUnk_170 != 0) {
        if (data_02098d84.mState.mUnk_0a == 0) {
            func_02024368(&data_02098d84.mModel[1], 0x80, 0x60, 0x1000, 0, 0x100);
        } else {
            func_0202733c(&data_02098d84.mModel[1], &data_020b6ee0.mUnk_f9c, 0);
        }
        if (data_02098d84.mModel[1].mUnk_172_10 == 1) {
            data_02098d84.mModel[1].mUnk_170 = 0;
        }
    }
}

int func_02007b18(int side, int row) {
    StageInfo *mine = &data_02098d84.mSide[side];
    StageInfo *other = &data_02098d84.mSide[side ^ 1];
    int base = row * 20;
    int kind = func_02021e44(0x1d9, base + 3);
    int p1 = func_02021e44(0x1d9, base + 4);
    int p2 = func_02021e44(0x1d9, base + 5);
    int myPower = func_020079a0(mine);
    int result = myPower;
    int otherPower = func_020079a0(other);
    switch (kind) {
        case 0:
        case 1:
            if (mine->mUnk_19 == 0 && myPower < otherPower) {
                int boosted = myPower + mine->mUnk_0c;
                if (boosted > otherPower) {
                    result = boosted;
                }
            }
            break;
        case 3:
        case 4:
            if (other->mUnk_15 == 1 && data_02098d84.mState.mUnk_15 < 2) {
                result = myPower + func_0201fb74(data_020b6ee0.mUnk_a534, 25, 40, 150) * 3;
            }
            break;
        case 5:
            if (mine->mUnk_08 > 0) {
                result = myPower + mine->mUnk_08 * 2;
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
            result = myPower - mine->mUnk_06;
            otherPower = otherPower - other->mUnk_06;
            break;
        case 8:
            result = myPower + mine->mUnk_06;
            otherPower = otherPower + other->mUnk_06;
            break;
        case 2:
            if (other->mUnk_0e > 0) {
                otherPower = other->mUnk_0e;
            }
            break;
        case 13:
            if (mine->mUnk_15 == 1) {
                p1 *= 3;
            }
            result += p1;
            break;
        case 14:
            if (mine->mUnk_15 == 1) {
                p1 *= 3;
            }
            result += p1;
            if (other->mUnk_10 == mine->mUnk_10) {
                otherPower += p1;
            }
            break;
        case 15:
            if (other->mUnk_10 == p2) {
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

void func_02007c84(void) {
    int side = data_02098d84.mUnk_33c;
    StageInfo *mine = &data_02098d84.mSide[side];
    StageInfo *other = &data_02098d84.mSide[side ^ 1];
    int base = data_02098d84.mState.mUnk_00 * 20;
    int kind;
    int p1;
    int p2;
    int done = 0;
    kind = func_02021e44(0x1d9, base + 3);
    p1 = func_02021e44(0x1d9, base + 4);
    p2 = func_02021e44(0x1d9, base + 5);
    switch (kind) {
        case 0:
        case 1:
            mine->mUnk_19 = 1;
            done = 1;
            break;
        case 3:
            data_02098d84.mState.mUnk_15 = 4;
            done = 1;
            break;
        case 4:
            other->mUnk_16 = 1;
            done = 1;
            break;
        case 5:
            if (mine->mUnk_08 > 0) {
                mine->mUnk_08 = -mine->mUnk_08;
                done = 1;
            }
            break;
        case 11:
            other->mUnk_0a = -func_020079a0(other);
            done = 1;
            break;
        case 6: {
            int myPower = func_020079a0(mine);
            int otherPower = func_020079a0(other);
            mine->mUnk_0a += otherPower - myPower;
            other->mUnk_0a += myPower - otherPower;
            done = 1;
            break;
        }
        case 7:
            mine->mUnk_06 = 0;
            other->mUnk_06 = 0;
            done = 1;
            break;
        case 8:
            mine->mUnk_06 <<= 1;
            other->mUnk_06 <<= 1;
            done = 1;
            break;
        case 2:
            if (other->mUnk_0e > 0) {
                other->mUnk_0a += other->mUnk_0e - func_020079a0(other);
            }
            done = 0;
            break;
        case 12:
            mine->mUnk_15 = 1;
            done = 1;
            break;
        case 13:
            if (mine->mUnk_15 == 1) {
                p1 *= 3;
                mine->mUnk_15 = 0;
            }
            mine->mUnk_04 += p1;
            func_0202f110(p1 > 0 ? 0xe2 : 0xe1, 100, 0, 64, 0xffff, 0);
            done = 1;
            break;
        case 14:
            if (mine->mUnk_15 == 1) {
                p1 *= 3;
                mine->mUnk_15 = 0;
            }
            mine->mUnk_04 += p1;
            func_0202f110(p1 > 0 ? 0xe2 : 0xe1, 100, 0, 64, 0xffff, 0);
            if (other->mUnk_10 == mine->mUnk_10) {
                other->mUnk_04 += p1;
            }
            done = 1;
            break;
        case 15:
            if (other->mUnk_10 == p2) {
                if (mine->mUnk_15 == 1) {
                    p1 *= 3;
                    mine->mUnk_15 = 0;
                }
                mine->mUnk_04 += p1;
                func_0202f110(p1 > 0 ? 0xe1 : 0xe2, 100, 0, 64, 0xffff, 0);
                done = 1;
            }
            break;
        case 16:
            if (mine->mUnk_15 == 1) {
                p1 *= 3;
                mine->mUnk_15 = 0;
            }
            if (mine->mUnk_1b != -1) {
                func_0202f110(p1 > 0 ? 0xe2 : 0xe1, 100, 0, 64, 0xffff, 0);
            } else if (other->mUnk_1b != -1) {
                func_0202f110(p1 > 0 ? 0xe1 : 0xe2, 100, 0, 64, 0xffff, 0);
            }
            other->mUnk_04 -= p1;
            mine->mUnk_04 += p1;
            done = 1;
            break;
    }
    data_02098d84.mState.mUnk_13 = done;
}

int func_02007ed4(void) {
    int i;
    int j;
    int bestDiff;
    int bestOther;
    int bestOwn;
    StageInfo *own;
    i = 0;
    bestDiff = -5000;
    own = data_020b1524;
    for (; i < 3; own++, i++) {
        StageRec *rec = data_020990e0;
        for (j = 0; j < 3; j++, rec++) {
            int a = func_020079cc(own);
            int b = func_020079cc(&rec->mInfo);
            if (a > -5000 && b > -5000 && a - b > bestDiff) {
                bestDiff = a - b;
                bestOther = j;
                bestOwn = i;
            }
        }
    }
    if (data_02098d84.mState.mUnk_0e[0] != bestOther) {
        return 1;
    }
    if (data_02098d84.mState.mUnk_0e[1] != bestOwn) {
        return 2;
    }
    return 0;
}

int func_02007f64(void) {
    StageInfo *side = &data_02098d84.mSide[1];
    int best = -5000;
    int bestIndex = -5000 >> 13;
    int i;
    if (side->mUnk_16 != 1) {
        for (i = 0; i < 5; i++) {
            if (side->mUnk_1c[i] >= 0) {
                int score = func_02007b18(1, func_020079ec(side->mUnk_1c[i]));
                if (score > best) {
                    best = score;
                    bestIndex = i;
                }
            }
        }
    }
    return bestIndex;
}

void func_02007fa4(int which, int flag) {
    int i;
    if (flag == 1) {
        for (i = 0; i < 3; i++) {
            StageInfo *p = (which == 0) ? &data_020990e0[i].mInfo : &data_020b1524[i];
            if (p->mUnk_04 > -5000) {
                p->mUnk_04 -= (p->mUnk_04 + p->mUnk_06) / 2;
            }
        }
    }
}

void func_02007ffc(u16 *a, u16 *b) {
    int x = func_0201fb74(*a, 0x4000, 150, 0);
    int y = func_0201fb74(*b, 0x4000, 150, 0);
    int z = func_02014538(data_020b6ee0.mUnk_a534, 7);
    func_0202cf08(0x1195, x + 255, 44, 150, 2);
    func_0202cf08(0x1196, x + 255, 55, 150, 2);
    y = -y;
    func_0202cf08(z + 0x11b6, y, 44, 150, 2);
    func_0202cf08(z + 0x11ba, y, 55, 150, 2);
}

void func_02008090(StageInfo *info) {
    char numBuf[8];
    char textBuf[8];
    func_020079cc(info);
    func_0202cf08(0x801082 + info->mUnk_10, 0, 30, 150, 1);
    func_020215fc(5, 19, data_0201a61c, func_0200f9f0(info->mUnk_13 + 98), 0, 250, 0, 1);
    func_0202cf08(0x80113f, 5, 191, 150, 1);
    func_0202cf08(0xf8d, 0, 121, 250, 1);
    func_020215fc(37, 131, data_0201a61c, func_0200f9f0(13), 2, 250, 0, 1);
    func_0202cf08(0xf4a, 39, 163, 250, 1);
    func_0202cf08(0xf4a + info->mUnk_11, 39, 163, 250, 1);
    func_0202cf08(0xf38, 71, 139, 148, 1);
    func_020215fc(133, 131, data_0201a61c, func_0200f9f0(24), 2, 250, 0, 1);
    func_0202cf08(0xf42 + info->mUnk_10, 116, 170, 250, 1);
    *func_0201fc08(func_020079cc(info), numBuf) = 0;
    func_020215fc(116, 170, data_0201a694, numBuf, 0, 250, 10, 1);
    if (func_020150c0(info)) {
        func_0202cf08(0xf57 + (data_020c18bc.mUnk_45c % 60 <= 3), 130, 187, 140, 1);
    }
    sprintf(textBuf, "+%d", func_020077d0(info));
    func_020215fc(190, 164, data_0201a68a, textBuf, 0, 250, 10, 1);
    func_020215fc(232, 179, data_0201a61c, func_0200f9f0(15), 1, 250, 0, 1);
    func_0202cf08(func_02021e44(0x10d5, info->mUnk_10 * 2 + 3) + 0xf32, 242, 175, 248, 1);
}

void func_0200827c(void) {
    func_0200fb50(1);
}

void func_02008288(void) {
    char text[8];
    s32 cur[2];
    s32 hp[2];
    int sideA = data_02098d84.mState.mUnk_0e[0];
    int sideB = data_02098d84.mState.mUnk_0e[1];
    int i;
    int x;
    int y;
    StageInfo *info;
    StageInfo *pa = &data_02098d84.mSide[0];
    StageInfo *pb = &data_02098d84.mSide[1];
    hp[0] = func_020079a0(pa);
    hp[1] = func_020079a0(pb);
    func_02007ffc(&data_020b6ee0.mUnk_fe2, &data_020b6ee0.mUnk_fe4);
    if (data_020b6ee0.mUnk_fee == 0x4000) {
        func_0201fb5c(&data_020b6ee0.mUnk_ff0, 400);
    } else if (data_020b6ee0.mUnk_fee != 0) {
        if (hp[0] > hp[1]) {
            if (data_020b1524[sideB].mUnk_04 > -5000) {
                func_0202de60(&data_020b6ee0.mUnk_a6d8[1], data_0201abec[pb->mUnk_10 + 33]);
                data_020990e0[sideA].mInfo.mUnk_04 = hp[0] - hp[1] - data_020990e0[sideA].mInfo.mUnk_06;
                memcpy(data_020990e0[sideA].mInfo.mUnk_1c, data_02098d84.mSide[0].mUnk_1c, 10);
                data_020b1524[sideB].mUnk_04 = -9999;
                data_02098d84.mState.mUnk_16 = 0;
                data_020b6ee0.mUnk_a8f3[sideA] |= 4;
                func_02007fa4(1, pb->mUnk_19);
                pb->mUnk_19 = 0;
            }
        } else if (data_020990e0[sideA].mInfo.mUnk_04 > -5000) {
            data_020b1524[sideB].mUnk_04 = hp[1] - hp[0] - data_020b1524[sideB].mUnk_06;
            memcpy(data_020b1524[sideB].mUnk_1c, data_02098d84.mSide[1].mUnk_1c, 10);
            data_020990e0[sideA].mInfo.mUnk_04 = -9999;
            data_02098d84.mState.mUnk_16 = 1;
            data_020b6ee0.mUnk_a8f3[sideA] &= ~4;
            func_02007fa4(0, pa->mUnk_19);
            pa->mUnk_19 = 0;
            func_0202de60(&data_020b6ee0.mUnk_a6d8[0], data_0201abec[pa->mUnk_10 + 33]);
            if (hp[1] == 0) {
                data_020b1524[sideB].mUnk_04 = -9999;
                func_0202de60(&data_020b6ee0.mUnk_a6d8[1], data_0201abec[pb->mUnk_10 + 33]);
            }
        }
        if (hp[0] >= hp[1]) {
            if (data_02098d84.mSide[0].mUnk_11 < data_02098d84.mSide[1].mUnk_11) {
                data_020b6ee0.mUnk_a8f3[sideA] |= 1;
            }
        }
    }
    i = 0;
    {
        u8 startX[2] = {0x9d, 0x61};
        u8 startY[2] = {0x5d, 0x5d};
        info = &data_02098d84.mSide[0];
        for (; i < 2; info++, i++) {
            if (hp[i] > hp[i ^ 1] || data_020b6ee0.mUnk_fee < 10) {
                x = func_0201fb74(data_020b6ee0.mUnk_ff0, 0x4000, startX[i], info->mUnk_17);
                y = func_0201fb74(data_020b6ee0.mUnk_ff0, 0x4000, startY[i], info->mUnk_18);
                func_0202cf08(info->mUnk_13 + ((i == 1 ? 0x10000 : 0) + 0x1089), x, y, 250, 2);
                func_0202cf08(0x41088, x, y, 252, 2);
                if (data_020b6ee0.mUnk_a6d8[i].mUnk_de == 0) {
                    func_0202cf08(func_0202db1c(&data_020b6ee0.mUnk_a6d8[i]), x, y, 240, 2);
                }
                cur[i] = func_020079a0(info);
                *func_0201fc08(cur[i], text) = 0;
                func_020215fc(x - 10, y - 40, data_0201a6bc, text, 0, 96, 0, 2);
                func_0202cf08(info->mUnk_10 + 0x11a1, x - 10, y - 40, 240, 2);
                if (data_02098d84.mState.mUnk_18[i] < 18) {
                    func_0202cf08(data_02098d84.mState.mUnk_18[i]++ / 3 + 0x11a7, x - 10, y - 40, 100, 2);
                }
            } else {
                int r = func_0201fb74(data_020b6ee0.mUnk_fee, 0x4000, 0, 20);
                if (data_020b6ee0.mUnk_fee < 0x4000) {
                    func_0202cf08(r + 0xfec + info->mUnk_10 * 21, startX[i], startY[i], 100, 2);
                    if (r == 1 && data_02098d80 == 0) {
                        data_02098d80 = 1;
                        func_0202f110(42, 100, 0, 64, 0xffff, 0);
                    } else if (r > 1) {
                        data_02098d80 = 0;
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
                data_02098d84.mSide[0].mUnk_08 += a;
                data_02098d84.mSide[1].mUnk_08 += a;
            }
        } else {
            data_02098d84.mState.mUnk_18[1] = 0;
            data_02098d84.mState.mUnk_18[0] = data_02098d84.mState.mUnk_18[1];
            *(vu16 *) 0x04001050 = 0x2848;
            *(vu16 *) 0x04001052 = 0;
        }
    }
    if (data_020b6ee0.mUnk_ff0 < 0x4000) {
        if (!(cur[0] > 0 && cur[1] > 0) && data_020b6ee0.mUnk_fee < 10) {
            s16 phase = data_020b6ee0.mModel[0].mUnk_00c;
            if (phase == 0) {
                func_02007244(cur[0] <= 0 ? data_02098d84.mSide[1].mUnk_13 : data_02098d84.mSide[0].mUnk_13);
                data_020b6ee0.mModel[0].mUnk_134 |= 0x10;
                data_020b6ee0.mUnk_fee = 1;
            } else if (phase == 1 &&
                       (data_020b6ee0.mModel[0].mUnk_172_10 == 1 || data_020c2660.mUnk_04 == 0x20000190)) {
                func_02007244(cur[0] <= 0 ? data_02098d84.mSide[0].mUnk_13 : data_02098d84.mSide[1].mUnk_13);
                data_020b6ee0.mModel[0].mUnk_134 |= 2;
            } else if (phase == 2 &&
                       (data_020b6ee0.mModel[0].mUnk_172_10 == 1 || data_020c2660.mUnk_04 == 0x20000190)) {
                int id = func_02014538(data_020b6ee0.mUnk_a534, 7) + 23;
                func_0201fb5c(&data_020b6ee0.mUnk_fee, 480);
                func_0201f880(1);
                if (cur[0] <= 0) {
                    id = 11;
                }
                data_020b6ee0.mModel[0].mUnk_172 |= 0x400;
                func_0200f848(id, 0);
            }
        }
        if (data_020b6ee0.mModel[0].mUnk_00c != 2 || data_020b6ee0.mModel[0].mUnk_172_10 != 1) {
            func_020073b0();
            func_02033614(data_020c5e44.mUnk_a4.x, data_020c5e44.mUnk_a4.z);
            func_020071ac();
            func_020072a8();
        } else {
            func_0200f898();
            func_0201fb5c(&data_020b6ee0.mUnk_fee, 480);
        }
    }
    if (data_020b6ee0.mUnk_fee == 0x4000 && data_020b6ee0.mUnk_ff0 == 0x4000) {
        if (data_020990e0[0].mInfo.mUnk_04 < -5000 && data_020990e0[1].mInfo.mUnk_04 < -5000 &&
            data_020990e0[2].mInfo.mUnk_04 < -5000) {
            data_020b6ee0.mUnk_a8f3[0] &= ~0x80;
            func_0200827c();
            return;
        }
        if (data_020b1524[0].mUnk_04 < -5000 && data_020b1524[1].mUnk_04 < -5000 && data_020b1524[2].mUnk_04 < -5000) {
            data_020b6ee0.mUnk_a8f3[0] |= 0x80;
            func_0200827c();
            return;
        }
        data_02098d84.mState.mUnk_14 ^= 1;
        data_020b6ee0.mUnk_fe0 = 0;
        data_02098d84.mState.mUnk_1b = -1;
        data_020b6ee0.mUnk_ff0 = 0;
        func_0201f880(1);
    }
}

int func_020088d8(StageInfo *info, int mode) {
    if (info->mUnk_16 == 1 || (mode == 0 && (data_020c18bc.mUnk_12 & 1)) || data_020c2660.mUnk_04 == 0x200000c8 ||
        (mode == 1 && data_020b6ee0.mUnk_ff4++ > 90)) {
        return 1;
    }
    return 0;
}

int func_02008920(int side) {
    if (data_02098d84.mState.mUnk_14 ^ (side == 0)) {
        return 1;
    }
    return func_02014538(data_020b6ee0.mUnk_a534, 7) + 2;
}

int func_02008950(int side) {
    static const s32 data_0204e19c[4] = {2, 0x1a1, 0x33, 0x17e};
    if (data_02098d84.mState.mUnk_14 ^ (side == 0)) {
        return data_020b1084.mLanguage + 0x6b;
    }
    return data_020b1084.mLanguage + data_0204e19c[func_02014538(data_020b6ee0.mUnk_a534, 7)];
}

void func_02008994(int side) {
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

    func_0201fb5c(&data_020b6ee0.mUnk_fee, 400);
    active = 0;
    if (data_020b6ee0.mUnk_fee == 0x4000) {
        func_02007ffc(&data_020b6ee0.mUnk_fe2, &data_020b6ee0.mUnk_fe4);
        if (data_020b6ee0.mUnk_ff6 == 0x4000) {
            func_0201fb5c(&data_020b6ee0.mUnk_ff0, 400);
            if (data_020b6ee0.mUnk_ff0 < 0x4000) {
                *(vu16 *) 0x04001052 = func_0201fb74(data_020b6ee0.mUnk_ff0, 0x4000, 0, 16);
            } else if (data_020b6ee0.mUnk_ff8 == 0x4000) {
                int a;
                func_0201fb24(&data_020b6ee0.mUnk_ff2, 4);
                a = func_0201fb74(data_020b6ee0.mUnk_ff2, 0x4000, 0, 8);
                *(vu16 *) 0x04001050 = 0x2840;
                *(vu16 *) 0x04001052 = (a << 8) + 16 - a;
            } else if (data_020b6ee0.mUnk_ff8 == 0) {
                j = func_020078f8();
                if (j != -1) {
                    func_0201f880(1);
                    func_02022120();
                    func_0202224c();
                    func_0200f848(j + 12, 0);
                    data_020b6ee0.mUnk_ff8 = 1;
                    data_02098d84.mState.mUnk_1a = data_02098d84.mSide[0].mUnk_10 == j;
                    data_02098d84.mState.mUnk_08 =
                        func_020077d0(&data_02098d84.mSide[(data_02098d84.mState.mUnk_1a & 0x7f) ^ 1]);
                } else {
                    data_020b6ee0.mUnk_ff8 = 0x4000;
                }
            } else if (data_020b6ee0.mUnk_ff8 == 1) {
                func_0200f898();
                if (func_0200f97c() > 50) {
                    int t = func_0201fbe4(func_0200f97c() < 100 ? func_0200f97c() : 100, 50, 100, 100, 0);
                    *func_0201fc08(func_020079a0(&data_02098d84.mSide[data_02098d84.mState.mUnk_1a & 0x7f]),
                                   valueText) = 0;
                    func_0202cf08(data_02098d84.mSide[data_02098d84.mState.mUnk_1a & 0x7f].mUnk_10 + 0xf42, 106,
                                  t + 168, 200, 1);
                    func_020215fc(106, t + 168, data_0201a694, valueText, 0, 96, 0, 1);
                    if (func_0200f97c() >= 100) {
                        if (func_0200f97c() <= 250) {
                            data_02098d84.mSide[data_02098d84.mState.mUnk_1a & 0x7f].mUnk_08 =
                                func_0201fbe4(func_0200f97c(), 100, 250, 0, data_02098d84.mState.mUnk_08);
                        } else {
                            if (!(data_02098d84.mState.mUnk_1a & 0x80)) {
                                func_02007940();
                                data_02098d84.mState.mUnk_1a |= 0x80;
                            }
                            if (func_0200f95c() == 1) {
                                func_0201f880(1);
                                data_020b6ee0.mUnk_ff8 = 0x4000;
                            }
                        }
                    }
                }
            }
        } else {
            switch (data_020b6ee0.mUnk_ffa) {
                case 0:
                    func_0200f848(func_02008920(0), 0);
                    data_020b6ee0.mUnk_ffa++;
                    func_0201f880(1);
                    func_0202f110(func_02008950(0), 100, 0, 64, 0xffff, 0);
                    break;
                case 1:
                    func_0200f898();
                    if (func_0200f95c() == 1) {
                        data_020b6ee0.mUnk_ffa++;
                        func_02007244(data_02098d84.mSide[data_02098d84.mState.mUnk_14].mUnk_13);
                        func_0201f880(1);
                    }
                    break;
                case 2:
                    func_020071ac();
                    active = 1;
                    data_020b6ee0.mModel[0].mUnk_134 |= 1;
                    func_020072a8();
                    if (data_020b6ee0.mModel[0].mUnk_172_10 == 1 || data_020c2660.mUnk_04 == 0x20000190) {
                        func_0200f848(func_02008920(1), 0);
                        data_020b6ee0.mUnk_ffa++;
                        func_0201f880(1);
                        func_0202f110(func_02008950(1), 100, 0, 64, 0xffff, 0);
                    }
                    break;
                case 3:
                    func_0200f898();
                    if (func_0200f95c() == 1) {
                        data_020b6ee0.mUnk_ffa++;
                        func_02007244(data_02098d84.mSide[data_02098d84.mState.mUnk_14 ^ 1].mUnk_13);
                        func_0201f880(1);
                    }
                    break;
                case 4:
                    func_020071ac();
                    active = 1;
                    data_020b6ee0.mModel[0].mUnk_134 |= 1;
                    func_020072a8();
                    if (data_020b6ee0.mModel[0].mUnk_172_10 == 1 || data_020c2660.mUnk_04 == 0x20000190) {
                        data_020b6ee0.mUnk_ffa = 6;
                        func_0201f880(1);
                        if (data_020b6ee0.mModel[0].mUnk_172_10 == 1 || data_020c2660.mUnk_04 == 0x20000190) {
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
            if (j == data_02098d84.mState.mUnk_0e[i]) {
                info = &data_02098d84.mSide[i];
                x = func_0201fb74(data_020b6ee0.mUnk_fee, 0x4000, info->mUnk_17, curX[i]);
                y = func_0201fb74(data_020b6ee0.mUnk_fee, 0x4000, info->mUnk_18, curY[i]);
                if (i == data_02098d84.mState.mUnk_1a ||
                    (data_02098d84.mState.mUnk_18[i] < 18 && data_02098d84.mState.mUnk_10[i] == 1)) {
                    x += func_0201fd84(-2, 2);
                }
            } else {
                info = (i == 0) ? &data_020990e0[j].mInfo : &data_020b1524[j];
                if (data_02098d84.mSide[i].mUnk_19 == 0) {
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
            if (info->mUnk_04 > -5000) {
                func_0202cf08(flags | (((i == 1) ? 0x10000 : 0) + 0x1089 + info->mUnk_13), x, y, 250, 2);
                func_0202cf08(0x41088, x, y, 252, 2);
            }
            if (j == data_02098d84.mState.mUnk_0e[i]) {
                if (i == side && data_020b6ee0.mUnk_ff6 == 0x4000) {
                    func_0202cf08(((data_020c18bc.mUnk_45c >> 2) & 7) + 0x11be + (i == 1) * 8, x, y, 251, 2);
                }
                if (data_020b6ee0.mUnk_a6d8[i].mUnk_de == 0) {
                    func_0202cf08(func_0202db1c(&data_020b6ee0.mUnk_a6d8[i]), x, y, 240, 2);
                }
                *func_0201fc08(func_020079a0(info), numText) = 0;
                func_020215fc(x - 10, y - 40, data_0201a6bc, numText, 0, 96, 0, 2);
                func_0202cf08(info->mUnk_10 + 0x11a1, x - 10, y - 40, 240, 2);
                if (data_02098d84.mState.mUnk_18[i] < 18) {
                    func_0202cf08(data_02098d84.mState.mUnk_18[i] / 3 + 0x11a7, x - 10, y - 40, 100, 2);
                }
            }
        }
        if (data_02098d84.mState.mUnk_18[i] < 18) {
            data_02098d84.mState.mUnk_18[i]++;
        }
    }

    info = &data_02098d84.mSide[side];
    other = &data_02098d84.mSide[side ^ 1];
    if (data_020b6ee0.mUnk_ff8 == 0x4000) {
        int baseX = (side == 0) ? 100 : 0;
        func_0202cf08(0xfeb, baseX + 78, 152, 150, 2);
        for (j = 0; j < 5; j++) {
            if (info->mUnk_1c[j] >= 0) {
                int slotX = j * 26 + 24;
                func_02013b48(info, info->mUnk_1c[j], &iconA, &iconB);
                func_02013c24(info, info->mUnk_1c[j], &baseId, &spriteA, &spriteB, &spriteC, &kind, &kindId);
                func_02020944(j, baseX + slotX, 141, 0, 0, 0x10b4, 0, (side == 0) ? 0x167 : 0);
                if (side == 0 || (data_02098d84.mState.mUnk_12 == j && data_020b6ee0.mUnk_ff4 > 10)) {
                    func_0202cf08(iconA, baseX + slotX, 141, 148, 2);
                    func_0202cf08(iconB, baseX + slotX + 8, 137, 146, 2);
                } else {
                    func_0202cf08(0x1194, baseX + slotX, 141, 148, 2);
                }
                if (data_02098d84.mState.mUnk_12 == j && info->mUnk_16 == 0) {
                    int panelX = func_0201fb74(data_020b6ee0.mUnk_ffc, 0x4000, -50, 53);
                    int panelY = func_0201fb74(data_020b6ee0.mUnk_ffe, 0x4000, 80, 0);
                    int textId0, textId1;
                    func_0201fb24(&data_020b6ee0.mUnk_ffc, 4);
                    func_0201fb24(&data_020b6ee0.mUnk_ffe, 4);
                    func_0202cf08(func_0202db1c(&data_020b6ee0.mUnk_a5d4), baseX + slotX, 141, 147, 2);
                    func_0202cf08(spriteA, panelX, 133, 150, 1);
                    func_0202cf08(spriteB, panelX, 133, 140, 1);
                    panelX += 25;
                    func_0202cf08(spriteC, panelX, 123, 130, 1);
                    func_0202cf08(0x80113f, 5, panelY + 191, 250, 1);
                    if (kind == 14) {
                        textId0 = info->mUnk_10 + 184;
                        textId1 = info->mUnk_10 + 172;
                    } else if (kind == 15) {
                        textId0 = kindId + 190;
                        textId1 = kindId + 178;
                    } else {
                        textId0 = baseId + 138;
                        textId1 = baseId + 155;
                    }
                    func_020215fc(95, panelY + 147, data_0201a61c, func_0200f9f0(textId0), 0, 240, 16, 1);
                    panelY += 164;
                    func_020215fc(18, panelY, data_0201a612, func_0200f9f0(textId1), 0, 232, 10, 1);
                }
            }
        }
        if (data_02098d84.mState.mUnk_1b != side) {
            data_02098d84.mState.mUnk_1b = side;
            func_02007244(data_02098d84.mSide[side].mUnk_13);
        }
        data_020b6ee0.mModel[0].mUnk_134 |= 4;
        func_020072a8();
        if (func_020079a0(info) <= 0 || func_020079a0(other) <= 0) {
            data_020b6ee0.mUnk_fe0 = 10;
            memset(&data_020b6ee0.mUnk_fee, 0, 18);
        } else if (data_02098d84.mState.mUnk_17 == 60) {
            if (side == 0) {
                int prev = data_02098d84.mState.mUnk_12;
                u32 touch = data_020c2660.mUnk_04;
                int pos = (u16) touch;
                if ((touch & 0x80000000) && pos != 0xffff && pos < 200) {
                    data_02098d84.mState.mUnk_12 = pos;
                }
                if (data_020c18bc.mUnk_12 & 0x10) {
                    do {
                        data_02098d84.mState.mUnk_12++;
                        if (data_02098d84.mState.mUnk_12 == 5) {
                            data_02098d84.mState.mUnk_12 = 0;
                        }
                    } while (info->mUnk_1c[data_02098d84.mState.mUnk_12] == -1);
                } else if (data_020c18bc.mUnk_12 & 0x20) {
                    do {
                        data_02098d84.mState.mUnk_12--;
                        if (data_02098d84.mState.mUnk_12 < 0) {
                            data_02098d84.mState.mUnk_12 = 4;
                        }
                    } while (info->mUnk_1c[data_02098d84.mState.mUnk_12] == -1);
                }
                if (prev != data_02098d84.mState.mUnk_12) {
                    data_020b6ee0.mUnk_ffc = 0;
                }
                if (data_020c18bc.mUnk_12 & 2) {
                    info->mUnk_16 = 1;
                }
            } else {
                data_02098d84.mState.mUnk_12 = func_02007f64();
                if (data_02098d84.mState.mUnk_12 < 0) {
                    info->mUnk_16 = 1;
                }
            }
            if (info->mUnk_1c[data_02098d84.mState.mUnk_12] == -1) {
                data_02098d84.mState.mUnk_12++;
                if (data_02098d84.mState.mUnk_12 == 5) {
                    data_02098d84.mState.mUnk_12 = 0;
                }
            }
            if (func_020088d8(info, side) == 1) {
                data_02098d84.mState.mUnk_13 = 0;
                if (info->mUnk_1c[data_02098d84.mState.mUnk_12] >= 0 && info->mUnk_16 == 0) {
                    int oldA = func_020079a0(info);
                    int oldB = func_020079a0(other);
                    int newA, newB;
                    data_02098d84.mUnk_33c = side;
                    data_02098d84.mState.mUnk_00 = func_020079ec(info->mUnk_1c[data_02098d84.mState.mUnk_12]);
                    func_02007c84();
                    if (info->mUnk_1b != -1 &&
                        func_020144e0(func_020144a4(info->mUnk_1c[data_02098d84.mState.mUnk_12]), 9) == 1) {
                        data_020b1990[info->mUnk_1b].mInfo.mUnk_1c[data_02098d84.mState.mUnk_12] = -1;
                    }
                    info->mUnk_1c[data_02098d84.mState.mUnk_12] = -1;
                    newA = func_020079a0(info);
                    newB = func_020079a0(other);
                    info->mUnk_0e = oldA;
                    other->mUnk_0e = oldB;
                    if (oldA != newA) {
                        data_02098d84.mState.mUnk_18[side] = 0;
                        if (oldA < newA) {
                            data_020b6ee0.mModel[0].mUnk_134 |= 8;
                            data_02098d84.mState.mUnk_10[side] = 0;
                            func_0202de60(&data_020b6ee0.mUnk_a6d8[side], data_0201abec[32]);
                        } else {
                            data_02098d84.mState.mUnk_10[side] = 1;
                            func_0202de60(&data_020b6ee0.mUnk_a6d8[side], data_0201abec[info->mUnk_10 + 33]);
                        }
                    }
                    if (oldB != newB) {
                        data_02098d84.mState.mUnk_18[side ^ 1] = 0;
                        if (oldB < newB) {
                            data_02098d84.mState.mUnk_10[side ^ 1] = 0;
                            func_0202de60(&data_020b6ee0.mUnk_a6d8[side ^ 1], data_0201abec[32]);
                        } else {
                            data_02098d84.mState.mUnk_10[side ^ 1] = 1;
                            func_0202de60(&data_020b6ee0.mUnk_a6d8[side ^ 1], data_0201abec[other->mUnk_10 + 33]);
                        }
                    }
                }
                data_020b6ee0.mUnk_ff4 = 0;
                data_02098d84.mState.mUnk_17 = 59;
                info->mUnk_16 = 0;
            }
        } else if (data_02098d84.mState.mUnk_17 == 0) {
            data_02098d84.mState.mUnk_15++;
            if (data_02098d84.mState.mUnk_15 > 3) {
                data_020b6ee0.mUnk_fe0 = 10;
                memset(&data_020b6ee0.mUnk_fee, 0, 18);
            }
            data_02098d84.mState.mUnk_12 = 0;
            data_02098d84.mState.mUnk_17 = 60;
        } else if (data_02098d84.mModel[1].mUnk_170 == 0) {
            data_02098d84.mState.mUnk_17--;
        }
        if (!((data_02098d84.mState.mUnk_15 ^ data_02098d84.mState.mUnk_14) & 1) &&
            data_02098d84.mState.mUnk_17 == 60 && data_02098d84.mState.mUnk_15 <= 3) {
            if (info->mUnk_1c[data_02098d84.mState.mUnk_12] >= 0) {
                func_020071f8();
            }
            func_02020944(201, 128, 179, 0, 0, 0x10b2, 0, 0x167);
            func_0202cf08((data_020c2660.mUnk_04 == 0x400000c9) + 0x10b2, 128, 179, 142, 2);
            func_020215fc(128, 179, data_0201a61c, func_0200f9f0(61), 2, 250, 0, 2);
            if (data_020c2660.mUnk_04 == 0x200000c9) {
                info->mUnk_16 = 1;
            }
        }
    }
    if (!(data_020b6ee0.mUnk_ffa & 1) && (data_020b6ee0.mUnk_ff8 == 0 || data_020b6ee0.mUnk_ff8 == 0x4000)) {
        if (data_02098d84.mState.mUnk_1b == -1 && active == 0) {
            func_02007370();
        } else {
            func_020073b0();
        }
        func_02033614(data_020c5e44.mUnk_a4.x, data_020c5e44.mUnk_a4.z);
    }
}

// One frame of the stage choice before a round: brings in both sides' three stage panels and their values, moves
// the cursor (picked panel, keys, or func_02007ed4's choice after a delay) and starts the round once both chosen
// stages are still in play and the choice is confirmed.
void func_02009728(void) {
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
        memset(data_020b6ee0.mUnk_fe6, 0, 8);
        data_02098d84.mState.mUnk_0e[1] = 0;
        data_02098d84.mState.mUnk_0e[0] = data_02098d84.mState.mUnk_0e[1];
        data_020b6ee0.mModel[1].mUnk_170 = 0;
        data_020b6ee0.mModel[0].mUnk_172 |= 0x10;
        data_02098d84.mState.mUnk_1b = -2;
    }
    func_0201fb5c(&data_020b6ee0.mUnk_fee, 400);
    if (data_020b6ee0.mUnk_fee == 0x4000) {
        func_0201fb5c(&data_020b6ee0.mUnk_ff0, 400);
        if (data_020b6ee0.mUnk_ff0 < 0x4000) {
            *(vu16 *) 0x04001052 = func_0201fb74(data_020b6ee0.mUnk_ff0, 0x4000, 0, 16);
        } else {
            int level;
            func_0201fb24(&data_020b6ee0.mUnk_ff2, 4);
            level = func_0201fb74(data_020b6ee0.mUnk_ff2, 0x4000, 0, 8);
            *(vu16 *) 0x04001050 = 0x2840;
            *(vu16 *) 0x04001052 = (level << 8) + 16 - level;
            func_0201fb5c(&data_020b6ee0.mUnk_fe2, 800);
        }
    }
    if (data_020b6ee0.mUnk_fe6[0] == 0x4000) {
        func_0201fb5c(&data_020b6ee0.mUnk_fe4, 800);
        if (data_020b6ee0.mUnk_fe6[1] == 0x4000) {
            func_0201fb5c(&data_020b6ee0.mUnk_fea, 800);
        }
    }
    func_02007ffc(&data_020b6ee0.mUnk_fe2, &data_020b6ee0.mUnk_fe4);
    if (data_020b6ee0.mUnk_fe2 == 0x4000) {
        for (i = 0; i < 2; i++) {
            if (i != 0 && data_020b6ee0.mUnk_fe4 != 0x4000) {
                continue;
            }
            func_0201fb5c(&data_020b6ee0.mUnk_fe6[i], 170);
            for (j = 0; j < 3; j++) {
                info = i == 0 ? &data_020990e0[j].mInfo : &data_020b1524[j];
                if (func_020079cc(info) > -5000) {
                    int x = xs[i * 3 + j];
                    int y = ys[i * 3 + j];
                    int d = (2 - j) * 1920;
                    if (data_020b6ee0.mUnk_fe6[0] >= j * 1920) {
                        int hit;
                        a = func_0201fb74(data_020b6ee0.mUnk_fe6[i] < 0x4000 - d ? data_020b6ee0.mUnk_fe6[i]
                                                                                 : 0x4000 - d,
                                          0x4000 - d, 0, 23);
                        if (data_020b6ee0.mUnk_fe6[i] < 0x4000) {
                            func_0202cf08(a + 0x106a, x, y, 248, 2);
                        }
                        if (a >= 16) {
                            a = (data_020b6ee0.mUnk_ff0 == 0x4000 ||
                                 (data_02098d84.mState.mUnk_16 == i && j == data_02098d84.mState.mUnk_0e[i]))
                                    ? 0
                                    : 0x40000;
                            func_02020944(j + 16 + i * 4, x, y, 0, 0, 0x1089, 0, 0x167);
                            func_0202cf08(a | (info->mUnk_13 + ((i == 1 ? 0x10000 : 0) + 0x1089)), x, y, 250, 2);
                            func_0202cf08(0x41088, x, y, 252, 2);
                            info->mUnk_17 = x;
                            info->mUnk_18 = y;
                            hit = 0;
                            if (i == 0) {
                                if (data_020b6ee0.mUnk_a8f3[j] & 1) {
                                    hit = 1;
                                }
                            } else {
                                int s = data_02098d84.mState.mUnk_0e[0];
                                if (!(data_020b6ee0.mUnk_a8f3[s] & 1) &&
                                    data_020990e0[s].mInfo.mUnk_11 < data_020b1524[j].mUnk_11) {
                                    hit = 1;
                                }
                            }
                            if (hit == 1) {
                                func_0202cf08(func_0201fb74(data_020c18bc.mUnk_45c & 31, 31, 0, 8) + 0x11ad, x + 22,
                                              y - 5, 230, 2);
                            }
                        }
                    }
                }
            }
        }
    }
    if (data_02098d84.mState.mUnk_04 > 0x3f6a && data_02098d84.mState.mUnk_04 < 0x4000) {
        data_02098d84.mState.mUnk_18[0] = 0;
    }
    func_0201fb5c(&data_02098d84.mState.mUnk_04, 150);
    for (j = 0; j < 2; j++) {
        for (i = 0; i < 3; i++) {
            int v;
            info = j == 0 ? &data_020990e0[i].mInfo : &data_020b1524[i];
            v = func_020079cc(info);
            v -= func_0201fb74(data_02098d84.mState.mUnk_04, 0x4000, info->mUnk_06, 0);
            if (v > -5000) {
                int x = func_0201fb74(data_020b6ee0.mUnk_fee, 0x4000, info->mUnk_17, xs[j * 3 + i]) - 10;
                int y = func_0201fb74(data_020b6ee0.mUnk_fee, 0x4000, info->mUnk_18, ys[j * 3 + i]) - 40;
                *func_0201fc08(v, text) = 0;
                func_020215fc(x, y, data_0201a6bc, text, 0, 96, 0, 2);
                func_0202cf08(info->mUnk_10 + 0x11a1, x, y, 240, 2);
                if (data_02098d84.mState.mUnk_18[0] < 18 && info->mUnk_06 != 0) {
                    func_0202cf08(data_02098d84.mState.mUnk_18[0] / 3 + 0x11a7, x, y, 100, 2);
                }
            }
        }
    }
    if (data_02098d84.mState.mUnk_18[0] < 18) {
        data_02098d84.mState.mUnk_18[0]++;
    }
    move = 0;
    if (data_020b6ee0.mUnk_fe6[1] == 0x4000) {
        func_02008090(data_02098d84.mState.mUnk_06 == 0 ? &data_020990e0[data_02098d84.mState.mUnk_0e[0]].mInfo
                                                        : &data_020b1524[data_02098d84.mState.mUnk_0e[1]]);
        selecting = 1;
        if (data_02098d84.mState.mUnk_14 == 0) {
            u32 pick = data_020c2660.mUnk_04;
            u16 keys;
            if ((pick & 0x40000000) && (u16) pick != 0xffff) {
                int n = (u16) pick - 16;
                int col = n % 4;
                int row = n / 4;
                if (row < 2) {
                    data_02098d84.mState.mUnk_06 = row;
                    data_02098d84.mState.mUnk_0e[data_02098d84.mState.mUnk_06] = col;
                }
            }
            keys = data_020c18bc.mUnk_12;
            if (keys & 0x40) {
                if (data_02098d84.mState.mUnk_06 == 0) {
                    data_02098d84.mState.mUnk_06 = 1;
                }
            } else if (keys & 0x80) {
                if (data_02098d84.mState.mUnk_06 == 1) {
                    data_02098d84.mState.mUnk_06 = 0;
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
            int r = func_02007ed4();
            move = 1;
            if (r & 1) {
                data_02098d84.mState.mUnk_06 = 0;
            } else if (r & 2) {
                data_02098d84.mState.mUnk_06 = 1;
            } else {
                move = SELECT_CONFIRM;
            }
            data_020b6ee0.mUnk_ff8 = 0;
        }
    }
    if (move != 0 && move != SELECT_CONFIRM) {
        do {
            int s;
            j = data_02098d84.mState.mUnk_06;
            data_02098d84.mState.mUnk_0e[j] = move + data_02098d84.mState.mUnk_0e[j];
            if (data_02098d84.mState.mUnk_0e[j] > 2) {
                data_02098d84.mState.mUnk_0e[j] = 0;
            } else if (data_02098d84.mState.mUnk_0e[j] < 0) {
                data_02098d84.mState.mUnk_0e[j] = 2;
            }
            s = data_02098d84.mState.mUnk_0e[j];
            info = j == 0 ? &data_020990e0[s].mInfo : &data_020b1524[s];
        } while (func_020079cc(info) <= -5000);
    }
    if (data_020b6ee0.mUnk_fe6[1] == 0x4000) {
        for (j = 0; j < 2; j++) {
            int s;
            int yi;
            int hi;
            for (;;) {
                s = data_02098d84.mState.mUnk_0e[j];
                if (func_020079cc(j == 0 ? &data_020990e0[s].mInfo : &data_020b1524[s]) > -5000) {
                    break;
                }
                data_02098d84.mState.mUnk_0e[j]++;
                if (data_02098d84.mState.mUnk_0e[j] > 2) {
                    data_02098d84.mState.mUnk_0e[j] = 0;
                }
            }
            a = j == data_02098d84.mState.mUnk_06 ? (data_020c18bc.mUnk_45c >> 2) & 7 : 0;
            yi = j == 0 ? s : s + 3;
            hi = j == 1 ? 1 : 0;
            func_0202cf08(a + (hi * 8 + 0x11be), xs[j == 0 ? s : s + 3], ys[yi], 251, 2);
        }
        if (move == SELECT_CONFIRM || data_020c2660.mUnk_04 == 0x200000c8) {
            if (data_020990e0[data_02098d84.mState.mUnk_0e[0]].mInfo.mUnk_04 > -5000 &&
                data_020b1524[data_02098d84.mState.mUnk_0e[1]].mUnk_04 > -5000) {
                data_020b6ee0.mUnk_fe0 = 1;
                data_02098d84.mState.mUnk_18[1] = 0xff;
                data_02098d84.mState.mUnk_18[0] = data_02098d84.mState.mUnk_18[1];
                data_02098d84.mState.mUnk_12 = 0;
                data_02098d84.mState.mUnk_15 = 0;
                data_02098d84.mState.mUnk_17 = 60;
                data_02098d84.mState.mUnk_1a = -1;
                data_02098d84.mState.mUnk_1b = -1;
                data_02098d84.mState.mUnk_10[1] = 0;
                data_02098d84.mState.mUnk_10[0] = data_02098d84.mState.mUnk_10[1];
                func_020077f0(data_02098d84.mState.mUnk_0e[0], data_02098d84.mState.mUnk_0e[1]);
                memset(&data_020b6ee0.mUnk_fee, 0, 18);
                func_0201eb74(2);
                func_02021fb0(0x1c7, 0x1c6, 0x100, 16);
                return;
            }
        }
        if (data_02098d84.mState.mUnk_14 == 0) {
            func_020071f8();
        }
        data_020b6ee0.mModel[0].mUnk_134 |= 4;
    } else {
        data_020b6ee0.mModel[0].mUnk_134 |= 1;
    }
    {
        int cur = data_02098d84.mState.mUnk_06;
        int s = data_02098d84.mState.mUnk_0e[cur];
        u8 id = cur == 1 ? data_020b1524[s].mUnk_13 : data_020990e0[s].mInfo.mUnk_13;
        if (data_02098d84.mState.mUnk_1b != id) {
            func_02007244(id);
            data_020b6ee0.mModel[0].mUnk_172 |= 0x80;
            data_020b6ee0.mModel[0].mUnk_172 |= 0x20;
            if (data_02098d84.mState.mUnk_1b == -2) {
                func_02007a1c(1);
            }
            data_02098d84.mState.mUnk_1b = id;
        }
    }
    if (selecting == 0) {
        func_02007390();
        data_020c5e44.mUnk_bc = 4;
        func_02007abc();
    } else {
        func_020073b0();
        func_020072a8();
    }
    func_02033614(data_020c5e44.mUnk_a4.x, data_020c5e44.mUnk_a4.z);
}

void func_02009fe8(void) {
    char text[8];
    int i;
    int j;
    int side;
    int from;
    int base = data_02098d84.mState.mUnk_0d * 7;
    int k;
    int o;
    int value;
    func_02007ffc(&data_020b6ee0.mUnk_fe2, &data_020b6ee0.mUnk_fe4);
    if (data_020b6ee0.mUnk_fe2 == 0) {
        data_02098d84.mState.mUnk_18[0] = 0xff;
    }
    if (data_020b6ee0.mUnk_fee == 0) {
        func_0202cf08(0x8011d0, 128, 36, 200, 1);
        func_020215fc(128, 36, data_0201a61c, func_0200f9f0(68), 2, 240, 16, 1);
        for (j = 0; j < 3; j++) {
            for (i = 0; i < 2; i++) {
                k = j * 2 + i;
                value = func_02021e44(0x1e7, k + 3 + base);
                func_0202cf08(0x11ce, i * 110 + 67, j * 34 + 76, 210, 1);
                func_0202cf08(k + 0xebe, i * 110 + 46, j * 34 + 71, 200, 1);
                *func_0201fc08(value, text) = 0;
                func_020215fc(i * 110 + 67, j * 34 + 76, data_0201a64e, text, 0, 250, 100, 1);
            }
        }
        func_02007370();
        func_02033614(data_020c5e44.mUnk_a4.x, data_020c5e44.mUnk_a4.z);
        func_0202cf08(0xec4, 128, 45, 100, 2);
        func_0202cf08(0x11cf, 127, 153, 200, 2);
        func_020215fc(127, 153, data_0201a61c, func_0200f9f0(69), 2, 240, 16, 2);
        func_020071f8();
        func_0201fb24(&data_020b6ee0.mUnk_fe2, 4);
        func_0201fb24(&data_020b6ee0.mUnk_fe4, 4);
        func_0201fb24(&data_020b6ee0.mUnk_ff0, 4);
    } else {
        func_0200f898();
        func_0201fb24(&data_020b6ee0.mUnk_fe2, -8);
        func_0201fb24(&data_020b6ee0.mUnk_fe4, -8);
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
                StageInfo *info = (side == 0) ? &data_020990e0[2 - i].mInfo : &data_020b1524[2 - i];
                if (func_020079cc(info) > -5000) {
                    int x = func_0201fbe4(data_020b6ee0.mUnk_ff0, 0, 0x4000, from, end[o] + i * step[o]);
                    int y = i * 20 + 70;
                    info->mUnk_17 = x + 10;
                    info->mUnk_18 = y + 40;
                    *func_0201fc08(func_020079cc(info), text) = 0;
                    func_0202cf08(info->mUnk_10 + 0x11a1, x, y, 150, 2);
                    func_020215fc(x, y, data_0201a6bc, text, 0, 96, 0, 2);
                }
            }
        }
    }
    if (data_020b6ee0.mUnk_fee == 0) {
        if ((data_020c18bc.mUnk_12 & 1) || data_020c2660.mUnk_04 == 0x200000c8) {
            func_0201f880(1);
            func_02022120();
            func_0200f848(func_02014538(data_020b6ee0.mUnk_a534, 7) + 19, 0);
            func_0202f110(91, 100, 0, 64, 0xffff, 0);
            data_020b6ee0.mUnk_fee = 1;
        }
    } else if (func_0200f95c() == 1) {
        int id;
        func_0201f880(1);
        data_020b6ee0.mUnk_fe0 = 0;
        data_020b6ee0.mUnk_fe2 = 0;
        data_020b6ee0.mUnk_fe4 = 0;
        data_02098d84.mState.mUnk_06 = 0;
        data_02098d84.mState.mUnk_14 = 0;
        data_02098d84.mState.mUnk_1b = -1;
        id = func_02014538(data_020b6ee0.mUnk_a534, 14);
        memset(&data_020b6ee0.mUnk_fee, 0, 18);
        *(vu16 *) 0x04001050 = 0x2848;
        *(vu16 *) 0x04001052 = 0;
        *(vu16 *) 0x05000400 = 0;
        func_0201dde0(id + 0xf59, 2);
        func_02007770();
    }
}
