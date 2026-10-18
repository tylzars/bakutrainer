#include <nitro/os.h>
#include <nitro/mi.h>
#include <nitro/pxi.h>
#include <nitro/math.h>
#include <nitro/wm.h>

u16 func_02046478(void) {
    u32 e = OS_DisableInterrupts();
    WMWork *w = data_020cd100.work;
    u16 r;
    if (w != 0) {
        r = w->mUnk_150;
    } else {
        r = 0;
    }
    OS_RestoreInterrupts(e);
    return r;
}

u16 func_020464a8(void) {
    u32 e = OS_DisableInterrupts();
    WMWork *w = data_020cd100.work;
    u32 r;
    if (w != 0) {
        r = w->bitmap;
    } else {
        r = 0;
    }
    OS_RestoreInterrupts(e);
    return (u16) r;
}

int func_020464d8(u32 idx, WMCallback cb, u32 arg) {
    WMInd ind;
    u32 e;
    int r;
    WMWork *w;
    if (cb != 0) {
        MI_CpuFill8(&ind, 0, 0x44);
        ind.id = 0x82;
        ind.mUnk_02 = 0;
        ind.mUnk_04 = 0x19;
        ind.mUnk_06 = idx;
        ind.mUnk_08 = 0;
        ind.mUnk_0c = 0;
        ind.mUnk_10 = 0;
        ind.mUnk_1a = 0xffff;
        ind.mUnk_1c = arg;
        ind.mUnk_12 = 0;
        OS_GetMacAddress(ind.mac);
    }
    e = OS_DisableInterrupts();
    r = func_02046380();
    if (r != 0) {
        OS_RestoreInterrupts(e);
        return r;
    }
    w = func_02046370();
    w->indCb[idx] = cb;
    w->indArg[idx] = arg;
    if (cb != 0) {
        ind.mUnk_22 = func_020464a8();
        ind.mUnk_20 = func_02046478();
        cb(&ind);
    }
    OS_RestoreInterrupts(e);
    return 0;
}

int func_020465b0(void *dest) {
    WMWork *w = func_02046370();
    int r = func_02046380();
    if (r != 0) {
        return r;
    }
    if (dest == 0) {
        return 6;
    }
    DC_InvalidateRange(w->st, 2000);
    MIi_CpuCopyFast(w->st, dest, 2000);
    return 0;
}

int func_020465fc(void) {
    WMWork *w = func_02046370();
    if (func_020463e4(2, 7, 8) != 0) {
        return 0;
    }
    DC_InvalidateRange(&w->st->mUnk_00c, 4);
    if (w->st->mUnk_00c == 1) {
        return 0;
    }
    DC_InvalidateRange(&w->st->mUnk_03c, 4);
    return (w->st->mUnk_03c + 0x1f) & ~0x1f;
}

int func_02046668(void) {
    WMWork *w = func_02046370();
    u32 r, t;
    if (func_020463e4(2, 7, 8) != 0) {
        return 0;
    }
    DC_InvalidateRange(&w->st->mUnk_00c, 4);
    if (w->st->mUnk_00c == 1) {
        return 0;
    }
    DC_InvalidateRange(&w->st->mUnk_188, 2);
    r = w->st->mUnk_188 == 0 ? 1 : 0;
    DC_InvalidateRange(&w->st->mUnk_03e, 2);
    t = w->st->mUnk_03e;
    if (r == 1) {
        DC_InvalidateRange(&w->st->mUnk_0f8, 2);
        return (((t + 12) * w->st->mUnk_0f8 + 0x29) & ~0x1f) << 1;
    }
    return ((t + 0x51) & ~0x1f) << 1;
}

u32 func_0204672c(void) {
    WMWork *w = func_02046370();
    if (func_02046380() != 0) {
        return 0;
    }
    DC_InvalidateRange(w->st, 2);
    switch (w->st->mUnk_000) {
        case 9:
            DC_InvalidateRange(&w->st->mUnk_182, 2);
            if (w->st->mUnk_182 == 0) {
                return 0;
            }
        case 10:
        case 11:
            DC_InvalidateRange(&w->st->mUnk_0bc, 2);
            return w->st->mUnk_0bc;
        default:
            return 0;
    }
}

u16 func_020467bc(void) {
    u8 mac[8];
    u16 sum;
    u16 t;
    int i;
    OS_GetMacAddress(mac);
    i = 0;
    sum = 0;
    for (; i < 6; i++) {
        sum = sum + mac[i];
    }
    t = sum + *(u32 *) 0x27ffc3c;
    t = t * 7;
    return 200 + t % 20;
}

