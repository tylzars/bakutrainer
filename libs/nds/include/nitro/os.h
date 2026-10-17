#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Start of the fast data memory (DTCM) and the stack sizes; the linker provides them.
extern char SDK_AUTOLOAD_DTCM_START[];
extern char SDK_IRQ_STACKSIZE[];
extern char SDK_SYS_STACKSIZE[];
#define HW_DTCM ((u32) SDK_AUTOLOAD_DTCM_START)

// Hardware registers (ARM9).
#define REG_EXMEMCNT (*(vu16 *) 0x4000204)
#define REG_IME (*(vu16 *) 0x4000208)
#define REG_IE (*(vu32 *) 0x4000210)
#define REG_IF (*(vu32 *) 0x4000214)

typedef u32 OSIntrMode;
typedef u32 OSIrqMask;
typedef u64 OSTick;
typedef s32 OSArenaId;
typedef s32 OSHeapHandle;

typedef struct OSThread OSThread;
typedef struct OSMutex OSMutex;
typedef struct OSAlarm OSAlarm;

typedef struct {
    /* 00 */ OSThread *head;
    /* 04 */ OSThread *tail;
} OSThreadQueue;

typedef struct {
    /* 00 */ OSMutex *head;
    /* 04 */ OSMutex *tail;
} OSMutexQueue;

typedef struct {
    /* 00 */ OSThread *prev;
    /* 04 */ OSThread *next;
} OSThreadLink;

typedef struct {
    /* 00 */ OSMutex *next;
    /* 04 */ OSMutex *prev;
} OSMutexLink;

// Register state saved for a thread.
typedef struct {
    /* 00 */ u32 cpsr;
    /* 04 */ u32 r[13];
    /* 38 */ u32 sp;
    /* 3c */ u32 lr;
    /* 40 */ u32 pc_plus4;
    /* 44 */ u32 sp_svc;
    /* 48 */ u32 cp_context[7];
} OSContext; /* 64 */

#define OS_THREAD_STATE_WAITING 0
#define OS_THREAD_STATE_READY 1
#define OS_THREAD_STATE_TERMINATED 2

struct OSThread {
    /* 00 */ OSContext context;
    /* 64 */ u32 state;
    /* 68 */ OSThread *next;
    /* 6c */ u32 id;
    /* 70 */ u32 priority;
    /* 74 */ void *profiler;
    /* 78 */ OSThreadQueue *queue;
    /* 7c */ OSThreadLink link;
    /* 84 */ OSMutex *mutex;
    /* 88 */ OSMutexQueue mutexQueue;
    /* 90 */ u32 stackTop;
    /* 94 */ u32 stackBottom;
    /* 98 */ u32 stackWarningOffset;
    /* 9c */ OSThreadQueue joinQueue;
    /* a4 */ void *specific[3];
    /* b0 */ OSAlarm *alarmForSleep;
    /* b4 */ void (*destructor)(void *);
    /* b8 */ void *userParameter;
    /* bc */ int systemErrno;
}; /* c0 */

// Thread-info block: scheduler state, current thread, thread list, switch callback, the idle and main threads and
// the idle thread's stack.
typedef struct {
    /* 00 */ u16 isNeedRescheduling;
    /* 02 */ u16 irqDepth;
    /* 04 */ OSThread *current;
    /* 08 */ OSThread *list;
    /* 0c */ void (*switchCallback)(OSThread *, OSThread *);
    /* 10 */ OSThread idleThread;
    /* d0 */ OSThread mainThread;
    /* 190 */ u8 idleStack[0xc8];
} OSThreadInfo;

extern OSThreadInfo data_020ca730;

struct OSMutex {
    /* 00 */ OSThreadQueue queue;
    /* 08 */ OSThread *thread;
    /* 0c */ s32 count;
    /* 10 */ OSMutexLink link;
}; /* 18 */

typedef struct {
    /* 00 */ OSThreadQueue queueSend;
    /* 08 */ OSThreadQueue queueReceive;
    /* 10 */ void **msgArray;
    /* 14 */ s32 msgCount;
    /* 18 */ s32 firstIndex;
    /* 1c */ s32 usedCount;
} OSMessageQueue; /* 20 */

#define OS_MESSAGE_NOBLOCK 0
#define OS_MESSAGE_BLOCK 1

// Hardware lock word shared between the processors.
typedef struct {
    /* 00 */ u32 lockFlag;
    /* 04 */ u16 ownerID;
    /* 06 */ u16 extension;
} OSLockWord;

struct OSAlarm {
    /* 00 */ void (*handler)(void *);
    /* 04 */ void *arg;
    /* 08 */ u32 tag;
    /* 0c */ u64 fire;
    /* 14 */ OSAlarm *prev;
    /* 18 */ OSAlarm *next;
    /* 1c */ u64 period;
    /* 24 */ u64 start;
}; /* 2c */

