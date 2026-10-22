#include "types.h"
#include <nitro/fx.h>
#include <nitro/gx.h>
#include <nitro/mi.h>
#include <nitro/os.h>
#include <Main/clib.h>
#include <Main/Graphics/angle_blend.h>
#include <Main/System/game_math.h>
#include <Main/System/game_util.h>
#include <Main/Graphics/model.h>
#include <Main/Scene/move_object.h>
#include <Main/System/random_angle.h>
#include <Main/System/system_init.h>
#include <Main/Graphics/vram_cache.h>
#include <Main/System/engine_asm.h>
#include <Main/Graphics/texture_blocks.h>
#include <Main/System/file_loader.h>
#include <Main/Map/map_entries.h>
#include <Main/System/align_util.h>

// Model script commands (bits 10-15 of a command word) and operand modes (bits 8-9), the MC_ and MM_ values of
// script_data.s.
#define MC_JUMP 0
#define MC_JUMP_IF_EQUAL 1
#define MC_JUMP_IF_NOT_EQUAL 2
#define MC_JUMP_IF_NOT_LESS 3
#define MC_JUMP_IF_LESS 4
// Jumps when the less flag is clear or the equal flag is set, so it also jumps on equal.
#define MC_JUMP_IF_NOT_LESS_OR_EQUAL 5
#define MC_JUMP_IF_EITHER 6
#define MC_SET 7
#define MC_ADD 8
#define MC_SUB 9
#define MC_MOD 10
#define MC_CALL 11
#define MC_COMPARE 12
#define MC_ANIM 14
#define MC_EVENT 15
#define MC_TRACK 16
#define MC_FLAG 17
#define MC_STEP 18
#define MC_SIGNAL 19
#define MC_SET_3C 20
#define MC_SET_3E 21
#define MC_WAIT 22
#define MC_WAIT_OR_IDLE 23
#define MC_LOOK_AT 24
#define MC_TEST_BIT 25
#define MC_TEST_SET_BIT 26
#define MC_TEST_CLEAR_BIT 27
#define MC_MUL 29
#define MC_AND 30
#define MC_MODEL 32
#define MC_READ 63
#define MM_BITS 0
#define MM_REG 1
#define MM_VALUE 2

// This file's .bss (ascending sizes as one file's): the working matrix, data_020c31dc, the map state and the matrix of
// each pose node. Objects without an initialiser join the compiler's size order of this file's data after the others,
// in reverse definition order; this order (with data_02050898 defined below, before its users) puts the vector
// initialisers of func_02023ff0 and func_020240ec in the game's .rodata order.
MtxFx43 data_020c31ac;
Unk_020c31dc data_020c31dc;
MapState gMapState;
MtxFx43 gPoseNodeMatrices[64];

typedef struct Unk_PoseChannel {
    u16 mUnk_0;
    u16 mUnk_2;
} Unk_PoseChannel;

// The channels of one node in a pose animation (ReadAnimTrackNode): three for the third vector, three angles and three
// position values, each a mode (above 1: one value per frame) and a byte offset into the file.
typedef struct Unk_PoseFrame {
    Unk_PoseChannel mUnk_00[3];
    Unk_PoseChannel mUnk_0c[3];
    Unk_PoseChannel mUnk_18[3];
} Unk_PoseFrame;

typedef struct Unk_PoseAnim {
    s16 mUnk_0;
    u8 mUnk_2[6];
    Unk_PoseFrame mUnk_8[1];
} Unk_PoseAnim;

static inline void PutTrans(fx32 x, fx32 y, fx32 z) {
    reg_G3X_MTX_TRANS = x;
    reg_G3X_MTX_TRANS = y;
    reg_G3X_MTX_TRANS = z;
}

static inline void PutScale(fx32 x, fx32 y, fx32 z) {
    reg_G3X_MTX_SCALE = x;
    reg_G3X_MTX_SCALE = y;
    reg_G3X_MTX_SCALE = z;
}

static inline void PutPolygonAttr(int light, int mode, int cull, int id, int alpha, int misc) {
    reg_G3X_POLYGON_ATTR = light | (mode << 4) | (cull << 6) | misc | (id << 24) | (alpha << 16);
}

static inline void PutMaterial(u32 difAmb, u32 speEmi) {
    reg_G3X_DIF_AMB = difAmb;
    reg_G3X_SPE_EMI = speEmi;
}

static inline void PutTexCoord(s16 s, s16 t) {
    reg_G3X_TEXCOORD = (u16) s | ((u16) t << 16);
}

static inline void PutVtx16(s16 x, s16 y, s16 z) {
    reg_G3X_VTX_16 = (u16) x | ((u16) y << 16);
    reg_G3X_VTX_16 = (u16) z;
}

static inline void PutVtxXY(s16 x, s16 y) {
    reg_G3X_VTX_XY = (u16) x | ((u16) y << 16);
}

// Value of a material key: the constant, or the halfword it indexes for this frame.
static inline int MatKeyValue(const MatKey *key, const ModelKeyTrack *track, int frame) {
    if (key->mIndexed == 0) {
        return key->mValue;
    }
    return ((const s16 *) track->mData)[frame + key->mIndex];
}

// Value of a byte material key: the constant, or the byte it indexes for this frame.
static inline int MatKeyByte(const MatKey *key, const ModelKeyTrack *track, int frame) {
    if (key->mIndexed == 0) {
        return key->mValue;
    }
    return (&track->mData[key->mValue])[frame];
}

#pragma thumb off

void ReadAnimTrackNode(u16 *rot, s32 *pos, s32 *vec, u32 node, u32 track) {
    u8 t;
    u32 next;
    u32 cur;
    Unk_PoseFrame *f;
    Unk_PoseAnim *hdr;
    u16 tmpRot[3];
    s32 tmpPos[3];
    s32 tmpVec[3];

    hdr = data_020c31dc.mAnimTracks[track].mData;
    f = &hdr->mUnk_8[node];
    t = data_020c31dc.mAnimTracks[track].mNextWeight;
    next = data_020c31dc.mAnimTracks[track].mNextFrameIndex;
    cur = data_020c31dc.mAnimTracks[track].mFrameIndex;

    if (!(hdr->mUnk_0 & 1)) {
        OS_Terminate();
    }
    if (!(hdr->mUnk_0 & 4)) {
        OS_Terminate();
    }
    if (rot != NULL) {
        rot[0] = ((s16 *) hdr)[(f->mUnk_0c[0].mUnk_0 > 1 ? cur : 0) + (f->mUnk_0c[0].mUnk_2 >> 1)];
        rot[1] = ((s16 *) hdr)[(f->mUnk_0c[1].mUnk_0 > 1 ? cur : 0) + (f->mUnk_0c[1].mUnk_2 >> 1)];
        rot[2] = ((s16 *) hdr)[(f->mUnk_0c[2].mUnk_0 > 1 ? cur : 0) + (f->mUnk_0c[2].mUnk_2 >> 1)];
    }
    if (pos != NULL) {
        if (!(hdr->mUnk_0 & 2)) {
            pos[0] = ((s16 *) hdr)[(f->mUnk_18[0].mUnk_0 > 1 ? cur : 0) + (f->mUnk_18[0].mUnk_2 >> 1)] << 5;
            pos[1] = ((s16 *) hdr)[(f->mUnk_18[1].mUnk_0 > 1 ? cur : 0) + (f->mUnk_18[1].mUnk_2 >> 1)] << 5;
            pos[2] = ((s16 *) hdr)[(f->mUnk_18[2].mUnk_0 > 1 ? cur : 0) + (f->mUnk_18[2].mUnk_2 >> 1)] << 5;
        } else {
            pos[0] = ((s32 *) hdr)[(f->mUnk_18[0].mUnk_0 > 1 ? cur : 0) + (f->mUnk_18[0].mUnk_2 >> 2)];
            pos[1] = ((s32 *) hdr)[(f->mUnk_18[1].mUnk_0 > 1 ? cur : 0) + (f->mUnk_18[1].mUnk_2 >> 2)];
            pos[2] = ((s32 *) hdr)[(f->mUnk_18[2].mUnk_0 > 1 ? cur : 0) + (f->mUnk_18[2].mUnk_2 >> 2)];
        }
    }
    if (vec != NULL) {
        vec[0] = ((s16 *) hdr)[(f->mUnk_00[0].mUnk_0 > 1 ? cur : 0) + (f->mUnk_00[0].mUnk_2 >> 1)];
        vec[1] = ((s16 *) hdr)[(f->mUnk_00[1].mUnk_0 > 1 ? cur : 0) + (f->mUnk_00[1].mUnk_2 >> 1)];
        vec[2] = ((s16 *) hdr)[(f->mUnk_00[2].mUnk_0 > 1 ? cur : 0) + (f->mUnk_00[2].mUnk_2 >> 1)];
    }
    if (t == 0 || cur == next) {
        return;
    }
    if (rot != NULL) {
        tmpRot[0] = ((s16 *) hdr)[(f->mUnk_0c[0].mUnk_0 > 1 ? next : 0) + (f->mUnk_0c[0].mUnk_2 >> 1)];
        tmpRot[1] = ((s16 *) hdr)[(f->mUnk_0c[1].mUnk_0 > 1 ? next : 0) + (f->mUnk_0c[1].mUnk_2 >> 1)];
        tmpRot[2] = ((s16 *) hdr)[(f->mUnk_0c[2].mUnk_0 > 1 ? next : 0) + (f->mUnk_0c[2].mUnk_2 >> 1)];
        BlendAngles(t, 8, rot, tmpRot);
    }
    if (pos != NULL) {
        if (!(hdr->mUnk_0 & 2)) {
            tmpPos[0] = ((s16 *) hdr)[(f->mUnk_18[0].mUnk_0 > 1 ? next : 0) + (f->mUnk_18[0].mUnk_2 >> 1)] << 5;
            tmpPos[1] = ((s16 *) hdr)[(f->mUnk_18[1].mUnk_0 > 1 ? next : 0) + (f->mUnk_18[1].mUnk_2 >> 1)] << 5;
            tmpPos[2] = ((s16 *) hdr)[(f->mUnk_18[2].mUnk_0 > 1 ? next : 0) + (f->mUnk_18[2].mUnk_2 >> 1)] << 5;
        } else {
            tmpPos[0] = ((s32 *) hdr)[(f->mUnk_18[0].mUnk_0 > 1 ? next : 0) + (f->mUnk_18[0].mUnk_2 >> 2)];
            tmpPos[1] = ((s32 *) hdr)[(f->mUnk_18[1].mUnk_0 > 1 ? next : 0) + (f->mUnk_18[1].mUnk_2 >> 2)];
            tmpPos[2] = ((s32 *) hdr)[(f->mUnk_18[2].mUnk_0 > 1 ? next : 0) + (f->mUnk_18[2].mUnk_2 >> 2)];
        }
        BlendVec3(t, 8, pos, tmpPos);
    }
    if (vec != NULL) {
        tmpVec[0] = ((s16 *) hdr)[(f->mUnk_00[0].mUnk_0 > 1 ? next : 0) + (f->mUnk_00[0].mUnk_2 >> 1)];
        tmpVec[1] = ((s16 *) hdr)[(f->mUnk_00[1].mUnk_0 > 1 ? next : 0) + (f->mUnk_00[1].mUnk_2 >> 1)];
        tmpVec[2] = ((s16 *) hdr)[(f->mUnk_00[2].mUnk_0 > 1 ? next : 0) + (f->mUnk_00[2].mUnk_2 >> 1)];
        BlendVec3(t, 8, vec, tmpVec);
    }
}

