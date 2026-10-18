#include <nitro/os.h>
#include <stdio.h>
#include <Main/clib.h>
#include <Main/Unknown/functions.h>
#include <Main/Unknown/globals.h>
#include <Main/Unknown/game_util.h>
#include <Main/Unknown/stage_data.h>
#include <Main/Unknown/stage_camera.h>
#include <Main/Unknown/stage_progress.h>

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

void func_02016624(void) {
    int n5 = 0;
    int n34 = 0;
    int n7 = 0;
    int n30 = 0;
    u16 cnt;
    u16 *q;
    VecFx32 pos;
    u8 flags;

    memset(data_020b6ee0.mUnk_8adc, 0, 0x384);
    memset(&data_020b6ee0.mUnk_0c4c, 0, 0x10);
    memset(&data_020b1598, 0, 0x3f8);
    data_020b6ee0.mUnk_8ad9 = 0;
    q = func_02031eb8(0x5514);
    cnt = *q;
    q += 2;
    while (cnt-- != 0) {
        int extra = *q & 0xf;
        int type = (*q >> 4) & 0x7f;

        func_02032018(q[1], &pos, &flags);
        if (type == 0xf) {
            data_020b6ee0.mUnk_1014[n7].mUnk_182 = type;
            data_020b6ee0.mUnk_1014[n7].mUnk_178 = pos.x / 512;
            data_020b6ee0.mUnk_1014[n7].mUnk_17c = pos.z / 512;
            data_020b6ee0.mUnk_1014[n7].mUnk_183 = func_0201fdbc(q + 2, 0, 3);
            n7++;
        } else if (type == 0x14) {
            data_020b6ee0.mUnk_1014[n7].mUnk_182 = type;
            data_020b6ee0.mUnk_1014[n7].mUnk_178 = pos.x / 512;
            data_020b6ee0.mUnk_1014[n7].mUnk_17c = pos.z / 512;
            data_020b6ee0.mUnk_1014[n7].mUnk_187 = func_0201fdbc(q + 2, 0, 1);
            data_020b6ee0.mUnk_1014[n7].mUnk_188 = 0;
            data_020b6ee0.mUnk_1014[n7].mUnk_184 = (u32) flags >> 6;
            func_020271ec(&data_020b6ee0.mUnk_1014[n7].mModel, 0, 0, 0, data_0201aba0[6]);
            n7++;
        } else if (type == 0x19) {
            data_020b6ee0.mUnk_1014[n7].mUnk_182 = type;
            data_020b6ee0.mUnk_1014[n7].mUnk_178 = pos.x / 512;
            data_020b6ee0.mUnk_1014[n7].mUnk_17c = pos.z / 512;
            func_020271ec(&data_020b6ee0.mUnk_1014[n7].mModel, 0, 0, 0, data_0201aba0[0]);
            n7++;
        } else if (type == 0x1e) {
            data_020b6ee0.mUnk_5a58[n30].mUnk_180 = type;
            data_020b6ee0.mUnk_5a58[n30].mUnk_00 = pos.x / 512;
            data_020b6ee0.mUnk_5a58[n30].mUnk_04 = pos.z / 512;
            data_020b6ee0.mUnk_5a58[n30].mUnk_183 = 1;
            data_020b6ee0.mUnk_5a58[n30].mUnk_181[1] = func_0201fdbc(q + 2, 0, 0);
            data_020b6ee0.mUnk_5a58[n30].mUnk_181[0] = func_0201fdbc(q + 2, 1, 7);
            if (data_020b6ee0.mUnk_5a58[n30].mUnk_181[1] == 0) {
                func_020271ec(&data_020b6ee0.mUnk_5a58[n30].mModel, 0xc89, 0xc88, 2, 0);
                data_020b6ee0.mUnk_8ad9++;
            } else {
                func_020271ec(&data_020b6ee0.mUnk_5a58[n30].mModel, 0xc8c, 0xc8b, 2, 0);
            }
            n30++;
        } else if (type == 0x23) {
            data_020b6ee0.mUnk_1014[n7].mUnk_182 = type;
            data_020b6ee0.mUnk_1014[n7].mUnk_178 = pos.x / 512;
            data_020b6ee0.mUnk_1014[n7].mUnk_17c = pos.z / 512;
            data_020b6ee0.mUnk_1014[n7].mUnk_184 = (u32) flags >> 6;
            n7++;
        } else if (type == 0x28) {
            data_020b6ee0.mUnk_1014[n7].mUnk_182 = type;
            data_020b6ee0.mUnk_1014[n7].mUnk_178 = pos.x / 512;
            data_020b6ee0.mUnk_1014[n7].mUnk_17c = pos.z / 512;
            data_020b6ee0.mUnk_1014[n7].mUnk_185 = func_0201fdbc(q + 2, 0, 3);
            data_020b6ee0.mUnk_1014[n7].mUnk_186 = func_0201fdbc(q + 2, 4, 7);
            func_020271ec(&data_020b6ee0.mUnk_1014[n7].mModel, 0xc8e, 0xc8d, 2, 0);
            n7++;
        } else if (type == 5) {
            data_020b6ee0.mUnk_8adc[n5].mUnk_00 = pos;
            data_020b6ee0.mUnk_8adc[n5].mUnk_18 = func_0201fdbc(q + 2, 0, 0);
            data_020b6ee0.mUnk_8adc[n5].mUnk_1a = 1;
            n5++;
        } else if (type == 0xa || type == 0x2d) {
            data_020b6ee0.mUnk_0008[n34].mUnk_178 = pos.x / 512;
            data_020b6ee0.mUnk_0008[n34].mUnk_17c = pos.z / 512;
            data_020b6ee0.mUnk_0008[n34].mUnk_185 = flags;
            data_020b6ee0.mUnk_0008[n34].mUnk_184 = type;
            if (type == 0xa) {
                data_020b6ee0.mUnk_0008[n34].mUnk_186 = func_0201fdbc(q + 2, 0, 2);
                func_020271ec(&data_020b6ee0.mUnk_0008[n34].mModel, 0, 0, 0, data_0201aba0[1]);
            } else if (type == 0x2d) {
                data_020b6ee0.mUnk_0008[n34].mUnk_187 = func_0201fdbc(q + 2, 0, 2);
                data_020b6ee0.mUnk_0008[n34].mUnk_186 = 0x20;
                func_020271ec(&data_020b6ee0.mUnk_0008[n34].mModel, 0, 0, 0, data_0201aba0[2]);
            }
            n34++;
        }
        q += extra + 2;
    }
    if (n34 >= 8) {
        OS_Terminate();
    }
    data_020b6ee0.mUnk_0c48 = n34;
    if (n5 >= 0x20) {
        OS_Terminate();
    }
    data_020b6ee0.mUnk_8e5c = n5;
    if (n7 >= 0x30) {
        OS_Terminate();
    }
    data_020b6ee0.mUnk_5a54 = n7;
    if (n30 >= 0x20) {
        OS_Terminate();
    }
    data_020b6ee0.mUnk_8ad8 = n30;
    data_020b1598.mUnk_3f0 = 1;
    data_020b1598.mUnk_3f2 = 600;
    data_020b1598.mUnk_3f4 = 0x640;
    data_020b1598.mUnk_3f6 = 1;
}

