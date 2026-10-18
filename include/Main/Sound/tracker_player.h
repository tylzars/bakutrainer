#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TRACKER_CHANNEL_COUNT 32
#define TRACKER_PATTERN_COUNT 256
#define TRACKER_INSTRUMENT_COUNT 128
#define TRACKER_ORDER_COUNT 256

// Envelope kinds of an instrument and the default levels used without one.
#define TRACKER_ENV_VOLUME 0
#define TRACKER_ENV_PAN 1
#define TRACKER_ENV_VOLUME_LEVEL 0x4000
#define TRACKER_ENV_PAN_LEVEL 0x2000

// Flags of an envelope.
#define TRACKER_ENV_ON 1
#define TRACKER_ENV_SUSTAIN 2
#define TRACKER_ENV_LOOP 4

// Row cell values: an empty note or effect column, and the note that releases the channel.
#define TRACKER_NOTE_NONE 255
#define TRACKER_NOTE_OFF 97
#define TRACKER_FX_NONE 255

// Effect commands of a row cell; the volume column is translated to these.
#define TRACKER_FX_ARPEGGIO 0
#define TRACKER_FX_SLIDE_UP 1
#define TRACKER_FX_SLIDE_DOWN 2
#define TRACKER_FX_TONE_SLIDE 3
#define TRACKER_FX_VIBRATO 4
#define TRACKER_FX_TONE_SLIDE_VOL 5
#define TRACKER_FX_VIBRATO_VOL 6
#define TRACKER_FX_TREMOLO 7
#define TRACKER_FX_PAN 8
#define TRACKER_FX_SAMPLE_OFFSET 9
#define TRACKER_FX_VOL_SLIDE 10
#define TRACKER_FX_ORDER_JUMP 11
#define TRACKER_FX_VOLUME 12
#define TRACKER_FX_ROW_BREAK 13
#define TRACKER_FX_EXTENDED 14
#define TRACKER_FX_SPEED 15
#define TRACKER_FX_GLOBAL_VOLUME 16
#define TRACKER_FX_GLOBAL_VOL_SLIDE 17
#define TRACKER_FX_ENV_POS 21
#define TRACKER_FX_PAN_SLIDE 25

// Sub-commands of TRACKER_FX_EXTENDED (high digit of the parameter).
#define TRACKER_FXE_FINE_SLIDE_UP 1
#define TRACKER_FXE_FINE_SLIDE_DOWN 2
#define TRACKER_FXE_VIBRATO_WAVE 4
#define TRACKER_FXE_FINETUNE 5
#define TRACKER_FXE_TREMOLO_WAVE 7
#define TRACKER_FXE_PAN 8
#define TRACKER_FXE_RETRIGGER 9
#define TRACKER_FXE_FINE_VOL_UP 10
#define TRACKER_FXE_FINE_VOL_DOWN 11
#define TRACKER_FXE_NOTE_CUT 12
#define TRACKER_FXE_NOTE_DELAY 13
#define TRACKER_FXE_ROW_DELAY 14

// How a channel's sample is started (TrackerChannel mUnk_4c): after a delay of ticks, or again every few ticks.
#define TRACKER_START_DELAYED 1
#define TRACKER_START_RETRIGGER 2

// Smallest period the slides leave on a channel.
#define TRACKER_PERIOD_MIN 54

// A loaded block: the data pointer sits in the second word.
typedef struct TrackerPattern {
    u8 mUnk_00;
    u8 mUnk_01;
    u8 mUnk_02[2];
    void *mUnk_04;
} TrackerPattern;

// Points of an instrument envelope: frame numbers and levels, with the point count and loop points.
typedef struct TrackerEnvelope {
    u16 mUnk_00[12];
    u16 mUnk_18[12];
    u8 mUnk_30;
    u8 mUnk_31;
    u8 mUnk_32;
    u8 mUnk_33;
    u8 mUnk_34;
    u8 mUnk_35;
} TrackerEnvelope;

