#include <nitro/snd.h>
#include <nitro/os.h>
#include <nitro/pxi.h>

#define SND_FIFO_TAG 7
#define SND_PXI_PROC 1
#define SND_COMMAND_POOL_SIZE 256
#define SND_COMMAND_RING_SIZE 9
#define SND_COMMAND_MAX_IN_FLIGHT 8

// cmdRing holds the head of each command list sent to the sound processor, in send order.
static u32 cmdProcCount;
static u32 cmdReplyCount;
static SNDCommand *cmdFreeTail;
static SNDCommand *cmdPendingHead;
static SNDCommand *cmdFreeList;
static SNDCommand *cmdPendingTail;
static int cmdRingRead;
static int cmdRingWrite;
static int cmdPendingCount;

static SNDCommand *cmdRing[SND_COMMAND_RING_SIZE];
static SNDSharedWork sharedWork __attribute__((aligned(32)));
static SNDCommand cmdPool[SND_COMMAND_POOL_SIZE];

void SND_CommandInit(void) {
    SNDCommand *cmd;
    int i;
    SNDCommand *n;
    func_020408bc();
    cmdFreeList = cmdPool;
    i = 0;
    while (i < SND_COMMAND_POOL_SIZE - 1) {
        n = &cmdPool[i + 1];
        cmdPool[i].next = n;
        i++;
    }
    cmdPool[SND_COMMAND_POOL_SIZE - 1].next = NULL;
    cmdFreeTail = &cmdPool[SND_COMMAND_POOL_SIZE - 1];
    cmdPendingHead = NULL;
    cmdPendingTail = NULL;
    cmdPendingCount = 0;
    cmdRingRead = 0;
    cmdRingWrite = 0;
    cmdReplyCount = 1;
    cmdProcCount = 0;
    SNDi_SharedWork = &sharedWork;
    func_02040ae4(&sharedWork);
    cmd = SND_AllocCommand(1);
    if (cmd == NULL) {
        return;
    }
    cmd->id = 0x1d;
    cmd->arg[0] = (u32) SNDi_SharedWork;
    SND_PushCommand(cmd);
    SND_FlushCommand(1);
}

SNDCommand *SND_RecvCommandReply(u32 flags) {
    OSIntrMode e = OS_DisableInterrupts();
    SNDCommand *node;
    SNDCommand *tail;
    if (flags & 1) {
        if (func_02040abc() == cmdProcCount) {
            do {
                OS_RestoreInterrupts(e);
                OS_SpinWait(100);
                e = OS_DisableInterrupts();
            } while (func_02040abc() == cmdProcCount);
        }
    } else {
        if (func_02040abc() == cmdProcCount) {
            OS_RestoreInterrupts(e);
            return NULL;
        }
    }
    node = cmdRing[cmdRingRead];
    cmdRingRead++;
    if (cmdRingRead > SND_COMMAND_RING_SIZE - 1) {
        cmdRingRead = 0;
    }
    tail = node;
    if (tail->next != NULL) {
        do {
            tail = tail->next;
        } while (tail->next != NULL);
    }
    if (cmdFreeTail != NULL) {
        cmdFreeTail->next = node;
    } else {
        cmdFreeList = node;
    }
    cmdFreeTail = tail;
    cmdPendingCount--;
    cmdProcCount++;
    OS_RestoreInterrupts(e);
    return node;
}

SNDCommand *SND_AllocCommand(u32 flags) {
    SNDCommand *cmd;
    if (!func_0204098c()) {
        return NULL;
    }
    cmd = func_02040944();
    if (cmd != NULL) {
        return cmd;
    }
    if (!(flags & 1)) {
        return NULL;
    }
    if ((int) func_0204087c() > 0) {
        while (SND_RecvCommandReply(0) != NULL) {
        }
        cmd = func_02040944();
        if (cmd != NULL) {
            return cmd;
        }
    } else {
        SND_FlushCommand(1);
    }
    func_0204091c();
    do {
        SND_RecvCommandReply(1);
        cmd = func_02040944();
    } while (cmd == NULL);
    return cmd;
}

void SND_PushCommand(SNDCommand *cmd) {
    OSIntrMode e = OS_DisableInterrupts();
    SNDCommand *t;
    t = cmdPendingTail;
    if (t == NULL) {
        cmdPendingHead = cmd;
        cmdPendingTail = cmd;
    } else {
        t->next = cmd;
        cmdPendingTail = cmd;
    }
    cmd->next = NULL;
    OS_RestoreInterrupts(e);
}

