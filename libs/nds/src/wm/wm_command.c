#include <nitro/os.h>
#include <nitro/mi.h>
#include <nitro/pxi.h>
#include <nitro/math.h>
#include <nitro/wm.h>

void func_020461e4(u32 idx, WMCallback callback) {
    data_020cd100.work->cbTable[idx] = callback;
}

void *func_020461fc(void) {
    u16 *msg;
    if (!OS_ReceiveMessage(&data_020cd108, (void **) &msg, 0)) {
        return NULL;
    }
    DC_InvalidateRange(msg, 2);
    if (*msg & 0x8000) {
        return msg;
    }
    OS_JamMessage(&data_020cd108, msg, 1);
    return NULL;
}

int func_02046254(u32 cmd, u16 argc, ...) {
    char *ap;
    int i;
    int r;
    u32 *msg = (u32 *) func_020461fc();
    if (msg == NULL) {
        return 8;
    }
    *(u16 *) msg = cmd;
    ap = (char *) (((u32) &argc & ~3) + 4);
    for (i = 0; i < argc; i++) {
        msg[1 + i] = *(u32 *) ((ap += 4) - 4);
    }
    DC_StoreRange(msg, 0x100);
    r = PXI_SendWordByFifo(0xa, (u32) msg, 0);
    OS_SendMessage(&data_020cd108, msg, 1);
    return r < 0 ? 8 : 2;
}

int func_02046300(void *src, u32 len) {
    int r;
    void *msg = func_020461fc();
    if (msg == NULL) {
        return 8;
    }
    MI_CpuCopy8(src, msg, len);
    DC_StoreRange(msg, len);
    r = PXI_SendWordByFifo(0xa, (u32) msg, 0);
    OS_SendMessage(&data_020cd108, msg, 1);
    return r < 0 ? 8 : 2;
}

WMWork *func_02046370(void) {
    return data_020cd100.work;
}

int func_02046380(void) {
    if (data_020cd100.init != 0) {
        return 0;
    }
    return 3;
}

int func_0204639c(void) {
    int r = func_02046380();
    if (r != 0) {
        return r;
    }
    DC_InvalidateRange(data_020cd100.work->st, 2);
    if (*(u16 *) data_020cd100.work->st <= 1) {
        return 3;
    }
    return 0;
}

int func_020463e4(int n, ...) {
    int r = func_02046380();
    char *ap;
    u32 st;
    if (r != 0) {
        return r;
    }
    DC_InvalidateRange(data_020cd100.work->st, 2);
    st = *(u16 *) data_020cd100.work->st;
    ap = (char *) (((u32) &n & ~3) + 4);
    r = 3;
    if (n == 0) {
        return 3;
    }
    do {
        if (*(u32 *) ((ap += 4) - 4) == st) {
            r = 0;
        }
    } while (--n != 0);
    return r;
}
