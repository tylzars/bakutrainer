#include <nitro/os.h>
#include <Main/Unknown/functions.h>
#include <Main/Unknown/stage_select.h>
#include <Main/Unknown/globals.h>
#include <Main/Unknown/stage_data.h>
#include <Main/Unknown/state_flow.h>
#include <Main/Unknown/prompt_screen.h>
#include <Main/Unknown/battle_screen.h>
#include <Main/Unknown/stage_info_panel.h>
#include <Main/Unknown/stage_progress.h>
#include <Main/Unknown/sprite_slots.h>
#include <Main/clib.h>

const u8 data_020500d4[3] = {1, 0, 2};

void func_020145bc(void) {
    StageRec *rec;
    int stage = data_020b6ee0.mUnk_a8e4;
    int idx;

    if (stage != -1) {
        rec = &data_020b1990[stage];
    } else {
        rec = NULL;
    }
    switch ((u32) data_020b6ee0.mUnk_a564) {
        case 0:
            break;
        case 1:
            if (func_0200fb00() == 10 && data_020b6ee0.mUnk_a146 <= 1) {
                data_020b6ee0.mUnk_a564 = 2;
                func_0201033c(1, 10);
                func_02011bcc(0x4c, 2, 0x1e0);
            }
            break;
        case 2:
            if (func_0200fb00() == 10 && data_020b6ee0.mUnk_a8e4 == 0) {
                data_020b6ee0.mUnk_a564 = 3;
                func_02011bcc(0x54, 1, 0x1e0);
                rec->mUnk_22e = 1;
            }
            break;
        case 3:
            if (func_0200fb00() == 10 && data_020b6ee0.mUnk_a570 != 0) {
                data_020b6ee0.mUnk_a564 = 4;
                func_02011bcc(0x55, 1, 0x1e0);
            }
            break;
        case 4:
            if (func_0200fb00() == 10 && data_020b1990[0].mUnk_226 == data_020b1990[0].mInfo.mUnk_10) {
                data_020b6ee0.mUnk_a564 = 5;
                data_020b6ee0.mUnk_a56c = 2;
                func_02011bcc(0x4d, 1, 0x1e0);
            }
            break;
        case 5:
            if (func_0200fb00() == 10) {
                u8 v = data_020b6ee0.mUnk_a146;
                if (v == 6 || v == 7 || v == 8 || v == 3) {
                    data_020b6ee0.mUnk_a56c = 1;
                    data_020b6ee0.mUnk_a564 = 6;
                }
            }
            break;
        case 6:
            if (func_0200fb00() == 10 && data_020b6ee0.mUnk_a146 <= 1) {
                data_020b6ee0.mUnk_a564 = 7;
                func_02011bcc(0x56, 1, 0x1e0);
            }
            break;
        case 7:
            if (func_0200fb00() == 10 && data_020b6ee0.mUnk_a146 <= 2 && data_020b1990[0].mInfo.mUnk_12 == 100 &&
                data_020b6ee0.mUnk_a128 == -1) {
                data_020b6ee0.mUnk_a564 = 8;
                data_020b6ee0.mUnk_a56d = 2;
                data_020b6ee0.mUnk_a8e4 = 0;
                data_020b6ee0.mUnk_a146 = 1;
                func_02011bcc(0x4f, 1, 0x1e0);
            }
            break;
        case 8:
            if (func_0200fb00() == 13) {
                data_020b6ee0.mUnk_a564 = 9;
                func_0201033c(0x27, 13);
            }
            break;
        case 9:
            if (func_0200fb00() == 4) {
                data_020b6ee0.mUnk_a564 = 10;
                data_020b6ee0.mUnk_a56d = 1;
            }
            break;
        case 10:
            if (func_0200fb00() == 10) {
                data_020b6ee0.mUnk_a564 = 11;
                data_020b6ee0.mUnk_a56e = 2;
                func_0201033c(2, 10);
                func_02011bcc(0x4e, 1, 0x1e0);
            }
            break;
        case 11:
            if (func_0200fb00() == 6) {
                data_020b6ee0.mUnk_a564 = 12;
                func_0201033c(4, 6);
            }
            break;
        case 12:
            if (func_0200fb00() == 10 && data_020b6ee0.mUnk_a146 <= 1 && data_020b6ee0.mUnk_a530 == 1) {
                data_020b6ee0.mUnk_a564 = 13;
                data_020b6ee0.mUnk_a56e = 0;
                func_02011bcc(0x57, 1, 0x1e0);
            }
            break;
        case 13:
            idx = func_02014f14(0, 0x12);
            if (idx == -1) {
                OS_Terminate();
            }
            if (func_0200fb00() == 10 && data_020b1990[idx].mUnk_226 == data_020b1990[idx].mInfo.mUnk_10) {
                data_020b6ee0.mUnk_a564 = 14;
                func_02011bcc(0x56, 1, 0x1e0);
            }
            break;
        case 14:
            idx = func_02014f14(0, 0x12);
            if (idx == -1) {
                OS_Terminate();
            }
            if (func_0200fb00() == 10 && data_020b6ee0.mUnk_a146 <= 2 && data_020b1990[idx].mInfo.mUnk_12 == 100 &&
                data_020b6ee0.mUnk_a128 == -1) {
                data_020b6ee0.mUnk_a564 = 15;
                data_020b6ee0.mUnk_a56d = 2;
                data_020b6ee0.mUnk_a8e4 = idx;
                data_020b6ee0.mUnk_a146 = 1;
                func_02011bcc(0x58, 1, 0x1e0);
            }
            break;
        case 15:
            idx = func_02014f14(0, 0x12);
            if (idx == -1) {
                OS_Terminate();
            }
            if ((func_0200fb00() == 4 || func_0200fb00() == 15) && data_020b6ee0.mUnk_a8e4 == idx) {
                data_020b6ee0.mUnk_a564 = 16;
                data_020b6ee0.mUnk_a56d = 1;
                data_020b6ee0.mUnk_a56e = 2;
                func_02011bcc(0x4e, 1, 0x1e0);
            }
            break;
        case 16:
            if (func_0200fb00() == 10 && data_020b6ee0.mUnk_a146 <= 1 && data_020b6ee0.mUnk_a530 == 2) {
                data_020b6ee0.mUnk_a564 = 17;
                data_020b6ee0.mUnk_a56e = 0;
                data_020b6ee0.mUnk_a56f = 2;
                func_02011bcc(0x50, 1, 0x1e0);
            }
            break;
        case 17:
            if (func_0200fb00() == 9) {
                data_020b6ee0.mUnk_a564 = 99;
                data_020b6ee0.mUnk_a56c = 1;
                data_020b6ee0.mUnk_a56d = 1;
                data_020b6ee0.mUnk_a56e = 1;
                data_020b6ee0.mUnk_a56f = 1;
                func_0201033c(0x2d, 9);
                func_02011bcc(-1, 0, 0);
            }
            break;
        case 98:
            data_020b6ee0.mUnk_a56c = 1;
            data_020b6ee0.mUnk_a56d = 1;
            data_020b6ee0.mUnk_a56e = 1;
            data_020b6ee0.mUnk_a56f = 1;
            data_020b6ee0.mUnk_a564 = 99;
            func_02011bcc(-1, 0, 0);
            break;
        case 99:
            break;
    }
}

