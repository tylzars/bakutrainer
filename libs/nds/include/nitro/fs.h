#pragma once

#include "types.h"
#include <nitro/os.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FSFile FSFile;
typedef struct FSArchive FSArchive;

// A file id inside an archive.
typedef struct {
    /* 00 */ FSArchive *arc;
    /* 04 */ u32 file_id;
} FSFileID;

// A position in a directory listing: the directory number, the number of the next file and the byte offset of the
// next name in the name table.
typedef struct {
    /* 00 */ FSArchive *arc;
    /* 04 */ u16 idx;
    /* 06 */ u16 sub;
    /* 08 */ u32 pos;
} FSDirPos;

// A byte offset in an archive's tables.
typedef struct {
    /* 00 */ FSArchive *arc;
    /* 04 */ u32 pos;
} FSTablePos;

// One entry of a directory listing: a file (arc, id) or a subdirectory (arc, idx, sub, pos) with its name.
#define FS_NAME_MAX 0x7f

typedef struct {
    union {
        /* 00 */ FSFileID file;
        /* 00 */ FSDirPos dir;
    } id;
    /* 0c */ u32 is_dir;
    /* 10 */ u32 name_len;
    /* 14 */ char name[FS_NAME_MAX + 1];
} FSDirEntry; /* 94 */

typedef int (*FSReadFunc)(FSArchive *arc, void *dst, u32 pos, u32 len);
typedef int (*FSProc)(void *file, int event);

// Archive flags.
#define FS_ARCHIVE_REGISTERED 0x001
#define FS_ARCHIVE_LOADED 0x002
#define FS_ARCHIVE_STOPPED 0x008
#define FS_ARCHIVE_RUNNING 0x010
#define FS_ARCHIVE_CANCEL_PENDING 0x020
#define FS_ARCHIVE_STOP_REQUESTED 0x040
#define FS_ARCHIVE_CLOSED 0x080
#define FS_ARCHIVE_ASYNC_BUSY 0x100
#define FS_ARCHIVE_SYNC_BUSY 0x200

// File flags.
#define FS_FILE_BUSY 0x01
#define FS_FILE_CANCEL_REQUESTED 0x02
#define FS_FILE_SYNC 0x04
#define FS_FILE_EXECUTING 0x08
#define FS_FILE_IS_FILE 0x10
#define FS_FILE_IS_DIR 0x20
#define FS_FILE_STARTED 0x40
// The flags a finished command clears.
#define FS_FILE_DONE_MASK (FS_FILE_BUSY | FS_FILE_CANCEL_REQUESTED | FS_FILE_SYNC | FS_FILE_EXECUTING | FS_FILE_STARTED)

// Commands.
#define FS_COMMAND_READ 0
#define FS_COMMAND_WRITE 1
#define FS_COMMAND_SEEK_DIR 2
#define FS_COMMAND_READ_DIR 3
#define FS_COMMAND_FIND_PATH 4
#define FS_COMMAND_GET_PATH 5
#define FS_COMMAND_OPEN_BY_ID 6
#define FS_COMMAND_OPEN_RANGE 7
#define FS_COMMAND_CLOSE 8
#define FS_COMMAND_NONE 0xe

// Commands 2 to 8 are always run to completion before the caller continues.
#define FS_COMMANDS_SYNC_MASK 0x1fc

// Events handed to an archive's callback in addition to the commands: a command sequence starts or ends.
#define FS_EVENT_START 9
#define FS_EVENT_END 10

// Results of a command. An archive's callback answers FS_RESULT_DEFAULT to leave the command to the built-in
// handler, or FS_RESULT_DEFAULT_ALWAYS to leave it to the built-in handler from now on.
#define FS_RESULT_OK 0
#define FS_RESULT_FAILURE 1
#define FS_RESULT_IN_PROGRESS 2
#define FS_RESULT_CANCELED 3
#define FS_RESULT_UNSUPPORTED 4
#define FS_RESULT_ERROR 5
#define FS_RESULT_ASYNC 6
#define FS_RESULT_DEFAULT 7
#define FS_RESULT_DEFAULT_ALWAYS 8

struct FSArchive {
    /* 00 */ u32 name;
    /* 04 */ FSArchive *next;
    /* 08 */ FSArchive *prev;
    /* 0c */ OSThreadQueue q0;
    /* 14 */ OSThreadQueue q1;
    /* 1c */ vu32 flags;
    /* 20 */ struct {
        FSFile *prev;
        FSFile *next;
    } list;
    /* 28 */ u32 base;
    /* 2c */ u32 fat;
    /* 30 */ u32 fat_size;
    /* 34 */ u32 fnt;
    /* 38 */ u32 fnt_size;
    /* 3c */ u32 fat_bak;
    /* 40 */ u32 fnt_bak;
    /* 44 */ u32 load_mem;
    /* 48 */ FSReadFunc read_func;
    /* 4c */ FSReadFunc write_func;
    /* 50 */ FSReadFunc read_cur;
    /* 54 */ FSProc proc;
    /* 58 */ u32 proc_mask;
}; /* 5c */