int func_0204684c(WMCallback cb) {
    int r = func_0204639c();
    if (r != 0) {
        return r;
    }
    func_020461e4(1, cb);
    r = func_02046254(1, 0);
    if (r == 0) {
        r = 2;
    }
    return r;
}

int func_02046884(WMCallback cb) {
    int r = func_020463e4(1, 2);
    if (r != 0) {
        return r;
    }
    func_020461e4(2, cb);
    r = func_02046254(2, 0);
    if (r == 0) {
        r = 2;
    }
    return r;
}

int func_020468c4(WMCallback cb, WMParentParam *p) {
    int r = func_020463e4(1, 2);
    if (r != 0) {
        return r;
    }
    if (p == 0) {
        return 6;
    }
    if (p->mUnk_04 != 0) {
        if (p->mUnk_00 == 0) {
            return 6;
        }
    }
    if (p->mUnk_34 + (p->mUnk_14 != 0 ? 0x2a : 0) > 0x200 || p->mUnk_36 + (p->mUnk_14 != 0 ? 6 : 0) > 0x200) {
        return 6;
    }
    func_020469a0(p);
    func_020461e4(7, cb);
    DC_StoreRange(p, 0x40);
    if (p->mUnk_04 != 0) {
        DC_StoreRange(p->mUnk_00, p->mUnk_04);
    }
    r = func_02046254(7, 1, p);
    if (r == 0) {
        r = 2;
    }
    return r;
}

int func_020469a0(WMParentParam *p) {
    if (p->mUnk_04 > 0x70) {
        return 0;
    }
    if (p->mUnk_18 < 10 || p->mUnk_18 > 0x3e8) {
        return 0;
    }
    if (p->mUnk_32 < 1 || p->mUnk_32 > 14) {
        return 0;
    }
    return 1;
}

int func_020469f0(WMCallback cb, int x) {
    int r = func_020463e4(1, 2);
    WMWork *w;
    if (r != 0) {
        return r;
    }
    w = func_02046370();
    w->mUnk_150 = 0;
    w->bitmap = 0;
    func_020461e4(8, cb);
    r = func_02046254(8, 1, x);
    if (r == 0) {
        r = 2;
    }
    return r;
}

int func_02046a4c(WMCallback cb) {
    return func_020469f0(cb, 1);
}

int func_02046a5c(WMCallback cb) {
    int r = func_020463e4(1, 7);
    if (r != 0) {
        return r;
    }
    func_020461e4(9, cb);
    r = func_02046254(9, 0);
    if (r == 0) {
        r = 2;
    }
    return r;
}

typedef struct {
    /* 00 */ u16 mUnk_00;
    /* 02 */ u8 mUnk_02[2];
    /* 04 */ void *mUnk_04;
    /* 08 */ u8 mUnk_08[0x18];
    /* 20 */ u32 mUnk_20;
    /* 24 */ u16 mUnk_24;
    /* 26 */ u16 mUnk_26;
} WMConnectMsg;

int func_02046a9c(WMCallback cb, u16 *p, void *q, u32 arg, u16 extra) {
    WMConnectMsg m;
    int r = func_020463e4(1, 2);
    WMWork *w;
    if (r != 0) {
        return r;
    }
    if (p == 0) {
        return 6;
    }
    DC_StoreRange(p, p[0] * 2);
    w = func_02046370();
    w->mUnk_150 = 0;
    w->bitmap = 0;
    func_020461e4(12, cb);
    m.mUnk_00 = 12;
    m.mUnk_04 = p;
    if (q != 0) {
        MI_CpuCopy8(q, m.mUnk_08, 0x18);
    } else {
        MI_CpuFill8(m.mUnk_08, 0, 0x18);
    }
    m.mUnk_20 = arg;
    m.mUnk_26 = extra;
    r = func_02046300(&m, 0x28);
    if (r == 0) {
        r = 2;
    }
    return r;
}

int func_02046b68(WMCallback cb, u32 idx) {
    WMWork *w = func_02046370();
    int r = func_020463e4(5, 7, 9, 8, 10, 11);
    if (r != 0) {
        return r;
    }
    if (w->st->mUnk_000 == 7 || w->st->mUnk_000 == 9) {
        if (idx < 1 || idx > 15) {
            return 6;
        }
        DC_InvalidateRange(&w->st->mUnk_182, 2);
        if (!(w->st->mUnk_182 & (1 << idx))) {
            return 7;
        }
    } else if (idx != 0) {
        return 6;
    }
    func_020461e4(13, cb);
    r = func_02046254(13, 1, 1 << idx);
    if (r == 0) {
        r = 2;
    }
    return r;
}

