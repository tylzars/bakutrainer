// Thread bookkeeping. The thread-info block holds the scheduler state, the
// current-thread pointer, the thread list, the idle and main threads and the
// idle thread's stack.
#include <nitro/os.h>
#include <nitro/cp.h>
#include <nitro/mi.h>

typedef struct {
    void (*switchHook)(OSThread *, OSThread *);
    u32 schedulerLock;
    OSThread **pCurrent;
    u32 initialized;
    u32 mUnk_10;
    u32 mUnk_14;
    u32 mUnk_18;
    u32 exitStack;
    u32 lastThreadId;
    OSThreadInfo ti;
} OSThreadSys;

OSThreadSys data_020ca70c;

#define OS_IRQ_MODE 0x12
#define STACK_END_MAGIC 0xfddb597d
#define STACK_OVERFLOW_MAGIC 0x7bf9dd5b
#define IRQ_STACK_BOTTOM (HW_DTCM + 0x3f80 - (u32) SDK_IRQ_STACKSIZE)

u32 OSi_GetUnusedThreadId(void) {
    return ++data_020ca70c.lastThreadId;
}

void OSi_InsertLinkToQueue(OSThreadQueue *queue, OSThread *thread) {
    OSThread *t = queue->head;
    OSThread *prev;
    while (t != NULL && t->priority <= thread->priority) {
        if (t == thread) {
            return;
        }
        t = t->link.next;
    }
    if (t == NULL) {
        prev = queue->tail;
        if (prev == NULL) {
            queue->head = thread;
        } else {
            prev->link.next = thread;
        }
        thread->link.prev = prev;
        thread->link.next = NULL;
        queue->tail = thread;
    } else {
        prev = t->link.prev;
        if (prev == NULL) {
            queue->head = thread;
        } else {
            prev->link.next = thread;
        }
        thread->link.prev = prev;
        thread->link.next = t;
        t->link.prev = thread;
    }
}

OSThread *OSi_RemoveLinkFromQueue(OSThreadQueue *queue) {
    OSThread *t = queue->head;
    if (t != NULL) {
        OSThread *next = t->link.next;
        queue->head = next;
        if (next != NULL) {
            next->link.prev = NULL;
        } else {
            queue->tail = NULL;
            t->queue = NULL;
        }
    }
    return t;
}

OSThread *OSi_RemoveSpecifiedLinkFromQueue(OSThreadQueue *queue, OSThread *thread) {
    OSThread *t;
    OSThread *next;
    OSThread *prev;
    prev = queue->head;
    t = prev;
    while (t != NULL) {
        next = t->link.next;
        if (t == thread) {
            if (prev == t) {
                prev = t->link.prev;
                queue->head = next;
            } else {
                prev = t->link.prev;
                prev->link.next = next;
            }
            if (queue->tail == t) {
                queue->tail = prev;
            } else {
                next->link.prev = prev;
            }
            break;
        }
        t = next;
    }
    return t;
}

OSMutex *OSi_RemoveMutexLinkFromQueue(OSMutexQueue *queue) {
    OSMutex *m = queue->head;
    if (m != NULL) {
        OSMutex *next = m->link.next;
        queue->head = next;
        if (next != NULL) {
            next->link.prev = NULL;
        } else {
            queue->tail = NULL;
        }
    }
    return m;
}

void OSi_InsertThreadToList(OSThread *thread) {
    OSThread *t;
    OSThread *prev = NULL;
    OSThread *head = data_020ca70c.ti.list;
    t = head;
    while (t != NULL && t->priority < thread->priority) {
        prev = t;
        t = t->next;
    }
    if (prev == NULL) {
        thread->next = head;
        data_020ca70c.ti.list = thread;
    } else {
        thread->next = prev->next;
        prev->next = thread;
    }
}

void OSi_RemoveThreadFromList(OSThread *thread) {
    OSThread *prev = NULL;
    OSThread *t = data_020ca70c.ti.list;
    while (t != NULL && t != thread) {
        prev = t;
        t = t->next;
    }
    if (prev == NULL) {
        data_020ca70c.ti.list = thread->next;
    } else {
        prev->next = thread->next;
    }
}

