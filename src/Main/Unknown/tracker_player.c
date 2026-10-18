#include <nitro/mi.h>
#include <nitro/os.h>
#include <nitro/snd.h>
#include <Main/Unknown/functions.h>
#include <Main/Unknown/globals.h>
#include <Main/Unknown/system_init.h>
#include <Main/Unknown/file_loader.h>
#include <Main/Unknown/wave_player.h>
#include <Main/Unknown/tracker_player.h>
#include <Main/clib.h>
#include <stdio.h>

// One period of a sine wave, 64 entries with amplitude 255; every channel points at it twice.
extern const s16 data_02050ac0[64];
// A curve falling from 255 to -255 over 64 entries.
extern const s16 data_02050a40[64];
// A square wave: 32 entries of -255, then 32 of 255.
extern const s16 data_020509c0[64];
// Volume levels for the sound command, indexed 0..64.
extern const s32 data_02050b40[65];
// Playback rates of the linear note scale: 768 entries per octave.
extern const u32 data_02050c44[768];
// Periods per note for each of the 16 fine-tune steps.
extern const u16 data_02051844[16][96];

// Interrupt mask bit of the timer that drives the player.
#define TRACKER_IRQ_TIMER2 0x20

TrackerPlayer data_020c6b14;

// Wave shapes the vibrato and tremolo effects select (low three bits of the parameter).
const s16 *data_02063c9c[8] = {data_02050ac0, data_02050a40, data_020509c0, data_02050ac0,
                               data_02050ac0, data_02050a40, data_020509c0, data_02050ac0};

#pragma thumb off

// Loads song a (a file id) and starts it with volume c (at most 0x80; 0 while data_020c18bc.mUnk_493 is 0). The song
// already loaded with nothing pending only takes the new volume. b is the number added to each sample name's number to
// get the sample's file id (0 keeps the last one); d goes to func_02035c7c. With e == 1 while a song plays the request
// is stored for func_020360f4 to replay. A new song's file gives the header bytes, the order table, the patterns (a
// cell byte with bit 7 set says which of the five bytes follow; the others become 0xff, 0, 0, 0xff, 0), the instruments
// with their two envelopes, and their samples, whose data is loaded by the number in the sample's name.
void func_02035108(u16 a, u16 b, u8 c, u16 d, int e, int f) {
    u8 name[0x17];
    u8 *patterns;
    TrackerInstrument *instruments;
    u8 *next;
    u32 size;
    u32 i;
    u8 *src;
    u8 *pattern;
    u32 j;
    TrackerSample *samples;
    u32 left;

    if (c > 0x80) {
        OS_Terminate();
    }
    if (data_020c18bc.mUnk_493 == 0) {
        c = 0;
    }
    if (a == data_020c6b14.mUnk_172e && data_020c6b14.mUnk_1748 == 0 && f == 0) {
        func_02038578(c, 0);
        return;
    }
    data_020c6b14.mUnk_1745 = 1;
    if (b == 0) {
        if (!(data_020c18bc.mUnk_46c & 0x1000000)) {
            OS_Terminate();
        }
        b = data_020c6b14.mUnk_172c;
    } else {
        data_020c6b14.mUnk_172c = b;
    }
    if (data_020c6b14.mUnk_1724 == 1 && e == 1) {
        if (data_020c6b14.mUnk_1748 != 3) {
            data_020c6b14.mUnk_1748 = 1;
        }
        data_020c6b14.mUnk_1730 = a;
        if (b != 0) {
            data_020c6b14.mUnk_1732 = b;
        }
        data_020c6b14.mUnk_1749 = c;
        data_020c6b14.mUnk_173c = d;
        data_020c6b14.mUnk_1734 = f;
        data_020c6b14.mUnk_1745 = 0;
        return;
    }
    data_020c6b14.mUnk_1748 = 0;
    if (a != data_020c6b14.mUnk_172e) {
        func_020359d0(0);
    }
    if (data_020c6b14.mUnk_1724 == 1) {
        func_02035e40();
    }
    if (data_020c6b14.mUnk_172e == 0) {
        if (a == 0) {
            OS_Terminate();
        }
        data_020c6b14.mUnk_172e = a;
        src = func_0202ded8(data_020c6b14.mUnk_172e);
        if (src == NULL) {
            OS_Terminate();
        }
        data_020c6b14.mUnk_174a = (src[0x41] << 8) | src[0x40];
        data_020c6b14.mUnk_174b = (src[0x43] << 8) | src[0x42];
        data_020c6b14.mUnk_174c = (src[0x45] << 8) | src[0x44];
        data_020c6b14.mUnk_174d = (src[0x47] << 8) | src[0x46];
        data_020c6b14.mUnk_174e = (src[0x49] << 8) | src[0x48];
        data_020c6b14.mUnk_174f = (src[0x4b] << 8) | src[0x4a];
        data_020c6b14.mUnk_1750 = (src[0x4d] << 8) | src[0x4c];
        data_020c6b14.mUnk_1751 = (src[0x4f] << 8) | src[0x4e];
        src += 0x50;
        for (i = 0; i < TRACKER_ORDER_COUNT; i++) {
            data_020c6b14.mUnk_1752[i] = *src++;
        }

        patterns = func_0201c430(data_020c6b14.mUnk_174d * sizeof(TrackerPattern));
        data_020c6b14.mUnk_04 = (TrackerPattern *) patterns;
        if (patterns == NULL) {
            OS_Terminate();
        }
        for (i = 0; i < data_020c6b14.mUnk_174d; i++) {
            u32 row;
            u32 channel;

            data_020c6b14.mUnk_10[i] = (TrackerPattern *) patterns;
            patterns += sizeof(TrackerPattern);
            data_020c6b14.mUnk_10[i]->mUnk_00 = src[4];
            data_020c6b14.mUnk_10[i]->mUnk_01 = (src[6] << 8) | src[5];
            size = (src[8] << 8) | src[7];
            src += 9;
            next = src;
            data_020c6b14.mUnk_10[i]->mUnk_04 =
                func_0201c430(data_020c6b14.mUnk_174c * data_020c6b14.mUnk_10[i]->mUnk_01 * 5);
            if (data_020c6b14.mUnk_10[i]->mUnk_04 == NULL) {
                OS_Terminate();
            }
            pattern = data_020c6b14.mUnk_10[i]->mUnk_04;
            for (row = 0; row < data_020c6b14.mUnk_10[i]->mUnk_01; row++) {
                for (channel = 0; channel < data_020c6b14.mUnk_174c; channel++) {
                    u8 flags = *src++;

                    if (flags & 0x80) {
                        if (flags & 1) {
                            *pattern++ = *src++;
                        } else {
                            *pattern++ = 0xff;
                        }
                        if (flags & 2) {
                            *pattern++ = *src++;
                        } else {
                            *pattern++ = 0;
                        }
                        if (flags & 4) {
                            *pattern++ = *src++;
                        } else {
                            *pattern++ = 0;
                        }
                        if (flags & 8) {
                            *pattern++ = *src++;
                        } else {
                            *pattern++ = 0xff;
                        }
                        if (flags & 0x10) {
                            *pattern++ = *src++;
                        } else {
                            *pattern++ = 0;
                        }
                    } else {
                        pattern[0] = flags;
                        pattern[1] = src[0];
                        pattern[2] = src[1];
                        pattern[3] = src[2];
                        pattern[4] = src[3];
                        src += 4;
                        pattern += 5;
                    }
                }
            }
            src = next + size;
        }

        instruments = func_0201c430(data_020c6b14.mUnk_174e * sizeof(TrackerInstrument));
        data_020c6b14.mUnk_08 = instruments;
        if (instruments == NULL) {
            OS_Terminate();
        }
        left = data_020c6b14.mUnk_174e * 2;
        samples = func_0201c430(left * sizeof(TrackerSample));
        data_020c6b14.mUnk_0c = samples;
        if (samples == NULL) {
            OS_Terminate();
        }
        for (i = 0; i < data_020c6b14.mUnk_174e; i++) {
            u8 *start;
            u32 header;

            start = src;
            data_020c6b14.mUnk_410[i] = instruments;
            instruments++;
            header = (src[3] << 24) | (src[2] << 16) | (src[1] << 8) | src[0];
            data_020c6b14.mUnk_410[i]->mUnk_26c = (src[0x1c] << 8) | src[0x1b];
            if (data_020c6b14.mUnk_410[i]->mUnk_26c != 0) {
                src += 0x21;
                for (j = 0; j < 0x60; j++) {
                    data_020c6b14.mUnk_410[i]->mUnk_26d[j] = *src++;
                }
                for (j = 0; j < 12; j++) {
                    data_020c6b14.mUnk_410[i]->mUnk_200.mUnk_00[j] = (src[1] << 8) | src[0];
                    data_020c6b14.mUnk_410[i]->mUnk_200.mUnk_18[j] = (src[3] << 8) | src[2];
                    src += 4;
                }
                for (j = 0; j < 12; j++) {
                    data_020c6b14.mUnk_410[i]->mUnk_236.mUnk_00[j] = (src[1] << 8) | src[0];
                    data_020c6b14.mUnk_410[i]->mUnk_236.mUnk_18[j] = (src[3] << 8) | src[2];
                    src += 4;
                }
                data_020c6b14.mUnk_410[i]->mUnk_200.mUnk_30 = src[0];
                data_020c6b14.mUnk_410[i]->mUnk_236.mUnk_30 = src[1];
                data_020c6b14.mUnk_410[i]->mUnk_200.mUnk_31 = src[2];
                data_020c6b14.mUnk_410[i]->mUnk_200.mUnk_32 = src[3];
                data_020c6b14.mUnk_410[i]->mUnk_200.mUnk_33 = src[4];
                data_020c6b14.mUnk_410[i]->mUnk_236.mUnk_31 = src[5];
                data_020c6b14.mUnk_410[i]->mUnk_236.mUnk_32 = src[6];
                data_020c6b14.mUnk_410[i]->mUnk_236.mUnk_33 = src[7];
                data_020c6b14.mUnk_410[i]->mUnk_200.mUnk_34 = src[8];
                data_020c6b14.mUnk_410[i]->mUnk_236.mUnk_34 = src[9];
                data_020c6b14.mUnk_410[i]->mUnk_2cd = src[10];
                data_020c6b14.mUnk_410[i]->mUnk_2ce = src[11];
                data_020c6b14.mUnk_410[i]->mUnk_2cf = src[12];
                data_020c6b14.mUnk_410[i]->mUnk_2d0 = src[13];
                data_020c6b14.mUnk_410[i]->mUnk_2d2 = ((src[15] << 8) | src[14]) * 2;
                src += 16;
                src += header - (src - start);
                for (j = 0; j < data_020c6b14.mUnk_410[i]->mUnk_26c; j++) {
                    if (left == 0) {
                        OS_Terminate();
                    }
                    left--;
                    data_020c6b14.mUnk_410[i]->mUnk_000[j] = samples;
                    samples++;
                    data_020c6b14.mUnk_410[i]->mUnk_000[j]->mUnk_00 =
                        (src[3] << 24) | (src[2] << 16) | (src[1] << 8) | src[0];
                    data_020c6b14.mUnk_410[i]->mUnk_000[j]->mUnk_04 =
                        (src[7] << 24) | (src[6] << 16) | (src[5] << 8) | src[4];
                    data_020c6b14.mUnk_410[i]->mUnk_000[j]->mUnk_08 =
                        (src[11] << 24) | (src[10] << 16) | (src[9] << 8) | src[8];
                    data_020c6b14.mUnk_410[i]->mUnk_000[j]->mUnk_12 = src[12];
                    data_020c6b14.mUnk_410[i]->mUnk_000[j]->mUnk_13 = src[13];
                    data_020c6b14.mUnk_410[i]->mUnk_000[j]->mUnk_14 = src[14];
                    data_020c6b14.mUnk_410[i]->mUnk_000[j]->mUnk_16 = src[15];
                    data_020c6b14.mUnk_410[i]->mUnk_000[j]->mUnk_15 = src[16];
                    if (data_020c6b14.mUnk_410[i]->mUnk_000[j]->mUnk_00 != 0) {
                        MI_CpuCopy8(src + 0x12, name, 0x16);
                        name[0x16] = 0;
                        data_020c6b14.mUnk_410[i]->mUnk_000[j]->mUnk_10 = func_02035bcc(name, b);
                        data_020c6b14.mUnk_410[i]->mUnk_000[j]->mUnk_0c =
                            func_0202e86c(data_020c6b14.mUnk_410[i]->mUnk_000[j]->mUnk_10);
                    }
                    src += 0x28;
                }
            } else {
                src += header - (src - start);
            }
        }
    }
    data_020c6b14.mUnk_1744 = 16 - data_020c6b14.mUnk_174c;
    data_020c6b14.mUnk_1718 = 0;
    data_020c18bc.mUnk_46c |= 0x1000000;
    data_020c648c.mUnk_181 = data_020c6b14.mUnk_1744;
    func_0202f084();
    func_02035c7c(c, (s16) d);
    data_020c6b14.mUnk_1745 = 0;
}

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

