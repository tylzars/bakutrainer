#include <nitro/os.h>
#include <nitro/gx.h>
#include <stdlib.h>
#include <Main/Unknown/functions.h>
#include <Main/Unknown/stage_info_panel.h>
#include <Main/Unknown/game_util.h>
#include <Main/clib.h>
#include <Main/Unknown/sprite_slots.h>
#include <Main/Unknown/prompt_screen.h>
#include <Main/Unknown/sound_control.h>
#include <Main/Unknown/globals.h>
#include <Main/Unknown/field_objects.h>
#include <stdio.h>
#include <Main/Unknown/mode_select.h>

s32 func_02005c10(int a, int b) {
    return func_02021e44(0xda0, b + a * 7);
}

s32 func_02005c24(int a, int b) {
    return func_02021e44(0xda1, b + a * 252);
}

s32 func_02005c3c(int a, int b) {
    return func_02021e44(0xd9f, b + a * 10);
}

void func_02005c54(s32 x, s32 y, s32 *outCol, s32 *outSub, s32 *outRow) {
    s32 cellH = (data_0206d460.mInfo.mUnk_30 - data_0206d460.mInfo.mUnk_24) / 8;
    s32 cellW = (data_0206d460.mInfo.mUnk_28 - data_0206d460.mInfo.mUnk_1c) / 8;
    *outSub = (x - data_0206d460.mInfo.mUnk_1c) / cellW;
    *outRow = (y - data_0206d460.mInfo.mUnk_24) / cellH;
    *outCol = *outSub / 8;
    *outSub = *outSub % 8;
}

void func_02005cb4(void) {
    int g, i, j;
    FieldObject *p;
    for (g = 0; g < 4; g++) {
        for (i = 0; i < 8; i++) {
            for (j = 0; j < 8; j++) {
                p = &data_0206d460.mGrid[g][i][j];
                switch (p->mKind) {
                    case 0:
                    case 2:
                        break;
                    case 1:
                        if (p->mType != 0) {
                            func_020248e8(data_0204e144[p->mKind], &p->mUnk_04, 0);
                        } else {
                            func_0202733c(p->mUnk_48, &p->mUnk_04, 0);
                        }
                        break;
                    case 3:
                        func_0202733c(p->mUnk_48, &p->mUnk_04, 0);
                        break;
                    case 4:
                        if (p->mType != 0) {
                            func_020248e8(data_0204e144[p->mKind], &p->mUnk_04, 0);
                        }
                        break;
                }
            }
        }
    }
    for (i = 0; i < 128; i++) {
        p = &data_0206d460.mList[i];
        if (p->mKind != 0) {
            func_0202733c(p->mUnk_48, &p->mUnk_04, 0);
        }
    }
}

void func_02005d94(u8 g) {
    int i, j;
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            data_0206d460.mGrid[g][i][j].mKind = 0;
            data_0206d460.mGrid[g][i][j].mUnk_1c2 = -1;
        }
    }
}

