#include <nitro/os.h>
#include <nitro/wm.h>

WMTaskThread *data_020cdbe0;

BOOL func_02047d74(void) {
    return data_020cdbe0 != 0;
}

void func_02047d90(WMTask *task, u32 a, void *b, u32 prio) {
    WMTaskThread *t = data_020cdbe0;
    u32 e;
    WMTask *p;
    WMTask *q;
    if (!func_02047d74()) {
        return;
    }
    if (task->busy) {
        return;
    }
    if (prio > 31) {
        u32 cur = func_0203e098(&t->thread);
        if (prio == 32) {
            prio = cur != 0 ? cur - 1 : 0;
        } else if (prio == 33) {
            prio = cur < 31 ? cur + 1 : 31;
        } else if (prio == 34) {
            prio = cur;
        } else {
            prio = 31;
        }
    }
    e = OS_DisableInterrupts();
    task->busy = 1;
    task->prio = prio;
    task->mUnk_08 = a;
    task->mUnk_0c = b;
    if (t->queue == 0) {
        if (task == &t->exitTask) {
            data_020cdbe0 = 0;
        }
        t->queue = task;
        OS_WakeupThreadDirect(&t->thread);
    } else {
        p = t->queue;
        if (task == &t->exitTask) {
            while (p->next != 0) {
                p = p->next;
            }
            p->next = task;
            data_020cdbe0 = 0;
        } else if (prio < p->prio) {
            t->queue = task;
            task->next = p;
        } else {
            while ((q = p->next) != 0 && prio >= q->prio) {
                p = q;
            }
            task->next = q;
            p->next = task;
        }
    }
    OS_RestoreInterrupts(e);
}

void func_02047ef4(void *callback) {
    u32 e = OS_DisableInterrupts();
    if (func_02047d74()) {
        func_02047d90(&data_020cdbe0->exitTask, 0, callback, 0);
    }
    OS_RestoreInterrupts(e);
}