void OSi_RescheduleThread(void) {
    OSThreadInfo *ti;
    OSThread *next;
    OSThread *cur;
    if (data_020ca70c.schedulerLock != 0) {
        return;
    }
    ti = &data_020ca70c.ti;
    if (data_020ca70c.ti.irqDepth != 0 || OS_GetProcMode() == OS_IRQ_MODE) {
        ti->isNeedRescheduling = TRUE;
        return;
    }
    cur = *data_020ca70c.pCurrent;
    next = func_0203dfac();
    if (cur == next || next == NULL) {
        return;
    }
    if (cur->state != OS_THREAD_STATE_TERMINATED) {
        if (OS_SaveContext(&cur->context) != 0) {
            return;
        }
    }
    if (data_020ca70c.switchHook != NULL) {
        data_020ca70c.switchHook(cur, next);
    }
    if (ti->switchCallback != NULL) {
        ti->switchCallback(cur, next);
    }
    data_020ca70c.ti.current = next;
    OS_LoadContext(&next->context);
}

void OS_InitThread(void) {
    u32 stackTop;
    if (data_020ca70c.initialized != 0) {
        return;
    }
    data_020ca70c.initialized = TRUE;
    data_020ca70c.pCurrent = &data_020ca70c.ti.current;
    data_020ca70c.ti.mainThread.priority = 0x10;
    data_020ca70c.ti.mainThread.id = 0;
    data_020ca70c.ti.mainThread.state = OS_THREAD_STATE_READY;
    data_020ca70c.ti.mainThread.next = NULL;
    data_020ca70c.ti.mainThread.profiler = NULL;
    data_020ca70c.ti.list = &data_020ca70c.ti.mainThread;
    data_020ca70c.ti.current = &data_020ca70c.ti.mainThread;
    if ((s32) SDK_SYS_STACKSIZE > 0) {
        stackTop = IRQ_STACK_BOTTOM - (s32) SDK_SYS_STACKSIZE;
    } else {
        stackTop = 0x27e0080 - (s32) SDK_SYS_STACKSIZE;
    }
    data_020ca70c.ti.mainThread.stackBottom = IRQ_STACK_BOTTOM;
    data_020ca70c.ti.mainThread.stackTop = stackTop;
    data_020ca70c.ti.mainThread.stackWarningOffset = 0;
    *(u32 *) (data_020ca70c.ti.mainThread.stackBottom - 4) = STACK_END_MAGIC;
    *(u32 *) data_020ca70c.ti.mainThread.stackTop = STACK_OVERFLOW_MAGIC;
    data_020ca70c.ti.mainThread.joinQueue.tail = NULL;
    data_020ca70c.ti.mainThread.joinQueue.head = NULL;
    data_020ca70c.ti.isNeedRescheduling = FALSE;
    data_020ca70c.ti.irqDepth = 0;
    *(void **) 0x27fffa0 = &data_020ca70c.ti.isNeedRescheduling;
    OS_SetSwitchThreadCallback(NULL);
    OS_CreateThread(&data_020ca70c.ti.idleThread, (void (*)(void *)) OSi_IdleThreadProc, NULL,
                    data_020ca70c.ti.idleStack + sizeof(data_020ca70c.ti.idleStack),
                    sizeof(data_020ca70c.ti.idleStack), 0x1f);
    data_020ca70c.ti.idleThread.priority = 0x20;
    data_020ca70c.ti.idleThread.state = OS_THREAD_STATE_READY;
}

static inline void OS_InitThreadQueue(OSThreadQueue *q) {
    q->head = q->tail = NULL;
}

void OS_CreateThread(OSThread *thread, void (*func)(void *), void *arg, void *stack, u32 stackSize, u32 prio) {
    OSIntrMode e = OS_DisableInterrupts();
    u32 stackTop;
    u32 id = OSi_GetUnusedThreadId();
    thread->priority = prio;
    thread->id = id;
    thread->state = OS_THREAD_STATE_WAITING;
    thread->profiler = NULL;
    OSi_InsertThreadToList(thread);
    thread->stackBottom = (u32) stack;
    stackTop = (u32) stack - stackSize;
    thread->stackTop = stackTop;
    thread->stackWarningOffset = 0;
    *(u32 *) (thread->stackBottom - 4) = STACK_END_MAGIC;
    *(u32 *) thread->stackTop = STACK_OVERFLOW_MAGIC;
    OS_InitThreadQueue(&thread->joinQueue);
    OS_InitContext(&thread->context, (u32) func, (u32) stack - 4);
    thread->context.r[0] = (u32) arg;
    thread->context.lr = (u32) OS_ExitThread;
    MI_CpuClear32((void *) (stackTop + 4), stackSize - 8);
    thread->mutex = NULL;
    thread->mutexQueue.head = NULL;
    thread->mutexQueue.tail = NULL;
    OS_SetThreadDestructor(thread, NULL);
    thread->queue = NULL;
    thread->link.next = NULL;
    thread->link.prev = NULL;
    MI_CpuClear32(&thread->specific, sizeof(thread->specific));
    thread->alarmForSleep = NULL;
    OS_RestoreInterrupts(e);
}

