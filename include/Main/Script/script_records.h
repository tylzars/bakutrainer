#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// One entry of the event queue: the values read from the text (the first two are the event and its argument) and
// how many there are.
typedef struct Unk_EventRec {
    s16 mUnk_00;
    s16 mUnk_02;
    u8 mUnk_04[0x20 - 4];
    u8 mUnk_20;
    u8 mUnk_21;
} Unk_EventRec;

typedef struct Unk_020c61ac_Slot {
    u16 mUnk_00;
    s16 mUnk_02;
    s16 mUnk_04;
    u16 mUnk_06;
    u16 mUnk_08;
    u16 mUnk_0a;
    s16 mUnk_0c;
    s16 mUnk_0e;
    s16 mUnk_10;
    s16 mUnk_12;
    u16 mUnk_14;
} Unk_020c61ac_Slot;

typedef struct Unk_020c61ac_Rec10 {
    u16 mUnk_00;
    s16 mUnk_02;
    u16 mUnk_04;
    u16 mUnk_06;
    u16 mUnk_08;
    s16 mUnk_0a;
    s16 mUnk_0c;
    s16 mUnk_0e;
} Unk_020c61ac_Rec10;

// One entry of the func_020297c8 lists.
typedef struct Unk_020c61ac_Entry {
    u16 mUnk_00;
    s16 mUnk_02;
    s16 mUnk_04;
    s16 mUnk_06;
    s16 mUnk_08;
    s16 mUnk_0a;
    u16 mUnk_0c;
    u16 mUnk_0e;
    u8 mUnk_10;
    u8 mUnk_11;
} Unk_020c61ac_Entry;

// One queued command of the script engine.
typedef struct Unk_020c61ac_Cmd {
    s32 mUnk_00;
    u32 mUnk_04;
    s16 mUnk_08;
    s16 mUnk_0a;
    s16 mUnk_0c;
    s16 mUnk_0e;
    s16 mUnk_10;
    s16 mUnk_12;
    s16 mUnk_14;
    u16 mUnk_16;
    u16 mUnk_18;
    u16 mUnk_1a;
    u8 mUnk_1c;
    u8 mUnk_1d;
    u8 mUnk_1e;
    u8 mUnk_1f;
    u8 mUnk_20;
    union {
        u8 mUnk_21;
        struct {
            u8 mUnk_21_0 : 1;
            u8 mUnk_21_1 : 1;
            u8 mUnk_21_2 : 1;
            u8 mUnk_21_3 : 1;
            u8 mUnk_21_4 : 1;
            u8 mUnk_21_5 : 3;
        };
    };
    u8 mUnk_22;
    u8 mUnk_23;
} Unk_020c61ac_Cmd;

// The part of data_020c61ac from 0x280 on, which the code reaches through one base address: the event queue's write
// and read indexes, the values the conditional commands compare, the callback and the selected indexes.
typedef struct Unk_020c642c {
    u8 mUnk_00[0x10];
    u8 mUnk_10;
    u8 mUnk_11;
    u8 mUnk_12[2];
    u8 mUnk_14[2];
    u16 mUnk_16[2];
    u8 mUnk_1a[2];
    s32 mUnk_1c;
    u16 mUnk_20;
    u16 mUnk_22;
    s16 mUnk_24;
    s16 mUnk_26;
    u8 (*mUnk_28)(void);
    u16 mUnk_2c;
    u16 mUnk_2e;
    u16 mUnk_30;
    u8 mUnk_32[2];
    s32 mUnk_34;
    s32 mUnk_38;
    u16 mUnk_3c;
    u8 mUnk_3e[2];
    s32 mUnk_40;
} Unk_020c642c;

