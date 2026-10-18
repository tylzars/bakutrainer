#include <nitro/os.h>
#include <stdio.h>
#include <Main/clib.h>
#include <Main/Unknown/functions.h>
#include <Main/Unknown/save_blocks.h>
#include <Main/Unknown/random_angle.h>
#include <Main/Unknown/screen_fade.h>
#include <Main/Unknown/game_math.h>
#include <Main/Unknown/bg_image.h>
#include <Main/Unknown/state_flow.h>
#include <Main/Unknown/stage_info_panel.h>
#include <Main/Unknown/mode_select.h>
#include <nitro/mi.h>
#include <Main/Unknown/prompt_screen.h>
#include <Main/Unknown/sound_control.h>
#include <Main/Unknown/globals.h>
#include <Main/Unknown/game_util.h>
#include <Main/Unknown/stage_data.h>
#include <Main/Unknown/stage_camera.h>
#include <Main/Unknown/move_object.h>
#include <Main/Unknown/three_in_row.h>
#include <Main/Unknown/stage_select.h>
#include <Main/Unknown/sprite_slots.h>
#include <Main/Unknown/stage_progress.h>
#include <Main/Unknown/text_draw.h>
#include <Main/Unknown/camera_state.h>
#include <Main/Unknown/vram_cache.h>
#include <Main/Unknown/model.h>
#include <Main/Unknown/texture_blocks.h>
#include <Main/Unknown/wave_player.h>
#include <Main/Unknown/file_loader.h>
#include <Main/Unknown/map_entries.h>

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
                int n = data_020c2560.mUnk_100.mUnk_10 - 1;
                if (n < 30 && n > 1) {
                    s32 dx = (data_020c2560.mUnk_000[n].mUnk_0 - data_020c2560.mUnk_000[0].mUnk_0) / 4;
                    s32 dy = (data_020c2560.mUnk_000[n].mUnk_1 - data_020c2560.mUnk_000[0].mUnk_1) / 4;
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
                int n = data_020c2560.mUnk_100.mUnk_10 - 1;
                if (n > 4) {
                    if ((int) func_0201faf0(data_020c2560.mUnk_000[n].mUnk_0 - (data_020c2560.mUnk_000 + n - 4)->mUnk_0,
                                            data_020c2560.mUnk_000[n].mUnk_1 -
                                                (data_020c2560.mUnk_000 + n - 4)->mUnk_1) < 4) {
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
            if (data_020b6ee0.mUnk_9ddc[2] < 3 && data_020b6ee0.mUnk_a146 != 2) {
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
    data_020b6ee0.mUnk_8e60.mModel.mUnk_134 |= 1;
}
#pragma thumb on

static const u16 data_02050164[4][6] = {
    {0, 1, 2, 3, 4, 5},
    {6, 7, 8, 9, 10, 11},
    {12, 13, 14, 15, 16, 17},
    {18, 19, 20, 21, 22, 23},
};

static const u16 data_02050194[24] = {
    2528, 2628, 2739, 2827, 2935, 3061, 608,  3216, 708,  792,  909,  1048,
    1188, 1288, 1397, 1481, 1596, 1732, 1866, 1966, 2075, 2161, 2269, 2395,
};

static const u16 data_020501c4[24] = {
    2527, 2627, 2738, 2826, 2934, 3060, 607,  3215, 707,  791,  908,  1047,
    1187, 1287, 1396, 1480, 1595, 1731, 1865, 1965, 2074, 2160, 2268, 2394,
};

int func_020182b8(int row, int column) {
    return func_02021e44(0xd61, column + row * 2);
}

void func_020182cc(void) {
    int stage;
    Unk_020bfd40 *obj;
    u8 resumed;
    StageRec *rec;
    int row;

    stage = data_020b6ee0.mUnk_a8e4;
    obj = &data_020b6ee0.mUnk_8e60;
    resumed = stage != -1;
    if (stage == -1) {
        stage = 0;
    }
    data_020b6ee0.mUnk_a8e4 = stage;
    {
        BOOL ok = 0;
        if (data_020b6ee0.mUnk_a8e4 >= 0 && data_020b6ee0.mUnk_a8e4 < STAGE_MAX) {
            ok = 1;
        }
        if (ok == 0) {
            OS_Terminate();
        }
    }
    rec = &data_020b1990[data_020b6ee0.mUnk_a8e4];
    if (resumed == 1) {
        if (rec->mInfo.mUnk_11 > 12) {
            OS_Terminate();
        }
        if (rec->mInfo.mUnk_10 >= 6) {
            OS_Terminate();
        }
        row = (rec->mInfo.mUnk_11 - 1) / 3;
        if (row > 4) {
            OS_Terminate();
        }
        data_020b6ee0.mUnk_a8f2 = data_02050164[row][rec->mInfo.mUnk_10];
        if (data_020b6ee0.mUnk_a8f2 >= 24) {
            OS_Terminate();
        }
    }
    func_0201eb20();
    func_0202c1c4();
    func_0201dde0(rec->mInfo.mUnk_13 + 0xdb3, 2);
    func_0202bdd8(0x10cb, 0x10ca, 0x10cc, 0, 1);
    func_0202bdd8(0x10cb, 0x10ca, 0x10cc, 0, 2);
    func_02020854(0);
    func_02021fb0(0x1c7, 0x1c6, 0x180, 0x10);
    func_020325b0(data_020501c4[data_020b6ee0.mUnk_a8f2], 0, data_02050194[data_020b6ee0.mUnk_a8f2], 0, 0, 0);
    data_020c5e44.mUnk_28 = func_02015d48;
    data_020c6144.mUnk_54 = 0x640;
    data_020c6144.mUnk_44 = 0x186a000;
    func_020104c0(3, 0x4c);
    MI_CpuFill8(&rec->mDraw, 0, 0x44);
    func_02031fc4(0, &rec->mUnk_1a0);
    rec->mDraw.mUnk_43 = 2;
    rec->mDraw.mUnk_28 = 2 << 19;
    rec->mUnk_220 = 0;
    rec->mUnk_224 = 0;
    func_020271ec(rec, 0, 0, 0, data_0201ab04[rec->mInfo.mUnk_13]);
    rec->mUnk_158 = rec->mUnk_1a0;
    func_020271ec(&data_020b6ee0.mUnk_8e60, 0, 0, 0, data_0201aba0[5]);
    data_020b6ee0.mUnk_8e60.mDraw = rec->mDraw;
    data_020b6ee0.mUnk_8e60.mDraw.mUnk_2c = -1;
    data_020b6ee0.mUnk_8e60.mDraw.mUnk_30 = -1;
    data_020b6ee0.mUnk_8e60.mDraw.mUnk_34 = -1;
    data_020b6ee0.mUnk_8e60.mDraw.mUnk_3c = 0;
    data_020b6ee0.mUnk_8e60.mDraw.mUnk_3a = data_020b6ee0.mUnk_8e60.mDraw.mUnk_3c;
    data_020c5e44.mUnk_bc = 4;
    func_02031fc4(0, &data_020c5e44.mUnk_98);
    data_020c5e44.mUnk_80 = data_020c5e44.mUnk_98;
    data_020c5e44.mUnk_80.y += 0x6d6000;
    func_02015d4c();
    func_02016624();
    obj->mUnk_1bc = func_020182b8(rec->mInfo.mUnk_11 - 1, 2) * 60000 / 1000;
    obj->mUnk_1c0 = 0;
    func_02005a8c(1, 0);
}

void func_02018558(void) {
    int stage = data_020b6ee0.mUnk_a8e4;
    Unk_020bfd40 *obj = &data_020b6ee0.mUnk_8e60;
    StageRec *rec = &data_020b1990[stage];
    VecFx32 pos;
    u8 mode;
    int busy;

    if (stage >= STAGE_MAX) {
        OS_Terminate();
    }
    func_0201168c(data_020b6ee0.mUnk_a8e4, obj->mUnk_1c0, data_0206d448, obj->mUnk_1bc, 2);
    func_02015d58(stage);
    rec->mUnk_158 = rec->mUnk_1a0;
    func_0200ba94(&rec->mUnk_1a0, 0x70800, 0x800);
    func_0202733c(rec, &rec->mUnk_1a0, 0);
    func_020105f4(rec);
    func_02025db4(&rec->mUnk_1a0, &rec->mUnk_1f0);
    func_020262a0(&data_020b6ee0.mUnk_8e60.mModel);
    if (data_020b6ee0.mUnk_8e60.mModel.mUnk_142 != 1) {
        data_020b6ee0.mUnk_8e60.mDraw.mPos = rec->mUnk_1a0;
        func_02024b5c(data_020b6ee0.mUnk_8e60.mModel.mUnk_170, &data_020b6ee0.mUnk_8e60.mDraw, &data_020b6ee0.mUnk_8e60,
                      0);
    } else {
        func_020271c8();
    }
    pos.x = data_020c5e44.mUnk_98.x;
    pos.z = data_020c5e44.mUnk_98.z;
    pos.y = -0x19000;
    func_0200ba08(data_020b6ee0.mUnk_a8f2 + 0xfbe, &pos, (data_020c18bc.mUnk_45c >> 1) % 100, 0, 0x3000, 0);
    func_02033614(data_020c5e44.mUnk_98.x, data_020c5e44.mUnk_98.z);
    func_02016ae8(stage);
    func_02015f34();
    func_02016008(stage);
    func_020164e0(stage);
    mode = func_02005bf8();
    if (mode != 0) {
        busy = func_02005b1c(1);
        if (mode == 2 && busy == 0) {
            func_020059b4(func_02005c04(), obj->mUnk_1c0);
        }
        return;
    }
    func_020175e8(stage, &data_020b6ee0.mUnk_a8e4, 2);
    func_020186fc(stage);
}

void func_020186cc(void) {
    func_020102f8();
    func_0202dec4(data_020501c4[data_020b6ee0.mUnk_a8f2]);
    func_0201eb20();
}

int func_020186f0(u8 stage) {
    return func_020182b8(stage, 3);
}

void func_020186fc(int stage) {
    Unk_020bfd40 *obj = &data_020b6ee0.mUnk_8e60;
    StageRec *rec = &data_020b1990[stage];

    if (stage < 0 || stage >= STAGE_MAX) {
        OS_Terminate();
    }
    obj->mUnk_1bc--;
    func_020059cc(obj->mUnk_1c0);
    if (obj->mUnk_1bc <= 0 || data_020b6ee0.mUnk_8ad9 == 0) {
        func_02005a8c(0, obj->mUnk_1c0 >= func_020182b8(rec->mInfo.mUnk_11 - 1, 3));
    }
}

void func_02018770(StageRec *rec, int kind, int amount) {
    char buf[32];
    ObjSlot *slot;

    if (kind == 0xf49) {
        func_02011624(amount, buf);
    } else if (amount > 0) {
        sprintf(buf, "+%d", amount < 0 ? -amount : amount);
    } else {
        sprintf(buf, "-%d", amount < 0 ? -amount : amount);
    }
    slot = func_0200d88c(func_0200d7cc(kind, 0, 30, 0, -15, 0, -256, 110, 1, 0));
    slot->mUnk_134 = rec->mUnk_1a0;
    slot->mUnk_140 = 1;
    slot = func_0200d88c(func_0200d838(buf, 2, data_0201a61c, 30, 0, 0, 0, -256, 100, 1, 0));
    slot->mUnk_134 = rec->mUnk_1a0;
    slot->mUnk_140 = 1;
}

void func_02018840(StageRec *rec, int kind, int index) {
    Unk_020bfd40 *obj = &data_020b6ee0.mUnk_8e60;
    int time, score;
    time = 0;
    score = 0;

    switch ((u32) kind) {
        case 0xf:
            if (index < 0 || index >= data_020b6ee0.mUnk_5a54) {
                OS_Terminate();
            }
            time = -data_020b6ee0.mUnk_1014[index].mUnk_183 * 100;
            data_020b6ee0.mUnk_8e60.mModel.mUnk_134 |= 1;
            break;
        case 0x14:
            time = -100;
            data_020b6ee0.mUnk_8e60.mModel.mUnk_134 |= 4;
            break;
        case 5:
            time = -100;
            data_020b6ee0.mUnk_8e60.mModel.mUnk_134 |= 1;
            break;
        case 0x1e:
            if (index < 0 || index >= data_020b6ee0.mUnk_8ad8) {
                OS_Terminate();
            }
            if (data_020b6ee0.mUnk_5a58[index].mUnk_181[1] == 0) {
                data_020b6ee0.mUnk_8ad9--;
                score = data_020b6ee0.mUnk_5a58[index].mUnk_181[0] * 100;
            } else {
                time = data_020b6ee0.mUnk_5a58[index].mUnk_181[0] * 300;
            }
            break;
    }
    if (time != 0 || score != 0) {
        func_02025db4(&rec->mUnk_1a0, &rec->mUnk_1f0);
    }
    if (time != 0) {
        obj->mUnk_1bc += time;
        func_02018770(rec, 0xf49, time);
    } else if (score != 0) {
        if (score <= 0) {
            OS_Terminate();
        }
        func_020059d0(&obj->mUnk_1c0, score, 0);
        func_02018770(rec, 0xf48, score);
    }
}

void func_0201896c(void) {
    Unk_020c1454 *panel = &data_020b6ee0.mUnk_a574;
    DrawParams params;
    VecFx32 offset;
    int i;
    StageRec *rec;
    int dx;
    int dz;

    func_0201eb20();
    MI_CpuFill8(&params, 0, 0x44);
    func_0202c1c4();
    func_0202bdd8(0x10cb, 0x10ca, 0x10cc, 0, 1);
    func_0202bdd8(0x10cb, 0x10ca, 0x10cc, 2, 2);
    func_02020854(0);
    func_02021fb0(0x1c7, 0x1c6, 0x180, 0x10);
    func_020325b0(0x1db, 0, 0x1dc, 0, 0, 0);
    data_020c5e44.mUnk_28 = func_02015d48;
    data_020c6144.mUnk_54 = 0x640;
    func_020104e0(6, 0x4c, 3);
    data_020c6144.mUnk_44 = 0x186a000;
    func_02031fc4(0, &offset);
    func_02033df4(0x18);
    data_020c5e44.mUnk_bc = 0;
    if (data_020b6ee0.mUnk_a147 == 0) {
        func_02031fc4(0, &data_020c5e44.mUnk_b0);
        func_02031fc4(0, &data_020b6ee0.mUnk_a574.mUnk_0c);
        data_020c5e44.mUnk_a4 = data_020c5e44.mUnk_b0;
        data_020c5e44.mUnk_a4.y += 0xcb2000;
        data_020c5e44.mUnk_a4.z += 0x32000;
        data_020b6ee0.mUnk_a574.mUnk_34 = 0x80;
    } else {
        data_020b6ee0.mUnk_a147 = 0;
    }
    params.mUnk_43 = 2;
    params.mUnk_28 = 2 << 19;
    rec = data_020b1990;
    for (i = 0; i < data_020b6ee0.mUnk_a8ee; i++, rec++) {
        func_020271ec(rec, 0, 0, 0, data_0201ab04[rec->mInfo.mUnk_13]);
        rec->mDraw = params;
        rec->mUnk_1d8 = func_0201fd68();
        rec->mUnk_220 = 0;
        rec->mUnk_224 = 0;
        if (rec->mUnk_1e8 == -1) {
            func_02031fc4(0, &rec->mUnk_1a0);
            rec->mUnk_220 = 0x6000;
        } else {
            rec->mUnk_1a0.x = offset.x + rec->mUnk_1e4;
            rec->mUnk_1a0.z = offset.z + rec->mUnk_1ec;
        }
        rec->mUnk_1a0.y = offset.y + 0x32000;
        rec->mUnk_1a0.y =
            func_02030830(rec->mUnk_1a0.x, rec->mUnk_1a0.z, rec->mUnk_1a0.y + 0xc800, 0, 0, 0, 0) + 0x32000;
        rec->mUnk_158 = rec->mUnk_1a0;
    }
    if (data_020b6ee0.mUnk_a8e4 == -1) {
        data_020b6ee0.mUnk_a146 = 0;
    } else {
        data_020b6ee0.mUnk_a146 = 1;
        {
            BOOL ok = 0;
            if (data_020b6ee0.mUnk_a8e4 >= 0 && data_020b6ee0.mUnk_a8e4 < STAGE_MAX) {
                ok = 1;
            }
            if (ok == 0) {
                OS_Terminate();
            }
        }
        rec = &data_020b1990[data_020b6ee0.mUnk_a8e4];
        dx = rec->mUnk_1a0.x - data_020c5e44.mUnk_b0.x;
        dz = rec->mUnk_1a0.z - data_020c5e44.mUnk_b0.z;
        data_020c5e44.mUnk_b0.x += dx;
        data_020c5e44.mUnk_b0.z += dz;
        data_020c5e44.mUnk_a4.x += dx;
        data_020c5e44.mUnk_a4.z += dz;
    }
    func_020151f8();
    func_020196b4();
    func_020198f0();
    panel->mUnk_3c = 0x28;
    data_020b6ee0.mUnk_a8e0 = -1;
    data_020b6ee0.mUnk_a5d0 = 1;
    func_020271ec(&data_020b6ee0.mUnk_9c64, 0, 0, 2, data_0201aba0[3]);
    data_020b6ee0.mUnk_a10c = -1;
    func_02011518(data_020b6ee0.mUnk_a8e4, 2);
    data_020b6ee0.mUnk_a14b = 0;
    func_020271ec(&data_020b6ee0.mUnk_8e60, 0, 0, 0, data_0201aba0[5]);
    func_02023f58(&data_020b6ee0.mUnk_8e60.mDraw, 0, 0, 0, 6 << 18, 0, 2, 0, 0x100);
    if (data_020b6ee0.mUnk_a14a == 1) {
        data_020b6ee0.mUnk_a14a = 0;
        data_020b6ee0.mUnk_9c64.mUnk_134 |= 1;
        if (data_020b6ee0.mUnk_a8e4 != -1) {
            data_020b6ee0.mUnk_8e60.mModel.mUnk_134 |= 4;
        }
    }
    if (data_020b6ee0.mUnk_a14c == 1) {
        func_02014430(0);
        data_020b6ee0.mUnk_a14c = 0;
    }
    func_0200c164(&data_020b6ee0.mUnk_9de8);
}

int func_02018ccc(const u16 *a, const u16 *b) {
    return *b - *a;
}

typedef struct SortEntry {
    u16 mKey;
    u16 mStage;
} SortEntry;

void func_02018cd4(void) {
    s32 *c = data_020b6ee0.mUnk_9ddc;
    DrawParams view;
    SortEntry order[64];
    DrawParams params;
    VecFx32 camPos;
    VecFx32 marker;
    s32 dist;
    Unk_020c1454 *fade = &data_020b6ee0.mUnk_a574;
    int flags;
    int i;
    int j;
    StageRec *rec;

    func_020192d4();
    memset(order, 0, data_020b6ee0.mUnk_a8ee * 2);
    memset(c, 0, 0xc);
    for (i = 0; i < data_020b6ee0.mUnk_a8ee; i++) {
        StageRec *rec = &data_020b1990[i];
        if (i == data_020b6ee0.mUnk_a8e4) {
            order[i].mKey = 0xffff;
        } else if (rec->mUnk_142 != 1) {
            order[i].mKey = 0xfffe;
        } else if (rec->mUnk_220 > 0x1000) {
            order[i].mKey = (rec->mUnk_220 >> 12) + 1;
        }
        order[i].mStage = i;
        if (rec->mUnk_22e == 1) {
            c[0]++;
        }
        if (rec->mUnk_220 != 0) {
            c[2]++;
        }
        if (rec->mUnk_220 != 0 || rec->mUnk_142 != 1) {
            c[1]++;
        }
    }
    qsort(order, data_020b6ee0.mUnk_a8ee, 4, (int (*)(const void *, const void *)) func_02018ccc);
    func_02031fc4(0, &camPos);
    func_02015e94(data_020b6ee0.mUnk_a8e4);
    func_02019504();
    for (j = 0; j < data_020b6ee0.mUnk_a8ee; j++) {
        int idx = order[j].mStage;
        int lim;
        rec = &data_020b1990[idx];
        params = rec->mDraw;
        rec->mUnk_158 = params.mPos;
        rec->mUnk_22f = func_02031d0c(&rec->mUnk_1a0, 0x64000, 0x64000, 0x64000);
        if (rec->mUnk_22f == 1 || idx == data_020b6ee0.mUnk_a8e4) {
            func_02025db4(&rec->mUnk_1a0, &rec->mUnk_1f0);
            lim = (data_020b6ee0.mUnk_a146 == 0 || data_020b6ee0.mUnk_a146 == 1) ? 2 : 1;
            if (order[j].mKey != 0 && j < lim) {
                rec->mUnk_170 = rec->mInfo.mUnk_13 * 6 + 0xfd8;
            } else {
                rec->mUnk_170 = rec->mInfo.mUnk_13 * 6 + 0xfdb;
            }
            if (rec->mUnk_142 != 1 || rec->mUnk_134 != 0 || idx == data_020b6ee0.mUnk_a8e4) {
                if (rec->mUnk_142 != 1 && data_020b6ee0.mUnk_ca8 != 0 && idx == data_020b6ee0.mUnk_a8e4) {
                    func_0202601c(data_020b6ee0.mUnk_ca8, &data_020b6ee0.mUnk_ca6, 2, 1, &data_020b6ee0.mUnk_c94,
                                  &data_020b6ee0.mUnk_c98, &data_020b6ee0.mUnk_c9c, 1);
                    func_02023478(data_020b6ee0.mUnk_ca8, data_020b6ee0.mUnk_ca0, &data_020b6ee0.mUnk_c88,
                                  &data_020b6ee0.mUnk_c7c, data_020b6ee0.mUnk_c94, data_020b6ee0.mUnk_c94,
                                  data_020b6ee0.mUnk_c9c, 0);
                    params.mUnk_3a += data_020b6ee0.mUnk_ca0[0];
                    params.mUnk_38 += data_020b6ee0.mUnk_ca0[1];
                    params.mUnk_3c += data_020b6ee0.mUnk_ca0[2];
                    params.mPos.x += data_020b6ee0.mUnk_c88.x * data_020b6ee0.mUnk_c7c.x;
                    params.mPos.y += data_020b6ee0.mUnk_c88.y * data_020b6ee0.mUnk_c7c.y;
                    params.mPos.z += data_020b6ee0.mUnk_c88.z * data_020b6ee0.mUnk_c7c.z;
                    rec->mUnk_158 = params.mPos;
                }
                if (data_020b6ee0.mUnk_a10c != -1 && idx == data_020b6ee0.mUnk_a8e4) {
                    func_02025ff4((Model *) rec, data_020b6ee0.mUnk_a10c);
                }
                func_0202733c(rec, &params, 0x10);
                func_020105f4(rec);
            } else {
                func_02024b5c(rec->mUnk_170, &params, rec, 0x10);
            }
            if (idx == data_020b6ee0.mUnk_a8e4) {
                func_020262a0(&data_020b6ee0.mUnk_8e60.mModel);
                if (data_020b6ee0.mUnk_8e60.mModel.mUnk_142 != 1) {
                    data_020b6ee0.mUnk_8e60.mDraw.mPos = rec->mUnk_1a0;
                    func_02024b5c(data_020b6ee0.mUnk_8e60.mModel.mUnk_170, &data_020b6ee0.mUnk_8e60.mDraw.mPos,
                                  &data_020b6ee0.mUnk_8e60.mModel, 0);
                } else {
                    func_020271c8();
                }
                marker = params.mPos;
                marker.y -= 0x19000;
                func_0200ba94(&params.mPos, 0x70800, 0x800);
            }
            if (data_020b6ee0.mUnk_a146 <= 1) {
                if (idx == data_020b6ee0.mUnk_a8e4) {
                    func_0200ba08(0x10c8, &marker, 0, data_020c18bc.mUnk_45c << 10, 0x100000, 0);
                    func_02019800(rec);
                }
                if (rec->mUnk_22d == 1 && (u32) data_020b6ee0.mUnk_a564 >= 0x11) {
                    func_02019974(rec, 0, 0x76, 0, 0x15, 0xc8);
                }
                func_02019974(rec, 1, 0x77, 0xc, 0x11, 0xc8);
            } else if (data_020b6ee0.mUnk_a146 == 2 || data_020b6ee0.mUnk_a146 == 5) {
                if (idx == data_020b6ee0.mUnk_a8e4 && data_020b6ee0.mUnk_a148 == 0) {
                    func_02019800(rec);
                    if (rec->mUnk_22d == 1 && (u32) data_020b6ee0.mUnk_a564 >= 0x11) {
                        func_02019974(rec, 0, 0x76, 0, 0x15, 0xc8);
                    }
                    func_02019974(rec, 1, rec->mInfo.mUnk_1a + 0x42, 0xc, 0x11, 0xc8);
                }
            }
        }
        flags = 4;
        if (data_020b6ee0.mUnk_a5d0 == 1 || (u8) (data_020b6ee0.mUnk_a146 + 253) <= 2) {
            flags |= 1;
        }
        func_020175e8(idx, &data_020b6ee0.mUnk_a8e4, flags);
    }
    func_02019aec();
    func_02031fc4(0, &view.mPos);
    func_02023f58(&view, view.mPos.x, view.mPos.y, view.mPos.z, 0x1000, 0, 0, 0, 0x100);
    func_0202733c(&data_020b6ee0.mUnk_9c64, &view, 0);
    func_02019788();
    func_02033614(data_020c5e44.mUnk_98.x, data_020c5e44.mUnk_98.z);
    if (data_020b6ee0.mUnk_a8e0 != data_020b6ee0.mUnk_a8e4) {
        if (data_020b6ee0.mUnk_a8e4 == -1) {
            data_020b6ee0.mUnk_a146 = 0;
        } else if (data_020b6ee0.mUnk_a8e0 == -1) {
            data_020b6ee0.mUnk_a146 = 1;
        }
        data_020b6ee0.mUnk_a14b = 1;
        data_020b6ee0.mUnk_a8ec = 0;
        data_020b6ee0.mUnk_a8e0 = data_020b6ee0.mUnk_a8e4;
        fade->mUnk_3c = 0x28;
        func_02019ad8();
    }
    func_02011d1c();
    if (data_020b6ee0.mUnk_a14b == 1) {
        func_02011518(data_020b6ee0.mUnk_a8e4, 2);
        data_020b6ee0.mUnk_a14b = 0;
    }
    if (data_020b6ee0.mUnk_a146 == 0 && data_020b6ee0.mUnk_a5d0 == 0) {
        if (data_020c18bc.mUnk_2ac_0) {
            data_020b6ee0.mUnk_a574.mUnk_34 = 4;
            data_020b6ee0.mUnk_a574.mUnk_24 = data_020c18bc.mUnk_2ae;
            data_020b6ee0.mUnk_a574.mUnk_28 = data_020c18bc.mUnk_2af;
            data_020b6ee0.mUnk_a574.mUnk_18 = data_020b6ee0.mUnk_a574.mUnk_0c;
        }
        if (data_020c18bc.mUnk_2ac_1) {
            data_020b6ee0.mUnk_a574.mUnk_0c.x =
                data_020b6ee0.mUnk_a574.mUnk_18.x +
                (data_020b6ee0.mUnk_a574.mUnk_24 - data_020c18bc.mUnk_2ae) * (0xe << 10);
            data_020b6ee0.mUnk_a574.mUnk_0c.z =
                data_020b6ee0.mUnk_a574.mUnk_18.z +
                (data_020b6ee0.mUnk_a574.mUnk_28 - data_020c18bc.mUnk_2af) * (0xe << 10);
            dist = func_0201faf0((data_020b6ee0.mUnk_a574.mUnk_0c.x - camPos.x) / 512,
                                 (data_020b6ee0.mUnk_a574.mUnk_0c.z - camPos.z) / 512);
            if (dist > 0x3200) {
                s32 scale = (0x3200 << 9) / dist;
                s32 dx = (data_020b6ee0.mUnk_a574.mUnk_0c.x - camPos.x) / 512;
                s32 dz = (data_020b6ee0.mUnk_a574.mUnk_0c.z - camPos.z) / 512;
                data_020b6ee0.mUnk_a574.mUnk_0c.x = camPos.x + dx * scale;
                data_020b6ee0.mUnk_a574.mUnk_0c.z = camPos.z + dz * scale;
            }
        }
    }
    func_020126a0(data_020b6ee0.mUnk_a8e4);
    data_020c5e44.mUnk_bc = 4;
}

void func_020192d4(void) {
    u16 keys = data_020c18bc.mUnk_12;

    if (keys & 0x10) {
        if (data_020b6ee0.mUnk_a146 <= 1) {
            int next;
            if (data_020b6ee0.mUnk_a8e4 < data_020b6ee0.mUnk_a8ee - 1) {
                next = data_020b6ee0.mUnk_a8e4 + 1;
            } else {
                next = 0;
            }
            data_020b6ee0.mUnk_a8e4 = next;
        }
    }
    if (keys & 0x20) {
        if (data_020b6ee0.mUnk_a146 <= 1) {
            int cur = data_020b6ee0.mUnk_a8e4;
            int n;
            if (cur > 0) {
                n = cur;
            } else {
                n = data_020b6ee0.mUnk_a8ee;
            }
            data_020b6ee0.mUnk_a8e4 = n - 1;
        }
    }
    if (data_020b6ee0.mUnk_a146 == 0 || data_020b6ee0.mUnk_a146 == 1 || data_020b6ee0.mUnk_a146 == 2 ||
        (u8) (data_020b6ee0.mUnk_a146 + 250) <= 2) {
        if (data_020c18bc.mUnk_2ac_0 == 1) {
            if (!func_0200bd80(0x80000000, 0x5a) && !func_0200bd80(0x80000000, 0xa) &&
                !func_0200bd80(0x80000000, 0xb) && !func_0200bd80(0x80000000, 0xc) && !func_0200bd80(0x80000000, 0xd) &&
                !func_0200bd80(0x80000000, 0xe) && !func_0200bd80(0x80000000, 0xf) &&
                !func_0200bd80(0x80000000, 0x10) && !func_0200bd80(0x80000000, 0x11) &&
                !func_0200bd80(0x80000000, 0x21) && !func_0200bd80(0x80000000, 0x111) &&
                !func_0200bd80(0x80000000, 0x112)) {
                data_020b6ee0.mUnk_a5d0 = 0;
            }
        }
        if (!data_020c18bc.mUnk_2ac_1 && !data_020c18bc.mUnk_2ac_2) {
            data_020b6ee0.mUnk_a5d0 = 1;
        }
    } else {
        data_020b6ee0.mUnk_a5d0 = 1;
    }
}

void func_02019438(void) {
    func_0200fb0c();
    func_0201944c();
    func_020102f8();
    func_0201eb20();
}

void func_0201944c(void) {
    if (func_0200fb00() == 10) {
        VecFx32 pos;
        StageRec *rec;
        int i;

        func_02031fc4(0, &pos);
        for (i = 0; i < data_020b6ee0.mUnk_a8ee; i++) {
            rec = &data_020b1990[i];
            rec->mUnk_1e4 = rec->mUnk_1a0.x - pos.x;
            rec->mUnk_1e8 = 0;
            rec->mUnk_1ec = rec->mUnk_1a0.z - pos.z;
        }
    }
}

void func_020194ac(int stage) {
    StageRec *rec = &data_020b1990[stage];
    int i;

    if (stage < 0 || stage >= STAGE_MAX) {
        OS_Terminate();
    }
    for (i = 10; i < 16; i++) {
        if (func_020320c0(i, 0, &rec->mUnk_1a0) == 1) {
            if (rec->mUnk_226 != i - 10) {
                rec->mUnk_226 = i - 10;
            }
            break;
        }
    }
    if (i == 16) {
        rec->mUnk_226 = 99;
    }
}

void func_02019504(void) {
    s32 *c = data_020b6ee0.mUnk_9ddc;
    int result = 0;
    int idx;
    StageRec *rec;
    int interval;

    if (data_020b6ee0.mUnk_a146 > 2) {
        return;
    }
    idx = func_0201fd84(0, data_020b6ee0.mUnk_a8ee - 1);
    if (!((idx >= 0) & (idx < data_020b6ee0.mUnk_a8ee))) {
        OS_Terminate();
    }
    rec = &data_020b1990[idx];
    if (data_020b6ee0.mUnk_a149 < 4 && !(func_0201fd68() & 0x1ff)) {
        func_020198f0();
    }
    if (idx != data_020b6ee0.mUnk_a8e4 && rec->mUnk_220 == 0 && rec->mUnk_22f != 0 && data_020b6ee0.mUnk_a146 != 2) {
        switch ((u8) func_0201fd68()) {
            case 0:
                if (c[1] == 0) {
                    rec->mUnk_1d8 = func_0201fd68();
                    rec->mUnk_134 |= 4;
                }
                break;
            case 1:
                if (c[1] == 0) {
                    rec->mUnk_1d8 = func_0201fd68();
                    rec->mUnk_220 = 0x4000;
                }
                break;
            case 2:
                if (rec->mInfo.mUnk_1a >= 4) {
                    OS_Terminate();
                }
                if (rec->mInfo.mUnk_12 < 0x64) {
                    rec->mUnk_22e = c[0] < 3 ? 1 : rec->mUnk_22e;
                } else {
                    rec->mUnk_22e = 0;
                }
                break;
        }
    }
    interval = 0x40 / data_020b6ee0.mUnk_a8ee + 8;
    if (func_0201fd68() % interval != 0) {
        return;
    }
    func_020194ac(idx);
    if (rec->mUnk_226 == rec->mInfo.mUnk_10) {
        result = 2;
    } else if (rec->mUnk_226 != 99 && (u32) data_020b6ee0.mUnk_a564 >= 0x63) {
        result = -2;
    }
    if (idx == data_020b6ee0.mUnk_a8e4) {
        if (result == 2) {
            data_020b6ee0.mUnk_8e60.mModel.mUnk_134 |= 1;
        } else if (result == -2) {
            data_020b6ee0.mUnk_8e60.mModel.mUnk_134 |= 4;
        }
    }
    func_02014ffc(rec, result, 0, idx == data_020b6ee0.mUnk_a8e4 ? 1 : 0);
}

void func_020196b4(void) {
    int i;

    data_020b6ee0.mUnk_a149 = 0;
    for (i = 0; i < 8; i++) {
        data_020b6ee0.mUnk_9024[i].mUnk_184 = 0;
    }
}

int func_020196e0(int value) {
    int i;

    if (value == 0) {
        OS_Terminate();
    }
    for (i = 0; i < 8; i++) {
        if (data_020b6ee0.mUnk_9024[i].mUnk_184 == 0) {
            Unk_020bff04 *e = &data_020b6ee0.mUnk_9024[i];
            e->mUnk_184 = value;
            if (e->mUnk_184 == 1) {
                func_020271ec(&e->mModel, 0, 0, 2, data_0201aba0[4]);
            }
            data_020b6ee0.mUnk_a149++;
            return i;
        }
    }
    return -1;
}

void func_02019750(u32 index) {
    if (index >= 8) {
        OS_Terminate();
    }
    if (data_020b6ee0.mUnk_9024[index].mUnk_184 == 0) {
        OS_Terminate();
    }
    data_020b6ee0.mUnk_9024[index].mUnk_184 = 0;
    data_020b6ee0.mUnk_a149--;
}

void func_02019788(void) {
    DrawParams params;
    Unk_020bff04 *e = data_020b6ee0.mUnk_9024;
    int i;

    for (i = 0; i < 8; i++) {
        if (data_020b6ee0.mUnk_9024[i].mUnk_184 != 0 && e[i].mUnk_184 == 1) {
            func_02023f58(&params, e[i].mUnk_178.x, e[i].mUnk_178.y, e[i].mUnk_178.z, 0x100000, 0, 2, 0, 0x100);
            func_0202733c(&e[i], &params, 0);
        }
    }
}

// Looks for an element of the table at mUnk_9024 near the record position; one in state 1 is released and the move
// object is started.
void func_02019800(StageRec *rec) {
    int i = func_02019874(&rec->mUnk_1a0);

    if (i != -1) {
        Unk_020bff04 *e = &data_020b6ee0.mUnk_9024[i];

        if (e->mUnk_184 == 1) {
            func_02019750(i);
            func_02019a38(rec, rec->mUnk_1f0, rec->mUnk_1f4, 0x3e, 0x5e, 1);
            func_0202f110(0xe0, 0x64, 0, 0x40, 0xffff, 0);
            data_020b6ee0.mUnk_a570 = 1;
        }
    }
}

// Index of the first used element of the table whose x/z distance to pos is under 8 units of 0xc800, or -1.
int func_02019874(VecFx32 *pos) {
    Unk_020bff04 *e = data_020b6ee0.mUnk_9024;
    int i;

    for (i = 0; i < 8; i++) {
        if (data_020b6ee0.mUnk_9024[i].mUnk_184 != 0) {
            s32 dx = e[i].mUnk_178.x - pos->x;
            s32 dz = e[i].mUnk_178.z - pos->z;
            u8 inRange = (abs(dx) + abs(dz)) / 0xc800 < 8;

            if (inRange == 1) {
                return i;
            }
        }
    }
    return -1;
}

// Picks a random point around a fixed position and adds a table element there when none lies near it.
void func_020198f0(void) {
    VecFx32 v;
    int j;

    func_02031fc4(0, &v);
    v.x += (0x40 - (func_0201fd68() & 0x7f)) * 0xc800;
    v.z += (0x40 - (func_0201fd68() & 0x7f)) * 0xc800;
    v.y += 0x19000;
    if (func_02019874(&v) == -1) {
        j = func_020196e0(1);
        if (j != -1) {
            Unk_020bff04 *e = &data_020b6ee0.mUnk_9024[j];

            e->mUnk_178 = v;
        }
    }
}

// Eases the offset of the record with number index toward mulX and mulY (scaled by the screen mode) and draws the
// sprite at it.
void func_02019974(StageRec *rec, int index, int sprite, int mulX, int mulY, int width) {
    int scale;
    StageOffset *e;

    if (rec->mUnk_22f != 0) {
        if ((u32) index >= 3) {
            OS_Terminate();
        }
        e = &rec->mUnk_1f8[index];
        switch (data_020b6ee0.mUnk_a146) {
            case 0:
                scale = 0x200;
                break;
            case 1:
                scale = 0x280;
                break;
            default:
                scale = 0x500;
                break;
        }
        if (index == 1 && rec->mUnk_22e == 0) {
            sprite = 0;
        }
        if (index == 1 && !((data_020c18bc.mUnk_45c >> 5) & 1)) {
            sprite = 0;
        }
        mulX = mulX * scale;
        mulY = mulY * scale;
        func_0200b928(&e->x, mulX, 8);
        func_0200b928(&e->y, mulY, 8);
        if (sprite != 0) {
            func_0202cf08(sprite, rec->mUnk_1f0 + e->x / 512, rec->mUnk_1f4 - e->y / 512, width, 1);
        }
    }
}

// Starts the move object for the record, with the end point depending on mode.
void func_02019a38(StageRec *rec, int x, int y, int c, int d, int mode) {
    MoveObj *o = &data_020b6ee0.mUnk_9de8;
    Vec2i pos;
    int r;

    o->mUnk_310 = rec;
    r = func_020117e0(rec->mInfo.mUnk_12);
    pos.x = mode == 1 ? x : r;
    pos.y = mode == 1 ? y : 0x5e;
    pos.y += mode == 1 ? 0 : -252;
    func_0200c034(o, 0x1f, -1, &pos, 0x3c, 1, 1);
    func_0200c124(o, 1, 7);
    pos.x = mode == 1 ? r : x;
    pos.y = mode == 1 ? 0x5e : y;
    pos.y += mode == 1 ? -252 : 0;
    func_0200c0dc(o, 0x800, &pos, 0);
}

void func_02019ad8(void) {
    func_0200c164(&data_020b6ee0.mUnk_9de8);
    func_0200d6c8();
}

// Runs the move object; when it has ended, calls the record update.
void func_02019aec(void) {
    MoveObj *o = &data_020b6ee0.mUnk_9de8;

    if (o->mActive == 1) {
        func_0200c174(o, 0);
        if (o->mActive == 1) {
            func_0200c470(o);
            return;
        }
        if (o->mUnk_310 == 0) {
            OS_Terminate();
        }
        func_02014ffc(o->mUnk_310, 10, 1, 1);
    }
}

void func_02019b38(void) {}

// Per-frame update of the stage camera: eases the zoom, follows the touch drag and moves both targets.
void func_02019b3c(VecFx32 *a, VecFx32 *b) {
    Unk_020c1030 *p = &data_020b6ee0.mUnk_a150;
    VecFx32 v;
    s32 out0[2];
    s32 out1[2];

    func_02031fc4(0, &v);
    data_020b6ee0.mUnk_a574.mUnk_0c.x = a->x;
    data_020b6ee0.mUnk_a574.mUnk_0c.y = a->y;
    data_020b6ee0.mUnk_a574.mUnk_0c.z = a->z;
    data_020b6ee0.mUnk_a574.mUnk_00.x = b->x;
    data_020b6ee0.mUnk_a574.mUnk_00.y = b->y;
    data_020b6ee0.mUnk_a574.mUnk_00.z = b->z;
    func_0200b928(&p->mUnk_3d0, p->mUnk_3d4, 0x20);
    if (p->mUnk_3d0 != 0x200) {
        data_020b6ee0.mUnk_a574.mUnk_00.x =
            (data_020b6ee0.mUnk_a574.mUnk_0c.x - data_020b6ee0.mUnk_a574.mUnk_00.x) * p->mUnk_3d0 / 512;
        data_020b6ee0.mUnk_a574.mUnk_00.y =
            (data_020b6ee0.mUnk_a574.mUnk_0c.y - data_020b6ee0.mUnk_a574.mUnk_00.y) * p->mUnk_3d0 / 512;
        data_020b6ee0.mUnk_a574.mUnk_00.z =
            (data_020b6ee0.mUnk_a574.mUnk_0c.z - data_020b6ee0.mUnk_a574.mUnk_00.z) * p->mUnk_3d0 / 512;
        data_020b6ee0.mUnk_a574.mUnk_00.x = data_020b6ee0.mUnk_a574.mUnk_0c.x - data_020b6ee0.mUnk_a574.mUnk_00.x;
        data_020b6ee0.mUnk_a574.mUnk_00.y = data_020b6ee0.mUnk_a574.mUnk_0c.y - data_020b6ee0.mUnk_a574.mUnk_00.y;
        data_020b6ee0.mUnk_a574.mUnk_00.z = data_020b6ee0.mUnk_a574.mUnk_0c.z - data_020b6ee0.mUnk_a574.mUnk_00.z;
    }
    func_0200b960(&data_020c5e44.mUnk_80, data_020b6ee0.mUnk_a574.mUnk_00, data_020b6ee0.mUnk_a574.mUnk_34);
    func_0200b960(&data_020c5e44.mUnk_98, data_020b6ee0.mUnk_a574.mUnk_0c, data_020b6ee0.mUnk_a574.mUnk_34);
    func_02033b8c();
    func_02033520();
    func_02025db4(p, out1);
    if (p->mUnk_3d8 == 0) {
        func_0202cf08(0xe25, 0x7f, out1[1], 0xdc, 1);
        func_0202cf08(0xe26, out1[0], 0x5f, 0xd2, 1);
        func_0202cf08(0xe29, out1[0], out1[1], 0xc8, 1);
        func_0202cf08(0xe2e, out1[0], out1[1], 0xbe, 1);
    } else {
        p->mUnk_3d8--;
    }
    if (data_020c18bc.mUnk_2ac_1 == 1 && func_0200bd80(0x40000000, 0x101) == 0 &&
        func_0200bd80(0x40000000, 0x102) == 0 && data_020b6ee0.mUnk_a5d0 == 0 && p->mUnk_3dd == 1) {
        s32 dx;
        s32 dy;

        func_02025db4(p, out0);
        dy = data_020c18bc.mUnk_2af - out0[1];
        dx = data_020c18bc.mUnk_2ae - out0[0];
        p->mUnk_44 = (dx << 9) / 16;
        p->mUnk_4c = (dy << 9) / 16;
    }
    if (p->mUnk_44 != 0 || p->mUnk_4c != 0) {
        s32 d;

        p->mUnk_00.mPos.x += p->mUnk_44;
        p->mUnk_00.mPos.z += p->mUnk_4c;
        p->mUnk_00.mPos.y =
            func_02031270(p->mUnk_00.mPos.x, p->mUnk_00.mPos.z, p->mUnk_00.mPos.y + 0x32000, 0, 0, 0) + 0x1900;
        d = func_0201faf0((p->mUnk_00.mPos.x - v.x) / 512, (p->mUnk_00.mPos.z - v.z) / 512);
        if (d > 0x320) {
            s32 r = 0x64000 / d;

            p->mUnk_00.mPos.x = v.x + (p->mUnk_00.mPos.x - v.x) / 512 * r;
            p->mUnk_00.mPos.z = v.z + (p->mUnk_00.mPos.z - v.z) / 512 * r;
        }
    }
    func_0200b928(&p->mUnk_44, 0, 0x10);
    func_0200b928(&p->mUnk_4c, 0, 0x10);
    {
        s32 d = (p->mUnk_00.mPos.z - a->z) / 16;

        b->z += d;
        a->z += d;
    }
    {
        s32 d = (p->mUnk_00.mPos.x - a->x) / 16;

        b->x += d;
        a->x += d;
    }
}

// Sets up the stage camera view and the follow state.
void func_02019df8(void) {
    Unk_020c1030 *p = &data_020b6ee0.mUnk_a150;

    func_0201eb20();
    func_0202c1c4();
    func_0202bdd8(0x10cb, 0x10ca, 0x10cc, 0x82, 2);
    func_0202bdd8(0x10cb, 0x10ca, 0x10cc, 0, 1);
    func_0201dde0(0xda3, 2);
    func_02020854(0);
    func_0200bdcc(2, 0x10);
    func_02021fb0(0x1c7, 0x1c6, 0x100, 0x10);
    func_020325b0(0xdda, 0, 0xddb, 0, 0, 0);
    data_020c5e44.mUnk_28 = func_02019b38;
    data_020c6144.mUnk_54 = 0x640;
    data_020c6144.mUnk_44 = 0x9c4000;
    func_02031fc4(0, &data_020c180c);
    func_02031fc4(1, &data_020c1800);
    func_02031fc4(0, &data_020c5e44.mUnk_80);
    func_02031fc4(0, &data_020c5e44.mUnk_98);
    data_020c5e44.mUnk_80.z += 0x352000;
    data_020c5e44.mUnk_98.y += 0x32000;
    data_020b6ee0.mUnk_a574.mUnk_34 = 0x80;
    func_0200bf38(8);
    memset(p, 0, 0x44);
    p->mUnk_00.mUnk_43 = 2;
    p->mUnk_00.mUnk_28 = 0x8000;
    func_02031fc4(data_020b6ee0.mUnk_a530 + 0xa00, &p->mUnk_00.mPos);
    p->mUnk_00.mPos.y += 0x1900;
    p->mUnk_3dc = 0;
    p->mUnk_3dd = 1;
    data_020b6ee0.mUnk_a5d0 = 1;
    p->mUnk_3d4 = 0x200;
    p->mUnk_3d0 = p->mUnk_3d4;
    data_020b6ee0.mUnk_a574.mUnk_38 = 0x28;
    p->mUnk_44 = 0;
    p->mUnk_4c = 0;
    p->mUnk_3d8 = 0xb4;
    func_02027230(&p->mUnk_50, 0xde5, 0xde4, 2, 0);
    if ((u32) data_020b6ee0.mUnk_a564 > 0xc) {
        func_0202f110(0x59, 0x64, 0, 0x40, 0xffff, 0);
    }
}
