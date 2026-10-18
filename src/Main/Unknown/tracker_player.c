#include <nitro/os.h>
#include <nitro/snd.h>
#include <Main/Unknown/functions.h>
#include <Main/Unknown/globals.h>
#include <Main/Unknown/system_init.h>
#include <Main/Unknown/tracker_player.h>

// One period of a sine wave, 64 entries with amplitude 255; every channel points at it twice.
extern const s16 data_02050ac0[64];

// Interrupt mask bit of the timer that drives the player.
#define TRACKER_IRQ_TIMER2 0x20

TrackerPlayer data_020c6b14;

#pragma thumb off

// Releases everything the loaded song allocated and resets the player; with a == 1 while a song plays it
// only requests a fade-out (mode 2).
void func_020359d0(int a) {
    u32 i;
    u32 j;

    data_020c6b14.mUnk_1746 = 1;
    if (data_020c6b14.mUnk_1724 == 1 && a == 1) {
        if (data_020c6b14.mUnk_1748 != 3) {
            data_020c6b14.mUnk_1748 = 2;
            data_020c6b14.mUnk_1746 = 0;
            return;
        }
    } else {
        data_020c6b14.mUnk_1748 = 0;
    }
    if (data_020c6b14.mUnk_1724 == 1) {
        func_02035e40();
    }
    for (i = 0; i < TRACKER_INSTRUMENT_COUNT; i++) {
        if (data_020c6b14.mUnk_410[i] != 0) {
            for (j = 0; j < data_020c6b14.mUnk_410[i]->mUnk_26c; j++) {
                if (data_020c6b14.mUnk_410[i]->mUnk_000[j]->mUnk_0c != 0) {
                    func_0201c4f0(&data_020c6b14.mUnk_410[i]->mUnk_000[j]->mUnk_0c);
                }
                data_020c6b14.mUnk_410[i]->mUnk_000[j] = 0;
            }
            data_020c6b14.mUnk_410[i] = 0;
        }
    }
    if (data_020c6b14.mUnk_0c != 0) {
        func_0201c4f0(&data_020c6b14.mUnk_0c);
    }
    if (data_020c6b14.mUnk_08 != 0) {
        func_0201c4f0(&data_020c6b14.mUnk_08);
    }
    for (i = 0; i < TRACKER_PATTERN_COUNT; i++) {
        if (data_020c6b14.mUnk_10[i] != 0) {
            func_0201c4f0(&data_020c6b14.mUnk_10[i]->mUnk_04);
            data_020c6b14.mUnk_10[i] = 0;
        }
    }
    if (data_020c6b14.mUnk_04 != 0) {
        func_0201c4f0(&data_020c6b14.mUnk_04);
    }
    data_020c6b14.mUnk_1748 = 0;
    data_020c6b14.mUnk_172e = 0;
    data_020c6b14.mUnk_1744 = 0;
    data_020c6b14.mUnk_1747 = -128;
    data_020c6b14.mUnk_1724 = 0;
    data_020c6b14.mUnk_174a = 0;
    data_020c6b14.mUnk_174b = 0;
    data_020c6b14.mUnk_174c = 0;
    data_020c6b14.mUnk_174d = 0;
    data_020c6b14.mUnk_174e = 0;
    data_020c6b14.mUnk_174f = 0;
    data_020c6b14.mUnk_1750 = 0;
    data_020c6b14.mUnk_1751 = 0;
    for (i = 0; i < TRACKER_ORDER_COUNT; i++) {
        data_020c6b14.mUnk_1752[i] = 0;
    }
    data_020c6b14.mUnk_1718 = 0;
    data_020c18bc.mUnk_46c &= ~0x1000000;
    data_020c6b14.mUnk_1746 = 0;
}

// Returns base plus the first decimal number found in text.
u16 func_02035bcc(const u8 *text, int base) {
    int value = 0;
    int start = 0;
    int end;
    int scale;
    u32 c;

    while ((c = text[start]) != 0 && (c < '0' || c > '9')) {
        start++;
    }
    if (c == 0) {
        OS_Terminate();
    }
    end = start + 1;
    while (text[end] != 0 && text[end] >= '0' && text[end] <= '9') {
        end++;
    }
    end--;
    scale = 1;
    while (end >= start) {
        value += scale * (text[end] - '0');
        scale *= 10;
        end--;
    }
    return base + value;
}

// Resets the 32 channel states and the timing state for the loaded song, then arms the timer interrupt
// and calls the player's callback with 0.
void func_02035c7c(u8 a, s16 b) {
    u32 mask = 0;
    u32 i;
    u32 k;

    if (data_020c6b14.mUnk_174c == 0) {
        OS_Terminate();
    }
    func_020402f0();
    for (i = 0; i < data_020c6b14.mUnk_174c; i++) {
        mask |= 1 << (data_020c6b14.mUnk_1744 + i);
    }
    func_02040198(mask, 0);
    data_020c6b14.mUnk_1747 = a;
    for (i = 0; i < TRACKER_CHANNEL_COUNT; i++) {
        TrackerChannel *c = &data_020c6b14.mUnk_610[i];
        c->mUnk_10 = 0;
        c->mUnk_14 = 255;
        c->mUnk_15 = 0;
        c->mUnk_17 = 255;
        c->mUnk_16 = (u8) c->mUnk_17;
        c->mUnk_1a = 0;
        c->mUnk_00 = 0;
        c->mUnk_04 = 0;
        c->mUnk_1c = 0;
        c->mUnk_1d = 0;
        c->mUnk_06 = 0;
        c->mUnk_1e = 0;
        c->mUnk_20 = 0;
        c->mUnk_21 = 0;
        c->mUnk_08 = data_02050ac0;
        c->mUnk_22 = 0;
        c->mUnk_23 = 0;
        c->mUnk_0c = data_02050ac0;
        c->mUnk_24 = 0;
        c->mUnk_25 = 128;
        c->mUnk_26 = 128;
        c->mUnk_48 = 0x10000;
        for (k = 0; k < 14; k++) {
            c->mUnk_6a[k] = 1;
        }
    }
    data_020c6b14.mUnk_1852 = data_020c6b14.mUnk_1750;
    data_020c6b14.mUnk_1856 = data_020c6b14.mUnk_1750 - 1;
    data_020c6b14.mUnk_1858 = 0;
    data_020c6b14.mUnk_1853 = data_020c6b14.mUnk_1751;
    data_020c6b14.mUnk_171c = 8192000 / (data_020c6b14.mUnk_1751 * 50);
    data_020c6b14.mUnk_1720 = 0;
    data_020c6b14.mUnk_1738 = b - 1;
    data_020c6b14.mUnk_173a = -1;
    data_020c6b14.mUnk_1854 = 255;
    data_020c6b14.mUnk_1740 = 0;
    data_020c6b14.mUnk_1742 = 1;
    data_020c6b14.mUnk_1724 = 1;
    data_020c6b14.mUnk_1728 = 0;
    func_02036194();
    func_02035f2c();
    if (data_020c6b14.mUnk_00 != 0) {
        data_020c6b14.mUnk_00(0);
    }
}

