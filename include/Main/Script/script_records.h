#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// States of the script engine (mState, returned by GetScriptState): reading lines; waiting after an EOF line for the
// wait timer or a key; finishing the EOF block (the sprites slide back unless EOF flag 1 is set, then END with EOF
// flag 4, else READ again); ended (UpdateScript still applies images, moves and draws sprites and text, but reads no
// more lines).
#define SCRIPT_STATE_READ 0
#define SCRIPT_STATE_WAIT 1
#define SCRIPT_STATE_FINISH 2
#define SCRIPT_STATE_END 3

// One entry of the event queue: the values read from the text (the first two are the event and its argument) and
// how many there are.
typedef struct ScriptEvent {
    s16 mUnk_00;
    s16 mUnk_02;
    u8 mUnk_04[0x20 - 4];
    // Not read in this build.
    u8 mValueCount;
    u8 mUnk_21;
} ScriptEvent;

typedef struct ScriptEye {
    u16 mUsed;
    s16 mId;
    s16 mAltId;
    // Bounds of the delay draw: mDelayMin up to mDelayLimit - 1 (mDelayLimit itself is not reached).
    u16 mDelayMin;
    u16 mDelayLimit;
    u16 mAltFrameCount;
    s16 mSpriteId;
    s16 mOffsetX;
    s16 mOffsetY;
    // -1: no countdown; otherwise counts the delay, then the frames the alternate id shows.
    s16 mTimer;
    u16 mShowingAlt;
} ScriptEye;

typedef struct ScriptLs {
    u16 mUsed;
    s16 mStillId;
    u16 mAnimId;
    // Number of animation frames of mAnimId.
    u16 mAnimFrameCount;
    u16 mFramesPerAnimFrame;
    s16 mOffsetX;
    s16 mOffsetY;
    s16 mSpriteId;
} ScriptLs;

// A text of the script (AddScriptText). The texts in mTexts are drawn as one string, placed and styled by the first
// entry only (mX, mY, mFontIndex and the other draw values); each entry keeps its own character count and speed.
typedef struct ScriptText {
    u16 mTextId;
    s16 mX;
    s16 mY;
    s16 mUnk_06;
    s16 mUnk_08;
    s16 mUnk_0a;
    u16 mCharCount;
    u16 mFontIndex;
    u8 mCharFrameCount;
    u8 mUnk_11;
} ScriptText;

// A sprite or shape of the script (AddScriptSprite). Sprites (top bit of mDrawId clear) are drawn through
// mSpriteDrawFunc or DrawSprite, shapes (SHAPE, top bit set) through func_020242e8; the eye and Ls overlays are drawn
// on both.
typedef struct ScriptSprite {
    s32 mId;
    u32 mDrawId;
    s16 mUnk_08;
    s16 mUnk_0a;
    s16 mUnk_0c;
    s16 mUnk_0e;
    s16 mUnk_10;
    s16 mUnk_12;
    s16 mUnk_14;
    u16 mDelayTimer;
    u16 mUnk_18;
    u16 mUnk_1a;
    // Animation frames (cells, not game frames): their number, and those remaining.
    u8 mAnimFrameCount;
    u8 mAnimFramesRemaining;
    u8 mFramesPerAnimFrame;
    u8 mAnimFrameFrames;
    u8 mSlideProgress;
    union {
        u8 mUnk_21;
        struct {
            u8 mUnk_21_0 : 1;
            u8 mUnk_21_1 : 1;
            u8 mUnk_21_2 : 1;
            u8 mUnk_21_3 : 1;
            u8 mUnk_21_4 : 1;
            u8 mUnk_21_5 : 3;
        };
    };
    u8 mLayer;
    u8 mUnk_23;
} ScriptSprite;

// The part of gScriptWork from 0x280 on, which the code reaches through one base address: the event queue's write
// and read indices, the values the conditional commands compare, the callback and the selected indices.
typedef struct Unk_020c642c {
    u8 mUnk_00[0x10];
    u8 mEventWriteIndex;
    u8 mEventReadIndex;
    u8 mUnk_12[2];
    // Image changes queued by ScriptCmdScreen for UpdateScript (0 = none): [0] holds the script's raw odd value and
    // [1] is 2; UpdateScript reads the value as the 1|2 layer mask (SetFaded, LoadImage) and compares it with 1, 2, 3.
    u8 mPendingImageLayers[2];
    u16 mPendingImageIndices[2];
    u8 mUnk_1a[2];
    s32 mUnk_1c;
    u16 mUnk_20;
    u16 mUnk_22;
    s16 mIfValueA;
    s16 mIfValueB;
    u8 (*mTextEscapeFunc)(void);
    u16 mUnk_2c;
    u16 mUnk_2e;
    u16 mUnk_30;
    u8 mUnk_32[2];
    s32 mLsIndex;
    s32 mLsSpriteIndex;
    // Counts down only while the selected Ls is drawn on its sprite; non-zero shows the animated id.
    u16 mLsTimer;
    u8 mUnk_3e[2];
    s32 mUnk_40;
} Unk_020c642c;