void func_02005de0(u8 g, u8 k) {
    s32 width = data_0206d460.mInfo.mUnk_28 - data_0206d460.mInfo.mUnk_1c;
    s32 height = data_0206d460.mInfo.mUnk_30 - data_0206d460.mInfo.mUnk_24;
    s32 cellW = width / 8;
    s32 cellH = height / 8;
    int a, b, idx;
    FieldObject *obj;
    FieldObject *row = data_0206d460.mGrid[g][0];
    for (a = 0; a < 8; a++) {
        obj = row;
        for (b = 0; b < 8; b++) {
            obj->mType = 1;
            obj->mUnk_04 = data_0206d460.mInfo.mUnk_1c + g * width + a * cellW + cellW / 2;
            obj->mUnk_08 = data_0206d460.mInfo.mUnk_20;
            obj->mUnk_0c = data_0206d460.mInfo.mUnk_24 + b * cellH + cellH / 2;
            obj->mUnk_1c0 = -1;
            obj->mUnk_1c2 = -1;
            obj->mKind = data_0206d460.mUnk_2a8b4[k * 64 + a * 8 + b];
            if (obj->mKind == 2) {
                func_020271ec(obj->mUnk_48, 0, 0, 0, data_0201abbc[9]);
                obj->mUnk_38 = -1;
                obj->mUnk_34 = -1;
                obj->mUnk_30 = -1;
            } else if (obj->mKind == 3) {
                func_020271ec(obj->mUnk_48, 0, 0, 0, data_0201abbc[10]);
                obj->mUnk_38 = -1;
                obj->mUnk_34 = -1;
                obj->mUnk_30 = -1;
            }
            if (obj->mKind == 2) {
                for (idx = 0; idx < 128; idx++) {
                    if (data_0206d460.mList[idx].mKind == 0) {
                        obj->mUnk_1c2 = idx;
                        data_0206d460.mList[idx] = *obj;
                        break;
                    }
                }
            }
            obj++;
        }
        row += 8;
    }
}

void func_02005f70(void) {
    int i = 0;
    FieldObject *o = data_0206d460.mList;
    s32 half = ((data_0206d460.mInfo.mUnk_30 - data_0206d460.mInfo.mUnk_24) / 8 - 1) / 2;
    for (; i < 128; i++, o++) {
        if (o->mKind != 2) {
            continue;
        }
        if (o->mType != 0) {
            s32 col, sub, row;
            func_02005c54(o->mUnk_04, o->mUnk_0c - half, &col, &sub, &row);
            if (data_0206d460.mGrid[col][sub][row].mKind == 2) {
                data_0206d460.mGrid[col][sub][row].mKind = 0;
                data_0206d460.mGrid[col][sub][row].mUnk_1c2 = -1;
            }
            func_02005c54(o->mUnk_04, o->mUnk_0c + half, &col, &sub, &row);
            if (data_0206d460.mGrid[col][sub][row].mKind == 2) {
                data_0206d460.mGrid[col][sub][row].mKind = 0;
                data_0206d460.mGrid[col][sub][row].mUnk_1c2 = -1;
            }
            o->mUnk_0c += o->mUnk_1c0 * (50 << 10) / 2;
            if (o->mUnk_0c < data_020c5e44.mUnk_0c0.mUnk_228.z - 0x177000) {
                o->mUnk_0c = data_020c5e44.mUnk_0c0.mUnk_228.z - 0x177000;
                o->mUnk_1c0 = -o->mUnk_1c0;
            } else if (o->mUnk_0c > data_020c5e44.mUnk_0c0.mUnk_228.z + (150 << 12)) {
                o->mUnk_0c = data_020c5e44.mUnk_0c0.mUnk_228.z + (150 << 12);
                o->mUnk_1c0 = -o->mUnk_1c0;
            }
            func_02005c54(o->mUnk_04, o->mUnk_0c - half, &col, &sub, &row);
            if (data_0206d460.mGrid[col][sub][row].mKind == 0) {
                data_0206d460.mGrid[col][sub][row].mKind = 2;
                data_0206d460.mGrid[col][sub][row].mUnk_1c2 = i;
            }
            func_02005c54(o->mUnk_04, o->mUnk_0c + half, &col, &sub, &row);
            if (data_0206d460.mGrid[col][sub][row].mKind == 0) {
                data_0206d460.mGrid[col][sub][row].mKind = 2;
                data_0206d460.mGrid[col][sub][row].mUnk_1c2 = i;
            }
        } else {
            s32 col, sub, row;
            func_02005c54(o->mUnk_04, o->mUnk_0c - half, &col, &sub, &row);
            if (data_0206d460.mGrid[col][sub][row].mKind == 2) {
                data_0206d460.mGrid[col][sub][row].mKind = 0;
            }
            func_02005c54(o->mUnk_04, o->mUnk_0c + half, &col, &sub, &row);
            if (data_0206d460.mGrid[col][sub][row].mKind == 2) {
                data_0206d460.mGrid[col][sub][row].mKind = 0;
            }
        }
    }
}

