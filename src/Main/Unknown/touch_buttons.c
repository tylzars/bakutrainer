#include <Main/Unknown/touch_buttons.h>
#include <Main/Unknown/functions.h>
#include <Main/Unknown/globals.h>
#include <Main/Unknown/random_angle.h>
#include <nitro/mi.h>
#include <nitro/os.h>

#define TOUCH_BUTTON_COUNT 0x80
#define TOUCH_SHAPE 0x8000
// Flag bits of a button id: a polygon hit area, and "insert before the existing buttons".
#define TOUCH_ADD_FRONT 0x10000
#define TOUCH_NONE 0xffff
#define TOUCH_DOWN 0x40000000
#define TOUCH_PRESSED 0x80000000
#define TOUCH_RELEASED 0x20000000
#define TOUCH_REPEAT 0x10000000
#define TOUCH_QUICK 0x08000000
#define TOUCH_QUICK_FRAMES 0xe
#define SYSTEM_TOUCH_LOCK 0x80000000
#define SYSTEM_TOUCH_QUIET 0x04000000

// Whether the display-swap bit of the power control register is set.
int func_02020878(void) {
    if (*(vu16 *) 0x04000304 & 0x8000) {
        return 1;
    }
    return 0;
}

int func_02020890(void) {
    if (data_020c18bc.mUnk_490[0] == 3 && data_020c18bc.mUnk_490[1] == 3 &&
        ((data_020c18bc.mUnk_12 & 1) || data_020c18bc.mUnk_2ac_0 == 1)) {
        data_020c18bc.mUnk_12 &= ~1;
        data_020c18bc.mUnk_2ac_0 = 0;
        return 1;
    }
    return 0;
}

// Sets the sound ids and key mask of the newest button (or of the first one when ids are inserted in front).
void func_020208dc(int flags, u16 a, u16 b, u16 c) {
    int index;
    if ((flags & TOUCH_ADD_FRONT) == 0) {
        index = data_020c2560.mUnk_100.mUnk_08 - 1;
    } else {
        index = 0;
    }
    data_020c1d60.mUnk_000[index].mUnk_c = b;
    data_020c1d60.mUnk_000[index].mUnk_a = a;
    data_020c1d60.mUnk_000[index].mUnk_e = c;
}

void func_02020914(int a, int b, int c, int d, int e, int f, u16 g, u16 h, u16 i) {
    func_02020944(a, b, c, d, e, f, g, h);
    func_020208dc(a, g, h, i);
}

void func_02020944(int a, int b, int c, int d, int e, int f, u16 g, u16 h) {
    if (!(data_020c18bc.mUnk_46c & SYSTEM_TOUCH_LOCK)) {
        func_02020978(a, b, c, d, e, f);
        func_020208dc(a, g, h, 0);
    }
}

void func_02020978(int id, int x, int y, int a, int b, int c) {
    s32 rect[4];
    int index;
    int shifting;
    Unk_TouchButton *button;
    if (data_020c2560.mUnk_100.mUnk_08 >= TOUCH_BUTTON_COUNT) {
        OS_Terminate();
    }
    if (data_020c18bc.mUnk_46c & SYSTEM_TOUCH_LOCK) {
        return;
    }
    shifting = id & TOUCH_ADD_FRONT;
    if (shifting == 0) {
        index = data_020c2560.mUnk_100.mUnk_08;
    } else {
        index = data_020c2560.mUnk_100.mUnk_08++;
        while (index != 0) {
            data_020c1d60.mUnk_000[index] = data_020c1d60.mUnk_000[index - 1];
            index--;
        }
    }
    button = &data_020c1d60.mUnk_000[index];
    button->mUnk_c = 0;
    button->mUnk_a = 0;
    button->mUnk_e = 0;
    if (c == -1) {
        button->mUnk_2 = y;
        button->mUnk_6 = b;
        button->mUnk_0 = x;
        button->mUnk_4 = a;
    } else if (!(id & TOUCH_SHAPE)) {
        if (func_0202d828(c, 0, rect) == 0) {
            return;
        }
        button->mUnk_2 = y + rect[1];
        button->mUnk_6 = y + rect[3];
        button->mUnk_0 = x + rect[0];
        button->mUnk_4 = x + rect[2];
    } else {
        button->mUnk_0 = x;
        button->mUnk_2 = y;
        button->mUnk_6 = c;
    }
    if (data_020c2560.mUnk_100.mUnk_16 != 0) {
        int mode = func_02020878() == 1 ? 2 : 1;
        func_0202cf08(data_020c2560.mUnk_100.mUnk_16, button->mUnk_0, button->mUnk_2, 0, mode);
        func_0202cf08(data_020c2560.mUnk_100.mUnk_16, button->mUnk_4, button->mUnk_6, 0, mode);
    }
    button->mUnk_8 = id;
    if (shifting == 0) {
        data_020c2560.mUnk_100.mUnk_08++;
    }
}