int func_02046c50(WMCallback cb, u32 a, int b, u32 c, u16 e, void *f) {
    WMWork *w = func_02046370();
    WMStatus *st = w->st;
    u32 buf[16];
    int r = func_020463e4(2, 7, 8);
    if (r != 0) {
        return r;
    }
    DC_InvalidateRange(&st->mUnk_188, 2);
    DC_InvalidateRange(&st->mUnk_0c6, 2);
    if (st->mUnk_188 != 0 && st->mUnk_0c6 != 1) {
        return 3;
    }
    DC_InvalidateRange(&st->mUnk_00c, 4);
    if (st->mUnk_00c == 1) {
        return 3;
    }
    if (b & 0x3f) {
        return 6;
    }
    if (e & 0x1f) {
        return 6;
    }
    DC_InvalidateRange(&st->mUnk_09c, 2);
    if (st->mUnk_09c == 0) {
        if (b < func_02046668()) {
            return 6;
        }
        if (e < func_020465fc()) {
            return 6;
        }
    }
    func_020461e4(0xe, cb);
    MIi_CpuClear32(0, buf, 0x40);
    ((u16 *) buf)[0] = 0xe;
    buf[1] = a;
    buf[2] = (u32) b >> 1;
    buf[3] = c;
    buf[4] = e;
    MIi_CpuClear32(0, &buf[5], 0x1c);
    MIi_CpuCopy32(f, &buf[12], 0x10);
    r = func_02046300(buf, 0x40);
    if (r == 0) {
        r = 2;
    }
    return r;
}

int func_02046dc0(WMCallback cb, u32 a, u32 b, u32 c, u16 e, u16 f) {
    u32 t[4];
    MIi_CpuClear32(0, t, 0x10);
    t[0] = 3;
    ((u16 *) t)[2] = f;
    ((u16 *) t)[3] = f;
    return func_02046c50(cb, a, b, c, e, t);
}

int func_02046e28(WMCallback cb, void *a, void *buf, u16 size, u16 x, u16 y, u16 z) {
    u16 ok = 1;
    WMStatus *st = func_02046370()->st;
    int r = func_020463e4(2, 9, 10);
    if (r != 0) {
        return r;
    }
    DC_InvalidateRange(&st->mUnk_188, 2);
    if (st->mUnk_188 == 0) {
        DC_InvalidateRange(&st->mUnk_182, 2);
        ok = st->mUnk_182;
        DC_InvalidateRange(&st->mUnk_086, 2);
    }
    if (buf == 0) {
        return 6;
    }
    if (ok == 0) {
        return 7;
    }
    DC_InvalidateRange(&st->mUnk_07c, 2);
    if ((u32) buf == st->mUnk_07c) {
        return 6;
    }
    if (size > 0x200) {
        return 6;
    }
    if (size == 0) {
        return 6;
    }
    DC_StoreRange(buf, size);
    r = func_02046254(0xf, 7, buf, size, x, y, z, cb, a);
    if (r == 0) {
        r = 2;
    }
    return r;
}

int func_02046f58(WMCallback cb) {
    WMWork *w = func_02046370();
    int r = func_020463e4(2, 9, 10);
    if (r != 0) {
        return r;
    }
    DC_InvalidateRange(&w->st->mUnk_00c, 4);
    if (w->st->mUnk_00c == 0) {
        return 3;
    }
    func_020461e4(16, cb);
    r = func_02046254(16, 0);
    if (r == 0) {
        r = 2;
    }
    return r;
}

