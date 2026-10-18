#include "types.h"
#include <nitro/fx.h>
#include <nitro/gx.h>
#include <nitro/mi.h>
#include <nitro/os.h>
#include <Main/clib.h>
#include <Main/Unknown/angle_blend.h>
#include <Main/Unknown/functions.h>
#include <Main/Unknown/game_math.h>
#include <Main/Unknown/game_util.h>
#include <Main/Unknown/globals.h>
#include <Main/Unknown/model.h>
#include <Main/Unknown/move_object.h>
#include <Main/Unknown/random_angle.h>
#include <Main/Unknown/system_init.h>
#include <Main/Unknown/vram_cache.h>
#include <Main/Unknown/engine_asm.h>
#include <Main/Unknown/texture_blocks.h>
#include <Main/Unknown/file_loader.h>
#include <Main/Unknown/map_entries.h>
#include <Main/Unknown/align_util.h>

typedef struct Unk_PoseChannel {
    u16 mUnk_0;
    u16 mUnk_2;
} Unk_PoseChannel;

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

// Texture size code of a power-of-two size, indexed by log2(size) - 3.
static const u8 data_02050898[8] = {0, 1, 2, 3, 4, 5, 6, 7};

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

void func_0202350c(u16 *rot, s32 *pos, s32 *vec, u32 frame, u32 map) {
    u8 t;
    u32 next;
    u32 cur;
    Unk_PoseFrame *f;
    Unk_PoseAnim *hdr;
    u16 tmpRot[3];
    s32 tmpPos[3];
    s32 tmpVec[3];

    hdr = data_020c31dc.mUnk_18[map].mUnk_00;
    f = &hdr->mUnk_8[frame];
    t = data_020c31dc.mUnk_18[map].mUnk_0c;
    next = data_020c31dc.mUnk_18[map].mUnk_08;
    cur = data_020c31dc.mUnk_18[map].mUnk_04;

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
        func_020223e8(t, 8, rot, tmpRot);
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
        func_0202258c(t, 8, pos, tmpPos);
    }
    if (vec != NULL) {
        tmpVec[0] = ((s16 *) hdr)[(f->mUnk_00[0].mUnk_0 > 1 ? next : 0) + (f->mUnk_00[0].mUnk_2 >> 1)];
        tmpVec[1] = ((s16 *) hdr)[(f->mUnk_00[1].mUnk_0 > 1 ? next : 0) + (f->mUnk_00[1].mUnk_2 >> 1)];
        tmpVec[2] = ((s16 *) hdr)[(f->mUnk_00[2].mUnk_0 > 1 ? next : 0) + (f->mUnk_00[2].mUnk_2 >> 1)];
        func_0202258c(t, 8, vec, tmpVec);
    }
}

// Walks the pose tree from `node`: mUnk_0 is a node's first child and mUnk_1 its next sibling (0xff = none). The
// matrix of `from` is the starting matrix; each node's matrix is stored in data_020c3244[node], and nodes with a
// slot in mUnk_e are also loaded into the geometry engine's matrix stack.
void func_0202397c(int from, int node) {
    Unk_PoseNode *n;
    MtxFx43 m;
    s32 pos[3];
    s32 scale[3];
    s32 tmpPos[3];
    s32 tmpScale[3];
    u16 rot[3];
    int done;
    u16 tmpRot[3];
    s16 sx, cx, sy, cy, sz, cz;
    s32 a, b, k0, k1, k2;
    s32 r00, r01, r02, r10, r11, r12, r20, r21, r22;
    u32 i;
    int ix, iy, iz;
    u16 ax, ay, az;
    int step;
    VecFx32 *v;

    n = &data_020c31dc.mUnk_38[node];
    if (from != -1) {
        func_0203fdd8(&data_020c3244[from], &data_020c31ac);
    }
    if (n->mUnk_1 != 0xff) {
        func_0203fdd8(&data_020c31ac, &data_020c3244[n->mUnk_1]);
    }
    if (n->mUnk_0 == 0xff && n->mUnk_e == -1 && data_020c31dc.mUnk_47 != 0) {
        goto sibling;
    }
    if (data_020c31dc.mUnk_49 == 0) {
        rot[0] = n->mUnk_5 << 8;
        rot[1] = n->mUnk_6 << 8;
        rot[2] = n->mUnk_7 << 8;
        pos[0] = n->mUnk_8 << 5;
        pos[1] = n->mUnk_a << 5;
        pos[2] = n->mUnk_c << 5;
        scale[0] = n->mUnk_2 << 4;
        scale[1] = n->mUnk_3 << 4;
        scale[2] = n->mUnk_4 << 4;
    } else {
        done = 0;
        func_020232f0(0, rot, pos, scale, node, &done, n);
        if (data_020c31dc.mUnk_48 != 0) {
            done = 0;
            func_020232f0(1, tmpRot, tmpPos, tmpScale, node, &done, n);
            func_020223e8(data_020c31dc.mUnk_48, 4, rot, tmpRot);
            func_0202258c(data_020c31dc.mUnk_48, 4, scale, tmpScale);
            if (data_020c31dc.mUnk_45 == 0 || node > data_020c31dc.mUnk_46) {
                func_0202258c(data_020c31dc.mUnk_48, 4, pos, tmpPos);
            }
        }
        pos[0] = pos[0] >> data_020c31dc.mUnk_44;
        pos[1] = pos[1] >> data_020c31dc.mUnk_44;
        pos[2] = pos[2] >> data_020c31dc.mUnk_44;
    }
    ax = -rot[0];
    az = -rot[2];
    ay = rot[1];
    iy = (ay >> 4) * 2;
    iz = (az >> 4) * 2;
    ix = (ax >> 4) * 2;
    sx = FX_SinCosTable_[ix];
    sy = FX_SinCosTable_[iy];
    cx = FX_SinCosTable_[ix + 1];
    cz = FX_SinCosTable_[iz + 1];
    cy = FX_SinCosTable_[iy + 1];
    sz = FX_SinCosTable_[iz];
    a = (sx * sy) >> 12;
    b = (cx * sy) >> 12;
    r00 = (cy * cz) >> 12;
    r01 = (-cy * sz) >> 12;
    r02 = -sy;
    r10 = (cx * sz + (-a) * cz) >> 12;
    r11 = (a * sz + cx * cz) >> 12;
    r12 = (-sx * cy) >> 12;
    r20 = (b * cz + sx * sz) >> 12;
    r21 = (sx * cz + (-b) * sz) >> 12;
    r22 = (cx * cy) >> 12;
    k0 = n->mUnk_2 * scale[0];
    k1 = n->mUnk_3 * scale[1];
    k2 = n->mUnk_4 * scale[2];
    m._[0] = (k0 * r00) / 4096;
    m._[1] = (k1 * r01) / 4096;
    m._[2] = (k2 * r02) / 4096;
    m._[3] = (k0 * r10) / 4096;
    m._[4] = (k1 * r11) / 4096;
    m._[5] = (k2 * r12) / 4096;
    m._[6] = (k0 * r20) / 4096;
    m._[7] = (k1 * r21) / 4096;
    m._[8] = (k2 * r22) / 4096;
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
            u32 len = func_0201fcec((s64) x * x + (s64) y * y + (s64) z * z);
            v->z = 0;
            v->y = 0;
            v->x = 0;
            ((s32 *) v)[i] = len;
            v += step;
        }
    }
    func_0203fdd8(&data_020c31ac, &data_020c3244[node]);
    if (data_020c31dc.mUnk_4a < node) {
        data_020c31dc.mUnk_4a = node;
    }
    if (node == data_020c31dc.mUnk_46) {
        data_020c31dc.mUnk_46 = 0xff;
        return;
    }
    if (n->mUnk_e != -1) {
        if (n->mUnk_e >= 0x1f) {
            OS_Terminate();
        }
        G3_LoadMtx43(&data_020c31ac);
        reg_G3X_MTX_STORE = n->mUnk_e;
    }
    if (n->mUnk_0 != 0xff) {
        func_0202397c(-1, n->mUnk_0);
    }
