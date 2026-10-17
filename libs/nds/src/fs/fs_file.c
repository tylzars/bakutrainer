#include <nitro/fs.h>

extern FSDirPos data_020cc6ac;

static int fs_initialized;

// Calls FS_Init once.
void func_020421d0(u32 dma) {
    if (fs_initialized) {
        return;
    }
    fs_initialized = 1;
    FS_Init(dma);
}

// Clears a file structure.
void func_020421f8(FSFile *file) {
    file->prev = NULL;
    file->next = NULL;
    file->queue.head = file->queue.tail = NULL;
    file->arc = NULL;
    file->command = FS_COMMAND_NONE;
    file->stat = 0;
}

// Starts a path lookup. The path may begin with an archive name and a colon, or with a slash; the file id or
// the directory position is stored in idOut or dirOut (one of them is NULL).
BOOL func_02042220(FSFile *file, const u8 *path, FSFileID *idOut, FSDirPos *dirOut) {
    FSDirPos pos;
    int i;
    if (path[0] == '/' || path[0] == '\\') {
        pos.idx = 0;
        pos.arc = data_020cc6ac.arc;
        pos.pos = 0;
        pos.sub = 0;
        path++;
    } else {
        pos = data_020cc6ac;
        for (i = 0; i <= 3; i++) {
            u32 c = path[i];
            if (c == 0 || c == '/' || c == '\\') {
                break;
            }
            if (c == ':') {
                FSArchive *arc = func_02041fc8((const char *) path, i);
                if (arc == NULL) {
                    return FALSE;
                }
                if (!FS_ARCHIVE_HAS(arc, FS_ARCHIVE_LOADED)) {
                    return FALSE;
                }
                pos.arc = arc;
                pos.pos = 0;
                pos.sub = 0;
                pos.idx = 0;
                path += i + 1;
                if (*path == '/' || *path == '\\') {
                    path++;
                }
                break;
            }
        }
    }
    file->arc = pos.arc;
    file->arg.find.path = path;
    file->arg.find.start = pos;
    if (dirOut != NULL) {
        file->arg.find.want_dir = 1;
        file->arg.find.result = dirOut;
    } else {
        file->arg.find.want_dir = 0;
        file->arg.find.result = idOut;
    }
    return func_02041df8(file, FS_COMMAND_FIND_PATH);
}

// Reads up to len bytes at the file position. Waits for completion unless async is set; returns the byte count,
// or -1 when the read failed.
int func_02042364(FSFile *file, void *buf, int len, BOOL async) {
    u32 pos = file->prop.file.pos;
    int remain = file->prop.file.bottom - pos;
    int requested = len;
    file->arg.io.buffer = buf;
    if (len > remain) {
        len = remain;
    }
    if (len < 0) {
        len = 0;
    }
    file->arg.io.length = requested;
    file->arg.io.clamped_length = len;
    if (!async) {
        file->stat |= FS_FILE_SYNC;
    }
    func_02041df8(file, FS_COMMAND_READ);
    if (!async) {
        if (func_02042514(file)) {
            len = file->prop.file.pos - pos;
        } else {
            len = -1;
        }
    }
    return len;
}

// Looks a file path up and stores its file id.
BOOL FS_ConvertPathToFileID(FSFileID *id, const char *path) {
    FSFile file;
    func_020421f8(&file);
    return func_02042220(&file, (const u8 *) path, id, NULL) ? TRUE : FALSE;
}

// Opens a file by file id.
BOOL func_02042420(FSFile *file, FSFileID id) {
    FSArchive *arc = id.arc;
    if (arc == NULL) {
        return FALSE;
    }
    file->arc = arc;
    file->arg.open_by_id = id;
    if (!func_02041df8(file, FS_COMMAND_OPEN_BY_ID)) {
        return FALSE;
    }
    file->stat = (file->stat | FS_FILE_IS_FILE) & ~FS_FILE_IS_DIR;
    return TRUE;
}

// Opens a file by path.
BOOL func_02042490(FSFile *file, const char *path) {
    FSFileID id;
    if (FS_ConvertPathToFileID(&id, path) && func_02042420(file, id)) {
        return TRUE;
    }
    return FALSE;
}

// Closes a file.
BOOL func_020424d8(FSFile *file) {
    if (!func_02041df8(file, FS_COMMAND_CLOSE)) {
        return FALSE;
    }
    file->arc = NULL;
    file->command = FS_COMMAND_NONE;
    file->stat &= ~(FS_FILE_IS_FILE | FS_FILE_IS_DIR);
    return TRUE;
}

// Waits until the file's pending command is done; returns whether it succeeded.
BOOL func_02042514(FSFile *file) {
    BOOL sync = FALSE;
    OSIntrMode intr = OS_DisableInterrupts();
    if (FS_FILE_HAS(file, FS_FILE_BUSY)) {
        sync = !(file->stat & (FS_FILE_STARTED | FS_FILE_SYNC));
        if (sync) {
            file->stat |= FS_FILE_SYNC;
            do {
                OS_SleepThread(&file->queue);
            } while (!(file->stat & FS_FILE_STARTED));
        } else {
            do {
                OS_SleepThread(&file->queue);
            } while (FS_FILE_HAS(file, FS_FILE_BUSY));
        }
    }
    OS_RestoreInterrupts(intr);
    if (sync) {
        return func_02041db4(file);
    }
    return file->error == 0;
}

// Starts an asynchronous read.
int func_020425d4(FSFile *file, void *buf, int len) {
    return func_02042364(file, buf, len, TRUE);
}

// Reads and waits.
int func_020425e4(FSFile *file, void *buf, int len) {
    return func_02042364(file, buf, len, FALSE);
}

// Moves the file position relative to the start (0), the current position (1) or the end (2) of the file, clamped
// to the file; returns FALSE for another origin.
BOOL func_020425f4(FSFile *file, int offset, int origin) {
    switch (origin) {
        case 0:
            offset += file->prop.file.top;
            break;
        case 1:
            offset += file->prop.file.pos;
            break;
        case 2:
            offset += file->prop.file.bottom;
            break;
        default:
            return FALSE;
    }
    if (offset < (int) file->prop.file.top) {
        offset = file->prop.file.top;
    }
    if (offset > (int) file->prop.file.bottom) {
        offset = file->prop.file.bottom;
    }
    file->prop.file.pos = offset;
    return TRUE;
}
