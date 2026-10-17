#include <stdlib.h>
#include <nitro/os.h>

// Returns the block to the main arena's current heap.
void free(void *ptr) {
    OS_FreeToHeap(0, -1, ptr);
}