// Player tick, run from the timer interrupt. Each time the tick accumulator passes the tick length the tick counter
// advances: on the first tick of a row the row's cells are read (note, instrument, volume column, effect, parameter;
// the volume column is applied as a second effect), the samples of new notes are started and the first-tick effects
// applied; on the other ticks the running effects and envelopes update period, volume and pan. The changes go to the
// sound driver as channel commands. With flag 0x400 of data_020c18bc.mUnk_2c8 each channel's effect parameter is
// formatted for display.
void func_02036198(void) {
    s8 globalVol;
    char text[3];
    BOOL volChanged;
    u8 *cell;
    s8 nextRow;
    u32 i;
    u8 volume;
    u8 cellEffect;
    BOOL trigger;
    BOOL periodUpd;
    BOOL panUpd;
    BOOL notePresent;
    BOOL instPresent;
    BOOL instChanged;
    BOOL porta;
    u32 sampleOffset;
    BOOL tickPanUpd;
    u32 tickK;
    u8 note;
    u8 inst;
    u8 vol;
    u8 cellParam;
    u16 param;
    u16 hi;
    int lo;
    u16 period;
    BOOL volUpd;
    u8 effect;
    TrackerChannel *c;
    u32 k;

    volChanged = FALSE;
    globalVol = data_020c6b14.mUnk_1747;
    data_020c6b14.mUnk_1720 += data_020c6b14.mUnk_1714;
    if (data_020c6b14.mUnk_1720 >= data_020c6b14.mUnk_171c) {
        data_020c6b14.mUnk_1720 -= data_020c6b14.mUnk_171c;
        nextRow = -1;
        data_020c6b14.mUnk_1856++;
        if (data_020c6b14.mUnk_1856 >= data_020c6b14.mUnk_1852 + data_020c6b14.mUnk_1858) {
            data_020c6b14.mUnk_1856 = 0;
        }
        if (data_020c6b14.mUnk_1856 == 0) {
            data_020c6b14.mUnk_1858 = 0;
            if (data_020c6b14.mUnk_1742 > data_020c6b14.mUnk_1740) {
                data_020c6b14.mUnk_1742 = 0;
                func_02037c10();
            }
            // Five bytes per cell, one cell per channel.
            cell = (u8 *) data_020c6b14.mUnk_10[data_020c6b14.mUnk_1854]->mUnk_04 +
                   data_020c6b14.mUnk_1742 * data_020c6b14.mUnk_174c * 5;
            for (i = 0; i < data_020c6b14.mUnk_174c; i++) {
                c = &data_020c6b14.mUnk_610[i];
                trigger = FALSE;
                c->mUnk_4c = 0;
                effect = TRACKER_FX_NONE;
                note = cell[0];
                periodUpd = FALSE;
                volUpd = FALSE;
                panUpd = FALSE;
                notePresent = FALSE;
                instPresent = FALSE;
                instChanged = FALSE;
                porta = FALSE;
                sampleOffset = 0;
                if (note != TRACKER_NOTE_OFF) {
                    inst = cell[1];
                    vol = cell[2];
                    cellEffect = cell[3];
                    cellParam = cell[4];
                    effect = cellEffect;
                    param = cellParam;
                    if (cellEffect != TRACKER_FX_NONE) {
                        hi = (param & 0xf0) >> 4;
                        lo = param & 0xf;
                        switch (cellEffect) {
                            case TRACKER_FX_ORDER_JUMP:
                                if (data_020c6b14.mUnk_00 != 0 && cellParam <= data_020c6b14.mUnk_1738) {
                                    data_020c6b14.mUnk_00(4);
                                }
                                data_020c6b14.mUnk_1742 = data_020c6b14.mUnk_1740;
                                data_020c6b14.mUnk_1738 = cellParam - 1;
                                break;
                            case TRACKER_FX_ROW_BREAK:
                                param = hi * 10 + lo;
                                if (param < data_020c6b14.mUnk_1740) {
                                    nextRow = param;
                                } else {
                                    nextRow = 0;
                                }
                                break;
                            case TRACKER_FX_SPEED:
                                if (cellParam < 32) {
                                    if (cellParam != 0) {
                                        data_020c6b14.mUnk_1852 = cellParam;
                                    }
                                } else {
                                    data_020c6b14.mUnk_1853 = cellParam;
                                    data_020c6b14.mUnk_171c = 8192000 / (data_020c6b14.mUnk_1853 * 50);
                                }
                                break;
                        }
                    }
                    if (inst != 0) {
                        if (inst > data_020c6b14.mUnk_174e) {
                            OS_Terminate();
                        }
                        if (data_020c6b14.mUnk_410[inst - 1]->mUnk_26c == 0) {
                            OS_Terminate();
                        }
                        instPresent = TRUE;
                        if (c->mUnk_15 != inst || c->mUnk_10 == 1) {
                            c->mUnk_15 = inst;
                            instChanged = TRUE;
                        }
                        c->mUnk_1d = 0;
                        trigger = TRUE;
                    } else {
                        inst = c->mUnk_15;
                    }
                    if (note == TRACKER_NOTE_NONE) {
                        note = c->mUnk_14;
                    } else {
                        c->mUnk_14 = note;
                        notePresent = TRUE;
                        c->mUnk_1d = 0;
                        trigger = notePresent;
                    }
                    if (note != TRACKER_NOTE_NONE && inst != 0 && inst <= data_020c6b14.mUnk_174e) {
                        if (cellEffect == TRACKER_FX_EXTENDED && lo == TRACKER_FXE_FINETUNE) {
                            TrackerInstrument *in = data_020c6b14.mUnk_410[inst - 1];

                            in->mUnk_000[in->mUnk_26d[note - 1]]->mUnk_13 = lo;
                        }
                        if (trigger == TRUE) {
                            inst--;
                            note--;
                            if (data_020c6b14.mUnk_410[inst]->mUnk_26d[note] >=
                                data_020c6b14.mUnk_410[inst]->mUnk_26c) {
                                OS_Terminate();
                            }
                            c->mUnk_00 =
                                data_020c6b14.mUnk_410[inst]->mUnk_000[data_020c6b14.mUnk_410[inst]->mUnk_26d[note]];
                            if (c->mUnk_00 == 0) {
                                OS_Terminate();
                            }
                            if (instPresent == TRUE) {
                                volUpd = TRUE;
                                panUpd = TRUE;
                                c->mUnk_24 = c->mUnk_00->mUnk_12;
                                c->mUnk_26 = c->mUnk_00->mUnk_16;
                            }
                            period = func_02037eac((u8) (note + c->mUnk_00->mUnk_15), c->mUnk_00->mUnk_13);
                        } else {
                            period = c->mUnk_04;
                        }
                        c->mUnk_17 = TRACKER_FX_NONE;
                        c->mUnk_16 = TRACKER_FX_NONE;
                        c->mUnk_1a = 0;
                        c->mUnk_18 = 0;
                        // Pass 0 applies the volume column, pass 1 the effect column.
                        for (k = 0; k < 2; k++) {
                            effect = TRACKER_FX_NONE;
                            if (k == 0) {
                                if (vol >= 0x10 && vol <= 0x50) {
                                    effect = TRACKER_FX_VOLUME;
                                    param = vol - 0x10;
                                } else if (vol >= 0x60 && vol <= 0x70) {
                                    effect = TRACKER_FX_VOL_SLIDE;
                                    param = vol & 0xf;
                                } else if (vol >= 0x70 && vol <= 0x80) {
                                    effect = TRACKER_FX_VOL_SLIDE;
                                    param = (vol & 0xf) << 4;
                                } else if (vol >= 0x80 && vol <= 0x90) {
                                    effect = TRACKER_FX_EXTENDED;
                                    param = 0xb0 | (vol & 0xf);
                                } else if (vol >= 0x90 && vol <= 0xa0) {
                                    effect = TRACKER_FX_EXTENDED;
                                    param = 0xa0 | (vol & 0xf);
                                } else if (vol >= 0xa0 && vol <= 0xb0) {
                                    c->mUnk_20 = vol & 0xf;
                                } else if (vol >= 0xb0 && vol <= 0xc0) {
                                    c->mUnk_21 = vol & 0xf;
                                } else if (vol >= 0xc0 && vol < 0xd0) {
                                    effect = TRACKER_FX_PAN;
                                    param = (vol & 0xf) << 4;
                                } else if (vol >= 0xd0 && vol <= 0xe0) {
                                    effect = TRACKER_FX_PAN_SLIDE;
                                    param = vol & 0xf;
                                } else if (vol >= 0xe0 && vol <= 0xf0) {
                                    effect = TRACKER_FX_PAN_SLIDE;
                                    param = (vol & 0xf) << 4;
                                } else if (vol >= 0xf0) {
                                    effect = TRACKER_FX_TONE_SLIDE;
                                    param = (vol & 0xf) << 4;
                                }
                            } else {
                                effect = cellEffect;
                                param = cellParam;
                                if (effect == TRACKER_FX_NONE && cellParam != 0) {
                                    effect = TRACKER_FX_ARPEGGIO;
                                }
                            }
                            if (effect == TRACKER_FX_NONE) {
                                continue;
                            }
                            hi = (param & 0xf0) >> 4;
                            lo = param & 0xf;
                            switch (effect) {
                                case TRACKER_FX_ARPEGGIO:
                                    if (param != 0) {
                                        c->mUnk_16 = effect;
                                        c->mUnk_18 = param;
                                        c->mUnk_1c = 0;
                                    }
                                    break;
                                case TRACKER_FX_SLIDE_UP:
                                    if (param != 0) {
                                        c->mUnk_6a[0] = param;
                                    } else {
                                        param = c->mUnk_6a[0];
                                    }
                                    c->mUnk_16 = effect;
                                    c->mUnk_18 = param;
                                    if (data_020c6b14.mUnk_174f & 1) {
                                        c->mUnk_18 <<= 2;
                                    }
                                    break;
                                case TRACKER_FX_SLIDE_DOWN:
                                    if (param != 0) {
                                        c->mUnk_6a[1] = param;
                                    } else {
                                        param = c->mUnk_6a[1];
                                    }
                                    c->mUnk_16 = effect;
                                    c->mUnk_18 = param;
                                    if (data_020c6b14.mUnk_174f & 1) {
                                        c->mUnk_18 <<= 2;
                                    }
                                    break;
                                case TRACKER_FX_TONE_SLIDE:
                                case TRACKER_FX_TONE_SLIDE_VOL:
                                    if (param != 0) {
                                        if (effect == TRACKER_FX_TONE_SLIDE) {
                                            c->mUnk_6a[2] = param;
                                        } else {
                                            c->mUnk_6a[4] = param;
                                        }
                                    } else {
                                        if (effect == TRACKER_FX_TONE_SLIDE) {
                                            param = c->mUnk_6a[2];
                                        } else {
                                            param = c->mUnk_6a[4];
                                        }
                                    }
                                    if (k != 0) {
                                        c->mUnk_16 = effect;
                                        c->mUnk_18 = param;
                                    } else {
                                        c->mUnk_17 = effect;
                                        c->mUnk_1a = param;
                                    }
                                    if (notePresent == TRUE && period != 0) {
                                        c->mUnk_06 = period;
                                    }
                                    if (param != 0 && effect == TRACKER_FX_TONE_SLIDE) {
                                        c->mUnk_1e = param;
                                        if (data_020c6b14.mUnk_174f & 1) {
                                            c->mUnk_1e <<= 2;
                                        }
                                    }
                                    porta = TRUE;
                                    break;
                                case TRACKER_FX_VIBRATO:
                                    if (param != 0) {
                                        c->mUnk_6a[3] = param;
                                    } else {
                                        param = c->mUnk_6a[3];
                                        hi = (param & 0xf0) >> 4;
                                        lo = param & 0xf;
                                    }
                                    c->mUnk_16 = effect;
                                    c->mUnk_18 = param;
                                    if (hi != 0 && lo != 0) {
                                        c->mUnk_20 = hi;
                                        c->mUnk_21 = lo;
                                    }
                                    break;
                                case TRACKER_FX_PAN:
                                    c->mUnk_26 = param;
                                    panUpd = TRUE;
                                    break;
                                case TRACKER_FX_TREMOLO:
                                    if (param != 0) {
                                        c->mUnk_6a[6] = param;
                                    } else {
                                        param = c->mUnk_6a[6];
                                        hi = (param & 0xf0) >> 4;
                                        lo = param & 0xf;
                                    }
                                    c->mUnk_16 = effect;
                                    c->mUnk_18 = param;
                                    if (hi != 0 && lo != 0) {
                                        c->mUnk_22 = hi;
                                        c->mUnk_23 = lo;
                                    }
                                    break;
                                case TRACKER_FX_SAMPLE_OFFSET:
                                    if (param != 0) {
                                        sampleOffset = param << 8;
                                        trigger = TRUE;
                                    }
                                    break;
                                case TRACKER_FX_VIBRATO_VOL:
                                    if (param != 0) {
                                        c->mUnk_6a[5] = param;
                                    } else {
                                        param = c->mUnk_6a[5];
                                    }
                                    c->mUnk_16 = effect;
                                    c->mUnk_18 = param;
                                    break;
                                case TRACKER_FX_VOL_SLIDE:
                                    if (param != 0) {
                                        c->mUnk_6a[7] = param;
                                    } else {
                                        param = c->mUnk_6a[7];
                                    }
                                    if (k != 0) {
                                        c->mUnk_16 = effect;
                                        c->mUnk_18 = param;
                                    } else {
                                        c->mUnk_17 = effect;
                                        c->mUnk_1a = param;
                                    }
                                    break;
                                case TRACKER_FX_VOLUME:
                                    if (param > 64) {
                                        param = 64;
                                    }
                                    c->mUnk_24 = param;
                                    volUpd = TRUE;
                                    break;
                                case TRACKER_FX_EXTENDED:
                                    c->mUnk_16 = effect;
                                    c->mUnk_18 = param;
                                    switch (hi) {
                                        case TRACKER_FXE_VIBRATO_WAVE:
                                            c->mUnk_08 = data_02063c9c[lo & 7];
                                            break;
                                        case TRACKER_FXE_TREMOLO_WAVE:
                                            c->mUnk_0c = data_02063c9c[lo & 7];
                                            break;
                                        case TRACKER_FXE_RETRIGGER:
                                            c->mUnk_4c = TRACKER_START_RETRIGGER;
                                            c->mUnk_4d = 0;
                                            c->mUnk_4e = lo;
                                            break;
                                        case TRACKER_FXE_PAN:
                                            c->mUnk_26 = lo << 4;
                                            panUpd = TRUE;
                                            break;
                                        case TRACKER_FXE_NOTE_DELAY:
                                            c->mUnk_4c = TRACKER_START_DELAYED;
                                            c->mUnk_4d = 0;
                                            c->mUnk_4e = lo;
                                            break;
                                        case TRACKER_FXE_ROW_DELAY:
                                            if (lo != 0) {
                                                data_020c6b14.mUnk_1858 = (lo - 1) * data_020c6b14.mUnk_1852;
                                            }
                                            break;
                                    }
                                    break;
                                case TRACKER_FX_GLOBAL_VOLUME:
                                    if (data_020c6b14.mUnk_1748 != 1 && data_020c6b14.mUnk_1748 != 2) {
                                        globalVol = param << 1;
                                        if (globalVol > 128) {
                                            globalVol = 128;
                                        }
                                        volChanged = TRUE;
                                    }
                                    break;
                                case TRACKER_FX_GLOBAL_VOL_SLIDE:
                                    if (data_020c6b14.mUnk_1748 != 1 && data_020c6b14.mUnk_1748 != 2) {
                                        if (param != 0) {
                                            c->mUnk_6a[12] = param;
                                        } else {
                                            param = c->mUnk_6a[12];
                                        }
                                        c->mUnk_16 = effect;
                                        c->mUnk_18 = param;
                                    }
                                    break;
                                case TRACKER_FX_PAN_SLIDE:
                                    if (param != 0) {
                                        c->mUnk_6a[13] = param;
                                    } else {
                                        param = c->mUnk_6a[13];
                                    }
                                    if (k != 0) {
                                        c->mUnk_16 = effect;
                                        c->mUnk_18 = param;
                                    } else {
                                        c->mUnk_17 = effect;
                                        c->mUnk_1a = param;
                                    }
                                    break;
                            }
                        }
                        if (porta == FALSE) {
                            c->mUnk_04 = period;
                        } else {
                            period = c->mUnk_04;
                        }
                        volume = c->mUnk_24;
                        if (porta == TRUE && instChanged == FALSE) {
                            trigger = FALSE;
                        }
                        // First-tick part of the two stored effects.
                        for (k = 0; k < 2; k++) {
                            if (k == 0) {
                                param = c->mUnk_1a;
                                effect = c->mUnk_17;
                            } else {
                                param = c->mUnk_18;
                                effect = c->mUnk_16;
                            }
                            hi = (param & 0xf0) >> 4;
                            lo = param & 0xf;
                            switch (effect) {
                                case TRACKER_FX_ARPEGGIO:
                                    period = c->mUnk_04;
                                    periodUpd = TRUE;
                                    break;
                                case TRACKER_FX_SLIDE_UP:
                                    if (data_020c6b14.mUnk_174f & 1) {
                                        c->mUnk_04 -= param;
                                        if (c->mUnk_04 < TRACKER_PERIOD_MIN) {
                                            c->mUnk_04 = TRACKER_PERIOD_MIN;
                                        }
                                    }
                                    period = c->mUnk_04;
                                    periodUpd = TRUE;
                                    break;
                                case TRACKER_FX_SLIDE_DOWN:
                                    if (data_020c6b14.mUnk_174f & 1) {
                                        c->mUnk_04 += param;
                                    }
                                    period = c->mUnk_04;
                                    periodUpd = TRUE;
                                    break;
                                case TRACKER_FX_TONE_SLIDE:
                                case TRACKER_FX_TONE_SLIDE_VOL:
                                    period = c->mUnk_04;
                                    periodUpd = TRUE;
                                    if (effect == TRACKER_FX_TONE_SLIDE_VOL) {
                                        func_02037df8(c, (s8) hi, (s8) lo);
                                        volume = c->mUnk_24;
                                        volUpd = TRUE;
                                    }
                                    break;
                                case TRACKER_FX_VIBRATO:
                                case TRACKER_FX_VIBRATO_VOL:
                                    period = func_02037d38(c);
                                    periodUpd = TRUE;
                                    if (effect == TRACKER_FX_VIBRATO_VOL) {
                                        func_02037df8(c, (s8) hi, (s8) lo);
                                        volume = c->mUnk_24;
                                        volUpd = TRUE;
                                    }
                                    break;
                                case TRACKER_FX_TREMOLO:
                                    volume = func_02037dac(c);
                                    volUpd = TRUE;
                                    break;
                                case TRACKER_FX_VOL_SLIDE:
                                    func_02037df8(c, (s8) hi, (s8) lo);
                                    volume = c->mUnk_24;
                                    volUpd = TRUE;
                                    break;
                                case TRACKER_FX_EXTENDED:
                                    switch (hi) {
                                        case TRACKER_FXE_FINE_SLIDE_UP:
                                            if (lo != 0) {
                                                c->mUnk_6a[8] = lo;
                                            } else {
                                                lo = c->mUnk_6a[8];
                                            }
                                            if (data_020c6b14.mUnk_174f & 1) {
                                                c->mUnk_04 -= lo << 2;
                                            } else {
                                                c->mUnk_04 -= lo;
                                            }
                                            if (c->mUnk_04 < TRACKER_PERIOD_MIN) {
                                                c->mUnk_04 = TRACKER_PERIOD_MIN;
                                            }
                                            period = c->mUnk_04;
                                            periodUpd = TRUE;
                                            break;
                                        case TRACKER_FXE_FINE_SLIDE_DOWN:
                                            if (lo != 0) {
                                                c->mUnk_6a[9] = lo;
                                            } else {
                                                lo = c->mUnk_6a[9];
                                            }
                                            if (data_020c6b14.mUnk_174f & 1) {
                                                c->mUnk_04 += lo << 2;
                                            } else {
                                                c->mUnk_04 += lo;
                                            }
                                            period = c->mUnk_04;
                                            periodUpd = TRUE;
                                            break;
                                        case TRACKER_FXE_FINE_VOL_UP:
                                            if (lo != 0) {
                                                c->mUnk_6a[10] = lo;
                                            } else {
                                                lo = c->mUnk_6a[10];
                                            }
                                            if (lo != 0) {
                                                func_02037df8(c, (s8) lo, 0);
                                                volume = c->mUnk_24;
                                                volUpd = TRUE;
                                            }
                                            break;
                                        case TRACKER_FXE_FINE_VOL_DOWN:
                                            if (lo != 0) {
                                                c->mUnk_6a[11] = lo;
                                            } else {
                                                lo = c->mUnk_6a[11];
                                            }
                                            if (lo != 0) {
                                                func_02037df8(c, 0, (s8) lo);
                                                volume = c->mUnk_24;
                                                volUpd = TRUE;
                                            }
                                            break;
                                    }
                                    break;
                                case TRACKER_FX_GLOBAL_VOL_SLIDE:
                                    if (data_020c6b14.mUnk_1748 != 1 && data_020c6b14.mUnk_1748 != 2) {
                                        func_02037e70(&globalVol, (s8) hi, (s8) lo);
                                        volChanged = TRUE;
                                    }
                                    break;
                                case TRACKER_FX_PAN_SLIDE:
                                    func_02037e34(c, (s8) hi, (s8) lo);
                                    panUpd = TRUE;
                                    break;
                            }
                        }
                        if (trigger == TRUE) {
                            u32 freq;
                            u16 chVol;
                            TrackerSample *s;
                            u32 data;

                            freq = func_02037efc(period, (data_020c6b14.mUnk_174f & 1) ? TRUE : FALSE);
                            c->mUnk_10 = 0;
                            if (cellEffect != TRACKER_FX_ENV_POS) {
                                func_02037f5c(&data_020c6b14, c, TRACKER_ENV_VOLUME);
                                func_02037f5c(&data_020c6b14, c, TRACKER_ENV_PAN);
                            } else {
                                func_020382c0(&data_020c6b14, c, TRACKER_ENV_VOLUME, cellParam);
                                func_020382c0(&data_020c6b14, c, TRACKER_ENV_PAN, cellParam);
                            }
                            c->mUnk_48 = 0x10000;
                            chVol = func_02040b60(data_02050b40[func_020384f8(
                                (s8) volume, c->mUnk_28.mUnk_04, c->mUnk_48, data_020c6b14.mUnk_1747, c->mUnk_25)]);
                            if (c->mUnk_4c != TRACKER_START_DELAYED) {
                                func_020400f4(1 << (data_020c6b14.mUnk_1744 + i), 0, 0, 0);
                            }
                            s = c->mUnk_00;
                            // A sample without a loop plays once from the offset; a looped one plays from its start.
                            if ((s->mUnk_14 & 3) == 0 || s->mUnk_08 == 0) {
                                if (sampleOffset >= s->mUnk_00) {
                                    goto next;
                                }
                                data = (u32) s->mUnk_0c;
                                if (data == 0) {
                                    goto next;
                                }
                                if (c->mUnk_4c != 0) {
                                    c->mUnk_4f = (s->mUnk_14 & 0x10) ? TRUE : FALSE;
                                    c->mUnk_54 = data + sampleOffset;
                                    c->mUnk_58 = (c->mUnk_00->mUnk_00 - sampleOffset) >> 2;
                                    c->mUnk_5c = 0;
                                    c->mUnk_52 = chVol;
                                    c->mUnk_60 = 0xffb0ff / freq;
                                    c->mUnk_50 = func_02038524(c->mUnk_26, c->mUnk_38.mUnk_04) >> 1;
                                }
                                if (c->mUnk_4c == TRACKER_START_DELAYED) {
                                    goto next;
                                }
                                c->mUnk_64 = 0xffb0ff / freq;
                                c->mUnk_68 = chVol;
                                func_02040240(data_020c6b14.mUnk_1744 + i, (c->mUnk_00->mUnk_14 & 0x10) ? TRUE : FALSE,
                                              data + sampleOffset, 2, 0, (c->mUnk_00->mUnk_00 - sampleOffset) >> 2,
                                              c->mUnk_68 & 0xff, c->mUnk_68 >> 8, c->mUnk_64,
                                              func_02038524(c->mUnk_26, c->mUnk_38.mUnk_04) >> 1);
                                func_020400cc(1 << (data_020c6b14.mUnk_1744 + i), 0, 0, 0);
                            } else {
                                data = (u32) s->mUnk_0c;
                                if (data == 0) {
                                    goto next;
                                }
                                if (c->mUnk_4c != 0) {
                                    c->mUnk_4f = (s->mUnk_14 & 0x10) ? TRUE : FALSE;
                                    c->mUnk_54 = data;
                                    c->mUnk_58 = c->mUnk_00->mUnk_04 >> 2;
                                    c->mUnk_5c = c->mUnk_00->mUnk_08 >> 2;
                                    c->mUnk_52 = chVol;
                                    c->mUnk_60 = 0xffb0ff / freq;
                                    c->mUnk_50 = func_02038524(c->mUnk_26, c->mUnk_38.mUnk_04) >> 1;
                                }
                                if (c->mUnk_4c == TRACKER_START_DELAYED) {
                                    goto next;
                                }
                                c->mUnk_64 = 0xffb0ff / freq;
                                c->mUnk_68 = chVol;
                                s = c->mUnk_00;
                                func_02040240(data_020c6b14.mUnk_1744 + i, (s->mUnk_14 & 0x10) ? TRUE : FALSE, data, 1,
                                              s->mUnk_04 >> 2, s->mUnk_08 >> 2, c->mUnk_68 & 0xff, c->mUnk_68 >> 8,
                                              c->mUnk_64, func_02038524(c->mUnk_26, c->mUnk_38.mUnk_04) >> 1);
                                func_020400cc(1 << (data_020c6b14.mUnk_1744 + i), 0, 0, 0);
                            }
                        } else {
                            BOOL changed;

                            if (cellEffect != TRACKER_FX_ENV_POS) {
                                changed = func_020380b8(&data_020c6b14, c, TRACKER_ENV_VOLUME, FALSE) |
                                          func_02038464(&data_020c6b14, c);
                            } else {
                                func_020382c0(&data_020c6b14, c, TRACKER_ENV_VOLUME, cellParam);
                                changed = TRUE;
                            }
                            if (changed == TRUE && volUpd == FALSE) {
                                volume = c->mUnk_24;
                                volUpd = TRUE;
                            }
                            if (cellEffect != TRACKER_FX_ENV_POS) {
                                if (func_020380b8(&data_020c6b14, c, TRACKER_ENV_PAN, FALSE) == TRUE) {
                                    panUpd = TRUE;
                                }
                            } else {
                                func_020382c0(&data_020c6b14, c, TRACKER_ENV_PAN, cellParam);
                                panUpd = TRUE;
                            }
                            if (periodUpd == TRUE) {
                                c->mUnk_64 =
                                    0xffb0ff / func_02037efc(period, (data_020c6b14.mUnk_174f & 1) ? TRUE : FALSE);
                                func_020401d8(1 << (data_020c6b14.mUnk_1744 + i), c->mUnk_64);
                            }
                            if (volUpd == TRUE) {
                                c->mUnk_68 = func_02040b60(data_02050b40[func_020384f8(
                                    (s8) volume, c->mUnk_28.mUnk_04, c->mUnk_48, data_020c6b14.mUnk_1747, c->mUnk_25)]);
                                func_020401f8(1 << (data_020c6b14.mUnk_1744 + i), c->mUnk_68 & 0xff, c->mUnk_68 >> 8);
                            }
                            if (panUpd == TRUE) {
                                func_02040220(1 << (data_020c6b14.mUnk_1744 + i),
                                              func_02038524(c->mUnk_26, c->mUnk_38.mUnk_04) >> 1);
                            }
                            if (data_020c18bc.mUnk_2c8 & 0x400) {
                                // The debug display flag is tested here, but nothing in this branch formats the
                                // parameter: the statement only keeps the test, as in the original.
                                param = (u16) param;
                            }
                        }
                    } else {
                        if (data_020c18bc.mUnk_2c8 & 0x400) {
                            effect = TRACKER_FX_NONE;
                            param = 0;
                        }
                    }
                } else {
                    BOOL changed;

                    c->mUnk_10 = 1;
                    changed =
                        func_020380b8(&data_020c6b14, c, TRACKER_ENV_VOLUME, TRUE) | func_02038464(&data_020c6b14, c);
                    if (changed == TRUE) {
                        c->mUnk_68 = func_02040b60(data_02050b40[func_020384f8(
                            c->mUnk_24, c->mUnk_28.mUnk_04, c->mUnk_48, data_020c6b14.mUnk_1747, c->mUnk_25)]);
                        func_020401f8(1 << (data_020c6b14.mUnk_1744 + i), c->mUnk_68 & 0xff, c->mUnk_68 >> 8);
                    }
                    if (func_020380b8(&data_020c6b14, c, TRACKER_ENV_PAN, TRUE) == TRUE) {
                        func_02040220(1 << (data_020c6b14.mUnk_1744 + i),
                                      func_02038524(c->mUnk_26, c->mUnk_38.mUnk_04) >> 1);
                    }
                }
            next:
                if (data_020c18bc.mUnk_2c8 & 0x400) {
                    if (effect != TRACKER_FX_NONE) {
                        sprintf(text, "%02x", param);
                    } else {
                        sprintf(text, "--");
                    }
                }
                cell += 5;
            }
            data_020c6b14.mUnk_1742++;
            if (nextRow != -1) {
                data_020c6b14.mUnk_1742 = nextRow;
                func_02037c10();
            }
        } else {
            for (i = 0; i < data_020c6b14.mUnk_174c; i++) {
                u8 tickEffect;
                u8 tickParam;
                u8 tickHi;
                u8 tickLo;
                u8 tickVolume;
                BOOL tickPeriodUpd;

                c = &data_020c6b14.mUnk_610[i];
                tickPeriodUpd = FALSE;
                volUpd = FALSE;
                tickPanUpd = FALSE;
                // A delayed note starts, or a retriggered one restarts, when its tick count is reached.
                if (c->mUnk_4c != 0) {
                    if (++c->mUnk_4d == c->mUnk_4e) {
                        if (c->mUnk_4c != TRACKER_START_DELAYED) {
                            c->mUnk_4d = 0;
                        } else {
                            c->mUnk_4c = 0;
                        }
                        func_020400f4(1 << (data_020c6b14.mUnk_1744 + i), 0, 0, 0);
                        if (c->mUnk_5c == 0) {
                            func_02040240(data_020c6b14.mUnk_1744 + i, c->mUnk_4f, c->mUnk_54, 2, 0, c->mUnk_58,
                                          c->mUnk_52 & 0xff, c->mUnk_52 >> 8, c->mUnk_60, c->mUnk_50);
                        } else {
                            func_02040240(data_020c6b14.mUnk_1744 + i, c->mUnk_4f, c->mUnk_54, 1, c->mUnk_58,
                                          c->mUnk_5c, c->mUnk_52 & 0xff, c->mUnk_52 >> 8, c->mUnk_60, c->mUnk_50);
                        }
                        func_020400cc(1 << (data_020c6b14.mUnk_1744 + i), 0, 0, 0);
                    }
                }
                for (tickK = 0; tickK < 2; tickK++) {
                    if (tickK == 0) {
                        tickParam = c->mUnk_1a;
                        tickEffect = c->mUnk_17;
                    } else {
                        tickParam = c->mUnk_18;
                        tickEffect = c->mUnk_16;
                    }
                    tickHi = (tickParam & 0xf0) >> 4;
                    tickLo = tickParam & 0xf;
                    switch (tickEffect) {
                        case TRACKER_FX_ARPEGGIO:
                            note = c->mUnk_14 - 1;
                            switch ((u8) (c->mUnk_1c % 3)) {
                                case 0:
                                    period = func_02037eac((u8) (note + c->mUnk_00->mUnk_15), c->mUnk_00->mUnk_13);
                                    break;
                                case 1:
                                    period =
                                        func_02037eac((u8) (note + tickHi + c->mUnk_00->mUnk_15), c->mUnk_00->mUnk_13);
                                    break;
                                case 2:
                                    period =
                                        func_02037eac((u8) (note + tickLo + c->mUnk_00->mUnk_15), c->mUnk_00->mUnk_13);
                                    break;
                            }
                            c->mUnk_1c++;
                            tickPeriodUpd = TRUE;
                            break;
                        case TRACKER_FX_SLIDE_UP:
                            c->mUnk_04 -= tickParam;
                            if (c->mUnk_04 < TRACKER_PERIOD_MIN) {
                                c->mUnk_04 = TRACKER_PERIOD_MIN;
                            }
                            tickPeriodUpd = TRUE;
                            period = c->mUnk_04;
                            break;
                        case TRACKER_FX_SLIDE_DOWN:
                            c->mUnk_04 += tickParam;
                            tickPeriodUpd = TRUE;
                            period = c->mUnk_04;
                            break;
                        case TRACKER_FX_TONE_SLIDE:
                        case TRACKER_FX_TONE_SLIDE_VOL:
                            func_02037cdc(c);
                            period = c->mUnk_04;
                            tickPeriodUpd = TRUE;
                            if (tickEffect == TRACKER_FX_TONE_SLIDE_VOL) {
                                func_02037df8(c, (s8) tickHi, (s8) tickLo);
                                tickVolume = c->mUnk_24;
                                volUpd = TRUE;
                            }
                            break;
                        case TRACKER_FX_VIBRATO:
                        case TRACKER_FX_VIBRATO_VOL:
                            period = func_02037d38(c);
                            tickPeriodUpd = TRUE;
                            if (tickEffect == TRACKER_FX_VIBRATO_VOL) {
                                func_02037df8(c, (s8) tickHi, (s8) tickLo);
                                tickVolume = c->mUnk_24;
                                volUpd = TRUE;
                            }
                            break;
                        case TRACKER_FX_TREMOLO:
                            tickVolume = func_02037dac(c);
                            volUpd = TRUE;
                            break;
                        case TRACKER_FX_VOL_SLIDE:
                            func_02037df8(c, (s8) tickHi, (s8) tickLo);
                            tickVolume = c->mUnk_24;
                            volUpd = TRUE;
                            break;
                        case TRACKER_FX_EXTENDED:
                            if (tickHi == TRACKER_FXE_NOTE_CUT && data_020c6b14.mUnk_1856 >= tickLo) {
                                c->mUnk_24 = 0;
                                tickVolume = 0;
                                volUpd = TRUE;
                            }
                            break;
                        case TRACKER_FX_GLOBAL_VOL_SLIDE:
                            if (data_020c6b14.mUnk_1748 != 1 && data_020c6b14.mUnk_1748 != 2) {
                                func_02037e70(&globalVol, (s8) tickHi, (s8) tickLo);
                                volChanged = TRUE;
                            }
                            break;
                        case TRACKER_FX_PAN_SLIDE:
                            func_02037e34(c, (s8) tickHi, (s8) tickLo);
                            tickPanUpd = TRUE;
                            break;
                    }
                }
                if ((func_020380b8(&data_020c6b14, c, TRACKER_ENV_VOLUME, FALSE) | func_02038464(&data_020c6b14, c)) ==
                        TRUE &&
                    volUpd == FALSE) {
                    tickVolume = c->mUnk_24;
                    volUpd = TRUE;
                }
                if (func_020380b8(&data_020c6b14, c, TRACKER_ENV_PAN, FALSE) == TRUE) {
                    tickPanUpd = TRUE;
                }
                if (tickPeriodUpd == TRUE) {
                    c->mUnk_64 = 0xffb0ff / func_02037efc(period, (data_020c6b14.mUnk_174f & 1) ? TRUE : FALSE);
                    func_020401d8(1 << (data_020c6b14.mUnk_1744 + i), c->mUnk_64);
                }
                if (volUpd == TRUE) {
                    c->mUnk_68 = func_02040b60(data_02050b40[func_020384f8(
                        (s8) tickVolume, c->mUnk_28.mUnk_04, c->mUnk_48, data_020c6b14.mUnk_1747, c->mUnk_25)]);
                    func_020401f8(1 << (data_020c6b14.mUnk_1744 + i), c->mUnk_68 & 0xff, c->mUnk_68 >> 8);
                }
                if (tickPanUpd == TRUE) {
                    func_02040220(1 << (data_020c6b14.mUnk_1744 + i),
                                  func_02038524(c->mUnk_26, c->mUnk_38.mUnk_04) >> 1);
                }
            }
        }
    }
    while (SND_RecvCommandReply(0) != 0) {
    }
    SND_FlushCommand(2);
    if (volChanged == TRUE) {
        func_02038578(globalVol, 0);
    }
}

