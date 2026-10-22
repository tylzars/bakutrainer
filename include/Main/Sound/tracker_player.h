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

// How a channel's sample is started (TrackerChannel.mStartKind): after a delay of ticks, or again every few ticks.
#define TRACKER_START_DELAYED 1
#define TRACKER_START_RETRIGGER 2

// Smallest period the slides leave on a channel.
#define TRACKER_PERIOD_MIN 54

// States of the global fade (TrackerPlayer.mGlobalFadeState), which lowers the global volume; a channel's fade-out
// after a note-off is separate. In TO_PENDING and TO_RELEASE the timer interrupt lowers the global volume by 4 per
// interrupt and the global-volume effects are ignored; at the end TO_PENDING becomes DONE (DONE is reached only from
// TO_PENDING; the stored request plays on the next frame update) and TO_RELEASE releases the song. In any state but
// NONE the frame update also releases the song when the interrupt has not run for more than 8 frames.
#define TRACKER_GLOBAL_FADE_NONE 0
#define TRACKER_GLOBAL_FADE_TO_PENDING 1
#define TRACKER_GLOBAL_FADE_TO_RELEASE 2
#define TRACKER_GLOBAL_FADE_DONE 3

// Timer flags of the tracker player (TrackerPlayer.mTimerFlags). TRACKER_TIMER_FINE: StartTrackerTimer starts timer 2
// with count 654 and adds 0x147 per interrupt instead of 0xa3a and 0x51e (about a quarter of each).
// TRACKER_TIMER_AUTO_FINE: each start sets TRACKER_TIMER_FINE unless the tick rate is 125.
#define TRACKER_TIMER_FINE 1
#define TRACKER_TIMER_AUTO_FINE 2

// A pattern: its row count and its cells (five bytes per cell, one cell per channel and row).
typedef struct TrackerPattern {
    u8 mUnk_00;
    u8 mRowCount;
    u8 mUnk_02[2];
    void *mCells;
} TrackerPattern;

// Points of an instrument envelope: tick positions and values, with the point count and loop points.
typedef struct TrackerEnvelope {
    u16 mPointTickPos[12];
    u16 mPointValues[12];
    u8 mPointCount;
    u8 mSustainPoint;
    u8 mLoopStartPoint;
    u8 mLoopEndPoint;
    u8 mFlags;
    u8 mUnk_35;
} TrackerEnvelope;

// Playback state of one envelope of a channel (0x10 bytes).
typedef struct TrackerEnvState {
    u8 mPointIndex;
    u16 mTickPos;
    u32 mValue;
    s32 mValuePerTick;
    s32 mNoteOff;
} TrackerEnvState;

// A sample of an instrument; its data pointer is released with the instrument.
typedef struct TrackerSample {
    u32 mLength;
    u32 mLoopStart;
    u32 mLoopLength;
    void *mData;
    u16 mFileId;
    u8 mVolume;
    s8 mFinetune;
    u8 mFlags;
    s8 mNoteOffset;
    s16 mPan;
} TrackerSample;

// An instrument: a table of samples, the number of them in mSampleCount.
typedef struct TrackerInstrument {
    TrackerSample *mSamples[0x200 / 4];
    TrackerEnvelope mVolumeEnvelope;
    TrackerEnvelope mPanEnvelope;
    u8 mSampleCount;
    u8 mNoteSamples[0x60];
    u8 mUnk_2cd;
    u8 mUnk_2ce;
    u8 mUnk_2cf;
    u8 mUnk_2d0;
    u8 mUnk_2d1;
    u16 mFadeOutPerTick;
} TrackerInstrument;

// State of one hardware channel (0x88 bytes).
typedef struct TrackerChannel {
    TrackerSample *mSample;
    u16 mPeriod;
    u16 mTargetPeriod;
    const s16 *mVibratoWave;
    const s16 *mTremoloWave;
    u32 mNoteOff;
    u8 mNote;
    u8 mInstrumentHandle;
    u8 mEffect;
    u8 mVolumeColumnEffect;
    u16 mEffectParam;
    u16 mVolumeColumnEffectParam;
    u8 mArpeggioTicks;
    u8 mWaveIndex;
    u16 mToneSlidePerTick;
    u8 mVibratoPerTick;
    u8 mVibratoDepth;
    u8 mTremoloPerTick;
    u8 mTremoloDepth;
    s8 mVolume;
    u8 mUnk_25;
    s16 mPan;
    TrackerEnvState mVolumeEnvState;
    TrackerEnvState mPanEnvState;
    // The channel's fade-out after a note-off (AdvanceFadeOut); not the global fade.
    s32 mFadeOutVolume;
    u8 mStartKind;
    u8 mStartTicks;
    u8 mStartTickCount;
    u8 mUnk_4f;
    u8 mStartPan;
    u16 mStartVolume;
    u32 mStartData;
    u32 mUnk_58;
    u32 mStartLoopLength;
    u32 mUnk_60;
    u32 mUnk_64;
    u16 mUnk_68;
    u16 mLastParams[14];
    u8 mUnk_86[0x88 - 0x86];
} TrackerChannel;

