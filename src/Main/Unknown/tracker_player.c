#include <nitro/os.h>
#include <nitro/snd.h>
#include <Main/Unknown/functions.h>
#include <Main/Unknown/globals.h>
#include <Main/Unknown/system_init.h>
#include <Main/Unknown/tracker_player.h>
#include <Main/clib.h>

// One period of a sine wave, 64 entries with amplitude 255; every channel points at it twice.
extern const s16 data_02050ac0[64];
// Volume levels for the sound command, indexed 0..64.
extern const s32 data_02050b40[65];
// Playback rates of the linear note scale: 768 entries per octave.
extern const u32 data_02050c44[768];
// Periods per note for each of the 16 fine-tune steps.
extern const u16 data_02051844[16][96];

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
int func_02038524(int pan, u16 level) {
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