void func_02016ae8(int stage) {
    StageRec *rec = &data_020b1990[stage];
    int n;
    int i;

    if (stage < 0 || stage >= STAGE_MAX) {
        OS_Terminate();
    }
    n = 0;
    for (i = 0; i < data_020b6ee0.mUnk_8e5c; i++) {
        if (i >= 0x20) {
            OS_Terminate();
        }
        if (n >= 0x11) {
            OS_Terminate();
        }
        if (data_020b6ee0.mUnk_8adc[i].mUnk_1a != 0) {
            int dx = abs(data_020b6ee0.mUnk_8adc[i].mUnk_00.x - rec->mUnk_1a0.x);
            int dz = abs(data_020b6ee0.mUnk_8adc[i].mUnk_00.z - rec->mUnk_1a0.z);
            if (dx + dz < 0x4b0000) {
                data_020b1598.mUnk_000[n].mUnk_00.x = data_020b6ee0.mUnk_8adc[i].mUnk_00.x / 512;
                data_020b1598.mUnk_000[n].mUnk_00.y = data_020b6ee0.mUnk_8adc[i].mUnk_00.y / 512;
                data_020b1598.mUnk_000[n].mUnk_00.z = data_020b6ee0.mUnk_8adc[i].mUnk_00.z / 512;
                data_020b1598.mUnk_000[n].mUnk_20 = 300;
                data_020b1598.mUnk_000[n].mUnk_1c = 32.0f;
                data_020b1598.mUnk_000[n].mUnk_36 = i;
                data_020b6ee0.mUnk_8adc[i].mUnk_19 = 1;
                n++;
            } else {
                data_020b6ee0.mUnk_8adc[i].mUnk_19 = 0;
            }
        }
    }
    if (data_020b6ee0.mUnk_0c5a != 0) {
        data_020b6ee0.mUnk_0c5a--;
        data_020b1598.mUnk_000[n].mUnk_00.x = data_020b6ee0.mUnk_0c50;
        data_020b1598.mUnk_000[n].mUnk_00.z = data_020b6ee0.mUnk_0c54;
        data_020b1598.mUnk_000[n].mUnk_20 = 0;
        data_020b1598.mUnk_3f7 = 1;
    } else {
        data_020b1598.mUnk_000[n].mUnk_00.x = rec->mUnk_1a0.x / 512;
        data_020b1598.mUnk_000[n].mUnk_00.y = rec->mUnk_1a0.y / 512;
        data_020b1598.mUnk_000[n].mUnk_00.z = rec->mUnk_1a0.z / 512;
        data_020b1598.mUnk_000[n].mUnk_20 = 900;
        data_020b1598.mUnk_000[n].mUnk_1c = 0;
        data_020b1598.mUnk_3f7 = 0;
    }
    data_020b1598.mUnk_3f0 = n;
    func_02017298(&data_020b1598);
    for (i = 0; i < n; i++) {
        int idx = data_020b1598.mUnk_000[i].mUnk_36;
        if (data_020b6ee0.mUnk_8adc[idx].mUnk_19 == 1) {
            data_020b6ee0.mUnk_8adc[idx].mUnk_00.x = 512.0f * data_020b1598.mUnk_000[i].mUnk_00.x;
            data_020b6ee0.mUnk_8adc[idx].mUnk_00.y = 512.0f * data_020b1598.mUnk_000[i].mUnk_00.y;
            data_020b6ee0.mUnk_8adc[idx].mUnk_00.z = 512.0f * data_020b1598.mUnk_000[i].mUnk_00.z;
        }
    }
    for (i = 0; i < data_020b6ee0.mUnk_8e5c; i++) {
        if (data_020b6ee0.mUnk_8adc[i].mUnk_19 == 1) {
            if (func_02031dc4(data_020b6ee0.mUnk_8adc[i].mUnk_00.x, data_020b6ee0.mUnk_8adc[i].mUnk_00.z) == 1) {
                func_02025db4(&data_020b6ee0.mUnk_8adc[i], data_020b6ee0.mUnk_8adc[i].mUnk_0c);
            }
        }
    }
    for (i = 0; i < data_020b6ee0.mUnk_8e5c; i++) {
        if (data_020b6ee0.mUnk_8adc[i].mUnk_19 == 1) {
            func_0200ba94(&data_020b6ee0.mUnk_8adc[i].mUnk_00, 0x38400, 0x400);
            func_0202cf08(data_020b6ee0.mUnk_8adc[i].mUnk_18 == 0 ? 0xe24 : 0xe23,
                          data_020b6ee0.mUnk_8adc[i].mUnk_0c[0], data_020b6ee0.mUnk_8adc[i].mUnk_0c[1], 0, 1);
        }
    }
}