// The whole state of the module player.
typedef struct TrackerPlayer {
    // Called with 0 when playback starts, 1 when it stops, 2 before the pending request plays, 3 before a global fade
    // releases the song and 4 at an order jump back or the wrap at the song's end; never set in this build.
    void (*mCallback)(int);
    void *mPatternBlock;
    void *mInstrumentBlock;
    void *mSampleBlock;
    TrackerPattern *mPatterns[TRACKER_PATTERN_COUNT];
    TrackerInstrument *mInstruments[TRACKER_INSTRUMENT_COUNT];
    TrackerChannel mChannels[TRACKER_CHANNEL_COUNT];
    u32 mTimerFlags;
    u32 mTimePerIrq;
    // Frames since the last timer interrupt, counted by UpdateTracker only while a global fade state is set.
    u32 mFramesSinceIrq;
    u32 mTickLength;
    u32 mTickTime;
    s32 mPlaying;
    s32 mUnk_1728;
    u16 mSampleFileBase;
    u16 mTrackFileId;
    u16 mPendingTrackFileId;
    u16 mPendingSampleFileBase;
    u32 mPendingRestart;
    s16 mOrderIndex;
    // Order index AdvanceOrder takes instead of the next one unless -1; only ever set to -1 in this build, so the jump
    // never runs.
    s16 mJumpOrderIndex;
    u16 mPendingStartOrder;
    u8 mUnk_173e;
    u8 mUnk_173f;
    s16 mLastRow;
    s16 mRowIndex;
    // Hardware channel number of mChannels[0] (16 minus the song's channel count).
    u8 mFirstChannel;
    u8 mLoading;
    u8 mReleasing;
    // ReleaseTrack stores -128, which GetGlobalVolume reads back as 128 (full volume).
    s8 mGlobalVolume;
    u8 mGlobalFadeState;
    u8 mPendingVolume;
    u8 mOrderCount;
    u8 mLoopOrder;
    u8 mChannelCount;
    u8 mPatternCount;
    u8 mInstrumentCount;
    u8 mFlags;
    u8 mInitialTicksPerRow;
    u8 mInitialTickRate;
    u8 mOrders[TRACKER_ORDER_COUNT];
    u8 mTicksPerRow;
    u8 mTickRate;
    u8 mPatternIndex;
    u8 mUnk_1855;
    u16 mTickIndex;
    u16 mRowDelayTickCount;
    u8 mUnk_185a[0x186c - 0x185a];
} TrackerPlayer;

extern TrackerPlayer gTrackerPlayer;

void PlayTrack(u16 fileId, u16 sampleFileBase, u8 volume, u16 startOrder, int fadeFirst, int restart);
void ReleaseTrack(int fadeOut);
u16 AddFirstDecimal(const u8 *text, int base);
void StartPlayback(u8 volume, s16 startOrder);
void StopPlayback(void);
void StopTrackerChannels(void);
void StartTrackerTimer(void);
void StopTrackerTimer(void);
void HandleTrackerIrq(void);
void UpdateTracker(void);
void func_02036194(void);
void AdvanceTracker(void);
void AdvanceOrder(void);
void ApplyToneSlide(TrackerChannel *c);
u16 ApplyVibrato(TrackerChannel *c);
u8 ApplyTremolo(TrackerChannel *c);
void ApplyVolumeSlide(TrackerChannel *c, int up, int down);
void ApplyPanSlide(TrackerChannel *c, int up, int down);
void ApplyGlobalVolumeSlide(s8 *v, int up, int down);
u16 GetNotePeriod(int note, int finetune);
u32 GetPeriodRate(int period, int linear);
void StartEnvelope(TrackerPlayer *p, TrackerChannel *c, int type);
int AdvanceEnvelope(TrackerPlayer *p, TrackerChannel *c, int type, int keyOff);
void SetEnvelopePosition(TrackerPlayer *p, TrackerChannel *c, int type, u16 pos);
int AdvanceFadeOut(TrackerPlayer *p, TrackerChannel *c);
int ComputeVolumeIndex(int vol, u16 env, int fade, u8 master, u8 chanVol);
u32 ComputePan(int pan, u16 level);
void SetGlobalVolume(u8 volume, int mode);
u8 GetGlobalVolume(void);

#ifdef __cplusplus
}
#endif
