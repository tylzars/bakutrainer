#include <nitro/os.h>
#include <Main/Unknown/save_data.h>
#include <Main/Unknown/state_flow.h>
#include <Main/Unknown/globals.h>
#include <Main/Unknown/functions.h>
#include <Main/Unknown/language_menu.h>

#define STAGE_UNIT 51200

void func_02010648(u32 a) {
    u32 state;
    u32 lang10;
    u32 base;
    u32 id;

    if (a > data_020c1bdc.mUnk_0e) {
        func_02021e44(SAVE_BLOCK_ID, 0x1c9);
        state = a;
        lang10 = data_020b1078.mLanguage * 10;
        id = data_020c1bbc.mUnk_2c + 4 + lang10;
        base = data_020c1bbc.mUnk_2c + 9;
        func_0201fed4(0);
        if (a != 2) {
            data_020c1bbc.mUnk_24 = data_020c1cbc.mUnk_5c;
            func_0201f880(3);
            func_02020854(1);
            func_0202c1c4();
            func_02022370();
            func_0201eb74(6);
            func_0201eb74(2);
            func_0201dde0(0xdaa, 3);
            func_0202bdd8(0x10cb, 0x10ca, 0x10cc, 2, 2);
        }
        switch (a) {
            case 1:
            case 2:
                id += 1;
                state = 1;
                break;
            case 5:
                data_020c1cbc.mUnk_50 |= 2;
                data_020c1cfc.mUnk_17 = 0x78;
                id += 4;
                break;
            case 3:
                id += 2;
                break;
            case 4:
                id += 3;
                break;
            case 6:
                id += 6;
                break;
            case 10:
                break;
        }
        func_020215fc(0x80, 0x60, data_0201a6c6, (const char *) func_02021e18(SAVE_BLOCK_ID, id), 2, 0xfa, 0x10, 2);
        if (a == 2) {
            func_020215fc(0x80, 0x88, data_0201a6c6, (const char *) func_02021e18(SAVE_BLOCK_ID, base + lang10), 2,
                          0xfa, 0x10, 2);
        }
        func_0201bb30();
        data_020c1cbc.mUnk_6c |= 0x80000;
        func_0201f830(3);
        data_020c1bdc.mUnk_0e = state;
    }
}

