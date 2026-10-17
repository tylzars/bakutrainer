#include <nitro/os.h>
#include <nitro/pxi.h>
#include <nitro/rtc.h>

#define RTC_PXI_TAG 5

typedef struct {
    /* 00 */ u16 init;
    /* 04 */ u32 mUnk_04;
    /* 08 */ u32 mUnk_08;
    /* 0c */ u32 busy;
    /* 10 */ RTCCallback cb;
    /* 14 */ void *p1;
    /* 18 */ void *p2;
    /* 1c */ void *arg;
    /* 20 */ u32 command;
    /* 24 */ u32 index;
    /* 28 */ void (*alarmCb)(void);
} RTCWork;

extern RTCWork data_020cc7b8;

typedef struct {
    u32 year : 8;
    u32 month : 5;
    u32 : 3;
    u32 day : 6;
    u32 : 10;
} RTCRawDate;
typedef struct {
    u32 hour : 6;
    u32 : 2;
    u32 minute : 7;
    u32 : 1;
    u32 second : 7;
    u32 : 9;
} RTCRawTime;
typedef struct {
    u16 mode : 4;
    u16 : 2;
    u16 flag : 1;
    u16 : 9;
} RTCRawStat;
typedef struct {
    u32 week : 3;
    u32 : 4;
    u32 b7 : 1;
    u32 hour : 6;
    u32 : 1;
    u32 b15 : 1;
    u32 minute : 7;
    u32 b23 : 1;
    u32 : 8;
} RTCRawAlarm;

#define RAW_DATE ((RTCRawDate *) 0x027ffde8)
#define RAW_TIME ((RTCRawTime *) 0x027ffdec)
#define RAW_STAT ((RTCRawStat *) 0x027ffdea)
#define RAW_ALARM ((RTCRawAlarm *) 0x027ffdec)

void RTC_Init(void) {
    if (data_020cc7b8.init != 0) {
        return;
    }
    data_020cc7b8.init = 1;
    data_020cc7b8.busy = 0;
    data_020cc7b8.cb = 0;
    data_020cc7b8.alarmCb = 0;
    data_020cc7b8.p1 = 0;
    data_020cc7b8.p2 = 0;
    PXI_Init();
    while (!PXI_IsCallbackReady(RTC_PXI_TAG, 1)) {
    }
    PXI_SetFifoRecvCallback(RTC_PXI_TAG, func_0204456c);
}