// Advances to the next order position (or the jump target), stops the voices when the song ends or loops and
// loads the pattern's row count.
void func_02037c10(void) {
    if (data_020c6b14.mUnk_173a != -1) {
        data_020c6b14.mUnk_1738 = data_020c6b14.mUnk_173a;
        data_020c6b14.mUnk_173a = -1;
        if (data_020c6b14.mUnk_173e == 1) {
            func_02035e7c();
        }
    } else {
        data_020c6b14.mUnk_1738 = data_020c6b14.mUnk_1738 + 1;
    }
    if (data_020c6b14.mUnk_1738 >= data_020c6b14.mUnk_174a) {
        func_02035e7c();
        data_020c6b14.mUnk_1738 = data_020c6b14.mUnk_174b;
        if (data_020c6b14.mUnk_00 != 0) {
            data_020c6b14.mUnk_00(4);
        }
    }
    data_020c6b14.mUnk_1854 = data_020c6b14.mUnk_1752[data_020c6b14.mUnk_1738];
    data_020c6b14.mUnk_1740 = data_020c6b14.mUnk_10[data_020c6b14.mUnk_1854]->mUnk_01 - 1;
}

// Moves the channel's period toward its target by the slide speed.
void func_02037cdc(TrackerChannel *c) {
    if (c->mUnk_04 < c->mUnk_06) {
        c->mUnk_04 = c->mUnk_04 + c->mUnk_1e;
        if (c->mUnk_04 > c->mUnk_06) {
            c->mUnk_04 = c->mUnk_06;
        }
    }
    if (c->mUnk_04 > c->mUnk_06) {
        c->mUnk_04 = c->mUnk_04 - c->mUnk_1e;
        if (c->mUnk_04 < c->mUnk_06) {
            c->mUnk_04 = c->mUnk_06;
        }
    }
}