sibling:
    if (n->mUnk_1 != 0xff) {
        func_0202397c(n->mUnk_1, n->mUnk_1);
    }
}

void func_02023f0c(VecFx32 *out, int node) {
    MtxFx43 m;

    func_0203bdec(&data_020c3244[node], &data_020c3e44.mUnk_6c, &m);
    out->x = m._[9];
    out->y = m._[10];
    out->z = m._[11];
}

void func_02023f58(DrawParams *p, int x, int y, int z, int a, u16 b, u8 c, u8 d, int flags) {
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
    if (data_020c3e44.mUnk_2024.mUnk_2dc.mUnk_4a != 1) {
        VecFx32 pos = {0, 0, 0x1000};
        VecFx32 target = {0, 0, 0};
        VecFx32 up = {0, 0x1000, 0};

        func_0203d0f0(&pos, &up, &target, TRUE, &data_020c3e44.mUnk_3c);
        func_0203ba60(&data_020c3e44.mUnk_3c, &data_020c3e44.mUnk_6c);
        func_0203d08c(0, 0x3000, 0, 0x4000, 0x4000, -0x4000, 0x1000, TRUE, &data_020c3e44.mUnk_9c);
        reg_G3X_MTX_MODE = 3;
        reg_G3X_MTX_IDENTITY = 0;
        reg_G3X_MTX_MODE = 2;
        reg_G3X_MTX_PUSH = 0;
        data_020c3e44.mUnk_2024.mUnk_2dc.mUnk_4a = 1;
    }
}