typedef struct Unk_020c61ac {
    void (*mUnk_00)(u32 id, int x, int y, int w, int mode);
    u32 (*mUnk_04)(u32);
    u16 mUnk_08;
    u16 mUnk_0a;
    u16 mUnk_0c;
    u16 mUnk_0e;
    u16 mUnk_10;
    u16 mUnk_12;
    u16 mUnk_14;
    u16 mUnk_16;
    u16 mUnk_18;
    u8 mUnk_1a;
    u8 mUnk_1b;
    union {
        u8 mUnk_1c;
        struct {
            u8 mUnk_1c_0 : 1;
            u8 mUnk_1c_1 : 1;
            u8 mUnk_1c_2 : 1;
            u8 mUnk_1c_3 : 1;
            u8 mUnk_1c_4 : 4;
        };
    };
    u8 mUnk_1d;
    u16 mUnk_1e;
    u16 mUnk_20;
    u16 mUnk_22;
    u16 mUnk_24;
    u16 mUnk_26;
    u16 mUnk_28;
    s16 mUnk_2a;
    u8 *mUnk_2c;
    u8 mUnk_30;
    u8 mUnk_31;
    s16 mUnk_32;
    u8 mUnk_34;
    u8 mUnk_35;
    u16 mUnk_36[8];
    u8 mUnk_46[8];
    u8 mUnk_4e[8];
    u8 mUnk_56;
    u8 mUnk_57;
    Unk_020c61ac_Rec10 *mUnk_58;
    Unk_020c61ac_Slot *mUnk_5c;
    Unk_020c61ac_Cmd *mUnk_60;
    Unk_020c61ac_Entry *mUnk_64;
    Unk_020c61ac_Entry *mUnk_68;
    void *mUnk_6c;
    // The event queue's indexes wrap at 16, so its last entry ends at 0x290 and covers the first 0x10 bytes of
    // mUnk_280, which nothing else uses.
    union {
        Unk_EventRec mUnk_70[16];
        struct {
            u8 mUnk_70_pad[0x280 - 0x70];
            Unk_020c642c mUnk_280;
        };
    };
} Unk_020c61ac;

extern Unk_020c61ac data_020c61ac;

void func_02028e5c(const char *name);
void func_02028eb4(int index);
void func_02028f0c(int a, int b, int c, int d, int e, int f, void (*g)(u32 id, int x, int y, int w, int mode),
                   u8 (*h)(void), u32 (*i)(u32));
void func_02028fe8(void);
u8 func_0202903c(u8 c);
int func_02029050(u8 c);
int func_0202906c(u8 c);
int func_02029084(u8 c);
int func_02029094(u8 c);
int func_020290ac(const char *a, const char *b);
void func_020290cc(u8 **src, u8 **dst);
void func_02029118(s16 value, u8 **out);
s16 func_02029128(u8 **text);
const u8 *func_020291a4(const u8 *name);
void func_0202920c(const u8 *p, u16 *values, u32 *count);
void func_02029250(const s16 *src, s16 *dst, int count, int op);
int func_02029298(u8 **text);
void func_020292f4(const char *text);
char *func_0202938c(char *dst, const char *src);
int func_020293c4(s16 *out, u8 *text, u32 size);
int func_020293dc(s16 *out, u8 *text);
void func_020294c4(u8 *text, int mode);
void func_020297c8(u8 *text);
void func_02029898(u8 *text);
void func_020298c8(void);
void func_02029970(u8 *text);
void func_020299d4(u8 *text);
void func_02029a1c(const char *name);
void func_02029a74(u8 *text);
void func_02029aac(u8 *text, int flag);
void func_02029b28(u8 *text);
void func_02029b6c(u8 *text);
void func_02029b98(u8 *text);
void func_02029bd8(u8 *text);
void func_02029c18(u8 *text);
void func_02029c58(u8 *text);
void func_02029c98(u8 *text);
void func_02029d00(u8 *text);
void func_02029d8c(const char *name);
void func_02029da0(u8 *text);
void func_02029dd8(u8 *text);
void func_02029ed8(int id, int *a, int *b, int *c);
void func_02029f6c(u8 *text);
void func_0202a034(u8 *text);
void func_0202a078(int a, int b);
void func_0202a0dc(int index, int id);
void func_0202a10c(Unk_020c61ac_Cmd *rec);
void func_0202a138(void);
void func_0202a160(void);
u8 func_0202a188(void);
void func_0202a194(void);
Unk_EventRec *func_0202a1a4(void);
void func_0202a1cc(void);
void func_0202a1ec(int (*callback)(void));

#ifdef __cplusplus
}
#endif
