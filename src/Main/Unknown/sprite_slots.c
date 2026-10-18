#include <Main/clib.h>
#include <nitro/os.h>
#include <Main/Unknown/functions.h>
#include <Main/Unknown/globals.h>
#include <Main/Unknown/sprite_slots.h>

void func_0200d6c8(void) {
    u32 i;
    data_020abfc0.mLastId = 0;
    for (i = 0; i < OBJ_SLOT_COUNT; i++) {
        data_020ace4c[i].mKind = 0;
    }
}

ObjSlot *func_0200d6ec(int kind, int a, int x, int y, int vx, int vy, int life, int h) {
    u32 i;
    ObjSlot *s = data_020ace4c;
    for (i = 0; i < OBJ_SLOT_COUNT; i++, s++) {
        if (s->mKind == 0) {
            if (life < 0) {
                OS_Terminate();
            }
            s->mKind = kind;
            s->mUnk_0c = a;
            data_020abfc0.mLastId++;
            s->mId = data_020abfc0.mLastId;
            s->mX = x << 8;
            s->mY = y << 8;
            s->mVx = vx;
            s->mVy = vy;
            s->mUnk_158 = life;
            s->mUnk_15c = h;
            s->mUnk_140 = 0;
            return s;
        }
    }
    return 0;
}

int func_0200d76c(int kind, int a, int b, int c, int d) {
    return func_0200d78c(kind, 0, a, b, 0, 0, c, d);
}

int func_0200d78c(int kind, int a, int b, int c, int d, int e, int f, int g) {
    ObjSlot *s = func_0200d6ec(3, a, b, c, d, e, f, g);
    if (s != 0) {
        s->mUnk_08 = kind;
        func_0202de60(&s->mUnk_30, data_0201abec[kind]);
        return s->mId;
    }
    return 0;
}

int func_0200d7cc(int a, int b, int duration, int c, int d, int e, int f, int g, int h, int i) {
    ObjSlot *s;
    if (duration == 0) {
        OS_Terminate();
    }
    if (duration <= 0 && f > 0) {
        OS_Terminate();
    }
    s = func_0200d6ec(2, b, c, d, e, f, g, h);
    if (s != 0) {
        s->mUnk_44 = a;
        s->mUnk_08 = a;
        if (duration > 0) {
            s->mUnk_10a = duration;
        } else {
            s->mUnk_10a = 0;
            s->mUnk_154 = -duration;
        }
        return s->mId;
    }
    return 0;
}

int func_0200d838(const char *text, int a, const void *font, int b, int x, int y, int c, int d, int e, int f, int g) {
    ObjSlot *s;
    if (b < 0) {
        OS_Terminate();
    }
    s = func_0200d6ec(4, 0, x, y, c, d, e, f);
    if (s != 0) {
        strcpy(s->mText, text);
        s->mUnk_44 = a;
        s->mUnk_30 = font;
        s->mUnk_10a = b;
        return s->mId;
    }
    return 0;
}

ObjSlot *func_0200d88c(int id) {
    u32 i;
    ObjSlot *s = data_020ace4c;
    for (i = 0; i < OBJ_SLOT_COUNT; i++, s++) {
        if (s->mKind != 0 && s->mId == id) {
            return s;
        }
    }
    return 0;
}