int func_020149b0(void) {
    if (data_020b6ee0.mUnk_a534 == -1) {
        OS_Terminate();
    }
    if (data_020b6ee0.mUnk_a534 == data_020b6ee0.mUnk_a530 && data_020b6ee0.mUnk_a560 == 0) {
        return 1;
    }
    return 0;
}

void func_020149e0(void) {
    switch (data_020b6ee0.mUnk_a538) {
        case 0:
            break;
        case 1:
            if (func_0200fb5c() == 1) {
                if (func_020149b0() == 1) {
                    int sel;
                    data_020b6ee0.mUnk_a538 = 2;
                    sel = func_02014538(data_020b6ee0.mUnk_a534, 3);
                    func_02014ac8(data_020b6ee0.mUnk_a534, 0);
                    func_0201033c(sel, 0x13);
                } else {
                    int sel;
                    data_020b6ee0.mUnk_a538 = 2;
                    sel = func_02014538(data_020b6ee0.mUnk_a534, 5);
                    func_02014ac8(data_020b6ee0.mUnk_a534, 1);
                    func_0201033c(sel, 0x13);
                }
            }
            break;
        case 2:
            if (func_0200fb5c() == 1) {
                if (func_020149b0() == 1) {
                    data_020b6ee0.mUnk_a538 = 0;
                    if (func_02007758() == 0) {
                        func_02014bf0();
                        func_02014afc(data_020b6ee0.mUnk_a534, 3);
                        func_0200fa80(0x14);
                        func_0200fab0();
                    } else {
                        func_02014bf0();
                        func_02014afc(data_020b6ee0.mUnk_a534, 1);
                        func_0200fa80(0x14);
                        func_0200fab0();
                    }
                } else {
                    data_020b6ee0.mUnk_a538 = 0;
                    func_02007758();
                    func_02014bf0();
                    func_02014afc(data_020b6ee0.mUnk_a534, 1);
                    func_0200fa80(0x14);
                    func_0200fab0();
                }
            }
            break;
    }
}

