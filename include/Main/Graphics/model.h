#pragma once

#include "types.h"
#include <nitro/fx.h>
#include <Main/Unknown/game_util.h>
#include <Main/Unknown/move_object.h>

#ifdef __cplusplus
extern "C" {
#endif

// Key of an animation track: a byte offset into the track data and how the frame indexes it.
typedef struct AnimKey {
    u16 mOffset : 14;
    u16 mMode : 2;
} AnimKey;

// The same key read by halfword tracks: the offset counts halfwords, so its low bit is unused.
typedef struct AnimKey16 {
    u16 mPad : 1;
    u16 mIndex : 13;
    u16 mMode : 2;
} AnimKey16;

// Key of a material track: a 15-bit constant, or (bit 15 set) an offset into the track data.
typedef union MatKey {
    struct {
        u16 mValue : 15;
        u16 mIndexed : 1;
    };
    struct {
        u16 mPad : 1;
        u16 mIndex : 14;
        u16 mPad2 : 1;
    };
} MatKey;

// One frame of a texture animation: texture and palette sources.
typedef struct ModelFrame {
    u16 mUnk_00;
    u16 mUnk_02;
    u16 mUnk_04;
    u16 mUnk_06;
    u16 mUnk_08;
    u16 mUnk_0a;
    u16 mUnk_0c;
} ModelFrame;

// A texture animation: its frame count and frames.
typedef struct ModelFrameAnim {
    u16 mCount;
    ModelFrame mFrames[1];
} ModelFrameAnim;

// Texture animations of a model: per animation, its offset (in halfwords from mEntries) and its speed.
typedef struct ModelTexAnims {
    u8 mCount;
    u8 mUnk_1;
    struct {
        u16 mOffset;
        u16 mDiv;
    } mEntries[1];
} ModelTexAnims;

// A key-frame track: frame count, byte offset of its keys in mData, then the data.
typedef struct ModelKeyTrack {
    u16 mCount;
    u16 mKeyOffset;
    u8 mData[1];
} ModelKeyTrack;

// Texture-matrix or material animations of a model: per animation, its offset in halfwords from mOffsets.
typedef struct ModelTracks {
    u8 mCount;
    u8 mUnk_1;
    u16 mOffsets[1];
} ModelTracks;

// One part of a model: material and texture; a display list of mUnk_34 bytes follows it.
typedef struct ModelPart {
    u32 mUnk_00;
    u32 mUnk_04;
    u32 mUnk_08;
    u32 mUnk_0c;
    u32 mUnk_10;
    u32 mUnk_14;
    u32 mUnk_18;
    u16 mUnk_1c;
    u16 mUnk_1e;
    u16 mUnk_20;
    u16 mUnk_22;
    u16 mUnk_24;
    u16 mUnk_26;
    u16 mUnk_28;
    u8 mUnk_2a;
    u8 mUnk_2b;
    u8 mUnk_2c;
    u8 mUnk_2d;
    u8 mUnk_2e[2];
    s16 mUnk_30;
    s16 mUnk_32;
    u16 mUnk_34;
    u8 mUnk_36[2];
} ModelPart;

void func_0202350c(u16 *rot, s32 *pos, s32 *vec, u32 frame, u32 map);
void func_0202397c(int from, int node);
void func_02023f0c(VecFx32 *out, int node);
void func_02023f58(DrawParams *p, int x, int y, int z, int a, u16 b, u8 c, u8 d, int flags);
void func_02023ff0(void);
void func_020240ec(void);
void func_02024208(void);
void func_020242a8(void *out, int x, int y, int z, int scale, u16 angle, int flags, int w);
void func_020242e8(u16 id, int x, int y, int a, int b, u16 c, u8 d, int e);
void func_02024338(void *a, void *b, int c);
void func_02024368(Model *obj, int x, int y, int scale, u16 angle, int flags);
void func_020243c0(int id, fx32 x, fx32 y, fx32 z, u16 angle, fx32 sx, fx32 sy, u32 alpha);
void func_020248e8(u32 id, void *p, int flags);
void func_020248fc(void);
ModelPart *func_02024910(int id, int a);
u16 func_02024968(const AnimKey *key, const u8 *table, int t);
int func_02024a64(const AnimKey16 *key, const u8 *table, int t);
u32 func_02024b14(const DrawParams *p);
s32 func_02024b2c(const u32 *data, const DrawParams *p);
void func_02024b5c(u32 id, void *params, void *obj, int flags);
void func_02025db4(void *pos, s32 *out);
void func_02025dc4(const VecFx32 *pos, s32 *out, int steps);
int func_02025f58(int log2w, int log2h, int format);
int func_02025fc0(u16 id);
int func_02025fe0(Model *m, int track);
void func_02025ff4(Model *m, int n);
int func_02026004(Model *m);
void func_0202601c(u16 id, u16 *frame, u32 count, int step, s32 *whole, s32 *next, s32 *rest, int loop);
int func_02026080(Model *m);
int func_020260f4(Model *m, u32 track);
int func_02026268(Model *m, u32 track);
void func_020262a0(Model *m);
void func_0202635c(Model *m, s32 *slot);
void func_02026384(Model *m, s32 *slot, int id, int count, int flags, int mode);
int func_020264d4(Model *m, u16 id);
void func_02026598(Model *m, u16 id, u32 value);
void func_020266a8(Model *m, s32 *slot);
int func_02026718(Model *m);
void func_020271c8(void);
void func_020271e0(Model *m);
void func_020271ec(void *obj, int id, int anim, int count, u32 script);
void func_02027200(Model *m, int id, int anim, int count, int mode, u32 script);
void func_02027230(void *obj, int id, int anim, int count, u32 script);
void func_02027244(Model *m, int id, int anim, int count, int mode, u32 script);
void func_0202733c(void *obj, void *params, int flags);

#ifdef __cplusplus
}
#endif