int func_0201079c(u32 slot) {
    int ok = 0;
    SaveRecord *rec = func_0201ffbc();
    Unk_020b6ee0 *work = &data_020b6ee0;
    int i;
    int j;
    int v;
    StageRec *stage;

    func_02020140(slot, rec, SAVE_BLOCK_ID);
    if (rec->mUnk_10 == 1) {
        ok = 1;
        work->mUnk_a530 = rec->mUnk_14;
        work->mUnk_a560 = rec->mUnk_18;
        work->mUnk_a564 = rec->mUnk_28;
        work->mUnk_a56c = rec->mUnk_2c;
        work->mUnk_a56d = rec->mUnk_30;
        work->mUnk_a56e = rec->mUnk_34;
        work->mUnk_a56f = rec->mUnk_38;
        v = rec->mUnk_3c;
        if (v == 99) {
            v = -1;
        }
        work->mUnk_a120 = v;
        v = rec->mUnk_40;
        if (v == 999) {
            v = -1;
        }
        work->mUnk_a128 = v;
        func_02011bcc(work->mUnk_a128, work->mUnk_a120, 480);
        data_020b10f8.mUnk_18 = rec->mUnk_44;
        data_020b10f8.mUnk_1c = rec->mUnk_48;
        data_020b10f8.mUnk_20 = rec->mUnk_4c;
        data_020b10f8.mUnk_24 = rec->mUnk_50;
        data_020b10f8.mUnk_28 = rec->mUnk_54;
        data_020c17c0.mUnk_0e = 0;
        for (i = 0; i < STAGE_MAX; i++) {
            if (rec->mUnk_5c[i] == 1) {
                stage = &data_020b1990[func_02014de4(0, i)];
                stage->mUnk_1e4 = (rec->mUnk_f8[i] - 0x100) * STAGE_UNIT;
                stage->mUnk_1e8 = 0;
                stage->mUnk_1ec = (rec->mUnk_194[i] - 0x100) * STAGE_UNIT;
                stage->mInfo.mUnk_12 = rec->mUnk_230[i];
                stage->mInfo.mUnk_04 = rec->mUnk_2cc[i];
                stage->mInfo.mUnk_11 = rec->mUnk_368[i];
                stage->mInfo.mUnk_1c[0] = rec->mUnk_404[i] == 0 ? -1 : func_020144e0(rec->mUnk_404[i] - 1, 2);
                stage->mInfo.mUnk_1c[1] = rec->mUnk_4a0[i] == 0 ? -1 : func_020144e0(rec->mUnk_4a0[i] - 1, 2);
                stage->mInfo.mUnk_1c[2] = rec->mUnk_53c[i] == 0 ? -1 : func_020144e0(rec->mUnk_53c[i] - 1, 2);
                stage->mInfo.mUnk_1c[3] = rec->mUnk_5d8[i] == 0 ? -1 : func_020144e0(rec->mUnk_5d8[i] - 1, 2);
                stage->mInfo.mUnk_1c[4] = rec->mUnk_674[i] == 0 ? -1 : func_020144e0(rec->mUnk_674[i] - 1, 2);
            }
        }
        for (i = 0; i < 3; i++) {
            u8 slotStage = rec->mUnk_1c[i];
            if (slotStage != 0) {
                u8 s = slotStage - 1;
                if (s >= STAGE_MAX) {
                    OS_Terminate();
                }
                j = func_02014f14(0, s);
                if (j == -1) {
                    OS_Terminate();
                }
                func_02015114(i, j);
            } else {
                func_02015114(i, -1);
            }
        }
    }
    func_0201ffcc(&rec);
    return ok;
}

