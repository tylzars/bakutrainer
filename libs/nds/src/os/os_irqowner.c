#include <nitro/os.h>

static u32 sIrqOwnerMask;
static u16 sIrqOwnerTable[9];

void func_0203f7e8(void) {
    int i;
    sIrqOwnerMask = 0;
    for (i = 0; i < 9; i++) {
        sIrqOwnerTable[i] = 0;
    }
}
