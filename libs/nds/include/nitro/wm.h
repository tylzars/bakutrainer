#pragma once

#include "types.h"
#include <nitro/os.h>

#ifdef __cplusplus
extern "C" {
#endif

// Callback of a command or an indication; called with a pointer to the message block.
typedef void (*WMCallback)();

typedef struct {
    /* 000 */ u16 mUnk_000;
    /* 002 */ u8 mUnk_002[0xa];
    /* 00c */ u32 mUnk_00c;
    /* 010 */ u8 mUnk_010[0x2c];
    /* 03c */ u16 mUnk_03c;
    /* 03e */ u16 mUnk_03e;
    /* 040 */ u8 mUnk_040[0x32];
    /* 072 */ u16 mUnk_072;
    /* 074 */ u8 mUnk_074[8];
    /* 07c */ u32 mUnk_07c;
    /* 080 */ u8 mUnk_080[6];
    /* 086 */ u16 mUnk_086;
    /* 088 */ u8 mUnk_088[0x14];
    /* 09c */ u16 mUnk_09c;
    /* 09e */ u8 mUnk_09e[0x1e];
    /* 0bc */ u16 mUnk_0bc;
    /* 0be */ u8 mUnk_0be[8];
    /* 0c6 */ u16 mUnk_0c6;
    /* 0c8 */ u8 mUnk_0c8[0x30];
    /* 0f8 */ u16 mUnk_0f8;
    /* 0fa */ u8 mUnk_0fa[0x88];
    /* 182 */ u16 mUnk_182;
    /* 184 */ u8 mUnk_184[4];
    /* 188 */ u16 mUnk_188;
} WMStatus;

typedef struct {
    /* 000 */ u32 mUnk_000;
    /* 004 */ WMStatus *st;
    /* 008 */ u8 mUnk_008[8];
    /* 010 */ void *mUnk_010;
    /* 014 */ u16 mUnk_014;
    /* 016 */ u16 mUnk_016;
    /* 018 */ WMCallback cbTable[0x2c];
    /* 0c8 */ WMCallback mUnk_0c8;
    /* 0cc */ WMCallback indCb[16];
    /* 10c */ u32 indArg[16];
    /* 14c */ u32 bitmap;
    /* 150 */ u16 mUnk_150;
} WMWork;

typedef struct {
    /* 00 */ u16 init;
    /* 04 */ WMWork *work;
} WMState;

// Buffer of 0x820 bytes handed to the data-sharing calls: four 0x200-byte sets, then the session state.
typedef struct {
    /* 00 */ u16 mUnk_00;
    /* 02 */ u16 mUnk_02;
    /* 04 */ u8 mUnk_04[0x1fc];
} WMDataSet;

typedef struct {
    /* 000 */ WMDataSet mUnk_000[4];
    /* 800 */ u16 mUnk_800[4];
    /* 808 */ u16 mUnk_808;
    /* 80a */ u16 mUnk_80a;
    /* 80c */ u16 mUnk_80c;
    /* 80e */ u16 mUnk_80e;
    /* 810 */ u16 mUnk_810;
    /* 812 */ u16 mUnk_812;
    /* 814 */ u16 mUnk_814;
    /* 816 */ u16 mUnk_816;
    /* 818 */ u16 mUnk_818;
    /* 81a */ u16 mUnk_81a;
    /* 81c */ u16 mUnk_81c;
    /* 81e */ u16 mUnk_81e;
} WMDataSharing;

// Indication block (0x44 bytes) handed to the indication callbacks.
typedef struct {
    /* 00 */ u16 id;
    /* 02 */ u16 mUnk_02;
    /* 04 */ u16 mUnk_04;
    /* 06 */ u16 mUnk_06;
    /* 08 */ u32 mUnk_08;
    /* 0c */ u32 mUnk_0c;
    /* 10 */ u16 mUnk_10;
    /* 12 */ u16 mUnk_12;
    /* 14 */ u8 mac[6];
    /* 1a */ u16 mUnk_1a;
    /* 1c */ u32 mUnk_1c;
    /* 20 */ u16 mUnk_20;
    /* 22 */ u16 mUnk_22;
    /* 24 */ u8 mUnk_24[0x20];
} WMInd;

// Parameter block of command 7 (0x40 bytes, written back to memory before the command is sent).
typedef struct {
    /* 00 */ void *mUnk_00;
    /* 04 */ u16 mUnk_04;
    /* 06 */ u8 mUnk_06[6];
    /* 0c */ u16 mUnk_0c;
    /* 0e */ u16 mUnk_0e;
    /* 10 */ u16 mUnk_10;
    /* 12 */ u16 mUnk_12;
    /* 14 */ u16 mUnk_14;
    /* 16 */ u16 mUnk_16;
    /* 18 */ u16 mUnk_18;
    /* 1a */ u8 mUnk_1a[0x18];
    /* 32 */ u16 mUnk_32;
    /* 34 */ u16 mUnk_34;
    /* 36 */ u16 mUnk_36;
} WMParentParam;

// Job queued for the worker thread; the queue is sorted by priority (lowest value first).
typedef struct WMTask WMTask;
struct WMTask {
    /* 00 */ WMTask *next;
    /* 04 */ u32 busy : 1;
    /* 04 */ u32 prio : 31;
    /* 08 */ u32 mUnk_08;
    /* 0c */ void *mUnk_0c;
};

// The worker thread followed by its job queue and the job that ends it.
typedef struct {
    /* 00 */ OSThread thread;
    /* c0 */ WMTask *volatile queue;
    /* c4 */ WMTask exitTask;
} WMTaskThread;

// Command state: initialisation flag and work area, the queue of free message slots.
extern WMState data_020cd100;
extern OSMessageQueue data_020cd108;

// Command transport (wm_command.c); the first stores the callback of command idx in the work area.
void func_020461e4(u32 idx, WMCallback callback);
void *func_020461fc(void);
int func_02046254(u32 cmd, u16 argc, ...);
int func_02046300(void *src, u32 len);
WMWork *func_02046370(void);
int func_02046380(void);
int func_0204639c(void);
int func_020463e4(int n, ...);

// Calls (wm_api.c)
u16 func_02046478(void);
u16 func_020464a8(void);
int func_020464d8(u32 idx, WMCallback cb, u32 arg);
int func_020465b0(void *dest);
int func_020465fc(void);
int func_02046668(void);
u32 func_0204672c(void);
u16 func_020467bc(void);
int func_0204684c(WMCallback cb);
int func_02046884(WMCallback cb);
int func_020468c4(WMCallback cb, WMParentParam *p);
int func_020469a0(WMParentParam *p);
int func_020469f0(WMCallback cb, int x);
int func_02046a4c(WMCallback cb);
int func_02046a5c(WMCallback cb);
int func_02046a9c(WMCallback cb, u16 *p, void *q, u32 arg, u16 extra);
int func_02046b68(WMCallback cb, u32 idx);
int func_02046c50(WMCallback cb, u32 a, int b, u32 c, u16 e, void *f);
int func_02046dc0(WMCallback cb, u32 a, u32 b, u32 c, u16 e, u16 f);
int func_02046e28(WMCallback cb, void *a, void *buf, u16 size, u16 x, u16 y, u16 z);
int func_02046f58(WMCallback cb);
int func_02046fc8(WMDataSharing *buf, u16 n, u16 mask, u16 m, int flag);
void func_020475ac(u16 *msg);
void func_02047684(WMInd *ind);
void func_020477b0(WMInd *ind);
int func_0204721c(WMDataSharing *buf);
int func_02047264(WMDataSharing *buf, void *pa, void *pb);
void func_020478a4(WMDataSharing *buf, u32 idx, void *src);
void func_02047970(WMDataSharing *buf, int flag);
u8 *func_02047ac4(WMDataSharing *buf, u16 *p, u32 n);
u8 *func_02047b18(WMDataSharing *buf, u32 mask, u8 *base, u32 n);
int func_02047b48(WMDataSharing *buf, u16 n);
int func_02047b64(WMDataSharing *buf);
int func_02047b70(WMCallback cb, u32 n, void *p);

// Worker thread (wm_worker.c)
void func_02047be4(void *arg);
int func_02047c28(void);
int func_02047c60(void);
int func_02047c6c(void);
void func_02047d00(void);
void func_02047d3c(u32 code, u32 status);

// Worker task queue (wm_task.c)
BOOL func_02047d74(void);
void func_02047d90(WMTask *task, u32 a, void *b, u32 prio);
void func_02047ef4(void *callback);

#ifdef __cplusplus
}
#endif