typedef struct ScriptWork {
    // Draws the sprites instead of DrawSprite when set, and maps the queued sound ids when set; neither is set in this
    // build (both SetupScript calls pass 0).
    void (*mSpriteDrawFunc)(u32 id, int x, int y, int w, int mode);
    u32 (*mSoundIdFunc)(u32);
    u16 mScriptFileId;
    u16 mTextFileId;
    u16 mTextNameFileId;
    u16 mLineNameFileId;
    u16 mLineId;
    u16 mNextTextId;
    u16 mUnk_14;
    u16 mSpriteCount;
    u16 mWaitTimer;
    u8 mEofFlags;
    u8 mState;
    union {
        u8 mUnk_1c;
        struct {
            // Set while a sprite waits its delay, counts its show timer, animates or slides; SCRIPT_STATE_FINISH ends
            // only when it is 0. A moving sprite's movement does not set it, and a looping animation's wrap clears it
            // for the sprites before it in the pass.
            u8 mSpritesBusy : 1;
            // Set by a key press in the wait state while the text is incomplete: the text position then advances 2
            // more per frame.
            u8 mFastText : 1;
            u8 mUnk_1c_2 : 1;
            // The drawn text still has characters past the text position, not drawn yet.
            u8 mTextIncomplete : 1;
            u8 mUnk_1c_4 : 4;
        };
    };
    u8 mUnk_1d;
    u16 mTextCount;
    // A text character advances every mCharFrameCount + 1 passes (TXTSPD sets 3 plus its value); mCharFrames counts
    // the passes since the last advance.
    u16 mCharFrameCount;
    u16 mCharFrames;
    u16 mUnk_24;
    u16 mUnk_26;
    // Played at each character advance when non-zero; never set in this build (both SetupScript calls pass 0).
    u16 mCharSoundId;
    s16 mTextOffsetY;
    u8 *mRecords;
    u8 mUnk_30;
    u8 mUnk_31;
    s16 mTrackFileId;
    u8 mTrackVolume;
    u8 mUnk_35;
    u16 mSoundIds[8];
    u8 mSoundVolumes[8];
    // 0 or 1: play with that repeat value; 2: stop.
    u8 mSoundModes[8];
    u8 mSoundCount;
    u8 mUnk_57;
    ScriptLs *mLsList;
    ScriptEye *mEyes;
    ScriptSprite *mSprites;
    // The texts that are drawn; mUnk_68 holds texts added with a non-zero seventh value and is not read in this build.
    ScriptText *mTexts;
    ScriptText *mUnk_68;
    void *mUnk_6c;
    // The event queue's indexes wrap at 16, so its last entry ends at 0x290 and covers the first 0x10 bytes of
    // mUnk_280, which nothing else uses.
    union {
        ScriptEvent mEvents[16];
        struct {
            u8 mUnk_70_pad[0x280 - 0x70];
            Unk_020c642c mUnk_280;
        };
    };
} ScriptWork;

extern ScriptWork gScriptWork;

void StartScriptAtName(const char *name);
void StartScript(int index);
void SetupScript(int a, int b, int c, int d, int e, int f, void (*g)(u32 id, int x, int y, int w, int mode),
                 u8 (*h)(void), u32 (*i)(u32));
void ReleaseScript(void);
u8 ToUpper(u8 c);
int IsLetterOrUnderscore(u8 c);
int IsDigitOrSign(u8 c);
int IsSign(u8 c);
int IsNotSignOrSpace(u8 c);
int StringsEqual(const char *x, const char *y);
void CopyUpperName(u8 **src, u8 **dst);
void WriteS16(s16 value, u8 **out);
s16 ReadNumber(u8 **text);
const u8 *FindScriptRecord(const u8 *name);
void ReadScriptRecordValues(const u8 *p, u16 *values, u32 *count);
void AddOrSubtractValues(const s16 *src, s16 *dst, int count, int op);
int ReadScriptCommand(u8 **text);
void SetScriptRecordFromText(const char *text);
char *CopyUpperWord(char *dst, const char *src);
int ClearAndEvaluate(s16 *out, u8 *text, u32 size);
int Evaluate(s16 *out, u8 *text);
void AddScriptSprite(u8 *text, int mode);
void AddScriptText(u8 *text);
void ScriptCmdTxtSpd(u8 *text);
void PlayScriptTrackAndSounds(void);
void ScriptCmdEof(u8 *text);
void QueueScriptEvent(u8 *text);
void ScriptCmdSync(const char *name);
void ScriptCmdGoto(u8 *text);
void func_02029aac(u8 *text, int flag);
void ScriptCmdScreen(u8 *text);
void ScriptCmdIf(u8 *text);
void ScriptCmdEq(u8 *text);
void ScriptCmdNe(u8 *text);
void ScriptCmdGt(u8 *text);
void ScriptCmdLt(u8 *text);
void ScriptCmdMusic(u8 *text);
void ScriptCmdSound(u8 *text);
void ScriptCmdBra(const char *name);
void ScriptCmdFade(u8 *text);
void ScriptCmdSetEye(u8 *text);
void UpdateScriptEye(int id, int *a, int *b, int *c);
void ScriptCmdSetLs(u8 *text);
void ScriptCmdSetLsc(u8 *text);
void SelectLs(int a, int b);
void SelectLsForSprite(int index, int id);
void ReverseScriptSprite(ScriptSprite *rec);
void ReverseScriptSprites(void);
void StartEofFinish(void);
u8 GetScriptState(void);
void ClearScriptEvents(void);
ScriptEvent *NextScriptEvent(void);
void func_0202a1cc(void);
void UpdateScript(int (*callback)(void));

#ifdef __cplusplus
}
#endif