// Playback state of one envelope of a channel (0x10 bytes).
typedef struct TrackerEnvState {
    u8 mUnk_00;
    u16 mUnk_02;
    u32 mUnk_04;
    s32 mUnk_08;
    s32 mUnk_0c;
} TrackerEnvState;

// A sample of an instrument; its data pointer is released with the instrument.
typedef struct TrackerSample {
    u32 mUnk_00;
    u32 mUnk_04;
    u32 mUnk_08;
    void *mUnk_0c;
    u16 mUnk_10;
    u8 mUnk_12;
    s8 mUnk_13;
    u8 mUnk_14;
    s8 mUnk_15;
    s16 mUnk_16;
} TrackerSample;

// An instrument: a table of samples, the number of them at 0x26c.
typedef struct TrackerInstrument {
    TrackerSample *mUnk_000[0x200 / 4];
    TrackerEnvelope mUnk_200;
    TrackerEnvelope mUnk_236;
    u8 mUnk_26c;
    u8 mUnk_26d[0x60];
    u8 mUnk_2cd;
    u8 mUnk_2ce;
    u8 mUnk_2cf;
    u8 mUnk_2d0;
    u8 mUnk_2d1;
    u16 mUnk_2d2;
} TrackerInstrument;

// State of one hardware channel (0x88 bytes).
typedef struct TrackerChannel {
    TrackerSample *mUnk_00;
    u16 mUnk_04;
    u16 mUnk_06;
    const s16 *mUnk_08;
    const s16 *mUnk_0c;
    u32 mUnk_10;
    u8 mUnk_14;
    u8 mUnk_15;
    u8 mUnk_16;
    u8 mUnk_17;
    u16 mUnk_18;
    u16 mUnk_1a;
    u8 mUnk_1c;
    u8 mUnk_1d;
    u16 mUnk_1e;
    u8 mUnk_20;
    u8 mUnk_21;
    u8 mUnk_22;
    u8 mUnk_23;
    s8 mUnk_24;
    u8 mUnk_25;
    s16 mUnk_26;
    TrackerEnvState mUnk_28;
    TrackerEnvState mUnk_38;
    s32 mUnk_48;
    u8 mUnk_4c;
    u8 mUnk_4d;
    u8 mUnk_4e;
    u8 mUnk_4f;
    u8 mUnk_50;
    u16 mUnk_52;
    u32 mUnk_54;
    u32 mUnk_58;
    u32 mUnk_5c;
    u32 mUnk_60;
    u32 mUnk_64;
    u16 mUnk_68;
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
    u16 mUnk_172c;
    u16 mUnk_172e;
    u16 mUnk_1730;
    u16 mUnk_1732;
    u32 mUnk_1734;
    s16 mUnk_1738;
    s16 mUnk_173a;
    u16 mUnk_173c;
    u8 mUnk_173e;
    u8 mUnk_173f;
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

void func_02035108(u16 a, u16 b, u8 c, u16 d, int e, int f);
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
void func_02037c10(void);
void func_02037cdc(TrackerChannel *c);
u16 func_02037d38(TrackerChannel *c);
u8 func_02037dac(TrackerChannel *c);
void func_02037df8(TrackerChannel *c, int up, int down);
void func_02037e34(TrackerChannel *c, int up, int down);
void func_02037e70(s8 *v, int up, int down);
u16 func_02037eac(int note, int finetune);
u32 func_02037efc(int period, int linear);
void func_02037f5c(TrackerPlayer *p, TrackerChannel *c, int type);
int func_020380b8(TrackerPlayer *p, TrackerChannel *c, int type, int keyOff);
void func_020382c0(TrackerPlayer *p, TrackerChannel *c, int type, u16 pos);
int func_02038464(TrackerPlayer *p, TrackerChannel *c);
int func_020384f8(int vol, u16 env, int fade, u8 master, u8 chanVol);
u32 func_02038524(int pan, u16 level);
void func_02038578(u8 volume, int mode);
u8 func_02038694(void);

#ifdef __cplusplus
}
#endif