// Walks the pose tree from `node` through each node's first child and next sibling. The matrix of `from` is the
// starting matrix; each node's matrix is stored in gPoseNodeMatrices[node], and nodes with a slot in mMtxStoreIndex
// are also loaded into the geometry engine's matrix stack. Without track data a node's scale is applied squared
// (mScaleX * (mScaleX << 4)). The walk stops at mStopNodeIndex, mUnk_4a keeps the highest node number reached, and
// bits 0-1 of mUnk_f reduce rows of the matrix to their length on the diagonal (all three, or with bit 1 rows 0
// and 2).
void ComputePoseMatrices(int from, int node) {
    PoseNode *n;
    MtxFx43 m;
    s32 pos[3];
    s32 scale[3];
    s32 tmpPos[3];
    s32 tmpScale[3];
    u16 rot[3];
    int done;
    u16 tmpRot[3];
    s32 sx, cx, sy, cy, sz, cz;
    s32 a, b;
    s32 r00, r01, r02, r10, r11, r12, r20, r21, r22;
    u32 i;
    int ix, iy, iz;
    u16 ax, ay, az;
    u32 step;
    VecFx32 *v;

    n = &data_020c31dc.mPoseNodes[node];
    if (from != -1) {
        func_0203fdd8(&gPoseNodeMatrices[from], &data_020c31ac);
    }
    if (n->mNextSibling != 0xff) {
        func_0203fdd8(&data_020c31ac, &gPoseNodeMatrices[n->mNextSibling]);
    }
    if (n->mFirstChild == 0xff && n->mMtxStoreIndex == -1 && data_020c31dc.mUnk_47 != 0) {
        goto sibling;
    }
    if (data_020c31dc.mTrackFillCount == 0) {
        rot[0] = n->mUnk_5 << 8;
        rot[1] = n->mUnk_6 << 8;
        rot[2] = n->mUnk_7 << 8;
        pos[0] = n->mPosX << 5;
        pos[1] = n->mPosY << 5;
        pos[2] = n->mPosZ << 5;
        scale[0] = n->mScaleX << 4;
        scale[1] = n->mScaleY << 4;
        scale[2] = n->mScaleZ << 4;
    } else {
        done = 0;
        ApplyAnimTrack(0, rot, pos, scale, node, &done, n);
        if (data_020c31dc.mUnk_48 != 0) {
            done = 0;
            ApplyAnimTrack(1, tmpRot, tmpPos, tmpScale, node, &done, n);
            BlendAngles(data_020c31dc.mUnk_48, 4, rot, tmpRot);
            BlendVec3(data_020c31dc.mUnk_48, 4, scale, tmpScale);
            if (data_020c31dc.mUnk_45 == 0 || node > data_020c31dc.mStopNodeIndex) {
                BlendVec3(data_020c31dc.mUnk_48, 4, pos, tmpPos);
            }
        }
        pos[0] = pos[0] >> data_020c31dc.mModelShift;
        pos[1] = pos[1] >> data_020c31dc.mModelShift;
        pos[2] = pos[2] >> data_020c31dc.mModelShift;
    }
    ax = -rot[0];
    az = -rot[2];
    ay = rot[1];
    ix = (ax >> 4) * 2;
    cx = FX_SinCosTable_[ix + 1];
    sx = FX_SinCosTable_[ix];
    iy = (ay >> 4) * 2;
    cy = FX_SinCosTable_[iy + 1];
    sy = FX_SinCosTable_[iy];
    iz = (az >> 4) * 2;
    cz = FX_SinCosTable_[iz + 1];
    sz = FX_SinCosTable_[iz];
    b = (cx * sy) >> 12;
    a = (sx * sy) >> 12;
    r00 = (cy * cz) >> 12;
    r01 = (-cy * sz) >> 12;
    r02 = -sy;
    r10 = ((-a) * cz + cx * sz) >> 12;
    r11 = (a * sz + cx * cz) >> 12;
    r12 = (-sx * cy) >> 12;
    r20 = (b * cz + sx * sz) >> 12;
    r21 = ((-b) * sz + sx * cz) >> 12;
    r22 = (cx * cy) >> 12;
    ix = n->mScaleX * scale[0];
    iy = n->mScaleY * scale[1];
    iz = n->mScaleZ * scale[2];
    m._[0] = (ix * r00) / 4096;
    m._[1] = (iy * r01) / 4096;
    m._[2] = (r02 * iz) / 4096;
    m._[3] = (ix * r10) / 4096;
    m._[4] = (iy * r11) / 4096;
    m._[5] = (iz * r12) / 4096;
    m._[6] = (ix * r20) / 4096;
    m._[7] = (iy * r21) / 4096;
    m._[8] = (iz * r22) / 4096;
    if (node == data_020c31dc.mUnk_42) {
        pos[0] = pos[0] * data_020c31dc.mUnk_40 / 256;
        pos[1] = pos[1] * data_020c31dc.mUnk_40 / 256;
        pos[2] = pos[2] * data_020c31dc.mUnk_40 / 256;
    }
    m._[9] = pos[0];
    m._[10] = pos[1];
    m._[11] = pos[2];
    func_0203bdec(&m, &data_020c31ac, &data_020c31ac);
    if (n->mUnk_f & 3) {
        step = (n->mUnk_f & 2) ? 2 : 1;
        v = (VecFx32 *) &data_020c31ac;
        for (i = 0; i < 3; i += step) {
            s32 x = v->x, y = v->y, z = v->z;
            u32 len = SqrtU64((s64) x * x + (s64) y * y + (s64) z * z);
            v->z = 0;
            v->y = 0;
            v->x = 0;
            ((s32 *) v)[i] = len;
            v += step;
        }
    }
    func_0203fdd8(&data_020c31ac, &gPoseNodeMatrices[node]);
    if (data_020c31dc.mUnk_4a < node) {
        data_020c31dc.mUnk_4a = node;
    }
    if (node == data_020c31dc.mStopNodeIndex) {
        data_020c31dc.mStopNodeIndex = 0xff;
        return;
    }
    if (n->mMtxStoreIndex != -1) {
        if (n->mMtxStoreIndex >= 0x1f) {
            OS_Terminate();
        }
        G3_LoadMtx43(&data_020c31ac);
        reg_G3X_MTX_STORE = n->mMtxStoreIndex;
    }
    if (n->mFirstChild != 0xff) {
        ComputePoseMatrices(-1, n->mFirstChild);
    }
sibling:
    if (n->mNextSibling != 0xff) {
        ComputePoseMatrices(n->mNextSibling, n->mNextSibling);
    }
}

// The translation of node `node`'s matrix combined with gMapState.mUnk_6c (func_0203bdec).
void GetPoseNodePos(VecFx32 *out, int node) {
    MtxFx43 m;

    func_0203bdec(&gPoseNodeMatrices[node], &gMapState.mUnk_6c, &m);
    out->x = m._[9];
    out->y = m._[10];
    out->z = m._[11];
}

// Sets the position, mUnk_28 (a), mUnk_38 (b), the mode in mUnk_43 (c) and mUnk_3e (d), clears mUnk_41, mUnk_3a,
// mUnk_3c and the two flip bits, and sets the three animation masks to all bits (flags bit 8) or none. mUnk_0c,
// mUnk_1c, mUnk_24, mUnk_3f, mUnk_40, mUnk_42 and mUnk_43_5 keep their values.
void SetDrawParams(DrawParams *p, int x, int y, int z, int a, u16 b, u8 c, u8 d, int flags) {
    p->mUnk_41 = 0;
    p->mUnk_28 = a;
    p->mUnk_43_4 = 0;
    p->mUnk_43_3 = p->mUnk_43_4;
    p->mUnk_38 = b;
    p->mUnk_3a = 0;
    p->mUnk_3c = 0;
    p->mUnk_43 = c;
    p->mPos.x = x;
    p->mPos.y = y;
    p->mPos.z = z;
    if (flags & 0x100) {
        p->mUnk_2c = -1;
        p->mUnk_34 = -1;
        p->mUnk_30 = -1;
    } else {
        p->mUnk_2c = 0;
        p->mUnk_34 = 0;
        p->mUnk_30 = 0;
    }
    p->mUnk_3e = d;
}

void func_02023ff0(void) {
    if (gMapState.mCamera.mUnk_2dc.mUnk_4a != 1) {
        VecFx32 pos = {0, 0, 0x1000};
        VecFx32 target = {0, 0, 0};
        VecFx32 up = {0, 0x1000, 0};

        func_0203d0f0(&pos, &up, &target, TRUE, &gMapState.mUnk_3c);
        func_0203ba60(&gMapState.mUnk_3c, &gMapState.mUnk_6c);
        func_0203d08c(0, 0x3000, 0, 0x4000, 0x4000, -0x4000, 0x1000, TRUE, &gMapState.mUnk_9c);
        reg_G3X_MTX_MODE = 3;
        reg_G3X_MTX_IDENTITY = 0;
        reg_G3X_MTX_MODE = 2;
        reg_G3X_MTX_PUSH = 0;
        gMapState.mCamera.mUnk_2dc.mUnk_4a = 1;
    }
}