// Receive handler for the clock FIFO channel: decodes the shared-memory reply for the current command and calls the
// pending callback.
void func_0204456c(int tag, u32 data, BOOL err) {
    u32 res;
    u8 cmd;
    if (err) {
        if (data_020cc7b8.index) {
            data_020cc7b8.index = 0;
        }
        if (data_020cc7b8.busy) {
            data_020cc7b8.busy = 0;
        }
        if (data_020cc7b8.cb) {
            RTCCallback cb = data_020cc7b8.cb;
            data_020cc7b8.cb = 0;
            cb(6, data_020cc7b8.arg);
        }
        return;
    }
    cmd = (data & 0x7f00) >> 8;
    err = data & 0xff;
    if (cmd == 0x30) {
        if (data_020cc7b8.alarmCb) {
            data_020cc7b8.alarmCb();
        }
        return;
    }
    if (err == 0) {
        res = 0;
        switch (data_020cc7b8.command) {
            case 0: {
                RTCDate *d = (RTCDate *) data_020cc7b8.p1;
                d->year = func_02044abc(RAW_DATE->year);
                d->month = func_02044abc(RAW_DATE->month);
                d->day = func_02044abc(RAW_DATE->day);
                d->week = func_02044b64(d);
                break;
            }
            case 1: {
                RTCTime *t = (RTCTime *) data_020cc7b8.p1;
                t->hour = func_02044abc(RAW_TIME->hour);
                t->minute = func_02044abc(RAW_TIME->minute);
                t->second = func_02044abc(RAW_TIME->second);
                break;
            }
            case 2: {
                RTCDate *d = (RTCDate *) data_020cc7b8.p1;
                RTCTime *t;
                d->year = func_02044abc((u8) * (vu32 *) 0x027ffde8);
                d->month = func_02044abc(RAW_DATE->month);
                d->day = func_02044abc(RAW_DATE->day);
                d->week = func_02044b64(d);
                t = (RTCTime *) data_020cc7b8.p2;
                t->hour = func_02044abc(RAW_TIME->hour);
                t->minute = func_02044abc(RAW_TIME->minute);
                t->second = func_02044abc(RAW_TIME->second);
                break;
            }
            case 6: {
                s32 *p = (s32 *) data_020cc7b8.p1;
                switch (RAW_STAT->mode) {
                    case 4:
                        *p = 1;
                        break;
                    default:
                        *p = res;
                        break;
                }
                break;
            }
            case 7: {
                s32 *p = (s32 *) data_020cc7b8.p1;
                if (RAW_STAT->flag) {
                    *p = 1;
                } else {
                    *p = res;
                }
                break;
            }
            case 8: {
                s32 *p = (s32 *) data_020cc7b8.p1;
                p[0] = RAW_ALARM->week;
                p[1] = func_02044abc(RAW_ALARM->hour);
                p[2] = func_02044abc(RAW_ALARM->minute);
                p[3] = res;
                if (RAW_ALARM->b7) {
                    p[3] += 1;
                }
                if (RAW_ALARM->b15) {
                    p[3] += 2;
                }
                if (RAW_ALARM->b23) {
                    p[3] += 4;
                }
                break;
            }
            case 9: {
                u32 i = data_020cc7b8.index;
                if (i == 0) {
                    if (*(s32 *) data_020cc7b8.p1 == 1) {
                        if (RAW_STAT->mode == 4) {
                            break;
                        }
                        data_020cc7b8.index = i + 1;
                        RAW_STAT->mode = 4;
                        if (!func_02044b2c()) {
                            data_020cc7b8.index = 0;
                            res = 3;
                        }
                    } else {
                        if (RAW_STAT->mode == 0) {
                            break;
                        }
                        data_020cc7b8.index = i + 1;
                        RAW_STAT->mode = 0;
                        if (!func_02044b2c()) {
                            data_020cc7b8.index = 0;
                            res = 3;
                        }
                    }
                } else {
                    data_020cc7b8.index = 0;
                }
                break;
            }
            case 10: {
                u32 i = data_020cc7b8.index;
                if (i == 0) {
                    if (*(s32 *) data_020cc7b8.p1 == 1) {
                        if (RAW_STAT->flag) {
                            break;
                        }
                        data_020cc7b8.index = i + 1;
                        RAW_STAT->flag = 1;
                        if (!func_02044b2c()) {
                            data_020cc7b8.index = 0;
                            res = 3;
                        }
                    } else {
                        if (!RAW_STAT->flag) {
                            break;
                        }
                        data_020cc7b8.index = i + 1;
                        RAW_STAT->flag = 0;
                        if (!func_02044b2c()) {
                            data_020cc7b8.index = 0;
                            res = 3;
                        }
                    }
                } else {
                    data_020cc7b8.index = 0;
                }
                break;
            }
            case 3:
            case 4:
            case 5:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
                break;
            default:
                data_020cc7b8.index = 0;
                res = 4;
                break;
        }
    } else {
        data_020cc7b8.index = 0;
        switch (err) {
            case 1:
                res = 4;
                break;
            case 2:
                res = 5;
                break;
            case 3:
                res = 1;
                break;
            case 0:
            case 4:
            default:
                res = 6;
                break;
        }
    }
    if (data_020cc7b8.index == 0) {
        if (data_020cc7b8.busy) {
            data_020cc7b8.busy = 0;
        }
        if (data_020cc7b8.cb) {
            RTCCallback cb = data_020cc7b8.cb;
            data_020cc7b8.cb = 0;
            cb(res, data_020cc7b8.arg);
        }
    }
}

// Converts a packed BCD value to binary; 0 if any digit is above 9.
u32 func_02044abc(u32 bcd) {
    int i;
    u32 r = 0;
    u32 m;
    for (i = 0; i < 8; i++) {
        if (((bcd >> (i * 4)) & 0xf) >= 10) {
            return 0;
        }
    }
    m = 1;
    for (i = 0; i < 8; i++) {
        r += m * ((bcd >> (i * 4)) & 0xf);
        m *= 10;
    }
    return r;
}

// Sends the command that writes back the status register bits changed in shared memory.
BOOL func_02044b2c(void) {
    return func_02044b3c(0x27);
}

// Sends one command word; returns whether the FIFO accepted it.
BOOL func_02044b3c(u32 cmd) {
    return PXI_SendWordByFifo(RTC_PXI_TAG, (cmd << 8) & 0x7f00, 0) >= 0;
}

// Day of the week (0-6) for a date, computed from year, month and day.
u32 func_02044b64(RTCDate *date) {
    s32 m;
    s32 y;
    s32 c;
    s32 yy;
    s32 t;
    s32 d;
    d = date->day;
    y = date->year + 2000;
    m = date->month - 2;
    if (m < 1) {
        y -= 1;
        m += 12;
    }
    c = y / 100;
    yy = y % 100;
    t = yy + (d + (m * 26 - 2) / 10) + yy / 4 + c / 4 + c * 5;
    return t % 7;
}
