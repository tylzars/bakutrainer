#include <nitro/fs.h>
#include <nitro/mi.h>

FSArchive *data_020cc6a8;
FSDirPos data_020cc6ac;

// Packs up to three characters of an archive name into one word, folding the case of ASCII letters.
u32 func_02041a78(const u8 *name, int len) {
    u32 packed = 0;
    int i, shift;
    if (len <= 3) {
        i = 0;
        if (len > 0) {
            shift = 0;
            do {
                u32 c = name[i];
                u32 d;
                if (c == 0) {
                    break;
                }
                d = c - 'A';
                if (d <= 25) {
                    c = d + 'a';
                } else {
                    c = d + 'A';
                }
                i++;
                packed |= c << shift;
                shift += 8;
            } while (i < len);
        }
    }
    return packed;
}

// Default read function: copies from memory at the archive base.
int func_02041ad0(FSArchive *arc, void *dst, u32 pos, u32 len) {
    MI_CpuCopy8((u8 *) arc->base + pos, dst, len);
    return 0;
}

// Default write function: copies to memory at the archive base.
int func_02041aec(FSArchive *arc, void *src, u32 pos, u32 len) {
    MI_CpuCopy8(src, (u8 *) arc->base + pos, len);
    return 0;
}

// Picks the next command to run on the archive: ends canceled commands, and returns the file at the head of the
// queue (or NULL when it is a synchronous caller, which is woken, or when nothing can start). Sends the start and end
// events to the archive's callback.
FSFile *func_02041b0c(FSArchive *arc) {
    OSIntrMode intr = OS_DisableInterrupts();
    FSFile *file, *next;
    if (FS_ARCHIVE_HAS(arc, FS_ARCHIVE_CANCEL_PENDING) != 0) {
        arc->flags &= ~FS_ARCHIVE_CANCEL_PENDING;
        file = arc->list.next;
        if (file != NULL) {
            do {
                next = file->next;
                if (FS_FILE_HAS(file, FS_FILE_CANCEL_REQUESTED) != 0) {
                    if (arc->list.next == file) {
                        arc->list.next = next;
                    }
                    func_02040eec(file, FS_RESULT_CANCELED);
                    if (next == NULL) {
                        next = arc->list.next;
                    }
                }
                file = next;
            } while (file != NULL);
        }
    }
    if (FS_ARCHIVE_HAS(arc, FS_ARCHIVE_STOP_REQUESTED) == 0 && FS_ARCHIVE_HAS(arc, FS_ARCHIVE_STOPPED) == 0 &&
        (file = arc->list.next) != NULL) {
        BOOL first = FS_ARCHIVE_HAS(arc, FS_ARCHIVE_RUNNING) == 0;
        if (first) {
            arc->flags |= FS_ARCHIVE_RUNNING;
        }
        OS_RestoreInterrupts(intr);
        if (first && (arc->proc_mask & (1 << FS_EVENT_START))) {
            arc->proc(file, FS_EVENT_START);
        }
        intr = OS_DisableInterrupts();
        file->stat |= FS_FILE_STARTED;
        if (FS_FILE_HAS(file, FS_FILE_SYNC) != 0) {
            OS_WakeupThread(&file->queue);
            OS_RestoreInterrupts(intr);
            return NULL;
        }
        OS_RestoreInterrupts(intr);
        return file;
    }
    if (FS_ARCHIVE_HAS(arc, FS_ARCHIVE_RUNNING) != 0) {
        arc->flags &= ~FS_ARCHIVE_RUNNING;
        if (arc->proc_mask & (1 << FS_EVENT_END)) {
            FSFile tmp;
            func_020421f8(&tmp);
            tmp.arc = arc;
            arc->proc(&tmp, FS_EVENT_END);
        }
    }
    if (FS_ARCHIVE_HAS(arc, FS_ARCHIVE_STOP_REQUESTED) != 0) {
        arc->flags = (arc->flags & ~FS_ARCHIVE_STOP_REQUESTED) | FS_ARCHIVE_STOPPED;
        OS_WakeupThread(&arc->q1);
    }
    OS_RestoreInterrupts(intr);
    return NULL;
}

// Runs queued commands one after another until one is left pending or the queue is empty.
void func_02041d20(FSFile *file) {
    FSArchive *arc = file->arc;
    OSIntrMode intr;
    if (file == NULL) {
        return;
    }
    do {
        intr = OS_DisableInterrupts();
        file->stat |= FS_FILE_STARTED;
        if (FS_FILE_HAS(file, FS_FILE_SYNC) != 0) {
            OS_WakeupThread(&file->queue);
            OS_RestoreInterrupts(intr);
            return;
        }
        file->stat |= FS_FILE_EXECUTING;
        OS_RestoreInterrupts(intr);
        if (func_02040f48(file, file->command) == FS_RESULT_ASYNC) {
            return;
        }
        file = func_02041b0c(arc);
    } while (file != NULL);
}

// Runs the queued command of a synchronous caller to completion and starts the next one.
BOOL func_02041db4(FSFile *file) {
    FSFile *next;
    func_02040eec(file, func_02040f48(file, file->command));
    next = func_02041b0c(file->arc);
    if (next != NULL) {
        func_02041d20(next);
    }
    return file->error == 0;
}

