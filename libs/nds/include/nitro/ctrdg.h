#pragma once

#include "types.h"
#include <nitro/os.h>

#ifdef __cplusplus
extern "C" {
#endif

// ROM access timing fields saved from and restored to the memory control register.
typedef struct CTRDGTiming {
    /* 00 */ s32 rom1st;
    /* 04 */ s32 rom2nd;
} CTRDGTiming;

// Cartridge slot lock: whether the slot was already owned, and the interrupt state to restore.
typedef struct CTRDGLock {
    /* 00 */ s32 locked;
    /* 04 */ u32 irq;
} CTRDGLock;

// A request handed to the cartridge thread.
typedef struct CTRDGTask {
    /* 00 */ void *func;
    /* 04 */ void (*cb)(struct CTRDGTask *);
    /* 08 */ u32 result;
    /* 0c */ u32 mUnk_0c[6];
} CTRDGTask; /* 24 */

typedef struct CTRDGWork {
    /* 00 */ u32 mUnk_00;
    /* 04 */ u32 mUnk_04;
    /* 08 */ u32 init;
    /* 0c */ u32 mUnk_0c;
    /* 10 */ u32 mUnk_10;
    /* 14 */ u32 mUnk_14;
    /* 18 */ u32 (*cb)(void);
} CTRDGWork;

typedef struct CTRDGState {
    /* 00 */ u16 flag;
    /* 02 */ u16 id;
} CTRDGState;

// ctrdg_init.c
extern CTRDGState data_020cdbe8;

// Cartridge information block in main memory, filled from the cartridge header.
typedef struct {
    /* 00 */ u16 mUnk_00;
    /* 02 */ u8 mUnk_02[3];
    /* 05 */ struct {
        u8 f0 : 1;
        u8 f1 : 1;
    } b;
    /* 06 */ u16 mUnk_06;
    /* 08 */ u32 mUnk_08;
} CartInfo;

typedef struct CTRDGGlobal {
    /* 00 */ void *work;
    /* 04 */ u8 mUnk_04[0x22];
    /* 26 */ u8 mUnk_26;
} CTRDGGlobal;

void func_02047f3c(void);
int func_02047f74(void);
int func_02047f9c(void);
int func_02047fb4(void);
void func_020480c0(CTRDGTiming *timing);
void func_02048108(CTRDGTiming *timing);
void func_0204813c(u16 id, CTRDGLock *lock);
void func_02048198(u16 id, CTRDGLock *lock);
void func_020481bc(u32 data);
void func_0204820c(u32 value);
void func_02048254(void);
void func_02048308(void);
void func_020484fc(int tag, u32 data, BOOL err);
void func_02048528(int tag, u32 data, BOOL err);
void func_02048584(void);
void func_02048598(int tag, u32 data, BOOL err);
void func_020485ac(u32 *work);
void func_02048638(void *p);
void func_0204864c(u32 *work);

#ifdef __cplusplus
}
#endif
