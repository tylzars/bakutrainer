#include <Main/Unknown/functions.h>
#include <Main/Unknown/touch_buttons.h>
#include <Main/Unknown/save_blocks.h>
#include <Main/Unknown/screen_fade.h>
#include <Main/Unknown/game_math.h>
#include <Main/Unknown/bg_image.h>
#include <Main/Unknown/system_init.h>
#include <Main/Unknown/sound_control.h>
#include <Main/Unknown/game_util.h>
#include <Main/Unknown/globals.h>
#include <Main/Unknown/move_object.h>
#include <Main/clib.h>
#include <nitro/fx.h>
#include <nitro/gx.h>
#include <nitro/os.h>

void func_0200c034(MoveObj *o, int sprite, int model, const Vec2i *pos, int e, int f, int g) {
    o->mActive = 1;
    o->mX = pos->x << 8;
    o->mY = pos->y << 8;
    o->mRepeat = g;
    o->mUnk_294 = e;
    o->mUnk_290 = f;
    o->mSpeed = 0;
    o->mFlags = 0;
    o->mArrived = 0;
    o->mTrailStart = 0;
    o->mTrailEnd = 0;
    if (sprite < 0 && model < 0) {
        OS_Terminate();
    }
    if (sprite >= 0) {
        o->mFlags |= 1;
        func_0202de60(o, data_0201abec[sprite]);
    }
    if (model >= 0) {
        o->mFlags |= 2;
        o->mScale = 0x1000;
    }
    func_0200c174(o, 0);
    func_020271c8();
}

void func_0200c0dc(MoveObj *o, int speed, const Vec2i *target, int limit) {
    o->mSpeed = speed;
    o->mTargetX = target->x << 8;
    o->mTargetY = target->y << 8;
    o->mStartX = o->mX;
    o->mStartY = o->mY;
    o->mProgress = 0;
    o->mUnk_2b4 = limit;
    o->mArrived = 0;
}

void func_0200c124(MoveObj *o, u16 lo, u16 hi) {
    u16 i;
    int count = hi - lo;
    if (count >= TRAIL_LEN) {
        count = TRAIL_LEN;
    }
    if ((u16) count != 0) {
        o->mTrailStart = lo;
        o->mTrailEnd = hi;
        for (i = 0; i < TRAIL_LEN; i++) {
            o->mTrail[i].x = -25600;
            o->mTrail[i].y = -25600;
        }
    }
}

void func_0200c164(MoveObj *o) {
    o->mActive = 0;
    o->mSpeed = 0;
}

int func_0200c174(MoveObj *o, const Vec2i *pos) {
    int result = 0;
    if (o->mActive != 0) {
        int keep = 1;
        if (o->mFlags & 1) {
            func_0202db1c(o);
            if (o->mSprite.mUnk_de == 0) {
                keep = 0;
            }
        }
        if (o->mFlags & 2) {
            func_020262a0(&o->mModel);
            if (o->mModel.mUnk_172_9 == 0) {
                keep = 0;
            }
            func_020105f4(&o->mModel);
        }
        if (pos != 0) {
            o->mX = pos->x << 8;
            o->mY = pos->y << 8;
        } else if (o->mSpeed != 0) {
            int oldX = o->mX;
            int oldY = o->mY;
            int dx = (o->mTargetX - o->mStartX) >> 8;
            int dy = (o->mTargetY - o->mStartY) >> 8;
            int dist = func_0201faf0(dx, dy);
            int tx;
            int ty;
            o->mX = o->mStartX + (o->mProgress * dx) / (dist + 1);
            o->mY = o->mStartY + (dy * o->mProgress) / (dist + 1);
            o->mProgress += o->mSpeed;
            tx = o->mTargetX;
            if ((oldX - tx != 0 ? ((oldX - tx) >> 31) | 1 : 0) != (o->mX - tx != 0 ? ((o->mX - tx) >> 31) | 1 : 0)) {
                o->mX = tx;
            }
            ty = o->mTargetY;
            if ((oldY - ty != 0 ? ((oldY - ty) >> 31) | 1 : 0) != (o->mY - ty != 0 ? ((o->mY - ty) >> 31) | 1 : 0)) {
                o->mY = ty;
            }
            if (o->mX == o->mTargetX && o->mY == o->mTargetY) {
                if (o->mUnk_2b4 == 0) {
                    int same = 1;
                    int n = o->mTrailEnd - o->mTrailStart;
                    int i;
                    if (n > 0) {
                        for (i = 0; i < n; i++) {
                            if (o->mTargetX != o->mTrail[i].x || o->mTargetY != o->mTrail[i].y) {
                                same = 0;
                                break;
                            }
                        }
                    }
                    if (same) {
                        keep = 1;
                        o->mRepeat = 0;
                    }
                }
                if (o->mArrived == 0) {
                    result = 1;
                    o->mArrived = 1;
                }
            }
        }
        {
            int n = o->mTrailEnd - o->mTrailStart;
            if (n > 0) {
                int i = n - 1;
                for (; i > 0; i--) {
                    o->mTrail[i] = o->mTrail[i - 1];
                }
                o->mTrail[0].x = o->mX;
                o->mTrail[0].y = o->mY;
            }
        }
        if (keep && o->mRepeat == 0) {
            o->mActive = 0;
        }
    }
    return result;
}