BOOL SND_FlushCommand(u32 flags) {
    OSIntrMode e = OS_DisableInterrupts();
    if (cmdPendingHead == NULL) {
        OS_RestoreInterrupts(e);
        return TRUE;
    }
    if (cmdPendingCount >= SND_COMMAND_MAX_IN_FLIGHT) {
        if (!(flags & 1)) {
            OS_RestoreInterrupts(e);
            return FALSE;
        }
        do {
            SND_RecvCommandReply(1);
        } while (cmdPendingCount >= SND_COMMAND_MAX_IN_FLIGHT);
        if (cmdPendingHead == NULL) {
            OS_RestoreInterrupts(e);
            return TRUE;
        }
    }
    DC_FlushRange(cmdPool, sizeof(cmdPool));
    if (PXI_SendWordByFifo(SND_FIFO_TAG, (u32) cmdPendingHead, FALSE) < 0) {
        if (!(flags & 1)) {
            OS_RestoreInterrupts(e);
            return FALSE;
        }
        while (cmdPendingCount >= SND_COMMAND_MAX_IN_FLIGHT ||
               PXI_SendWordByFifo(SND_FIFO_TAG, (u32) cmdPendingHead, FALSE) < 0) {
            OS_RestoreInterrupts(e);
            SND_RecvCommandReply(0);
            e = OS_DisableInterrupts();
            DC_FlushRange(cmdPool, sizeof(cmdPool));
            if (cmdPendingHead == NULL) {
                OS_RestoreInterrupts(e);
                return TRUE;
            }
        }
    }
    {
        cmdRing[cmdRingWrite] = cmdPendingHead;
        cmdRingWrite++;
        if (cmdRingWrite > SND_COMMAND_RING_SIZE - 1) {
            cmdRingWrite = 0;
        }
    }
    cmdPendingHead = NULL;
    cmdPendingTail = NULL;
    cmdPendingCount++;
    cmdReplyCount++;
    OS_RestoreInterrupts(e);
    if (flags & 2) {
        func_0204091c();
    }
    return TRUE;
}

u32 SND_GetCurrentCommandTag(void) {
    OSIntrMode e = OS_DisableInterrupts();
    u32 tag;
    if (cmdPendingHead == NULL) {
        tag = cmdProcCount;
    } else {
        tag = cmdReplyCount;
    }
    OS_RestoreInterrupts(e);
    return tag;
}

BOOL SND_IsFinishedCommandTag(u32 tag) {
    OSIntrMode e = OS_DisableInterrupts();
    if (tag > cmdProcCount) {
        if ((tag - cmdProcCount) < 0x80000000) {
            tag = FALSE;
        } else {
            tag = TRUE;
        }
    } else {
        tag = (cmdProcCount - tag) < 0x80000000 ? TRUE : FALSE;
    }
    OS_RestoreInterrupts(e);
    return tag;
}

u32 SND_CountFreeCommand(void) {
    OSIntrMode e = OS_DisableInterrupts();
    u32 count = 0;
    SNDCommand *c = cmdFreeList;
    for (; c != NULL; c = c->next) {
        count++;
    }
    OS_RestoreInterrupts(e);
    return count;
}

u32 SND_CountWaitingCommand(void) {
    OSIntrMode e = OS_DisableInterrupts();
    u32 count = 0;
    SNDCommand *c = cmdPendingHead;
    for (; c != NULL; c = c->next) {
        count++;
    }
    OS_RestoreInterrupts(e);
    return count;
}

u32 func_0204087c(void) {
    u32 a = SND_CountFreeCommand();
    return SND_COMMAND_POOL_SIZE - a - SND_CountWaitingCommand();
}

void func_02040898(int tag, u32 data, BOOL err) {
    OSIntrMode e = OS_DisableInterrupts();
    func_02040a4c(data);
    OS_RestoreInterrupts(e);
}

void func_020408bc(void) {
    PXI_SetFifoRecvCallback(SND_FIFO_TAG, func_02040898);
    if (!func_0204098c()) {
        return;
    }
    if (PXI_IsCallbackReady(SND_FIFO_TAG, SND_PXI_PROC)) {
        return;
    }
    do {
        OS_SpinWait(100);
    } while (!PXI_IsCallbackReady(SND_FIFO_TAG, SND_PXI_PROC));
}

void func_0204091c(void) {
    while (PXI_SendWordByFifo(SND_FIFO_TAG, 0, FALSE) < 0) {
    }
}

SNDCommand *func_02040944(void) {
    OSIntrMode e = OS_DisableInterrupts();
    SNDCommand *cmd = cmdFreeList;
    if (cmd == NULL) {
        OS_RestoreInterrupts(e);
        return NULL;
    }
    {
        SNDCommand *next = cmd->next;
        cmdFreeList = next;
        if (next == NULL) {
            cmdFreeTail = NULL;
        }
    }
    OS_RestoreInterrupts(e);
    return cmd;
}

BOOL func_0204098c(void) {
    u32 v;
    OSIntrMode e;
    if (func_0203e25c() == 0) {
        return TRUE;
    }
    e = OS_DisableInterrupts();
    *(vu32 *) 0x4fff200 = 0x10;
    v = *(vu32 *) 0x4fff200;
    OS_RestoreInterrupts(e);
    return v != 0;
}
