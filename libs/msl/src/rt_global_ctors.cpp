#include <runtime.h>

extern "C" void (*ARM9_CTOR_START)(void);

// Does nothing.
extern "C" void func_0204ded0(void) {
}

void operator delete(void *ptr) {
    if (ptr != 0) {
        func_02048b3c(ptr);
    }
}

// Calls each entry of the static-constructor table in turn, stopping at the first null entry.
extern "C" void func_0204dee8(void) {
    void (**p)(void) = &ARM9_CTOR_START;
    while (p != 0 && *p != 0) {
        (*p)();
        p++;
    }
}
