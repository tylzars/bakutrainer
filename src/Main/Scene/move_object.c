#include <Main/Input/touch_buttons.h>
#include <Main/Save/save_blocks.h>
#include <Main/System/screen_fade.h>
#include <Main/System/game_math.h>
#include <Main/System/bg_image.h>
#include <Main/System/system_init.h>
#include <Main/System/sound_control.h>
#include <Main/System/game_util.h>
#include <Main/Scene/move_object.h>
#include <Main/Graphics/model.h>
#include <Main/Graphics/sprite_script.h>
#include <Main/Map/map_entries.h>
#include <Main/Graphics/script_data.h>
#include <Main/Graphics/gx_pool.h>
#include <Main/Graphics/texture_blocks.h>
#include <Main/clib.h>
#include <nitro/fx.h>
#include <nitro/gx.h>
#include <nitro/os.h>

void SetupMoveObj(MoveObj *o, int sprite, int model, const Vec2i *pos, int e, int f, int g) {
    o->mPresent = 1;
    o->mX = pos->x << 8;
    o->mY = pos->y << 8;
    o->mRepeat = g;
    o->mDrawOrder = e;
    o->mLayer = f;
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
        StartSprite(o, gSpriteScripts[sprite]);
    }
    if (model >= 0) {
        o->mFlags |= 2;
        o->mScale = 0x1000;
    }
    UpdateMoveObj(o, 0);
    ClearAnimTracks();
}

void StartMoveObj(MoveObj *o, int speed, const Vec2i *target, int limit) {
    o->mSpeed = speed;
    o->mTargetX = target->x << 8;
    o->mTargetY = target->y << 8;
    o->mStartX = o->mX;
    o->mStartY = o->mY;
    o->mProgress = 0;
    o->mUnk_2b4 = limit;
    o->mArrived = 0;
}