// Period with the vibrato offset from the channel's wave table added; steps the wave position.
u16 func_02037d38(TrackerChannel *c) {
    u16 period = c->mUnk_04;
    u8 pos;

    if (data_020c6b14.mUnk_174f & 1) {
        period += (c->mUnk_21 * c->mUnk_08[c->mUnk_1d]) >> 5;
    } else {
        period += (c->mUnk_21 * c->mUnk_08[c->mUnk_1d]) >> 7;
    }
    pos = c->mUnk_1d + c->mUnk_20;
    c->mUnk_1d = pos & 0x3f;
    return period;
}

// Volume with the tremolo offset added (capped at 64); steps the wave position.
u8 func_02037dac(TrackerChannel *c) {
    u8 v = (u8) c->mUnk_24 + ((c->mUnk_23 * c->mUnk_0c[c->mUnk_1d]) >> 6);
    u8 pos;

    if (v > 64) {
        v = 64;
    }
    pos = c->mUnk_1d + c->mUnk_22;
    c->mUnk_1d = pos & 0x3f;
    return v;
}

// Volume slide: adds up when it is non-zero, subtracts down otherwise; kept within 0..64.
void func_02037df8(TrackerChannel *c, int up, int down) {
    if (up != 0) {
        c->mUnk_24 = c->mUnk_24 + up;
    } else {
        c->mUnk_24 = c->mUnk_24 - down;
    }
    if (c->mUnk_24 < 0) {
        c->mUnk_24 = 0;
    }
    if (c->mUnk_24 > 64) {
        c->mUnk_24 = 64;
    }
}

