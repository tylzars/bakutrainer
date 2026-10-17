#include <nitro/snd.h>

void func_020400cc(u32 a, u32 b, u32 c, u32 d) {
    func_020402ac(0xc, a, b, c, d);
}

void func_020400f4(u32 a, u32 b, u32 c, u32 d) {
    int i;
    u32 mask = c;
    for (i = 0; i < 8 && mask != 0; i++, mask >>= 1) {
        if (mask & 1) {
            func_020409fc(i);
        }
    }
    func_020402ac(0xd, a, b, c, d);
}

void func_02040158(u32 a, u32 b, u32 c, void (*callback)(void *), void *arg) {
    u32 tag = func_02040a1c(a, callback, arg);
    func_020402ac(0x12, a, b, c, tag);
}

void func_02040198(u32 a, u32 b) {
    func_020402ac(0x1a, a, b, 0, 0);
}

void func_020401b8(u32 a, u32 b) {
    func_020402ac(0x1b, a, b, 0, 0);
}

void func_020401d8(u32 a, u32 b) {
    func_020402ac(0x13, a, b, 0, 0);
}

void func_020401f8(u32 a, u32 b, u32 c) {
    func_020402ac(0x14, a, b, c, 0);
}

void func_02040220(u32 a, u32 b) {
    func_020402ac(0x15, a, b, 0, 0);
}

void func_02040240(u32 p0, u32 p1, u32 p2, u32 p3, u32 p4, u32 p5, u32 p6, u32 p7, u32 p8, u32 p9) {
    func_020402ac(0xe, p0 | (p8 << 16), p2, (p6 << 24) | (p7 << 22) | p5, (p3 << 26) | (p1 << 24) | (p9 << 16) | p4);
}

void func_0204028c(u32 a) {
    func_020402ac(0x17, a, 0, 0, 0);
}

void func_020402ac(u32 id, u32 a, u32 b, u32 c, u32 d) {
    SNDCommand *cmd = SND_AllocCommand(1);
    if (cmd == NULL) {
        return;
    }
    cmd->id = id;
    cmd->arg[0] = a;
    cmd->arg[1] = b;
    cmd->arg[2] = c;
    cmd->arg[3] = d;
    SND_PushCommand(cmd);
}
