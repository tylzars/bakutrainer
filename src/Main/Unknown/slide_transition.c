#include <Main/Unknown/functions.h>
#include <Main/Unknown/globals.h>
#include <Main/Unknown/sprite_slots.h>
#include <Main/Unknown/slide_transition.h>

#define SLIDE_STEP 0x1e00
#define SLIDE_STEP_SMALL 0x780
#define SLIDE_FAR 0x18000
#define SLIDE_MID 0x8000
#define SLIDE_NEAR 0x4000

int func_0200de68(void) {
    s32 x;
    int i;
    s32 sum;
    switch (data_020ae504.mUnk_1b8) {
        case 0:
            switch (data_020ae504.mUnk_24) {
                case 1:
                    if (data_020ae504.mUnk_1dc > 0) {
                        x = data_020ae504.mUnk_1bc[0] + SLIDE_STEP;
                        if (x >= 0) {
                            x = 0;
                        }
                        data_020ae504.mUnk_1bc[0] = x;
                        x = data_020ae504.mUnk_1bc[1] + SLIDE_STEP;
                        if (x >= 0) {
                            x = 0;
                        }
                        data_020ae504.mUnk_1bc[1] = x;
                        x = data_020ae504.mUnk_1bc[2] + SLIDE_STEP;
                        if (x >= 0) {
                            x = 0;
                        }
                        data_020ae504.mUnk_1bc[2] = x;
                    } else {
                        x = data_020ae504.mUnk_1bc[0] - SLIDE_STEP;
                        if (x < 0) {
                            x = 0;
                        }
                        data_020ae504.mUnk_1bc[0] = x;
                        x = data_020ae504.mUnk_1bc[1] - SLIDE_STEP;
                        if (x < 0) {
                            x = 0;
                        }
                        data_020ae504.mUnk_1bc[1] = x;
                        x = data_020ae504.mUnk_1bc[2] - SLIDE_STEP;
                        if (x < 0) {
                            x = 0;
                        }
                        data_020ae504.mUnk_1bc[2] = x;
                    }
                    if (data_020ae504.mUnk_1bc[0] == 0 && data_020ae504.mUnk_1bc[1] == 0 &&
                        data_020ae504.mUnk_1bc[2] == 0) {
                        data_020ae504.mUnk_1b8 = 1;
                    }
                    break;
                case 2:
                case 5:
                    if (data_020ae504.mUnk_1dc > 0) {
                        x = data_020ae504.mUnk_1bc[0] + SLIDE_STEP;
                        if (x >= 0) {
                            x = 0;
                        }
                        data_020ae504.mUnk_1bc[0] = x;
                        x = data_020ae504.mUnk_1bc[1] + SLIDE_STEP;
                        if (x >= 0) {
                            x = 0;
                        }
                        data_020ae504.mUnk_1bc[1] = x;
                        x = data_020ae504.mUnk_1bc[2] + SLIDE_STEP;
                        if (x >= 0) {
                            x = 0;
                        }
                        data_020ae504.mUnk_1bc[2] = x;
                    } else {
                        x = data_020ae504.mUnk_1bc[0] - SLIDE_STEP;
                        if (x < 0) {
                            x = 0;
                        }
                        data_020ae504.mUnk_1bc[0] = x;
                        x = data_020ae504.mUnk_1bc[1] - SLIDE_STEP;
                        if (x < 0) {
                            x = 0;
                        }
                        data_020ae504.mUnk_1bc[1] = x;
                        x = data_020ae504.mUnk_1bc[2] - SLIDE_STEP;
                        if (x < 0) {
                            x = 0;
                        }
                        data_020ae504.mUnk_1bc[2] = x;
                    }
                    x = data_020ae504.mUnk_1bc[3] + SLIDE_STEP_SMALL;
                    if (x >= 0) {
                        x = 0;
                    }
                    data_020ae504.mUnk_1bc[3] = x;
                    x = data_020ae504.mUnk_1bc[4] - SLIDE_STEP_SMALL;
                    if (x < 0) {
                        x = 0;
                    }
                    data_020ae504.mUnk_1bc[4] = x;
                    if (data_020ae504.mUnk_1bc[0] == 0 && data_020ae504.mUnk_1bc[1] == 0 &&
                        data_020ae504.mUnk_1bc[2] == 0 && data_020ae504.mUnk_1bc[3] == 0 && x == 0) {
                        data_020ae504.mUnk_1b8 = 1;
                    }
                    break;
                case 4:
                    x = data_020ae504.mUnk_1bc[0] + SLIDE_STEP_SMALL;
                    if (x >= 0) {
                        x = 0;
                    }
                    data_020ae504.mUnk_1bc[0] = x;
                    x = data_020ae504.mUnk_1bc[1] - SLIDE_STEP_SMALL;
                    if (x < 0) {
                        x = 0;
                    }
                    data_020ae504.mUnk_1bc[1] = x;
                    x = data_020ae504.mUnk_1bc[2] - SLIDE_STEP;
                    if (x < 0) {
                        x = 0;
                    }
                    data_020ae504.mUnk_1bc[2] = x;
                    sum = 0;
                    for (i = 0; i < 8; i++) {
                        sum += data_020ae504.mUnk_1bc[i];
                    }
                    if (sum == 0) {
                        data_020ae504.mUnk_1b8 = 1;
                    }
                    break;
                case 3:
                    x = data_020ae504.mUnk_1bc[0] - SLIDE_STEP;
                    if (x < 0) {
                        x = 0;
                    }
                    data_020ae504.mUnk_1bc[0] = x;
                    x = data_020ae504.mUnk_1bc[4] - SLIDE_STEP_SMALL;
                    if (x < 0) {
                        x = 0;
                    }
                    data_020ae504.mUnk_1bc[4] = x;
                    if (data_020ae504.mUnk_1bc[0] == 0 && x == 0) {
                        data_020ae504.mUnk_1b8 = 1;
                    }
                    break;
                case 6:
                    x = data_020ae504.mUnk_1bc[0] - SLIDE_STEP;
                    if (x < 0) {
                        x = 0;
                    }
                    data_020ae504.mUnk_1bc[0] = x;
                    x = data_020ae504.mUnk_1bc[1] - SLIDE_STEP;
                    if (x < 0) {
                        x = 0;
                    }
                    data_020ae504.mUnk_1bc[1] = x;
                    x = data_020ae504.mUnk_1bc[3] + SLIDE_STEP_SMALL;
                    if (x >= 0) {
                        x = 0;
                    }
                    data_020ae504.mUnk_1bc[3] = x;
                    x = data_020ae504.mUnk_1bc[4] - SLIDE_STEP_SMALL;
                    if (x < 0) {
                        x = 0;
                    }
                    data_020ae504.mUnk_1bc[4] = x;
                    if (data_020ae504.mUnk_1bc[0] == 0 && data_020ae504.mUnk_1bc[1] == 0 &&
                        data_020ae504.mUnk_1bc[3] == 0 && x == 0) {
                        data_020ae504.mUnk_1b8 = 1;
                    }
                    break;
            }
            break;
        case 2:
            switch (data_020ae504.mUnk_24) {
                case 1:
                    x = data_020ae504.mUnk_1bc[0] - SLIDE_STEP;
                    if (x < -SLIDE_FAR) {
                        x = -SLIDE_FAR;
                    }
                    data_020ae504.mUnk_1bc[0] = x;
                    if (x <= -SLIDE_MID) {
                        x = data_020ae504.mUnk_1bc[1] - SLIDE_STEP;
                        if (x < -SLIDE_FAR) {
                            x = -SLIDE_FAR;
                        }
                        data_020ae504.mUnk_1bc[1] = x;
                    }
                    if (data_020ae504.mUnk_1bc[1] <= -2 * SLIDE_MID) {
                        x = data_020ae504.mUnk_1bc[2] - SLIDE_STEP;
                        if (x < -SLIDE_FAR) {
                            x = -SLIDE_FAR;
                        }
                        data_020ae504.mUnk_1bc[2] = x;
                    }
                    if (data_020ae504.mUnk_1bc[0] == -SLIDE_FAR && data_020ae504.mUnk_1bc[1] == -SLIDE_FAR &&
                        data_020ae504.mUnk_1bc[2] == -SLIDE_FAR) {
                        return 1;
                    }
                    break;
                case 2:
                case 5:
                    if (data_020ae504.mUnk_1dc > 0) {
                        x = data_020ae504.mUnk_1bc[0] + SLIDE_STEP;
                        if (x >= SLIDE_FAR) {
                            x = SLIDE_FAR;
                        }
                        data_020ae504.mUnk_1bc[0] = x;
                        if (x >= 2 * SLIDE_NEAR) {
                            x = data_020ae504.mUnk_1bc[1] + SLIDE_STEP;
                            if (x >= SLIDE_FAR) {
                                x = SLIDE_FAR;
                            }
                            data_020ae504.mUnk_1bc[1] = x;
                        }
                        if (data_020ae504.mUnk_1bc[1] >= 4 * SLIDE_NEAR) {
                            x = data_020ae504.mUnk_1bc[2] + SLIDE_STEP;
                            if (x >= SLIDE_FAR) {
                                x = SLIDE_FAR;
                            }
                            data_020ae504.mUnk_1bc[2] = x;
                        }
                    } else {
                        x = data_020ae504.mUnk_1bc[0] - SLIDE_STEP;
                        if (x < -SLIDE_FAR) {
                            x = -SLIDE_FAR;
                        }
                        data_020ae504.mUnk_1bc[0] = x;
                        if (x <= -2 * SLIDE_NEAR) {
                            x = data_020ae504.mUnk_1bc[1] - SLIDE_STEP;
                            if (x < -SLIDE_FAR) {
                                x = -SLIDE_FAR;
                            }
                            data_020ae504.mUnk_1bc[1] = x;
                        }
                        if (data_020ae504.mUnk_1bc[1] <= -4 * SLIDE_NEAR) {
                            x = data_020ae504.mUnk_1bc[2] - SLIDE_STEP;
                            if (x < -SLIDE_FAR) {
                                x = -SLIDE_FAR;
                            }
                            data_020ae504.mUnk_1bc[2] = x;
                        }
                    }
                    x = data_020ae504.mUnk_1bc[3] - SLIDE_STEP_SMALL;
                    if (x < -SLIDE_NEAR) {
                        x = -SLIDE_NEAR;
                    }
                    data_020ae504.mUnk_1bc[3] = x;
                    x = data_020ae504.mUnk_1bc[4] + SLIDE_STEP_SMALL;
                    if (x >= SLIDE_NEAR) {
                        x = SLIDE_NEAR;
                    }
                    data_020ae504.mUnk_1bc[4] = x;
                    sum = data_020ae504.mUnk_1dc * SLIDE_FAR;
                    if (data_020ae504.mUnk_1bc[0] == sum && data_020ae504.mUnk_1bc[1] == sum &&
                        data_020ae504.mUnk_1bc[2] == sum && data_020ae504.mUnk_1bc[3] == -SLIDE_NEAR &&
                        x == SLIDE_NEAR) {
                        return 1;
                    }
                    break;
                case 4:
                    x = data_020ae504.mUnk_1bc[0] - SLIDE_STEP_SMALL;
                    if (x < -SLIDE_NEAR) {
                        x = -SLIDE_NEAR;
                    }
                    data_020ae504.mUnk_1bc[0] = x;
                    x = data_020ae504.mUnk_1bc[1] + SLIDE_STEP_SMALL;
                    if (x >= SLIDE_NEAR) {
                        x = SLIDE_NEAR;
                    }
                    data_020ae504.mUnk_1bc[1] = x;
                    x = data_020ae504.mUnk_1bc[2] + SLIDE_STEP;
                    if (x >= SLIDE_FAR) {
                        x = SLIDE_FAR;
                    }
                    data_020ae504.mUnk_1bc[2] = x;
                    if (data_020ae504.mUnk_1bc[0] == -SLIDE_NEAR && data_020ae504.mUnk_1bc[1] == SLIDE_NEAR) {
                        for (i = 2; i < 3; i++) {
                            if (data_020ae504.mUnk_1bc[i] < SLIDE_FAR) {
                                return 0;
                            }
                        }
                        return 1;
                    }
                    break;
                case 3:
                    x = data_020ae504.mUnk_1bc[0] + SLIDE_STEP;
                    if (x >= SLIDE_FAR) {
                        x = SLIDE_FAR;
                    }
                    data_020ae504.mUnk_1bc[0] = x;
                    x = data_020ae504.mUnk_1bc[4] + SLIDE_STEP_SMALL;
                    if (x >= SLIDE_NEAR) {
                        x = SLIDE_NEAR;
                    }
                    data_020ae504.mUnk_1bc[4] = x;
                    if (data_020ae504.mUnk_1bc[0] == SLIDE_FAR && x == SLIDE_NEAR) {
                        return 1;
                    }
                    break;
                case 6:
                    x = data_020ae504.mUnk_1bc[0] + SLIDE_STEP;
                    if (x >= SLIDE_FAR) {
                        x = SLIDE_FAR;
                    }
                    data_020ae504.mUnk_1bc[0] = x;
                    if (x >= 4 * SLIDE_NEAR) {
                        x = data_020ae504.mUnk_1bc[1] + SLIDE_STEP;
                        if (x >= SLIDE_FAR) {
                            x = SLIDE_FAR;
                        }
                        data_020ae504.mUnk_1bc[1] = x;
                    }
                    x = data_020ae504.mUnk_1bc[3] - SLIDE_STEP_SMALL;
                    if (x < -SLIDE_NEAR) {
                        x = -SLIDE_NEAR;
                    }
                    data_020ae504.mUnk_1bc[3] = x;
                    x = data_020ae504.mUnk_1bc[4] + SLIDE_STEP_SMALL;
                    if (x >= SLIDE_NEAR) {
                        x = SLIDE_NEAR;
                    }
                    data_020ae504.mUnk_1bc[4] = x;
                    if (data_020ae504.mUnk_1bc[0] == SLIDE_FAR && data_020ae504.mUnk_1bc[1] == SLIDE_FAR &&
                        data_020ae504.mUnk_1bc[3] == -SLIDE_NEAR && x == SLIDE_NEAR) {
                        return 1;
                    }
                    break;
            }
            break;
    }
    return 0;
}