// Panning slide: adds up when it is non-zero, subtracts down otherwise; kept within 0..255.
void func_02037e34(TrackerChannel *c, int up, int down) {
    if (up != 0) {
        c->mUnk_26 = c->mUnk_26 + up;
    } else {
        c->mUnk_26 = c->mUnk_26 - down;
    }
    if (c->mUnk_26 < 0) {
        c->mUnk_26 = 0;
    }
    if (c->mUnk_26 > 255) {
        c->mUnk_26 = 255;
    }
}

// Master volume slide in steps of two: adds up when it is non-zero, subtracts down otherwise; kept within 0..128.
void func_02037e70(s8 *v, int up, int down) {
    if (up != 0) {
        *v = *v + up * 2;
    } else {
        *v = *v - down * 2;
    }
    if (*v < 0) {
        *v = 0;
    }
    if (*v > 128) {
        *v = 128;
    }
}

// Period of a note at a fine-tune value: linear scale or the period table, by the song's flag.
u16 func_02037eac(int note, int finetune) {
    if ((data_020c6b14.mUnk_174f & 1) == 0) {
        return data_02051844[(finetune >> 4) + 8][note];
    }
    return 7680 - note * 64 - (finetune >> 1);
}

// Playback rate of a period: a clock divided by twice the period, or with the linear flag the table entry
// shifted down by the octave.
u32 func_02037efc(int period, int linear) {
    if (linear == 0) {
        return 7093789 / (period * 2);
    }
    return data_02050c44[period % 768] >> (period / 768);
}

