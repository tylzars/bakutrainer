#include <nitro/os.h>
#include <stdio.h>
#include <stdlib.h>
#include <Main/Unknown/functions.h>
#include <Main/Unknown/globals.h>
#include <Main/Unknown/game_util.h>
#include <Main/Unknown/stage_data.h>
#include <Main/Unknown/stage_camera.h>

void func_02015d4c(void) {
    data_020b6ee0.mUnk_a146 = 0;
}

void func_02015d58(int stage) {
    StageRec *rec = &data_020b1990[stage];
    int angle = rec->mUnk_220 >> 4;
    int dist;

    if (stage < 0 || stage >= STAGE_MAX) {
        OS_Terminate();
    }
    data_020c5e44.mUnk_b0.x = rec->mUnk_158.x;
    data_020c5e44.mUnk_b0.z = rec->mUnk_158.z;
    data_020c5e44.mUnk_b0.y = rec->mUnk_158.y;
    data_020c5e44.mUnk_a4.x = rec->mUnk_1a0.x - 400 * FX_SinCosTable_[(rec->mUnk_224 >> 4) * 2];
    data_020c5e44.mUnk_a4.z = rec->mUnk_1a0.z + 400 * FX_SinCosTable_[(rec->mUnk_224 >> 4) * 2 + 1];
    dist = rec->mUnk_1a0.y + func_0201fb74(angle, 0xc00, 0x320000, 0x4b0000);
    data_020c5e44.mUnk_a4.y = dist;
    if (dist > 0x5dc000) {
        data_020c5e44.mUnk_a4.y = 0x5dc000;
    }
    func_02033488();
}

void func_02015e08(int stage) {
    StageRec *rec = &data_020b1990[stage];

    if (stage < 0 || stage >= STAGE_MAX) {
        OS_Terminate();
    }
    data_020c5e44.mUnk_b0.x = rec->mUnk_158.x;
    data_020c5e44.mUnk_b0.z = rec->mUnk_158.z;
    data_020c5e44.mUnk_b0.y = rec->mUnk_158.y;
    data_020c5e44.mUnk_a4.x = rec->mUnk_1a0.x - 300 * FX_SinCosTable_[(rec->mUnk_224 >> 4) * 2];
    data_020c5e44.mUnk_a4.z = rec->mUnk_1a0.z + 300 * FX_SinCosTable_[(rec->mUnk_224 >> 4) * 2 + 1];
    data_020c5e44.mUnk_a4.y = rec->mUnk_1a0.y + 0x64000;
    func_02033488();
}

void func_02015e94(int stage) {
    if (stage < -1 || stage >= STAGE_MAX) {
        OS_Terminate();
    }
    if (stage != -1) {
        if (data_020b6ee0.mUnk_a146 <= 1) {
            func_02015d58(stage);
        } else {
            func_02015e08(stage);
        }
        return;
    }
    data_020b6ee0.mUnk_a574.mUnk_00.x = data_020b6ee0.mUnk_a574.mUnk_0c.x - 600 * FX_SinCosTable_[0];
    data_020b6ee0.mUnk_a574.mUnk_00.z = data_020b6ee0.mUnk_a574.mUnk_0c.z + 600 * FX_SinCosTable_[1];
    data_020b6ee0.mUnk_a574.mUnk_00.y = data_020b6ee0.mUnk_a574.mUnk_0c.y + 0x3e8000;
    func_0200b960(&data_020c5e44.mUnk_b0, data_020b6ee0.mUnk_a574.mUnk_0c, data_020b6ee0.mUnk_a574.mUnk_34);
    func_0200b960(&data_020c5e44.mUnk_a4, data_020b6ee0.mUnk_a574.mUnk_00, data_020b6ee0.mUnk_a574.mUnk_34);
    func_02033488();
}

