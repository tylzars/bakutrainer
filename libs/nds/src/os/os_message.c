#include <nitro/os.h>

u32 func_0203e25c(void) {
    return 0;
}

static u32 data_020644b8 = 0xffffffff;

u32 func_0203e264(void) {
    data_020644b8 = 0x82000001;
    return 0x82000001;
}

BOOL OS_SendMessage(OSMessageQueue *mq, void *msg, s32 flags) {
    OSIntrMode e = OS_DisableInterrupts();
    while (mq->msgCount <= mq->usedCount) {
        if (!(flags & OS_MESSAGE_BLOCK)) {
            OS_RestoreInterrupts(e);
            return FALSE;
        }
        OS_SleepThread(&mq->queueSend);
    }
    mq->msgArray[(mq->firstIndex + mq->usedCount) % mq->msgCount] = msg;
    mq->usedCount++;
    OS_WakeupThread(&mq->queueReceive);
    OS_RestoreInterrupts(e);
    return TRUE;
}

BOOL OS_ReceiveMessage(OSMessageQueue *mq, void **msg, s32 flags) {
    OSIntrMode e = OS_DisableInterrupts();
    while (mq->usedCount == 0) {
        if (!(flags & OS_MESSAGE_BLOCK)) {
            OS_RestoreInterrupts(e);
            return FALSE;
        }
        OS_SleepThread(&mq->queueReceive);
    }
    if (msg != NULL) {
        *msg = mq->msgArray[mq->firstIndex];
    }
    mq->firstIndex = (mq->firstIndex + 1) % mq->msgCount;
    mq->usedCount--;
    OS_WakeupThread(&mq->queueSend);
    OS_RestoreInterrupts(e);
    return TRUE;
}

BOOL OS_JamMessage(OSMessageQueue *mq, void *msg, s32 flags) {
    OSIntrMode e = OS_DisableInterrupts();
    while (mq->msgCount <= mq->usedCount) {
        if (!(flags & OS_MESSAGE_BLOCK)) {
            OS_RestoreInterrupts(e);
            return FALSE;
        }
        OS_SleepThread(&mq->queueSend);
    }
    mq->firstIndex = (mq->firstIndex + mq->msgCount - 1) % mq->msgCount;
    mq->msgArray[mq->firstIndex] = msg;
    mq->usedCount++;
    OS_WakeupThread(&mq->queueReceive);
    OS_RestoreInterrupts(e);
    return TRUE;
}

void OS_InitMutex(OSMutex *mutex) {
    mutex->queue.head = mutex->queue.tail = NULL;
    mutex->thread = NULL;
    mutex->count = 0;
}

void OSi_UnlockAllMutex(OSThread *thread) {
    if (thread->mutexQueue.head != NULL) {
        do {
            OSMutex *mutex = OSi_RemoveMutexLinkFromQueue(&thread->mutexQueue);
            mutex->count = 0;
            mutex->thread = NULL;
            OS_WakeupThread(&mutex->queue);
        } while (thread->mutexQueue.head != NULL);
    }
}