void func_020240ec(void) {
    if (gMapState.mCamera.mUnk_2dc.mUnk_4a != 2) {
        VecFx32 pos = {0, 0, 0x1000};
        VecFx32 target = {0, 0, 0};
        VecFx32 up = {0, 0x1000, 0};

        func_0203d0f0(&pos, &up, &target, TRUE, NULL);
        func_0203d08c(0, 0xc0, 0, 0x100, 0x400, 0xc8000, 0x1000, TRUE, &gMapState.mUnk_9c);
        reg_G3X_MTX_MODE = 3;
        reg_G3X_MTX_IDENTITY = 0;
        reg_G3X_MTX_MODE = 2;
        reg_G3X_MTX_PUSH = 0;
        gMapState.mCamera.mUnk_2dc.mUnk_4b = 0x1f;
        gMapState.mCamera.mUnk_2dc.mUnk_4c = 0;
        reg_G3X_POLYGON_ATTR = 0x1f00c1;
        reg_G3X_TEXIMAGE_PARAM = 0x20000000;
        reg_G3X_DIF_AMB = 0x7fff0000;
        reg_G3X_SPE_EMI = 0;
        reg_G3X_LIGHT_COLOR = 0x7fff;
        gMapState.mCamera.mUnk_2dc.mUnk_4a = 2;
    }
}

void func_02024208(void) {
    u32 planes;

    G3X_SetClearColor(0, 0, 0x7fff, 0, 0);
    reg_G3X_VIEWPORT = 0xbfff0000;
    reg_GX_DISPCNT |= 0x10008;
    planes = (reg_GX_DISPCNT & 0x1f00) >> 8;
    reg_GX_DISPCNT = (reg_GX_DISPCNT & ~0x1f00) | ((planes | 1) << 8);
    reg_G2_BG0CNT &= ~3;
    reg_G2_BG3CNT = (reg_G2_BG3CNT & ~3) | 3;
    SetLight0(0x1f, 0x1f, 0x1f, -0xffc);
}

void func_020242a8(void *out, int x, int y, int z, int scale, u16 angle, int flags, int w) {
    SetDrawParams(out, x, y, z, scale, angle, 4, flags, w);
}

void func_020242e8(u16 id, int x, int y, int a, int b, u16 c, u8 d, int e) {
    DrawParams params;

    func_020242a8(&params, x, y, a, b, c, d, e);
    DrawModelFileEx(id, &params, NULL, e);
}

void func_02024338(void *a, void *b, int c) {
    UpdateModelScript(a);
    DrawModelFileEx(((Model *) a)->mUnk_170, b, a, c);
}

void func_02024368(Model *obj, int x, int y, int scale, u16 angle, int flags) {
    DrawParams params;
    func_020242a8(&params, x, y, 0, scale, angle, -1, flags);
    UpdateModelScript(obj);
    DrawModelFileEx(obj->mUnk_170, &params, obj, flags);
}

// Texture size code of a power-of-two size, indexed by log2(size) - 3.
static const u8 data_02050898[8] = {0, 1, 2, 3, 4, 5, 6, 7};

void func_020243c0(int id, fx32 x, fx32 y, fx32 z, u16 angle, fx32 sx, fx32 sy, u32 alpha) {
    MtxFx33 rot;
    s32 rect[4];
    TexInfo info;
    s32 clip[4];
    int base;
    u16 *pal;
    int i;
    u16 key;
    int idx;
    int size;
    int s, t;
    int fmt;
    int opaque;
    u32 tex;
    u32 pltt;
    u32 w;
    u32 *hdr;
    int slot;
    int a, b, c, n;
    int col;

    if (!(gSystemState.mFlags & 0x80)) {
        OS_Terminate();
    }
    if (alpha > 31) {
        OS_Terminate();
    }
    if (id == -1) {
        return;
    }
    GetSpriteRect(id, 0, rect);
    if (rect[2] == rect[0] && rect[3] == rect[1]) {
        return;
    }
    func_020240ec();
    if (gMapState.mCamera.mUnk_2dc.mUnk_4b != alpha) {
        reg_G3X_POLYGON_ATTR = (gMapState.mCamera.mUnk_2dc.mUnk_4c << 24) | 0xc1 | (alpha << 16);
        gMapState.mCamera.mUnk_2dc.mUnk_4b = alpha;
    }
    hdr = GetBlockHeader(id);
    w = hdr[0];
    a = w & 0xff;
    b = (w >> 8) & 0xff;
    c = (w >> 16) & 0xff;
    n = (w >> 24) & 0xff;
    hdr += 1 + a + b * 2 + c / 2;
    pal = (u16 *) (hdr - c / 2);
    idx = (u16) -angle >> 4;
    func_0203b998(&rot, FX_SinCosTable_[idx * 2], FX_SinCosTable_[idx * 2 + 1]);
    if (id & 0x10000) {
        sx = -sx;
    }
    if (id & 0x20000) {
        sy = -sy;
    }
    reg_G3X_MTX_MODE = 1;
    reg_G3X_MTX_PUSH = 0;
    PutTrans(x, y, z);
    G3_MultMtx33(&rot);
    PutScale(sx, sy, 0x1000);
    reg_G3X_BEGIN_VTXS = 1;
    for (i = 0; i < n; i++) {
        GetTexInfo(id, &info, i);
        GetSpriteRect(id, 0, clip);
        base = func_0201c538();
        slot = id << 6;
        slot += i;
        col = pal[info.mUnk_14] & 0x7fff;
        key = func_0201c544() + (c ^ (col << 2));
        opaque = pal[info.mUnk_14] >> 15;
        size = info.mWidth * info.mHeight;
        switch (info.mUnk_12) {
            case 1:
            case 4:
                size >>= 1;
                fmt = 3;
                break;
            case 0:
            case 7:
                fmt = 4;
                if (info.mUnk_14 != 0) {
                    OS_Terminate();
                }
                break;
            case 6:
                fmt = 1;
                if (info.mUnk_15 > 2) {
                    OS_Terminate();
                }
                if (info.mUnk_14 != 0) {
                    OS_Terminate();
                }
                break;
            case 5:
                fmt = 6;
                if (info.mUnk_15 != 1) {
                    OS_Terminate();
                }
                break;
        }
        tex = FindOrLoadTex(1, slot + base, size, 0);
        pltt = FindOrLoadPltt(col, key, info.mUnk_15 << 5);
        s = data_02050898[0x1c - LeadingZeroCount(info.mWidth)];
        t = data_02050898[0x1c - LeadingZeroCount(info.mHeight)];
        reg_G3X_TEXIMAGE_PARAM = (tex >> 3) | (fmt << 26) | 0x40000000 | (s << 20) | (t << 23) | ((opaque == 0) << 29);
        reg_G3X_PLTT_BASE = pltt >> (4 - (fmt == 2));
        if (opaque != 0) {
            if (info.mX + info.mWidth > clip[2]) {
                info.mWidth = clip[2] - info.mX + 1;
            }
            if (info.mY + info.mHeight > clip[3]) {
                info.mHeight = clip[3] - info.mY + 1;
            }
        }
        reg_G3X_MTX_MODE = 1;
        reg_G3X_NORMAL = 0x1ff00000;
        PutTexCoord(0, 0);
        PutVtx16(info.mX, info.mY, 0);
        PutTexCoord(info.mWidth << 4, 0);
        PutVtxXY(info.mX + info.mWidth, info.mY);
        PutTexCoord(info.mWidth << 4, info.mHeight << 4);
        PutVtxXY(info.mX + info.mWidth, info.mY + info.mHeight);
        PutTexCoord(0, info.mHeight << 4);
        PutVtxXY(info.mX, info.mY + info.mHeight);
    }
    reg_G3X_END_VTXS = 0;
    reg_G3X_MTX_POP = 1;
}

void DrawModelFile(u32 id, void *p, int flags) {
    DrawModelFileEx(id, p, NULL, flags);
}

void ClearTrackCount(void) {
    data_020c31dc.mTrackFillCount = 0;
}

ModelPart *GetFirstModelPart(int id, int a) {
    u32 *data = FindOrLoadCachedFile(id);
    u8 *p = (u8 *) (data + ((u16) data[0] * 4 + 1));
    p += p[0] * 4 + 2;
    p += p[0] * 2 + 2;
    p += p[0] * 2 + 2;
    return AlignPtrUp4(p + 1);
}

u16 GetAnimKeyAngle(const AnimKey *key, const u8 *table, int t) {
    u16 v;
    int r;
    int off = key->mOffset;
    switch (key->mMode) {
        case 0:
            v = (table + off)[4] << 8;
            break;
        case 1:
            v = (table + (t + off))[4] << 8;
            break;
        case 2:
            table += (t >> 1) + off;
            v = table[4] << 8;
            if ((r = t & 1) != 0) {
                int d = AngleDelta(v, (u16) (table[5] << 8));
                v += d * r / 2;
            }
            break;
        case 3:
            table += (t >> 2) + off;
            v = table[4] << 8;
            if ((r = t & 3) != 0) {
                int d = AngleDelta(v, (u16) (table[5] << 8));
                v += d * r / 4;
            }
            break;
    }
    return v;
}

int GetAnimKeyValue(const AnimKey16 *key, const u8 *table, int t) {
    const s16 *s;
    int v;
    int r;
    switch (key->mMode) {
        case 0:
            return ((const s16 *) (table + 4))[key->mIndex];
        case 1:
            return ((const s16 *) (table + 4))[t + key->mIndex];
        case 2:
            s = (const s16 *) table + ((t >> 1) + key->mIndex);
            v = s[2];
            if (t & 1) {
                return (v + s[3]) >> 1;
            }
            return v;
        case 3:
            s = (const s16 *) table + ((t >> 2) + key->mIndex);
            v = s[2];
            if ((r = t & 3) != 0) {
                v = (v * (4 - r) + s[3] * r) >> 2;
            }
            return v;
    }
}

u32 GetDrawFrame(const DrawParams *p) {
    if (p->mUnk_3e == 0xff) {
        return gSystemState.mElapsedFrames;
    }
    return p->mUnk_3e;
}

s32 func_02024b2c(const u32 *data, const DrawParams *p) {
    int s = ((*data & 0xff0000) >> 16) - data_020c31dc.mModelShift;
    if (s >= 0) {
        return p->mUnk_28 << s;
    }
    return p->mUnk_28 >> -s;
}