int func_02046fc8(WMDataSharing *buf, u16 n, u16 mask, u16 m, int flag) {
    int i;
    u32 cnt;
    u8 *cur;
    u32 bm = 1;
    u32 aid;
    int k;
    int r = func_020463e4(2, 9, 10);
    if (r != 0) {
        return r;
    }
    if (buf == 0) {
        return 6;
    }
    if (n >= 0x10) {
        return 6;
    }
    if (mask == 0) {
        return 6;
    }
    aid = func_02046478();
    if (aid == 0) {
        bm = func_020464a8();
    }
    MIi_CpuClearFast(0, buf, 0x820);
    buf->mUnk_808 = 0;
    buf->mUnk_80a = 0;
    buf->mUnk_80c = 0;
    buf->mUnk_810 = m;
    buf->mUnk_816 = n;
    buf->mUnk_80e = 0;
    buf->mUnk_818 = flag != 0 ? 1 : 0;
    {
        u16 t = mask | (1 << aid);
        buf->mUnk_80e = t;
        cnt = func_0204873c(t);
    }
    buf->mUnk_812 = cnt;
    buf->mUnk_814 = m * cnt;
    if (buf->mUnk_814 > 0x1fc) {
        buf->mUnk_80e = 0;
        return 6;
    }
    buf->mUnk_814 = buf->mUnk_814 + 4;
    buf->mUnk_81c = 1;
    if (aid == 0) {
        aid = bm | 1;
        for (i = 0; i < 4; i++) {
            buf->mUnk_000[i].mUnk_00 = buf->mUnk_80e & aid;
        }
        func_020464d8(n, func_02047684, (u32) buf);
        cur = (u8 *) buf;
        for (k = 0; k < (buf->mUnk_818 == 1 ? 2 : 1); k++) {
            buf->mUnk_808 = (buf->mUnk_808 + 1) & 3;
            r = func_02046e28(func_020475ac, buf, cur, buf->mUnk_814, buf->mUnk_80e & bm, buf->mUnk_816, 1);
            if (r == 7) {
                buf->mUnk_800[k] = 0xffff;
                buf->mUnk_80a = (buf->mUnk_80a + 1) & 3;
            } else if (r != 0 && r != 2) {
                buf->mUnk_81c = 5;
                return 1;
            }
            cur += 0x200;
        }
    } else {
        buf->mUnk_80a = 3;
        func_020464d8(n, func_020477b0, (u32) buf);
    }
    return 0;
}

int func_0204721c(WMDataSharing *buf) {
    if (buf == 0) {
        return 6;
    }
    if (buf->mUnk_80e == 0) {
        return 3;
    }
    func_020464d8(buf->mUnk_816, 0, 0);
    buf->mUnk_80e = 0;
    buf->mUnk_81c = 0;
    return 0;
}

int func_02047264(WMDataSharing *buf, void *pa, void *pb) {
    u32 bm;
    u32 aid;
    u32 got;
    u32 idx;
    u32 arm;
    u32 res;
    u32 off;
    int r = func_020463e4(2, 9, 10);
    if (r != 0) {
        return r;
    }
    if (buf == 0) {
        return 6;
    }
    if (pa == 0) {
        return 6;
    }
    if (pb == 0) {
        return 6;
    }
    aid = func_02046478();
    if (aid == 0) {
        bm = func_020464a8();
    }
    if (buf->mUnk_81c == 5) {
        return 1;
    }
    if (buf->mUnk_81c != 1 && buf->mUnk_81c != 4) {
        return 3;
    }
    res = 5;
    if (aid == 0) {
        got = 0;
        arm = 0;
        if (buf->mUnk_81c == 4) {
            buf->mUnk_81c = 1;
            idx = (buf->mUnk_808 + 3) & 3;
            r = func_02046e28(func_020475ac, buf, (u8 *) buf + (idx << 9), buf->mUnk_814, buf->mUnk_80e & bm,
                              buf->mUnk_816, 1);
            if (r == 7) {
                buf->mUnk_800[idx] = 0xffff;
                buf->mUnk_80a = (buf->mUnk_80a + 1) & 3;
            } else if (r != 0 && r != 2) {
                buf->mUnk_81c = 5;
                return 1;
            }
        }
        if (buf->mUnk_80c != buf->mUnk_80a) {
            off = buf->mUnk_80c << 9;
            *(u16 *) ((u8 *) buf + off) |= 1;
            MIi_CpuCopy16((u8 *) buf + (buf->mUnk_80c << 9), pb, 0x200);
            got = 1;
            res = 0;
            buf->mUnk_81a = buf->mUnk_800[buf->mUnk_80c];
            buf->mUnk_80c = (buf->mUnk_80c + 1) & 3;
            if (buf->mUnk_818 == 0 && bm != 0 && *(u16 *) ((u8 *) buf + (buf->mUnk_808 << 9)) == 1) {
                arm = got;
            } else {
                arm = 0;
            }
        }
        func_02047970(buf, 0);
        if (got != 0) {
            func_020478a4(buf, 0, pa);
            if (buf->mUnk_818 == 0) {
                func_02047970(buf, arm);
            }
        }
    } else {
        got = 0;
        if (buf->mUnk_81c == 4) {
            got = 1;
            buf->mUnk_81c = 1;
        } else if (buf->mUnk_80c != buf->mUnk_808) {
            off = buf->mUnk_80c << 9;
            if ((*(u16 *) ((u8 *) buf + off) & 1) == 0) {
                *(u16 *) ((u8 *) buf + off) |= 1;
            } else {
                MIi_CpuCopy16((u8 *) buf + off, pb, 0x200);
                buf->mUnk_81a = buf->mUnk_800[buf->mUnk_80c];
                got = 1;
                res = 0;
                buf->mUnk_80c = (buf->mUnk_80c + 1) & 3;
            }
        }
        if (got != 0) {
            u8 *p = (u8 *) buf + (buf->mUnk_80a << 9);
            MIi_CpuCopy16(pa, p + 0x20, buf->mUnk_810);
            r = func_02046e28(func_020475ac, buf, p + 0x20, buf->mUnk_810, buf->mUnk_80e, buf->mUnk_816, 1);
            buf->mUnk_80a = (buf->mUnk_80a + 1) & 3;
            if (r != 2 && r != 0) {
                buf->mUnk_81c = 5;
                res = 1;
            }
        }
    }
    return res;
}