void func_020240ec(void) {
    if (data_020c3e44.mUnk_2024.mUnk_2dc.mUnk_4a != 2) {
        VecFx32 pos = {0, 0, 0x1000};
        VecFx32 target = {0, 0, 0};
        VecFx32 up = {0, 0x1000, 0};

        func_0203d0f0(&pos, &up, &target, TRUE, NULL);
        func_0203d08c(0, 0xc0, 0, 0x100, 0x400, 0xc8000, 0x1000, TRUE, &data_020c3e44.mUnk_9c);
        reg_G3X_MTX_MODE = 3;
        reg_G3X_MTX_IDENTITY = 0;
        reg_G3X_MTX_MODE = 2;
        reg_G3X_MTX_PUSH = 0;
        data_020c3e44.mUnk_2024.mUnk_2dc.mUnk_4b = 0x1f;
        data_020c3e44.mUnk_2024.mUnk_2dc.mUnk_4c = 0;
        reg_G3X_POLYGON_ATTR = 0x1f00c1;
        reg_G3X_TEXIMAGE_PARAM = 0x20000000;
        reg_G3X_DIF_AMB = 0x7fff0000;
        reg_G3X_SPE_EMI = 0;
        reg_G3X_LIGHT_COLOR = 0x7fff;
        data_020c3e44.mUnk_2024.mUnk_2dc.mUnk_4a = 2;
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
    func_020348ac(0x1f, 0x1f, 0x1f, -0xffc);
}

void func_020242a8(void *out, int x, int y, int z, int scale, u16 angle, int flags, int w) {
    func_02023f58(out, x, y, z, scale, angle, 4, flags, w);
}

void func_020242e8(u16 id, int x, int y, int a, int b, u16 c, u8 d, int e) {
    DrawParams params;

    func_020242a8(&params, x, y, a, b, c, d, e);
    func_02024b5c(id, &params, NULL, e);
}

void func_02024338(void *a, void *b, int c) {
    func_020262a0(a);
    func_02024b5c(((Model *) a)->mUnk_170, b, a, c);
}

void func_02024368(Model *obj, int x, int y, int scale, u16 angle, int flags) {
    DrawParams params;
    func_020242a8(&params, x, y, 0, scale, angle, -1, flags);
    func_020262a0(obj);
    func_02024b5c(obj->mUnk_170, &params, obj, flags);
}

void func_020243c0(int id, fx32 x, fx32 y, fx32 z, u16 angle, fx32 sx, fx32 sy, u32 alpha) {
    MtxFx33 rot;
    s32 rect[4];
    TexInfo info;
    s32 clip[4];
    u16 *pal;
    int i;
    u16 key;
    int idx;
    int size;
    int s, t;
    int fmt;
    int opaque;
    u32 tex;
    u32 col;
    u32 pltt;
    u32 w;
    u32 *hdr;
    int base;
    int a, b, c, n;

    if (!(data_020c18bc.mUnk_46c & 0x80)) {
        OS_Terminate();
    }
    if (alpha > 31) {
        OS_Terminate();
    }
    if (id == -1) {
        return;
    }
    func_0202d828(id, 0, rect);
    if (rect[2] == rect[0] && rect[3] == rect[1]) {
        return;
    }
    func_020240ec();
    if (data_020c3e44.mUnk_2024.mUnk_2dc.mUnk_4b != alpha) {
        reg_G3X_POLYGON_ATTR = (data_020c3e44.mUnk_2024.mUnk_2dc.mUnk_4c << 24) | 0xc1 | (alpha << 16);
        data_020c3e44.mUnk_2024.mUnk_2dc.mUnk_4b = alpha;
    }
    hdr = func_0202adc0(id);
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
        func_0202ae84(id, &info, i);
        func_0202d828(id, 0, clip);
        base = func_0201c538();
        col = pal[info.mUnk_14] & 0x7fff;
        key = func_0201c544() + (c ^ (col << 2));
        size = info.mUnk_0c * info.mUnk_0e;
        opaque = pal[info.mUnk_14] >> 15;
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
        tex = func_020229b4(1, base + (id << 6) + i, size, 0);
        pltt = func_020230b0(col, key, info.mUnk_15 << 5);
        s = data_02050898[0x1c - func_02028250(info.mUnk_0c)];
        t = data_02050898[0x1c - func_02028250(info.mUnk_0e)];
        reg_G3X_TEXIMAGE_PARAM = (tex >> 3) | (fmt << 26) | 0x40000000 | (s << 20) | (t << 23) | ((opaque == 0) << 29);
        reg_G3X_PLTT_BASE = pltt >> (4 - (fmt == 2));
        if (opaque != 0) {
            if (info.mUnk_08 + info.mUnk_0c > clip[2]) {
                info.mUnk_0c = clip[2] - info.mUnk_08 + 1;
            }
            if (info.mUnk_0a + info.mUnk_0e > clip[3]) {
                info.mUnk_0e = clip[3] - info.mUnk_0a + 1;
            }
        }
        reg_G3X_MTX_MODE = 1;
        reg_G3X_NORMAL = 0x1ff00000;
        PutTexCoord(0, 0);
        PutVtx16(info.mUnk_08, info.mUnk_0a, 0);
        PutTexCoord(info.mUnk_0c << 4, 0);
        PutVtxXY(info.mUnk_08 + info.mUnk_0c, info.mUnk_0a);
        PutTexCoord(info.mUnk_0c << 4, info.mUnk_0e << 4);
        PutVtxXY(info.mUnk_08 + info.mUnk_0c, info.mUnk_0a + info.mUnk_0e);
        PutTexCoord(0, info.mUnk_0e << 4);
        PutVtxXY(info.mUnk_08, info.mUnk_0a + info.mUnk_0e);
    }
    reg_G3X_END_VTXS = 0;
    reg_G3X_MTX_POP = 1;
}

void func_020248e8(u32 id, void *p, int flags) {
    func_02024b5c(id, p, NULL, flags);
}

void func_020248fc(void) {
    data_020c31dc.mUnk_49 = 0;
}

ModelPart *func_02024910(int id, int a) {
    u32 *data = func_0202ded8(id);
    u8 *p = (u8 *) (data + ((u16) data[0] * 4 + 1));
    p += p[0] * 4 + 2;
    p += p[0] * 2 + 2;
    p += p[0] * 2 + 2;
    return func_0203492c(p + 1);
}

u16 func_02024968(const AnimKey *key, const u8 *table, int t) {
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
                int d = func_0201fe34(v, (u16) (table[5] << 8));
                v += d * r / 2;
            }
            break;
        case 3:
            table += (t >> 2) + off;
            v = table[4] << 8;
            if ((r = t & 3) != 0) {
                int d = func_0201fe34(v, (u16) (table[5] << 8));
                v += d * r / 4;
            }
            break;
    }
    return v;
}

