#include <nitro/fs.h>
#include <nitro/mi.h>

// Built-in handlers by command number.
static int (*const data_020637d8[])(FSFile *file) = {
    func_0204121c, func_02041248, func_02041274, func_0204130c, func_02041438,
    func_0204161c, func_020419c8, func_02041a48, func_02041a70,
};

// Finishes a command: unlinks the file from its archive's queue, clears its run flags, stores the result and wakes the
// threads waiting on it.
void func_02040eec(FSFile *file, int result) {
    OSIntrMode intr = OS_DisableInterrupts();
    FSFile *prev = file->prev;
    FSFile *next = file->next;
    if (prev != NULL) {
        prev->next = next;
    }
    if (next != NULL) {
        next->prev = prev;
    }
    file->prev = NULL;
    file->next = NULL;
    file->stat &= ~FS_FILE_DONE_MASK;
    file->error = result;
    OS_WakeupThread(&file->queue);
    OS_RestoreInterrupts(intr);
}

// Runs a command. The archive's callback gets the first go; the built-in handler for the command number runs when it
// answers FS_RESULT_DEFAULT or does not take that command. A synchronous caller waits here for an asynchronous handler
// to finish; otherwise the command is finished here.
int func_02040f48(FSFile *file, int command) {
    int result;
    FSArchive *arc = file->arc;
    u32 bit = 1 << command;
    BOOL isSync = FS_FILE_HAS(file, FS_FILE_SYNC);
    u32 flags = arc->flags;
    if (isSync != 0) {
        flags |= FS_ARCHIVE_SYNC_BUSY;
    } else {
        flags |= FS_ARCHIVE_ASYNC_BUSY;
    }
    arc->flags = flags;
    if (arc->proc_mask & bit) {
        result = arc->proc(file, command);
        switch (result) {
            case FS_RESULT_OK:
            case FS_RESULT_FAILURE:
            case FS_RESULT_UNSUPPORTED:
                file->error = result;
                break;
            case FS_RESULT_DEFAULT_ALWAYS:
                arc->proc_mask &= ~bit;
                result = FS_RESULT_DEFAULT;
                break;
            default:
                break;
        }
    } else {
        result = FS_RESULT_DEFAULT;
    }
    if (result == FS_RESULT_DEFAULT) {
        result = data_020637d8[command](file);
    }
    if (result == FS_RESULT_ASYNC) {
        if (FS_FILE_HAS(file, FS_FILE_SYNC) != 0) {
            OSIntrMode intr = OS_DisableInterrupts();
            if (FS_ARCHIVE_HAS(arc, FS_ARCHIVE_SYNC_BUSY) != 0) {
                do {
                    OS_SleepThread(&arc->q0);
                } while (FS_ARCHIVE_HAS(arc, FS_ARCHIVE_SYNC_BUSY) != 0);
            }
            result = file->error;
            OS_RestoreInterrupts(intr);
        }
    } else {
        if (FS_FILE_HAS(file, FS_FILE_SYNC) == 0) {
            arc->flags &= ~FS_ARCHIVE_ASYNC_BUSY;
            func_02040eec(file, result);
        } else {
            arc->flags &= ~FS_ARCHIVE_SYNC_BUSY;
            file->error = result;
        }
    }
    return result;
}

// Compares n characters, ignoring ASCII case; returns the difference of the first pair that differs.
int func_020410d8(const u8 *a, const u8 *b, u32 n) {
    u32 i;
    for (i = 0; i < n; i++) {
        int x = a[i] - 'A';
        int y = b[i] - 'A';
        if ((u32) x <= 25) {
            x += 32;
        }
        if ((u32) y <= 25) {
            y += 32;
        }
        if (x != y) {
            return x - y;
        }
    }
    return 0;
}

// Reads len bytes of an archive table at tablePos through the archive's current read function (waiting for an
// asynchronous one) and advances the position.
int func_02041128(FSTablePos *tablePos, void *dst, u32 len) {
    FSArchive *arc = tablePos->arc;
    int result;
    arc->flags |= FS_ARCHIVE_SYNC_BUSY;
    result = arc->read_cur(arc, dst, tablePos->pos, len);
    switch (result) {
        case FS_RESULT_OK:
        case FS_RESULT_FAILURE:
            arc->flags &= ~FS_ARCHIVE_SYNC_BUSY;
            break;
        case FS_RESULT_ASYNC: {
            OSIntrMode intr = OS_DisableInterrupts();
            if (FS_ARCHIVE_HAS(arc, FS_ARCHIVE_SYNC_BUSY) != 0) {
                do {
                    OS_SleepThread(&arc->q0);
                } while (FS_ARCHIVE_HAS(arc, FS_ARCHIVE_SYNC_BUSY) != 0);
            }
            OS_RestoreInterrupts(intr);
            result = arc->list.next->error;
            break;
        }
    }
    tablePos->pos += len;
    return result;
}