// Owner information from the firmware settings.
typedef struct {
    /* 00 */ u8 favoriteColor;
    /* 01 */ u8 birthMonth;
    /* 02 */ u8 birthDay;
    /* 03 */ u8 mUnk_03;
    /* 04 */ u16 nickName[10];
    /* 18 */ u16 nickNameEnd;
    /* 1a */ u16 nickNameLength;
    /* 1c */ u16 comment[26];
    /* 50 */ u16 commentEnd;
    /* 52 */ u16 commentLength;
} OSOwnerInfo; /* 54 */

// Arenas.
#define OS_ARENA_MAIN 0
#define OS_ARENA_MAINEX 2
#define OS_ARENA_ITCM 3
#define OS_ARENA_DTCM 4
#define OS_ARENA_SHARED 5
#define OS_ARENA_WRAM_MAIN 6

// Heap cell header.
typedef struct Cell {
    /* 00 */ struct Cell *prev;
    /* 04 */ struct Cell *next;
    /* 08 */ s32 size;
} Cell;

// Interrupt dispatch and the thread-switch return path (os_irq_itcm.c, in ITCM)
void func_01ff8000(void);
void func_01ff8068(void);

// Thread queue the interrupt-wait function sleeps on (in DTCM).
extern OSThreadQueue data_027e0060;

// Interrupt table (os_irqtable.c)
void OS_WaitIrq(BOOL clear, OSIrqMask irqFlags);
void func_0203d22c(void);
void func_0203d230(int n);
void func_0203d2b8(void);
void func_0203d2c8(void);
void func_0203d2d8(void);
void func_0203d2e8(void);
void func_0203d2f8(void);
void func_0203d308(void);
void func_0203d318(void);
void func_0203d328(void);
void func_0203d338(void);
void OS_SetIrqFunction(OSIrqMask mask, void (*function)(void));
void OSi_EnterDmaCallback(int dmaNo, void (*callback)(void *), void *arg);
void OSi_EnterTimerCallback(int timerNo, void (*callback)(void *), void *arg);

// Interrupt masks (os_interrupt.c)
OSIrqMask OS_SetIrqMask(OSIrqMask mask);
OSIrqMask OS_EnableIrqMask(OSIrqMask mask);
OSIrqMask OS_DisableIrqMask(OSIrqMask mask);
OSIrqMask OS_ResetRequestIrqMask(OSIrqMask mask);
void func_0203d520(void);

// Spinlocks (os_spinlock.c)
void OS_InitLock(void);
s32 OSi_DoLockByWord(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp)(void), BOOL disableFiq);
s32 OS_LockByWord(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp)(void));
s32 OSi_DoUnlockByWord(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp)(void), BOOL disableFiq);
s32 OS_UnlockByWord(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp)(void));
s32 OSi_DoTryLockByWord(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp)(void), BOOL disableFiq);
s32 OS_UnlockCartridge(u16 lockID);
s32 OS_UnLockCartridge(u16 lockID);
s32 OS_TryLockCartridge(u16 lockID);
s32 OS_LockCard(u16 lockID);
s32 OS_UnlockCard(u16 lockID);
u16 OS_ReadOwnerOfLockWord(OSLockWord *lockp);
int OS_GetLockID(void);

// Threads (os_thread.c)
u32 OSi_GetUnusedThreadId(void);
void OSi_InsertLinkToQueue(OSThreadQueue *queue, OSThread *thread);
OSThread *OSi_RemoveLinkFromQueue(OSThreadQueue *queue);
OSThread *OSi_RemoveSpecifiedLinkFromQueue(OSThreadQueue *queue, OSThread *thread);
OSMutex *OSi_RemoveMutexLinkFromQueue(OSMutexQueue *queue);
void OSi_InsertThreadToList(OSThread *thread);
void OSi_RemoveThreadFromList(OSThread *thread);
void OSi_RescheduleThread(void);
void OS_InitThread(void);
void OS_CreateThread(OSThread *thread, void (*func)(void *), void *arg, void *stack, u32 stackSize, u32 prio);
void OS_ExitThread(void);
void OSi_ExitThread(OSThread *thread, void *arg);
void func_0203de38(void *arg);
void OSi_ExitThread_Destroy(void);
void OS_SleepThread(OSThreadQueue *queue);
void OS_WakeupThread(OSThreadQueue *queue);
void OS_WakeupThreadDirect(OSThread *thread);
OSThread *func_0203dfac(void);
void OS_RescheduleThread(void);
BOOL OS_SetThreadPriority(OSThread *thread, u32 prio);
u32 func_0203e098(const OSThread *thread);
void *OS_SetSwitchThreadCallback(void (*callback)(OSThread *, OSThread *));
void OSi_IdleThreadProc(void);
u32 OS_DisableScheduler(void);
u32 OS_EnableScheduler(void);
void OS_SetThreadDestructor(OSThread *thread, void (*destructor)(void *));
void OS_InitContext(OSContext *context, u32 newpc, u32 newsp);
u32 OS_SaveContext(OSContext *context);
void OS_LoadContext(OSContext *context);

