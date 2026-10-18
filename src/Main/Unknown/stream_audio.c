#include <nitro/os.h>
#include <nitro/snd.h>
#include <Main/Unknown/stream_audio.h>
#include <Main/Unknown/system_init.h>

#pragma thumb off

void *func_0202f500(u32 size) {
    return func_0201c430(size);
}

void func_0202f50c(void *p) {
    void *q = p;
    func_0201c4f0(&q);
}

void func_0202f520(void *arg) {
    data_020c661c.mUnk_2c++;
}

void func_0202f538(u32 a, u32 b) {
    u32 step = 0xffb0ff / a;
    u32 total = (b >> 6) * (step * 2);
    DC_StoreRange(data_020c661c.mUnk_34, (data_020c661c.mUnk_3c * b) << 1);
    func_02040198(1, 0);
    func_02040240(0, 1, (u32) data_020c661c.mUnk_34, 1, 0, (data_020c661c.mUnk_3c * b) >> 1, 0x7f, 0, step, 0x40);
    func_02040158(0, total, total, func_0202f520, 0);
    func_020400cc(1, 0, 1, 0);
    SND_FlushCommand(1);
}

void func_0202f614(void *arg) {
    data_020c661c.mUnk_40 = 1;
}

void *func_0202f628(u32 screen) {
    return (screen & 1) ? (void *) 0x6000000 : (void *) 0x6200000;
}

#pragma thumb on
