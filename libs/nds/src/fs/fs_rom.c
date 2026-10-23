#include <nitro/fs.h>
#include <nitro/card.h>

// With this value in the mode halfword the archive gets no tables and refuses every command.
#define FS_MODE_NO_TABLES 2

// Address of the mode halfword; the table positions and sizes follow at offset 0x200. Written as an enum
// constant, the compiler keeps it in one register across the calls.
typedef enum { FS_MODE_ADDR = 0x027ffc40 } FSModeAddr;

typedef struct {
    /* 000 */ u16 mode;
    /* 002 */ u8 mUnk_002[0x1fe];
    /* 200 */ u32 fnt;
    /* 204 */ u32 fnt_size;
    /* 208 */ u32 fat;
    /* 20c */ u32 fat_size;
} FSRomInfo;

typedef struct {
    /* 00 */ int lock_id;
    /* 04 */ u32 dma;
    /* 08 */ u32 mUnk_08;
    /* 0c */ u32 mUnk_0c;
    /* 10 */ u32 mUnk_10;
    /* 14 */ u32 mUnk_14;
} FSRomWork; /* 18 */

FSRomWork data_020cc6bc;
FSArchive data_020cc6d4;

static char data_020644c0[] = "rom";

// Completion of a card read: ends the pending command with the card's result.
void func_02042660(void *arc) {
    func_0204214c(arc, func_02046098() ? FS_RESULT_ERROR : FS_RESULT_OK);
}

// Read function of the card ROM archive: starts a DMA transfer from the card and completes later.
int func_02042684(FSArchive *arc, void *dst, u32 pos, u32 len) {
    func_02045ccc(data_020cc6bc.dma, pos, dst, len, func_02042660, arc, 1);
    return FS_RESULT_ASYNC;
}

// Write function of an archive that cannot be written.
int func_020426cc(FSArchive *arc, void *dst, u32 pos, u32 len) {
    return FS_RESULT_FAILURE;
}

// Callback of the card ROM archive: takes the card lock around a command sequence and refuses writes.
int func_020426d4(void *file, int event) {
    switch (event) {
        case FS_EVENT_START:
            func_02044f1c((u16) data_020cc6bc.lock_id);
            return 0;
        case FS_EVENT_END:
            func_02044f38((u16) data_020cc6bc.lock_id);
            return 0;
        case FS_COMMAND_WRITE:
            return FS_RESULT_UNSUPPORTED;
    }
    return FS_RESULT_DEFAULT_ALWAYS;
}

// Read function of an archive with no data.
int func_02042740(FSArchive *arc, void *dst, u32 pos, u32 len) {
    return FS_RESULT_FAILURE;
}

// Callback of the empty archive: refuses everything.
int func_02042748(void *file, int event) {
    return FS_RESULT_UNSUPPORTED;
}

// Registers the card ROM archive under the name "rom". With mode 2 the archive has no tables; otherwise its tables
// are the ones whose positions and sizes are stored in the words after the mode.
void FS_Init(u32 dma) {
    volatile FSRomInfo *header;
    FSModeAddr addr;
    u32 fnt, fat;
    data_020cc6bc.dma = dma;
    data_020cc6bc.lock_id = OS_GetLockID();
    data_020cc6bc.mUnk_08 = 0;
    data_020cc6bc.mUnk_0c = 0;
    data_020cc6bc.mUnk_10 = 0;
    data_020cc6bc.mUnk_14 = 0;
    func_02045dc4();
    func_02041f9c(&data_020cc6d4);
    func_02042008(&data_020cc6d4, data_020644c0, 3);
    addr = FS_MODE_ADDR;
    header = (volatile FSRomInfo *) addr;
    if (header->mode == FS_MODE_NO_TABLES) {
        data_020cc6bc.mUnk_08 = -1;
        data_020cc6bc.mUnk_0c = 0;
        data_020cc6bc.mUnk_10 = -1;
        data_020cc6bc.mUnk_14 = 0;
        func_0204212c(&data_020cc6d4, func_02042748, -1);
        func_020420bc(&data_020cc6d4, 0, 0, 0, 0, 0, func_02042740, func_020426cc);
    } else {
        func_0204212c(&data_020cc6d4, func_020426d4,
                      (1 << FS_COMMAND_WRITE) | (1 << FS_EVENT_START) | (1 << FS_EVENT_END));
        fnt = header->fnt;
        if (fnt != -1 && fnt != 0) {
            fat = header->fat;
            if (fat != -1 && fat != 0) {
                func_020420bc(&data_020cc6d4, 0, fat, header->fat_size, fnt, header->fnt_size, func_02042684,
                              func_020426cc);
            }
        }
    }
}