void func_02016db4(Unk_020b1598 *followers, int index, u16 angle) {
    int bestIdx = 0;
    Unk_Follower *f = &followers->mUnk_000[index];
    StageRec *rec = &data_020b1990[data_020b6ee0.mUnk_a8e4];
    int bestDist;
    int j;
    int k;

    if (data_020b6ee0.mUnk_a8e4 < 0 || data_020b6ee0.mUnk_a8e4 >= STAGE_MAX) {
        OS_Terminate();
    }
    if (f->mUnk_36 >= 0x20) {
        OS_Terminate();
    }
    bestDist = followers->mUnk_3f4;
    for (k = 0; k < 8; k++) {
        f->mUnk_26[k] = 30000;
    }
    if (followers->mUnk_3f6 == 1) {
        VecFx32 pos;
        pos.x = 512.0f * f->mUnk_00.x;
        pos.y = 512.0f * f->mUnk_00.y;
        pos.z = 512.0f * f->mUnk_00.z;
        if (func_02031824(&pos, f->mUnk_20, 0x6e, 0, 0x13, 0)) {
            if ((data_020c5e44.mUnk_5e & 0x10) && data_020b6ee0.mUnk_0c5a != 0 &&
                data_020b6ee0.mUnk_8adc[f->mUnk_36].mUnk_18 == 0 && data_020b6ee0.mUnk_0c4c == data_020c5e44.mUnk_34 &&
                data_020b6ee0.mUnk_0008[data_020b6ee0.mUnk_0c58].mUnk_186 > 0) {
                data_020b6ee0.mUnk_0008[data_020b6ee0.mUnk_0c58].mModel.mUnk_134 |= 1;
                data_020b6ee0.mUnk_0008[data_020b6ee0.mUnk_0c58].mUnk_186--;
                if (data_020b6ee0.mUnk_0008[data_020b6ee0.mUnk_0c58].mUnk_186 == 0) {
                    func_02031c98(data_020b6ee0.mUnk_0c4c, 0, 0);
                    data_020b6ee0.mUnk_0008[data_020b6ee0.mUnk_0c58].mModel.mUnk_134 |= 2;
                    func_0202f110(0xdb, 0x64, 0, 0x40, 0xffff, 0);
                } else {
                    func_0202f110(0xdd, 0x64, 0, 0x40, 0xffff, 0);
                }
                data_020b6ee0.mUnk_8adc[f->mUnk_36].mUnk_1a = 0;
                data_020b6ee0.mUnk_8adc[f->mUnk_36].mUnk_19 = 0;
            }
            {
                u32 cam = func_0203129c();
                int cross = func_0201febc(data_020c5e44.mUnk_38, data_020c5e44.mUnk_3c, data_020c5e44.mUnk_44,
                                          data_020c5e44.mUnk_48, f->mUnk_00.x, f->mUnk_00.z);
                f->mUnk_00 = f->mUnk_0c;
                f->mUnk_24 = cam + (((cross != 0) ? ((cross >> 31) | 1) : 0) << 14);
                f->mUnk_22 = f->mUnk_24;
            }
        }
    }
    for (j = 0; j < followers->mUnk_3f0 + 1; j++) {
        Unk_Follower *o;
        int dx;
        int dz;
        int dist;

        o = &followers->mUnk_000[j];
        if (j != index) {
            dx = o->mUnk_00.x - f->mUnk_00.x;
            dz = o->mUnk_00.z - f->mUnk_00.z;
            dist = func_0201faf0(dx, dz);
            if (dist < followers->mUnk_3f4) {
                u32 ang = func_0201feb0(dx, dz);
                const s16 *s1 = &FX_SinCosTable_[((u16) (ang - 0x4000) >> 4) * 2];
                int ax = f->mUnk_00.x + (float) (f->mUnk_20 * s1[0] / 2 / 0x1000);
                int az = f->mUnk_00.z - (float) (f->mUnk_20 * s1[1] / 2 / 0x1000);
                const s16 *s2 = &FX_SinCosTable_[((u16) (ang + 0x4000) >> 4) * 2];
                int bx = o->mUnk_00.x + (float) (o->mUnk_20 * s2[0] / 2 / 0x1000);
                int bz = o->mUnk_00.z - (float) (o->mUnk_20 * s2[1] / 2 / 0x1000);
                u32 span = func_0201fe14(ang, func_0201feb0(bx - ax, bz - az));
                u16 cur = ang - (u16) span;
                u32 end = ang + span;

                while (func_0201fe6c(end, cur) == 1) {
                    u16 sector = ((cur + 0x1000) / 0x2000) & 7;
                    if (f->mUnk_26[sector] > dist) {
                        f->mUnk_26[sector] = dist;
                    }
                    cur += 0x200;
                }
                if (f->mUnk_26[(u32) f->mUnk_24 >> 13] < bestDist) {
                    bestDist = dist;
                    bestIdx = j;
                }
            }
        }
    }
    if (bestDist < followers->mUnk_3f4) {
        int avg = (f->mUnk_20 + followers->mUnk_000[bestIdx].mUnk_20) / 2;
        int limit = followers->mUnk_3f2 + avg;

        if (bestDist >= limit) {
            bestDist = limit;
        }
        f->mUnk_18 = f->mUnk_1c * (float) func_0201fbe4(bestDist, avg, limit, 0, 0x400) / 1024.0f;
        f->mUnk_37 = 1;
        if (f->mUnk_18 < 0.1) {
            if (data_020b6ee0.mUnk_8adc[f->mUnk_36].mUnk_18 == 1 && bestIdx == followers->mUnk_3f0 &&
                data_020b1598.mUnk_3f7 == 0) {
                data_020b6ee0.mUnk_8adc[f->mUnk_36].mUnk_1a = 0;
                data_020b6ee0.mUnk_8adc[f->mUnk_36].mUnk_19 = 0;
                func_02018840(rec, 5, -1);
            }
        }
    } else {
        f->mUnk_37 = 0;
        if (f->mUnk_18 != f->mUnk_1c) {
            f->mUnk_18 = f->mUnk_18 + (f->mUnk_1c - f->mUnk_18) / 16.0f;
        }
    }
}

