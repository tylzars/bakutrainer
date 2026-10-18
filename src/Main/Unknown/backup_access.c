#include <Main/Unknown/backup_access.h>
#include <Main/Unknown/globals.h>
#include <nitro/card.h>
#include <nitro/os.h>

#define SYSTEM_BACKUP_READY 0x4000

// Marks the backup memory as ready after taking the card lock id and checking the backup type.
void func_02020ee0(u32 a) {
    int ok;
    if (!(data_020c18bc.mUnk_46c & SYSTEM_BACKUP_READY)) {
        data_020c18bc.mUnk_456 = 0xb4;
        func_02045dc4();
        data_020c18bc.mUnk_330 = OS_GetLockID();
        func_02044f54(data_020c18bc.mUnk_330);
        ok = func_020455fc(a);
        func_02044f64(data_020c18bc.mUnk_330);
        if (ok != 0) {
            data_020c18bc.mUnk_46c |= SYSTEM_BACKUP_READY;
        }
    }
}

// Writes size bytes from buf to the backup memory at address; returns 0 when the backup memory is not ready.
int func_02020f38(const void *buf, u32 address, u32 size) {
    int result;
    if (!(data_020c18bc.mUnk_46c & SYSTEM_BACKUP_READY)) {
        return 0;
    }
    data_020c18bc.mUnk_456 = 0xf0;
    func_02044f54(data_020c18bc.mUnk_330);
    result = func_02045500((u32) buf, (void *) address, size, 0, 0, 0, 8, 10, 2);
    func_02044f64(data_020c18bc.mUnk_330);
    return result;
}

int func_02020f9c(void *buf, u32 address, u32 size) {
    int result;
    if ((data_020c18bc.mUnk_46c & SYSTEM_BACKUP_READY) == 0) {
        return 0;
    }
    func_02044f54(data_020c18bc.mUnk_330);
    result = func_02045500(address, buf, size, 0, 0, 0, 6, 1, 0);
    func_02044f64(data_020c18bc.mUnk_330);
    return result;
}