// Draws model file `id` with the draw parameters `params`. In modes other than 3 and 5, with a model `obj`: when its
// mUnk_172_0 is set it moves params->mPos to the position of map entry data_020c31dc.mUnk_3c (func_02031fc4) or, when
// that is 0xffff, so that node mUnk_175 lands on mUnk_158, then clears mUnk_172_0 and sets mUnk_172_7; with
// mUnk_172_2 set it adds mUnk_164 to params->mPos; with mUnk_172_3 set it stores node 0's position in mUnk_158.
//
// The table loads in the order cos2, sin2, cos0, sin0, cos1, sin1 give the index and load block, `y` holding
// height << 16 gives the texture scale block, and `cur += 0x38` placed right before each DC_StoreRange keeps the
// argument copy into r0 behind the material and polygon stores. The sh0 statement written after the six table
// loads, with the sy/sz copies before the if, gives the shift block (the `16 << sh0` shift is scheduled after the
// two products and the .bss address load, so the 16 lands in r8). The material colours dif and amb held in the dead
// locals y and x give the material block's registers, and the texture key index held in the dead local sc, declared
// long, gives the stack slots (an int index takes the other slot).
void DrawModelFileEx(u32 id, void *params, void *obj, int flags) {
    DrawParams *p = params;
    Model *model = obj;
    MtxFx43 m;
    VecFx32 pos;
    VecFx32 ofs;
    const u16 *keys[5];
    const u16 *mkeys[5];
    u32 *res;
    ModelTexAnims *texAnim;
    ModelTracks *srtAnim;
    ModelTracks *matAnim;
    int q;
    u8 *p4;
    int count;
    u32 lightOfs;
    int angOfs;
    ModelPart *mesh;
    long sc;
    ModelPart *src;
    int mode;
    u16 ang;
    s32 x, y;
    int sh0, sh1;
    int part;
    s32 cos2, sin2, cos0, sin0, cos1, sin1;
    const ModelKeyTrack *track;
    int cull;
    s32 sx, sy, sz;
    int a0, a1, a2;
    s32 tx, ty;
    s32 c2s0, s2s0;
    int e;
    u32 w0;
    u32 polyFlags;
    int polyId;
    int alpha;
    u32 visMask;
    int sizeS, fmt, sizeT;
    int width, height;
    u32 tex, pltt;
    u32 texMask, srtMask;
    int k;
    int frame;
    s32 tsx, tsy;
    const u16 *kp;
    const ModelFrameAnim *anim;
    const u16 *anim0;
    int pltt0;
    int rot;
    s32 hw, hh;
    long idx;
    u16 spe, emi;
    u32 se;
    u8 *cur;

    angOfs = 0;
    lightOfs = 0;
    if (!(gSystemState.mFlags & 0x80)) {
        OS_Terminate();
    }
    res = FindOrLoadCachedFile(id);
    data_020c31dc.mPoseNodes = (PoseNode *) (res + 1);
    data_020c31dc.mUnk_42 = 0xff;
    data_020c31dc.mUnk_4a = 0;
    if (p->mUnk_43 >= 4) {
        if (gMapState.mCamera.mUnk_2dc.mUnk_4a != 1) {
            func_02023ff0();
        }
        angOfs = 0x8000;
    } else if (gMapState.mCamera.mUnk_2dc.mUnk_4a != 0) {
        func_020334f8(0);
    }
    mode = p->mUnk_43;
    if (mode != 3 && mode != 5) {
        ang = ((*res & 0xff000000) >> 16) - p->mUnk_38;
        if (mode != 1) {
            a0 = ang >> 4;
            a1 = (u16) (angOfs + p->mUnk_3a) >> 4;
            a2 = p->mUnk_3c >> 4;
            cos2 = FX_SinCosTable_[a2 * 2 + 1];
            sin2 = FX_SinCosTable_[a2 * 2];
            cos0 = FX_SinCosTable_[a0 * 2 + 1];
            sin0 = FX_SinCosTable_[a0 * 2];
            cos1 = FX_SinCosTable_[a1 * 2 + 1];
            sin1 = FX_SinCosTable_[a1 * 2];
            sh0 = (*res & 0xff0000) >> 16;
            c2s0 = (cos2 * sin0) >> 12;
            s2s0 = (sin2 * sin0) >> 12;
            data_020c31dc.mModelShift = sh0;
            sx = 16 << sh0;
            sy = sx;
            sz = sx;
            if (sh0 != 0) {
                sh0 = sh0 - 1;
                sh1 = 19 - sh0;
            } else {
                sh0 = 0;
                sh1 = 19;
            }
            if (p->mUnk_43_3) {
                sx = -sx;
            }
            if (p->mUnk_43_4) {
                sz = -sy;
            }
            m._[2] = (sz * -sin0) >> 7;
            m._[0] = (sx * ((cos0 * cos1) >> sh0)) >> sh1;
            m._[1] = (sy * ((-cos0 * sin1) >> sh0)) >> sh1;
            m._[3] = (sx * ((-s2s0 * cos1 + cos2 * sin1) >> sh0)) >> sh1;
            m._[4] = (sy * ((s2s0 * sin1 + cos2 * cos1) >> sh0)) >> sh1;
            m._[8] = (sz * ((cos2 * cos0) >> sh0)) >> sh1;
            m._[5] = (sz * ((-sin2 * cos0) >> sh0)) >> sh1;
            m._[6] = (sx * ((c2s0 * cos1 + sin2 * sin1) >> sh0)) >> sh1;
            m._[7] = (sy * ((-c2s0 * sin1 + sin2 * cos1) >> sh0)) >> sh1;
            switch (p->mUnk_43) {
                case 0:
                    m._[9] = p->mPos.x;
                    m._[10] = p->mPos.y;
                    m._[11] = p->mPos.z;
                    func_0203bdec(&m, &gMapState.mUnk_3c, &data_020c31ac);
                    break;
                case 2:
                    m._[9] = p->mPos.x;
                    m._[10] = p->mPos.y;
                    m._[11] = p->mPos.z;
                    func_0203bdec(&m, &gMapState.mUnk_3c, &data_020c31ac);
                    func_0203ba2c(&data_020c31ac, &data_020c31ac, p->mUnk_28, p->mUnk_28, p->mUnk_28);
                    if (p->mUnk_42 != -1 && p->mUnk_28 != 0x1000) {
                        data_020c31dc.mUnk_42 = p->mUnk_42;
                        data_020c31dc.mUnk_40 = 0x100000 / p->mUnk_28;
                    }
                    lightOfs = p->mUnk_41;
                    break;
                case 4:
                    m._[9] = p->mPos.x << 6;
                    m._[10] = p->mPos.y << 6;
                    m._[11] = p->mPos.z;
                    sc = (p->mUnk_28 << 2) / 32;
                    func_0203ba2c(&m, &data_020c31ac, sc, sc, sc);
                    lightOfs = p->mUnk_41;
                    break;
            }
        } else {
            sc = func_02024b2c(res, p);
            func_0203ba2c(&gPoseNodeMatrices[p->mUnk_38], &data_020c31ac, sc, sc, sc);
        }
        if (model != NULL) {
            data_020c31dc.mUnk_45 = model->mUnk_172_0 | model->mUnk_172_7;
            data_020c31dc.mUnk_47 = model->mUnk_172_8;
            if (model->mUnk_172_0 == 1) {
                if (data_020c31dc.mUnk_3c == 0xffff) {
                    data_020c31dc.mStopNodeIndex = model->mUnk_175;
                    ComputePoseMatrices(-1, 0);
                    GetPoseNodePos(&pos, model->mUnk_175);
                    p->mPos.x -= pos.x - model->mUnk_158.x;
                    p->mPos.y -= pos.y - model->mUnk_158.y;
                    p->mPos.z -= pos.z - model->mUnk_158.z;
                } else {
                    func_02031fc4(data_020c31dc.mUnk_3c, &p->mPos);
                }
                m._[9] = p->mPos.x;
                m._[10] = p->mPos.y;
                m._[11] = p->mPos.z;
                func_0203bdec(&m, &gMapState.mUnk_3c, &data_020c31ac);
                if (gMapState.mCamera.mUnk_2dc.mUnk_4a == 1) {
                    func_0203ba2c(&data_020c31ac, &data_020c31ac, 0x80, 0x80, 0x80);
                }
                model->mUnk_172_0 = 0;
                model->mUnk_172_7 = 1;
            }
            if (model->mUnk_172_2 == 1) {
                p->mPos.x += model->mUnk_164.x;
                p->mPos.y += model->mUnk_164.y;
                p->mPos.z += model->mUnk_164.z;
            }
        } else {
            data_020c31dc.mUnk_45 = 0;
            data_020c31dc.mUnk_47 = 0;
        }
        if (model != NULL && (model->mUnk_172_6 | model->mUnk_172_5) == 1) {
            data_020c31dc.mUnk_48 = 0;
        }
        ComputePoseMatrices(-1, 0);
        if (model != NULL && model->mUnk_172_3 == 1) {
            GetPoseNodePos(&ofs, 0);
            model->mUnk_158.x = ofs.x;
            model->mUnk_158.y = ofs.y;
            model->mUnk_158.z = ofs.z;
        }
    } else {
        sc = func_02024b2c(res, p);
        G3_LoadMtx43(&gPoseNodeMatrices[p->mUnk_38]);
        PutScale(sc, sc, sc);
        reg_G3X_MTX_STORE = 0;
    }
    texAnim = (ModelTexAnims *) (res + ((u16) res[0] * 4 + 1));
    srtAnim = (ModelTracks *) ((u8 *) texAnim + (texAnim->mCount * 4 + 2));
    matAnim = (ModelTracks *) ((u8 *) srtAnim + (srtAnim->mCount * 2 + 2));
    p4 = (u8 *) matAnim + (matAnim->mCount * 2 + 2);
    count = p4[0];
    cur = AlignPtrUp4(p4 + 1);
    if (model != NULL && model->mUnk_172_4 == 1) {
        count = 0;
    }
    part = 0;
    while (count-- != 0) {
        mesh = (ModelPart *) cur;
        if ((p->mUnk_24 & (1 << part)) && (gSystemState.mFlags & 0x8000)) {
            src = GetFirstModelPart(p->mUnk_0c[p->mUnk_1c[part]], 0);
        } else {
            src = mesh;
        }
        visMask = p->mUnk_34 & mesh->mMatAnimMask;
        polyFlags = 0;
        w0 = mesh->mUnk_00;
        pltt0 = (w0 & 0x10) ? 1 : 0;
        alpha = (w0 >> 23) & 0x1f;
        polyId = (lightOfs + (w0 >> 16)) & 0x3f;
        if (!(flags & 1)) {
            polyFlags = 0x8000;
        }
        if (flags & 2) {
            alpha = (u32) flags >> 27;
        }
        if (flags & 0x40) {
            polyId = (u32) flags >> 26;
        }
        if (flags & 0x1000) {
            polyFlags |= 0x800;
        }
        if (mesh->mTexFormat != 0) {
            fmt = mesh->mTexFormat & 7;
            sizeS = mesh->mUnk_2a & 0xf;
            sizeT = mesh->mUnk_2a >> 4;
            width = 8 << sizeS;
            height = 8 << sizeT;
            texMask = p->mUnk_2c & mesh->mTexAnimMask;
            srtMask = p->mUnk_30 & mesh->mUnk_08;
            if (texMask != 0 && (model == NULL || model->mUnk_172_12 == 0)) {
                sc = 31 - LeadingZeroCount(texMask);
                anim = (const ModelFrameAnim *) ((u32) texAnim + 2 + texAnim->mEntries[sc].mOffset * 2);
                if (flags & 0x20) {
                    frame = (u32) flags >> 26;
                } else {
                    if ((flags & 0x200) && p->mUnk_3f != 0xff) {
                        frame = p->mUnk_3f;
                    } else if (model != NULL && (model->mUnk_14c.mUnk_0 != 0 || model->mUnk_172_11 == 1)) {
                        frame = model->mUnk_14c.mUnk_2 / model->mUnk_14c.mUnk_4;
                    } else {
                        frame = GetDrawFrame(p);
                    }
                    frame = ((u32) (frame << 8) / texAnim->mEntries[sc].mDiv) % anim->mCount;
                }
                tex = FindOrLoadTex(anim->mFrames[frame].mTexSrcLow + (anim->mFrames[frame].mTexSrcHigh << 16),
                                    anim->mFrames[frame].mTexKey, GetTexByteSize(sizeS, sizeT, fmt),
                                    anim->mFrames[frame].mTexStoredSize);
                if (mesh->mPlttSrc != 0) {
                    pltt = FindOrLoadPltt(anim->mFrames[frame].mPlttSrcLow + (anim->mFrames[frame].mPlttSrcHigh << 16),
                                          anim->mFrames[frame].mPlttKey, mesh->mPlttSize);
                }
            } else {
                tex = FindOrLoadTex(src->mTexSrc, src->mTexKey, GetTexByteSize(sizeS, sizeT, fmt), src->mTexStoredSize);
                if (src->mPlttSrc != 0) {
                    pltt = FindOrLoadPltt(src->mPlttSrc, src->mPlttKey, src->mPlttSize);
                }
            }
            reg_G3X_TEXIMAGE_PARAM = (tex >> 3) | (fmt << 26) | 0x40000000 | (sizeS << 20) | (sizeT << 23) |
                                     (((mesh->mUnk_00 & 0xc00) >> 10) << 16) | (((mesh->mUnk_00 & 0x60) >> 5) << 18) |
                                     (pltt0 << 29);
            if (mesh->mPlttSrc != 0) {
                reg_G3X_PLTT_BASE = pltt >> (4 - (fmt == 2));
            }
            reg_G3X_MTX_MODE = 3;
            reg_G3X_MTX_IDENTITY = 0;
            if (srtMask != 0 && (model == NULL || model->mUnk_172_12 == 0)) {
                k = 31 - LeadingZeroCount(srtMask);
                track = (const ModelKeyTrack *) ((u32) srtAnim + 2 + srtAnim->mOffsets[k] * 2);
                kp = (const u16 *) ((const u8 *) track + track->mKeyOffset + 4);
                if (srtAnim->mCount <= k) {
                    OS_Terminate();
                }
                if ((flags & 0x200) && (frame = p->mUnk_3e) != 0xff) {
                } else if (model != NULL) {
                    frame = model->mUnk_14c.mUnk_2 - 1;
                    if (frame < 0) {
                        frame += GetAnimFrameCount(model->mUnk_14c.mUnk_0) * model->mUnk_14c.mUnk_4;
                    }
                    frame = frame / model->mUnk_14c.mUnk_4;
                } else {
                    frame = GetDrawFrame(p) % track->mCount;
                }
                for (q = 0; q < 5; q++) {
                    keys[q] = kp;
                    kp++;
                }
                rot = GetAnimKeyAngle((const AnimKey *) keys[2], (const u8 *) track, frame);
                if (rot != 0) {
                    hw = (width << 16) / 2;
                    hh = (height << 16) / 2;
                    idx = (rot >> 4) * 2;
                    PutTrans(hw, hh, 0);
                    func_0203d13c(FX_SinCosTable_[idx], FX_SinCosTable_[idx + 1]);
                    PutTrans(-hw, -hh, 0);
                }
                tsx = GetAnimKeyValue((const AnimKey16 *) keys[0], (const u8 *) track, frame);
                tsy = GetAnimKeyValue((const AnimKey16 *) keys[1], (const u8 *) track, frame);
                if (tsx != 0x1000 || tsy != 0x1000) {
                    y = height << 16;
                    PutTrans(0, y, 0);
                    PutScale(tsx, tsy, 0x1000);
                    PutTrans(0, -y, 0);
                }
                tx = GetAnimKeyValue((const AnimKey16 *) keys[3], (const u8 *) track, frame);
                ty = GetAnimKeyValue((const AnimKey16 *) keys[4], (const u8 *) track, frame);
                PutTrans((-tx * width) << 4, (ty * height) << 4, 0);
            } else {
                if ((mesh->mUnk_30 | mesh->mUnk_32) != 0) {
                    x = !(mesh->mUnk_00 & 0x20) ? width * mesh->mUnk_30 / 128
                                                : mesh->mUnk_2c * width / 16 * mesh->mUnk_30 / 128;
                    y = !(mesh->mUnk_00 & 0x40) ? height * mesh->mUnk_32 / 128
                                                : -mesh->mUnk_2d * height / 16 * mesh->mUnk_32 / 128;
                    PutTrans(x << 16, y << 16, 0x10000);
                }
                if (mesh->mUnk_2c != 16 || mesh->mUnk_2d != 16) {
                    PutTrans((width * (mesh->mUnk_2c - 16)) << 16, (height * (mesh->mUnk_2d - 16) / 16) << 16, 0);
                    PutScale((mesh->mUnk_2c << 12) / 16, (mesh->mUnk_2d << 12) / 16, 0x1000);
                }
            }
            reg_G3X_MTX_MODE = 2;
        } else {
            reg_G3X_TEXIMAGE_PARAM = 0;
        }
        if (visMask != 0 && (model == NULL || model->mUnk_172_12 == 0)) {
            k = 31 - LeadingZeroCount(visMask);
            track = (const ModelKeyTrack *) ((u32) matAnim + 2 + matAnim->mOffsets[k] * 2);
            kp = (const u16 *) ((const u8 *) track + track->mKeyOffset + 4);
            if (matAnim->mCount <= k) {
                OS_Terminate();
            }
            if ((flags & 0x200) && p->mUnk_40 != 0xff) {
                frame = p->mUnk_40;
            } else if (model != NULL) {
                frame = model->mUnk_14c.mUnk_2 / model->mUnk_14c.mUnk_4;
            } else {
                frame = GetDrawFrame(p) % track->mCount;
            }
            for (q = 0; q < 5; q++) {
                mkeys[q] = kp;
                kp++;
            }
            if (!(flags & 2)) {
                alpha = MatKeyByte((const MatKey *) mkeys[4], track, frame);
                if (alpha == 0) {
                    cur = cur + 0x38 + (mesh->mCommandSize & ~3);
                    continue;
                }
            }
            y = (u16) MatKeyValue((const MatKey *) mkeys[0], track, frame);
            x = (u16) MatKeyValue((const MatKey *) mkeys[1], track, frame);
            spe = MatKeyValue((const MatKey *) mkeys[2], track, frame);
            emi = MatKeyValue((const MatKey *) mkeys[3], track, frame);
        } else {
            x = mesh->mDifAmbHigh;
            y = mesh->mDifAmbLow;
            spe = mesh->mSpeEmiLow;
            emi = mesh->mSpeEmiHigh;
        }
        if (flags & 4) {
            PutPolygonAttr(gMapState.mCamera.mUnk_2dc.mLightMask, 3, 1, 0, alpha, 0);
            PutMaterial(y | (((flags & 0x10) ? 0x7fff : x) << 16), spe | (emi << 16));
            cur += 0x38;
            DC_StoreRange(cur, mesh->mCommandSize);
            func_01ff831c(3, cur, mesh->mCommandSize);
            PutPolygonAttr(gMapState.mCamera.mUnk_2dc.mLightMask, 3, 3, 8, alpha, 0);
            if (flags & 0x10) {
                x = 0x7fff;
            }
            PutMaterial(y | (x << 16) | 0x8000, spe | (emi << 16));
            func_01ff831c(3, cur, mesh->mCommandSize);
        } else {
            if (!(p->mUnk_43_3 ^ p->mUnk_43_4)) {
                cull = (mesh->mUnk_00 >> 7) & 3;
            } else {
                cull = ((mesh->mUnk_00 & 0x80) >> 6) + ((mesh->mUnk_00 & 0x100) >> 8);
            }
            if (flags & 0x10) {
                x = 0x7fff;
            }
            PutMaterial(y | (x << 16) | 0x8000, spe | (emi << 16));
            PutPolygonAttr(gMapState.mCamera.mUnk_2dc.mLightMask, (flags & 8) ? 2 : 0, cull, polyId, alpha, polyFlags);
            cur += 0x38;
            DC_StoreRange(cur, mesh->mCommandSize);
            func_01ff831c(3, cur, mesh->mCommandSize);
        }
        part++;
        cur += mesh->mCommandSize & ~3;
    }
    ClearAnimTracks();
}