void OS_ExitThread(void) {
    OS_DisableInterrupts();
    OSi_ExitThread(data_020ca70c.ti.current, NULL);
}

void OSi_ExitThread(OSThread *thread, void *arg) {
    if (data_020ca70c.exitStack != 0) {
        OS_InitContext(&thread->context, (u32) func_0203de38, data_020ca70c.exitStack);
        thread->context.r[0] = (u32) arg;
        thread->context.cpsr |= 0x80;
        thread->state = OS_THREAD_STATE_READY;
        OS_LoadContext(&thread->context);
    } else {
        func_0203de38(arg);
    }
}

// Runs the current thread's destructor (if any), then finishes the exit.
void func_0203de38(void *arg) {
    OSThread *cur = *data_020ca70c.pCurrent;
    void (*dtor)(void *) = cur->destructor;
    if (dtor != NULL) {
        cur->destructor = NULL;
        dtor(arg);
        OS_DisableInterrupts();
    }
    OSi_ExitThread_Destroy();
}

void OSi_ExitThread_Destroy(void) {
    OSThread *cur = *data_020ca70c.pCurrent;
    OS_DisableScheduler();
    OSi_UnlockAllMutex(cur);
    if (cur->queue != NULL) {
        OSi_RemoveSpecifiedLinkFromQueue(cur->queue, cur);
    }
    OSi_RemoveThreadFromList(cur);
    cur->state = OS_THREAD_STATE_TERMINATED;
    OS_WakeupThread(&cur->joinQueue);
    OS_EnableScheduler();
    OS_RescheduleThread();
    OS_Terminate();
}

void OS_SleepThread(OSThreadQueue *queue) {
    OSIntrMode e = OS_DisableInterrupts();
    OSThread *cur = *data_020ca70c.pCurrent;
    if (queue != NULL) {
        cur->queue = queue;
        OSi_InsertLinkToQueue(queue, cur);
    }
    cur->state = OS_THREAD_STATE_WAITING;
    OSi_RescheduleThread();
    OS_RestoreInterrupts(e);
}

void OS_WakeupThread(OSThreadQueue *queue) {
    OSIntrMode e = OS_DisableInterrupts();
    if (queue->head != NULL) {
        while (queue->head != NULL) {
            OSThread *t = OSi_RemoveLinkFromQueue(queue);
            t->state = OS_THREAD_STATE_READY;
            t->queue = NULL;
            t->link.next = NULL;
            t->link.prev = NULL;
        }
        queue->tail = NULL;
        queue->head = NULL;
        OSi_RescheduleThread();
    }
    OS_RestoreInterrupts(e);
}

void OS_WakeupThreadDirect(OSThread *thread) {
    OSIntrMode e = OS_DisableInterrupts();
    thread->state = OS_THREAD_STATE_READY;
    OSi_RescheduleThread();
    OS_RestoreInterrupts(e);
}

OSThread *func_0203dfac(void) {
    OSThread *t = data_020ca70c.ti.list;
    while (t != NULL && t->state != OS_THREAD_STATE_READY) {
        t = t->next;
    }
    return t;
}

void OS_RescheduleThread(void) {
    OSIntrMode e = OS_DisableInterrupts();
    OSi_RescheduleThread();
    OS_RestoreInterrupts(e);
}

BOOL OS_SetThreadPriority(OSThread *thread, u32 prio) {
    OSThread *t = data_020ca70c.ti.list;
    OSThread *prev = NULL;
    OSIntrMode e = OS_DisableInterrupts();
    while (t != NULL && t != thread) {
        prev = t;
        t = t->next;
    }
    if (t == NULL || t == &data_020ca70c.ti.idleThread) {
        OS_RestoreInterrupts(e);
        return FALSE;
    }
    if (t->priority != prio) {
        if (prev == NULL) {
            data_020ca70c.ti.list = thread->next;
        } else {
            prev->next = thread->next;
        }
        thread->priority = prio;
        OSi_InsertThreadToList(thread);
        OSi_RescheduleThread();
    }
    OS_RestoreInterrupts(e);
    return TRUE;
}

u32 func_0203e098(const OSThread *thread) {
    return thread->priority;
}

void *OS_SetSwitchThreadCallback(void (*callback)(OSThread *, OSThread *)) {
    OSIntrMode e = OS_DisableInterrupts();
    void *old = (void *) data_020ca70c.ti.switchCallback;
    data_020ca70c.ti.switchCallback = callback;
    OS_RestoreInterrupts(e);
    return old;
}

void OSi_IdleThreadProc(void) {
    OS_EnableInterrupts();
    while (1) {
        OS_Halt();
    }
}