void func_020475ac(u16 *m) {
    WMWork *w = func_02046370();
    WMCallback cb = w->indCb[m[5]];
    WMDataSharing *buf = (WMDataSharing *) w->indArg[m[5]];
    u16 aid;
    if (cb != func_02047684 && cb != func_020477b0) {
        return;
    }
    if (buf == 0) {
        return;
    }
    if ((u32) buf != *(u32 *) (m + 0x10)) {
        return;
    }
    aid = func_02046478();
    if (m[1] == 0) {
        if (aid != 0) {
            return;
        }
        buf->mUnk_800[buf->mUnk_80a] = m[0xd] >> 1;
        buf->mUnk_80a = (buf->mUnk_80a + 1) & 3;
        return;
    }
    if (m[1] == 10) {
        if (aid != 0) {
            buf->mUnk_80a = (buf->mUnk_80a + 3) & 3;
        }
        buf->mUnk_81c = 4;
        return;
    }
    buf->mUnk_81c = 5;
}

void func_02047684(WMInd *m) {
    WMDataSharing *buf = (WMDataSharing *) m->mUnk_1c;
    u32 bit;
    u32 e;
    u16 j;
    u16 idx;
    u32 mask;
    if (buf == 0) {
        return;
    }
    if (m->mUnk_02 == 0) {
        switch (m->mUnk_04) {
            case 0x15:
                func_020478a4(buf, m->mUnk_12, (void *) m->mUnk_0c);
                func_02047970(buf, 0);
                return;
            case 7:
                func_02047970(buf, 0);
                return;
            case 9:
            case 0x1a:
                bit = 1 << m->mUnk_12;
                e = OS_DisableInterrupts();
                idx = buf->mUnk_808;
                mask = ~bit;
                buf->mUnk_000[idx].mUnk_00 &= mask;
                if (buf->mUnk_818 == 1) {
                    j = (idx + 1) & 3;
                    buf->mUnk_000[j].mUnk_00 &= mask;
                }
                OS_RestoreInterrupts(e);
                func_02047970(buf, 0);
                if (buf->mUnk_818 == 1) {
                    func_02047970(buf, 0);
                }
                return;
            case 0x19:
                return;
            default:
                return;
        }
    } else {
        buf->mUnk_81c = 5;
    }
}

void func_020477b0(WMInd *m) {
    WMDataSharing *buf = (WMDataSharing *) m->mUnk_1c;
    u32 len;
    u16 bits;
    u16 *data;
    u16 aid;
    if (buf == 0) {
        return;
    }
    if (m->mUnk_02 == 0) {
        switch (m->mUnk_04) {
            case 0x15:
                data = (u16 *) m->mUnk_0c;
                len = m->mUnk_10;
                bits = data[0];
                aid = func_02046478();
                if (len != buf->mUnk_814) {
                    if (len > 0x200) {
                        len = 0x200;
                    }
                }
                if (len < 4) {
                    return;
                }
                if (!(bits & (1 << aid))) {
                    return;
                }
                MIi_CpuCopy16(data, (u8 *) buf + (buf->mUnk_808 << 9), len);
                buf->mUnk_800[buf->mUnk_808] = m->mUnk_1a >> 1;
                buf->mUnk_808 = (buf->mUnk_808 + 1) & 3;
                return;
            case 7:
            case 9:
            case 0x19:
            case 0x1a:
                return;
            default:
                return;
        }
    } else {
        buf->mUnk_81c = 5;
    }
}