// Queues a command on the file's archive and runs it when the archive is idle. Synchronous commands return
// after completion.
BOOL func_02041df8(FSFile *file, u32 command) {
    FSArchive *arc = file->arc;
    u32 bit = 1 << command;
    OSIntrMode intr;
    FSFile *tail;
    FSFile *prev;
    FSFile *next;
    file->command = command;
    file->error = FS_RESULT_IN_PROGRESS;
    file->stat |= FS_FILE_BUSY;
    intr = OS_DisableInterrupts();
    if (arc->flags & FS_ARCHIVE_CLOSED) {
        func_02040eec(file, FS_RESULT_CANCELED);
        OS_RestoreInterrupts(intr);
        return FALSE;
    }
    if (bit & FS_COMMANDS_SYNC_MASK) {
        file->stat |= FS_FILE_SYNC;
    }
    tail = (FSFile *) &arc->list;
    prev = file->prev;
    next = file->next;
    if (prev != NULL) {
        prev->next = next;
    }
    if (next != NULL) {
        next->prev = prev;
    }
    while (tail->next != NULL) {
        tail = tail->next;
    }
    tail->next = file;
    file->prev = tail;
    file->next = NULL;
    if (FS_ARCHIVE_HAS(arc, FS_ARCHIVE_STOPPED) == 0 && FS_ARCHIVE_HAS(arc, FS_ARCHIVE_RUNNING) == 0) {
        arc->flags |= FS_ARCHIVE_RUNNING;
        OS_RestoreInterrupts(intr);
        if (arc->proc_mask & (1 << FS_EVENT_START)) {
            arc->proc(file, FS_EVENT_START);
        }
        intr = OS_DisableInterrupts();
        file->stat |= FS_FILE_STARTED;
        if (FS_FILE_HAS(file, FS_FILE_SYNC) == 0) {
            OS_RestoreInterrupts(intr);
            func_02041d20(file);
            return TRUE;
        }
        OS_RestoreInterrupts(intr);
    } else {
        if (FS_FILE_HAS(file, FS_FILE_SYNC) == 0) {
            OS_RestoreInterrupts(intr);
            return TRUE;
        }
        do {
            OS_SleepThread(&file->queue);
        } while (!(file->stat & FS_FILE_STARTED));
        OS_RestoreInterrupts(intr);
    }
    return func_02041db4(file);
}

// Clears an archive.
void func_02041f9c(FSArchive *arc) {
    MI_CpuFill8(arc, 0, sizeof(FSArchive));
    arc->q0.head = arc->q0.tail = NULL;
    arc->q1.head = arc->q1.tail = NULL;
}

// Finds a registered archive by name.
FSArchive *func_02041fc8(const char *name, int len) {
    u32 packed = func_02041a78((const u8 *) name, len);
    OSIntrMode intr = OS_DisableInterrupts();
    FSArchive *arc = data_020cc6a8;
    while (arc != NULL && arc->name != packed) {
        arc = arc->next;
    }
    OS_RestoreInterrupts(intr);
    return arc;
}

// Registers an archive under a name of up to three characters; the first one registered also sets the current
// directory.
BOOL func_02042008(FSArchive *arc, const char *name, int len) {
    BOOL registered = FALSE;
    OSIntrMode intr = OS_DisableInterrupts();
    if (func_02041fc8(name, len) == NULL) {
        if (data_020cc6a8 == NULL) {
            data_020cc6a8 = arc;
            data_020cc6ac.arc = arc;
            data_020cc6ac.pos = 0;
            data_020cc6ac.sub = 0;
            data_020cc6ac.idx = 0;
        } else {
            FSArchive *last = data_020cc6a8;
            while (last->next != NULL) {
                last = last->next;
            }
            last->next = arc;
            arc->prev = last;
        }
        arc->name = func_02041a78((const u8 *) name, len);
        arc->flags |= FS_ARCHIVE_REGISTERED;
        registered = TRUE;
    }
    OS_RestoreInterrupts(intr);
    return registered;
}

// Sets the archive's table positions and read and write functions (memory copies when none is given).
BOOL func_020420bc(FSArchive *arc, u32 base, u32 fat, u32 fatSize, u32 fnt, u32 fntSize, FSReadFunc read,
                   FSReadFunc write) {
    arc->base = base;
    arc->fat_size = fatSize;
    arc->fat = arc->fat_bak = fat;
    arc->fnt_size = fntSize;
    arc->fnt = arc->fnt_bak = fnt;
    arc->read_func = read != NULL ? read : func_02041ad0;
    arc->write_func = write != NULL ? write : func_02041aec;
    arc->read_cur = arc->read_func;
    arc->load_mem = 0;
    arc->flags |= FS_ARCHIVE_LOADED;
    return TRUE;
}

// Sets the archive's callback and the mask of commands and events it handles.
void func_0204212c(FSArchive *arc, FSProc proc, u32 mask) {
    if (mask == 0) {
        proc = NULL;
    } else if (proc == NULL) {
        mask = 0;
    }
    arc->proc = proc;
    arc->proc_mask = mask;
}

// Called when an archive's asynchronous operation is done: finishes the command, or wakes the thread waiting for a
// table read.
void func_0204214c(FSArchive *arc, int result) {
    FSFile *file, *next;
    OSIntrMode intr;
    if (FS_ARCHIVE_HAS(arc, FS_ARCHIVE_ASYNC_BUSY) != 0) {
        FSFile *current = arc->list.next;
        arc->flags &= ~FS_ARCHIVE_ASYNC_BUSY;
        func_02040eec(current, result);
        next = func_02041b0c(arc);
        if (next != NULL) {
            func_02041d20(next);
        }
    } else {
        file = arc->list.next;
        intr = OS_DisableInterrupts();
        file->error = result;
        arc->flags &= ~FS_ARCHIVE_SYNC_BUSY;
        OS_WakeupThread(&arc->q0);
        OS_RestoreInterrupts(intr);
    }
}