void ProjectToScreen(void *pos, s32 *out) {
    ProjectToScreenEx(pos, out, 0);
}

// Projects `pos` through gMapState.mUnk_3c to the screen, centred on 128, 96. With steps 0 out receives the point;
// otherwise out (in 1/0x1000 pixels) moves 1/steps of the way to it and jumps there when the distance is below
// `steps` or 0xc000 or more. That path does not run in this build: the only caller, ProjectToScreen, passes 0.
void ProjectToScreenEx(const VecFx32 *pos, s32 *out, int steps) {
    VecFx32 v;
    fx32 w;
    s32 x, y, z;
    reg_G3X_MTX_MODE = 1;
    G3_LoadMtx43(&gMapState.mUnk_3c);
    z = pos->z;
    y = pos->y;
    x = pos->x;
    reg_G3X_MTX_TRANS = x;
    reg_G3X_MTX_TRANS = y;
    reg_G3X_MTX_TRANS = z;
    reg_G3X_POS_TEST = 0;
    reg_G3X_POS_TEST = 0;
    while (func_0203cf5c(&v, &w) != 0) {
    }
    if (steps == 0) {
        s32 sy = (-v.y * 192) / (w << 1) + 96;
        s32 sx = (v.x << 8) / (w << 1) + 128;
        out[0] = sx;
        out[1] = sy;
    } else {
        s32 dx, nx, d;
        s32 dy, ny;
        d = (w << 1) >> 12;
        nx = (v.x << 8) / d + 0x80000;
        ny = (-v.y * 192) / d + 0x60000;
        dx = nx - out[0];
        dy = ny - out[1];
        if (abs(dx) >= steps && abs(dx) < 0xc000) {
            out[0] += dx / steps;
        } else {
            out[0] = nx;
        }
        if (abs(dy) >= steps && abs(dy) < 0xc000) {
            out[1] += dy / steps;
        } else {
            out[1] = ny;
        }
    }
}