void func_0200e3d0(int a, int b) {
    data_020ae504.mUnk_28 = a;
    data_020ae504.mUnk_1b8 = 2;
    data_020ae504.mUnk_1dc = b;
}

void func_0200e3ec(void) {
    func_0202f2e8(data_020ae464);
    data_020ae44c.mUnk_0c = data_020b1084.mUnk_04;
    data_020ae44c.mUnk_10 = data_020b1084.mUnk_08;
    func_02020ff4(0x400, data_020b1084.mUnk_04);
    func_02021074(0x82e, 0);
}

void func_0200e424(int a) {
    func_02021458();
    if (a != 0) {
        if (data_020ae44c.mUnk_10 != data_020b1084.mUnk_08 || data_020ae44c.mUnk_0c != data_020b1084.mUnk_04) {
            data_020b1084.mUnk_04 = data_020ae44c.mUnk_0c;
            data_020b1084.mUnk_08 = data_020ae44c.mUnk_10;
            func_02014430(1);
        }
    }
    func_0202f290(data_020ae464, "BAK_strm_Title.raw", 0x5654);
}

void func_0200e474(void) {
    data_020ae504.mUnk_38 = -1;
}

void func_0200e484(void) {
    func_0200c5d8(0, 0, 0x25e, 0x100, 0x10, 1, 2);
    func_02024208();
    func_02027230(data_020ae504.mUnk_3c, 0x25d, 0x25c, 2, 0);
}