void func_020109f4(u32 slot) {
    SaveRecord *rec = func_0201ffbc();
    Unk_020b6ee0 *work = &data_020b6ee0;
    int i;
    int v;
    StageRec *stage;

    func_02010d4c(rec);
    rec->mUnk_10 = 1;
    rec->mUnk_14 = data_020c13e0.mUnk_30;
    rec->mUnk_18 = data_020c13e0.mUnk_60;
    rec->mUnk_28 = data_020c13e0.mUnk_64;
    rec->mUnk_2c = data_020c1440.mUnk_0c;
    rec->mUnk_30 = data_020c1440.mUnk_0d;
    rec->mUnk_34 = data_020c1440.mUnk_0e;
    rec->mUnk_38 = data_020c1440.mUnk_0f;
    if (data_020c0fe0.mUnk_28 == -1) {
        v = work->mUnk_a12c;
        if (v == -1) {
            v = 999;
        }
        rec->mUnk_40 = v;
        v = work->mUnk_a124;
        if (v == -1) {
            v = 99;
        }
        rec->mUnk_3c = v;
    } else {
        v = work->mUnk_a128;
        if (v == -1) {
            v = 999;
        }
        rec->mUnk_40 = v;
        v = work->mUnk_a120;
        if (v == -1) {
            v = 99;
        }
        rec->mUnk_3c = v;
    }
    rec->mUnk_44 = data_020b10f8.mUnk_18;
    rec->mUnk_48 = data_020b10f8.mUnk_1c;
    rec->mUnk_4c = data_020b10f8.mUnk_20;
    rec->mUnk_50 = data_020b10f8.mUnk_24;
    rec->mUnk_54 = data_020b10f8.mUnk_28;
    for (i = 0; i < 3; i++) {
        u8 slotStage = 0;
        stage = func_02015190(i);
        if (stage != NULL) {
            slotStage = stage->mInfo.mUnk_13;
            if (slotStage >= STAGE_MAX) {
                OS_Terminate();
            }
            slotStage = slotStage + 1;
        }
        rec->mUnk_1c[i] = slotStage;
    }
    for (i = 0; i < STAGE_MAX; i++) {
        rec->mUnk_5c[i] = 0;
    }
    rec->mUnk_58 = data_020c17c0.mUnk_0e;
    for (i = 0; i < data_020c17c0.mUnk_0e; i++) {
        u8 id;
        stage = &data_020b1990[i];
        id = stage->mInfo.mUnk_13;
        if (id >= STAGE_MAX) {
            OS_Terminate();
        }
        if (rec->mUnk_5c[id] == 1) {
            OS_Terminate();
        }
        rec->mUnk_5c[id] = 1;
        v = stage->mUnk_1e4 / STAGE_UNIT;
        if (v <= -0x100 || v >= 0x100) {
            OS_Terminate();
        }
        rec->mUnk_f8[id] = v + 0x100;
        v = stage->mUnk_1ec / STAGE_UNIT;
        if (v <= -0x100 || v >= 0x100) {
            OS_Terminate();
        }
        rec->mUnk_194[id] = v + 0x100;
        rec->mUnk_230[id] = stage->mInfo.mUnk_12;
        rec->mUnk_2cc[id] = stage->mInfo.mUnk_04;
        rec->mUnk_368[id] = stage->mInfo.mUnk_11;
        rec->mUnk_404[id] = func_020144a4(stage->mInfo.mUnk_1c[0]) + 1;
        rec->mUnk_4a0[id] = func_020144a4(stage->mInfo.mUnk_1c[1]) + 1;
        rec->mUnk_53c[id] = func_020144a4(stage->mInfo.mUnk_1c[2]) + 1;
        rec->mUnk_5d8[id] = func_020144a4(stage->mInfo.mUnk_1c[3]) + 1;
        rec->mUnk_674[id] = func_020144a4(stage->mInfo.mUnk_1c[4]) + 1;
    }
    func_02020258(slot, rec, SAVE_BLOCK_ID);
    func_0201ffcc(&rec);
}

void func_02010c94(void) {
    SaveRecord *buf = func_0201ffbc();
    int i;
    int usedDefaults = 0;
    for (i = 0; (u32) i < SAVE_SLOT_COUNT; i++) {
        func_02020140(i, buf, SAVE_BLOCK_ID);
        if (buf->mUnk_00[0] == 1) {
            data_020b1078.mLanguage = buf->mUnk_00[1];
            data_020b1078.mUnk_10 = buf->mUnk_00[2];
            data_020b1078.mUnk_14 = buf->mUnk_00[3];
            break;
        }
    }
    if (i == SAVE_SLOT_COUNT) {
        func_02010d34();
        usedDefaults = 1;
    }
    func_0201ffcc(&buf);
    if (usedDefaults == 1) {
        data_020c1bbc.mUnk_1c(6);
        func_02010cfc();
    }
}

void func_02010cfc(void) {
    SaveRecord *buf = func_0201ffbc();
    int i;
    for (i = 0; (u32) i < SAVE_SLOT_COUNT; i++) {
        func_02020140(i, buf, SAVE_BLOCK_ID);
        func_02010d4c(buf);
        func_02020258(i, buf, SAVE_BLOCK_ID);
    }
    func_0201ffcc(&buf);
}

void func_02010d34(void) {
    data_020b1078.mLanguage = func_0200f9b8();
    data_020b1078.mUnk_10 = 1;
    data_020b1078.mUnk_14 = 3;
}

void func_02010d4c(SaveRecord *rec) {
    rec->mUnk_00[0] = 1;
    rec->mUnk_00[1] = data_020b1078.mLanguage;
    rec->mUnk_00[2] = data_020b1078.mUnk_10;
    rec->mUnk_00[3] = data_020b1078.mUnk_14;
}
