#include <nitro/os.h>
#include <nitro/pxi.h>
#include <nitro/tp.h>

#define TP_PXI_TAG 6

// Touch sample as the other processor writes it into shared memory.
#define TP_SHARED ((vu16 *) 0x027fffaa)

// Calibration data in the user settings copy.
#define TP_USER_DATA 0x027ffc80

#define REG_DIVCNT (*(vu16 *) 0x04000280)
#define REG_DIV_NUMER (*(vu32 *) 0x04000290)
#define REG_DIV_DENOM64 (*(vu64 *) 0x04000298)
#define REG_DIV_RESULT (*(vu32 *) 0x040002a0)
#define DIVCNT_BUSY 0x8000

#define TP_CMD_SAMPLE_AUTO 0x10
#define TP_FIFO_FLAG_REPLY 0x1000000

typedef struct {
    u32 x : 12;
    u32 y : 12;
    u32 touch : 1;
    u32 validity : 2;
    u32 pad : 5;
} TPRawBits;

typedef union {
    TPRawBits b;
    u16 h[2];
} TPRawUnion;

typedef struct {
    /* 00 */ u16 initialized;
    /* 04 */ void (*callback)(u32 cmd, u32 arg, u32 arg2);
    /* 08 */ TPData raw;
    /* 10 */ u16 index;
    /* 14 */ TPData *buf;
    /* 18 */ u16 bufSize;
    /* 1c */ struct {
        s32 x0, dx, xs, y0, dy, ys;
    } cal;
    /* 34 */ u16 calibrate;
    /* 36 */ u16 state;
    /* 38 */ u16 err;
    /* 3a */ u16 busy;
} TPWork;

TPWork tpWork;

// Receive handler for the touch panel's FIFO channel.
void func_02042914(int tag, u32 data, BOOL err) {
    u16 cmd = ((u16) data & 0x7f00) >> 8;
    if (err) {
        tpWork.err |= 1 << cmd;
        if (tpWork.callback) {
            tpWork.callback(cmd, 4, 0);
        }
        return;
    }
    if (cmd == TP_CMD_SAMPLE_AUTO) {
        TPRawUnion u;
        TPData *e;
        tpWork.index++;
        if (tpWork.index >= tpWork.bufSize) {
            tpWork.index = 0;
        }
        u.h[0] = TP_SHARED[0];
        u.h[1] = TP_SHARED[1];
        e = &tpWork.buf[tpWork.index];
        e->x = u.b.x;
        e->y = u.b.y;
        e->touch = (u8) u.b.touch;
        e->validity = (u8) u.b.validity;
        if (tpWork.callback) {
            tpWork.callback(cmd, 0, tpWork.index & 0xff);
        }
        return;
    }
    if (!(data & TP_FIFO_FLAG_REPLY)) {
        return;
    }
    {
        u32 res = (u16) data & 0xff;
        u32 r;
        switch (res) {
            case 0:
                switch (cmd) {
                    case 0: {
                        TPRawUnion u;
                        u.h[0] = TP_SHARED[0];
                        u.h[1] = TP_SHARED[1];
                        tpWork.raw.x = u.b.x;
                        tpWork.raw.y = u.b.y;
                        tpWork.raw.touch = (u8) u.b.touch;
                        tpWork.raw.validity = (u8) u.b.validity;
                        tpWork.state = 0;
                        break;
                    }
                    case 1:
                        tpWork.state = 2;
                        break;
                    case 2:
                        tpWork.state = 0;
                        break;
                }
                tpWork.busy &= ~(1 << cmd);
                if (tpWork.callback) {
                    tpWork.callback(cmd, 0, 0);
                }
                return;
            case 4:
                r = 3;
                break;
            case 2:
                r = 1;
                break;
            case 3:
                r = 2;
                break;
            default:
                goto term;
        }
        tpWork.err |= 1 << cmd;
        tpWork.busy &= ~(1 << cmd);
        if (tpWork.callback) {
            tpWork.callback(cmd, r & 0xff, 0);
        }
        return;
    }
term:
    OS_Terminate();
}

