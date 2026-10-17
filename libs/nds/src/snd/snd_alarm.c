#include <nitro/snd.h>

typedef struct {
    void (*callback)(void *);
    void *arg;
    u8 count;
} AlarmEntry;

static AlarmEntry alarmTable[8];

void func_020409cc(void) {
    int i;
    for (i = 0; i < 8; i++) {
        alarmTable[i].callback = NULL;
        alarmTable[i].arg = NULL;
        alarmTable[i].count = 0;
    }
}

void func_020409fc(int index) {
    AlarmEntry *e = &alarmTable[index];
    e->count++;
}

u32 func_02040a1c(int index, void (*callback)(void *), void *arg) {
    AlarmEntry *e = &alarmTable[index];
    e->callback = callback;
    e->arg = arg;
    e->count++;
    return e->count;
}

void func_02040a4c(u32 data) {
    AlarmEntry *e = &alarmTable[data & 0xff];
    if (((int) data >> 8 & 0xff) != e->count) {
        return;
    }
    if (e->callback == NULL) {
        return;
    }
    e->callback(e->arg);
}