void func_0200d8bc(void) {
    u32 i;
    s32 sx, sy;
    ObjSlot *s;
    s32 obj;
    s32 done;
    for (i = 0, s = data_020ace4c; i < OBJ_SLOT_COUNT; s++, i++) {
        s32 rect[4];
        s32 pt[2];
        char buf[0x20];
        if (s->mKind == 0) {
            continue;
        }
        s->mX += s->mVx;
        s->mY += s->mVy;
        sx = s->mX >> 8;
        sy = s->mY >> 8;
        if (s->mKind != OBJ_KIND_TEXT_PLUS && s->mKind != OBJ_KIND_TEXT) {
            done = 0;
            if (s->mKind == OBJ_KIND_ANIM) {
                obj = func_0202db1c(&s->mUnk_30);
                if (s->mUnk_10e == 1) {
                    done = 1;
                }
            } else if (s->mUnk_10a == 0) {
                obj = s->mUnk_44;
                func_0202d828(obj, 0, rect);
                if (sx + rect[0] > 0x100 || sx + rect[2] < 0 || sy + rect[1] > 0xc0 || sy + rect[3] < 0) {
                    done = 1;
                } else {
                    s->mVy += s->mUnk_154;
                }
            } else {
                s->mUnk_10a--;
                if (s->mUnk_10a != 0) {
                    obj = s->mUnk_44;
                } else {
                    done = 1;
                }
            }
            if (done == 0) {
                if (s->mUnk_140 != 0) {
                    func_02025db4(&s->mUnk_134, pt);
                    func_0202cf08(obj + s->mUnk_0c, pt[0] + sx, pt[1] + sy, s->mUnk_158, s->mUnk_15c);
                } else {
                    func_0202cf08(obj + s->mUnk_0c, sx, sy, s->mUnk_158, s->mUnk_15c);
                }
            } else {
                s->mKind = 0;
            }
        } else {
            s->mUnk_10a--;
            if (s->mUnk_10a != 0) {
                u16 saved;
                s32 textObj = s->mUnk_44;
                if (s->mKind == OBJ_KIND_TEXT_PLUS) {
                    char *end;
                    buf[0] = '+';
                    end = func_0201fc08(s->mUnk_40, &buf[1]);
                    *end = 0;
                } else {
                    strcpy(buf, s->mText);
                }
                saved = data_020c18bc.mUnk_308;
                data_020c18bc.mUnk_308 = s->mUnk_158;
                if (s->mUnk_140 != 0) {
                    func_02025db4(&s->mUnk_134, pt);
                    func_020215fc(pt[0] + sx, pt[1] + sy, s->mUnk_30, buf, textObj, 0x100, 0x18, s->mUnk_15c);
                } else {
                    func_020215fc(sx, sy, s->mUnk_30, buf, textObj, 0x100, 0x18, s->mUnk_15c);
                }
                data_020c18bc.mUnk_308 = saved;
            } else {
                s->mKind = 0;
            }
        }
    }
}

void func_0200dacc(void) {
    u32 i;
    for (i = 0; i < ANIM_SLOT_COUNT; i++) {
        data_020abfcc[i].mKind = 0;
    }
    func_0200daec();
}

void func_0200daec(void) {
    data_020abfc0.mUnk_04 = 0;
}

void func_0200daf8(void) {
    u32 i;
    AnimSlot *s = data_020abfcc;
    for (i = 0; i < ANIM_SLOT_COUNT; i++, s++) {
        if ((u32) (s->mKind - 1) <= 1) {
            s->mUnk_17e = (s->mUnk_17e & ~0x1000) | (((u16) data_020abfc0.mUnk_04 & 1) << 12);
            if (s->mKind == 1) {
                func_0202733c(&s->mUnk_0c, s->mUnk_18c, 1);
            } else {
                func_02024338(&s->mUnk_0c, s->mUnk_18c, 1);
            }
            if (data_020abfc0.mUnk_04 == 0 && !(s->mUnk_15d & 0x20)) {
                if (s->mUnk_188 == 0) {
                    if (s->mUnk_158 == 0) {
                        s->mKind = 0;
                    }
                } else {
                    s->mUnk_188--;
                    if (s->mUnk_188 == 0) {
                        s->mKind = 0;
                    }
                }
            }
        }
    }
}

void func_0200dbac(void) {
    func_0201f980(0x24a, 0x78, 0x78, 0, 3);
    func_0201f980(0x247, 0x78, 0x78, 0, 3);
    func_0201f6d0(0x10cc, 0x10cb, 0x10ca, 0x248, 0xda3);
    func_0201f980(0x249, 0x3c, 0x3c, 0, 3);
}