// Moves the file to the directory with the given index (command FS_COMMAND_SEEK_DIR).
int func_020411e8(FSFile *file, u32 dirId) {
    file->stat |= FS_FILE_SYNC;
    file->arg.seek.arc = file->arc;
    file->arg.seek.pos = 0;
    file->arg.seek.sub = 0;
    file->arg.seek.idx = dirId;
    return func_02040f48(file, FS_COMMAND_SEEK_DIR);
}

// Built-in handler of FS_COMMAND_READ: reads the clamped length at the file position.
int func_0204121c(FSFile *file) {
    u32 pos = file->prop.file.pos;
    u32 len = file->arg.io.clamped_length;
    FSArchive *arc = file->arc;
    void *buffer = file->arg.io.buffer;
    file->prop.file.pos = pos + len;
    return arc->read_func(arc, buffer, pos, len);
}

// Built-in handler of FS_COMMAND_WRITE.
int func_02041248(FSFile *file) {
    u32 pos = file->prop.file.pos;
    u32 len = file->arg.io.clamped_length;
    FSArchive *arc = file->arc;
    void *buffer = file->arg.io.buffer;
    file->prop.file.pos = pos + len;
    return arc->write_func(arc, buffer, pos, len);
}

// Built-in handler of FS_COMMAND_SEEK_DIR: reads the directory's table entry and makes it the current directory.
int func_02041274(FSFile *file) {
    FSArchive *arc = file->arc;
    struct {
        u32 nameOffset;
        u16 firstFile;
        u16 parent;
    } entry;
    FSTablePos tablePos;
    FSDirPos *dirPos = &file->arg.seek;
    int result;
    tablePos.arc = arc;
    tablePos.pos = arc->fnt + dirPos->idx * 8;
    result = func_02041128(&tablePos, &entry, 8);
    if (result == 0) {
        file->prop.dir.pos = *dirPos;
        if (dirPos->sub == 0 && dirPos->pos == 0) {
            file->prop.dir.pos.sub = entry.firstFile;
            file->prop.dir.pos.pos = arc->fnt + entry.nameOffset;
        }
        file->prop.dir.parent = entry.parent & 0xfff;
    }
    return result;
}

// Built-in handler of FS_COMMAND_READ_DIR: reads the next entry of the current directory into the entry buffer.
// Returns FS_RESULT_FAILURE at the end of the directory.
int func_0204130c(FSFile *file) {
    FSDirEntry *entry = file->arg.read_dir.entry;
    u8 header;
    u16 dirId;
    FSTablePos tablePos;
    int result;
    tablePos.arc = file->arc;
    tablePos.pos = file->prop.dir.pos.pos;
    result = func_02041128(&tablePos, &header, 1);
    if (result != 0) {
        return result;
    }
    entry->name_len = header & 0x7f;
    entry->is_dir = (header >> 7) & 1;
    if (entry->name_len == 0) {
        return FS_RESULT_FAILURE;
    }
    if (file->arg.read_dir.skip_name == 0) {
        result = func_02041128(&tablePos, entry->name, entry->name_len);
        if (result != 0) {
            return result;
        }
        entry->name[entry->name_len] = 0;
    } else {
        tablePos.pos += entry->name_len;
    }
    if (entry->is_dir != 0) {
        result = func_02041128(&tablePos, &dirId, 2);
        if (result != 0) {
            return result;
        }
        entry->id.dir.arc = file->arc;
        entry->id.dir.idx = dirId & 0xfff;
        entry->id.dir.sub = 0;
        entry->id.dir.pos = 0;
    } else {
        entry->id.file.arc = file->arc;
        entry->id.file.file_id = file->prop.dir.pos.sub;
        file->prop.dir.pos.sub++;
    }
    file->prop.dir.pos.pos = tablePos.pos;
    return result;
}

static inline BOOL IsPathNameChar(u32 c) {
    BOOL result = FALSE;
    if (c != 0 && c != '/' && c != '\\') {
        result = TRUE;
    }
    return result;
}

