#include <nitro/os.h>

static struct {
    u16 initialized;
    u32 head;
    u32 tail;
    u32 mUnk_0c;
    u32 mUnk_10;
} sVAlarmState;

void OS_InitVAlarm(void) {
    if (sVAlarmState.initialized) {
        return;
    }
    sVAlarmState.initialized = TRUE;
    sVAlarmState.mUnk_0c = 0;
    sVAlarmState.mUnk_10 = 0;
    OS_DisableIrqMask(4);
    sVAlarmState.tail = 0;
    sVAlarmState.head = 0;
}