void SetMoveObjTrail(MoveObj *o, u16 lo, u16 hi) {
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

void StopMoveObj(MoveObj *o) {
    o->mPresent = 0;
    o->mSpeed = 0;
}

int UpdateMoveObj(MoveObj *o, const Vec2i *pos) {
    int result = 0;
    if (o->mPresent != 0) {
        int keep = 1;
        if (o->mFlags & 1) {
            UpdateSprite(o);
            if (o->mSprite.mUnk_de == 0) {
                keep = 0;
            }
        }
        if (o->mFlags & 2) {
            UpdateModelScript(&o->mModel);
            if (o->mModel.mUnk_172_9 == 0) {
                keep = 0;
            }
            PlayObjectSe(&o->mModel);
        }
        if (pos != 0) {
            o->mX = pos->x << 8;
            o->mY = pos->y << 8;
        } else if (o->mSpeed != 0) {
            int dx, dy;
            int oldX, oldY;
            int dist;
            int tx, ty;
            oldX = o->mX;
            oldY = o->mY;
            dx = (o->mTargetX - o->mStartX) >> 8;
            dy = (o->mTargetY - o->mStartY) >> 8;
            dist = Length2d(dx, dy);
            o->mX = o->mStartX + (o->mProgress * dx) / (dist + 1);
            o->mY = o->mStartY + (o->mProgress * dy) / (dist + 1);
            o->mProgress += o->mSpeed;
            tx = o->mTargetX;
            if (((oldX - tx) != 0 ? (((oldX - tx) >> 31) | 1) : 0) !=
                ((o->mX - tx) != 0 ? (((o->mX - tx) >> 31) | 1) : 0)) {
                o->mX = tx;
            }
            ty = o->mTargetY;
            if (((oldY - ty) != 0 ? (((oldY - ty) >> 31) | 1) : 0) !=
                ((o->mY - ty) != 0 ? (((o->mY - ty) >> 31) | 1) : 0)) {
                o->mY = ty;
            }
            if (o->mX == o->mTargetX && o->mY == o->mTargetY) {
                if (o->mUnk_2b4 == 0) {
                    int same = 1;
                    int i;
                    dx = o->mTrailEnd - o->mTrailStart;
                    if (dx > 0) {
                        for (i = 0; i < dx; i++) {
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
            o->mPresent = 0;
        }
    }
    return result;
}

// The screen position of the object. When y has passed the edge of *layer's screen toward the other screen
// (y < -0x1e00 or y > 0xde00, depending on which screen is on top), it switches *layer to the other layer and moves the
// y position by 252.
void GetMoveObjDrawPos(MoveObj *o, int *outX, int *outY, int *layer) {
    int flag = IsLcdSwapped();
    *outX = o->mX >> 8;
    *outY = o->mY >> 8;
    if (*layer == 1) {
        if ((flag && o->mY > 0xde00) || (!flag && o->mY < -0x1e00)) {
            *layer = 2;
            *outY = flag ? *outY - 252 : *outY + 252;
            return;
        }
    }
    if (*layer == 2) {
        if ((flag && o->mY < -0x1e00) || (!flag && o->mY > 0xde00)) {
            *layer = 1;
            *outY = flag ? *outY + 252 : *outY - 252;
        }
    }
}

void DrawMoveObj(MoveObj *o) {
    MoveObj tmp;
    DrawParams params;
    int sx;
    int sy;
    int layer;
    int i;
    if (o->mPresent != 0) {
        if (o->mFlags & 1) {
            layer = o->mLayer;
            if (o->mSpeed == 0 || o->mUnk_2b4 != 0 || o->mX != o->mTargetX || o->mY != o->mTargetY) {
                GetMoveObjDrawPos(o, &sx, &sy, &layer);
                func_0202de80(o->mSprite.mUnk_04, o->mX >> 8, sy, o->mDrawOrder, -1, layer);
            }
            if (o->mTrailEnd - o->mTrailStart > 0) {
                for (i = 0; i < o->mTrailEnd - o->mTrailStart; i++) {
                    tmp = *o;
                    tmp.mTrailStart = tmp.mTrailEnd = 0;
                    tmp.mSprite.mUnk_04 = o->mSprite.mUnk_04 + o->mTrailStart + i;
                    tmp.mX = o->mTrail[i].x;
                    tmp.mY = o->mTrail[i].y;
                    tmp.mDrawOrder = o->mDrawOrder + 1 >= 255 ? 255 : o->mDrawOrder + 1;
                    DrawMoveObj(&tmp);
                }
            }
        }
        if (o->mFlags & 2) {
            SetOrthoDrawParams(&params, o->mX >> 8, o->mY >> 8, -0x3000, o->mScale, 0x8000, -1, 0);
            params.mUnk_2c = -1;
            params.mUnk_34 = -1;
            DrawModelFileEx(o->mModel.mId, &params, &o->mModel, 0);
            func_020334f8(0);
        }
    } else {
        ClearAnimTracks();
    }
}

// The screen set-up of many states: one system frame (UpdateSystemAndWait), SetFaded(3), ResetKeys, the release of
// the texture layers and the gx pool, ResetVramBanks, ResetTouchButtons(0, 0), the LCD swap, texture layers 1 and 2
// (with f and g), the gx pool (with d and e), the images layer1Image on layer 1 and layer2Image on layer 2 when not 0,
// and the 3D display registers (func_02024208).
void ResetAndSetupGraphics(int lcdSwap, int layer1Image, int layer2Image, int d, int e, int f, int g) {
    UpdateSystemAndWait();
    SetFaded(3);
    ResetKeys();
    ReleaseTextureLayers();
    ReleaseGxPool();
    ResetVramBanks();
    ResetTouchButtons(0, 0);
    SetLcdSwap(lcdSwap);
    SetupTextureLayer(0x10cb, 0x10ca, 0x10cc, f, 1);
    SetupTextureLayer(0x10cb, 0x10ca, 0x10cc, g, 2);
    InitGxPool(0x1c7, 0x1c6, d, e);
    if (layer1Image != 0) {
        LoadImage(layer1Image, 1);
    }
    if (layer2Image != 0) {
        LoadImage(layer2Image, 2);
    }
    func_02024208();
}
