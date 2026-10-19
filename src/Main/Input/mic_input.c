#include <Main/Input/mic_input.h>
#include <Main/System/game_math.h>
#include <Main/System/system_init.h>
#include <Main/System/align_util.h>
#include <nitro/math.h>
#include <Main/clib.h>
#include <nitro/mic.h>
#include <nitro/mi.h>
#include <nitro/os.h>
#include <nitro/pm.h>

#define SAMPLE_SHIFT 12
#define POWER_SHIFT 22
#define MAX_SHIFT 9

const u16 data_02050210[8] = {90, 150, 150, 200, 200, 300, 300, 400};

void func_02020ff4(int size, int b) {
    if (data_020c18bc.mUnk_46c & MIC_FLAG_ACTIVE) {
        func_02021458();
    }
    data_020c18bc.mUnk_0cc = size;
    data_020c18bc.mUnk_0c4 = func_0201c430(size + 0x1f);
    data_020c18bc.mUnk_0c8 = func_0201c430(size);
    MIC_Init();
    func_02043d98(1);
    func_02043de0(b);
    data_020c18bc.mUnk_46c |= MIC_FLAG_ACTIVE;
    func_02021194();
}

void func_02021054(int a) {
    if ((data_020c18bc.mUnk_46c & MIC_FLAG_ACTIVE) == 0) {
        OS_Terminate();
    }
    func_02043de0(a);
}

void func_02021074(int rate, int type) {
    data_020c18bc.mUnk_0d0.buffer = (void *) func_02034918((u32) data_020c18bc.mUnk_0c4);
    data_020c18bc.mUnk_0d0.size = data_020c18bc.mUnk_0cc;
    data_020c18bc.mUnk_0d0.type = type;
    data_020c18bc.mUnk_0d0.rate = rate;
    data_020c18bc.mUnk_0d0.loop = 1;
    data_020c18bc.mUnk_0d0.full = 0;
    data_020c18bc.mUnk_492 = 0xb4;
    data_020c18bc.mUnk_495 = 1;
    if (MIC_StartAutoSampling(&data_020c18bc.mUnk_0d0) != 0) {
        OS_Terminate();
    }
    func_02021194();
}

void func_020210c4(void) {
    MIC_StopAutoSampling();
}

u32 func_020210cc(void) {
    u32 pos = func_02043454();
    if (pos >= func_02034918((u32) data_020c18bc.mUnk_0c4) &&
        pos < func_02034918((u32) data_020c18bc.mUnk_0c4) + data_020c18bc.mUnk_0cc && data_020c18bc.mUnk_492 == 0) {
        DC_InvalidateRange((void *) (pos & ~0x1f), 0x20);
        return pos;
    }
    return 0;
}

void func_0202111c(u32 count) {
    u32 i;
    u32 pos;
    u32 start;
    if ((data_020c18bc.mUnk_46c & MIC_FLAG_ACTIVE) == 0) {
        OS_Terminate();
    }
    pos = func_020210cc();
    if (pos != 0) {
        pos -= count;
        start = func_02034918((u32) data_020c18bc.mUnk_0c4);
        if (start == 0) {
            OS_Terminate();
        }
        if (pos < start) {
            pos += data_020c18bc.mUnk_0cc;
        }
        for (i = 0; i < count; i++) {
            if ((pos & 0x1f) == 0) {
                DC_InvalidateRange((void *) pos, 0x20);
                if (pos >= start + data_020c18bc.mUnk_0cc) {
                    pos = start;
                }
            }
            data_020c18bc.mUnk_0c8[i] = *(u8 *) pos;
            pos++;
        }
    }
}

void func_02021194(void) {
    int j;
    int i;
    for (i = 0; i < MIC_HISTORY; i++) {
        for (j = 0; j < MIC_MAX_BANDS; j++) {
            data_020c18bc.mUnk_0ec[i][j] = 0;
        }
    }
    MI_CpuClearFast(data_020c18bc.mUnk_20c, 0x20);
    MI_CpuClearFast(data_020c18bc.mUnk_1ec, 0x20);
}