void func_020061ac(void) {
    int g, i, j, k;
    for (g = 0; g < 3; g++) {
        for (i = 0; i < 8; i++) {
            for (j = 0; j < 8; j++) {
                s16 idx = data_0206d460.mGrid[g][i][j].mUnk_1c2;
                if (idx >= 0) {
                    data_0206d460.mList[idx].mKind = 0;
                }
            }
        }
    }
    for (i = 0; i < 128; i++) {
        if (data_0206d460.mList[i].mKind != 0) {
            data_0206d460.mList[i].mUnk_04 -= data_0206d460.mUnk_26c << 9;
        }
    }
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            data_0206d460.mGrid[0][i][j] = data_0206d460.mGrid[3][i][j];
            data_0206d460.mGrid[0][i][j].mUnk_04 -= data_0206d460.mUnk_26c << 9;
        }
    }
    for (k = 1; k < 4; k++) {
        if (data_0206d460.mUnk_004 != 0) {
            {
                u8 r = func_0201fd84(0, data_0206d460.mUnk_2b8f4 - 2);
                func_02005de0(k, data_0206d460.mUnk_2b8b4[r]);
            }
        } else {
            func_02005d94(k);
        }
    }
}

int func_020062e8(void) {
    return 0;
}

int func_020062ec(void) {
    if (data_0206d460.mUnk_260 != 0 || data_0206d460.mUnk_25c == 0) {
        return 1;
    }
    return 0;
}

void func_0200630c(s32 a, u32 b) {
    if (data_0206d460.mUnk_264 == 0 && data_0206d460.mUnk_004 != 0) {
        data_0206d460.mUnk_260 = 0;
        data_0206d460.mUnk_250 = data_0206d460.mUnk_240;
        data_0206d460.mUnk_258 = a;
        data_0206d460.mUnk_25c = b * 60 / 1000;
        data_0206d460.mUnk_248 = 0;
    }
}

void func_02006350(s32 a, u32 b) {
    if (data_0206d460.mUnk_25c == 0 && data_0206d460.mUnk_268 == 0 && data_0206d460.mUnk_004 != 0) {
        data_0206d460.mUnk_260 = 1;
        data_0206d460.mUnk_250 = data_0206d460.mUnk_240;
        data_0206d460.mUnk_258 = a;
        data_0206d460.mUnk_25c = b * 60 / 1000;
    }
}

void func_02006398(FieldObject *p) {
    char text[32];
    s32 pos[2];
    switch (p->mKind) {
        case 1:
        case 2:
            if (p->mType == 0) {
                break;
            }
            p->mType = 0;
            func_0200630c(250 << 10, 250 << 1);
            func_0202f110(48, 100, 0, 64, 0xffff, 0);
            if (p->mKind == 2) {
                p = &data_0206d460.mList[p->mUnk_1c2];
            }
            func_020271ec(p->mUnk_48, 0, 0, 0, data_0201abbc[11]);
            p->mUnk_38 = -1;
            p->mUnk_34 = -1;
            p->mUnk_30 = -1;
            data_0206d460.mUnk_00c = data_0206d460.mUnk_2b910;
            return;
        case 3:
            func_02006350(25 << 12, 0x15e);
            if (p->mType == 0) {
                break;
            }
            p->mType = 0;
            func_0202f110(1, 100, 0, 64, 0xffff, 0);
            return;
        case 4:
            if (p->mType == 0) {
                break;
            }
            func_02025db4(&p->mUnk_04, pos);
            p->mType = 0;
            func_020059e8(&data_0206d460.mUnk_004, 60000 * data_0206d460.mUnk_2b90c / 1000);
            sprintf(text, "+%d'", data_0206d460.mUnk_2b90c);
            func_0200d838(text, 2, data_0201a61c, 30, pos[0], pos[1], 0, -128, 100, 1, 0);
            func_0202f110(117, 100, 0, 64, 0xffff, 0);
            break;
    }
}