// Starts the volume (type 0) or panning (type 1) envelope of a channel's instrument; without a usable instrument
// the level is set to its default.
void func_02037f5c(TrackerPlayer *p, TrackerChannel *c, int type) {
    TrackerEnvelope *env;
    TrackerEnvState *st;
    u8 inst = c->mUnk_15;

    if (inst != 0 && inst <= data_020c6b14.mUnk_174e) {
        switch (type) {
            case TRACKER_ENV_VOLUME:
                st = &c->mUnk_28;
                env = &p->mUnk_410[inst - 1]->mUnk_200;
                st->mUnk_04 = TRACKER_ENV_VOLUME_LEVEL;
                break;
            case TRACKER_ENV_PAN:
                st = &c->mUnk_38;
                env = &p->mUnk_410[inst - 1]->mUnk_236;
                st->mUnk_04 = TRACKER_ENV_PAN_LEVEL;
                break;
        }
        if ((env->mUnk_34 & TRACKER_ENV_ON) == 0 || env->mUnk_30 <= 1) {
            return;
        }
        st->mUnk_00 = 0;
        st->mUnk_02 = env->mUnk_00[st->mUnk_00];
        if (env->mUnk_00[st->mUnk_00 + 1] - st->mUnk_02 > 0) {
            st->mUnk_04 = env->mUnk_18[st->mUnk_00] << 8;
            st->mUnk_08 = (env->mUnk_18[st->mUnk_00 + 1] << 8) - st->mUnk_04;
            st->mUnk_08 = st->mUnk_08 / (env->mUnk_00[st->mUnk_00 + 1] - st->mUnk_02);
        } else {
            st->mUnk_02 = st->mUnk_02 - 1;
            st->mUnk_04 = env->mUnk_18[st->mUnk_00 + 1] << 8;
            st->mUnk_08 = 0;
        }
        st->mUnk_0c = 0;
    } else {
        switch (type) {
            case TRACKER_ENV_VOLUME:
                c->mUnk_28.mUnk_04 = TRACKER_ENV_VOLUME_LEVEL;
                break;
            case TRACKER_ENV_PAN:
                c->mUnk_38.mUnk_04 = TRACKER_ENV_PAN_LEVEL;
                break;
        }
    }
}

