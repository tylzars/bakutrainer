#include <runtime.h>

extern void (*ARM9_CTOR_START)(void);

// Empty routine: returns immediately.
void func_0204ded0(void) {
}

// Frees a block unless the pointer is null; compiled C++ calls it for delete.
void func_0204ded4(void *ptr) {
    if (ptr != 0) {
        func_02048b3c(ptr);
    }
}

// Calls each entry of the static-constructor table in turn, stopping at the first null entry.
void func_0204dee8(void) {
    void (**p)(void) = &ARM9_CTOR_START;
    while (p != 0 && *p != 0) {
        (*p)();
        p++;
    }
}
