#include <nitro/os.h>
#include <stdio.h>
#include <Main/Unknown/functions.h>
#include <Main/Unknown/game_util.h>
#include <Main/Unknown/mode_select.h>
#include <Main/Unknown/state_flow.h>
#include <Main/Unknown/globals.h>
#include <Main/Unknown/stage_data.h>
#include <Main/Unknown/stage_info_panel.h>

void func_02011518(int stage, int mode) {
    u16 current;
    int id;

    if (mode == 1) {
        current = data_020c1d3c.mUnk_1c;
    } else {
        current = data_020c1d3c.mUnk_1e;
    }
    if (!(stage >= -1 && stage < STAGE_MAX)) {
        OS_Terminate();
    }
    data_020c0fe0.mUnk_3c = 0;
    if (func_0200fb00() == 10 && (stage == -1 || data_020c0fe0.mUnk_28 != -1)) {
        id = data_020c0fe0.mUnk_20 + 0x1e4;
        func_0200bdcc(2, 8);
    } else {
        id = data_020b1990[stage].mInfo.mUnk_13 + 0xdb3;
    }
    if ((u16) id != current) {
        func_0201f880(mode);
        func_0201dde0(id, mode);
    }
}

void func_020115ac(int frames, char *buf) {
    int t;

    if (frames < 0) {
        frames = 0;
    }
    t = frames * 1000 / 60;
    sprintf(buf, "%d%d:%d%d", t / 10000 % 10, t / 1000 % 10, t / 100 % 10, t / 10 % 10);
}

void func_02011624(int frames, char *buf) {
    int t;

    t = (frames >= 0 ? frames : -frames) * 1000 / 60;
    if (t % 1000 != 0) {
        t += 1000;
    }
    if (frames > 0) {
        sprintf(buf, "+%d", t / 1000);
    } else {
        sprintf(buf, "-%d", t / 1000);
    }
}

void func_0201168c(int level, u32 a, int b, int c, int d) {
    char time[12];
    char text[32];

    if (level == -1) {
        level = 0;
    }
    if (!(level >= 0 && level < STAGE_MAX)) {
        OS_Terminate();
    }
    func_0202cf08(0xf38, 0xa3, 0x8b, 0xc8, d);
    func_0202cf08(0xf39, 0x53, 0x8b, 0xc8, d);
    func_0202cf08(0xf48, 0x15, 0x8e, 0xc8, d);
    func_020215fc(0x23, 0x95, data_0201a612, func_0200f9f0(0x2f), 0, 0xc8, 0x10, d);
    sprintf(text, "%003d", a);
    func_020215fc(0x30, 0xb1, data_0201a694, text, 2, 0x82, 0x10, d);
    func_020215fc(0x5f, 0x95, data_0201a612, func_0200f9f0(0x30), 0, 0xc8, 0x10, d);
    sprintf(text, "%003d", b);
    func_020215fc(0x7b, 0xb0, data_0201a68a, text, 2, 0x82, 0x10, d);
    func_0202cf08(0xf49, 0xb2, 0x8e, 0xc8, d);
    func_020215fc(0xc1, 0x95, data_0201a612, func_0200f9f0(0x31), 0, 0xc8, 0x10, d);
    if (func_02005a00(c) == 0) {
        func_020115ac(c, time);
        func_020215fc(0xd2, 0xb0, data_0201a694, time, 2, 0xc8, 0x10, d);
    }
}

int func_020117e0(int percent) {
    int low = percent < 100 ? percent : 100;

    if (low < 0) {
        percent = 0;
    } else if (percent >= 100) {
        percent = 100;
    }
    return func_0201fbe4(percent, 0, 100, 0, 100) + 14;
}

void func_0201180c(int stage, int mode) {
    StageRec *rec;

    if (stage >= 0) {
        if (!(stage < STAGE_MAX)) {
            OS_Terminate();
        }
        func_0202cf08(0xf59, 8, 0x54, 0xa0, mode);
        rec = &data_020b1990[stage];
        func_0202cf08(func_0201fbe4(rec->mInfo.mUnk_12, 0, 100, 50, 0) + 0xf5a, 7, 0x58, 0x9f, mode);
        func_020215fc(0x1c, 0x54, data_0201a612, func_0200f9f0(10), 0, 0xf0, 0x10, mode);
    }
}

