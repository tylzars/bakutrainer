#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Volume fade of SoundCtl mVolumeFadeState: set by StartVolumeFade, stepped once per UpdateSoundCtl call.
#define VOLUME_FADE_NONE 0
#define VOLUME_FADE_DOWN 1           // down to 0
#define VOLUME_FADE_UP 2             // up to the target volume
#define VOLUME_FADE_DOWN_TO_TARGET 3 // down to the target volume (0x40); no caller passes it in this build

// Volume and track control.
typedef struct SoundCtl {
    u32 mVolumeFadeState;
    u32 mDelayedVolume;
    u32 mNextVolume;
    // The volume the fades stop at. Also the volume UpdateSoundCtl uses while the tracker player is stopped, and the
    // start volume of a new track.
    u32 mTargetVolume;
    u32 mDelayedTrack;
    u32 mNextTrack;
    // 0 = none. Set back to 0 while the tracker player is stopped, so UpdateSoundCtl then starts mNextTrack again.
    u32 mTrack;
    // Counts down once per UpdateSoundCtl call; reaching 0 requests mDelayedTrack at mDelayedVolume.
    u32 mDelayTimer;
} SoundCtl;

// Sound effect definitions, 0x24 bytes each; one of the first mIdCount entries of mIds is picked at random.
typedef struct SeEntry {
    u16 mIdCount;
    u16 mVolume;
    u8 mPan;
    // 1 sends PlayObjectSe to PlayWaveKeepAlive instead of PlaySe; 0 in every entry of gSeEntries.
    u8 mUnk_05;
    u8 mUnk_06[2];
    u32 mGap;
    u32 mGapRandom;
    u32 mCurrentGap;
    u32 mCallsSincePlay;
    u16 mIds[6];
} SeEntry;

typedef struct SeOwner {
    u8 mUnk_00[0x12];
    // 0 = none; PlayObjectSe clears it after it plays.
    u16 mSeIndex;
} SeOwner;

extern SoundCtl gSoundCtl;
extern const SeEntry gSeEntries[];

void InitSoundCtl(u32 track, s32 volume);
void UpdateSoundCtl(void);
void RequestTrack(u32 track, int volume);
void RequestTrackDelayed(u32 track, int volume, int delay);
void StartVolumeFade(u32 fade);
void PlaySeEx(u32 index, u16 volume, int b, int c, u32 d);
void PlaySe(u32 index);
void PlayObjectSe(void *obj);

#ifdef __cplusplus
}
#endif