void TP_Init(void) {
    if (tpWork.initialized) {
        return;
    }
    tpWork.initialized = 1;
    PXI_Init();
    tpWork.index = 0;
    tpWork.callback = NULL;
    tpWork.buf = NULL;
    tpWork.state = 0;
    tpWork.calibrate = 0;
    tpWork.busy = 0;
    tpWork.err = 0;
    while (!PXI_IsCallbackReady(TP_PXI_TAG, 1)) {
    }
    PXI_SetFifoRecvCallback(TP_PXI_TAG, func_02042914);
}

typedef struct {
    u8 pad[0x58];
    u16 x1, y1;
    u8 px1, py1;
    u16 x2, y2;
    u8 px2, py2;
} TPUserCalibration;

int TP_GetUserInfo(TPCalibrateParam *p) {
    TPUserCalibration *c = (TPUserCalibration *) TP_USER_DATA;
    u16 ax1 = c->x1, ay1 = c->y1;
    u16 ax2 = c->x2, ay2 = c->y2;
    u8 px1 = c->px1, py1 = c->py1, px2 = c->px2, py2 = c->py2;
    if ((ax1 == 0 && ax2 == 0 && ay1 == 0 && ay2 == 0) ||
        TP_CalcCalibrateParam(p, ax1, ay1, px1, py1, ax2, ay2, px2, py2) != 0) {
        p->x0 = 0;
        p->y0 = 0;
        p->xDotSize = 0;
        p->yDotSize = 0;
        return 1;
    }
    return 1;
}

void TP_SetCalibrateParam(const TPCalibrateParam *p) {
    u32 e;
    s32 xd, yd;
    if (p == NULL) {
        tpWork.calibrate = 0;
        return;
    }
    e = OS_DisableInterrupts();
    xd = p->xDotSize;
    if (xd != 0) {
        REG_DIVCNT = 0;
        REG_DIV_NUMER = 0x10000000;
        REG_DIV_DENOM64 = (u32) xd;
        tpWork.cal.x0 = p->x0;
        tpWork.cal.dx = p->xDotSize;
        while (REG_DIVCNT & DIVCNT_BUSY) {
        }
        tpWork.cal.xs = REG_DIV_RESULT;
    } else {
        tpWork.cal.x0 = 0;
        tpWork.cal.dx = 0;
        tpWork.cal.xs = 0;
    }
    yd = p->yDotSize;
    if (yd != 0) {
        REG_DIVCNT = 0;
        REG_DIV_NUMER = 0x10000000;
        REG_DIV_DENOM64 = (u32) yd;
        tpWork.cal.y0 = p->y0;
        tpWork.cal.dy = p->yDotSize;
        while (REG_DIVCNT & DIVCNT_BUSY) {
        }
        tpWork.cal.ys = REG_DIV_RESULT;
    } else {
        tpWork.cal.y0 = 0;
        tpWork.cal.dy = 0;
        tpWork.cal.ys = 0;
    }
    OS_RestoreInterrupts(e);
    tpWork.calibrate = 1;
}

static inline BOOL TpSendFifo(u32 data) {
    return PXI_SendWordByFifo(TP_PXI_TAG, data, 0) >= 0;
}

// Asks the other processor for one touch sample.
void func_02042dc8(void) {
    u32 e = OS_DisableInterrupts();
    if (!TpSendFifo(0x3000000)) {
        OS_RestoreInterrupts(e);
        tpWork.err |= 1;
        if (tpWork.callback) {
            tpWork.callback(0, 4, 0);
        }
        return;
    }
    tpWork.busy |= 1;
    tpWork.err &= ~1;
    OS_RestoreInterrupts(e);
}