u32 OS_DisableScheduler(void) {
    OSIntrMode e = OS_DisableInterrupts();
    u32 cnt = data_020ca70c.schedulerLock;
    u32 old;
    if (cnt < 0xffffffff) {
        old = cnt;
        data_020ca70c.schedulerLock = cnt + 1;
    }
    OS_RestoreInterrupts(e);
    return old;
}

u32 OS_EnableScheduler(void) {
    OSIntrMode e = OS_DisableInterrupts();
    u32 old = 0;
    u32 cnt = data_020ca70c.schedulerLock;
    if (cnt != 0) {
        old = cnt;
        data_020ca70c.schedulerLock = cnt - 1;
    }
    OS_RestoreInterrupts(e);
    return old;
}

void OS_SetThreadDestructor(OSThread *thread, void (*destructor)(void *)) {
    thread->destructor = destructor;
}

// Fills a context record for a fresh start: +0x40 = newpc + 4 (the resume
// address OS_LoadContext uses), +0x44 = newsp, +0x38 (sp slot) = newsp - 0x40,
// lowered by 4 more if that leaves bit 2 set; +0 (CPSR) = 0x3f if newpc has
// bit 0 set, else 0x1f; zeroes r0-r12 (+4..+0x34) and lr (+0x3c).
asm void OS_InitContext(OSContext *context, u32 newpc, u32 newsp) {
    add r1, r1, #4
    str r1, [r0, #0x40]
    str r2, [r0, #0x44]
    sub r2, r2, #0x40
    tst r2, #4
    bne sp_adj
    b sp_done
sp_adj:
    sub r2, r2, #4
sp_done:
    str r2, [r0, #0x38]
    ands r1, r1, #1
    bne thumb
    b chk
thumb:
    mov r1, #0x3f
chk:
    beq arm
    b store
arm:
    mov r1, #0x1f
store:
    str r1, [r0, #0]
    mov r1, #0
    str r1, [r0, #4]
    str r1, [r0, #8]
    str r1, [r0, #0xc]
    str r1, [r0, #0x10]
    str r1, [r0, #0x14]
    str r1, [r0, #0x18]
    str r1, [r0, #0x1c]
    str r1, [r0, #0x20]
    str r1, [r0, #0x24]
    str r1, [r0, #0x28]
    str r1, [r0, #0x2c]
    str r1, [r0, #0x30]
    str r1, [r0, #0x34]
    str r1, [r0, #0x3c]
    bx lr
}

// r0 points at a context record. First saves the divider and square-root
// registers into the tail of the record (+0x48), then stores CPSR at +0, the
// supervisor-mode stack pointer at +0x44, and r0..lr at +4..+0x3c (the r0 slot
// holds 1, the r1 slot ctx+4 and the r2 slot the CPSR value), plus at +0x40
// the address 4 bytes past the final 'bx lr'. Returns 0 on the direct path;
// OS_LoadContext resumes at that address minus 4, i.e. at the 'bx lr', with
// r0 = 1, so the caller sees a second return of 1.
asm u32 OS_SaveContext(OSContext *context) {
    stmfd sp!, {r0, lr}
    add r0, r0, #0x48
    ldr r1, =func_02042898
    blx r1
    ldmfd sp!, {r0, lr}
    add r1, r0, #0
    mrs r2, cpsr
    str r2, [r1], #4
    mov r0, #0xd3
    msr cpsr_c, r0
    str sp, [r1, #0x40]
    msr cpsr_c, r2
    mov r0, #1
    stmia r1, {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, ip, sp, lr}
    add r0, pc, #8
    str r0, [r1, #0x3c]
    mov r0, #0
    bx lr
}

// r0 points at a context record produced by OS_SaveContext. Restores the
// divider and square-root registers from +0x48, enters supervisor mode with
// IRQ/FIQ masked, loads SPSR from the saved CPSR (+0), the supervisor sp from
// +0x44 and lr from +0x40, then loads r0-r12 and the user/system-bank sp and
// lr from +4..+0x3c, and returns to lr - 4 with CPSR restored from SPSR.
asm void OS_LoadContext(OSContext *context) {
    stmfd sp!, {r0, lr}
    add r0, r0, #0x48
    ldr r1, =func_020428d8
    blx r1
    ldmfd sp!, {r0, lr}
    mrs r1, cpsr
    bic r1, r1, #0x1f
    orr r1, r1, #0xd3
    msr cpsr_c, r1
    ldr r1, [r0], #4
    msr spsr_fsxc, r1
    ldr sp, [r0, #0x40]
    ldr lr, [r0, #0x3c]
    ldmia r0, {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, ip, sp, lr}^
    mov r0, r0
    subs pc, lr, #4
}
