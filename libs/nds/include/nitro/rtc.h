#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    /* 00 */ s32 year;
    /* 04 */ s32 month;
    /* 08 */ s32 day;
    /* 0c */ s32 week;
} RTCDate;

typedef struct {
    /* 00 */ s32 hour;
    /* 04 */ s32 minute;
    /* 08 */ s32 second;
} RTCTime;

typedef void (*RTCCallback)(u32 result, void *arg);

void RTC_Init(void);
void func_0204456c(int tag, u32 data, BOOL err);
u32 func_02044abc(u32 bcd);
BOOL func_02044b2c(void);
BOOL func_02044b3c(u32 cmd);
u32 func_02044b64(RTCDate *date);

#ifdef __cplusplus
}
#endif
