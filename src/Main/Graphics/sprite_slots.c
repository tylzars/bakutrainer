#include <Main/clib.h>
#include <nitro/os.h>
#include <Main/System/screen_fade.h>
#include <Main/System/game_math.h>
#include <Main/System/system_init.h>
#include <Main/Graphics/sprite_slots.h>
#include <Main/Scene/slide_transition.h>
#include <Main/Graphics/text_draw.h>
#include <Main/Graphics/model.h>
#include <Main/System/engine_asm.h>
#include <Main/Graphics/texture_blocks.h>
#include <Main/Video/stream_audio.h>
#include <Main/Graphics/sprite_script.h>
#include <Main/Graphics/script_data.h>

SlotCounters data_020abfc0;
AnimSlot data_020abfcc[ANIM_SLOT_COUNT];
ObjSlot data_020ace4c[OBJ_SLOT_COUNT];

void ClearObjSlots(void) {
    u32 i;
    data_020abfc0.mLastId = 0;
    for (i = 0; i < OBJ_SLOT_COUNT; i++) {
        data_020ace4c[i].mKind = OBJ_KIND_FREE;
    }
}

ObjSlot *AddObjSlot(int kind, int a, int x, int y, int vx, int vy, int life, int h) {
    u32 i;
    ObjSlot *s = data_020ace4c;
    for (i = 0; i < OBJ_SLOT_COUNT; i++, s++) {
        if (s->mKind == OBJ_KIND_FREE) {
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

int AddAnimObjSlot(int script, int a, int b, int c, int d) {
    return AddAnimObjSlotEx(script, 0, a, b, 0, 0, c, d);
}

int AddAnimObjSlotEx(int script, int a, int b, int c, int d, int e, int f, int g) {
    ObjSlot *s = AddObjSlot(OBJ_KIND_ANIM, a, b, c, d, e, f, g);
    if (s != 0) {
        s->mUnk_08 = script;
        StartSprite(&s->mUnk_30, data_0201abec[script]);
        return s->mId;
    }
    return 0;
}

int AddSpriteObjSlot(int a, int b, int duration, int c, int d, int e, int f, int g, int h, int i) {
    ObjSlot *s;
    if (duration == 0) {
        OS_Terminate();
    }
    if (duration <= 0 && f > 0) {
        OS_Terminate();
    }
    s = AddObjSlot(OBJ_KIND_SPRITE, b, c, d, e, f, g, h);
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

int AddTextObjSlot(const char *text, int a, const void *font, int b, int x, int y, int c, int d, int e, int f, int g) {
    ObjSlot *s;
    if (b < 0) {
        OS_Terminate();
    }
    s = AddObjSlot(OBJ_KIND_TEXT, 0, x, y, c, d, e, f);
    if (s != 0) {
        strcpy(s->mText, text);
        s->mUnk_44 = a;
        s->mUnk_30 = font;
        s->mUnk_10a = b;
        return s->mId;
    }
    return 0;
}

ObjSlot *FindObjSlot(int id) {
    u32 i;
    ObjSlot *s = data_020ace4c;
    for (i = 0; i < OBJ_SLOT_COUNT; i++, s++) {
        if (s->mKind != OBJ_KIND_FREE && s->mId == id) {
            return s;
        }
    }
    return 0;
}

void UpdateObjSlots(void) {
    u32 i;
    s32 sx, sy;
    ObjSlot *s;
    s32 obj;
    s32 done;
    for (i = 0, s = data_020ace4c; i < OBJ_SLOT_COUNT; s++, i++) {
        s32 rect[4];
        s32 pt[2];
        char buf[0x20];
        if (s->mKind == OBJ_KIND_FREE) {
            continue;
        }
        s->mX += s->mVx;
        s->mY += s->mVy;
        sx = s->mX >> 8;
        sy = s->mY >> 8;
        if (s->mKind != OBJ_KIND_TEXT_PLUS && s->mKind != OBJ_KIND_TEXT) {
            done = 0;
            if (s->mKind == OBJ_KIND_ANIM) {
                obj = UpdateSprite(&s->mUnk_30);
                if (s->mUnk_10e == 1) {
                    done = 1;
                }
            } else if (s->mUnk_10a == 0) {
                obj = s->mUnk_44;
                GetSpriteRect(obj, 0, rect);
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
                    ProjectToScreen(&s->mUnk_134, pt);
                    DrawSprite(obj + s->mUnk_0c, pt[0] + sx, pt[1] + sy, s->mUnk_158, s->mUnk_15c);
                } else {
                    DrawSprite(obj + s->mUnk_0c, sx, sy, s->mUnk_158, s->mUnk_15c);
                }
            } else {
                s->mKind = OBJ_KIND_FREE;
            }
        } else {
            s->mUnk_10a--;
            if (s->mUnk_10a != 0) {
                u16 saved;
                s32 textObj = s->mUnk_44;
                // No slot is created with OBJ_KIND_TEXT_PLUS in this build (AddObjSlot's callers pass OBJ_KIND_ANIM,
                // OBJ_KIND_SPRITE or OBJ_KIND_TEXT), so this branch does not run.
                if (s->mKind == OBJ_KIND_TEXT_PLUS) {
                    char *end;
                    buf[0] = '+';
                    end = FormatDecimal(s->mUnk_40, &buf[1]);
                    *end = 0;
                } else {
                    strcpy(buf, s->mText);
                }
                saved = gSystemState.mUnk_308;
                gSystemState.mUnk_308 = s->mUnk_158;
                if (s->mUnk_140 != 0) {
                    ProjectToScreen(&s->mUnk_134, pt);
                    DrawText(pt[0] + sx, pt[1] + sy, s->mUnk_30, buf, textObj, 0x100, 0x18, s->mUnk_15c);
                } else {
                    DrawText(sx, sy, s->mUnk_30, buf, textObj, 0x100, 0x18, s->mUnk_15c);
                }
                gSystemState.mUnk_308 = saved;
            } else {
                s->mKind = OBJ_KIND_FREE;
            }
        }
    }
}

void ClearAnimSlots(void) {
    u32 i;
    for (i = 0; i < ANIM_SLOT_COUNT; i++) {
        data_020abfcc[i].mKind = 0;
    }
    func_0200daec();
}

void func_0200daec(void) {
    data_020abfc0.mUnk_04 = 0;
}

// Draws the animation slots of kind 1 (UpdateAndDrawModel) and 2 (func_02024338) and counts their lifetimes down.
// Nothing in this build sets a slot's mKind to 1 or 2 (ClearAnimSlots and this function only write 0), so the loop
// body does not run.
void UpdateAnimSlots(void) {
    u32 i;
    AnimSlot *s = data_020abfcc;
    for (i = 0; i < ANIM_SLOT_COUNT; i++, s++) {
        if ((u32) (s->mKind - 1) <= 1) {
            s->mUnk_17e = (s->mUnk_17e & ~0x1000) | (((u16) data_020abfc0.mUnk_04 & 1) << 12);
            if (s->mKind == 1) {
                UpdateAndDrawModel(&s->mUnk_0c, s->mUnk_18c, 1);
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
    RunImage(0x24a, 0x78, 0x78, 0, 3);
    RunImage(0x247, 0x78, 0x78, 0, 3);
    RunImageWithSprite(0x10cc, 0x10cb, 0x10ca, 0x248, 0xda3);
    RunImage(0x249, 0x3c, 0x3c, 0, 3);
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
            zero = 0;
            *outZ1 = zero;
            *outZ2 = zero;
            break;
        default:
            OpenTextScreen("Map inconnue\002");
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

void RunVideo(const char *path, int b) {
    StartVideo(path, b);
    while (UpdateVideo() == 1 && gSystemState.mKeysPressed == 0 && !gSystemState.mTouchPressed) {
        UpdateSystemAndWait();
    }
    StopVideo();
}

void func_0200dd90(void) {
    int i;
    gTitleScreen.mSlideState = TITLE_SLIDE_STATE_IN;
    for (i = 0; i < 8; i++) {
        gTitleScreen.mSlideOffsets[i] = 0;
    }
    if (gTitleScreen.mReturnState < 0) {
        s32 v;
        switch (gTitleScreen.mState) {
            case 1:
                v = -gTitleScreen.mSlideDirection;
                gTitleScreen.mSlideOffsets[0] = v << 16;
                gTitleScreen.mSlideOffsets[1] = (v * 0x180) << 8;
                gTitleScreen.mSlideOffsets[2] = v << 17;
                break;
            case 2:
            case 5:
                v = -gTitleScreen.mSlideDirection;
                gTitleScreen.mSlideOffsets[0] = v << 16;
                gTitleScreen.mSlideOffsets[1] = (v * 0x180) << 8;
                gTitleScreen.mSlideOffsets[2] = v << 17;
                gTitleScreen.mSlideOffsets[3] = -0x4000;
                gTitleScreen.mSlideOffsets[4] = 0xa000;
                break;
            case 4:
                gTitleScreen.mSlideOffsets[0] = -0x4000;
                gTitleScreen.mSlideOffsets[1] = 0xa000;
                gTitleScreen.mSlideOffsets[2] = 0x10000;
                break;
            case 3:
                gTitleScreen.mSlideOffsets[0] = -gTitleScreen.mSlideDirection << 16;
                gTitleScreen.mSlideOffsets[4] = 0x8000;
                break;
            case 6:
                v = -gTitleScreen.mSlideDirection;
                gTitleScreen.mSlideOffsets[0] = v << 16;
                gTitleScreen.mSlideOffsets[1] = (v * 0x180) << 8;
                gTitleScreen.mSlideOffsets[3] = -0x4000;
                gTitleScreen.mSlideOffsets[4] = 0xa000;
                break;
        }
    }
}