// Advances an envelope by one frame (holding at the sustain point until keyOff is 1, looping when the envelope
// loops); returns 1 when the level's whole part changed.
int func_020380b8(TrackerPlayer *p, TrackerChannel *c, int type, int keyOff) {
    TrackerEnvelope *env;
    TrackerEnvState *st;
    u32 before;
    u8 inst = c->mUnk_15;

    if (inst != 0 && inst <= data_020c6b14.mUnk_174e) {
        switch (type) {
            case TRACKER_ENV_VOLUME:
                st = &c->mUnk_28;
                env = &p->mUnk_410[inst - 1]->mUnk_200;
                break;
            case TRACKER_ENV_PAN:
                st = &c->mUnk_38;
                env = &p->mUnk_410[inst - 1]->mUnk_236;
                break;
        }
        if ((env->mUnk_34 & TRACKER_ENV_ON) && env->mUnk_30 > 1) {
            if (keyOff == 1) {
                st->mUnk_0c = 1;
            }
            if (st->mUnk_00 < env->mUnk_30 - 1) {
                if (st->mUnk_0c == 1 || st->mUnk_00 != env->mUnk_31 || (env->mUnk_34 & TRACKER_ENV_SUSTAIN) == 0) {
                    before = st->mUnk_04 >> 8;
                    st->mUnk_04 = st->mUnk_04 + st->mUnk_08;
                    st->mUnk_02 = st->mUnk_02 + 1;
                    if (st->mUnk_02 == env->mUnk_00[st->mUnk_00 + 1]) {
                        st->mUnk_00++;
                        if (env->mUnk_34 & TRACKER_ENV_LOOP) {
                            if (st->mUnk_00 == env->mUnk_33) {
                                st->mUnk_00 = env->mUnk_32;
                            }
                        }
                        if (st->mUnk_00 < env->mUnk_30 - 1) {
                            st->mUnk_02 = env->mUnk_00[st->mUnk_00];
                            if (env->mUnk_00[st->mUnk_00 + 1] - st->mUnk_02 > 0) {
                                st->mUnk_04 = env->mUnk_18[st->mUnk_00] << 8;
                                st->mUnk_08 = (env->mUnk_18[st->mUnk_00 + 1] << 8) - st->mUnk_04;
                                st->mUnk_08 = st->mUnk_08 / (env->mUnk_00[st->mUnk_00 + 1] - st->mUnk_02);
                            } else {
                                st->mUnk_02 = st->mUnk_02 - 1;
                                st->mUnk_04 = env->mUnk_18[st->mUnk_00 + 1] << 8;
                                st->mUnk_08 = 0;
                            }
                        }
                    }
                    return before != (st->mUnk_04 >> 8);
                }
                return 0;
            }
            return 0;
        }
        return 0;
    }
    return 0;
}