void func_020064e8(void) {
    s32 col, sub, row;
    s32 quarter = ((data_0206d460.mInfo.mUnk_30 - data_0206d460.mInfo.mUnk_24) / 8) / 4;
    func_02005c54(data_0206d460.mUnk_1b0.x, data_0206d460.mUnk_1b0.z - quarter, &col, &sub, &row);
    if (data_0206d460.mGrid[col][sub][row].mKind != 0) {
        func_02006398(&data_0206d460.mGrid[col][sub][row]);
    }
    func_02005c54(data_0206d460.mUnk_1b0.x, data_0206d460.mUnk_1b0.z + quarter, &col, &sub, &row);
    if (data_0206d460.mGrid[col][sub][row].mKind != 0) {
        func_02006398(&data_0206d460.mGrid[col][sub][row]);
    }
}

void func_0200659c(void) {
    void *entry;
    data_020c5e44.mUnk_344 = 0x186a000;
    data_020c5e44.mUnk_354 = 0x578;
    func_02033e38(1, 1, 1);
    func_02033e38(8, 0, 1);
    data_020c5e44.mUnk_330 = 1;
    data_020c5e44.mUnk_0c0.mUnk_204.x = data_020c5e44.mUnk_0c0.mUnk_228.x;
    data_0206d460.mUnk_274 = data_020c5e44.mUnk_0c0.mUnk_228;
    entry = func_0203204c(2, 0);
    if (entry == NULL) {
        OS_Terminate();
    }
    data_0206d460.mInfo.mUnk_00.x = func_02031f98(entry);
    data_0206d460.mInfo.mUnk_00.y = func_02031fa4(entry);
    data_0206d460.mInfo.mUnk_00.z = func_02031fb0(entry);
    data_0206d460.mInfo.mUnk_0c.x = data_020c5e44.mUnk_0c0.mUnk_204.x - data_020c5e44.mUnk_0c0.mUnk_228.x;
    data_0206d460.mInfo.mUnk_0c.y = data_020c5e44.mUnk_0c0.mUnk_204.y - data_020c5e44.mUnk_0c0.mUnk_228.y;
    data_0206d460.mInfo.mUnk_0c.z = data_020c5e44.mUnk_0c0.mUnk_204.z - data_020c5e44.mUnk_0c0.mUnk_228.z;
    data_0206d460.mInfo.mUnk_18 = 0;
    entry = func_0203204c(3, 0);
    if (entry == NULL) {
        OS_Terminate();
    }
    data_0206d460.mInfo.mUnk_1c = func_02031f98(entry);
    data_0206d460.mInfo.mUnk_20 = func_02031fa4(entry);
    data_0206d460.mInfo.mUnk_24 = func_02031fb0(entry);
    entry = func_0203204c(4, 0);
    if (entry == NULL) {
        OS_Terminate();
    }
    data_0206d460.mInfo.mUnk_28 = func_02031f98(entry);
    data_0206d460.mInfo.mUnk_2c = func_02031fa4(entry);
    data_0206d460.mInfo.mUnk_30 = func_02031fb0(entry);
    data_020c5e44.mUnk_338 = 0;
    data_020c5e44.mUnk_340 = 0;
    data_020c5e44.mUnk_33c = 0;
    data_020c5e44.mUnk_7f &= ~2;
    func_02033df4(0x12);
    data_020c5e44.mUnk_314 = data_020c5e44.mUnk_318;
    data_020c5e44.mUnk_310 = 0;
}

