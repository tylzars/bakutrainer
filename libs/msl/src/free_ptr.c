#include <stdlib.h>
#include <runtime.h>

void func_02048b3c(void *ptr) {
    if (ptr != 0) {
        free(ptr);
    }
}