void func_02015f34(void) {
    int i;
    DrawParams params;
    char text[12];
    s32 pos[2];
    Unk_020b6ee8 *obj;

    for (i = 0; i < data_020b6ee0.mUnk_0c48; i++) {
        obj = &data_020b6ee0.mUnk_0008[i];
        if (obj->mModel.mUnk_170 != 0) {
            func_02023f58(&params, obj->mUnk_178 << 9, 0, obj->mUnk_17c << 9, 0x1000, obj->mUnk_185 << 8, 0, 0xff,
                          0x100);
            if (func_02031d0c(&params.mPos, 0x12c000, 0x190000, 0x190000) == 1) {
                func_0202733c(&obj->mModel, &params, 0);
                if (obj->mUnk_184 == 10) {
                    sprintf(text, "%d", obj->mUnk_186);
                    func_02025db4(&params, pos);
                    func_020215fc(pos[0], pos[1], data_0201a61c, text, 2, 0x82, 0x10, 1);
                }
            }
        }
    }
}

void func_02016008(int stage) {
    StageRec *rec = &data_020b1990[stage];
    s32 px = rec->mUnk_1a0.x / 512;
    s32 pz = rec->mUnk_1a0.z / 512;
    int moved = 0;
    Unk_020b7ef4 *f;
    Unk_020b7ef4 *b;
    int i;
    int d;
    int dx;
    int dz;
    DrawParams params;
    VecFx32 pos;

    if (stage < 0 || stage >= STAGE_MAX) {
        OS_Terminate();
    }
    f = data_020b6ee0.mUnk_1014;
    for (i = 0; i < data_020b6ee0.mUnk_5a54; i++, f++) {
        dx = f->mUnk_178 - px;
        dz = f->mUnk_17c - pz;
        if (f->mUnk_182 == 0x19) {
            d = func_0201faf0(dx, dz);
            if (d < 400) {
                break;
            }
        } else if (f->mUnk_182 == 0x14) {
            if (abs(dx) < 400 && abs(dz) < 800) {
                f->mUnk_188++;
                f->mModel.mUnk_134 |= 1;
                if (f->mUnk_188 > (f->mUnk_187 + 1) * 6) {
                    rec->mUnk_1d8 += 0x8000;
                    rec->mUnk_220 += 0x2000;
                    f->mUnk_188 = 0;
                    func_02018840(rec, 0x14, i);
                    f->mModel.mUnk_134 |= 2;
                    func_0202f110(0x2b, 0x64, 0, 0x40, 0xffff, 0);
                }
            } else {
                f->mUnk_188 = 0;
            }
        } else {
            d = abs(dx) + abs(dz);
            if (d < 400) {
                break;
            }
        }
    }
    if (i != data_020b6ee0.mUnk_5a54) {
        switch (f->mUnk_182) {
            case 0xf:
                rec->mUnk_1a0 = rec->mUnk_210;
                rec->mUnk_1d8 += 0x8000;
                rec->mUnk_220 = 0x2000;
                func_02018840(rec, 0xf, i);
                break;
            case 0x19: {
                int q = 0x32000 / d;
                int ax = f->mUnk_178;
                int az = f->mUnk_17c;
                ax = px - ax;
                az = pz - az;
                ax *= q;
                az *= q;
                rec->mUnk_1a0.x = ax + (f->mUnk_178 << 9);
                rec->mUnk_1a0.z = az + (f->mUnk_17c << 9);
                rec->mUnk_1d8 = func_0201feb0(rec->mUnk_1a0.x / 512 - f->mUnk_178, rec->mUnk_1a0.z / 512 - f->mUnk_17c);
                rec->mUnk_220 += 0x4000;
                func_0202f110(0x28, 0x64, 0, 0x40, 0xffff, 0);
                break;
            }
            case 0x23:
                rec->mUnk_1d8 = ((f->mUnk_184 + 1) & 3) << 14;
                rec->mUnk_220 = 0x14000;
                func_0202f110(1, 0x64, 0, 0x40, 0xffff, 0);
                break;
            case 0x28:
                moved = 1;
                if (rec->mUnk_22c == 0) {
                    u8 key = f->mUnk_185;
                    Unk_020b7ef4 *p = data_020b6ee0.mUnk_1014;
                    int j;

                    func_0201f880(moved);
                    func_0202f110(0x59, 0x64, 0, 0x40, 0xffff, 0);
                    rec->mUnk_220 = 0;
                    for (j = 0; j < data_020b6ee0.mUnk_5a54; j++, p++) {
                        if (p->mUnk_182 == 0x28 && p->mUnk_186 == key) {
                            rec->mUnk_1a0.x = p->mUnk_178 << 9;
                            rec->mUnk_1a0.z = p->mUnk_17c << 9;
                            break;
                        }
                    }
                }
                break;
        }
    } else {
        rec->mUnk_210 = rec->mUnk_1a0;
    }
    rec->mUnk_22c = moved;
    b = data_020b6ee0.mUnk_1014;
    i = 0;
    while (i < data_020b6ee0.mUnk_5a54) {
        pos.x = b->mUnk_178 << 9;
        pos.y = 0;
        pos.z = b->mUnk_17c << 9;
        if (func_02031d0c(&pos, 0x190000, 0x190000, 0x190000) == 1) {
            switch (b->mUnk_182) {
                case 0x19:
                    if (b == f) {
                        b->mModel.mUnk_134 |= 1;
                    }
                    func_02023f58(&params, pos.x, pos.y, pos.z, 0x1000, 0, 0, 0xff, 0x100);
                    func_0202733c(&b->mModel, &params, 0);
                    break;
                case 0x14:
                    pos.y = 0;
                    func_02023f58(&params, pos.x, pos.y, pos.z, 0x1000, (u16) (((b->mUnk_184 + 1) & 3) << 14), 0, 0xff,
                                  0x100);
                    func_0202733c(&b->mModel, &params, 0);
                    break;
                case 0x28:
                    pos.y = 0;
                    func_02023f58(&params, pos.x, pos.y, pos.z, 0x1000, (u16) (((b->mUnk_184 + 1) & 3) << 14), 0, 0xff,
                                  0x100);
                    func_0202733c(&b->mModel, &params, 0);
                    break;
                case 0xf:
                    pos.y = 0;
                    func_02023f58(&params, pos.x, pos.y, pos.z, 0x1000, (u16) (((b->mUnk_184 + 1) & 3) << 14), 0, 0xff,
                                  0x100);
                    func_020248e8(0xc87, &params, 0);
                    break;
                case 0x23:
                    pos.y = 0;
                    func_02023f58(&params, pos.x, pos.y, pos.z, 0x1000, (b->mUnk_184 & 3) << 14, 0, 0xff, 0x100);
                    func_020248e8(0xc8a, &params, 0);
                    break;
            }
        }
        i++;
        b++;
    }
}