void func_020066c8(void) {
    s32 step = (((((data_0206d460.mUnk_270 >> 8) * 100000) / 3600) << 9) / 60);
    s32 remain;
    data_0206d460.mInfo.mUnk_18 += step;
    remain = data_0206d460.mInfo.mUnk_18;
    while (remain > 0) {
        data_020c5e44.mUnk_0c0.mUnk_228.x = data_0206d460.mUnk_274.x + remain;
        remain = data_0206d460.mInfo.mUnk_18 - (data_0206d460.mUnk_26c << 9);
        if (remain >= 0) {
            data_0206d460.mInfo.mUnk_18 = remain;
            func_020061ac();
        }
    }
    data_020c5e44.mUnk_0c0.mUnk_204.x = data_020c5e44.mUnk_0c0.mUnk_228.x + data_0206d460.mInfo.mUnk_0c.x;
    data_020c5e44.mUnk_0c0.mUnk_204.y = data_020c5e44.mUnk_0c0.mUnk_228.y + data_0206d460.mInfo.mUnk_0c.y;
    data_020c5e44.mUnk_0c0.mUnk_204.z = data_020c5e44.mUnk_0c0.mUnk_228.z + data_0206d460.mInfo.mUnk_0c.z;
    if (data_0206d460.mUnk_00c != 0) {
        data_0206d460.mUnk_00c--;
    }
    if (data_0206d460.mUnk_004 != 0 && data_0206d460.mUnk_00c == 0) {
        s32 cellW = (data_0206d460.mInfo.mUnk_28 - data_0206d460.mInfo.mUnk_1c) / 8;
        func_020059d0(&data_0206d460.mUnk_008,
                      func_0201fbe4(data_0206d460.mUnk_270 >> 8, data_0206d460.mUnk_2b8f8 >> 8,
                                    data_0206d460.mUnk_2b8fc >> 8, 8, 256) *
                          step / cellW,
                      8);
    }
    func_02033fc8();
}

