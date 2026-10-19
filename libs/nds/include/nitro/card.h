#pragma once

#include "types.h"
#include <nitro/os.h>

#ifdef __cplusplus
extern "C" {
#endif

// Command block shared with the other processor.
typedef struct {
    /* 00 */ u32 result;
    /* 04 */ u32 id;
    /* 08 */ u32 mUnk_08;
    /* 0c */ u32 mUnk_0c;
    /* 10 */ u32 mUnk_10;
    /* 14 */ u32 mUnk_14;
    /* 18 */ u32 mUnk_18;
    /* 1c */ u32 mUnk_1c;
    /* 20 */ u32 mUnk_20;
    /* 24 */ u32 mUnk_24;
    /* 28 */ u32 mUnk_28;
    /* 2c */ u32 mUnk_2c;
    /* 30 */ u32 mUnk_30;
    /* 34 */ u32 mUnk_34;
    /* 38 */ u32 mUnk_38;
    /* 3c */ u32 mUnk_3c;
    /* 40 */ u32 mUnk_40;
    /* 44 */ u32 mUnk_44;
    /* 48 */ u32 mUnk_48;
    /* 4c */ u32 mUnk_4c;
    /* 50 */ u32 mUnk_50;
    /* 54 */ u8 mUnk_54;
    /* 55 */ u8 mUnk_55[3];
    /* 58 */ u32 mUnk_58;
    /* 5c */ u32 mUnk_5c;
} CardArg; /* 60 */

// State flags of a request.
#define CF_READY 0x1
#define CF_PXI 0x2
#define CF_BUSY 0x4
#define CF_TASK 0x8
#define CF_TASKWAIT 0x10
#define CF_PXIWAIT 0x20
#define CF_CANCEL 0x40

// FIFO tag of the card commands, and of the card-removed message; the message is the low six bits of the word.
#define CARD_PXI_TAG 0xb
#define CARD_PULLOUT_PXI_TAG 0xe
#define CARD_PULLOUT_MESSAGE 0x11

// Interrupt mask of the card transfer.
#define CARD_IRQ_MASK 0x80000

// Lock owner value while nobody holds the card.
#define CARD_OWNER_NONE (-3)

typedef struct CardCommon {
    /* 000 */ CardArg *arg;
    /* 004 */ u32 mUnk_04;
    /* 008 */ vu32 owner;
    /* 00c */ vu32 lockCount;
    /* 010 */ OSThreadQueue lockQueue;
    /* 018 */ u32 lockTarget;
    /* 01c */ u32 src;
    /* 020 */ u32 dst;
    /* 024 */ u32 len;
    /* 028 */ u32 dma;
    /* 02c */ u32 cmd;
    /* 030 */ u32 retry;
    /* 034 */ u32 mode;
    /* 038 */ void (*callback)(void *);
    /* 03c */ void *cbArg;
    /* 040 */ void (*task)(struct CardCommon *);
    /* 044 */ OSThread thread;
    /* 104 */ OSThread *curThread;
    /* 108 */ u32 priority;
    /* 10c */ OSThreadQueue queue;
    /* 114 */ vu32 flags;
    /* 118 */ u32 mUnk_118;
    /* 11c */ u32 mUnk_11c;
    /* 120 */ u8 buf[0x100];
} CardCommon; /* 220 */

// Request for the page-wise ROM read.
typedef struct CardRomReq {
    /* 00 */ void (*func)(struct CardRomReq *);
    /* 04 */ u32 ctrl;
    /* 08 */ u32 page;
    /* 0c */ u8 mUnk_0c[0x14];
    /* 20 */ u8 cache[0x200];
} CardRomReq; /* 220 */

// Header copy in RAM; only the ROM control word is used.
typedef struct {
    /* 00 */ u8 mUnk_00[0x60];
    /* 60 */ u32 romctrl;
} CardHeader;

typedef struct {
    /* 0 */ BOOL pulledOut;
    /* 4 */ BOOL (*callback)(void);
} CardPullout;

// card_common.c
// Bytes of the CardCommon state (read and written as a CardCommon).
extern u8 data_020cc880[];

void func_02044c20(void (*task)(CardCommon *));
void func_02044c5c(u32 id, u32 type);
void func_02044ce0(u32 id, u32 type);
void func_02044d6c(void);
u32 func_02044e7c(void);
void func_02044e8c(void);
void func_02044ea4(u32 v);
int func_02044eb4(void);
int func_02044f00(void);
void func_02044f1c(u16 id);
void func_02044f38(u16 id);
void func_02044f54(u32 id);
void func_02044f64(u32 id);
void func_02044f8c(s32 id);
void func_02045310(CardCommon *c);
int func_02045500(u32 src, void *dst, u32 len, void (*cb)(void *), void *cbarg, int async, u32 cmd, u32 retry,
                  u32 mode);
u32 func_020455e8(void);
int func_020455fc(u32 id);
int func_02045734(void);
int func_02045740(void);

// card_rom.c
int func_0204574c(CardRomReq *r);
void func_020457d8(u32 a, u32 b);
void func_02045838(void);
void func_0204588c(void);
int func_0204595c(CardRomReq *r);
void func_02045ae8(CardRomReq *r);
u32 func_02045bd8(void);
void func_02045c34(CardCommon *c);
void func_02045ccc(u32 dma, u32 src, void *dst, u32 len, void (*cb)(void *), void *cbarg, int async);
void func_02045dc4(void);
int func_02045e28(void);
void (*func_02045e34(void))(CardRomReq *);

// card_request.c
void func_02045e40(int tag, u32 data, BOOL err);
void func_02045e74(void *arg);
int func_02045ec4(CardCommon *c, u32 cmd, int tries);

// card_pullout.c
void func_02046018(void);
void func_02046044(int tag, u32 data, BOOL err);
BOOL func_02046098(void);
void func_020460a8(void);
void func_02046134(u32 id);
void func_02046194(u32 a, u32 b);

#ifdef __cplusplus
}
#endif