int GetTexByteSize(int log2w, int log2h, int format) {
    u8 bits[8] = {0, 8, 2, 4, 8, 0, 8, 16};
    return ((8 << log2w) * (8 << log2h) * bits[format]) / 8;
}

#pragma thumb on

int GetAnimFrameCount(u16 id) {
    u16 *p = FindOrLoadCachedFile(id);
    if (p[2] == 1) {
        return p[1];
    }
    if (p[2] == 2) {
        return p[1] >> 1;
    }
    return p[1] / p[2];
}

int GetTrackFrameCount(Model *m, int track) {
    return GetAnimFrameCount(m->mAnim[track].mUnk_0);
}

// Sets the frame of animation 0 (mUnk_14c is mAnim[0]) to n; GetModelFrameIndex reads it back.
void SetModelFrameIndex(Model *m, int n) {
    m->mUnk_14c.mUnk_2 = n * m->mUnk_14c.mUnk_4;
}

int GetModelFrameIndex(Model *m) {
    return m->mUnk_14c.mUnk_2 / m->mUnk_14c.mUnk_4;
}

void SplitAnimFrameAndAdvance(u16 id, u16 *frame, u32 count, int step, s32 *whole, s32 *next, s32 *rest, int loop) {
    u16 saved = *frame;
    *whole = DivideWithRemainder(*frame, count, (u32 *) rest);
    *next = *whole + 1;
    *rest = (*rest << 8) / count;
    if (step != 0) {
        *frame += step;
        if (*frame >= GetAnimFrameCount(id) * count) {
            if (loop == 1) {
                *frame = 0;
            } else {
                *frame = saved;
            }
        }
    }
}

int func_02026080(Model *m) {
    u16 *q = FindOrLoadCachedFile(m->mUnk_170);
    u16 *r;
    int scale, limit, count;
    q = (u16 *) ((u8 *) q + ((*(u32 *) q & 0xffff) * 4 + 1) * 4);
    r = q + q[1];
    limit = r[1];
    count = m->mUnk_14c.mUnk_4;
    scale = q[2];
    if ((u32) ((m->mUnk_14c.mUnk_2 << 8) / (scale * count)) >= limit) {
        s32 d = (limit * scale * count) / 256;
        if (m->mUnk_172_13 == 0) {
            m->mUnk_14c.mUnk_2 -= d;
        } else {
            m->mUnk_14c.mUnk_2 = d - 1;
        }
        return 1;
    }
    return 0;
}

int func_020260f4(Model *m, u32 track) {
    ModelAnim *a = &m->mAnim[track];
    AnimTrack *t = &data_020c31dc.mAnimTracks[track];
    u32 pos;
    u32 rest;
    if (a->mUnk_0 != 0) {
        t->mData = FindOrLoadCachedFile(a->mUnk_0);
        t->mFlags = a->mUnk_5;
        if (a->mUnk_4 == 2) {
            t->mFrameIndex = (u32) a->mUnk_2 >> 1;
            t->mNextWeight = (a->mUnk_2 & 1) << 7;
        } else {
            t->mFrameIndex = DivideWithRemainder(a->mUnk_2, a->mUnk_4, &rest);
            t->mNextWeight = (rest << 8) / a->mUnk_4;
        }
        data_020c31dc.mTrackFillCount++;
        pos = t->mFrameIndex;
        if (pos == GetTrackFrameCount(m, track) - 1) {
            t->mNextWeight = 0;
            if (a->mUnk_5 & 0x10) {
                a->mUnk_0 = 0;
                return 1;
            }
            if (a->mUnk_5 & 8) {
                return 1;
            }
        } else {
            t->mNextFrameIndex = pos + 1;
        }
        if (track < 1) {
            if (!(a->mUnk_5 & 0x20) && !(gSystemState.mFlags & 0x80000000) && !m->mUnk_172_12) {
                a->mUnk_2++;
            }
            if (m->mUnk_174 == 0) {
                m->mUnk_172_6 = 0;
                m->mUnk_172_7 = 0;
            }
            if (a->mUnk_2 == GetTrackFrameCount(m, track) * a->mUnk_4 - 1) {
                if (m->mUnk_172_13 == 1) {
                    a->mUnk_5 |= 0x20;
                    a->mUnk_2--;
                }
                return 1;
            }
            if (a->mUnk_2 == GetTrackFrameCount(m, track) * a->mUnk_4) {
                a->mUnk_2 = 0;
            }
        }
    } else {
        t->mData = NULL;
        if (m->mUnk_172_11 == 1) {
            if (m->mUnk_172_9 == 0) {
                m->mUnk_14c.mUnk_2++;
            }
            return func_02026080(m);
        }
    }
    return 0;
}

int func_02026268(Model *m, u32 track) {
    int r = func_020260f4(m, track);
    if (r == 0 && gSystemState.mUnk_48c > 0 && (gSystemState.mFlags & 0x40)) {
        r |= func_020260f4(m, track);
    }
    return r;
}

void UpdateModelScript(Model *m) {
    u32 mask = m->mUnk_134 & m->mUnk_130 & ~m->mUnk_138;
    if (mask != 0) {
        int best = -1;
        int i = 0;
        u32 bestKey;
        if (mask != 0) {
            u32 cur = m->mUnk_142;
            for (; mask != 0; mask >>= 1) {
                u32 key = m->mUnk_028[i].mUnk_4;
                if ((mask & 1) && key != cur && (key >> 8) >= (cur >> 8)) {
                    if (best == -1 || (key >> 8) >= (bestKey >> 8)) {
                        bestKey = key;
                        best = i;
                    }
                }
                i++;
            }
        }
        if (best != -1) {
            if (m->mUnk_024 & 4) {
                m->mUnk_000 = m->mUnk_028[best].mUnk_0;
                m->mUnk_13c = m->mUnk_028[32].mUnk_0;
            } else {
                m->mUnk_13c = m->mUnk_028[best].mUnk_0;
            }
        }
    }
    ExecModelScript(m);
    m->mUnk_134 = 0;
}

void func_0202635c(Model *m, s32 *slot) {
    m->mAnim[1] = m->mAnim[0];
    *slot = 0;
}

void StartModelAnim(Model *m, s32 *slot, int id, int count, int flags, int mode) {
    int ratio;
    if (*slot == 0) {
        ratio = GetModelFrameIndex(m);
        ClearAnimTracks();
        func_020271e0(m);
        if (m->mUnk_172_1 == 1) {
            u16 *hdr = FindOrLoadCachedFile(id);
            VecFx32 target;
            func_02031fc4(data_020c31dc.mUnk_3e, &target);
            m->mUnk_164.x = (target.x - m->mUnk_158.x) / (hdr[1] * count);
            m->mUnk_164.y = (target.y - m->mUnk_158.y) / (hdr[1] * count);
            m->mUnk_164.z = (target.z - m->mUnk_158.z) / (hdr[1] * count);
            m->mUnk_172_2 = 1;
            m->mUnk_172_1 = 0;
        } else {
            m->mUnk_172_2 = 0;
        }
        if (id != m->mUnk_152.mUnk_0 && m->mUnk_152.mUnk_0 != 0) {
            if (flags & 0x20) {
                m->mUnk_174 = 1;
            } else {
                m->mUnk_174 = 0x10;
            }
        }
    }
    if (count == 0) {
        OS_Terminate();
    }
    m->mAnim[*slot].mUnk_0 = id;
    m->mAnim[*slot].mUnk_4 = count;
    m->mAnim[*slot].mUnk_5 = flags & ~0x20;
    m->mAnim[*slot].mUnk_2 = 0;
    m->mUnk_172_10 = 0;
    if (*slot == 0) {
        if (mode < 0) {
            SetModelFrameIndex(m, ratio);
        } else {
            SetModelFrameIndex(m, mode);
        }
    }
}

int GetModelScriptReg(Model *m, u16 id) {
    id &= 0x1f;
    if (id > 0xf) {
        if (id == 0x1f) {
            return NextRandom();
        }
        if (id == 0x1e) {
            return m->mUnk_142;
        }
        if (id == 0x17) {
            return m->mUnk_14c.mUnk_0;
        }
        if (id == 0x1d) {
            return m->mUnk_170;
        }
        if (id == 0x1c) {
            return m->mUnk_130;
        }
        if (id == 0x19) {
            return m->mUnk_134;
        }
        if (id == 0x1b) {
            return m->mUnk_175;
        }
        if (id == 0x1a) {
            return m->mUnk_138;
        }
        if (id == 0x18) {
            return (m->mUnk_172_4 << 4) + (((m->mUnk_024 & 3) << 2) + (m->mUnk_172_5 + (m->mUnk_172_7 << 1)));
        }
        if (id == 0x16) {
            return gSystemState.mUnk_496;
        }
        if (id > 0xf) {
            OS_Terminate();
        }
    }
    return m->mUnk_004_all[id];
}

void SetModelScriptReg(Model *m, u16 id, u32 value) {
    id &= 0x1f;
    if (id > 0xf) {
        if (id == 0x1e) {
            if (m->mUnk_140 != value) {
                m->mUnk_140 = m->mUnk_142;
            }
            m->mUnk_142 = value;
            m->mUnk_149 = m->mUnk_148;
            m->mUnk_138 = 0;
            m->mUnk_148 = 0;
            return;
        }
        if (id == 0x17) {
            m->mUnk_14c.mUnk_0 = value;
            return;
        }
        if (id == 0x1d) {
            if (m->mUnk_170 != value) {
                m->mUnk_172_6 = 1;
                m->mUnk_170 = value;
            }
            return;
        }
        if (id == 0x1c) {
            m->mUnk_130 = value;
            return;
        }
        if (id == 0x1b) {
            m->mUnk_175 = value;
            return;
        }
        if (id == 0x1a) {
            m->mUnk_138 = value;
            return;
        }
        if (id == 0x19) {
            m->mUnk_134 = value;
            return;
        }
        if (id == 0x18) {
            m->mUnk_172_5 = value & 1;
            m->mUnk_172_7 = (value >> 1) & 1;
            m->mUnk_172_4 = (value >> 4) & 1;
            return;
        }
        if (id == 0x16) {
            gSystemState.mUnk_496 = value;
            return;
        }
        if (id > 0xf) {
            OS_Terminate();
        }
    }
    m->mUnk_004_all[id] = value;
}

