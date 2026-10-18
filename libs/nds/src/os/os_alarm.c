#include <nitro/os.h>

static struct {
    u16 initialized;
    OSAlarm *head;
    OSAlarm *tail;
} sAlarmQueue;

void OSi_ArrangeTimer(OSAlarm *alarm) {
    OSTick tick = OS_GetTick();
    s64 delta;
    u16 timer;
    *(vu16 *) 0x4000106 = 0;
    delta = (s64) (alarm->fire - tick);
    OSi_EnterTimerCallback(1, AlarmTimerEntry, NULL);
    if (delta < 0) {
        timer = 0xfffe;
    } else if (delta < 0x10000) {
        timer = (u16) ~delta;
    } else {
        timer = 0;
    }
    *(vu16 *) 0x4000104 = timer;
    *(vu16 *) 0x4000106 = 0xc1;
    OS_EnableIrqMask(0x10);
}

void OS_InitAlarm(void) {
    if (sAlarmQueue.initialized) {
        return;
    }
    sAlarmQueue.initialized = TRUE;
    OSi_SetTimerReserved(1);
    sAlarmQueue.head = NULL;
    sAlarmQueue.tail = NULL;
    OS_DisableIrqMask(0x10);
}

void OS_CreateAlarm(OSAlarm *alarm) {
    alarm->handler = NULL;
    alarm->tag = 0;
}

void OSi_InsertAlarm(OSAlarm *alarm, OSTick fire) {
    OSAlarm *next;
    if (alarm->period != 0) {
        OSTick tick = OS_GetTick();
        fire = alarm->start;
        if (fire < tick) {
            fire += alarm->period * ((tick - fire) / alarm->period + 1);
        }
    }
    alarm->fire = fire;
    for (next = sAlarmQueue.head; next != NULL; next = next->next) {
        if ((s64) (fire - next->fire) < 0) {
            alarm->prev = next->prev;
            next->prev = alarm;
            alarm->next = next;
            if (alarm->prev != NULL) {
                alarm->prev->next = alarm;
                return;
            }
            sAlarmQueue.head = alarm;
            OSi_ArrangeTimer(alarm);
            return;
        }
    }
    alarm->next = NULL;
    next = sAlarmQueue.tail;
    sAlarmQueue.tail = alarm;
    alarm->prev = next;
    if (alarm->prev != NULL) {
        alarm->prev->next = alarm;
        return;
    }
    sAlarmQueue.tail = alarm;
    sAlarmQueue.head = alarm;
    OSi_ArrangeTimer(alarm);
}

void OS_SetPeriodicAlarm(OSAlarm *alarm, OSTick start, OSTick period, void (*handler)(void *), void *arg) {
    OSIntrMode e;
    if (alarm == NULL || alarm->handler != NULL) {
        OS_Terminate();
    }
    e = OS_DisableInterrupts();
    alarm->period = period;
    alarm->start = start;
    alarm->handler = handler;
    alarm->arg = arg;
    OSi_InsertAlarm(alarm, 0);
    OS_RestoreInterrupts(e);
}

void OS_CancelAlarm(OSAlarm *alarm) {
    OSIntrMode e = OS_DisableInterrupts();
    OSAlarm *next;
    OSAlarm *prev;
    if (alarm->handler == NULL) {
        OS_RestoreInterrupts(e);
        return;
    }
    next = alarm->next;
    if (next == NULL) {
        sAlarmQueue.tail = alarm->prev;
    } else {
        next->prev = alarm->prev;
    }
    prev = alarm->prev;
    if (prev != NULL) {
        prev->next = next;
    } else {
        sAlarmQueue.head = next;
        if (next != NULL) {
            OSi_ArrangeTimer(next);
        }
    }
    alarm->handler = NULL;
    alarm->period = 0;
    OS_RestoreInterrupts(e);
}

// Timer interrupt entry: calls OSi_AlarmHandler with r0 kept.
// Not compiled: saves r0 beside lr and returns with `ldmfd sp!, {r0, lr}; bx lr`. A compiled function
// saves r0 only as an interrupt function, which saves every register among r0-r7 it writes (the call
// writes r1-r3 too), and a compiled function that saved lr returns by loading it into pc.
asm void AlarmTimerEntry(void *arg) {
    stmfd sp!, {r0, lr}
    bl OSi_AlarmHandler
    ldmfd sp!, {r0, lr}
    bx lr
}

void OSi_AlarmHandler(void) {
    OSAlarm *alarm;
    void (*handler)(void *);
    OSTick tick;
    *(vu16 *) 0x4000106 = 0;
    OS_DisableIrqMask(0x10);
    *(vu32 *) (HW_DTCM + 0x3ff8) |= 0x10;
    tick = OS_GetTick();
    alarm = sAlarmQueue.head;
    if (alarm == NULL) {
        return;
    }
    if (tick < alarm->fire) {
        OSi_ArrangeTimer(alarm);
        return;
    }
    sAlarmQueue.head = alarm->next;
    if (sAlarmQueue.head == NULL) {
        sAlarmQueue.tail = NULL;
    } else {
        sAlarmQueue.head->prev = NULL;
    }
    handler = alarm->handler;
    if (alarm->period == 0) {
        alarm->handler = NULL;
    }
    if (handler != NULL) {
        handler(alarm->arg);
    }
    if (alarm->period != 0) {
        alarm->handler = handler;
        OSi_InsertAlarm(alarm, 0);
    }
    if (sAlarmQueue.head != NULL) {
        OSi_ArrangeTimer(sAlarmQueue.head);
    }
}