void func_02014ac8(u32 stage, int mode) {
    int i;

    if (stage >= 0x19) {
        OS_Terminate();
    }
    data_020b6ee0.mUnk_a8ef = 0;
    for (i = 0; i < 3; i++) {
        func_02014de4(1, func_0201456c(data_020500d4[i]));
    }
}

void func_02014afc(int stage, int flags) {
    int i;

    if (flags & 1) {
        for (i = 0; i < 3; i++) {
            StageRec *rec = func_02015190(i);
            if (rec != NULL) {
                if (data_020b6ee0.mUnk_a8f3[i] & 1) {
                    func_020150a0(rec, 1);
                }
                if (!(data_020b6ee0.mUnk_a8f3[i] & 2)) {
                    if (data_020b6ee0.mUnk_a8f3[i] & 4) {
                        func_02014ffc(rec, -10, 0, 0);
                    } else {
                        func_02014ffc(rec, -30, 0, 0);
                    }
                }
            }
        }
    }
    data_020b6ee0.mUnk_a53c = 0;
    if (flags & 2) {
        for (i = 0; i < 3; i++) {
            int sel = func_02014538(stage, i + 15);
            if (sel != -1) {
                int found = func_02014f14(0, sel);
                if (found == -1) {
                    int v = func_02014de4(0, sel);
                    data_020b6ee0.mUnk_a540[data_020b6ee0.mUnk_a53c++] = v;
                    if (data_020b6ee0.mUnk_a8e4 == -1) {
                        data_020b6ee0.mUnk_a8e4 = v;
                        data_020b6ee0.mUnk_a14a = 1;
                    }
                } else {
                    data_020b6ee0.mUnk_a540[data_020b6ee0.mUnk_a53c++] = found;
                }
            }
        }
    }
}

void func_02014bf0(void) {
    int i;
    StageRec *rec;

    for (i = 0; i < data_020b6ee0.mUnk_a8ee; i++) {
        rec = &data_020b1990[i];
        func_02014ccc(&rec->mInfo);
        if (rec->mUnk_22d == 0) {
            func_02014ffc(rec, 10, 0, 0);
        }
    }
}

void func_02014c38(StageInfo *info) {
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
    info->mUnk_1a = func_0201fd68() & 3;
}

