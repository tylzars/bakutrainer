#include <nitro/os.h>

typedef struct {
    s32 size;            /* 00: -1 while the slot is unused */
    Cell *free;          /* 04 */
    Cell *allocated;     /* 08 */
} HeapDesc;

typedef struct {
    OSHeapHandle current; /* 00 */
    s32 numHeaps;         /* 04 */
    void *arenaStart;     /* 08 */
    void *arenaEnd;       /* 0c */
    HeapDesc *heapArray;  /* 10 */
} HeapInfo;               /* 14: the descriptors follow */

static HeapInfo *sHeapTable[9];

Cell *DLAddFront(Cell *list, Cell *cell) {
    cell->next = list;
    cell->prev = NULL;
    if (list != NULL) {
        list->prev = cell;
    }
    return cell;
}

Cell *DLExtract(Cell *list, Cell *cell) {
    if (cell->next != NULL) {
        cell->next->prev = cell->prev;
    }
    if (cell->prev == NULL) {
        list = cell->next;
    } else {
        cell->prev->next = cell->next;
    }
    return list;
}

Cell *DLInsert(Cell *list, Cell *cell) {
    Cell *prev = NULL;
    Cell *next = list;
    while (next != NULL) {
        if (cell <= next) {
            break;
        }
        prev = next;
        next = next->next;
    }
    cell->prev = prev;
    cell->next = next;
    if (next != NULL) {
        next->prev = cell;
        if ((u8 *) cell + cell->size == (u8 *) next) {
            cell->size += next->size;
            next = next->next;
            cell->next = next;
            if (next != NULL) {
                next->prev = cell;
            }
        }
    }
    if (prev != NULL) {
        prev->next = cell;
        if ((u8 *) prev + prev->size == (u8 *) cell) {
            prev->size += cell->size;
            prev->next = next;
            if (next != NULL) {
                next->prev = prev;
            }
        }
        return list;
    }
    return cell;
}

void *OS_AllocFromHeap(OSArenaId id, OSHeapHandle heap, u32 size) {
    HeapInfo *info;
    HeapDesc *hd;
    Cell *cell;
    u32 left;
    OSIntrMode e = OS_DisableInterrupts();
    info = sHeapTable[id];
    if (info == NULL) {
        OS_RestoreInterrupts(e);
        return NULL;
    }
    if (heap < 0) {
        heap = info->current;
    }
    hd = &info->heapArray[heap];
    size = (size + 0x3f) & ~0x1f;
    for (cell = hd->free; cell != NULL; cell = cell->next) {
        if ((s32) size <= cell->size) {
            break;
        }
    }
    if (cell == NULL) {
        OS_RestoreInterrupts(e);
        return NULL;
    }
    left = cell->size - size;
    if (left < 0x40) {
        hd->free = DLExtract(hd->free, cell);
    } else {
        Cell *rest;
        cell->size = size;
        rest = (Cell *) ((u8 *) cell + size);
        rest->size = left;
        rest->prev = cell->prev;
        rest->next = cell->next;
        if (rest->next != NULL) {
            rest->next->prev = rest;
        }
        if (rest->prev != NULL) {
            rest->prev->next = rest;
        } else {
            hd->free = rest;
        }
    }
    hd->allocated = DLAddFront(hd->allocated, cell);
    OS_RestoreInterrupts(e);
    return (u8 *) cell + 0x20;
}

void OS_FreeToHeap(OSArenaId id, OSHeapHandle heap, void *ptr) {
    HeapInfo *info;
    HeapDesc *hd;
    Cell *cell;
    OSIntrMode e = OS_DisableInterrupts();
    info = sHeapTable[id];
    if (heap < 0) {
        heap = info->current;
    }
    hd = &info->heapArray[heap];
    cell = (Cell *) ((u8 *) ptr - 0x20);
    hd->allocated = DLExtract(hd->allocated, cell);
    hd->free = DLInsert(hd->free, cell);
    OS_RestoreInterrupts(e);
}

OSHeapHandle OS_SetCurrentHeap(OSArenaId id, OSHeapHandle heap) {
    OSIntrMode e = OS_DisableInterrupts();
    HeapInfo *info = sHeapTable[id];
    OSHeapHandle old = info->current;
    info->current = heap;
    OS_RestoreInterrupts(e);
    return old;
}

void *OS_InitAlloc(OSArenaId id, void *lo, void *hi, int maxHeaps) {
    OSIntrMode e = OS_DisableInterrupts();
    HeapInfo *info = (HeapInfo *) lo;
    int i;
    u32 arraySize;
    sHeapTable[id] = info;
    info->heapArray = (HeapDesc *) ((u8 *) info + sizeof(HeapInfo));
    arraySize = maxHeaps * sizeof(HeapDesc);
    info->numHeaps = maxHeaps;
    for (i = 0; i < info->numHeaps; i++) {
        HeapDesc *hd = &info->heapArray[i];
        hd->size = -1;
        hd->allocated = NULL;
        hd->free = NULL;
    }
    info->current = -1;
    info->arenaStart = (void *) (((u32) info->heapArray + arraySize + 0x1f) & ~0x1f);
    info->arenaEnd = (void *) ((u32) hi & ~0x1f);
    OS_RestoreInterrupts(e);
    return info->arenaStart;
}

OSHeapHandle OS_CreateHeap(OSArenaId id, void *lo, void *hi) {
    OSIntrMode e = OS_DisableInterrupts();
    HeapInfo *info = sHeapTable[id];
    HeapDesc *hd;
    int i;
    lo = (void *) (((u32) lo + 0x1f) & ~0x1f);
    hi = (void *) ((u32) hi & ~0x1f);
    for (i = 0; i < info->numHeaps; i++) {
        hd = &info->heapArray[i];
        if (hd->size < 0) {
            Cell *c = (Cell *) lo;
            hd->size = (u32) hi - (u32) lo;
            c->prev = NULL;
            c->next = NULL;
            c->size = hd->size;
            hd->free = c;
            hd->allocated = NULL;
            OS_RestoreInterrupts(e);
            return i;
        }
    }
    OS_RestoreInterrupts(e);
    return -1;
}

s32 func_0203ed14(OSArenaId id, OSHeapHandle heap) {
    u32 maxFree = 0;
    OSIntrMode e = OS_DisableInterrupts();
    HeapInfo *info = sHeapTable[id];
    Cell *cell;
    if (heap < 0) {
        heap = info->current;
    }
    for (cell = info->heapArray[heap].free; cell != NULL; cell = cell->next) {
        u32 size = cell->size - 0x20;
        if (size > maxFree) {
            maxFree = size;
        }
    }
    OS_RestoreInterrupts(e);
    return maxFree;
}
