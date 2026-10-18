#include <Main/Save/save_blocks.h>
#include <Main/Input/mic_input.h>
#include <Main/System/random_angle.h>
#include <Main/System/screen_fade.h>
#include <Main/System/game_math.h>
#include <Main/System/bg_image.h>
#include <Main/Scene/stage_select.h>
#include <Main/System/state_flow.h>
#include <Main/System/prompt_screen.h>
#include <Main/System/sound_control.h>
#include <Main/Graphics/sprite_slots.h>
#include <Main/Scene/slide_transition.h>
#include <Main/Unknown/game_util.h>
#include <Main/Graphics/text_draw.h>
#include <Main/Graphics/model.h>
#include <Main/Graphics/texture_blocks.h>
#include <Main/Sound/wave_player.h>
#include <Main/Graphics/script_data.h>
#include <Main/Input/touch_buttons.h>
#include <Main/System/system_init.h>
#include <Main/Graphics/gx_pool.h>
#include <nitro/os.h>
#include <stdio.h>

// The record buffer func_0201ffbc returns.
typedef struct Unk_RecordBuf {
    u8 mUnk_00[0x10];
    s32 mUnk_10;
    s32 mUnk_14;
    s32 mUnk_18;
} Unk_RecordBuf;

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
                    for (i = 0; i < 1; i++) {
                        x = data_020ae504.mUnk_1bc[i + 2] - SLIDE_STEP;
                        if (x < 0) {
                            x = 0;
                        }
                        data_020ae504.mUnk_1bc[i + 2] = x;
                    }
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
                    for (i = 0; i < 1; i++) {
                        x = data_020ae504.mUnk_1bc[i + 2] + SLIDE_STEP;
                        if (x >= SLIDE_FAR) {
                            x = SLIDE_FAR;
                        }
                        data_020ae504.mUnk_1bc[i + 2] = x;
                    }
                    if (data_020ae504.mUnk_1bc[0] == -SLIDE_NEAR && data_020ae504.mUnk_1bc[1] == SLIDE_NEAR) {
                        for (i = 0; i < 1; i++) {
                            if (data_020ae504.mUnk_1bc[i + 2] < SLIDE_FAR) {
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

typedef struct Unk_020ae458 {
    u32 mUnk_00;
    u32 mUnk_04;
    u32 mUnk_08;
} Unk_020ae458;

static Unk_020ae458 data_020ae458;

void func_0200e3ec(void) {
    func_0202f2e8(&data_020ae464);
    data_020ae458.mUnk_00 = data_020b1084.mUnk_04;
    data_020ae458.mUnk_04 = data_020b1084.mUnk_08;
    func_02020ff4(0x400, data_020b1084.mUnk_04);
    func_02021074(0x82e, 0);
}

void func_0200e424(int a) {
    func_02021458();
    if (a != 0) {
        if (data_020ae458.mUnk_04 != data_020b1084.mUnk_08 || data_020ae458.mUnk_00 != data_020b1084.mUnk_04) {
            data_020b1084.mUnk_04 = data_020ae458.mUnk_00;
            data_020b1084.mUnk_08 = data_020ae458.mUnk_04;
            func_02014430(1);
        }
    }
    func_0202f290(&data_020ae464, "BAK_strm_Title.raw", 0x5654);
}

void func_0200e474(void) {
    data_020ae504.mUnk_38 = -1;
}

void func_0200e484(void) {
    func_0200c5d8(0, 0, 0x25e, 0x100, 0x10, 1, 2);
    func_02024208();
    func_02027230(&data_020ae504.mUnk_3c, 0x25d, 0x25c, 2, 0);
}

void func_0200e4c4(void) {
    Unk_020ae504 *s = &data_020ae504;
    int i;
    func_0200e484();
    s->mUnk_24 = data_020ae504.mUnk_38 < 0 ? 1 : s->mUnk_38;
    s->mUnk_28 = -1;
    s->mUnk_2c = 0;
    s->mUnk_30 = 0;
    s->mUnk_34 = (data_020b6ee0.mUnk_a91a != 0 && s->mUnk_38 < 0) ? 1 : 0x4b0;
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
    data_020b6ee0.mUnk_a91a = 0;
    data_020b6ee0.mUnk_a91b = 0;
    func_020104c0(0, 0x80);
    func_0202f290(&data_020ae464, "BAK_strm_Title.raw", 0x5654);
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

static const s16 data_0204e1e4[1] = {0};
static const u32 data_0204e1e8[1] = {0xdce};
static const u32 data_0204e1ec[1] = {0xdd1};
static const s16 data_0204e1f0[4] = {140, 70, -22, 0};

typedef struct Unk_0204e1f8 {
    u32 mUnk_00;
    u32 mUnk_04;
} Unk_0204e1f8;

static const Unk_0204e1f8 data_0204e1f8[2] = {{0, 3}, {0, 7}};

void func_0200e5b0(void) {
    static s32 data_020ae454;
    static s32 data_020ae44c;
    static s32 data_020ae450;
    Unk_020ae504 *s = &data_020ae504;
    DrawParams params;
    char text[0x40];
    char num[8];
    int flag;
    int i;
    u8 pct;
    s16 idx;
    const s16 *pos;
    const u32 *id;
    u32 v;
    u32 *level;
    const Unk_0204e1f8 *range;

    if (s->mUnk_34 == 0) {
        func_0201f880(3);
        func_02022370();
        func_0202c1c4();
        func_0201dde0(0xdaa, 1);
        func_0202f410(&data_020ae464);
        func_0200dd5c("BAK_Intro.mods", 2);
        s->mUnk_34 = 0x4b0;
        func_0201f450(1, 0, 0x100, 0x100);
        func_0200e484();
        func_0202f470(&data_020ae464);
        return;
    }
    if (s->mUnk_24 != 6 && s->mUnk_28 != 6) {
        func_0202f31c(&data_020ae464);
    }
    if (s->mUnk_24 == 1 && s->mUnk_28 < 0) {
        if (data_020c18bc.mUnk_10 != 0 || data_020c18bc.mUnk_2ac_1) {
            s->mUnk_34 = 0x4b0;
        } else {
            s->mUnk_34--;
        }
    }
    func_020242a8(&params, 0xff, 0, 0, 0x1000, 0x8000, -1, 0x100);
    params.mUnk_43_3 = 1;
    func_020262a0(&s->mUnk_3c);
    func_02024b5c(s->mUnk_3c.mUnk_170, &params, &s->mUnk_3c, 0);
    if (s->mUnk_1e4 == 0) {
        if (s->mUnk_1e0 >= 0) {
            s32 x = s->mUnk_1e0 - 0x1900;
            if (x < 0) {
                x = 0;
            }
            s->mUnk_1e0 = x;
            func_0202cf08(0xdfa, 0x59 - (s->mUnk_1e0 >> 8), 0x35, 0x96, 2);
            func_0202cf08(0xdfb, (s->mUnk_1e0 >> 8) + 0x59, 0x35, 0x8c, 2);
            if (s->mUnk_1e0 == 0) {
                func_0202f110(0xe7, 100, 0, 0x40, 0xffff, 0);
                s->mUnk_1e0 = -1;
                data_020ae454 = 12;
                return;
            }
        } else {
            int sign;
            s32 d;
            sign = (func_0201fd68() & 1) ? -1 : 1;
            d = func_0201fd84(0, 2);
            d *= sign;
            data_020ae450 = d;
            sign = (func_0201fd68() & 1) ? -1 : 1;
            d = func_0201fd84(0, 2);
            d *= sign;
            data_020ae44c = d;
            func_0202cf08(0xdfa, data_020ae450 + 0x59, d + 0x35, 0x96, 2);
            func_0202cf08(0xdfb, data_020ae450 + 0x59, data_020ae44c + 0x35, 0x8c, 2);
            if (--data_020ae454 == 0) {
                s->mUnk_1e4 = 1;
                return;
            }
        }
        return;
    }
    func_0202cf08(0xdfa, 0x59, 0x35, 0x96, 2);
    func_0202cf08(0xdfb, 0x59, 0x35, 0x8c, 2);
    if (func_0200de68() != 0 && s->mUnk_28 >= 0) {
        s->mUnk_24 = s->mUnk_28;
        s->mUnk_28 = -1;
        func_0200dd90();
    }
    flag = data_020ae504.mUnk_1b8 == 1 ? 1 : 0;
    switch (s->mUnk_24) {
        case 1:
            func_0200bc50((data_020ae504.mUnk_1bc[0] >> 8) + 0x80, 0x3e, 0, 0, 0x8c, 0x701, 0xdda, func_0200f9f0(0xc4),
                          data_0201a61c, 2, 0, 0x167, flag, 0, 0, 0);
            if (s->mUnk_30 != 0) {
                func_0200bc50((data_020ae504.mUnk_1bc[1] >> 8) + 0x80, 0x66, 0, 0, 0x8c, 0x700, 0xdda,
                              func_0200f9f0(0xc5), data_0201a61c, 2, 0, 0x167, flag, 0, 0, 0);
            } else {
                func_0200bc50((data_020ae504.mUnk_1bc[1] >> 8) + 0x80, 0x66, 0, 0, 0x8c, 0x700, 0xddc,
                              func_0200f9f0(0xc5), data_0201a626, 2, 0, 0x167, 0, 0, 0, 0);
            }
            func_0200bc50((data_020ae504.mUnk_1bc[2] >> 8) + 0x80, 0x8e, 0, 0, 0x8c, 0x706, 0xdda, func_0200f9f0(0xc6),
                          data_0201a61c, 2, 0, 0x167, flag, 0, 0, 0);
            if (func_0200bd80(0x20000000, 0x700)) {
                func_0200e3d0(2, -1);
                s->mUnk_2c = 1;
            }
            if (func_0200bd80(0x20000000, 0x701)) {
                func_0200e3d0(2, -1);
                s->mUnk_2c = 0;
            }
            if (func_0200bd80(0x20000000, 0x706)) {
                func_0200e3d0(5, -1);
                return;
            }
            break;
        case 2:
            func_0202cf08(0xe22, 0x2f, (data_020ae504.mUnk_1bc[3] >> 8) + 0x11, 0x8c, 1);
            if (s->mUnk_2c == 0) {
                func_020215fc(0x2f, (data_020ae504.mUnk_1bc[3] >> 8) + 0x11, data_0201a61c, func_0200f9f0(0xc4), 0, 200,
                              16, 1);
            } else {
                func_020215fc(0x2f, (data_020ae504.mUnk_1bc[3] >> 8) + 0x11, data_0201a61c, func_0200f9f0(0xc5), 0, 200,
                              16, 1);
            }
            for (i = 0; i < 3; i++) {
                if (s->mUnk_00[i].mUnk_00 == 1) {
                    pct = s->mUnk_00[i].mUnk_08 != 0 ? 100 : s->mUnk_00[i].mUnk_04 * 100 / 25;
                    func_0200bc50((data_020ae504.mUnk_1bc[i] >> 8) + 0x39, i * 0x2c + 0x36, 0, 0, 0x8c, 0x704 + i,
                                  0xdf6, 0, 0, 2, 0, 0x167, flag, 0, 0, 0);
                    sprintf(text, "%s %d", func_0200f9f0(0xcd), i + 1);
                    func_020215fc((data_020ae504.mUnk_1bc[i] >> 8) + 0x39, i * 0x2c + 0x36, data_0201a61c, text, 0,
                                  0xf0, 0x10, 1);
                    if (pct != 0) {
                        u8 n = func_0201fbe4(pct, 0, 100, 0, 0x18);
                        func_0202cf08(0xddd + n, (data_020ae504.mUnk_1bc[i] >> 8) + 0xa0, i * 0x2c + 0x36, 0x82, 1);
                    }
                    sprintf(text, "%d%", pct);
                    func_020215fc((data_020ae504.mUnk_1bc[i] >> 8) + 0xa0, i * 0x2c + 0x36, data_0201a662, text, 2,
                                  0xf0, 0x10, 1);
                } else if (s->mUnk_2c == 0) {
                    func_0200bc50((data_020ae504.mUnk_1bc[i] >> 8) + 0x39, i * 0x2c + 0x36, 0, 0, 0x8c, 0x704 + i,
                                  0xdf8, 0, 0, 2, 0, 0, flag, 0, 0, 0);
                    func_020215fc((data_020ae504.mUnk_1bc[i] >> 8) + 0x39, i * 0x2c + 0x36, data_0201a61c,
                                  func_0200f9f0(0xcc), 0, 0xf0, 0x10, 1);
                }
            }
            func_0200bb14(0, (data_020ae504.mUnk_1bc[4] >> 8) + 0xa1, 0x102, 0xe, 0, 0, 0x5a, flag);
            if (func_0200bdac(0x20000000, 0x704, 0x706)) {
                data_020b1084.mUnk_0c = (u16) data_020c2560.mUnk_100.mUnk_04 - 0x704;
                if (s->mUnk_2c == 0) {
                    if (s->mUnk_00[data_020b1084.mUnk_0c].mUnk_00 == 1) {
                        func_0200e3d0(3, -1);
                    } else {
                        func_020143f4();
                        func_0200fa80(10);
                        func_0200fab0();
                    }
                } else {
                    func_0201440c();
                    func_0200fa80(10);
                    func_0200fab0();
                }
            }
            if (func_0200bd80(0x20000000, 0x102)) {
                func_0200e3d0(1, 1);
                return;
            }
            break;
        case 3:
            func_0202cf08(0xe21, (data_020ae504.mUnk_1bc[0] >> 8) + 0x7f, 0x50, 0x8c, 1);
            func_020215fc((data_020ae504.mUnk_1bc[0] >> 8) + 0x7f, 0x50, data_0201a61c, func_0200f9f0(0xcf), 2, 0xc0,
                          0xc, 1);
            func_0200bb14(0xff, (data_020ae504.mUnk_1bc[4] >> 8) + 0xa1, 0x101, 0x15, 0, 0, 0x17d, flag);
            func_0200bb14(0, (data_020ae504.mUnk_1bc[4] >> 8) + 0xa1, 0x102, 0xe, 0, 0, 0x5a, flag);
            if (func_0200bd80(0x20000000, 0x101)) {
                func_020143f4();
                func_02014430(0);
                func_0200fa80(10);
                func_0200fab0();
            }
            if (func_0200bd80(0x20000000, 0x102)) {
                func_0200e3d0(2, 1);
                s->mUnk_2c = 0;
                return;
            }
            break;
        case 5:
            func_0202cf08(0xe22, 0x2f, (data_020ae504.mUnk_1bc[3] >> 8) + 0x11, 0x8c, 1);
            func_020215fc(0x2f, (data_020ae504.mUnk_1bc[3] >> 8) + 0x11, data_0201a61c, func_0200f9f0(0xc6), 0, 200, 16,
                          1);
            func_0200bc50((data_020ae504.mUnk_1bc[1] >> 8) + 0x80, 0x48, 0, 0, 0x8c, 0x703, 0xdda, func_0200f9f0(0xc8),
                          data_0201a61c, 2, 0, 0x167, flag, 0, 0, 0);
            func_0200bc50((data_020ae504.mUnk_1bc[2] >> 8) + 0x80, 0x70, 0, 0, 0x8c, 0x708, 0xdda, func_0200f9f0(0xc9),
                          data_0201a61c, 2, 0, 0x167, flag, 0, 0, 0);
            func_0200bb14(0, (data_020ae504.mUnk_1bc[4] >> 8) + 0xa1, 0x102, 0xe, 0, 0, 0x5a, flag);
            if (func_0200bd80(0x20000000, 0x702)) {
                data_020ae504.mUnk_1b4 = data_020b1084.mLanguage;
                func_0200e3d0(4, -1);
            }
            if (func_0200bd80(0x20000000, 0x703)) {
                s->mUnk_38 = s->mUnk_24;
                func_0200fa80(0x18);
                func_0200fab0();
            }
            if (func_0200bd80(0x20000000, 0x708)) {
                func_0200e3d0(6, -1);
                func_0200e3ec();
            }
            if (func_0200bd80(0x20000000, 0x102)) {
                func_0200e3d0(1, 1);
                return;
            }
            break;
        case 4:
            func_0202cf08(0xe22, 0x2f, (data_020ae504.mUnk_1bc[0] >> 8) + 0x11, 0x8c, 1);
            func_020215fc(0x2f, (data_020ae504.mUnk_1bc[0] >> 8) + 0x11, data_0201a61c, func_0200f9f0(0xc7), 0, 200, 16,
                          1);
            pos = data_0204e1f0;
            id = data_0204e1ec;
            for (i = 0; i < 1; i++) {
                idx = data_0204e1e4[i] + 2;
                func_0200bb14((data_020ae504.mUnk_1bc[idx] >> 8) + data_0204e1f0[0], data_0204e1f0[1], 0x705 + i,
                              data_0204e1e8[i], 0, 0, 0x167, 1);
                if (data_020ae504.mUnk_1b4 == i) {
                    func_0202cf08(0xdd0, pos[0] + (data_020ae504.mUnk_1bc[idx] >> 8), pos[1], 0xfa, 1);
                }
                func_0202cf08(*id, (data_020ae504.mUnk_1bc[idx] >> 8) + (pos[0] + pos[2]), pos[1], 0xff, 1);
            }
            func_0200bb14(0xff, (data_020ae504.mUnk_1bc[1] >> 8) + 0xa1, 0x101, 0x15, 0, 0, 0x17d, flag);
            func_0200bb14(0, (data_020ae504.mUnk_1bc[1] >> 8) + 0xa1, 0x102, 0xe, 0, 0, 0x5a, flag);
            for (i = 0; i < 1; i++) {
                if (func_0200bd80(0x80000000, 0x705 + i)) {
                    data_020ae504.mUnk_1b4 = i;
                    break;
                }
            }
            if (func_0200bd80(0x20000000, 0x102)) {
                func_0200e3d0(5, 1);
            }
            if (func_0200bd80(0x20000000, 0x101)) {
                if (data_020ae504.mUnk_1b4 != data_020b1084.mLanguage) {
                    data_020b1084.mLanguage = data_020ae504.mUnk_1b4;
                    func_0202f2e8(&data_020ae464);
                    func_02014430(1);
                    func_0202f290(&data_020ae464, "BAK_strm_Title.raw", 0x5654);
                }
                func_0200e3d0(5, 1);
                return;
            }
            break;
        case 6:
            func_0202cf08(0xe22, 0x2f, (data_020ae504.mUnk_1bc[3] >> 8) + 0x11, 0x8c, 1);
            func_020215fc(0x2f, (data_020ae504.mUnk_1bc[3] >> 8) + 0x11, data_0201a61c, func_0200f9f0(0xc9), 0, 200, 16,
                          1);
            func_0202cf08(0xe21, (data_020ae504.mUnk_1bc[0] >> 8) + 0x7f, 0x28, 0x8c, 1);
            func_0202cf08(0xe00, (data_020ae504.mUnk_1bc[1] >> 8) + 0x7f, 0x8f, 0x8c, 1);
            func_020215fc((data_020ae504.mUnk_1bc[1] >> 8) + 0x7f, 0x9e, data_0201a61c, func_0200f9f0(0xd2), 2, 0xc0,
                          0xc, 1);
            level = &data_020ae458.mUnk_04;
            range = &data_0204e1f8[1];
            sprintf(num, "%d", *level + 1);
            func_020215fc((data_020ae504.mUnk_1bc[1] >> 8) + 0x82, 0xb4, data_0201a676, num, 0, 0xf0, 1, 1);
            if (*level > range->mUnk_00) {
                func_0200bb14((data_020ae504.mUnk_1bc[1] >> 8) + 0x50, 0x85, 0x70b, 0xdfc, 0, 0, 0x17d,
                              data_020c18bc.mUnk_492 == 0 ? 1 : 0);
            }
            if (*level < range->mUnk_04) {
                func_0200bb14((data_020ae504.mUnk_1bc[1] >> 8) + 0xaf, 0x85, 0x70c, 0xdfe, 0, 0, 0x17d,
                              data_020c18bc.mUnk_492 == 0 ? 1 : 0);
            }
            if (data_020c18bc.mUnk_492 == 0) {
                func_020215fc((data_020ae504.mUnk_1bc[0] >> 8) + 0x7f, 0x28, data_0201a61c, func_0200f9f0(0xd1), 2,
                              0xc0, 0xc, 1);
                if (s->mUnk_28 < 0) {
                    if (!(data_020c18bc.mUnk_46c & 0x800)) {
                        OS_Terminate();
                    }
                    v = func_0201fb74(func_020213e8((data_0204e1f8[1].mUnk_04 - data_020ae458.mUnk_04 + 1) * 25), 7, 0,
                                      0x1f);
                } else {
                    v = 0;
                }
            } else {
                func_020215fc((data_020ae504.mUnk_1bc[0] >> 8) + 0x7f, 0x28, data_0201a61c, func_0200f9f0(0xd3), 2,
                              0xc0, 0xc, 1);
                data_020ae458.mUnk_08 = 0;
                v = 0;
            }
            if (v > data_020ae458.mUnk_08) {
                data_020ae458.mUnk_08 = v;
            }
            if (data_020ae458.mUnk_08 != 0) {
                func_0202cf08(data_020ae458.mUnk_08 + 0xe01, (data_020ae504.mUnk_1bc[1] >> 8) + 0x7f, 0x8f, 0x8c, 1);
                data_020ae458.mUnk_08--;
            }
            func_0200bb14(0xff, (data_020ae504.mUnk_1bc[4] >> 8) + 0xa1, 0x101, 0x15, 0, 0, 0x17d, flag);
            func_0200bb14(0, (data_020ae504.mUnk_1bc[4] >> 8) + 0xa1, 0x102, 0xe, 0, 0, 0x5a, flag);
            if (func_0200bd80(0x20000000, 0x709)) {
                func_02021054(--data_020ae458.mUnk_00);
            } else if (func_0200bd80(0x20000000, 0x70a)) {
                func_02021054(++data_020ae458.mUnk_00);
            } else if (func_0200bd80(0x20000000, 0x70b)) {
                data_020ae458.mUnk_04--;
            } else if (func_0200bd80(0x20000000, 0x70c)) {
                data_020ae458.mUnk_04++;
            }
            if (func_0200bd80(0x20000000, 0x102)) {
                func_0200e3d0(5, 1);
                func_0200e424(0);
            }
            if (func_0200bd80(0x20000000, 0x101)) {
                func_0200e3d0(5, 1);
                func_0200e424(1);
            }
            break;
    }
}

void func_0200f360(void) {
    func_0202f2e8(&data_020ae464);
    func_020102f8();
}