// Stops the timer and the hardware channels, then calls the player's callback with 1.
void func_02035e40(void) {
    func_02035fc4();
    func_02035e7c();
    data_020c6b14.mUnk_1724 = 0;
    if (data_020c6b14.mUnk_00 != 0) {
        data_020c6b14.mUnk_00(1);
    }
}

// Sends the stop command to each hardware channel of the song, then sends the combined mask.
void func_02035e7c(void) {
    u32 mask = 0;
    u32 i;

    for (i = 0; i < data_020c6b14.mUnk_174c; i++) {
        func_020400f4(1 << (data_020c6b14.mUnk_1744 + i), 0, 0, 0);
        while (SND_RecvCommandReply(0) != 0) {
        }
        SND_FlushCommand(2);
    }
    for (i = 0; i < data_020c6b14.mUnk_174c; i++) {
        mask |= 1 << (data_020c6b14.mUnk_1744 + i);
    }
    func_020401b8(mask, 0);
}

// Installs the timer interrupt handler and starts the timer at the rate the flags select.
void func_02035f2c(void) {
    OS_SetIrqFunction(TRACKER_IRQ_TIMER2, func_02035fd4);
    OS_EnableIrqMask(TRACKER_IRQ_TIMER2);
    if (data_020c6b14.mUnk_1710 & 2) {
        if (data_020c6b14.mUnk_1853 != 125) {
            data_020c6b14.mUnk_1710 |= 1;
        } else {
            data_020c6b14.mUnk_1710 &= ~1;
        }
    }
    if (data_020c6b14.mUnk_1710 & 1) {
        data_020c6b14.mUnk_1714 = 0x147;
        func_0203f004(2, 654, 2);
    } else {
        data_020c6b14.mUnk_1714 = 0x51e;
        func_0203f004(2, 0xa3a, 2);
    }
}

void func_02035fc4(void) {
    func_0203f024(2);
}

// Timer interrupt handler: runs one player tick, or steps the fade-out while stopping (modes 1 and 2).
void func_02035fd4(void) {
    data_020c6b14.mUnk_1718 = 0;
    if (data_020c6b14.mUnk_1728 == 1) {
        *(vu32 *) (HW_DTCM + 0x3ff8) |= TRACKER_IRQ_TIMER2;
        func_02035fc4();
        func_02035f2c();
        return;
    }
    if (data_020c6b14.mUnk_1724 == 1 && data_020c6b14.mUnk_1745 == 0 && data_020c6b14.mUnk_1746 == 0) {
        u8 mode = data_020c6b14.mUnk_1748;
        u8 vol;

        if (mode == 0) {
            *(vu32 *) (HW_DTCM + 0x3ff8) |= TRACKER_IRQ_TIMER2;
            func_02035fc4();
            func_02036198();
            func_02035f2c();
            return;
        }
        // Only modes 1 and 2 (fading out) go on.
        if ((u8) (mode + 255) > 1) {
            return;
        }
        *(vu32 *) (HW_DTCM + 0x3ff8) |= TRACKER_IRQ_TIMER2;
        func_02035fc4();
        vol = func_02038694();
        if (vol >= 4) {
            func_02038578(vol - 4, 1);
            func_02036198();
            func_02035f2c();
            return;
        }
        if (data_020c6b14.mUnk_1748 == 1) {
            data_020c6b14.mUnk_1748 = 3;
            return;
        }
        if (data_020c6b14.mUnk_00 != 0) {
            data_020c6b14.mUnk_00(3);
        }
        func_020359d0(0);
    }
}

// Frame update: counts the frames a stop has been pending and, in mode 3, calls the player's callback with 2
// and starts the next song.
void func_020360f4(void) {
    if (data_020c6b14.mUnk_1748 != 0) {
        data_020c6b14.mUnk_1718++;
        if (data_020c6b14.mUnk_1718 > 8) {
            func_020359d0(0);
        }
    }
    if (data_020c6b14.mUnk_1748 != 3) {
        return;
    }
    if (data_020c6b14.mUnk_00 != 0) {
        data_020c6b14.mUnk_00(2);
    }
    func_02035108(data_020c6b14.mUnk_1730, data_020c6b14.mUnk_1732, data_020c6b14.mUnk_1749, data_020c6b14.mUnk_173c, 0,
                  data_020c6b14.mUnk_1734);
}

void func_02036194(void) {}

#pragma thumb on
