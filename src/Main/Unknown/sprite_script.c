#include <nitro/mi.h>
#include <Main/Unknown/globals.h>
#include <Main/Unknown/falling_blocks.h>
#include <Main/Unknown/texture_blocks.h>
#include <Main/Unknown/sprite_script.h>

// Commands of a sprite script: a 16-bit word above 0xfff0 is a command, any other word is a frame to show.
#define SPRITE_CMD_SET_SPEED 0xffff
#define SPRITE_CMD_SET_STATE 0xfffe
#define SPRITE_CMD_END 0xfffd
#define SPRITE_CMD_RESET 0xfffc
#define SPRITE_CMD_ADD_STATE 0xfffb
#define SPRITE_CMD_JUMP 0xfffa
#define SPRITE_CMD_SET_REG 0xfff9
#define SPRITE_CMD_COPY_REG 0xfff8
#define SPRITE_CMD_REQUEST_STATE 0xfff7
#define SPRITE_CMD_SET_RETURN 0xfff6
#define SPRITE_CMD_SET_FRAME_HIGH 0xfff5
#define SPRITE_CMD_COMPARE_REG 0xfff4
#define SPRITE_CMD_JUMP_IF_EQUAL 0xfff3
#define SPRITE_CMD_JUMP_IF_NOT_EQUAL 0xfff2

int func_0202db1c(void *obj) {
    Sprite *o = obj;
    u32 dur;
    u32 pend;
    u32 m;
    int best;
    int i;
    u32 bestval;
    u32 cur;
    u32 v;
    const u16 *p;
    u32 op;

    dur = o->mUnk_da;
    pend = o->mUnk_14;
    for (;;) {
        m = o->mUnk_14 & o->mUnk_10;
        if (m != 0) {
            best = -1;
            i = 0;
            if (m != 0) {
                cur = o->mUnk_d8;
                for (; m != 0; i++, m >>= 1) {
                    v = o->mUnk_98[i];
                    if ((m & 1) && v != cur && (v >> 8) >= (cur >> 8)) {
                        if (best == -1 || (v >> 8) >= (bestval >> 8)) {
                            bestval = v;
                            best = i;
                        }
                    }
                }
            }
            if (best != -1) {
                o->mUnk_00 = o->mUnk_18[best];
                o->mUnk_d8 = o->mUnk_98[best];
                o->mUnk_dc = 1;
                o->mUnk_da = 1;
                o->mUnk_0c = 0;
            }
        }
        if (o->mUnk_0c != (o->mUnk_14 & o->mUnk_0c)) {
            o->mUnk_0c = 0;
            o->mUnk_00 = o->mUnk_08;
            o->mUnk_dc = o->mUnk_da;
        } else if (o->mUnk_da <= 31) {
            o->mUnk_dc += 1;
        } else {
            int step = 167;
            if (data_020c18bc.mUnk_46c & 0x40) {
                s8 sp = data_020c18bc.mUnk_48c;
                if (sp > 0) {
                    step += sp * step;
                }
            }
            o->mUnk_dc += step;
        }
        o->mUnk_14 = 0;
        while (o->mUnk_dc >= o->mUnk_da) {
            p = o->mUnk_00;
            op = *p;
            if (op > 0xfff0) {
                switch (op) {
                    case SPRITE_CMD_SET_SPEED:
                        o->mUnk_00 = p + 2;
                        dur = p[1];
                        o->mUnk_dc = 1;
                        o->mUnk_da = 1;
                        break;
                    case SPRITE_CMD_SET_STATE:
                        o->mUnk_00 = p + 2;
                        if (p[1] != 0xffff) {
                            o->mUnk_d8 = p[1];
                        }
                        o->mUnk_0c = 0;
                        o->mUnk_df = 0;
                        break;
                    case SPRITE_CMD_END:
                        o->mUnk_de = 1;
                        return o->mUnk_04;
                    case SPRITE_CMD_RESET:
                        o->mUnk_00 = p + 1;
                        o->mUnk_10 = 0;
                        o->mUnk_0c = 0;
                        o->mUnk_df = 0;
                        o->mUnk_e0 = 0;
                        break;
                    case SPRITE_CMD_ADD_STATE:
                        o->mUnk_10 |= 1 << p[1];
                        o->mUnk_18[p[1]] = (const u16 *) ((u8 *) o->mUnk_00 + (s16) p[2] + 4);
                        o->mUnk_98[o->mUnk_00[1]] = o->mUnk_00[3];
                        o->mUnk_00 += 4;
                        break;
                    case SPRITE_CMD_JUMP:
                        o->mUnk_00 = (const u16 *) ((u8 *) p + (s16) p[1] + 2);
                        break;
                    case SPRITE_CMD_SET_REG:
                        o->mUnk_e2[p[1]] = p[2];
                        o->mUnk_00 += 3;
                        break;
                    case SPRITE_CMD_COPY_REG:
                        o->mUnk_e2[p[1] >> 8] = o->mUnk_e2[p[1] & 0x7f];
                        o->mUnk_00 += 2;
                        break;
                    case SPRITE_CMD_REQUEST_STATE:
                        o->mUnk_00 = p + 2;
                        if (p[1] != 0xffff) {
                            pend |= 1 << p[1];
                        }
                        o->mUnk_14 = pend;
                        o->mUnk_d8 = 0;
                        goto next;
                    case SPRITE_CMD_SET_RETURN:
                        o->mUnk_0c |= 1 << p[1];
                        o->mUnk_08 = (const u16 *) ((u8 *) o->mUnk_00 + (s16) p[2] + 4);
                        o->mUnk_00 += 3;
                        break;
                    case SPRITE_CMD_SET_FRAME_HIGH:
                        o->mUnk_df = p[1];
                        o->mUnk_00 += 2;
                        break;
                    case SPRITE_CMD_COMPARE_REG:
                        o->mUnk_e0 &= ~1;
                        if (o->mUnk_00[2] == o->mUnk_e2[o->mUnk_00[1]]) {
                            o->mUnk_e0 |= 1;
                        }
                        o->mUnk_00 += 3;
                        break;
                    case SPRITE_CMD_JUMP_IF_EQUAL:
                        if (o->mUnk_e0 & 1) {
                            o->mUnk_00 = (const u16 *) ((u8 *) p + (s16) p[1] + 2);
                        } else {
                            o->mUnk_00 = p + 2;
                        }
                        break;
                    case SPRITE_CMD_JUMP_IF_NOT_EQUAL:
                        if (!(o->mUnk_e0 & 1)) {
                            o->mUnk_00 = (const u16 *) ((u8 *) p + (s16) p[1] + 2);
                        } else {
                            o->mUnk_00 = p + 2;
                        }
                        break;
                }
            } else {
                o->mUnk_dc -= o->mUnk_da;
                o->mUnk_00 = p + 1;
                o->mUnk_04 = op | (o->mUnk_df << 16);
            }
        }
        o->mUnk_da = dur;
        return o->mUnk_04;
    next:;
    }
}

// Clears the sprite object and starts it on the script `p`.
void func_0202de60(void *obj, void *p) {
    Sprite *o = obj;

    MI_CpuFill8(o, 0, sizeof(Sprite));
    o->mUnk_00 = p;
    o->mUnk_da = 1;
    o->mUnk_dc = 1;
}

// Queues the frame `a` of a sprite at (x, y) with priority `prio`; `e` is not used.
int func_0202de80(int a, int x, int y, int prio, int e, int mode) {
    func_0202cf08(a, x, y, prio, mode);
    return 1;
}