void func_020067ac(void) {
    s32 level = data_020b6ee0.mUnk_a8e4;
    u16 v[5];
    s32 i, j, k;
    s32 stageNo;
    s8 value;
    BOOL ok = 0;
    if (level < 0) {
        level = 0;
    }
    data_0206d460.mStage = data_020b1990[level].mInfo;
    stageNo = data_0206d460.mStage.mUnk_11 - 1;
    data_020b10fc.mUnk_08 = stageNo;
    if (stageNo >= 0 && stageNo < 12) {
        ok = 1;
    }
    if (ok == 0) {
        OS_Terminate();
    }
    func_0200c5d8(0, 0, 0xdb3 + data_0206d460.mStage.mUnk_13, 0x100, 16, 1, 0x82);
    func_02024208();
    func_020104c0(2, 0x4c);
    func_0200dc08(data_0204e138[data_0206d460.mStage.mUnk_10], &v[4], &v[3], &v[2], &v[1], &v[0], NULL);
    func_020325b0(v[4], v[2], v[3], 0, v[1], v[0]);
    func_0201bb30();
    func_0200659c();
    G3X_SetClearColor(0x7fff, 8, 0x7fff, 0, 0);
    {
        s32 dz = (data_0206d460.mInfo.mUnk_00.z - data_0206d460.mUnk_274.z) / 512;
        s32 dx = (data_0206d460.mInfo.mUnk_00.x - data_0206d460.mUnk_274.x) / 512;
        data_0206d460.mUnk_26c = func_0201fcec((s64) dx * dx + (s64) dz * dz);
    }
    func_020271ec(&data_0206d460.mUnk_010, 0, 0, 0, data_0201ab04[data_0206d460.mStage.mUnk_13]);
    data_0206d460.mUnk_1e8 = 3 << 14;
    data_0206d460.mUnk_1b0 = data_020c5e44.mUnk_0c0.mUnk_228;
    data_0206d460.mUnk_1f3 = (data_0206d460.mUnk_1f3 & ~7) | 2;
    data_0206d460.mUnk_1d8 = 2 << 18;
    data_0206d460.mUnk_144 |= 1;
    data_0206d460.mUnk_240.mX = -0x106800;
    data_0206d460.mUnk_240.mY = -0x70800;
    data_0206d460.mUnk_248 = 0;
    data_0206d460.mUnk_24c = 0;
    data_0206d460.mUnk_1b0.x += data_0206d460.mUnk_240.mX;
    data_0206d460.mUnk_1b0.z += data_0206d460.mUnk_240.mY;
    data_0206d460.mUnk_258 = 0;
    data_0206d460.mUnk_25c = 0;
    data_0206d460.mUnk_264 = 0;
    data_0206d460.mUnk_268 = 0;
    data_0206d460.mUnk_2b8f8 = func_02005c10(data_020b10fc.mUnk_08, 2) << 8;
    data_0206d460.mUnk_2b8fc = func_02005c10(data_020b10fc.mUnk_08, 3) << 8;
    data_0206d460.mUnk_2b900 = func_02005c10(data_020b10fc.mUnk_08, 4) - 1;
    data_0206d460.mUnk_2b904 = func_02005c10(data_020b10fc.mUnk_08, 5) * 60000 / 1000;
    data_0206d460.mUnk_2b908 = func_02005c10(data_020b10fc.mUnk_08, 6);
    data_0206d460.mUnk_2b90c = func_02005c10(data_020b10fc.mUnk_08, 7);
    data_0206d460.mUnk_2b910 = func_02005c10(data_020b10fc.mUnk_08, 8) * 60 / 1000;
    memset(data_0206d460.mUnk_2a8b4, 0, 0x1000);
    for (i = 0; i < 42; i++) {
        for (j = 0; j < 6; j++) {
            for (k = 0; k < 8; k++) {
                data_0206d460.mUnk_2a8b4[i * 64 + k * 8 + j + 1] = func_02005c24(k, j + i * 6 + 2);
            }
        }
    }
    memset(data_0206d460.mUnk_2b8b4, 0, 0x40);
    if (!(data_0206d460.mUnk_2b900 < 10)) {
        OS_Terminate();
    }
    data_0206d460.mUnk_2b8f4 = 0;
    do {
        value = func_02005c3c(data_0206d460.mUnk_2b8f4, data_0206d460.mUnk_2b900 + 2);
        data_0206d460.mUnk_2b8b4[data_0206d460.mUnk_2b8f4] = value - 1;
        data_0206d460.mUnk_2b8f4++;
    } while (value >= 0);
    for (i = 0; i < 4; i++) {
        func_02005d94(i);
    }
    for (i = 0; i < 128; i++) {
        data_0206d460.mList[i].mKind = i;
    }
    for (i = 2; i < 4; i++) {
        func_02005de0(i, data_0206d460.mUnk_2b8b4[func_0201fd84(0, data_0206d460.mUnk_2b8f4 - 2)]);
    }
    data_0206d460.mUnk_000[0] = 0;
    data_0206d460.mUnk_004 = data_0206d460.mUnk_2b904;
    data_0206d460.mUnk_008 = 0;
    data_0206d460.mUnk_00c = 0;
    data_0206d460.mUnk_270 = data_0206d460.mUnk_2b8f8;
    func_02005a8c(1, 0);
}

void func_02006b1c(void) {
    func_0201168c(data_020b6ee0.mUnk_a8e4, data_0206d460.mUnk_008 >> 8, data_0206d448, data_0206d460.mUnk_004, 2);
    func_02033614(data_020c5e44.mUnk_0c0.mUnk_228.x, data_020c5e44.mUnk_0c0.mUnk_228.z);
    func_0202733c(&data_0206d460.mUnk_010, &data_0206d460.mUnk_1b0, 0);
    func_02005cb4();
}