int func_02020a90(int x, int y, int shape) {
    if (data_020c18bc.mUnk_2ac_1) {
        return func_02020ab8(x, y, shape, data_020c18bc.mUnk_2ae, data_020c18bc.mUnk_2af);
    }
    return 0;
}

typedef struct TouchPoint {
    s32 x;
    s32 y;
} TouchPoint;

// Tests whether the point (px, py) lies inside the polygon the shape table describes, placed at (x, y).
int func_02020ab8(int x, int y, int shape, int px, int py) {
    u32 i;
    int inside;
    u16 id;
    TouchPoint points[16];
    TouchPoint corner;
    int found = 1;
    int next = 0;
    u32 count;
    TouchPoint *base;
    TouchPoint *p;
    while (next == 0 || found == 1) {
        count = 0;
        inside = 0;
        base = points;
        p = base;
        id = shape;
        for (;;) {
            found = func_0202d7a0(id, next++ + 2, (s32 *) &corner);
            if (found != 1) {
                break;
            }
            if (count != 0 && points[0].x == x + corner.x && points[0].y == y + corner.y) {
                break;
            }
            p->x = x + corner.x;
            base[count].y = y + corner.y;
            p++;
            count++;
        }
        if (count != 0 && count <= 2) {
            OS_Terminate();
        }
        if (count > 2) {
            points[count] = points[0];
            p = points;
            for (i = 0; i < count; i++) {
                inside = inside * 2 + ((func_0201febc(p[0].x, p[0].y, p[1].x, p[1].y, px, py) >> 31) & 1);
                p++;
            }
            if (inside == (1 << count) - 1) {
                return 1;
            }
        }
    }
    return 0;
}

void func_02020ba0(void) {
    data_020c2560.mUnk_100.mUnk_08 = 0;
}

// Polls the touch screen and the keys and returns the button state: the button id in the low half and the
// TOUCH_* flags above it.
u32 func_02020bac(u32 mask) {
    u32 result = data_020c2560.mUnk_100.mUnk_12;
    u32 wanted = result;
    int hit = -1;
    u8 pos[2];
    u32 i;
    if (mask != TOUCH_NONE && mask != result) {
        wanted = mask;
    }
    if (func_02020e3c(&pos[1], &pos[0]) == 1) {
        for (i = 0; i < data_020c2560.mUnk_100.mUnk_08; i++) {
            Unk_TouchButton *button = &data_020c1d60.mUnk_000[i];
            if (!(button->mUnk_8 & TOUCH_SHAPE)) {
                if (pos[0] >= button->mUnk_2 && pos[0] <= button->mUnk_6 && pos[1] >= button->mUnk_0 &&
                    pos[1] <= button->mUnk_4) {
                    hit = i;
                    data_020c2560.mUnk_100.mUnk_12 = data_020c1d60.mUnk_000[i].mUnk_8 & 0x7fff;
                    break;
                }
            } else if (func_02020a90(button->mUnk_0, button->mUnk_2, button->mUnk_6)) {
                hit = i;
                data_020c2560.mUnk_100.mUnk_12 = data_020c1d60.mUnk_000[i].mUnk_8 & 0x7fff;
                break;
            }
        }
        if (i == data_020c2560.mUnk_100.mUnk_08) {
            data_020c2560.mUnk_100.mUnk_12 = TOUCH_NONE;
        }
        result = data_020c2560.mUnk_100.mUnk_12 | TOUCH_DOWN;
        if (data_020c2560.mUnk_100.mUnk_14 == 0 ||
            (wanted != data_020c2560.mUnk_100.mUnk_12 && !(data_020c18bc.mUnk_46c & SYSTEM_TOUCH_QUIET))) {
            result |= TOUCH_PRESSED;
            if (wanted == data_020c2560.mUnk_100.mUnk_12) {
                result |= TOUCH_REPEAT;
                if (data_020c18bc.mUnk_45c - data_020c2560.mUnk_100.mUnk_0c < TOUCH_QUICK_FRAMES) {
                    result |= TOUCH_QUICK;
                }
            }
            if (data_020c2560.mUnk_100.mUnk_14 == 0) {
                data_020c2560.mUnk_100.mUnk_0c = data_020c18bc.mUnk_45c;
            }
        }
        data_020c2560.mUnk_100.mUnk_14 = 1;
    } else {
        if (data_020c2560.mUnk_100.mUnk_14 == 1) {
            result |= TOUCH_RELEASED;
            for (i = 0; i < data_020c2560.mUnk_100.mUnk_08; i++) {
                if ((u16) result == data_020c1d60.mUnk_000[i].mUnk_8) {
                    hit = i;
                    break;
                }
            }
        }
        data_020c2560.mUnk_100.mUnk_14 = 0;
    }
    if ((result & (TOUCH_DOWN | TOUCH_PRESSED | TOUCH_RELEASED)) == 0) {
        if ((u16) (data_020c18bc.mUnk_10 | data_020c18bc.mUnk_12 | data_020c18bc.mUnk_014) != 0) {
            for (i = 0; i < data_020c2560.mUnk_100.mUnk_08; i++) {
                u16 keys = data_020c1d60.mUnk_000[i].mUnk_e;
                if (keys != 0) {
                    if (data_020c18bc.mUnk_10 & keys) {
                        result = (result & 0xffff0000) | data_020c1d60.mUnk_000[i].mUnk_8 | TOUCH_DOWN;
                    }
                    if (data_020c18bc.mUnk_12 & keys) {
                        result = (result & 0xffff0000) | data_020c1d60.mUnk_000[i].mUnk_8 | TOUCH_PRESSED;
                    }
                    if (keys & data_020c18bc.mUnk_014) {
                        result = (result & 0xffff0000) | data_020c1d60.mUnk_000[i].mUnk_8 | TOUCH_RELEASED;
                    }
                }
            }
        }
    }
    data_020c2560.mUnk_100.mUnk_08 = 0;
    if (hit != -1 && (u16) result != TOUCH_NONE) {
        if (result & TOUCH_RELEASED) {
            if (data_020c1d60.mUnk_000[hit].mUnk_c != 0) {
                func_0202eb64(data_020c1d60.mUnk_000[hit].mUnk_c, 100, 0, 0x40, TOUCH_NONE, 0);
                if (data_020c1d60.mUnk_000[hit].mUnk_a != 0) {
                    func_0202edd0(data_020c1d60.mUnk_000[hit].mUnk_a);
                }
            }
        } else if (result & TOUCH_PRESSED) {
            if (data_020c1d60.mUnk_000[hit].mUnk_a != 0) {
                func_0202eb64(data_020c1d60.mUnk_000[hit].mUnk_a, 100, 0, 0x40, TOUCH_NONE, 0);
            }
        }
    }
    return result;
}