// Built-in handler of FS_COMMAND_FIND_PATH: walks a path (with "." and ".." parts, separated by / or \) from the start
// directory and stores the file id, or the directory position when a directory is wanted.
int func_02041438(FSFile *file) {
    FSDirEntry entry;
    const u8 *path = file->arg.find.path;
    u32 wantDir = file->arg.find.want_dir;
    func_02040f48(file, FS_COMMAND_SEEK_DIR);
    while (*path != 0) {
        int len = 0;
        u32 terminator;
        while (IsPathNameChar(terminator = path[len])) {
            len++;
        }
        if (terminator != 0 || wantDir != 0) {
            terminator = 1;
        }
        if (len == 0) {
            return FS_RESULT_FAILURE;
        }
        if (*path == '.') {
            if (len == 1) {
                path += 1;
                goto next;
            }
            if ((len == 2) & (path[1] == '.')) {
                if (file->prop.dir.pos.idx != 0) {
                    func_020411e8(file, file->prop.dir.parent);
                }
                path += 2;
                goto next;
            }
        }
        if (len > FS_NAME_MAX) {
            return FS_RESULT_FAILURE;
        }
        file->arg.read_dir.entry = &entry;
        file->arg.read_dir.skip_name = 0;
        do {
            if (func_02040f48(file, FS_COMMAND_READ_DIR) != 0) {
                return FS_RESULT_FAILURE;
            }
        } while (terminator != entry.is_dir || len != entry.name_len ||
                 func_020410d8(path, (u8 *) entry.name, len) != 0);
        if (terminator != 0) {
            file->arg.seek = entry.id.dir;
            path += len;
            func_02040f48(file, FS_COMMAND_SEEK_DIR);
        } else {
            if (wantDir != 0) {
                return FS_RESULT_FAILURE;
            }
            *(FSFileID *) file->arg.find.result = entry.id.file;
            return FS_RESULT_OK;
        }
    next:
        path += (*path != 0);
    }
    if (wantDir == 0) {
        return FS_RESULT_FAILURE;
    }
    *(FSDirPos *) file->arg.find.result = file->prop.dir.pos;
    return FS_RESULT_OK;
}

static char data_020644bc[] = ":/";

// Stands for "no directory": the object a path is built for is a directory itself, or it was not found.
#define FS_NO_DIR 0x10000