// Copies the last sample and converts it to screen coordinates; 1 if the request failed.
int func_02042e5c(TPData *out) {
    if (tpWork.err & 1) {
        return 1;
    }
    *out = tpWork.raw;
    TP_GetCalibratedPoint(out, out);
    return 0;
}

int func_02042ea8(TPData *out) {
    func_020431d4(1);
    return func_02042e5c(out);
}

int TP_CalcCalibrateParam(TPCalibrateParam *p, u16 x1, u16 y1, u16 px1, u16 py1, u16 x2, u16 y2, u16 px2, u16 py2) {
    u32 e;
    s32 r, dy, ty;
    u16 lx2, lpx2, lpy2, lpy1, ly2;
    if (!(x1 < 0x1000 && y1 < 0x1000 && x2 < 0x1000 && y2 < 0x1000)) {
        return 1;
    }
    if (!(px1 < 0x100 && px2 < 0x100 && py1 < 0xc0 && py2 < 0xc0)) {
        return 1;
    }
    if (px1 == px2 || py1 == py2 || x1 == x2 || y1 == y2) {
        return 1;
    }
    e = OS_DisableInterrupts();
    lx2 = x2;
    lpx2 = px2;
    lpy2 = py2;
    lpy1 = py1;
    ly2 = y2;
    REG_DIVCNT = 0;
    REG_DIV_NUMER = (x1 - lx2) << 8;
    REG_DIV_DENOM64 = (u32) (px1 - lpx2);
    ty = y1 - ly2;
    dy = lpy1 - lpy2;
    while (REG_DIVCNT & DIVCNT_BUSY) {
    }
    r = REG_DIV_RESULT;
    REG_DIVCNT = 0;
    REG_DIV_NUMER = ty << 8;
    *(u64 *) 0x04000298 = (u32) dy;
    if (r >= 0x8000 || r < -0x8000) {
        OS_RestoreInterrupts(e);
        return 1;
    }
    p->xDotSize = r;
    // Offsets are narrowed to 16 bits and range-checked.
    {
        s32 v = (s16) ((((x1 + lx2) << 8) - p->xDotSize * (px1 + lpx2)) >> 7);
        if (v >= 0x8000 || v < -0x8000) {
            OS_RestoreInterrupts(e);
            return 1;
        }
        p->x0 = v;
    }
    while (REG_DIVCNT & DIVCNT_BUSY) {
    }
    r = REG_DIV_RESULT;
    OS_RestoreInterrupts(e);
    if (r >= 0x8000 || r < -0x8000) {
        return 1;
    }
    p->yDotSize = r;
    {
        s32 v = (s16) ((((y1 + ly2) << 8) - p->yDotSize * (lpy1 + lpy2)) >> 7);
        if (v >= 0x8000 || v < -0x8000) {
            return 1;
        }
        p->y0 = v;
    }
    return 0;
}

void TP_GetCalibratedPoint(TPData *out, const TPData *raw) {
    s32 *c;
    s32 v;
    if (!tpWork.calibrate) {
        *out = *raw;
        return;
    }
    c = &tpWork.cal.x0;
    out->touch = raw->touch;
    out->validity = raw->validity;
    if (raw->touch == 0) {
        out->x = 0;
        out->y = 0;
        return;
    }
    out->x = ((s64) c[2] * ((s64) (raw->x << 2) - c[0])) >> 22;
    v = (s16) out->x;
    if (v < 0) {
        out->x = 0;
    } else if (v > 0xff) {
        out->x = 0xff;
    }
    out->y = ((s64) c[5] * ((s64) (raw->y << 2) - c[3])) >> 22;
    v = (s16) out->y;
    if (v < 0) {
        out->y = 0;
    } else if (v > 0xbf) {
        out->y = 0xbf;
    }
}

// Waits until none of the commands in mask is pending.
void func_020431d4(u32 mask) {
    while ((*(vu16 *) &tpWork.busy & mask) != 0) {
    }
}