void func_020164e0(int stage) {
    StageRec *rec = &data_020b1990[stage];
    s32 px = rec->mUnk_1a0.x / 512;
    s32 pz = rec->mUnk_1a0.z / 512;
    DrawParams params;
    VecFx32 pos;
    int i;
    Unk_020bc938 *obj = data_020b6ee0.mUnk_5a58;
    int found;
    int dx;
    int dz;

    if (stage < 0 || stage >= STAGE_MAX) {
        OS_Terminate();
    }
    func_02023f58(&params, 0, 0, 0, 0x1000, 0, 2, 0xff, 0);
    i = 0;
    pos.y = 0;
    found = -1;
    while (i < data_020b6ee0.mUnk_8ad8) {
        if (obj->mUnk_183 == 1) {
            pos.x = obj->mUnk_00 << 9;
            pos.z = obj->mUnk_04 << 9;
            if (func_02031d0c(&pos, 0x190000, 0x190000, 0x190000) == 1) {
                params.mPos.x = pos.x;
                params.mPos.z = pos.z;
                if (obj->mUnk_180 == 0x1e) {
                    func_0202733c(&obj->mModel, &params, 0);
                }
                if (found == -1) {
                    dx = obj->mUnk_00;
                    dz = obj->mUnk_04;
                    dx -= px;
                    dz -= pz;
                    if (abs(dx) + abs(dz) < 400) {
                        found = i;
                    }
                }
            }
        }
        i++;
        obj++;
    }
    if (found != -1) {
        obj = &data_020b6ee0.mUnk_5a58[found];
        if (obj->mUnk_180 == 0x1e) {
            func_02018840(rec, 0x1e, found);
            obj->mUnk_183 = 0;
            func_0202f110(0xe0, 0x64, 0, 0x40, 0xffff, 0);
        }
    }
}