int func_02024a64(const AnimKey16 *key, const u8 *table, int t) {
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

u32 func_02024b14(const DrawParams *p) {
    if (p->mUnk_3e == 0xff) {
        return data_020c18bc.mUnk_45c;
    }
    return p->mUnk_3e;
}

s32 func_02024b2c(const u32 *data, const DrawParams *p) {
    int s = ((*data & 0xff0000) >> 16) - data_020c31dc.mUnk_44;
    if (s >= 0) {
        return p->mUnk_28 << s;
    }
    return p->mUnk_28 >> -s;
}

void func_02024b5c(u32 id, void *params, void *obj, int flags) {
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
    s32 sc;
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
    int idx;
    u16 dif, amb, spe, emi;
    u32 se;
    u8 *cur;

    angOfs = 0;
    lightOfs = 0;
    if (!(data_020c18bc.mUnk_46c & 0x80)) {
        OS_Terminate();
    }
    res = func_0202ded8(id);
    data_020c31dc.mUnk_38 = (Unk_PoseNode *) (res + 1);
    data_020c31dc.mUnk_42 = 0xff;
    data_020c31dc.mUnk_4a = 0;
    if (p->mUnk_43 >= 4) {
        if (data_020c3e44.mUnk_2024.mUnk_2dc.mUnk_4a != 1) {
            func_02023ff0();
        }
        angOfs = 0x8000;
    } else if (data_020c3e44.mUnk_2024.mUnk_2dc.mUnk_4a != 0) {
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
            sin0 = FX_SinCosTable_[a0 * 2];
            sin2 = FX_SinCosTable_[a2 * 2];
            cos0 = FX_SinCosTable_[a0 * 2 + 1];
            cos1 = FX_SinCosTable_[a1 * 2 + 1];
            sin1 = FX_SinCosTable_[a1 * 2];
            c2s0 = (cos2 * sin0) >> 12;
            s2s0 = (sin2 * sin0) >> 12;
            sh0 = (*res & 0xff0000) >> 16;
            data_020c31dc.mUnk_44 = sh0;
            sx = 16 << sh0;
            if (sh0 != 0) {
                sh0 = sh0 - 1;
                sh1 = 19 - sh0;
            } else {
                sh0 = 0;
                sh1 = 19;
            }
            sy = sx;
            sz = sx;
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
                    func_0203bdec(&m, &data_020c3e44.mUnk_3c, &data_020c31ac);
                    break;
                case 2:
                    m._[9] = p->mPos.x;
                    m._[10] = p->mPos.y;
                    m._[11] = p->mPos.z;
                    func_0203bdec(&m, &data_020c3e44.mUnk_3c, &data_020c31ac);
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
            func_0203ba2c(&data_020c3244[p->mUnk_38], &data_020c31ac, sc, sc, sc);
        }
        if (model != NULL) {
            data_020c31dc.mUnk_45 = model->mUnk_172_0 | model->mUnk_172_7;
            data_020c31dc.mUnk_47 = model->mUnk_172_8;
            if (model->mUnk_172_0 == 1) {
                if (data_020c31dc.mUnk_3c == 0xffff) {
                    data_020c31dc.mUnk_46 = model->mUnk_175;
                    func_0202397c(-1, 0);
                    func_02023f0c(&pos, model->mUnk_175);
                    p->mPos.x -= pos.x - model->mUnk_158.x;
                    p->mPos.y -= pos.y - model->mUnk_158.y;
                    p->mPos.z -= pos.z - model->mUnk_158.z;
                } else {
                    func_02031fc4(data_020c31dc.mUnk_3c, &p->mPos);
                }
                m._[9] = p->mPos.x;
                m._[10] = p->mPos.y;
                m._[11] = p->mPos.z;
                func_0203bdec(&m, &data_020c3e44.mUnk_3c, &data_020c31ac);
                if (data_020c3e44.mUnk_2024.mUnk_2dc.mUnk_4a == 1) {
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
        func_0202397c(-1, 0);
        if (model != NULL && model->mUnk_172_3 == 1) {
            func_02023f0c(&ofs, 0);
            model->mUnk_158.x = ofs.x;
            model->mUnk_158.y = ofs.y;
            model->mUnk_158.z = ofs.z;
        }
    } else {
        sc = func_02024b2c(res, p);
        G3_LoadMtx43(&data_020c3244[p->mUnk_38]);
        PutScale(sc, sc, sc);
        reg_G3X_MTX_STORE = 0;
    }
    texAnim = (ModelTexAnims *) (res + ((u16) res[0] * 4 + 1));
    srtAnim = (ModelTracks *) ((u8 *) texAnim + (texAnim->mCount * 4 + 2));
    matAnim = (ModelTracks *) ((u8 *) srtAnim + (srtAnim->mCount * 2 + 2));
    p4 = (u8 *) matAnim + (matAnim->mCount * 2 + 2);
    count = p4[0];
    cur = func_0203492c(p4 + 1);
    if (model != NULL && model->mUnk_172_4 == 1) {
        count = 0;
    }
    part = 0;
    while (count-- != 0) {
        mesh = (ModelPart *) cur;
        if ((p->mUnk_24 & (1 << part)) && (data_020c18bc.mUnk_46c & 0x8000)) {
            src = func_02024910(p->mUnk_0c[p->mUnk_1c[part]], 0);
        } else {
            src = mesh;
        }
        visMask = p->mUnk_34 & mesh->mUnk_0c;
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
        if (mesh->mUnk_2b != 0) {
            fmt = mesh->mUnk_2b & 7;
            sizeS = mesh->mUnk_2a & 0xf;
            sizeT = mesh->mUnk_2a >> 4;
            width = 8 << sizeS;
            height = 8 << sizeT;
            texMask = p->mUnk_2c & mesh->mUnk_04;
            srtMask = p->mUnk_30 & mesh->mUnk_08;
            if (texMask != 0 && (model == NULL || model->mUnk_172_12 == 0)) {
                k = 31 - func_02028250(texMask);
                anim = (const ModelFrameAnim *) ((u32) texAnim + 2 + texAnim->mEntries[k].mOffset * 2);
                if (flags & 0x20) {
                    frame = (u32) flags >> 26;
                } else {
                    if ((flags & 0x200) && p->mUnk_3f != 0xff) {
                        frame = p->mUnk_3f;
                    } else if (model != NULL && (model->mUnk_14c.mUnk_0 != 0 || model->mUnk_172_11 == 1)) {
                        frame = model->mUnk_14c.mUnk_2 / model->mUnk_14c.mUnk_4;
                    } else {
                        frame = func_02024b14(p);
                    }
                    frame = ((u32) (frame << 8) / texAnim->mEntries[k].mDiv) % anim->mCount;
                }
                tex = func_020229b4(anim->mFrames[frame].mUnk_04 + (anim->mFrames[frame].mUnk_06 << 16),
                                    anim->mFrames[frame].mUnk_00, func_02025f58(sizeS, sizeT, fmt),
                                    anim->mFrames[frame].mUnk_02);
                if (mesh->mUnk_14 != 0) {
                    pltt = func_020230b0(anim->mFrames[frame].mUnk_0a + (anim->mFrames[frame].mUnk_0c << 16),
                                         anim->mFrames[frame].mUnk_08, mesh->mUnk_20);
                }
            } else {
                tex = func_020229b4(src->mUnk_10, src->mUnk_1c, func_02025f58(sizeS, sizeT, fmt), src->mUnk_18);
                if (src->mUnk_14 != 0) {
                    pltt = func_020230b0(src->mUnk_14, src->mUnk_1e, src->mUnk_20);
                }
            }
            reg_G3X_TEXIMAGE_PARAM = (tex >> 3) | (fmt << 26) | 0x40000000 | (sizeS << 20) | (sizeT << 23) |
                                     (((mesh->mUnk_00 & 0xc00) >> 10) << 16) | (((mesh->mUnk_00 & 0x60) >> 5) << 18) |
                                     (pltt0 << 29);
            if (mesh->mUnk_14 != 0) {
                reg_G3X_PLTT_BASE = pltt >> (4 - (fmt == 2));
            }
            reg_G3X_MTX_MODE = 3;
            reg_G3X_MTX_IDENTITY = 0;
            if (srtMask != 0 && (model == NULL || model->mUnk_172_12 == 0)) {
                k = 31 - func_02028250(srtMask);
                track = (const ModelKeyTrack *) ((u32) srtAnim + 2 + srtAnim->mOffsets[k] * 2);
                kp = (const u16 *) ((const u8 *) track + track->mKeyOffset + 4);
                if (srtAnim->mCount <= k) {
                    OS_Terminate();
                }
                if ((flags & 0x200) && (frame = p->mUnk_3e) != 0xff) {
                } else if (model != NULL) {
                    frame = model->mUnk_14c.mUnk_2 - 1;
                    if (frame < 0) {
                        frame += func_02025fc0(model->mUnk_14c.mUnk_0) * model->mUnk_14c.mUnk_4;
                    }
                    frame = frame / model->mUnk_14c.mUnk_4;
                } else {
                    frame = func_02024b14(p) % track->mCount;
                }
                for (q = 0; q < 5; q++) {
                    keys[q] = kp;
                    kp++;
                }
                rot = func_02024968((const AnimKey *) keys[2], (const u8 *) track, frame);
                if (rot != 0) {
                    hw = (width << 16) / 2;
                    hh = (height << 16) / 2;
                    idx = rot >> 4;
                    PutTrans(hw, hh, 0);
                    func_0203d13c(FX_SinCosTable_[idx * 2], FX_SinCosTable_[idx * 2 + 1]);
                    PutTrans(-hw, -hh, 0);
                }
                tsx = func_02024a64((const AnimKey16 *) keys[0], (const u8 *) track, frame);
                tsy = func_02024a64((const AnimKey16 *) keys[1], (const u8 *) track, frame);
                if (tsx != 0x1000 || tsy != 0x1000) {
                    PutTrans(0, height << 16, 0);
                    PutScale(tsx, tsy, 0x1000);
                    PutTrans(0, -(height << 16), 0);
                }
                tx = func_02024a64((const AnimKey16 *) keys[3], (const u8 *) track, frame);
                ty = func_02024a64((const AnimKey16 *) keys[4], (const u8 *) track, frame);
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
            k = 31 - func_02028250(visMask);
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
                frame = func_02024b14(p) % track->mCount;
            }
            for (q = 0; q < 5; q++) {
                mkeys[q] = kp;
                kp++;
            }
            if (!(flags & 2)) {
                alpha = MatKeyByte((const MatKey *) mkeys[4], track, frame);
                if (alpha == 0) {
                    cur = cur + 0x38 + (mesh->mUnk_34 & ~3);
                    continue;
                }
            }
            dif = MatKeyValue((const MatKey *) mkeys[0], track, frame);
            amb = MatKeyValue((const MatKey *) mkeys[1], track, frame);
            spe = MatKeyValue((const MatKey *) mkeys[2], track, frame);
            emi = MatKeyValue((const MatKey *) mkeys[3], track, frame);
        } else {
            amb = mesh->mUnk_24;
            dif = mesh->mUnk_22;
            spe = mesh->mUnk_26;
            emi = mesh->mUnk_28;
        }
        if (flags & 4) {
            cur += 0x38;
            PutPolygonAttr(data_020c3e44.mUnk_2024.mUnk_2dc.mUnk_5b, 3, 1, 0, alpha, 0);
            PutMaterial(dif | (((flags & 0x10) ? 0x7fff : amb) << 16), spe | (emi << 16));
            DC_StoreRange(cur, mesh->mUnk_34);
            func_01ff831c(3, cur, mesh->mUnk_34);
            PutPolygonAttr(data_020c3e44.mUnk_2024.mUnk_2dc.mUnk_5b, 3, 3, 8, alpha, 0);
            if (flags & 0x10) {
                amb = 0x7fff;
            }
            PutMaterial(dif | (amb << 16) | 0x8000, spe | (emi << 16));
            func_01ff831c(3, cur, mesh->mUnk_34);
        } else {
            if (!(p->mUnk_43_3 ^ p->mUnk_43_4)) {
                cull = (mesh->mUnk_00 >> 7) & 3;
            } else {
                cull = ((mesh->mUnk_00 & 0x80) >> 6) + ((mesh->mUnk_00 & 0x100) >> 8);
            }
            if (flags & 0x10) {
                amb = 0x7fff;
            }
            PutMaterial(dif | (amb << 16) | 0x8000, spe | (emi << 16));
            cur += 0x38;
            PutPolygonAttr(data_020c3e44.mUnk_2024.mUnk_2dc.mUnk_5b, (flags & 8) ? 2 : 0, cull, polyId, alpha,
                           polyFlags);
            DC_StoreRange(cur, mesh->mUnk_34);
            func_01ff831c(3, cur, mesh->mUnk_34);
        }
        part++;
        cur += mesh->mUnk_34 & ~3;
    }
    func_020271c8();
}

void func_02025db4(void *pos, s32 *out) {
    func_02025dc4(pos, out, 0);
}

void func_02025dc4(const VecFx32 *pos, s32 *out, int steps) {
    VecFx32 v;
    fx32 w;
    s32 x, y, z;
    reg_G3X_MTX_MODE = 1;
    G3_LoadMtx43(&data_020c3e44.mUnk_3c);
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

int func_02025f58(int log2w, int log2h, int format) {
    u8 bits[8] = {0, 8, 2, 4, 8, 0, 8, 16};
    return ((8 << log2w) * (8 << log2h) * bits[format]) / 8;
}

#pragma thumb on

int func_02025fc0(u16 id) {
    u16 *p = func_0202ded8(id);
    if (p[2] == 1) {
        return p[1];
    }
    if (p[2] == 2) {
        return p[1] >> 1;
    }
    return p[1] / p[2];
}

int func_02025fe0(Model *m, int track) {
    return func_02025fc0(m->mAnim[track].mUnk_0);
}

void func_02025ff4(Model *m, int n) {
    m->mUnk_14c.mUnk_2 = n * m->mUnk_14c.mUnk_4;
}

int func_02026004(Model *m) {
    return m->mUnk_14c.mUnk_2 / m->mUnk_14c.mUnk_4;
}

void func_0202601c(u16 id, u16 *frame, u32 count, int step, s32 *whole, s32 *next, s32 *rest, int loop) {
    u16 saved = *frame;
    *whole = func_0201fd10(*frame, count, (u32 *) rest);
    *next = *whole + 1;
    *rest = (*rest << 8) / count;
    if (step != 0) {
        *frame += step;
        if (*frame >= func_02025fc0(id) * count) {
            if (loop == 1) {
                *frame = 0;
            } else {
                *frame = saved;
            }
        }
    }
}

int func_02026080(Model *m) {
    u16 *q = func_0202ded8(m->mUnk_170);
    u16 *r;
    int scale, limit, count;
    q = (u16 *) ((u8 *) q + ((*(u32 *) q & 0xffff) * 4 + 1) * 4);
    r = q + q[1];
    scale = q[2];
    limit = r[1];
    count = m->mUnk_14c.mUnk_4;
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
    Unk_AnimTrack *t = &data_020c31dc.mUnk_18[track];
    u32 pos;
    u32 rest;
    if (a->mUnk_0 != 0) {
        t->mUnk_00 = func_0202ded8(a->mUnk_0);
        t->mUnk_0d = a->mUnk_5;
        if (a->mUnk_4 == 2) {
            t->mUnk_04 = (u32) a->mUnk_2 >> 1;
            t->mUnk_0c = (a->mUnk_2 & 1) << 7;
        } else {
            t->mUnk_04 = func_0201fd10(a->mUnk_2, a->mUnk_4, &rest);
            t->mUnk_0c = (rest << 8) / a->mUnk_4;
        }
        data_020c31dc.mUnk_49++;
        pos = t->mUnk_04;
        if (pos == func_02025fe0(m, track) - 1) {
            t->mUnk_0c = 0;
            if (a->mUnk_5 & 0x10) {
                a->mUnk_0 = 0;
                return 1;
            }
            if (a->mUnk_5 & 8) {
                return 1;
            }
        } else {
            t->mUnk_08 = pos + 1;
        }
        if (track < 1) {
            if (!(a->mUnk_5 & 0x20) && !(data_020c18bc.mUnk_46c & 0x80000000) && !m->mUnk_172_12) {
                a->mUnk_2++;
            }
            if (m->mUnk_174 == 0) {
                m->mUnk_172_6 = 0;
                m->mUnk_172_7 = 0;
            }
            if (a->mUnk_2 == func_02025fe0(m, track) * a->mUnk_4 - 1) {
                if (m->mUnk_172_13 == 1) {
                    a->mUnk_5 |= 0x20;
                    a->mUnk_2--;
                }
                return 1;
            }
            if (a->mUnk_2 == func_02025fe0(m, track) * a->mUnk_4) {
                a->mUnk_2 = 0;
            }
        }
    } else {
        t->mUnk_00 = NULL;
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
    if (r == 0 && data_020c18bc.mUnk_48c > 0 && (data_020c18bc.mUnk_46c & 0x40)) {
        r |= func_020260f4(m, track);
    }
    return r;
}

void func_020262a0(Model *m) {
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
    func_02026718(m);
    m->mUnk_134 = 0;
}

void func_0202635c(Model *m, s32 *slot) {
    m->mAnim[1] = m->mAnim[0];
    *slot = 0;
}

void func_02026384(Model *m, s32 *slot, int id, int count, int flags, int mode) {
    int ratio;
    if (*slot == 0) {
        ratio = func_02026004(m);
        func_020271c8();
        func_020271e0(m);
        if (m->mUnk_172_1 == 1) {
            u16 *hdr = func_0202ded8(id);
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
            func_02025ff4(m, ratio);
        } else {
            func_02025ff4(m, mode);
        }
    }
}

int func_020264d4(Model *m, u16 id) {
    id &= 0x1f;
    if (id > 0xf) {
        if (id == 0x1f) {
            return func_0201fd68();
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
            return data_020c18bc.mUnk_496;
        }
        if (id > 0xf) {
            OS_Terminate();
        }
    }
    return m->mUnk_004_all[id];
}

void func_02026598(Model *m, u16 id, u32 value) {
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
            data_020c18bc.mUnk_496 = value;
            return;
        }
        if (id > 0xf) {
            OS_Terminate();
        }
    }
    m->mUnk_004_all[id] = value;
}

void func_020266a8(Model *m, s32 *slot) {
    int r;
    switch (m->mUnk_14c.mUnk_0) {
        default:
            r = func_02026268(m, 0);
            break;
        case 0:
            r = func_02026268(m, 0);
            break;
    }
    m->mUnk_172_10 = r | m->mUnk_172_10;
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
// 16-bit operand word. In the register commands the argument names the register (read with func_020264d4, written
// with func_02026598); mode 2 takes the operand word as a value, mode 1 as a register number, and mode 0 takes the
// value from bits 5-7 of the argument and has no operand word. mUnk_024 bit 0 is the equal flag, bit 1 the less flag.
int func_02026718(Model *m) {
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
                case 0:
                    if (mode == 2) {
                        val = arg;
                    }
                    m->mUnk_13c += val;
                    break;
                // Mode 0 returns to the saved position; mode 2 saves the next position and jumps.
                case 11:
                    if (mode == 0) {
                        m->mUnk_13c = m->mUnk_000;
                    } else if (mode == 2) {
                        m->mUnk_000 = m->mUnk_13c + 2;
                        m->mUnk_13c += val;
                    }
                    break;
                // Jump if the equal flag (bit 0) is set.
                case 1:
                    if (mode == 2) {
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
                case 3:
                    if (mode == 2) {
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
                case 5:
                    if (mode == 2) {
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
                case 4:
                    if (mode == 2) {
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
                case 6:
                    if (mode == 2) {
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
                case 2:
                    if (mode == 2) {
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
                case 25:
                    m->mUnk_024 &= ~3;
                    if (mode == 1) {
                        val = func_020264d4(m, val);
                    } else if (mode == 0) {
                        val = (u8) arg >> 5;
                        step = 1;
                    }
                    if (func_020264d4(m, arg) & (1 << val)) {
                        m->mUnk_024 |= 1;
                    }
                    m->mUnk_13c += step;
                    break;
                // Test a bit of a register, then clear it.
                case 27: {
                    m->mUnk_024 &= ~3;
                    if (mode == 1) {
                        val = func_020264d4(m, val);
                    } else if (mode == 0) {
                        val = (u8) arg >> 5;
                        step = 1;
                    }
                    v = func_020264d4(m, arg);
                    if (v & (1 << val)) {
                        m->mUnk_024 |= 1;
                    }
                    v &= ~(1 << val);
                    func_02026598(m, arg, v);
                    m->mUnk_13c += step;
                    break;
                }
                // Test a bit of a register, then set it.
                case 26: {
                    m->mUnk_024 &= ~3;
                    if (mode == 1) {
                        val = func_020264d4(m, val);
                    } else if (mode == 0) {
                        val = (u8) arg >> 5;
                        step = 1;
                    }
                    v = func_020264d4(m, arg);
                    if (v & (1 << val)) {
                        m->mUnk_024 |= 1;
                    }
                    v |= 1 << val;
                    func_02026598(m, arg, v);
                    m->mUnk_13c += step;
                    break;
                }
                // Read a register (mode 1) and drop the value.
                case 63:
                    if (mode == 1) {
                        func_020264d4(m, arg);
                        step = 1;
                    }
                    m->mUnk_13c += step;
                    break;
                // Register = operand.
                case 7:
                    if (mode == 1) {
                        val = func_020264d4(m, val);
                    } else if (mode == 0) {
                        val = arg >> 5;
                        step = 1;
                    }
                    func_02026598(m, arg, val);
                    m->mUnk_13c += step;
                    break;
                // Register += operand.
                case 8:
                    if (mode == 2) {
                        func_02026598(m, arg, val + func_020264d4(m, arg));
                    } else if (mode == 1) {
                        func_02026598(m, arg, func_020264d4(m, arg) + func_020264d4(m, val));
                    } else if (mode == 0) {
                        func_02026598(m, arg, (arg >> 5) + func_020264d4(m, arg));
                        step = 1;
                    }
                    m->mUnk_13c += step;
                    break;
                // Register -= operand.
                case 9:
                    if (mode == 2) {
                        func_02026598(m, arg, func_020264d4(m, arg) - val);
                    } else if (mode == 1) {
                        func_02026598(m, arg, func_020264d4(m, arg) - func_020264d4(m, val));
                    } else if (mode == 0) {
                        func_02026598(m, arg, func_020264d4(m, arg) - (arg >> 5));
                        step = 1;
                    }
                    m->mUnk_13c += step;
                    break;
                // Register *= operand.
                case 29:
                    if (mode == 2) {
                        func_02026598(m, arg, func_020264d4(m, arg) * val);
                    } else if (mode == 1) {
                        func_02026598(m, arg, func_020264d4(m, arg) * func_020264d4(m, val));
                    } else if (mode == 0) {
                        func_02026598(m, arg, func_020264d4(m, arg) * (arg >> 5));
                        step = 1;
                    }
                    m->mUnk_13c += step;
                    break;
                // Register &= operand.
                case 30:
                    if (mode == 2) {
                        func_02026598(m, arg, func_020264d4(m, arg) & val);
                    } else if (mode == 1) {
                        func_02026598(m, arg, func_020264d4(m, arg) & func_020264d4(m, val));
                    } else if (mode == 0) {
                        func_02026598(m, arg, func_020264d4(m, arg) & (arg >> 5));
                        step = 1;
                    }
                    m->mUnk_13c += step;
                    break;
                // Compare a register with the operand: less flag if smaller, equal flag if equal.
                case 12:
                    m->mUnk_024 &= ~3;
                    if (mode == 1) {
                        val = func_020264d4(m, val);
                    } else if (mode == 0) {
                        val = arg >> 5;
                        step = 1;
                    }
                    if (func_020264d4(m, arg) < val) {
                        m->mUnk_024 |= 2;
                    } else if (val == func_020264d4(m, arg)) {
                        m->mUnk_024 |= 1;
                    }
                    m->mUnk_13c += step;
                    break;
                // Register %= operand (unsigned).
                case 10:
                    if (mode == 2) {
                        func_02026598(m, arg, (u32) func_020264d4(m, arg) % val);
                    } else if (mode == 1) {
                        func_02026598(m, arg, (u32) func_020264d4(m, arg) % func_020264d4(m, val));
                    }
                    m->mUnk_13c += 2;
                    break;
                // Set data_020c31dc.mUnk_3c.
                case 20:
                    if (mode == 2) {
                        data_020c31dc.mUnk_3c = val;
                        m->mUnk_13c += 2;
                    } else if (mode == 1) {
                        data_020c31dc.mUnk_3c = func_020264d4(m, arg);
                        m->mUnk_13c += 1;
                    } else if (mode == 0) {
                        data_020c31dc.mUnk_3c = ((arg & 0xf0) << 4) + (arg & 0xf);
                        m->mUnk_13c += 1;
                    }
                    m->mUnk_172_0 = 1;
                    break;
                // Set data_020c3e44.mUnk_2024.mUnk_9c.mUnk_1ec from entry `id` (func_0203204c), or clear it for id -1.
                case 24: {
                    u16 id;
                    void *entry;
                    if (mode == 2) {
                        id = val;
                        m->mUnk_13c += 2;
                    } else if (mode == 1) {
                        id = func_020264d4(m, arg);
                        m->mUnk_13c += 1;
                    } else if (mode == 0) {
                        id = ((arg & 0xf0) << 4) + (arg & 0xf);
                        m->mUnk_13c += 1;
                    }
                    // The test against -1 is always true: id is 16-bit.
                    if (id != -1) {
                        entry = func_0203204c(id, 0);
                        if (entry == NULL) {
                            OS_Terminate();
                        }
                        data_020c3e44.mUnk_2024.mUnk_9c.mUnk_1ec.mUnk_00.x = func_02031f98(entry);
                        data_020c3e44.mUnk_2024.mUnk_9c.mUnk_1ec.mUnk_00.y = func_02031fa4(entry);
                        data_020c3e44.mUnk_2024.mUnk_9c.mUnk_1ec.mUnk_00.z = func_02031fb0(entry);
                    } else {
                        data_020c3e44.mUnk_2024.mUnk_9c.mUnk_1ec.mUnk_00.x = 0;
                        data_020c3e44.mUnk_2024.mUnk_9c.mUnk_1ec.mUnk_00.y = 0;
                        data_020c3e44.mUnk_2024.mUnk_9c.mUnk_1ec.mUnk_00.z = 0;
                    }
                    break;
                }
                // Set data_020c31dc.mUnk_3e and reset mUnk_3c.
                case 21:
                    if (mode == 2) {
                        data_020c31dc.mUnk_3e = val;
                        m->mUnk_13c += 2;
                    } else if (mode == 1) {
                        data_020c31dc.mUnk_3e = func_020264d4(m, arg);
                        m->mUnk_13c += 1;
                    } else if (mode == 0) {
                        data_020c31dc.mUnk_3e = ((arg & 0xf0) << 4) + (arg & 0xf);
                        m->mUnk_13c += 1;
                    }
                    data_020c31dc.mUnk_3c = 0xffff;
                    m->mUnk_172_0 = 1;
                    m->mUnk_172_1 = 1;
                    break;
                // Set flag bit 2 from the argument.
                case 17:
                    m->mUnk_024 = (m->mUnk_024 & ~4) | (arg << 2);
                    m->mUnk_13c += 1;
                    break;
                // Set mUnk_148, mUnk_144 and mUnk_146.
                case 16:
                    if (mode == 2) {
                        m->mUnk_148 = arg;
                    } else {
                        m->mUnk_148 = func_020264d4(m, arg);
                    }
                    m->mUnk_144 = val;
                    m->mUnk_146 = m->mUnk_13c[2];
                    m->mUnk_13c += 3;
                    break;
                // Set event `arg` (target and key) and its bit in mUnk_130; index -1 uses entry 32 and sets flag bit 2.
                case 15:
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
                case 32:
                    if (mode == 1) {
                        val = func_020264d4(m, val);
                    }
                    m->mUnk_170 = val;
                    func_02026384(m, &slot, 0, arg & 0x7f, 0, 0);
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
                case 14:
                    if (mode == 1) {
                        val = func_020264d4(m, val);
                    }
                    func_02026384(m, &slot, val, arg & 0x7f, (arg >> 7) << 5, 0);
                    slot++;
                    m->mUnk_13c += 2;
                    m->mUnk_172_14 = 0;
                    break;
                // Clear mUnk_142, set bit `arg` of mUnk_134 and return 0.
                case 19:
                    m->mUnk_142 = 0;
                    m->mUnk_134 |= 1 << arg;
                    return 0;
                // Advance the animation and return 1: with mUnk_172_14 set, clear it and step past, else set
                // mUnk_172_9 and mUnk_172_7.
                case 18:
                    if (m->mUnk_172_14 == 1) {
                        m->mUnk_172_14 = 0;
                        m->mUnk_13c += 1;
                    } else {
                        m->mUnk_172_9 = 1;
                        m->mUnk_172_7 = 1;
                    }
                    func_020266a8(m, &slot);
                    return 1;
                // Set mUnk_172_13; with mUnk_172_10 set, step past and copy the animation state, else go on as 22.
                case 23:
                    m->mUnk_172_13 = 1;
                    if (m->mUnk_172_10) {
                        m->mUnk_13c += 1;
                        func_0202635c(m, &slot);
                        break;
                    }
                    // fall through
                // Stop for this frame (advance the animation; the command runs again) while the mask bits are set in
                // mUnk_134 and clear in mUnk_138 and mUnk_172_14 is clear, or while mUnk_172_12 is set; else step past.
                case 22: {
                    u32 mask;
                    if (mode == 2) {
                        mask = 1 << arg;
                    } else {
                        mask = 1 << func_020264d4(m, arg);
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

void func_020271c8(void) {
    data_020c31dc.mUnk_49 = 0;
    data_020c31dc.mUnk_48 = 0;
    data_020c31dc.mUnk_18[0].mUnk_00 = NULL;
}

void func_020271e0(Model *m) {
    m->mUnk_14c.mUnk_0 = 0;
}

void func_020271ec(void *obj, int id, int anim, int count, u32 script) {
    func_02027200(obj, id, anim, count, 0, script);
}

void func_02027200(Model *m, int id, int anim, int count, int mode, u32 script) {
    MI_CpuFill8(m, 0, sizeof(Model));
    func_02027244(m, id, anim, count, mode, script);
}

void func_02027230(void *obj, int id, int anim, int count, u32 script) {
    func_02027244(obj, id, anim, count, 0, script);
}

void func_02027244(Model *m, int id, int anim, int count, int mode, u32 script) {
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
        if (func_02026718(m) == 0) {
            func_020262a0(m);
        }
    } else {
        s32 slot = 0;
        func_02026384(m, &slot, anim, count, 0, mode);
    }
    m->mUnk_152.mUnk_0 = m->mUnk_14c.mUnk_0;
    m->mUnk_174 = 0;
    func_020271c8();
}

void func_0202733c(void *obj, void *params, int flags) {
    Model *m = obj;
    VecFx32 pos;
    func_020262a0(m);
    if (m->mUnk_170 != 0 && !(flags & 0x80)) {
        func_02024b5c(m->mUnk_170, params, m, flags);
        if (m->mUnk_175 != 0xff) {
            func_02023f0c(&pos, m->mUnk_175);
            m->mUnk_158.x = pos.x;
            m->mUnk_158.y = pos.y;
            m->mUnk_158.z = pos.z;
        }
    } else {
        func_020271c8();
    }
}
