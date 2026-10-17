#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Callback list node: func(arg) runs around sleep.
typedef struct PMCallbackInfo PMCallbackInfo;
struct PMCallbackInfo {
    /* 00 */ void (*func)(void *arg);
    /* 04 */ void *arg;
    /* 08 */ PMCallbackInfo *next;
};

BOOL func_020436e0(void);
void func_02043700(void);
void func_02043740(void);
void func_02043790(void);
void func_020437ec(u32 value, u32 *dest);
void func_020437f8(u32 result);
void func_02043828(void);
void func_020438c0(int tag, u32 data, BOOL err);
void func_02043954(u32 *words, int count);
int func_02043a14(u32 a, u32 b);
int func_02043aa0(u32 cmd, u32 data, u16 *res, void (*cb)(u32, u32 *), u32 *arg);
int func_02043b24(u32 cmd, u32 data, u16 *res);
int func_02043b68(u32 a, void (*cb)(u32, u32 *), u32 *arg);
int func_02043bcc(u32 a);
int func_02043c00(u32 a, u32 b, void (*cb)(u32, u32 *), u32 *arg);
int func_02043c88(u32 a, u32 b);
int func_02043cbc(void (*cb)(u32, u32 *), u32 *arg);
int func_02043d10(void);
int func_02043d5c(void);
int func_02043d98(u32 value);
int func_02043db0(u32 value);
int func_02043de0(u32 value);
int func_02043dfc(u32 *a, u32 *b);
void func_02043e5c(u32 word);
void func_02043e88(u32 flags, u32 a, u32 b);
int func_02044194(int a, int b, int c, int d);
int func_020443ac(int a);
BOOL func_020443cc(void);
void func_020443e8(PMCallbackInfo **head, PMCallbackInfo *node);
void func_02044434(PMCallbackInfo *list);
void func_0204445c(PMCallbackInfo *node);
void func_02044474(void);

#ifdef __cplusplus
}
#endif