void func_020266a8(Model *m, s32 *slot) {
    switch (m->mUnk_14c.mUnk_0) {
        default:
            m->mUnk_172_10 |= func_02026268(m, 0);
            break;
        case 0:
            m->mUnk_172_10 |= func_02026268(m, 0);
            break;
    }
    if (m->mUnk_174 != 0) {
        u8 t;
        func_02026268(m, 1);
        t = m->mUnk_174;
        m->mUnk_174 = t - 1;
        data_020c31dc.mUnk_48 = t;
    } else {
        func_0202635c(m, slot);
    }
}

// Runs the model's command list at mUnk_13c until a command stops it. A command is a 16-bit word (bits 10-15 the
// command, bits 8-9 a mode that selects how the operand is given, bits 0-7 a signed argument), mostly followed by a
// 16-bit operand word. In the register commands the argument names the register (read with GetModelScriptReg, written
// with SetModelScriptReg); mode 2 takes the operand word as a value, mode 1 as a register number, and mode 0 takes the
// value from bits 5-7 of the argument and has no operand word. mUnk_024 bit 0 is the equal flag, bit 1 the less flag.
int ExecModelScript(Model *m) {
    s32 slot;
    BOOL cont;
    if (m->mUnk_13c == NULL) {
        m->mUnk_172_10 = func_02026268(m, 0) | m->mUnk_172_10;
    } else {
        cont = TRUE;
        slot = 0;
        m->mUnk_172_9 = 0;
        m->mUnk_172_13 = 0;
        m->mUnk_172_0 = 0;
        do {
            u16 op = m->mUnk_13c[0];
            s8 arg = op;
            u16 mode = (op & 0x300) >> 8;
            s16 val = m->mUnk_13c[1];
            s32 step = 2;
            s32 v;
            switch ((u16) (op >> 10)) {
                // Jump by the offset.
                case MC_JUMP:
                    if (mode == MM_VALUE) {
                        val = arg;
                    }
                    m->mUnk_13c += val;
                    break;
                // MM_BITS returns to the saved position; MM_VALUE saves the next position and jumps.
                case MC_CALL:
                    if (mode == MM_BITS) {
                        m->mUnk_13c = m->mUnk_000;
                    } else if (mode == MM_VALUE) {
                        m->mUnk_000 = m->mUnk_13c + 2;
                        m->mUnk_13c += val;
                    }
                    break;
                // Jump if the equal flag (bit 0) is set.
                case MC_JUMP_IF_EQUAL:
                    if (mode == MM_VALUE) {
                        step = 1;
                        val = arg;
                    }
                    if (m->mUnk_024 & 1) {
                        m->mUnk_13c += val;
                    } else {
                        m->mUnk_13c += step;
                    }
                    break;
                // Jump if the less flag (bit 1) is clear.
                case MC_JUMP_IF_NOT_LESS:
                    if (mode == MM_VALUE) {
                        step = 1;
                        val = arg;
                    }
                    if (!(m->mUnk_024 & 2)) {
                        m->mUnk_13c += val;
                    } else {
                        m->mUnk_13c += step;
                    }
                    break;
                // Jump if the less flag is clear or the equal flag is set.
                case MC_JUMP_IF_NOT_LESS_OR_EQUAL:
                    if (mode == MM_VALUE) {
                        step = 1;
                        val = arg;
                    }
                    if (!(m->mUnk_024 & 2) || (m->mUnk_024 & 1)) {
                        m->mUnk_13c += val;
                    } else {
                        m->mUnk_13c += step;
                    }
                    break;
                // Jump if the less flag is set.
                case MC_JUMP_IF_LESS:
                    if (mode == MM_VALUE) {
                        step = 1;
                        val = arg;
                    }
                    if (m->mUnk_024 & 2) {
                        m->mUnk_13c += val;
                    } else {
                        m->mUnk_13c += step;
                    }
                    break;
                // Jump if either flag is set.
                case MC_JUMP_IF_EITHER:
                    if (mode == MM_VALUE) {
                        step = 1;
                        val = arg;
                    }
                    if (m->mUnk_024 & 3) {
                        m->mUnk_13c += val;
                    } else {
                        m->mUnk_13c += step;
                    }
                    break;
                // Jump if the equal flag is clear.
                case MC_JUMP_IF_NOT_EQUAL:
                    if (mode == MM_VALUE) {
                        step = 1;
                        val = arg;
                    }
                    if (!(m->mUnk_024 & 1)) {
                        m->mUnk_13c += val;
                    } else {
                        m->mUnk_13c += step;
                    }
                    break;
                // Test a bit of a register: the equal flag is set when the bit is set.
                case MC_TEST_BIT:
                    m->mUnk_024 &= ~3;
                    if (mode == MM_REG) {
                        val = GetModelScriptReg(m, val);
                    } else if (mode == MM_BITS) {
                        val = (u8) arg >> 5;
                        step = 1;
                    }
                    if (GetModelScriptReg(m, arg) & (1 << val)) {
                        m->mUnk_024 |= 1;
                    }
                    m->mUnk_13c += step;
                    break;
                // Test a bit of a register, then clear it.
                case MC_TEST_CLEAR_BIT: {
                    m->mUnk_024 &= ~3;
                    if (mode == MM_REG) {
                        val = GetModelScriptReg(m, val);
                    } else if (mode == MM_BITS) {
                        val = (u8) arg >> 5;
                        step = 1;
                    }
                    v = GetModelScriptReg(m, arg);
                    if (v & (1 << val)) {
                        m->mUnk_024 |= 1;
                    }
                    v &= ~(1 << val);
                    SetModelScriptReg(m, arg, v);
                    m->mUnk_13c += step;
                    break;
                }
                // Test a bit of a register, then set it.
                case MC_TEST_SET_BIT: {
                    m->mUnk_024 &= ~3;
                    if (mode == MM_REG) {
                        val = GetModelScriptReg(m, val);
                    } else if (mode == MM_BITS) {
                        val = (u8) arg >> 5;
                        step = 1;
                    }
                    v = GetModelScriptReg(m, arg);
                    if (v & (1 << val)) {
                        m->mUnk_024 |= 1;
                    }
                    v |= 1 << val;
                    SetModelScriptReg(m, arg, v);
                    m->mUnk_13c += step;
                    break;
                }
                // Read a register (MM_REG) and drop the value.
                case MC_READ:
                    if (mode == MM_REG) {
                        GetModelScriptReg(m, arg);
                        step = 1;
                    }
                    m->mUnk_13c += step;
                    break;
                // Register = operand.
                case MC_SET:
                    if (mode == MM_REG) {
                        val = GetModelScriptReg(m, val);
                    } else if (mode == MM_BITS) {
                        val = arg >> 5;
                        step = 1;
                    }
                    SetModelScriptReg(m, arg, val);
                    m->mUnk_13c += step;
                    break;
                // Register += operand.
                case MC_ADD:
                    if (mode == MM_VALUE) {
                        SetModelScriptReg(m, arg, val + GetModelScriptReg(m, arg));
                    } else if (mode == MM_REG) {
                        SetModelScriptReg(m, arg, GetModelScriptReg(m, arg) + GetModelScriptReg(m, val));
                    } else if (mode == MM_BITS) {
                        SetModelScriptReg(m, arg, (arg >> 5) + GetModelScriptReg(m, arg));
                        step = 1;
                    }
                    m->mUnk_13c += step;
                    break;
                // Register -= operand.
                case MC_SUB:
                    if (mode == MM_VALUE) {
                        SetModelScriptReg(m, arg, GetModelScriptReg(m, arg) - val);
                    } else if (mode == MM_REG) {
                        SetModelScriptReg(m, arg, GetModelScriptReg(m, arg) - GetModelScriptReg(m, val));
                    } else if (mode == MM_BITS) {
                        SetModelScriptReg(m, arg, GetModelScriptReg(m, arg) - (arg >> 5));
                        step = 1;
                    }
                    m->mUnk_13c += step;
                    break;
                // Register *= operand.
                case MC_MUL:
                    if (mode == MM_VALUE) {
                        SetModelScriptReg(m, arg, GetModelScriptReg(m, arg) * val);
                    } else if (mode == MM_REG) {
                        SetModelScriptReg(m, arg, GetModelScriptReg(m, arg) * GetModelScriptReg(m, val));
                    } else if (mode == MM_BITS) {
                        SetModelScriptReg(m, arg, GetModelScriptReg(m, arg) * (arg >> 5));
                        step = 1;
                    }
                    m->mUnk_13c += step;
                    break;
                // Register &= operand.
                case MC_AND:
                    if (mode == MM_VALUE) {
                        SetModelScriptReg(m, arg, GetModelScriptReg(m, arg) & val);
                    } else if (mode == MM_REG) {
                        SetModelScriptReg(m, arg, GetModelScriptReg(m, arg) & GetModelScriptReg(m, val));
                    } else if (mode == MM_BITS) {
                        SetModelScriptReg(m, arg, GetModelScriptReg(m, arg) & (arg >> 5));
                        step = 1;
                    }
                    m->mUnk_13c += step;
                    break;
                // Compare a register with the operand: less flag if smaller, equal flag if equal.
                case MC_COMPARE:
                    m->mUnk_024 &= ~3;
                    if (mode == MM_REG) {
                        val = GetModelScriptReg(m, val);
                    } else if (mode == MM_BITS) {
                        val = arg >> 5;
                        step = 1;
                    }
                    if (GetModelScriptReg(m, arg) < val) {
                        m->mUnk_024 |= 2;
                    } else if (val == GetModelScriptReg(m, arg)) {
                        m->mUnk_024 |= 1;
                    }
                    m->mUnk_13c += step;
                    break;
                // Register %= operand (unsigned).
                case MC_MOD:
                    if (mode == MM_VALUE) {
                        SetModelScriptReg(m, arg, (u32) GetModelScriptReg(m, arg) % val);
                    } else if (mode == MM_REG) {
                        SetModelScriptReg(m, arg, (u32) GetModelScriptReg(m, arg) % GetModelScriptReg(m, val));
                    }
                    m->mUnk_13c += 2;
                    break;
                // Set data_020c31dc.mUnk_3c.
                case MC_SET_3C:
                    if (mode == MM_VALUE) {
                        data_020c31dc.mUnk_3c = val;
                        m->mUnk_13c += 2;
                    } else if (mode == MM_REG) {
                        data_020c31dc.mUnk_3c = GetModelScriptReg(m, arg);
                        m->mUnk_13c += 1;
                    } else if (mode == MM_BITS) {
                        data_020c31dc.mUnk_3c = ((arg & 0xf0) << 4) + (arg & 0xf);
                        m->mUnk_13c += 1;
                    }
                    m->mUnk_172_0 = 1;
                    break;
                // Set gMapState.mCamera.mUnk_9c.mUnk_1ec from entry `id` (GetMapEntry), or clear it for id -1.
                // UpdateCamera's modes 10 and 11 add this point to both the position and the look-at point.
                case MC_LOOK_AT: {
                    u16 id;
                    void *entry;
                    if (mode == MM_VALUE) {
                        id = val;
                        m->mUnk_13c += 2;
                    } else if (mode == MM_REG) {
                        id = GetModelScriptReg(m, arg);
                        m->mUnk_13c += 1;
                    } else if (mode == MM_BITS) {
                        id = ((arg & 0xf0) << 4) + (arg & 0xf);
                        m->mUnk_13c += 1;
                    }
                    // The test against -1 is always true: id is 16-bit.
                    if (id != -1) {
                        entry = GetMapEntry(id, 0);
                        if (entry == NULL) {
                            OS_Terminate();
                        }
                        gMapState.mCamera.mUnk_9c.mUnk_1ec.mUnk_00.x = GetMapEntryX(entry);
                        gMapState.mCamera.mUnk_9c.mUnk_1ec.mUnk_00.y = GetMapEntryY(entry);
                        gMapState.mCamera.mUnk_9c.mUnk_1ec.mUnk_00.z = GetMapEntryZ(entry);
                    } else {
                        gMapState.mCamera.mUnk_9c.mUnk_1ec.mUnk_00.x = 0;
                        gMapState.mCamera.mUnk_9c.mUnk_1ec.mUnk_00.y = 0;
                        gMapState.mCamera.mUnk_9c.mUnk_1ec.mUnk_00.z = 0;
                    }
                    break;
                }
                // Set data_020c31dc.mUnk_3e and reset mUnk_3c.
                case MC_SET_3E:
                    if (mode == MM_VALUE) {
                        data_020c31dc.mUnk_3e = val;
                        m->mUnk_13c += 2;
                    } else if (mode == MM_REG) {
                        data_020c31dc.mUnk_3e = GetModelScriptReg(m, arg);
                        m->mUnk_13c += 1;
                    } else if (mode == MM_BITS) {
                        data_020c31dc.mUnk_3e = ((arg & 0xf0) << 4) + (arg & 0xf);
                        m->mUnk_13c += 1;
                    }
                    data_020c31dc.mUnk_3c = 0xffff;
                    m->mUnk_172_0 = 1;
                    m->mUnk_172_1 = 1;
                    break;
                // Set flag bit 2 from the argument.
                case MC_FLAG:
                    m->mUnk_024 = (m->mUnk_024 & ~4) | (arg << 2);
                    m->mUnk_13c += 1;
                    break;
                // Set mUnk_148, mUnk_144 and mUnk_146, none of which is read in this build (mUnk_148 is only copied
                // to mUnk_149, which nothing reads).
                case MC_TRACK:
                    if (mode == MM_VALUE) {
                        m->mUnk_148 = arg;
                    } else {
                        m->mUnk_148 = GetModelScriptReg(m, arg);
                    }
                    m->mUnk_144 = val;
                    m->mUnk_146 = m->mUnk_13c[2];
                    m->mUnk_13c += 3;
                    break;
                // Set event `arg` (target and key) and its bit in mUnk_130; index -1 uses entry 32 and sets flag bit 2.
                case MC_EVENT:
                    if (arg == -1) {
                        m->mUnk_024 |= 4;
                        arg = 32;
                    } else {
                        m->mUnk_130 |= 1 << arg;
                    }
                    m->mUnk_028[arg].mUnk_0 = m->mUnk_13c + val;
                    m->mUnk_028[arg].mUnk_4 = m->mUnk_13c[2];
                    m->mUnk_13c += 3;
                    break;
                // Set the model id and start animation 0 of it.
                case MC_MODEL:
                    if (mode == MM_REG) {
                        val = GetModelScriptReg(m, val);
                    }
                    m->mUnk_170 = val;
                    StartModelAnim(m, &slot, 0, arg & 0x7f, 0, 0);
                    slot++;
                    m->mUnk_172_11 = 1;
                    if (arg & 0x80) {
                        m->mUnk_14c.mUnk_2 = (m->mUnk_152.mUnk_2 * m->mUnk_14c.mUnk_4) / m->mUnk_152.mUnk_4;
                        func_02026080(m);
                    }
                    m->mUnk_13c += 2;
                    m->mUnk_172_14 = 0;
                    break;
                // Start an animation.
                case MC_ANIM:
                    if (mode == MM_REG) {
                        val = GetModelScriptReg(m, val);
                    }
                    StartModelAnim(m, &slot, val, arg & 0x7f, (arg >> 7) << 5, 0);
                    slot++;
                    m->mUnk_13c += 2;
                    m->mUnk_172_14 = 0;
                    break;
                // Clear mUnk_142, set bit `arg` of mUnk_134 and return 0.
                case MC_SIGNAL:
                    m->mUnk_142 = 0;
                    m->mUnk_134 |= 1 << arg;
                    return 0;
                // Advance the animation and return 1: with mUnk_172_14 set, clear it and step past, else set
                // mUnk_172_9 and mUnk_172_7.
                case MC_STEP:
                    if (m->mUnk_172_14 == 1) {
                        m->mUnk_172_14 = 0;
                        m->mUnk_13c += 1;
                    } else {
                        m->mUnk_172_9 = 1;
                        m->mUnk_172_7 = 1;
                    }
                    func_020266a8(m, &slot);
                    return 1;
                // Set mUnk_172_13; with mUnk_172_10 set, step past and copy the animation state, else go on as MC_WAIT.
                case MC_WAIT_OR_IDLE:
                    m->mUnk_172_13 = 1;
                    if (m->mUnk_172_10) {
                        m->mUnk_13c += 1;
                        func_0202635c(m, &slot);
                        break;
                    }
                    // fall through
                // Stop for this frame (advance the animation; the command runs again) while the mask bits are set in
                // mUnk_134 and clear in mUnk_138 and mUnk_172_14 is clear, or while mUnk_172_12 is set; else step past.
                case MC_WAIT: {
                    u32 mask;
                    if (mode == MM_VALUE) {
                        mask = 1 << arg;
                    } else {
                        mask = 1 << GetModelScriptReg(m, arg);
                    }
                    if ((mask != (m->mUnk_134 & mask & ~m->mUnk_138) || m->mUnk_172_14 == 1) && m->mUnk_172_12 == 0) {
                        m->mUnk_172_14 = 0;
                        m->mUnk_13c += 1;
                        func_0202635c(m, &slot);
                    } else {
                        func_020266a8(m, &slot);
                        cont = FALSE;
                    }
                    break;
                }
                default:
                    OS_Terminate();
                    break;
            }
        } while (cont);
    }
    return 1;
}

