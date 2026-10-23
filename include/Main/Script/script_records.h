#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// States of the script engine (mState, returned by GetScriptState).
// Reading lines.
#define SCRIPT_STATE_READ 0
// Waiting after an EOF line for the wait timer or a key.
#define SCRIPT_STATE_WAIT 1
// Finishing the EOF block: the sprites slide back unless EOF flag 1 is set; then END with EOF flag 4, else READ again.
#define SCRIPT_STATE_FINISH 2
// Ended: UpdateScript still applies images, moves and draws sprites and text, but reads no more lines.
#define SCRIPT_STATE_END 3

// One entry of the event queue: up to 16 values read from the text. mKind is the event, mParam its argument, and the
// rest follow from mUnk_04.
typedef struct ScriptEvent {
    s16 mKind;
    s16 mParam;
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
// entry only (mX, mY, mFontIndex and the other draw values). Each entry keeps its own character count and speed.
typedef struct ScriptText {
    u16 mTextId;
    s16 mX;
    s16 mY;
    // DrawTextEx's width (the most a line fills before it breaks) and height (the step from one line to the next).
    s16 mLineWidth;
    s16 mLineHeight;
    // DrawTextEx alignment (the TEXT_ALIGN_MASK bits), from sTextAligns.
    s16 mAlign;
    u16 mCharCount;
    u16 mFontIndex;
    u8 mCharFrameCount;
    // 2, or 1 when the text's eighth value is non-zero.
    u8 mLayer;
} ScriptText;

// A sprite or shape of the script (AddScriptSprite). Sprites (top bit of mDrawId clear) are drawn through
// mSpriteDrawFunc or DrawSprite, shapes (SHAPE, top bit set) through DrawModelFileOrtho. The eye and Ls overlays are
// drawn on both.
typedef struct ScriptSprite {
    s32 mId;
    u32 mDrawId;
    // Where the sprite is drawn when it is not sliding, and the end point of its slide: pixels, or hundredths of a
    // pixel while mMovePerFrame is non-zero.
    s16 mX;
    s16 mY;
    // Where the move heads while mMovePerFrame is positive, in hundredths of a pixel.
    s16 mMoveEndX;
    s16 mMoveEndY;
    // 0: no move. Otherwise its absolute value moves mX and mY (hundredths of a pixel) once per UpdateScript pass:
    // towards mMoveEndX/mMoveEndY while it is positive, towards mStartX/mStartY while it is negative.
    s16 mMovePerFrame;
    // Where the slide starts; ReverseScriptSprite swaps it with mX/mY so that the sprite slides back.
    // For a moving sprite it is also the target while mMovePerFrame is negative. Such a sprite first gets its own
    // position here, in hundredths of a pixel. AddScriptSprite then overwrites mStartX (SLIDE_X set) or mStartY
    // (SLIDE_X clear) with the slide start point in pixels, while mX/mY stay in hundredths. That pixel value is the
    // target when the move runs back.
    s16 mStartX;
    s16 mStartY;
    u16 mDelayTimer;
    // Counts down once mDelayTimer is 0. At 0 the sprite is removed (mDrawId -1) if mStartX/mStartY equal mX/mY;
    // otherwise it is sent back (ReverseScriptSprite) and mNoReverse is set.
    u16 mShowTimer;
    // DrawSprite's w for a sprite (its eye and Ls overlays use mDrawOrder - 2); shifted left by 6 for a shape's draw.
    // For a shape, AddScriptSprite stops the system when the value is >= 0x100. That check runs before the new value
    // is stored, so it tests the slot's previous value.
    u16 mDrawOrder;
    // Animation frames (cells, not game frames): their number, and those remaining.
    u8 mAnimFrameCount;
    u8 mAnimFramesRemaining;
    u8 mFramesPerAnimFrame;
    u8 mAnimFrameFrames;
    u8 mSlideProgress;
    union {
        u8 mUnk_21;
        struct {
            // While set, ReverseScriptSprite does nothing. Set for a moving sprite and after the show timer sent the
            // sprite back. Cleared for a sprite that neither slides nor moves. A sprite that slides without moving
            // keeps the slot's previous value.
            u8 mNoReverse : 1;
            u8 mAnimLoop : 1;
            // Each axis negates mMovePerFrame on its own when it reaches its target, so the axis that arrives first
            // turns the other one round as well.
            u8 mMoveBounce : 1;
            // Mirrors the drawn sprite in x whenever mMovePerFrame is negative (from the start, if it starts negative).
            // A shape itself is drawn without the flip, but the eye and Ls overlays on it still get it.
            u8 mFlipOnReturn : 1;
            u8 mUnk_21_4 : 1;
            u8 mUnk_21_5 : 3;
        };
    };
    u8 mLayer;
    u8 mUnk_23;
} ScriptSprite;

// The part of gScriptWork from 0x280 on, which the code reaches through one base address.
typedef struct Unk_020c642c {
    u8 mUnk_00[0x10];
    u8 mEventWriteIndex;
    u8 mEventReadIndex;
    u8 mUnk_12[2];
    // Image changes queued by ScriptCmdScreen for UpdateScript (0 = none). [0] holds the script's raw odd value and
    // [1] is 2. UpdateScript reads the value as the 1|2 layer mask (SetFaded, LoadImage) and compares it with 1, 2, 3.
    u8 mPendingImageLayers[2];
    u16 mPendingImageIndices[2];
    u8 mUnk_1a[2];
    // The sprite or shape of SPRSCR/SHPSCR (set by SetScriptScrSprite), drawn at mScrX, mScrY on each UpdateScript
    // pass. 0 = none; StartScript and an image queued with layer value 1 clear it. Its top bits differ from
    // ScriptSprite.mDrawId's:
    //   - bit 31 marks it new; the first pass clears it, calls SetFaded(1) and copies the id to mUnk_30;
    //   - bit 30 set means a sprite (DrawSprite on layer 1), clear a shape (DrawModelFileOrtho).
    s32 mScrDrawId;
    u16 mScrX;
    u16 mScrY;
    s16 mIfValueA;
    s16 mIfValueB;
    u8 (*mTextEscapeFunc)(void);
    // Images last loaded by a queued image change: layer value 1 sets mLayer1ImageId, 2 mLayer2ImageId, 3 both, any
    // other value neither. Nothing reads them in this build.
    u16 mLayer1ImageId;
    u16 mLayer2ImageId;
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
    // When set, draws the sprites instead of DrawSprite, and maps the queued sound ids. Neither is set in this build
    // (both SetupScript calls pass 0).
    void (*mSpriteDrawFunc)(u32 id, int x, int y, int w, int mode);
    u32 (*mSoundIdFunc)(u32);
    u16 mScriptFileId;
    u16 mTextFileId;
    u16 mTextNameFileId;
    u16 mLineNameFileId;
    u16 mLineId;
    u16 mNextTextId;
    // Lines shown at a time (gSystemState.mTextWindowLineCount) after an EOF line without flag 0x10; 2 in this build.
    u16 mDefaultWindowLineCount;
    u16 mSpriteCount;
    u16 mWaitTimer;
    u8 mEofFlags;
    u8 mState;
    union {
        u8 mUnk_1c;
        struct {
            // Set while a sprite waits its delay, counts its show timer, animates or slides. SCRIPT_STATE_FINISH ends
            // only when it is 0. A moving sprite's movement does not set it. A looping animation's wrap clears it for
            // the sprites before it in the pass.
            u8 mSpritesBusy : 1;
            // Set by a key press in the wait state while the text is incomplete: the text position then advances 2
            // more per frame.
            u8 mFastText : 1;
            // Never set in this build (StartScript clears it). While set, one UpdateScript call runs the script on
            // until SCRIPT_STATE_END, without drawing sprites or text, waiting or playing queued waves.
            u8 mSkipToEnd : 1;
            // The drawn text still has characters past the text position, not drawn yet.
            u8 mTextIncomplete : 1;
            u8 mUnk_1c_4 : 4;
        };
    };
    u8 mUnk_1d;
    u16 mTextCount;
    // A text character advances every mCharFrameCount + 1 passes (TXTSPD sets 3 plus its value). mCharFrames counts
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
    // The texts that are drawn. mUnk_68 holds texts added with a non-zero seventh value; nothing reads it in this
    // build.
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
void SetupScript(int scriptFileId, int textFileId, int textNameFileId, int lineNameFileId, int line, int charSoundId,
                 void (*spriteDrawFunc)(u32 id, int x, int y, int w, int mode), u8 (*textEscapeFunc)(void),
                 u32 (*soundIdFunc)(u32));
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
void SetScriptScrSprite(u8 *text, int flag);
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
void UpdateScriptEye(int spriteId, int *id, int *dx, int *dy);
void ScriptCmdSetLs(u8 *text);
void ScriptCmdSetLsc(u8 *text);
void SelectLs(int lsIndex, int spriteIndex);
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