// Built-in handler of FS_COMMAND_GET_PATH: builds the archive-qualified path of the open file or directory
// into the buffer and records its length; with no buffer it only measures.
int func_0204161c(FSFile *file) {
    FSArchive *arc = file->arc;
    FSDirEntry entry;
    FSFile tmp;
    int pos;
    u8 *buf;
    u32 target;
    int result;
    u32 i;
    u32 dirCount;
    u32 j;
    int len;
    u32 dir;
    func_020421f8(&tmp);
    tmp.arc = file->arc;
    if (FS_FILE_HAS(file, FS_FILE_IS_DIR)) {
        dir = file->prop.dir.pos.idx;
        target = FS_NO_DIR;
    } else {
        target = file->prop.file.own_id;
        if (file->arg.get_path.total_length != 0) {
            dir = file->arg.get_path.dir_id;
        } else {
            i = 0;
            dirCount = 0;
            dir = FS_NO_DIR;
            do {
                func_020411e8(&tmp, i);
                if (i == 0) {
                    // The root directory's parent field holds the number of directories.
                    dirCount = tmp.prop.dir.parent;
                }
                tmp.arg.read_dir.entry = &entry;
                tmp.arg.read_dir.skip_name = 1;
                if (func_02040f48(&tmp, FS_COMMAND_READ_DIR) == 0) {
                    do {
                        if (entry.is_dir == 0 && entry.id.file.file_id == target) {
                            dir = tmp.prop.dir.pos.idx;
                            break;
                        }
                    } while (func_02040f48(&tmp, FS_COMMAND_READ_DIR) == 0);
                }
                if (dir != FS_NO_DIR) {
                    break;
                }
                i++;
            } while (i < dirCount);
        }
    }
    if (dir == FS_NO_DIR) {
        file->arg.get_path.total_length = 0;
        return FS_RESULT_FAILURE;
    }
    if (file->arg.get_path.total_length == 0) {
        u32 name;
        len = 0;
        name = arc->name;
        if (name <= 0xff) {
            len += 1;
        } else if (name <= 0xff00) {
            len += 2;
        } else {
            len += 3;
        }
        len += 2;
        if (target != FS_NO_DIR) {
            len += entry.name_len;
        }
        j = dir;
        if (j != 0) {
            func_020411e8(&tmp, dir);
            do {
                func_020411e8(&tmp, tmp.prop.dir.parent);
                tmp.arg.read_dir.entry = &entry;
                tmp.arg.read_dir.skip_name = 1;
                if (func_02040f48(&tmp, FS_COMMAND_READ_DIR) == 0) {
                    do {
                        if (entry.is_dir != 0 && entry.id.dir.idx == j) {
                            len += entry.name_len + 1;
                            break;
                        }
                    } while (func_02040f48(&tmp, FS_COMMAND_READ_DIR) == 0);
                }
                j = tmp.prop.dir.pos.idx;
            } while (j != 0);
        }
        file->arg.get_path.total_length = len + 1;
        file->arg.get_path.dir_id = dir;
    }
    buf = file->arg.get_path.buffer;
    if (buf == NULL) {
        return FS_RESULT_OK;
    }
    pos = file->arg.get_path.total_length;
    if (file->arg.get_path.buffer_size < pos) {
        return FS_RESULT_FAILURE;
    }
    {
        u32 name = arc->name;
        u32 off = 0;
        if (name <= 0xff) {
            len = 1;
        } else if (name <= 0xff00) {
            len = 2;
        } else {
            len = 3;
        }
        MI_CpuCopy8(&arc->name, buf, len);
        off += len;
        MI_CpuCopy8(data_020644bc, buf + off, 2);
    }
    func_020411e8(&tmp, dir);
    if (target != FS_NO_DIR) {
        tmp.arg.read_dir.entry = &entry;
        tmp.arg.read_dir.skip_name = 0;
        if (func_02040f48(&tmp, FS_COMMAND_READ_DIR) == 0) {
            do {
                if (entry.is_dir == 0 && entry.id.file.file_id == target) {
                    break;
                }
            } while (func_02040f48(&tmp, FS_COMMAND_READ_DIR) == 0);
        }
        result = entry.name_len + 1;
        MI_CpuCopy8(entry.name, buf + pos - result, result);
        pos -= result;
    } else {
        *(buf + pos - 1) = 0;
        pos -= 1;
    }
    while (dir != 0) {
        func_020411e8(&tmp, tmp.prop.dir.parent);
        tmp.arg.read_dir.entry = &entry;
        tmp.arg.read_dir.skip_name = 0;
        *(buf + pos - 1) = '/';
        pos--;
        if (func_02040f48(&tmp, FS_COMMAND_READ_DIR) == 0) {
            do {
                if (entry.is_dir != 0 && entry.id.dir.idx == dir) {
                    dir = entry.name_len;
                    MI_CpuCopy8(entry.name, buf + pos - dir, dir);
                    pos -= dir;
                    break;
                }
            } while (func_02040f48(&tmp, FS_COMMAND_READ_DIR) == 0);
        }
        dir = tmp.prop.dir.pos.idx;
    }
    return FS_RESULT_OK;
}

// Built-in handler of FS_COMMAND_OPEN_BY_ID: reads the file's allocation table entry and opens that range.
int func_020419c8(FSFile *file) {
    FSArchive *arc = file->arc;
    u32 id = file->arg.open_by_id.file_id;
    u32 pos = id * 8;
    u32 entry[2];
    FSTablePos tablePos;
    int result;
    if (pos >= arc->fat_size) {
        return FS_RESULT_FAILURE;
    }
    tablePos.arc = arc;
    tablePos.pos = arc->fat + pos;
    result = func_02041128(&tablePos, entry, 8);
    if (result != 0) {
        return result;
    }
    file->arg.open_range.top = entry[0];
    file->arg.open_range.bottom = entry[1];
    file->arg.open_range.file_id = id;
    return func_02040f48(file, FS_COMMAND_OPEN_RANGE);
}

// Built-in handler of FS_COMMAND_OPEN_RANGE: positions the file on its byte range.
int func_02041a48(FSFile *file) {
    file->prop.file.top = file->arg.open_range.top;
    file->prop.file.pos = file->arg.open_range.top;
    file->prop.file.bottom = file->arg.open_range.bottom;
    file->prop.file.own_id = file->arg.open_range.file_id;
    return FS_RESULT_OK;
}

// Built-in handler of FS_COMMAND_CLOSE.
int func_02041a70(FSFile *file) {
    return FS_RESULT_OK;
}