struct FSFile {
    /* 00 */ FSFile *prev;
    /* 04 */ FSFile *next;
    /* 08 */ FSArchive *arc;
    /* 0c */ vu32 stat;
    /* 10 */ u32 command;
    /* 14 */ u32 error;
    /* 18 */ OSThreadQueue queue;
    // What the file or directory is positioned at.
    union {
        struct {
            /* 20 */ u32 own_id;
            /* 24 */ u32 top;
            /* 28 */ u32 bottom;
            /* 2c */ u32 pos;
        } file;
        struct {
            /* 20 */ FSDirPos pos;
            /* 2c */ u32 parent;
        } dir;
    } prop;
    // Arguments of the command being run.
    union {
        struct {
            /* 30 */ void *buffer;
            /* 34 */ u32 length;
            /* 38 */ u32 clamped_length;
        } io;
        /* 30 */ FSDirPos seek;
        struct {
            /* 30 */ FSDirEntry *entry;
            /* 34 */ u32 skip_name;
        } read_dir;
        struct {
            /* 30 */ FSDirPos start;
            /* 3c */ const u8 *path;
            /* 40 */ u32 want_dir;
            /* 44 */ void *result;
        } find;
        struct {
            /* 30 */ u8 *buffer;
            /* 34 */ u32 buffer_size;
            /* 38 */ u16 total_length;
            /* 3a */ u16 dir_id;
        } get_path;
        /* 30 */ FSFileID open_by_id;
        struct {
            /* 30 */ u32 top;
            /* 34 */ u32 bottom;
            /* 38 */ u32 file_id;
        } open_range;
    } arg;
}; /* 48 */

// Tests flag bits of an archive or a file; the result is a BOOL.
#define FS_ARCHIVE_HAS(arc, mask) ((((arc)->flags & (mask))) ? TRUE : FALSE)
#define FS_FILE_HAS(file, mask) ((((file)->stat & (mask))) ? TRUE : FALSE)

// Commands and their built-in handlers (fs_command.c).
void func_02040eec(FSFile *file, int result);
int func_02040f48(FSFile *file, int command);
int func_020410d8(const u8 *a, const u8 *b, u32 n);
int func_02041128(FSTablePos *tablePos, void *dst, u32 len);
int func_020411e8(FSFile *file, u32 dirId);
int func_0204121c(FSFile *file);
int func_02041248(FSFile *file);
int func_02041274(FSFile *file);
int func_0204130c(FSFile *file);
int func_02041438(FSFile *file);
int func_0204161c(FSFile *file);
int func_020419c8(FSFile *file);
int func_02041a48(FSFile *file);
int func_02041a70(FSFile *file);

// Archives (fs_archive.c)
u32 func_02041a78(const u8 *name, int len);
int func_02041ad0(FSArchive *arc, void *dst, u32 pos, u32 len);
int func_02041aec(FSArchive *arc, void *src, u32 pos, u32 len);
FSFile *func_02041b0c(FSArchive *arc);
void func_02041d20(FSFile *file);
BOOL func_02041db4(FSFile *file);
BOOL func_02041df8(FSFile *file, u32 command);
void func_02041f9c(FSArchive *arc);
FSArchive *func_02041fc8(const char *name, int len);
BOOL func_02042008(FSArchive *arc, const char *name, int len);
BOOL func_020420bc(FSArchive *arc, u32 base, u32 fat, u32 fatSize, u32 fnt, u32 fntSize, FSReadFunc read,
                   FSReadFunc write);
void func_0204212c(FSArchive *arc, FSProc proc, u32 mask);
void func_0204214c(FSArchive *arc, int result);

// Files (fs_file.c)
void func_020421d0(u32 dma);
void func_020421f8(FSFile *file);
BOOL func_02042220(FSFile *file, const u8 *path, FSFileID *idOut, FSDirPos *dirOut);
int func_02042364(FSFile *file, void *buf, int len, BOOL async);
BOOL FS_ConvertPathToFileID(FSFileID *id, const char *path);
BOOL func_02042420(FSFile *file, FSFileID id);
BOOL func_02042490(FSFile *file, const char *path);
BOOL func_020424d8(FSFile *file);
BOOL func_02042514(FSFile *file);
int func_020425d4(FSFile *file, void *buf, int len);
int func_020425e4(FSFile *file, void *buf, int len);
BOOL func_020425f4(FSFile *file, int offset, int origin);

// The ROM archive (fs_rom.c)
void func_02042660(void *arc);
int func_02042684(FSArchive *arc, void *dst, u32 pos, u32 len);
int func_020426cc(FSArchive *arc, void *dst, u32 pos, u32 len);
int func_020426d4(void *file, int event);
int func_02042740(FSArchive *arc, void *dst, u32 pos, u32 len);
int func_02042748(void *file, int event);
void FS_Init(u32 dma);

#ifdef __cplusplus
}
#endif