// Sums the spectrum over each band's range, stores the sums in the current history slot and totals each band over
// the history.
void func_020211d8(const u8 *spectrum, const u16 *ranges, u32 count, u32 *totals, u32 *sums) {
    int i;
    u32 j;
    u32 lo;
    u32 hi;
    if (count >= MIC_MAX_BANDS) {
        OS_Terminate();
    }
    if (data_020c18bc.mUnk_0d0.buffer == 0) {
        OS_Terminate();
    }
    for (i = 0; i < count; i++) {
        lo = func_02021364(ranges[0]);
        hi = func_02021364(ranges[1]);
        sums[i] = 0;
        for (; lo < hi; lo++) {
            sums[i] += spectrum[lo];
        }
        ranges += 2;
    }
    j = data_020c18bc.mUnk_45c & 0xf;
    for (i = 0; i < count; i++) {
        data_020c18bc.mUnk_0ec[j][i] = sums[i];
    }
    for (j = 0; j < count; j++) {
        totals[j] = 0;
        for (i = 0; i < MIC_HISTORY; i++) {
            totals[j] += data_020c18bc.mUnk_0ec[i][j];
        }
    }
}

typedef struct Unk_Complex {
    fx32 re;
    fx32 im;
} Unk_Complex;

void func_020212b4(u8 *out, u32 nShift) {
    u32 count = 1 << nShift;
    fx16 sinTable[(3 << MAX_SHIFT) / 4];
    Unk_Complex buf[1 << MAX_SHIFT];
    u32 i;
    if (nShift > MAX_SHIFT) {
        OS_Terminate();
    }
    data_020c18bc.mUnk_22c = nShift;
    func_0202111c(count);
    for (i = 0; i < count; i++) {
        buf[i].re = data_020c18bc.mUnk_0c8[i] << SAMPLE_SHIFT;
        buf[i].im = 0;
    }
    func_0204882c(sinTable, nShift);
    func_02048af8((fx32 *) buf, nShift, sinTable);
    for (i = 1; i < count; i++) {
        out[i - 1] = (buf[i].re * buf[i].re + buf[i].im * buf[i].im) >> POWER_SHIFT;
    }
}

u32 func_02021364(u32 x) {
    return (x << data_020c18bc.mUnk_22c) / data_020c18bc.mUnk_0d0.rate;
}

u32 func_02021380(u32 index) {
    return data_020c18bc.mUnk_20c[index] >> 4;
}

void func_02021390(void) {
    u8 spectrum[0x400];
    if ((data_020c18bc.mUnk_46c & MIC_FLAG_ACTIVE) == 0) {
        func_02020ff4(0x400, 1);
        func_02021074(0x82e, 0);
    }
    func_020212b4(spectrum, 8);
    func_020211d8(spectrum, data_02050210, 4, data_020c18bc.mUnk_20c, data_020c18bc.mUnk_1ec);
}

int func_020213e8(u32 a) {
    int v0;
    int v1;
    int v2;
    int v3;
    u32 limit;
    func_02021390();
    v0 = func_02021380(0);
    v1 = func_02021380(1);
    v2 = func_02021380(2);
    v3 = func_02021380(3);
    if (v0 > a && 40 * v3 > v0 && v3 < v0 && v1 < v0 && abs(v1 - v2) < v1 / 2 && v0 > 2 * v3) {
        limit = a * 6;
        if (v0 >= limit) {
            v0 = limit;
        }
        return func_0201fb74(v0, limit, 1, 7);
    }
    return 0;
}

void func_02021458(void) {
    if (data_020c18bc.mUnk_46c & MIC_FLAG_ACTIVE) {
        func_020210c4();
        func_02043d98(0);
        func_0201c4f0(&data_020c18bc.mUnk_0c4);
        func_0201c4f0(&data_020c18bc.mUnk_0c8);
        data_020c18bc.mUnk_46c &= ~MIC_FLAG_ACTIVE;
    }
}
