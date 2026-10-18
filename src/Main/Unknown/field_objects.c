#include <Main/Unknown/functions.h>
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
            if (o->mUnk_0c < data_020c60c4.mUnk_70 - 0x177000) {
                o->mUnk_0c = data_020c60c4.mUnk_70 - 0x177000;
                o->mUnk_1c0 = -o->mUnk_1c0;
            } else if (o->mUnk_0c > data_020c60c4.mUnk_70 + (150 << 12)) {
                o->mUnk_0c = data_020c60c4.mUnk_70 + (150 << 12);
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
                u8 r = func_0201fd84(0, data_02098d40.mUnk_14 - 2);
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
            data_0206d460.mUnk_00c = data_02098d60.mUnk_10;
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
            func_020059e8(&data_0206d460.mUnk_004, 60000 * data_02098d60.mUnk_0c / 1000);
            sprintf(text, "+%d'", data_02098d60.mUnk_0c);
            func_0200d838(text, 2, data_0201a61c, 30, pos[0], pos[1], 0, -128, 100, 1, 0);
            func_0202f110(117, 100, 0, 64, 0xffff, 0);
            break;
    }
}