void func_0200e4c4(void) {
    Unk_020ae504 *s = &data_020ae504;
    int i;
    func_0200e484();
    s->mUnk_24 = data_020ae504.mUnk_38 < 0 ? 1 : s->mUnk_38;
    s->mUnk_28 = -1;
    s->mUnk_2c = 0;
    s->mUnk_30 = 0;
    s->mUnk_34 = (data_020c17e0.mUnk_1a != 0 && s->mUnk_38 < 0) ? 1 : 0x4b0;
    s->mUnk_1dc = -1;
    func_0200dd90();
    if (s->mUnk_38 < 0) {
        s->mUnk_1e4 = 0;
        s->mUnk_1e0 = 0x10000;
    } else {
        s->mUnk_1e4 = 1;
        s->mUnk_1e0 = 0;
    }
    s->mUnk_38 = -1;
    data_020c17e0.mUnk_1a = 0;
    data_020c17e0.mUnk_1b = 0;
    func_020104c0(0, 0x80);
    func_0202f290(data_020ae464, "BAK_strm_Title.raw", 0x5654);
    for (i = 0; i < 3; i++) {
        Unk_RecordBuf *buf = func_0201ffbc();
        func_02020140(i, buf, 0xfba);
        s->mUnk_00[i].mUnk_00 = buf->mUnk_10;
        if (s->mUnk_00[i].mUnk_00 == 1) {
            s->mUnk_30 = 1;
            s->mUnk_00[i].mUnk_04 = buf->mUnk_14;
            s->mUnk_00[i].mUnk_08 = buf->mUnk_18;
        }
        func_0201ffcc(&buf);
    }
}
