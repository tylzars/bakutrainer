#include <nitro/os.h>
#include <nitro/mi.h>
#include <nitro/svc.h>

#define OS_LOCKWORD_BUF ((OSLockWord *) 0x27ffff0)
#define OS_CARTRIDGE_LOCK ((OSLockWord *) 0x27fffe8)
#define OS_CARD_LOCK ((OSLockWord *) 0x27fffe0)

static void func_0203d7b8(void);
static void func_0203d7d0(void);
static void func_0203d820(void);
static void func_0203d838(void);

void OS_InitLock(void) {
    static BOOL lockInitialized;
    OSLockWord *lockp = OS_LOCKWORD_BUF;
    if (lockInitialized) {
        return;
    }
    lockInitialized = TRUE;
    lockp->lockFlag = 0;
    OS_LockByWord(0x7e, lockp, NULL);
    while (lockp->extension != 0) {
        WaitByLoop(0x400);
    }
    {
        vu32 *ids = (vu32 *) 0x27fffb0;
        ids[0] = (u32) -1;
        ids[1] = (u32) -1 << 16;
        MI_CpuClear32((void *) 0x27fffc0, 0x28);
    }
    REG_EXMEMCNT |= 0x800;
    REG_EXMEMCNT |= 0x80;
    OS_UnlockByWord(0x7e, lockp, NULL);
    OS_LockByWord(0x7f, lockp, NULL);
}

s32 OSi_DoLockByWord(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp)(void), BOOL disableFiq) {
    s32 lastLockFlag = OSi_DoTryLockByWord(lockID, lockp, ctrlFuncp, disableFiq);
    if (lastLockFlag > 0) {
        do {
            WaitByLoop(0x400);
            lastLockFlag = OSi_DoTryLockByWord(lockID, lockp, ctrlFuncp, disableFiq);
        } while (lastLockFlag > 0);
    }
    return lastLockFlag;
}

s32 OS_LockByWord(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp)(void)) {
    return OSi_DoLockByWord(lockID, lockp, ctrlFuncp, FALSE);
}

s32 OSi_DoUnlockByWord(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp)(void), BOOL disableFiq) {
    OSIntrMode irq;
    if (lockID != lockp->ownerID) {
        return -2;
    }
    if (disableFiq) {
        irq = OS_DisableInterrupts_IrqAndFiq();
    } else {
        irq = OS_DisableInterrupts();
    }
    lockp->ownerID = 0;
    if (ctrlFuncp) {
        ctrlFuncp();
    }
    lockp->lockFlag = 0;
    if (disableFiq) {
        OS_RestoreInterrupts_IrqAndFiq(irq);
    } else {
        OS_RestoreInterrupts(irq);
    }
    return 0;
}

s32 OS_UnlockByWord(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp)(void)) {
    return OSi_DoUnlockByWord(lockID, lockp, ctrlFuncp, FALSE);
}

s32 OSi_DoTryLockByWord(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp)(void), BOOL disableFiq) {
    OSIntrMode irq;
    s32 lastLockFlag;
    if (disableFiq) {
        irq = OS_DisableInterrupts_IrqAndFiq();
    } else {
        irq = OS_DisableInterrupts();
    }
    lastLockFlag = MI_SwapWord(lockID, &lockp->lockFlag);
    if (lastLockFlag == 0) {
        if (ctrlFuncp) {
            ctrlFuncp();
        }
        lockp->ownerID = lockID;
    }
    if (disableFiq) {
        OS_RestoreInterrupts_IrqAndFiq(irq);
    } else {
        OS_RestoreInterrupts(irq);
    }
    return lastLockFlag;
}

s32 OS_UnlockCartridge(u16 lockID) {
    return OSi_DoUnlockByWord(lockID, OS_CARTRIDGE_LOCK, func_0203d7d0, TRUE);
}

// Jumps straight to OS_UnlockCartridge with the argument unchanged.
// Not compiled: jumps on through r1. The compiler turns a final call into a jump only when it calls a named
// function, and then always as `ldr ip, =f; bx ip` (`b f` without interworking); any other call saves lr.
// clang-format off
asm s32 OS_UnLockCartridge(u16 lockID) {
    ldr r1, =OS_UnlockCartridge
    bx r1
}

// clang-format on

s32 OS_TryLockCartridge(u16 lockID) {
    return OSi_DoTryLockByWord(lockID, OS_CARTRIDGE_LOCK, func_0203d7b8, TRUE);
}

static void func_0203d7b8(void) {
    REG_EXMEMCNT &= ~0x80;
}

static void func_0203d7d0(void) {
    REG_EXMEMCNT |= 0x80;
}

s32 OS_LockCard(u16 lockID) {
    return OS_LockByWord(lockID, OS_CARD_LOCK, func_0203d820);
}

s32 OS_UnlockCard(u16 lockID) {
    return OS_UnlockByWord(lockID, OS_CARD_LOCK, func_0203d838);
}

static void func_0203d820(void) {
    REG_EXMEMCNT &= ~0x800;
}

static void func_0203d838(void) {
    REG_EXMEMCNT |= 0x800;
}

u16 OS_ReadOwnerOfLockWord(OSLockWord *lockp) {
    return lockp->ownerID;
}

// Allocates a lock id from the pair of free-bit words in shared memory (the
// first covers ids 0x40-0x5f, the second 0x60-0x7f). Takes the highest set bit
// of the first word, or failing that of the second; the id is the base plus
// the leading-zero count, and that bit is cleared. Returns -3 when both words
// are empty. The compares feed two consecutive conditional branches, reusing
// their flags.
// Not compiled: clz has no C expression.
// clang-format off
asm int OS_GetLockID(void) {
    ldr r3, =0x27fffb0
    ldr r1, [r3, #0]
    clz r2, r1
    cmp r2, #0x20
    bne Lm1
    b Lm2
Lm1:
    mov r0, #0x40
Lm2:
    bne Ltake
    add r3, r3, #4
    ldr r1, [r3, #0]
    clz r2, r1
    cmp r2, #0x20
    ldr r0, =0xfffffffd
    beq Lnone
    b Lsecond
Lnone:
    bx lr
Lsecond:
    mov r0, #0x60
Ltake:
    add r0, r0, r2
    mov r1, #0x80000000
    mov r1, r1, lsr r2
    ldr r2, [r3, #0]
    bic r2, r2, r1
    str r2, [r3, #0]
    bx lr
}
// clang-format on