void func_0200c3d8(MoveObj *o, int *outX, int *outY, int *state) {
    int flag = func_02020878();
    *outX = o->mX >> 8;
    *outY = o->mY >> 8;
    if (*state == 1) {
        if ((flag && o->mY > 0xde00) || (!flag && o->mY < -0x1e00)) {
            *state = 2;
            *outY = flag ? *outY - 252 : *outY + 252;
            return;
        }
    }
    if (*state == 2) {
        if ((flag && o->mY < -0x1e00) || (!flag && o->mY > 0xde00)) {
            *state = 1;
            *outY = flag ? *outY + 252 : *outY - 252;
        }
    }
}

void func_0200c470(MoveObj *o) {
    MoveObj tmp;
    DrawParams params;
    int sx;
    int sy;
    int state;
    int i;
    if (o->mActive != 0) {
        if (o->mFlags & 1) {
            state = o->mUnk_290;
            if (o->mSpeed == 0 || o->mUnk_2b4 != 0 || o->mX != o->mTargetX || o->mY != o->mTargetY) {
                func_0200c3d8(o, &sx, &sy, &state);
                func_0202de80(o->mSprite.mUnk_04, o->mX >> 8, sy, o->mUnk_294, -1, state);
            }
            if (o->mTrailEnd - o->mTrailStart > 0) {
                for (i = 0; i < o->mTrailEnd - o->mTrailStart; i++) {
                    tmp = *o;
                    tmp.mTrailStart = tmp.mTrailEnd = 0;
                    tmp.mSprite.mUnk_04 = o->mSprite.mUnk_04 + o->mTrailStart + i;
                    tmp.mX = o->mTrail[i].x;
                    tmp.mY = o->mTrail[i].y;
                    tmp.mUnk_294 = o->mUnk_294 + 1 >= 255 ? 255 : o->mUnk_294 + 1;
                    func_0200c470(&tmp);
                }
            }
        }
        if (o->mFlags & 2) {
            func_020242a8(&params, o->mX >> 8, o->mY >> 8, -0x3000, o->mScale, 0x8000, -1, 0);
            params.mUnk_2c = -1;
            params.mUnk_34 = -1;
            func_02024b5c(o->mModel.mUnk_170, &params, &o->mModel, 0);
            func_020334f8(0);
        }
    } else {
        func_020271c8();
    }
}

void func_0200c5d8(int a, int b, int c, int d, int e, int f, int g) {
    func_0201bb30();
    func_0201f880(3);
    func_0201c41c();
    func_0202c1c4();
    func_02022370();
    func_0201eb20();
    func_02020e18(0, 0);
    func_02020854(a);
    func_0202bdd8(0x10cb, 0x10ca, 0x10cc, f, 1);
    func_0202bdd8(0x10cb, 0x10ca, 0x10cc, g, 2);
    func_02021fb0(0x1c7, 0x1c6, d, e);
    if (b != 0) {
        func_0201dde0(b, 1);
    }
    if (c != 0) {
        func_0201dde0(c, 2);
    }
    func_02024208();
}