void func_02017298(Unk_020b1598 *followers) {
    Unk_Follower *target = &followers->mUnk_000[followers->mUnk_3f0];
    int i;

    if (followers->mUnk_3f0 >= 0x11) {
        OS_Terminate();
    }
    for (i = 0; i < followers->mUnk_3f0; i++) {
        Unk_Follower *f = &followers->mUnk_000[i];
        const s16 *s1;
        const s16 *s2;
        float fa;
        float fb;
        float fc;
        float fd;
        float px;
        float pz;
        u32 ang;
        int diff;
        int bi;
        u32 sec;
        int best;
        int j;

        s1 = &FX_SinCosTable_[(f->mUnk_24 >> 4) * 2];
        fa = (f->mUnk_20 * s1[0] * 2) / 0x1000;
        fb = (f->mUnk_20 * -s1[1] * 2) / 0x1000;
        s2 = &FX_SinCosTable_[(f->mUnk_22 >> 4) * 2];
        fc = (f->mUnk_20 * s2[0] * 3) / 0x1000;
        fd = (f->mUnk_20 * -s2[1] * 3) / 0x1000;
        px = fa + f->mUnk_00.x;
        pz = fb + f->mUnk_00.z;
        px = fc + px;
        pz = fd + pz;
        diff = func_0201fe34(f->mUnk_24, (u16) func_0201feb0(px - f->mUnk_00.x, pz - f->mUnk_00.z));
        if (abs(diff) > 0x100) {
            f->mUnk_24 += diff / 8;
        }
        if (0 != f->mUnk_1c) {
            func_02016db4(followers, i, f->mUnk_24);
        }
        f->mUnk_0c = f->mUnk_00;
        f->mUnk_00.x = f->mUnk_00.x + f->mUnk_18 * (float) FX_SinCosTable_[(f->mUnk_24 >> 4) * 2] / 4096.0f;
        f->mUnk_00.z = f->mUnk_00.z - f->mUnk_18 * (float) FX_SinCosTable_[(f->mUnk_24 >> 4) * 2 + 1] / 4096.0f;
        ang = func_0201feb0(target->mUnk_00.x - f->mUnk_00.x, target->mUnk_00.z - f->mUnk_00.z);
        if (f->mUnk_37 == 1 && 2.0f * f->mUnk_18 < f->mUnk_1c) {
            best = 0;
            sec = ang >> 13;

            for (j = 0; j < 4; j++) {
                int off = (j & 1) ? -(j / 2 + 1) : (j / 2 + 1);
                int s = (sec + off) & 7;
                if (f->mUnk_26[s] > best) {
                    best = f->mUnk_26[s];
                    bi = s;
                }
            }
            if (best > f->mUnk_20) {
                ang = (u16) ((bi << 13) + 0x1000);
            }
        }
        diff = func_0201fe34(f->mUnk_22, ang);
        if (abs(diff) > 0x100) {
            f->mUnk_22 = f->mUnk_22 + diff / 8;
        } else {
            f->mUnk_22 = ang;
        }
    }
}

