#pragma once

#include "types.h"
#include <nitro/fx.h>
#include <Main/Unknown/game_util.h>
#include <Main/Unknown/move_object.h>
#include <Main/System/system_init.h>
#include <Main/Map/map_entries.h>

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
    u16 mTexKey;
    u16 mTexStoredSize;
    u16 mTexSrcLow;
    u16 mTexSrcHigh;
    u16 mPlttKey;
    u16 mPlttSrcLow;
    u16 mPlttSrcHigh;
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

// One part of a model: material and texture; a display list of mCommandSize bytes follows it.
typedef struct ModelPart {
    u32 mUnk_00;
    u32 mTexAnimMask;
    u32 mUnk_08;
    u32 mMatAnimMask;
    // FindOrLoadTex's `kind`: the data's offset in mTexFile (1 and 2 would select the block file and the file
    // gSystemState.mUnk_42c).
    u32 mTexSrc;
    // 0 = no palette; otherwise FindOrLoadPltt's `kind`.
    u32 mPlttSrc;
    u32 mTexStoredSize;
    u16 mTexKey;
    u16 mPlttKey;
    u16 mPlttSize;
    // The low and high halves of the DIF_AMB and SPE_EMI words (draw flag 0x10 forces the DIF_AMB high half to
    // 0x7fff).
    u16 mDifAmbLow;
    u16 mDifAmbHigh;
    u16 mSpeEmiLow;
    u16 mSpeEmiHigh;
    u8 mUnk_2a;
    // 0 = untextured; otherwise the low 3 bits are the texture format.
    u8 mTexFormat;
    u8 mUnk_2c;
    u8 mUnk_2d;
    u8 mUnk_2e[2];
    s16 mUnk_30;
    s16 mUnk_32;
    u16 mCommandSize;
    u8 mUnk_36[2];
} ModelPart;

// This file's .bss: the working matrix, the graphics pools and model animation state, the map state and the matrix of
// each pose node.
extern MtxFx43 data_020c31ac;
extern Unk_020c31dc data_020c31dc;
extern MapState gMapState;
extern MtxFx43 gPoseNodeMatrices[64];

void ReadAnimTrackNode(u16 *rot, s32 *pos, s32 *vec, u32 node, u32 track);
void ComputePoseMatrices(int from, int node);
void GetPoseNodePos(VecFx32 *out, int node);
void SetDrawParams(DrawParams *p, int x, int y, int z, int a, u16 b, u8 c, u8 d, int flags);
void func_02023ff0(void);
void func_020240ec(void);
void func_02024208(void);
void func_020242a8(void *out, int x, int y, int z, int scale, u16 angle, int flags, int w);
void func_020242e8(u16 id, int x, int y, int a, int b, u16 c, u8 d, int e);
void func_02024338(void *a, void *b, int c);
void func_02024368(Model *obj, int x, int y, int scale, u16 angle, int flags);
void func_020243c0(int id, fx32 x, fx32 y, fx32 z, u16 angle, fx32 sx, fx32 sy, u32 alpha);
void DrawModelFile(u32 id, void *p, int flags);
void ClearTrackCount(void);
ModelPart *GetFirstModelPart(int id, int a);
u16 GetAnimKeyAngle(const AnimKey *key, const u8 *table, int t);
int GetAnimKeyValue(const AnimKey16 *key, const u8 *table, int t);
u32 GetDrawFrame(const DrawParams *p);
s32 func_02024b2c(const u32 *data, const DrawParams *p);
void DrawModelFileEx(u32 id, void *params, void *obj, int flags);
void ProjectToScreen(void *pos, s32 *out);
void ProjectToScreenEx(const VecFx32 *pos, s32 *out, int steps);
int GetTexByteSize(int log2w, int log2h, int format);
int GetAnimFrameCount(u16 id);
int GetTrackFrameCount(Model *m, int track);
void SetModelFrameIndex(Model *m, int n);
int GetModelFrameIndex(Model *m);
void SplitAnimFrameAndAdvance(u16 id, u16 *frame, u32 count, int step, s32 *whole, s32 *next, s32 *rest, int loop);
int func_02026080(Model *m);
int func_020260f4(Model *m, u32 track);
int func_02026268(Model *m, u32 track);
void UpdateModelScript(Model *m);
void func_0202635c(Model *m, s32 *slot);
void StartModelAnim(Model *m, s32 *slot, int id, int count, int flags, int mode);
int GetModelScriptReg(Model *m, u16 id);
void SetModelScriptReg(Model *m, u16 id, u32 value);
void func_020266a8(Model *m, s32 *slot);
int ExecModelScript(Model *m);
void ClearAnimTracks(void);
void func_020271e0(Model *m);
void InitModel(void *obj, int id, int anim, int count, u32 script);
void InitModelEx(Model *m, int id, int anim, int count, int mode, u32 script);
void StartModel(void *obj, int id, int anim, int count, u32 script);
void StartModelEx(Model *m, int id, int anim, int count, int mode, u32 script);
void UpdateAndDrawModel(void *obj, void *params, int flags);

#ifdef __cplusplus
}
#endif