void func_02014cdc(StageInfo *info) {
    u32 r = func_0201fd68() % 3;
    info->mUnk_1a = (info->mUnk_1a + 1 + r) & 3;
}

void func_02014cf8(StageInfo *info) {
    int i;

    for (i = 0; i < 5; i++) {
        if (info->mUnk_1c[i] == -1) {
            info->mUnk_1c[i] = func_02014d2c(info);
        } else if (info->mUnk_1c[i] == -2) {
            info->mUnk_1c[i] = -1;
        }
    }
}

int func_02014d2c(StageInfo *info) {
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
    roll = func_0201fd68() % 10000;
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

static void func_02014e4c(int index, int stage);
static void func_02014ebc(int index, int stage);

void func_02014d8c(int a) {
    memset(data_020b1990, 0, 0x5550);
    memset(data_020b1524, 0, 0x72);
    data_020b6ee0.mUnk_a8ee = 0;
    data_020b6ee0.mUnk_a8ef = data_020b6ee0.mUnk_a8ee;
    func_02014de4(1, 3);
    func_02014de4(1, 4);
    func_02014de4(1, 5);
    if (a == 0) {
        func_02014de4(0, 0x19);
    }
}

int func_02014de4(int kind, int index) {
    int slot;

    if (index >= STAGE_MAX) {
        OS_Terminate();
    }
    switch (kind) {
        case 0:
            if (data_020b6ee0.mUnk_a8ee >= STAGE_MAX) {
                OS_Terminate();
            }
            slot = data_020b6ee0.mUnk_a8ee++;
            func_02014e4c(slot, index);
            break;
        case 1:
            if (data_020b6ee0.mUnk_a8ef >= 3) {
                OS_Terminate();
            }
            slot = data_020b6ee0.mUnk_a8ef++;
            func_02014ebc(slot, index);
            break;
        default:
            if (kind != 0 && kind != 1) {
                OS_Terminate();
            }
            break;
    }
    return slot;
}

static void func_02014e4c(int index, int stage) {
    StageRec *rec;

    if (index >= STAGE_MAX) {
        OS_Terminate();
    }
    if (index >= data_020b6ee0.mUnk_a8ee) {
        OS_Terminate();
    }
    rec = &data_020b1990[index];
    memset(rec, 0, sizeof(StageRec));
    rec->mUnk_226 = 99;
    rec->mInfo.mUnk_13 = stage;
    func_02014c38(&rec->mInfo);
    func_02014cf8(&rec->mInfo);
    rec->mUnk_1e4 = rec->mUnk_1e8 = rec->mUnk_1ec = -1;
}

static void func_02014ebc(int index, int stage) {
    StageInfo *info;

    if (index >= 3) {
        OS_Terminate();
    }
    if (index >= data_020b6ee0.mUnk_a8ef) {
        OS_Terminate();
    }
    info = &data_020b1524[index];
    // The original clears 0x230 bytes, a StageRec's size, from this StageInfo.
    memset(info, 0, sizeof(StageRec));
    if (stage != -1) {
        info->mUnk_13 = stage;
        func_02014c38(info);
        func_02014cf8(info);
    } else {
        info->mUnk_04 = -9999;
    }
}

int func_02014f14(int kind, int index) {
    int i;
    u8 n;
    int v;

    if ((u32) index >= STAGE_MAX) {
        OS_Terminate();
    }
    switch (kind) {
        case 0:
            n = data_020b6ee0.mUnk_a8ee;
            for (i = 0; i < n; i++) {
                v = data_020b1990[i].mInfo.mUnk_13;
                if (v == index) {
                    break;
                }
            }
            if (i == n) {
                i = -1;
            }
            break;
        case 1:
            n = data_020b6ee0.mUnk_a8ef;
            for (i = 0; i < n; i++) {
                v = data_020b1524[i].mUnk_13;
                if (v == index) {
                    break;
                }
            }
            if (i == n) {
                i = -1;
            }
            break;
        default:
            if (kind != 0 && kind != 1) {
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

int func_02014ffc(StageRec *rec, int amount, int a, int b) {
    StageInfo *info = &rec->mInfo;
    int value = info->mUnk_12;

    if (a != 0 && amount > 0) {
        func_0202f110(0xe8, 100, 0, 0x40, 0xffff, 0);
    }
    if (b != 0 && amount > 0 && data_020b6ee0.mUnk_a8e4 != -1) {
        func_0200d78c(0x1e, 0, func_020117e0(value), 0x5e, 0, 0, 0x3c, 2);
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

void func_020150ec(int a) {
    int i;

    for (i = 0; i < 3; i++) {
        data_020b6ee0.mUnk_c60[i] = -1;
    }
    data_020b6ee0.mUnk_c78 = 0;
    data_020b6ee0.mUnk_c79 = 0;
}

int func_02015114(int a, int b) {
    int old;
    StageRec *rec;

    if ((u32) a >= 3) {
        OS_Terminate();
    }
    if (b >= STAGE_MAX) {
        OS_Terminate();
    }
    old = data_020b6ee0.mUnk_c60[a];
    if (old != -1) {
        if (old >= STAGE_MAX) {
            OS_Terminate();
        }
        rec = &data_020b1990[old];
        rec->mUnk_22d = 0;
    }
    if (b != -1) {
        if (b >= data_020b6ee0.mUnk_a8ee) {
            OS_Terminate();
        }
        data_020b6ee0.mUnk_c60[a] = b;
        rec = &data_020b1990[b];
        rec->mUnk_22d = 1;
    } else {
        data_020b6ee0.mUnk_c60[a] = -1;
    }
    return old;
}

StageRec *func_02015190(int index) {
    int stage;

    if ((u32) index >= 3) {
        OS_Terminate();
    }
    stage = data_020b6ee0.mUnk_c60[index];
    if (stage >= STAGE_MAX) {
        OS_Terminate();
    }
    if (stage == -1) {
        return 0;
    }
    return &data_020b1990[stage];
}

int func_020151cc(int stage) {
    int i;

    if (stage < 0) {
        OS_Terminate();
    }
    for (i = 0; i < 3; i++) {
        if (stage == data_020b6ee0.mUnk_c60[i]) {
            return i;
        }
    }
    return -1;
}

void func_020151f8(void) {
    int i;
    int j;
    u8 stage;

    for (i = 0; i < 3; i++) {
        stage = data_020500d4[i];
        if (func_02015190(stage) == 0) {
            for (j = 0; j < data_020b6ee0.mUnk_a8ee; j++) {
                if (data_020b1990[j].mUnk_22d == 0) {
                    func_02015114(stage, j);
                }
            }
        }
    }
}

int func_02015260(void) {
    int i;

    for (i = 0; i < data_020b6ee0.mUnk_a8ee; i++) {
        if (data_020b1990[i].mUnk_22d == 0) {
            return i;
        }
    }
    return -1;
}

int func_02015294(int stage) {
    int i;

    if (stage < 0) {
        OS_Terminate();
    }
    for (i = stage + 1; i < data_020b6ee0.mUnk_a8ee; i++) {
        if (data_020b1990[i].mUnk_22d == 0) {
            return i;
        }
    }
    return func_02015260();
}

int func_020152d4(void) {
    int i;

    for (i = data_020b6ee0.mUnk_a8ee - 1; i >= 0; i--) {
        if (data_020b1990[i].mUnk_22d == 0) {
            return i;
        }
    }
    return -1;
}

int func_0201530c(int stage) {
    int i;

    if (stage < 0) {
        OS_Terminate();
    }
    for (i = stage - 1; i >= 0; i--) {
        if (data_020b1990[i].mUnk_22d == 0) {
            return i;
        }
    }
    return func_020152d4();
}
