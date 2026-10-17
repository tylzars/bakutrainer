#include <nitro/os.h>

#define OS_ARENA_INFO_BASE 0x27ffda0

typedef struct {
    BOOL initialized;
    BOOL exArena;
} ArenaState;

static ArenaState sArenaState;

void OS_InitArena(void) {
    if (sArenaState.initialized) {
        return;
    }
    sArenaState.initialized = TRUE;
    OS_SetArenaHi(OS_ARENA_MAIN, OS_GetInitArenaHi(OS_ARENA_MAIN));
    OS_SetArenaLo(OS_ARENA_MAIN, OS_GetInitArenaLo(OS_ARENA_MAIN));
    OS_SetArenaLo(OS_ARENA_MAINEX, NULL);
    OS_SetArenaHi(OS_ARENA_MAINEX, NULL);
    OS_SetArenaHi(OS_ARENA_ITCM, OS_GetInitArenaHi(OS_ARENA_ITCM));
    OS_SetArenaLo(OS_ARENA_ITCM, OS_GetInitArenaLo(OS_ARENA_ITCM));
    OS_SetArenaHi(OS_ARENA_DTCM, OS_GetInitArenaHi(OS_ARENA_DTCM));
    OS_SetArenaLo(OS_ARENA_DTCM, OS_GetInitArenaLo(OS_ARENA_DTCM));
    OS_SetArenaHi(OS_ARENA_SHARED, OS_GetInitArenaHi(OS_ARENA_SHARED));
    OS_SetArenaLo(OS_ARENA_SHARED, OS_GetInitArenaLo(OS_ARENA_SHARED));
    OS_SetArenaHi(OS_ARENA_WRAM_MAIN, OS_GetInitArenaHi(OS_ARENA_WRAM_MAIN));
    OS_SetArenaLo(OS_ARENA_WRAM_MAIN, OS_GetInitArenaLo(OS_ARENA_WRAM_MAIN));
}

void OS_InitArenaEx(void) {
    OS_SetArenaHi(OS_ARENA_MAINEX, OS_GetInitArenaHi(OS_ARENA_MAINEX));
    OS_SetArenaLo(OS_ARENA_MAINEX, OS_GetInitArenaLo(OS_ARENA_MAINEX));
    if (sArenaState.exArena) {
        if ((func_0203e264() & 3) != 1) {
            return;
        }
    }
    OS_SetProtectionRegion1(0x200002b);
    OS_SetProtectionRegion2(0x23e0021);
}

void *OS_GetArenaHi(OSArenaId id) {
    return ((void **) (OS_ARENA_INFO_BASE + 0x24))[id];
}

void *OS_GetArenaLo(OSArenaId id) {
    return ((void **) OS_ARENA_INFO_BASE)[id];
}

void *OS_GetInitArenaHi(OSArenaId id) {
    switch (id) {
    case OS_ARENA_MAIN:
        return (void *) 0x23e0000;
    case OS_ARENA_MAINEX:
        if (sArenaState.exArena == FALSE || (func_0203e264() & 3) == 1) {
            return NULL;
        }
        return (void *) 0x2700000;
    case OS_ARENA_ITCM:
        return (void *) 0x2000000;
    case OS_ARENA_DTCM: {
        u32 dtcm = HW_DTCM;
        u32 irqBottom = dtcm + 0x3f80 - (u32) SDK_IRQ_STACKSIZE;
        s32 sysSize = (s32) SDK_SYS_STACKSIZE;
        if (sysSize == 0) {
            return (void *) (dtcm < 0x27e0080 ? 0x27e0080 : dtcm);
        }
        if (sysSize < 0) {
            return (void *) (0x27e0080 - sysSize);
        }
        return (void *) (irqBottom - sysSize);
    }
    case OS_ARENA_SHARED:
        return (void *) 0x27ff680;
    case OS_ARENA_WRAM_MAIN:
        return (void *) 0x37f8000;
    default:
        return NULL;
    }
}

void *OS_GetInitArenaLo(OSArenaId id) {
    switch (id) {
    case OS_ARENA_MAIN:
        return (void *) 0x20ce200;
    case OS_ARENA_MAINEX:
        if (sArenaState.exArena == FALSE || (func_0203e264() & 3) == 1) {
            return NULL;
        }
        return (void *) 0x23e0000;
    case OS_ARENA_ITCM:
        return (void *) 0x1ff83c0;
    case OS_ARENA_DTCM:
        return (void *) 0x27e0080;
    case OS_ARENA_SHARED:
        return (void *) 0x27ff000;
    case OS_ARENA_WRAM_MAIN:
        return (void *) 0x37f8000;
    default:
        return NULL;
    }
}

void OS_SetArenaHi(OSArenaId id, void *newHi) {
    ((void **) (OS_ARENA_INFO_BASE + 0x24))[id] = newHi;
}

void OS_SetArenaLo(OSArenaId id, void *newLo) {
    ((void **) OS_ARENA_INFO_BASE)[id] = newLo;
}