// Messages and mutexes (os_message.c)
u32 func_0203e25c(void);
u32 func_0203e264(void);
BOOL OS_SendMessage(OSMessageQueue *mq, void *msg, s32 flags);
BOOL OS_ReceiveMessage(OSMessageQueue *mq, void **msg, s32 flags);
BOOL OS_JamMessage(OSMessageQueue *mq, void *msg, s32 flags);
void OS_InitMutex(OSMutex *mutex);
void OSi_UnlockAllMutex(OSThread *thread);

// Cache (os_cache.c)
void func_0203e4a4(void);
void DC_FlushAll(void);
void DC_InvalidateRange(void *addr, u32 size);
void DC_StoreRange(void *addr, u32 size);
void DC_FlushRange(void *addr, u32 size);
void DC_WaitWriteBufferEmpty(void);
void IC_InvalidateAll(void);
void IC_InvalidateRange(void *addr, u32 size);

// Initialisation (os_init.c)
void OSi_WaitVCount0(void);
void OS_Init(void);

// Arenas (os_arena.c)
void OS_InitArena(void);
void OS_InitArenaEx(void);
void *OS_GetArenaHi(OSArenaId id);
void *OS_GetArenaLo(OSArenaId id);
void *OS_GetInitArenaHi(OSArenaId id);
void *OS_GetInitArenaLo(OSArenaId id);
void OS_SetArenaHi(OSArenaId id, void *newHi);
void OS_SetArenaLo(OSArenaId id, void *newLo);

// Heaps (os_alloc.c)
Cell *DLAddFront(Cell *list, Cell *cell);
Cell *DLExtract(Cell *list, Cell *cell);
Cell *DLInsert(Cell *list, Cell *cell);
void *OS_AllocFromHeap(OSArenaId id, OSHeapHandle heap, u32 size);
void OS_FreeToHeap(OSArenaId id, OSHeapHandle heap, void *ptr);
OSHeapHandle OS_SetCurrentHeap(OSArenaId id, OSHeapHandle heap);
void *OS_InitAlloc(OSArenaId id, void *lo, void *hi, int maxHeaps);
OSHeapHandle OS_CreateHeap(OSArenaId id, void *lo, void *hi);
s32 func_0203ed14(OSArenaId id, OSHeapHandle heap);

// Protection unit (os_protection.c)
u32 func_0203ed7c(void);
void func_0203ed90(void);
void func_0203eda0(void);
void func_0203edb0(u32 clear, u32 set);
void OS_SetProtectionRegion1(u32 value);
void OS_SetProtectionRegion2(u32 value);

// Exceptions (os_exception.c)
void OS_InitException(void);
void OSi_ExceptionHandler(void);
void func_0203eec8(void);
void func_0203eedc(void);
void func_0203ef6c(void);

// Timers (os_timer.c)
void OSi_SetTimerReserved(int timerNum);
void func_0203f004(int timerNum, u32 count, u32 control);
void func_0203f024(int timerNum);

// Tick (os_tick.c)
void OS_InitTick(void);
BOOL OS_IsTickAvailable(void);
void OSi_CountUpTick(void);
OSTick OS_GetTick(void);

// Alarms (os_alarm.c, os_valarm.c)
void OSi_ArrangeTimer(OSAlarm *alarm);
void OS_InitAlarm(void);
void OS_CreateAlarm(OSAlarm *alarm);
void OSi_InsertAlarm(OSAlarm *alarm, OSTick fire);
void OS_SetPeriodicAlarm(OSAlarm *alarm, OSTick start, OSTick period, void (*handler)(void *), void *arg);
void OS_CancelAlarm(OSAlarm *alarm);
void AlarmTimerEntry(void *arg);
void OSi_AlarmHandler(void);
void OS_InitVAlarm(void);

// Interrupts and CPU state (os_system.c)
OSIntrMode OS_EnableInterrupts(void);
OSIntrMode OS_DisableInterrupts(void);
OSIntrMode OS_RestoreInterrupts(OSIntrMode old);
OSIntrMode OS_DisableInterrupts_IrqAndFiq(void);
OSIntrMode OS_RestoreInterrupts_IrqAndFiq(OSIntrMode old);
u32 OS_GetCpsrIrqBits(void);
u32 OS_GetProcMode(void);
void OS_SpinWait(u32 n);

// Reset (os_reset.c)
void func_0203f6a4(void);
void OS_InitReset(void);
void ResetFifoReceive(int tag, u32 data, BOOL err);

// Owner (os_owner.c)
void OS_GetMacAddress(u8 *macAddress);
void OS_GetOwnerInfo(OSOwnerInfo *info);

// Interrupt owner table (os_irqowner.c)
void func_0203f7e8(void);

// Termination (os_terminate.c)
void OS_Terminate(void);
void OS_Halt(void);

#ifdef __cplusplus
}
#endif