void func_0200dc08(int id, u16 *outId, u16 *outVal, u16 *outOpt1, u16 *outZ1, u16 *outZ2, u16 *outOpt2) {
    u16 zero = 0;
    switch (id) {
        case 0xd10:
            *outVal = 0xd11;
            *outZ1 = zero;
            *outZ2 = zero;
            break;
        case 0xd14:
            *outVal = 0xd15;
            *outZ1 = zero;
            *outZ2 = zero;
            break;
        case 0xd18:
            *outVal = 0xd19;
            *outZ1 = zero;
            *outZ2 = zero;
            break;
        case 0xd1c:
            *outVal = 0xd1d;
            *outZ1 = zero;
            *outZ2 = zero;
            break;
        case 0xd20:
            *outVal = 0xd21;
            *outZ1 = zero;
            *outZ2 = zero;
            break;
        case 0xd24:
            *outVal = 0xd25;
            *outZ1 = zero;
            *outZ2 = zero;
            break;
        case 0xd67:
            *outVal = 0xd68;
            *outZ1 = zero;
            *outZ2 = zero;
            break;
        case 0xd6f:
            *outVal = 0xd70;
            *outZ1 = zero;
            *outZ2 = zero;
            break;
        case 0xd77:
            *outVal = 0xd78;
            *outZ1 = zero;
            *outZ2 = zero;
            break;
        case 0xd7f:
            *outVal = 0xd80;
            *outZ1 = zero;
            *outZ2 = zero;
            break;
        case 0xd87:
            *outVal = 0xd88;
            *outZ1 = zero;
            *outZ2 = zero;
            break;
        case 0xd8f:
            *outVal = 0xd90;
            *outZ1 = zero;
            *outZ2 = zero;
            break;
        default:
            zero = 0;
            func_02027fc2("Map inconnue\002");
            break;
    }
    *outId = id;
    if (outOpt1) {
        *outOpt1 = zero;
    }
    if (outOpt2) {
        *outOpt2 = zero;
    }
}

void func_0200dd5c(const char *path, int b) {
    func_0202f638(path, b);
    while (func_0202f8d0() == 1 && data_020c18bc.mUnk_12 == 0 && !data_020c18bc.mUnk_2ac_0) {
        func_0201bb30();
    }
    func_0202fb4c();
}

void func_0200dd90(void) {
    int i;
    data_020ae504.mUnk_1b8 = 0;
    for (i = 0; i < 8; i++) {
        data_020ae504.mUnk_1bc[i] = 0;
    }
    if (data_020ae504.mUnk_38 < 0) {
        s32 v;
        switch (data_020ae504.mUnk_24) {
            case 1:
                v = -data_020ae504.mUnk_1dc;
                data_020ae504.mUnk_1bc[0] = v << 16;
                data_020ae504.mUnk_1bc[1] = (v * 0x180) << 8;
                data_020ae504.mUnk_1bc[2] = v << 17;
                break;
            case 2:
            case 5:
                v = -data_020ae504.mUnk_1dc;
                data_020ae504.mUnk_1bc[0] = v << 16;
                data_020ae504.mUnk_1bc[1] = (v * 0x180) << 8;
                data_020ae504.mUnk_1bc[2] = v << 17;
                data_020ae504.mUnk_1bc[3] = -0x4000;
                data_020ae504.mUnk_1bc[4] = 0xa000;
                break;
            case 4:
                data_020ae504.mUnk_1bc[0] = -0x4000;
                data_020ae504.mUnk_1bc[1] = 0xa000;
                data_020ae504.mUnk_1bc[2] = 0x10000;
                break;
            case 3:
                data_020ae504.mUnk_1bc[0] = -data_020ae504.mUnk_1dc << 16;
                data_020ae504.mUnk_1bc[4] = 0x8000;
                break;
            case 6:
                v = -data_020ae504.mUnk_1dc;
                data_020ae504.mUnk_1bc[0] = v << 16;
                data_020ae504.mUnk_1bc[1] = (v * 0x180) << 8;
                data_020ae504.mUnk_1bc[3] = -0x4000;
                data_020ae504.mUnk_1bc[4] = 0xa000;
                break;
        }
    }
}