// Places an envelope at frame pos: finds the segment, sets the level and the per-frame step.
void func_020382c0(TrackerPlayer *p, TrackerChannel *c, int type, u16 pos) {
    TrackerEnvelope *env;
    TrackerEnvState *st;
    u8 inst = c->mUnk_15;

    if (inst == 0 || inst > data_020c6b14.mUnk_174e) {
        return;
    }
    switch (type) {
        case TRACKER_ENV_VOLUME:
            st = &c->mUnk_28;
            env = &p->mUnk_410[inst - 1]->mUnk_200;
            break;
        case TRACKER_ENV_PAN:
            st = &c->mUnk_38;
            env = &p->mUnk_410[inst - 1]->mUnk_236;
            break;
    }
    if ((env->mUnk_34 & TRACKER_ENV_ON) == 0 || env->mUnk_30 <= 1) {
        return;
    }
    st->mUnk_00 = 0;
    for (;;) {
        if (st->mUnk_00 == env->mUnk_30 - 1) {
            break;
        }
        if (pos < env->mUnk_00[st->mUnk_00 + 1]) {
            break;
        }
        st->mUnk_00 = st->mUnk_00 + 1;
    }
    if (st->mUnk_00 == env->mUnk_30 - 1) {
        st->mUnk_02 = env->mUnk_00[st->mUnk_00];
        st->mUnk_04 = env->mUnk_18[st->mUnk_00] << 8;
        st->mUnk_08 = 0;
        return;
    }
    if (env->mUnk_00[st->mUnk_00 + 1] - st->mUnk_02 > 0) {
        st->mUnk_04 = env->mUnk_18[st->mUnk_00] << 8;
        st->mUnk_08 = (env->mUnk_18[st->mUnk_00 + 1] << 8) - st->mUnk_04;
        st->mUnk_08 = st->mUnk_08 / (env->mUnk_00[st->mUnk_00 + 1] - env->mUnk_00[st->mUnk_00]);
        st->mUnk_02 = pos;
        st->mUnk_04 = st->mUnk_08 * (st->mUnk_02 - env->mUnk_00[st->mUnk_00]) + st->mUnk_04;
    } else {
        st->mUnk_04 = env->mUnk_18[st->mUnk_00 + 1] << 8;
        st->mUnk_08 = 0;
        st->mUnk_02 = pos - 1;
    }
}

// After key-off, lowers the channel's fade-out level by the instrument's fade step (or clears it without a
// volume envelope); returns 1 when the level changed.
int func_02038464(TrackerPlayer *p, TrackerChannel *c) {
    TrackerInstrument *instr;
    s32 before;
    u8 inst;

    if (c->mUnk_10 == 1) {
        inst = c->mUnk_15;
        if (inst != 0 && inst <= data_020c6b14.mUnk_174e) {
            instr = p->mUnk_410[inst - 1];
            before = c->mUnk_48;
            if (instr->mUnk_200.mUnk_34 & TRACKER_ENV_ON) {
                c->mUnk_48 = before - instr->mUnk_2d2;
                if (c->mUnk_48 <= 0) {
                    c->mUnk_48 = 0;
                }
            } else {
                c->mUnk_48 = 0;
            }
            return before != c->mUnk_48;
        }
        return 0;
    }
    return 0;
}

// Table index (0..64) of a channel's output volume: channel volume, volume envelope level, fade-out level and
// master volume multiplied together.
int func_020384f8(int vol, u16 env, int fade, u8 master, u8 chanVol) {
    return ((((fade * master) >> 16) * (((vol * chanVol) >> 7 << 3) * env >> 16)) >> 8);
}

// Panning (0..255) after the panning envelope level is applied, weighted toward the middle.
u32 func_02038524(int pan, u16 level) {
    int v = pan + (((level >> 8) - 32) * (128 - abs(pan - 128))) / 32;

    if ((v < 255 ? v : 255) < 0) {
        return 0;
    }
    return v < 255 ? v : 255;
}

// Sets the master volume (0 unless the system flag allows sound, at most 128) and, while a song plays, sends the
// new volume of each active hardware channel.
void func_02038578(u8 volume, int mode) {
    u32 mask;
    u32 i;

    if (data_020c18bc.mUnk_493 == 0) {
        volume = 0;
    }
    if (volume > 128) {
        volume = 128;
    }
    data_020c6b14.mUnk_1747 = volume;
    if (data_020c6b14.mUnk_1724 != 1) {
        return;
    }
    if (data_020c6b14.mUnk_1748 != 0 && mode != 1) {
        return;
    }
    mask = func_02040a90();
    for (i = 0; i < data_020c6b14.mUnk_174c; i++) {
        if (mask & (1 << (data_020c6b14.mUnk_1744 + i))) {
            TrackerChannel *c = &data_020c6b14.mUnk_610[i];

            c->mUnk_68 = func_02040b60(data_02050b40[func_020384f8(c->mUnk_24, c->mUnk_28.mUnk_04, c->mUnk_48,
                                                                   data_020c6b14.mUnk_1747, c->mUnk_25)]);
            func_020401f8(1 << (data_020c6b14.mUnk_1744 + i), c->mUnk_68 & 0xff, c->mUnk_68 >> 8);
        }
    }
    while (SND_RecvCommandReply(0) != 0) {
    }
    SND_FlushCommand(2);
}

u8 func_02038694(void) {
    return data_020c6b14.mUnk_1747;
}

#pragma thumb on
