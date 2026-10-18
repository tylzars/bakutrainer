#include <nitro/os.h>
#include <Main/Unknown/sound_control.h>
#include <Main/Unknown/functions.h>

#define SE_COUNT 0x78

void func_02010388(u32 a, s32 b) {
    SoundCtl *ctl = &data_020b10b4;

    if (a >= 9) {
        OS_Terminate();
    }
    data_020c8214.mUnk_10 |= 2;
    ctl->mUnk_14 = 0;
    ctl->mUnk_18 = 0;
    ctl->mUnk_08 = b;
    ctl->mUnk_0c = b;
    ctl->mUnk_00 = 0;
    ctl->mUnk_1c = 0;
    ctl->mUnk_14 = a;
    func_020103c0();
}

void func_020103c0(void) {
    SoundCtl *ctl = &data_020b10b4;
    u8 volume;

    if (data_020c8254.mUnk_08 != 0) {
        return;
    }
    volume = data_020c8214.mUnk_24 == 1 ? func_02038694() : ctl->mUnk_0c;
    if (data_020c8214.mUnk_24 == 0 && ctl->mUnk_18 != 0) {
        ctl->mUnk_18 = 0;
    }
    switch (ctl->mUnk_00) {
        case 1:
            if (volume != 0) {
                func_02038578(volume - 1, 0);
            } else {
                ctl->mUnk_00 = 0;
            }
            break;
        case 3:
            if (volume > ctl->mUnk_0c) {
                func_02038578(volume - 1, 0);
            } else {
                ctl->mUnk_00 = 0;
            }
            break;
        case 2:
            if (volume < ctl->mUnk_0c) {
                func_02038578(volume + 1, 0);
            } else {
                ctl->mUnk_00 = 0;
            }
            break;
    }
    if (ctl->mUnk_1c != 0) {
        ctl->mUnk_1c--;
        if (ctl->mUnk_1c == 0) {
            func_020104c0(ctl->mUnk_10, ctl->mUnk_04);
        }
    }
    if (ctl->mUnk_14 != ctl->mUnk_18) {
        ctl->mUnk_00 = 0;
        ctl->mUnk_0c = ctl->mUnk_08;
        ctl->mUnk_18 = ctl->mUnk_14;
        if (ctl->mUnk_18 >= 9) {
            OS_Terminate();
        }
        if (ctl->mUnk_18 == 0) {
            func_020359d0(1);
            return;
        }
        {
            static const u16 ids[9] = {0, 362, 49, 364, 363, 216, 92, 228, 227};
            func_02035108(ids[ctl->mUnk_18], 0x10d7, ctl->mUnk_0c, 0, 1, 0);
        }
    }
}

void func_020104c0(u32 a, int b) {
    SoundCtl *ctl = &data_020b10b4;

    if (a >= 9) {
        OS_Terminate();
    }
    ctl->mUnk_1c = 0;
    ctl->mUnk_14 = a;
    ctl->mUnk_08 = b;
}

void func_020104e0(u32 a, int b, int c) {
    SoundCtl *ctl = &data_020b10b4;

    if (a >= 9) {
        OS_Terminate();
    }
    ctl->mUnk_1c = c;
    ctl->mUnk_10 = a;
    ctl->mUnk_04 = b;
}

void func_02010500(u32 a) {
    SoundCtl *ctl = &data_020b10b4;

    if (a >= 4) {
        OS_Terminate();
    }
    ctl->mUnk_00 = a;
    if (a == 3) {
        ctl->mUnk_0c = 0x40;
    }
}

void func_02010520(u32 index, u16 volume, int b, int c, u32 d) {
    SeEntry *e;
    u32 pick;
    u16 id;

    if (index == 0 || index >= SE_COUNT) {
        OS_Terminate();
    }
    e = &data_0204edfc[index];
    pick = func_0201fd68() % e->mUnk_00;
    if (pick >= 6) {
        OS_Terminate();
    }
    id = e->mUnk_18[pick];
    if (id != 0 && e->mUnk_14 >= e->mUnk_10) {
        func_0202eb64(id, volume, 0, e->mUnk_04, 0xffff, 0);
        if (b != 0x100) {
            func_0202f240(id, b);
        }
        e->mUnk_14 = 0;
        e->mUnk_10 = c;
        if (e->mUnk_0c != 0) {
            e->mUnk_10 += func_0201fd68() % d;
        }
    } else {
        e->mUnk_14++;
    }
}

void func_020105c0(u32 index) {
    SeEntry *e;

    if (index == 0 || index >= SE_COUNT) {
        OS_Terminate();
    }
    e = &data_0204edfc[index];
    func_02010520(index, e->mUnk_02, 0x100, e->mUnk_08, e->mUnk_0c);
}

void func_020105f4(void *obj) {
    SeOwner *owner = obj;
    u32 index = owner->mUnk_12;
    SeEntry *e;

    if (index != 0) {
        if (index == 0 || index >= SE_COUNT) {
            OS_Terminate();
        }
        e = &data_0204edfc[index];
        if (e->mUnk_05 == 1) {
            func_0202f110(e->mUnk_18[0], e->mUnk_02, 1, e->mUnk_04, 0xffff, 0);
            return;
        }
        func_020105c0(index);
        owner->mUnk_12 = 0;
    }
}