#pragma thumb off
int func_0201750c(int stage, int *out) {
    StageRec *rec = &data_020b1990[stage];

    if (data_020c18bc.mUnk_2ac_2 && rec->mUnk_22f == 1) {
        if ((int) func_0201faf0(data_020c18bc.mUnk_2ae - rec->mUnk_1f0, data_020c18bc.mUnk_2af - rec->mUnk_1f4) <
            0x10) {
            if ((int) func_0201faf0(data_020c18bc.mUnk_2ae - data_020b6ee0.mUnk_a574.mUnk_24,
                                    data_020c18bc.mUnk_2af - data_020b6ee0.mUnk_a574.mUnk_28) < 6) {
                *out = stage;
                func_0202f110(0x167, 0x64, 0, 0x40, 0xffff, 0);
                return 1;
            }
        }
    }
    return 0;
}
#pragma thumb on

static inline s32 sinIdx(int a) {
    return FX_SinCosTable_[(a >> 4) * 2];
}

static inline s32 cosIdx(int a) {
    return FX_SinCosTable_[(a >> 4) * 2 + 1];
}

#pragma thumb off

void func_020175e8(int index, int *sel, int flags) {
    StageRec *rec = &data_020b1990[index];
    int hit = 0;
    int i;
    s32 x;
    s32 z;
    s32 ground;
    VecFx32 old;
    VecFx32 push;
    u32 base;
    int res;
    s32 turn;
    s32 ex;
    s32 ez;
    u16 angle;

    if (index < 0 || index >= STAGE_MAX) {
        OS_Terminate();
    }
    rec->mUnk_220 = (rec->mUnk_220 < 0x20000 ? rec->mUnk_220 : 0x20000) < -0x20000
                        ? -0x20000
                        : (rec->mUnk_220 < 0x20000 ? rec->mUnk_220 : 0x20000);
    if (data_020c18bc.mUnk_2ac_0) {
        data_020b6ee0.mUnk_a574.mUnk_24 = data_020c18bc.mUnk_2ae;
        data_020b6ee0.mUnk_a574.mUnk_28 = data_020c18bc.mUnk_2af;
    }
    if (rec->mUnk_142 != 1 || data_020b6ee0.mUnk_a146 == 3 || data_020b6ee0.mUnk_a146 == 4 ||
        data_020b6ee0.mUnk_a146 == 6 || data_020b6ee0.mUnk_a146 == 7 || data_020b6ee0.mUnk_a146 == 8) {
        if (index == *sel) {
            if (!(flags & 1) && (data_020b6ee0.mUnk_a146 == 3 || data_020b6ee0.mUnk_a146 == 4)) {
                if (data_020c18bc.mUnk_2ac_0) {
                    data_020b6ee0.mUnk_0c5c = data_020c18bc.mUnk_2ae;
                    data_020b6ee0.mUnk_0c5d = data_020c18bc.mUnk_2af;
                    data_020b6ee0.mUnk_a8e8 = 0;
                } else if (data_020c18bc.mUnk_2ac_1) {
                    data_020b6ee0.mUnk_a8e8 += (data_020c18bc.mUnk_2ae - data_020b6ee0.mUnk_0c5c) << 7;
                    data_020b6ee0.mUnk_0c5c = data_020c18bc.mUnk_2ae;
                }
            }
            rec->mUnk_1d8 -= data_020b6ee0.mUnk_a8e8;
            data_020b6ee0.mUnk_a8e8 = data_020b6ee0.mUnk_a8e8 * 31 / 32;
            return;
        }
        if (data_020b6ee0.mUnk_a146 > 2) {
            return;
        }
        if (func_0201750c(index, sel) != 1) {
            return;
        }
        rec->mUnk_134 |= 2;
        return;
    }
    if (!(flags & 1)) {
        if (index == *sel) {
            if (data_020c18bc.mUnk_2ac_2) {
                int n = data_020c2660.mUnk_10 - 1;
                if (n < 30 && n > 1) {
                    s32 dx = (data_020c1d60.mUnk_800[n].mUnk_0 - data_020c1d60.mUnk_800[0].mUnk_0) / 4;
                    s32 dy = (data_020c1d60.mUnk_800[n].mUnk_1 - data_020c1d60.mUnk_800[0].mUnk_1) / 4;
                    int len = func_0201faf0(dx, dy);
                    if (len > 8) {
                        dx = (dx << 3) / len;
                        dy = (dy << 3) / len;
                        func_0201faf0(dx, dy);
                    }
                    dx = (dx << 12) + rec->mUnk_220 * sinIdx(rec->mUnk_1d8) / 4096;
                    dy = (dy << 12) + rec->mUnk_220 * -cosIdx(rec->mUnk_1d8) / 4096;
                    rec->mUnk_1d8 = func_0201feb0(dx, dy);
                    len = func_0201faf0(dx, dy);
                    rec->mUnk_220 = len < 0x14000 ? len : 0x14000;
                }
            } else if (data_020c18bc.mUnk_2ac_1) {
                int n = data_020c2660.mUnk_10 - 1;
                if (n > 4) {
                    if ((int) func_0201faf0(data_020c1d60.mUnk_800[n].mUnk_0 - (data_020c1d60.mUnk_800 + n - 4)->mUnk_0,
                                            data_020c1d60.mUnk_800[n].mUnk_1 -
                                                (data_020c1d60.mUnk_800 + n - 4)->mUnk_1) < 4) {
                        rec->mUnk_220 -= 3276.8;
                    }
                }
            }
        } else {
            func_0201750c(index, sel);
        }
    }
    rec->mUnk_220 -= 204.8;
    if (rec->mUnk_220 <= 0) {
        rec->mUnk_220 = 0;
        return;
    }
    old = rec->mUnk_1a0;
    rec->mUnk_1dc += rec->mUnk_220 * 0xc0 >> 12;
    x = rec->mUnk_220 * sinIdx(rec->mUnk_1d8) >> 12;
    z = rec->mUnk_220 * -cosIdx(rec->mUnk_1d8) >> 12;
    rec->mUnk_1a0.x += x;
    rec->mUnk_1a0.z += z;
    ground = func_02030830(rec->mUnk_1a0.x, rec->mUnk_1a0.z, rec->mUnk_1a0.y + 0xc800, 0, 0, NULL, &push) + 0x32000;
    if (rec->mUnk_220 != 0) {
        func_0202f110(0x1c4, func_0201fb74(abs(rec->mUnk_220 >> 12), 0x20, 0, 0x4b), 1, 0x40, 0xffff, 0);
    }
    if (push.y != 0x1000) {
        rec->mUnk_1a0.x += push.x;
        rec->mUnk_1a0.z += push.z;
        rec->mUnk_1dc -= 0x50;
    }
    if (ground >= rec->mUnk_1a0.y) {
        rec->mUnk_1a0.y = ground;
        rec->mUnk_21c = 0.0f;
    } else {
        rec->mUnk_1a0.y = rec->mUnk_1a0.y - rec->mUnk_21c;
        if (rec->mUnk_21c < 51200.0f) {
            rec->mUnk_21c += 5120.0f;
        }
    }
    if (!(flags & 2) && rec->mUnk_220 != 0) {
        for (i = 0; i < data_020b6ee0.mUnk_a8ee; i++) {
            StageRec *other;
            u32 dir;
            int diff;
            s32 along;
            s32 across;

            if (i == index) {
                continue;
            }
            other = &data_020b1990[i];
            ex = (other->mUnk_1a0.x - rec->mUnk_1a0.x) / 512;
            ez = (other->mUnk_1a0.z - rec->mUnk_1a0.z) / 512;
            if (func_0201fafc(ex, ez) >= 0x60e64) {
                continue;
            }
            dir = func_0201feb0(ex, ez);
            diff = func_0201fe34(rec->mUnk_1d8, dir);
            if (abs(diff) >= 0x3000) {
                continue;
            }
            along = rec->mUnk_220 * cosIdx(abs(diff)) / 4096;
            x = along * sinIdx((u16) (dir + 0x8000)) / 4096;
            z = along * -cosIdx((u16) (dir + 0x8000)) / 4096;
            across = rec->mUnk_220 * sinIdx(abs(diff)) / 4096;
            x += across * sinIdx((u16) (rec->mUnk_1d8 - diff)) / 4096;
            z += across * -cosIdx((u16) (rec->mUnk_1d8 - diff)) / 4096;
            if (data_020c0cbc.mUnk_08 < 3 && data_020b6ee0.mUnk_a146 != 2) {
                other->mUnk_1d8 = func_0201feb0(-x, -z);
                other->mUnk_220 = func_0201faf0(x, z) >> 1;
                if (other->mUnk_142 != 1) {
                    other->mUnk_220 = 0;
                }
            }
            func_0202f110(0x30, 100, 0, 0x40, 0xffff, 0);
            if ((u32) data_020b6ee0.mUnk_a564 >= 99) {
                func_02014ffc(rec, -4, 0, 0);
                func_02014ffc(other, -4, 0, 0);
            }
            hit = 1;
        }
    }
    if (hit == 1) {
        rec->mUnk_1d8 = func_0201feb0(x, z);
        rec->mUnk_220 = func_0201faf0(x, z);
    }
    {
        int mask = 1;
        if (func_02031824(&rec->mUnk_1a0, 400, 200, 0, 0x400, 0)) {
            s32 oz = old.z / 512;
            s32 ox = old.x / 512;
            if (func_0201febc(data_020c5e44.mUnk_38, data_020c5e44.mUnk_3c, data_020c5e44.mUnk_44,
                              data_020c5e44.mUnk_48, ox, oz) > 0) {
                if (func_0201fe14(func_0201feb0(data_020c5e44.mUnk_50 - ox, data_020c5e44.mUnk_54 - oz),
                                  rec->mUnk_1d8) < 0x4000) {
                    mask |= 0x400;
                }
            }
        }
        res = func_02031824(&rec->mUnk_1a0, 400, 200, 0, mask, 0);
    }
    if (res == 0) {
        return;
    }
    base = func_0203129c();
    func_0202f110(0x30, 100, 0, 0x40, 0xffff, 0);
    turn = func_0201febc(data_020c5e44.mUnk_38, data_020c5e44.mUnk_3c, data_020c5e44.mUnk_44, data_020c5e44.mUnk_48,
                         old.x / 512, old.z / 512);
    turn = turn != 0 ? (turn >> 31) | 1 : 0;
    turn = -(turn * 0x4000);
    if (!(res & 0x10000000)) {
        angle = base - turn + func_0201fe34(rec->mUnk_1d8, (u16) (base + turn));
    } else {
        angle = base - turn;
    }
    if (data_020c5e44.mUnk_5e & 0x10) {
        s32 cx = data_020c5e44.mUnk_50;
        s32 cz = data_020c5e44.mUnk_54;
        int j;
        u32 best;
        int near;

        data_020b6ee0.mUnk_0c5a = 600;
        best = -1;
        for (j = 0; j < data_020b6ee0.mUnk_0c48; j++) {
            u32 dist = abs(data_020b6ee0.mUnk_0008[j].mUnk_178 - cx) + abs(data_020b6ee0.mUnk_0008[j].mUnk_17c - cz);
            if (dist < best) {
                best = dist;
                near = j;
            }
        }
        data_020b6ee0.mUnk_0c50 = data_020b6ee0.mUnk_0008[near].mUnk_178;
        data_020b6ee0.mUnk_0c54 = data_020b6ee0.mUnk_0008[near].mUnk_17c;
        data_020b6ee0.mUnk_0c58 = near;
        data_020b6ee0.mUnk_0c4c = data_020c5e44.mUnk_34;
    }
    if (data_020c5e44.mUnk_5e & 2) {
        void *key = func_02031c70(data_020c5e44.mUnk_34);
        int count = data_020b6ee0.mUnk_0c48;
        s32 dmg;

        for (i = 0; i < count; i++) {
            Unk_020b6ee8 *g = &data_020b6ee0.mUnk_0008[i];
            if (g->mUnk_184 == 0x2d && g->mUnk_180 == key) {
                break;
            }
        }
        if (i == count) {
            int j;
            u32 best = -1;
            int near;
            for (j = 0; j < data_020b6ee0.mUnk_0c48; j++) {
                Unk_020b6ee8 *g = &data_020b6ee0.mUnk_0008[j];
                s32 dx = g->mUnk_178 - rec->mUnk_1a0.x / 512;
                s32 dz = g->mUnk_17c - rec->mUnk_1a0.z / 512;
                u32 dist = abs(dx) + abs(dz);
                if (dist < best) {
                    best = dist;
                    near = j;
                }
            }
            data_020b6ee0.mUnk_0008[near].mUnk_180 = key;
            i = near;
        }
        dmg = abs(rec->mUnk_220 * cosIdx((u16) func_0201fe14(angle, rec->mUnk_1d8)) / 4096 >> 12);
        data_020b6ee0.mUnk_0008[i].mUnk_186 -= dmg;
        if (data_020b6ee0.mUnk_0008[i].mUnk_186 < 0) {
            func_02031c98(data_020c5e44.mUnk_34, 0, 0);
            data_020b6ee0.mUnk_0008[i].mModel.mUnk_134 |= 2;
            func_0202f110(0xda, 100, 0, 0x40, 0xffff, 0);
        } else {
            data_020b6ee0.mUnk_0008[i].mModel.mUnk_134 |= 1;
        }
    }
    rec->mUnk_220 -= rec->mUnk_220 / 2 < 1 ? 1 : rec->mUnk_220 / 2;
    rec->mUnk_1a0 = old;
    rec->mUnk_1d8 = angle;
    data_020b6ee0.mUnk_8f94 |= 1;
}
#pragma thumb on
