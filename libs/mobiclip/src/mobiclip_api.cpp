#include <mobiclip.h>
#include <nitro/fs.h>
#include <nitro/init.h>
#include <nitro/mi.h>

// Reads the file header and sets the player up; n is the number of pictures the decoder cycles through.
int func_02039dd8(Player *p, Reader *r, u32 n) {
    func_02000b9c(data_02000bc4);
    return func_0203a0b0(p, r, n);
}

// Opens a player on an open file; returns 0 and frees what it allocated when the file is not usable.
Player *func_02039e08(FSFile *src, u32 n) {
    Reader *r;
    Player *p;
    r = new Reader;
    if (r == 0) {
        return 0;
    }
    if (func_0203adb8(r, src) == 0) {
        delete r;
        return 0;
    }
    p = new Player;
    if (func_02039dd8(p, r, n) == 0) {
        delete p;
        return 0;
    }
    return p;
}

void func_02039ef0(Player *p) {
    if (p != 0) {
        delete p;
    }
}

int func_02039f10(Player *p) {
    if (p == 0) {
        return 0;
    }
    return func_0203a7c4(p);
}

int func_02039f28(Player *p) {
    if (p == 0) {
        return 0;
    }
    return func_0203a7cc(p);
}

int func_02039f40(Player *p) {
    if (p == 0) {
        return 0;
    }
    return func_0203a7d4(p);
}

int func_02039f58(Player *p) {
    if (p == 0) {
        return 0;
    }
    return func_0203a7dc(p);
}

int func_02039f70(Player *p) {
    if (p == 0) {
        return 0;
    }
    return func_0203a800(p) == 1;
}

int func_02039f94(Player *p) {
    if (p == 0) {
        return 0;
    }
    return func_0203a938(p) == 1;
}

int func_02039fb8(Player *p, u32 a, u32 b, int c) {
    if (p == 0) {
        return 0;
    }
    return func_0203aa1c(p, a, b, c) == 1;
}

int func_02039fdc(Player *p) {
    if (p == 0) {
        return 0;
    }
    return func_0203aba0(p) == 1;
}

int func_0203a000(Player *p) {
    if (p == 0) {
        return 0;
    }
    return func_0203abe0(p);
}

int func_0203a018(Player *p, void *buffer) {
    if (p == 0) {
        return 0;
    }
    return func_0203abf0(p, buffer) == 1;
}

// Ends a reader's lifetime without freeing it. Nothing calls it and the compiler drops it, but its
// destructor call emits ReaderBase's destructors here, after the other functions.
static void DestroyReader(ReaderBase *r) {
    r->~ReaderBase();
}