// Zeroes mTrackFillCount and mUnk_48 and clears track 0's data; track 1's data pointer is left.
void ClearAnimTracks(void) {
    data_020c31dc.mTrackFillCount = 0;
    data_020c31dc.mUnk_48 = 0;
    data_020c31dc.mAnimTracks[0].mData = NULL;
}

void func_020271e0(Model *m) {
    m->mUnk_14c.mUnk_0 = 0;
}

void InitModel(void *obj, int id, int anim, int count, u32 script) {
    InitModelEx(obj, id, anim, count, 0, script);
}

void InitModelEx(Model *m, int id, int anim, int count, int mode, u32 script) {
    MI_CpuFill8(m, 0, sizeof(Model));
    StartModelEx(m, id, anim, count, mode, script);
}

void StartModel(void *obj, int id, int anim, int count, u32 script) {
    StartModelEx(obj, id, anim, count, 0, script);
}

// Starts model `id` on `script` (or, without one, animation `anim`). A different or zero id clears the whole object
// first unless mode is -1.
void StartModelEx(Model *m, int id, int anim, int count, int mode, u32 script) {
    if (m->mUnk_170 != id || id == 0) {
        if (mode != -1) {
            memset(m, 0, sizeof(Model));
        }
        m->mUnk_170 = id;
        m->mUnk_172_6 = 1;
    } else {
        m->mUnk_172_6 = 0;
    }
    m->mUnk_172_12 = 0;
    m->mUnk_13c = (s16 *) script;
    m->mUnk_134 = 0;
    m->mUnk_138 = 0;
    m->mUnk_148 = 0;
    m->mUnk_172_5 = 0;
    m->mUnk_172_0 = 0;
    m->mUnk_175 = 0xff;
    m->mUnk_172_8 = 1;
    m->mUnk_172_11 = 0;
    // Clear the return position, the registers and the flags (everything before the events).
    MIi_CpuClear16(0, m, 0x28);
    if (script != 0) {
        if (ExecModelScript(m) == 0) {
            UpdateModelScript(m);
        }
    } else {
        s32 slot = 0;
        StartModelAnim(m, &slot, anim, count, 0, mode);
    }
    m->mUnk_152.mUnk_0 = m->mUnk_14c.mUnk_0;
    m->mUnk_174 = 0;
    ClearAnimTracks();
}

void UpdateAndDrawModel(void *obj, void *params, int flags) {
    Model *m = obj;
    VecFx32 pos;
    UpdateModelScript(m);
    if (m->mUnk_170 != 0 && !(flags & 0x80)) {
        DrawModelFileEx(m->mUnk_170, params, m, flags);
        if (m->mUnk_175 != 0xff) {
            GetPoseNodePos(&pos, m->mUnk_175);
            m->mUnk_158.x = pos.x;
            m->mUnk_158.y = pos.y;
            m->mUnk_158.z = pos.z;
        }
    } else {
        ClearAnimTracks();
    }
}
