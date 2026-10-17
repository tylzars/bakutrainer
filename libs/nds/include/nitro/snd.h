#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Command record shared with the sound processor (the pool holds 256).
typedef struct SNDCommand SNDCommand;
struct SNDCommand {
    /* 00 */ SNDCommand *next;
    /* 04 */ u32 id;
    /* 08 */ u32 arg[4];
}; /* 18 */

typedef struct {
    /* 00 */ s16 mUnk_00[16];
    /* 20 */ u32 mUnk_20;
} SNDSharedPlayer; /* 24 */

// Work area the sound processor shares with this processor; 0x280 bytes.
typedef struct {
    /* 000 */ u32 mUnk_000;
    /* 004 */ u32 mUnk_004;
    /* 008 */ u16 mUnk_008;
    /* 00a */ u16 mUnk_00a;
    /* 00c */ u8 mUnk_00c[0x14];
    /* 020 */ SNDSharedPlayer mUnk_020[16];
    /* 260 */ s16 mUnk_260[16];
} SNDSharedWork; /* 280 */

extern SNDSharedWork *SNDi_SharedWork;

// Command wrappers.
void func_020400cc(u32 a, u32 b, u32 c, u32 d);
void func_020400f4(u32 a, u32 b, u32 c, u32 d);
void func_02040158(u32 a, u32 b, u32 c, void (*callback)(void *), void *arg);
void func_02040198(u32 a, u32 b);
void func_020401b8(u32 a, u32 b);
void func_020401d8(u32 a, u32 b);
void func_020401f8(u32 a, u32 b, u32 c);
void func_02040220(u32 a, u32 b);
void func_02040240(u32 p0, u32 p1, u32 p2, u32 p3, u32 p4, u32 p5, u32 p6, u32 p7, u32 p8, u32 p9);
void func_0204028c(u32 a);
void func_020402ac(u32 id, u32 a, u32 b, u32 c, u32 d);

// Initialisation and command queue.
void func_020402f0(void);
void SND_CommandInit(void);
SNDCommand *SND_RecvCommandReply(u32 flags);
SNDCommand *SND_AllocCommand(u32 flags);
void SND_PushCommand(SNDCommand *cmd);
BOOL SND_FlushCommand(u32 flags);
u32 SND_GetCurrentCommandTag(void);
BOOL SND_IsFinishedCommandTag(u32 tag);
u32 SND_CountFreeCommand(void);
u32 SND_CountWaitingCommand(void);
u32 func_0204087c(void);
void func_02040898(int tag, u32 data, BOOL err);
void func_020408bc(void);
void func_0204091c(void);
SNDCommand *func_02040944(void);
BOOL func_0204098c(void);

// Alarm table.
void func_020409cc(void);
void func_020409fc(int index);
u32 func_02040a1c(int index, void (*callback)(void *), void *arg);
void func_02040a4c(u32 data);

// Shared work area and volume conversion.
u16 func_02040a90(void);
u32 func_02040abc(void);
void func_02040ae4(SNDSharedWork *work);
u16 func_02040b60(int volume);

#ifdef __cplusplus
}
#endif