void func_020478a4(WMDataSharing *buf, u32 idx, void *src) {
    u16 bit = 1 << idx;
    u32 cur;
    u8 *p;
    u32 e;
    u32 off;
    if (!(buf->mUnk_80e & bit)) {
        return;
    }
    cur = buf->mUnk_808;
    if (!(((u16 *) buf)[cur << 8] & bit)) {
        if (buf->mUnk_818 != 1) {
            return;
        }
        cur = (cur + 1) & 3;
        if (!(((u16 *) buf)[cur << 8] & bit)) {
            return;
        }
    }
    p = func_02047b18(buf, buf->mUnk_80e, (u8 *) buf + cur * 0x200 + 4, idx);
    if (src != 0) {
        MIi_CpuCopy16(src, p, buf->mUnk_810);
    } else {
        MIi_CpuClear16(0, p, buf->mUnk_810);
    }
    e = OS_DisableInterrupts();
    off = cur << 9;
    *(u16 *) ((u8 *) buf + off) &= ~bit;
    *(u16 *) (((u8 *) buf + 2) + off) |= bit;
    OS_RestoreInterrupts(e);
}

void func_02047970(WMDataSharing *buf, int flag) {
    u32 e = OS_DisableInterrupts();
    u32 nxt;
    u32 cur;
    u32 bm;
    u32 clr;
    int r;
    if (*(u16 *) ((u8 *) buf + (buf->mUnk_808 << 9)) == 0) {
        bm = func_020464a8();
        cur = buf->mUnk_808;
        nxt = (cur + 1) & 3;
        if (buf->mUnk_818 == 1) {
            clr = (nxt + 1) & 3;
        } else {
            clr = nxt;
        }
        MIi_CpuClear16(0, (u8 *) buf + (clr << 9), 0x200);
        *(u16 *) ((u8 *) buf + (clr * 0x200)) = buf->mUnk_80e & (bm | 1);
        buf->mUnk_808 = nxt;
        *(u16 *) ((u8 *) buf + (cur << 9)) = buf->mUnk_80e;
        if (flag == 1) {
            *(u16 *) ((u8 *) buf + (cur << 9)) &= ~1;
        }
        OS_RestoreInterrupts(e);
        r = func_02046e28(func_020475ac, buf, (u8 *) buf + (cur * 0x200), buf->mUnk_814, buf->mUnk_80e & bm,
                          buf->mUnk_816, 1);
        if (r == 7) {
            buf->mUnk_800[cur] = 0xffff;
            buf->mUnk_80a = (buf->mUnk_80a + 1) & 3;
            return;
        }
        if (r != 0 && r != 2) {
            buf->mUnk_81c = 5;
        }
    } else {
        OS_RestoreInterrupts(e);
    }
}

u8 *func_02047ac4(WMDataSharing *buf, u16 *p, u32 n) {
    u16 a = p[0];
    u16 b = p[1];
    u32 bit = 1 << n;
    if (buf == 0) {
        return 0;
    }
    if (p == 0) {
        return 0;
    }
    if (!(a & bit)) {
        return 0;
    }
    if (!(b & bit)) {
        return 0;
    }
    return func_02047b18(buf, a, (u8 *) (p + 2), n);
}

u8 *func_02047b18(WMDataSharing *buf, u32 mask, u8 *base, u32 n) {
    u32 cnt = func_0204873c(mask & ((1 << n) - 1));
    return base + buf->mUnk_810 * cnt;
}

int func_02047b48(WMDataSharing *buf, u16 n) {
    return func_02046fc8(buf, n, 0xffff, 2, 1);
}

int func_02047b64(WMDataSharing *buf) {
    return func_0204721c(buf);
}

int func_02047b70(WMCallback cb, u32 n, void *p) {
    int r = func_0204639c();
    if (r != 0) {
        return r;
    }
    if (n > 3) {
        return 6;
    }
    if (n != 0) {
        if (p == 0) {
            return 6;
        }
        DC_StoreRange(p, 0x50);
    }
    func_020461e4(0x14, cb);
    r = func_02046254(0x14, 2, n, p);
    if (r == 0) {
        r = 2;
    }
    return r;
}