void func_02006b68(void) {
    s32 pos[2];
    u8 mode;
    func_020059cc(data_0206d460.mUnk_008 >> 8);
    mode = func_02005bf8();
    if (mode != 0) {
        int r;
        func_02033fc8();
        func_02006b1c();
        r = func_02005b1c(1);
        if (mode == 2 && r == 0) {
            func_020059b4(func_02005c04(), data_0206d460.mUnk_008 >> 8);
        }
        return;
    }
    func_020066c8();
    data_0206d460.mUnk_248 = (data_0206d460.mUnk_248 < 0x2ee000 ? data_0206d460.mUnk_248 : 0x2ee000) < -0x2ee000
                                 ? -0x2ee000
                                 : (data_0206d460.mUnk_248 < 0x2ee000 ? data_0206d460.mUnk_248 : 0x2ee000);
    data_0206d460.mUnk_24c = (data_0206d460.mUnk_24c < 0x2ee000 ? data_0206d460.mUnk_24c : 0x2ee000) < -0x2ee000
                                 ? -0x2ee000
                                 : (data_0206d460.mUnk_24c < 0x2ee000 ? data_0206d460.mUnk_24c : 0x2ee000);
    data_0206d460.mUnk_240.mX += data_0206d460.mUnk_248 / 60;
    data_0206d460.mUnk_240.mY += data_0206d460.mUnk_24c / 60;
    if (data_0206d460.mUnk_25c == 0 && data_0206d460.mUnk_004 != 0) {
        if (data_0206d460.mUnk_240.mX < -0x106800 ||
            (data_0206d460.mUnk_240.mX > (50 << 10) && data_0206d460.mUnk_268 == 0)) {
            data_0206d460.mUnk_248 = 0;
        }
        if (data_0206d460.mUnk_264 != 0) {
            if (data_0206d460.mUnk_240.mX < -0x106800) {
                data_0206d460.mUnk_240.mX += 25 << 10;
            } else {
                data_0206d460.mUnk_264 = 0;
            }
            if (data_0206d460.mUnk_240.mX > (50 << 10)) {
                data_0206d460.mUnk_240.mX = 50 << 10;
            }
        } else if (data_0206d460.mUnk_268 != 0) {
            if (data_0206d460.mUnk_240.mX < (50 << 10)) {
                data_0206d460.mUnk_268 = 0;
            }
        } else {
            data_0206d460.mUnk_240.mX =
                (data_0206d460.mUnk_240.mX < (50 << 10) ? data_0206d460.mUnk_240.mX : 50 << 10) < -0x106800
                    ? -0x106800
                    : (data_0206d460.mUnk_240.mX < (50 << 10) ? data_0206d460.mUnk_240.mX : 50 << 10);
        }
    }
    if (data_0206d460.mUnk_240.mY < -0x177000 || data_0206d460.mUnk_240.mY > (150 << 12)) {
        data_0206d460.mUnk_24c = 0;
    }
    data_0206d460.mUnk_240.mY =
        (data_0206d460.mUnk_240.mY < (150 << 12) ? data_0206d460.mUnk_240.mY : 150 << 12) < -0x177000
            ? -0x177000
            : (data_0206d460.mUnk_240.mY < (150 << 12) ? data_0206d460.mUnk_240.mY : 150 << 12);
    func_02025db4(&data_0206d460.mUnk_1b0, pos);
    if (data_0206d460.mUnk_004 != 0) {
        if (func_020062ec()) {
            if (data_020c18bc.mUnk_2ac_1) {
                s32 diff = data_020c18bc.mUnk_2af - pos[1];
                if (abs(diff) > 5) {
                    data_0206d460.mUnk_24c += (diff != 0 ? (diff >> 31) | 1 : 0) * (50 << 12);
                } else {
                    data_0206d460.mUnk_24c >>= 1;
                }
            } else if (data_020c18bc.mUnk_10 & 0x40) {
                data_0206d460.mUnk_24c -= 50 << 12;
            } else if (data_020c18bc.mUnk_10 & 0x80) {
                data_0206d460.mUnk_24c += 50 << 12;
            } else {
                func_0200b9cc(&data_0206d460.mUnk_24c, 50 << 12);
            }
        }
        if (func_020062e8()) {
            if (data_020c18bc.mUnk_2ac_1) {
                s32 diff = data_020c18bc.mUnk_2ae - pos[0];
                if (abs(diff) > 5) {
                    data_0206d460.mUnk_248 += (diff != 0 ? (diff >> 31) | 1 : 0) * (25 << 12);
                } else {
                    data_0206d460.mUnk_248 >>= 1;
                }
            } else if (data_020c18bc.mUnk_10 & 0x20) {
                data_0206d460.mUnk_248 -= 25 << 12;
            } else if (data_020c18bc.mUnk_10 & 0x10) {
                data_0206d460.mUnk_248 += 25 << 12;
            } else {
                func_0200b9cc(&data_0206d460.mUnk_248, 25 << 12);
            }
        } else {
            switch (data_0206d460.mUnk_260) {
                case 0:
                    if (data_0206d460.mUnk_25c != 0) {
                        func_0200b9cc(&data_0206d460.mUnk_24c, 50 << 12);
                        data_0206d460.mUnk_25c--;
                        if (data_0206d460.mUnk_240.mX > data_0206d460.mUnk_250.mX - data_0206d460.mUnk_258) {
                            data_0206d460.mUnk_248 -= 50 << 12;
                        } else {
                            func_0200b9cc(&data_0206d460.mUnk_248, 25 << 12);
                        }
                    } else {
                        data_0206d460.mUnk_248 = 0;
                        data_0206d460.mUnk_258 = 0;
                    }
                    if (data_0206d460.mUnk_240.mX < -0x106800) {
                        data_0206d460.mUnk_264 = 1;
                    }
                    break;
                case 1:
                    if (data_0206d460.mUnk_25c != 0) {
                        data_0206d460.mUnk_25c--;
                        if (data_0206d460.mUnk_240.mX < data_0206d460.mUnk_250.mX + data_0206d460.mUnk_258) {
                            data_0206d460.mUnk_248 += 50 << 12;
                        } else {
                            func_0200b9cc(&data_0206d460.mUnk_248, 25 << 12);
                        }
                    } else if (data_0206d460.mUnk_240.mX > data_0206d460.mUnk_250.mX) {
                        data_0206d460.mUnk_248 -= 25 << 12;
                    } else {
                        data_0206d460.mUnk_248 = 0;
                        data_0206d460.mUnk_258 = 0;
                    }
                    if (data_0206d460.mUnk_240.mX > (50 << 10)) {
                        data_0206d460.mUnk_268 = 1;
                    }
                    break;
            }
        }
    }
    data_0206d460.mUnk_1b0 = data_020c5e44.mUnk_0c0.mUnk_228;
    data_0206d460.mUnk_1b0.x += data_0206d460.mUnk_240.mX;
    data_0206d460.mUnk_1b0.z += data_0206d460.mUnk_240.mY;
    func_0200b928(&data_0206d460.mUnk_270,
                  func_0201fbe4(data_0206d460.mUnk_240.mX / (50 << 10), -31, 1, data_0206d460.mUnk_2b8f8 >> 8,
                                data_0206d460.mUnk_2b8fc >> 8)
                      << 8,
                  2);
    func_02005f70();
    func_020064e8();
    if (data_0206d460.mUnk_004 != 0) {
        data_0206d460.mUnk_004--;
        if (data_0206d460.mUnk_004 == 0) {
            data_0206d460.mUnk_25c = 0;
        }
    } else {
        data_0206d460.mUnk_248 -= 25 << 12;
        if (data_0206d460.mUnk_270 <= 0) {
            int flag = (data_0206d460.mUnk_008 >> 8) >= data_0206d460.mUnk_2b908;
            data_0206d460.mUnk_270 = 0;
            func_02005a8c(0, flag);
        }
    }
    func_02006b1c();
}

void func_02006fac(void) {
    func_020102f8();
}

int func_02006fb4(u8 stage) {
    return func_02005c10(stage, 6);
}