void func_02011890(int level, int mode) {
    StageRec *rec;
    int total;
    char label[8];
    char number[8];

    if (level >= 0) {
        rec = &data_020b1990[level];
        total = func_02021e44(0x1e8, 0x13);
        if (!(level >= 0 && level < STAGE_MAX)) {
            OS_Terminate();
        }
        func_0201fb24(&data_020c17a0.mUnk_2c, 4);
        func_0201fbe4(data_020c17a0.mUnk_2c, 0, 0x4000, 0x100, 0);
        func_0201180c(level, mode);
        func_0202cf08(0xf8d, 0, 0x79, 0xfa, mode);
        func_020215fc(0x25, 0x83, data_0201a61c, func_0200f9f0(0xd), 2, 0xfa, 0, mode);
        func_0202cf08(0xf4a, 0x27, 0xa3, 0xfa, mode);
        func_0202cf08(0xf4a + rec->mInfo.mUnk_11, 0x27, 0xa3, 0xfa, mode);
        func_0202cf08(0xf38, 0x47, 0x8b, 0x94, mode);
        func_020215fc(0x85, 0x83, data_0201a61c, func_0200f9f0(0x18), 2, 0xfa, 0, mode);
        func_0202cf08(0xf42 + rec->mInfo.mUnk_10, 0x74, 0xaa, 0xfa, mode);
        *func_0201fc08(rec->mInfo.mUnk_04, label) = 0;
        func_020215fc(0x74, 0xaa, data_0201a694, label, 0, 0xfa, 10, mode);
        if (func_020150c0(&rec->mInfo)) {
            func_0202cf08(0xf57 + (data_020c1cbc.mUnk_5c % 60 <= 3 ? 1 : 0), 0x82, 0xbb, 0x8c, mode);
        }
        sprintf(number, "+%d", rec->mInfo.mUnk_04 * total / 100);
        func_020215fc(0xbe, 0xa4, data_0201a68a, number, 0, 0xfa, 10, mode);
        func_020215fc(0xe8, 0xb3, data_0201a61c, func_0200f9f0(0xf), 1, 0xfa, 0, mode);
        func_0202cf08(func_02021e44(0x10d5, rec->mInfo.mUnk_10 * 2 + 3) + 0xf32, 0xf2, 0xaf, 0xf8, mode);
    }
    data_020c0fe0.mUnk_30 = 0x100;
}

void func_02011a94(int stage, int mode) {
    if (stage >= 0) {
        if (!(stage >= 0 && stage < STAGE_MAX)) {
            OS_Terminate();
        }
        func_0201fb24(&data_020c17a0.mUnk_2c, 4);
        func_0201fbe4(data_020c17a0.mUnk_2c, 0, 0x4000, 0x100, 0);
        if (data_020c0fe0.mUnk_10 < 11) {
            data_020c0fe0.mUnk_10 += data_020c1cbc.mUnk_5c & 1;
        }
        func_0201180c(stage, mode);
        func_0202cf08(data_020c0fe0.mUnk_10 + 0x19, 0x1f, 0x83, 0xf8, mode);
        if (data_020c0fe0.mUnk_10 == 11) {
            func_020215fc(0x1f, 0x83, data_0201a61c, func_0200f9f0(0x10), 0, 0xdc, 0xd, mode);
            func_020215fc(0x15, 0x97, data_0201a612, func_0200f9f0(data_020b7ee0.mUnk_0c + 0x229), 0, 0xdc, 0xd, mode);
        }
    }
    data_020c0fe0.mUnk_30 = 0x100;
}

void func_02011b70(int stage, int mode) {
    if (stage >= 0) {
        if (!(stage >= 0 && stage < STAGE_MAX)) {
            OS_Terminate();
        }
        func_0201180c(stage, mode);
        func_020215fc(0x12, 0x96, data_0201a612, func_0200f9f0(data_020c0fe0.mUnk_3d + 0x5f), 0, 0xdc, 0x10, mode);
    }
    data_020c0fe0.mUnk_30 = 0x100;
}

void func_02011bcc(int a, int b, int c) {
    Unk_020c1454 *p = &data_020c1454;

    if (a == -1) {
        data_020c0fe0.mUnk_2c = -1;
        data_020c0fe0.mUnk_28 = -1;
        data_020c0fe0.mUnk_24 = 0;
        data_020c0fe0.mUnk_20 = 0;
        data_020c0fe0.mUnk_38 = 0;
        data_020c0fe0.mUnk_34 = 0;
    } else if (a == -2) {
        data_020c0fe0.mUnk_28 = -1;
        data_020c0fe0.mUnk_20 = 0;
        data_020c0fe0.mUnk_34 = 0;
    } else if (a == -3) {
        data_020c0fe0.mUnk_28 = data_020c0fe0.mUnk_2c;
        data_020c0fe0.mUnk_20 = data_020c0fe0.mUnk_24;
        data_020c0fe0.mUnk_34 = data_020c0fe0.mUnk_38;
    } else {
        data_020c0fe0.mUnk_2c = a;
        data_020c0fe0.mUnk_28 = a;
        data_020c0fe0.mUnk_24 = b;
        data_020c0fe0.mUnk_20 = b;
        data_020c0fe0.mUnk_38 = c;
        data_020c0fe0.mUnk_34 = c;
    }
    data_020c0fe0.mUnk_30 = 0x100;
    data_020c0fe0.mUnk_3c = 1;
    data_020c0fe0.mUnk_4b = 1;
    p->mUnk_40 = 0x28;
    if (data_020c0fe0.mUnk_20 < 0) {
        OS_Terminate();
    }
    if (data_020c0fe0.mUnk_28 < -1) {
        OS_Terminate();
    }
}
