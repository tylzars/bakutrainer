#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TRACKER_CHANNEL_COUNT 32
#define TRACKER_PATTERN_COUNT 256
#define TRACKER_INSTRUMENT_COUNT 128
#define TRACKER_ORDER_COUNT 256

// A loaded block: the data pointer sits in the second word.
typedef struct TrackerPattern {
    u32 mUnk_00;
    void *mUnk_04;
} TrackerPattern;

// A sample of an instrument; its data pointer is released with the instrument.
typedef struct TrackerSample {
    u8 mUnk_00[0xc];
    void *mUnk_0c;
    u8 mUnk_10[0x18 - 0x10];
} TrackerSample;

// An instrument: a table of samples, the number of them at 0x26c.
typedef struct TrackerInstrument {
    TrackerSample *mUnk_000[0x26c / 4];
    u8 mUnk_26c;
    u8 mUnk_26d[0x2d4 - 0x26d];
} TrackerInstrument;

// State of one hardware channel (0x88 bytes).
typedef struct TrackerChannel {
    u32 mUnk_00;
    u16 mUnk_04;
    u16 mUnk_06;
    const s16 *mUnk_08;
    const s16 *mUnk_0c;
    u32 mUnk_10;
    u8 mUnk_14;
    u8 mUnk_15;
    u8 mUnk_16;
    u8 mUnk_17;
    u8 mUnk_18[2];
    u16 mUnk_1a;
    u8 mUnk_1c;
    u8 mUnk_1d;
    u16 mUnk_1e;
    u8 mUnk_20;
    u8 mUnk_21;
    u8 mUnk_22;
    u8 mUnk_23;
    u8 mUnk_24;
    u8 mUnk_25;
    u16 mUnk_26;
    u8 mUnk_28[0x48 - 0x28];
    u32 mUnk_48;
    u8 mUnk_4c[0x6a - 0x4c];
    u16 mUnk_6a[14];
    u8 mUnk_86[0x88 - 0x86];
} TrackerChannel;

// The whole state of the module player.
typedef struct TrackerPlayer {
    void (*mUnk_00)(int);
    void *mUnk_04;
    void *mUnk_08;
    void *mUnk_0c;
    TrackerPattern *mUnk_10[TRACKER_PATTERN_COUNT];
    TrackerInstrument *mUnk_410[TRACKER_INSTRUMENT_COUNT];
    TrackerChannel mUnk_610[TRACKER_CHANNEL_COUNT];
    u32 mUnk_1710;
    u32 mUnk_1714;
    u32 mUnk_1718;
    u32 mUnk_171c;
    u32 mUnk_1720;
    s32 mUnk_1724;
    s32 mUnk_1728;
    u8 mUnk_172c[0x172e - 0x172c];
    u16 mUnk_172e;
    u16 mUnk_1730;
    u16 mUnk_1732;
    u32 mUnk_1734;
    u16 mUnk_1738;
    s16 mUnk_173a;
    u16 mUnk_173c;
    u8 mUnk_173e[0x1740 - 0x173e];
    s16 mUnk_1740;
    s16 mUnk_1742;
    u8 mUnk_1744;
    u8 mUnk_1745;
    u8 mUnk_1746;
    s8 mUnk_1747;
    u8 mUnk_1748;
    u8 mUnk_1749;
    u8 mUnk_174a;
    u8 mUnk_174b;
    u8 mUnk_174c;
    u8 mUnk_174d;
    u8 mUnk_174e;
    u8 mUnk_174f;
    u8 mUnk_1750;
    u8 mUnk_1751;
    u8 mUnk_1752[TRACKER_ORDER_COUNT];
    u8 mUnk_1852;
    u8 mUnk_1853;
    u8 mUnk_1854;
    u8 mUnk_1855;
    u16 mUnk_1856;
    u16 mUnk_1858;
    u8 mUnk_185a[0x186c - 0x185a];
} TrackerPlayer;

extern TrackerPlayer data_020c6b14;

void func_020359d0(int a);
u16 func_02035bcc(const u8 *text, int base);
void func_02035c7c(u8 a, s16 b);
void func_02035e40(void);
void func_02035e7c(void);
void func_02035f2c(void);
void func_02035fc4(void);
void func_02035fd4(void);
void func_020360f4(void);
void func_02036194(void);
void func_02036198(void);

#ifdef __cplusplus
}
#endif