void func_02020df4(void) {
    data_020c2560.mUnk_100.mUnk_18 = 0;
    data_020c2560.mUnk_100.mUnk_10 = 0;
    MIi_CpuClear16(0xffff, data_020c2560.mUnk_000, sizeof(data_020c2560.mUnk_000));
}

void func_02020e18(u16 a, int b) {
    data_020c2560.mUnk_100.mUnk_12 = TOUCH_NONE;
    data_020c2560.mUnk_100.mUnk_08 = 0;
    data_020c2560.mUnk_100.mUnk_14 = 0;
    data_020c2560.mUnk_100.mUnk_0c = 0;
    data_020c2560.mUnk_100.mUnk_16 = a;
    data_020c2560.mUnk_100.mUnk_04 = TOUCH_NONE;
    func_02020df4();
}

int func_02020e3c(u8 *x, u8 *y) {
    if (data_020c18bc.mUnk_2ac_1 == 0) {
        if (x != 0) {
            *x = 0xff;
        }
        if (y != 0) {
            *y = 0xff;
        }
        if (data_020c2560.mUnk_100.mUnk_18 == 1) {
            data_020c2560.mUnk_100.mUnk_00[2] = data_020c18bc.mUnk_2ae;
            data_020c2560.mUnk_100.mUnk_00[3] = data_020c18bc.mUnk_2af;
            data_020c2560.mUnk_100.mUnk_18 = 2;
        } else if (data_020c2560.mUnk_100.mUnk_18 == 2) {
            data_020c2560.mUnk_100.mUnk_18 = 3;
        }
        return 0;
    }
    if (data_020c2560.mUnk_100.mUnk_18 != 1) {
        func_02020df4();
        data_020c2560.mUnk_100.mUnk_00[0] = data_020c18bc.mUnk_2ae;
        data_020c2560.mUnk_100.mUnk_00[1] = data_020c18bc.mUnk_2af;
        data_020c2560.mUnk_100.mUnk_18 = 1;
    }
    {
        u8 px = data_020c18bc.mUnk_2ae;
        u32 n = data_020c2560.mUnk_100.mUnk_10;
        data_020c2560.mUnk_000[n].mUnk_0 = px;
        data_020c2560.mUnk_000[n].mUnk_1 = data_020c18bc.mUnk_2af;
        data_020c2560.mUnk_100.mUnk_10 = (data_020c2560.mUnk_100.mUnk_10 + 1) & 0x7f;
        if (x != 0) {
            *x = px;
        }
    }
    if (y != 0) {
        *y = data_020c18bc.mUnk_2af;
    }
    return 1;
}
