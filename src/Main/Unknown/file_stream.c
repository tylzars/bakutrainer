#include <nitro/fs.h>
#include <nitro/os.h>
#include <Main/Unknown/file_stream.h>
#include <Main/Unknown/functions.h>
#include <Main/Unknown/system_init.h>
#include <Main/Unknown/globals.h>

void func_0201d9a8(int offset) {
    if ((data_020c18bc.mUnk_46c & 0x200) == 0) {
        func_020425f4(&data_020c1874, offset, 0);
    } else {
        data_020c18bc.mUnk_250.mUnk_00 = offset;
    }
}

void func_0201d9d4(void *dst, u32 size) {
    int remaining;
    int state;
    int i;
    int j;
    u32 len;
    u32 sum;
    u8 *cmd = data_020c67e0;

    if ((data_020c18bc.mUnk_46c & 0x200) == 0) {
        func_020425e4(&data_020c1874, dst, size);
        return;
    }
restart:
    state = 0;
    i = 0;
    remaining = size;
    if (size > 0xfb800) {
        OS_Terminate();
    }
    for (; remaining > 0; remaining -= 0x1f7) {
        data_020c18bc.mUnk_250.mUnk_24[i >> 3] &= ~(1 << (i & 7));
        i++;
    }
    data_020c18bc.mUnk_250.mUnk_24[i >> 3] |= 1 << (i & 7);
    data_020c18bc.mUnk_250.mUnk_08 = size;
    data_020c18bc.mUnk_250.mUnk_04 = size;
    data_020c18bc.mUnk_250.mUnk_0c = dst;
    data_020c18bc.mUnk_46c |= 3 << 18;
    remaining = size;
    while (data_020c18bc.mUnk_250.mUnk_08 != 0 && (s8) data_020c18bc.mUnk_2a4[1] >= 0) {
        if (data_020c18bc.mUnk_250.mUnk_08 != remaining) {
            remaining = data_020c18bc.mUnk_250.mUnk_08;
            state = 5;
        } else if (state-- == 0) {
            i = 0;
            while (data_020c18bc.mUnk_250.mUnk_24[i >> 3] & (1 << (i & 7))) {
                i++;
            }
            j = i;
            sum = 0;
            while (!(data_020c18bc.mUnk_250.mUnk_24[j >> 3] & (1 << (j & 7)))) {
                j++;
                sum += 0x1f7;
            }
            len = (j - i) * 0x1f7;
            if (len >= data_020c18bc.mUnk_250.mUnk_04) {
                len = data_020c18bc.mUnk_250.mUnk_04;
            }
            cmd[0] = 0x51;
            func_02034ce0(data_020c18bc.mUnk_250.mUnk_00 + 0x1f7 * i, cmd + 6);
            func_02034ce0(len, cmd + 2);
            state = 5;
            if (func_02034cf0(cmd + 6) > data_020c18bc.mUnk_250.mUnk_00 + data_020c18bc.mUnk_250.mUnk_04) {
                goto restart;
            }
        }
        func_0201bb30();
    }
    data_020c18bc.mUnk_250.mUnk_00 += size;
    data_020c18bc.mUnk_46c &= 0xfff3ffff;
}
