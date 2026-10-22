#include <Main/Map/map_entries.h>
#include <Main/Graphics/model.h>
#include <Main/System/random_angle.h>
#include <Main/System/screen_fade.h>
#include <Main/System/system_init.h>
#include <Main/Graphics/text_draw.h>
#include <Main/Graphics/texture_blocks.h>
#include <nitro/mi.h>
#include <nitro/os.h>
#include <Main/System/bg_image.h>
#include <Main/System/game_math.h>
#include <Main/clib.h>
#include <Main/Script/script_records.h>
#include <Main/Sound/wave_player.h>
#include <Main/Sound/tracker_player.h>
#include <Main/Input/touch_buttons.h>
#include <Main/Scene/language_menu.h>

ScriptWork gScriptWork;

// Script commands: the index of the command's name in sCommandNames, which UpdateScript switches on.
#define SCRIPT_CMD_LET 0
#define SCRIPT_CMD_SPR 1
#define SCRIPT_CMD_SPRX 2
#define SCRIPT_CMD_SPRSCR 3
#define SCRIPT_CMD_SHPSCR 4
#define SCRIPT_CMD_SHAPE 5
#define SCRIPT_CMD_TXT 6
#define SCRIPT_CMD_EOF 7
#define SCRIPT_CMD_TXTSPD 8
#define SCRIPT_CMD_ESC 9
#define SCRIPT_CMD_SYNC 10
#define SCRIPT_CMD_GOTO 11
#define SCRIPT_CMD_SCREEN 12
#define SCRIPT_CMD_IF 13
#define SCRIPT_CMD_EQ 14
#define SCRIPT_CMD_NE 15
#define SCRIPT_CMD_GT 16
#define SCRIPT_CMD_LT 17
#define SCRIPT_CMD_MUSIC 18
#define SCRIPT_CMD_SOUND 19
#define SCRIPT_CMD_BRA 20
#define SCRIPT_CMD_FADE 21
#define SCRIPT_CMD_SETEYE 22
#define SCRIPT_CMD_SETLS 23
#define SCRIPT_CMD_SETLSC 24
#define SCRIPT_CMD_COUNT 25

// What ReadNumber returns when the text holds no further number.
#define SCRIPT_NO_NUMBER 0x55aa

// Flags of a script sprite (the fifth value of SPR, SPRX and SHAPE, read by AddScriptSprite). SLIDE: the sprite slides
// to its position from a start point before x 0 or past x 0x100, or before y 0 or past y 0xc0 (for a sprite, offset
// from that edge by its rect; for a shape, at 0x100 or 0xc0 itself or a power-of-two size from its table entry before
// 0); a moving sprite gets that point too but does not slide (see mStartX). SLIDE_X: the point is in x at the sprite's
// y, otherwise in y at its x; 0x2 (left as a literal: its side differs between x and y) puts it before x 0 or past y
// 0xc0, otherwise past x 0x100 or before y 0. LAYER_1: a sprite is drawn on layer 1, otherwise 2 (a shape always on 1).
// ANIM_LOOP: the animation starts again after its last frame instead of stopping on it. MOVE_BOUNCE: a moving sprite
// turns round at each end. FLIP_ON_RETURN: a moving sprite is drawn mirrored in x while its mMovePerFrame is negative
// (heading for mStartX and mStartY); sprites only, a shape is drawn without the flip.
#define SCRIPT_SPRITE_SLIDE_X 1
#define SCRIPT_SPRITE_SLIDE 4
#define SCRIPT_SPRITE_LAYER_1 8
#define SCRIPT_SPRITE_ANIM_LOOP 0x10
#define SCRIPT_SPRITE_MOVE_BOUNCE 0x20
#define SCRIPT_SPRITE_FLIP_ON_RETURN 0x40

// Finds the entry of the line-name file (from entry 2 on) whose first word, upper-cased, equals name's and starts the
// script at it. The search has no end: it does not stop when no entry matches.
void StartScriptAtName(const char *name) {
    char first[0x20];
    char second[0x20];
    int i = 2;
    CopyUpperWord(first, name);
    for (;; i++) {
        if (GetFileValueType(gScriptWork.mLineNameFileId, i) == 4) {
            const u8 *s = GetFileString(gScriptWork.mLineNameFileId, i);
            if (s[0] != 0) {
                CopyUpperWord(second, s);
                if (StringsEqual(first, second) == 1) {
                    StartScript(i);
                    return;
                }
            }
        }
    }
}

// Starts the script at line index: empties the event queue; clears the sprite and text counts, the Ls selection, the
// state (SCRIPT_STATE_READ), the EOF flags, the pending images and the wait timer; sets every eye's timer to -1.
void StartScript(int index) {
    u32 i;
    ClearScriptEvents();
    gScriptWork.mLineId = index;
    gScriptWork.mSpriteCount = 0;
    gScriptWork.mUnk_280.mLsSpriteIndex = -1;
    gScriptWork.mUnk_280.mLsIndex = -1;
    gScriptWork.mUnk_280.mLsTimer = 0;
    gScriptWork.mTextCount = 0;
    gScriptWork.mTextOffsetY = 0;
    gScriptWork.mState = SCRIPT_STATE_READ;
    gScriptWork.mEofFlags = 0;
    gScriptWork.mTextIncomplete = 0;
    gScriptWork.mSkipToEnd = 0;
    gScriptWork.mUnk_280.mPendingImageLayers[0] = gScriptWork.mUnk_280.mPendingImageLayers[1] = 0;
    gScriptWork.mUnk_280.mScrDrawId = 0;
    gScriptWork.mWaitTimer = 0;
    for (i = 0; i < 0x20; i++) {
        gScriptWork.mEyes[i].mTimer = -1;
    }
}

// Sets up the script engine: stores the callbacks, releases the old buffers when the file ids change, on first use
// stores the ids and allocates the buffers, then starts the script at line index `line` and runs its first frame
// (UpdateScript).
void SetupScript(int scriptFileId, int textFileId, int textNameFileId, int lineNameFileId, int line, int charSoundId,
                 void (*spriteDrawFunc)(u32 id, int x, int y, int w, int mode), u8 (*textEscapeFunc)(void),
                 u32 (*soundIdFunc)(u32)) {
    gScriptWork.mUnk_280.mTextEscapeFunc = textEscapeFunc;
    gScriptWork.mSoundIdFunc = soundIdFunc;
    if ((scriptFileId != gScriptWork.mScriptFileId && scriptFileId != 0) ||
        (textFileId != gScriptWork.mTextFileId && textFileId != 0)) {
        ReleaseScript();
    }
    if (gScriptWork.mScriptFileId == 0) {
        if (scriptFileId == 0) {
            OS_Terminate();
        }
        if (textFileId == 0) {
            OS_Terminate();
        }
        gScriptWork.mScriptFileId = scriptFileId;
        gScriptWork.mTextFileId = textFileId;
        gScriptWork.mTextNameFileId = textNameFileId;
        gScriptWork.mLineNameFileId = lineNameFileId;
        gScriptWork.mTrackFileId = 0;
        gScriptWork.mRecords = Alloc(0x800);
        gScriptWork.mLsList = Alloc(0x200);
        gScriptWork.mEyes = Alloc(0x2c0);
        gScriptWork.mSprites = Alloc(0x480);
        gScriptWork.mTexts = Alloc(0x120);
        gScriptWork.mUnk_68 = Alloc(0x48);
        gScriptWork.mUnk_6c = Alloc(0x48);
        gScriptWork.mCharSoundId = charSoundId;
    }
    gScriptWork.mUnk_280.mLayer1ImageId = gScriptWork.mUnk_280.mLayer2ImageId = gScriptWork.mUnk_280.mUnk_30 = 0;
    gScriptWork.mDefaultWindowLineCount = 2;
    gScriptWork.mUnk_24 = 0;
    gScriptWork.mUnk_26 = 0;
    gScriptWork.mSpriteDrawFunc = spriteDrawFunc;
    StartScript(line);
    UpdateScript(0);
}

// Frees the script engine's seven buffers and clears the script file id.
void ReleaseScript(void) {
    gScriptWork.mScriptFileId = 0;
    Free(&gScriptWork.mRecords);
    Free(&gScriptWork.mEyes);
    Free(&gScriptWork.mLsList);
    Free(&gScriptWork.mSprites);
    Free(&gScriptWork.mTexts);
    Free(&gScriptWork.mUnk_68);
    Free(&gScriptWork.mUnk_6c);
}

u8 ToUpper(u8 c) {
    if (c >= 'a' && c <= 'z') {
        c &= 0x5f;
    }
    return c;
}

int IsLetterOrUnderscore(u8 c) {
    c = ToUpper(c);
    if ((c >= 'A' && c <= 'Z') || c == '_') {
        return 1;
    }
    return 0;
}

int IsDigitOrSign(u8 c) {
    if ((c >= '0' && c <= '9') || c == '-' || c == '+') {
        return 1;
    }
    return 0;
}

int IsSign(u8 c) {
    if (c == '-' || c == '+') {
        return 1;
    }
    return 0;
}

int IsNotSignOrSpace(u8 c) {
    if (IsSign(c) == 0 && c != ' ') {
        return 1;
    }
    return 0;
}

int StringsEqual(const char *x, const char *y) {
    const u8 *a = (const u8 *) x;
    const u8 *b = (const u8 *) y;
    while (*a == *b) {
        if (*a == 0) {
            return 1;
        }
        a++;
        b++;
    }
    return 0;
}

// Copies the next identifier of *src, upper-cased, to *dst and advances both.
void CopyUpperName(u8 **src, u8 **dst) {
    u8 *p = *src;
    u8 *q = *dst;
    while (IsLetterOrUnderscore(*p) == 0) {
        p++;
    }
    while (IsLetterOrUnderscore(*p) == 1) {
        *q++ = ToUpper(*p++);
    }
    *q = 0;
    *dst = q;
    *src = p;
}

// Stores a 16-bit value little-endian at *out and advances *out.
void WriteS16(s16 value, u8 **out) {
    u8 *p = *out;
    p[0] = value;
    p[1] = value >> 8;
    *out = p + 2;
}

// Reads the next signed decimal number of *text; SCRIPT_NO_NUMBER when there is none.
s16 ReadNumber(u8 **text) {
    u8 *p = *text;
    s16 sign = 1;
    s16 value = 0;
    int found = 0;
    while (IsDigitOrSign(*p) == 0) {
        if (*p == 0) {
            return SCRIPT_NO_NUMBER;
        }
        p++;
    }
    while (IsDigitOrSign(*p) == 1) {
        int c = *p;
        if (c == '+') {
        } else if (c == '-') {
            sign = -sign;
        } else {
            found = 1;
            value = c - '0' + value * 10;
        }
        p++;
    }
    *text = p;
    if (found == 1) {
        return sign * value;
    }
    return SCRIPT_NO_NUMBER;
}

const u8 *FindScriptRecord(const u8 *name) {
    const u8 *rec = gScriptWork.mRecords;
    const u8 *p;
    const u8 *q;
    while (*rec != 0) {
        for (p = rec + 1, q = name; *q == *p && IsLetterOrUnderscore(*q) != 0 && IsLetterOrUnderscore(*q) == 1 &&
                                    IsLetterOrUnderscore(*p) == 1;
             p++, q++) {
        }
        if (IsLetterOrUnderscore(*q) == 0 && IsLetterOrUnderscore(*p) == 0) {
            return rec;
        }
        rec += *rec;
    }
    return NULL;
}

// Reads the values of a record: a count byte, then that many little-endian 16-bit values.
void ReadScriptRecordValues(const u8 *p, u16 *values, u32 *count) {
    u8 n;
    p++;
    while (IsLetterOrUnderscore(*p) == 1) {
        p++;
    }
    n = *p++;
    *count = n;
    if (n != 0) {
        do {
            *values++ = p[0] + (p[1] << 8);
            p += 2;
            n--;
        } while (n != 0);
    }
}

// Names of the script commands, in command order, padded to 6 characters.
static const u8 sCommandNames[SCRIPT_CMD_COUNT][6] = {
    "LET   ", "SPR   ", "SPRX  ", "SPRSCR", "SHPSCR", "SHAPE ", "TXT   ", "EOF   ", "TXTSPD",
    "ESC   ", "SYNC  ", "GOTO  ", "SCREEN", "IF    ", "EQ\t  ", "NE    ", "GT\t  ", "LT\t  ",
    "MUSIC ", "SOUND ", "BRA   ", "FADE  ", "SETEYE", "SETLS ", "SETLSC",
};

// DrawTextEx alignment for each value of a text's fifth value: 0 and 3 start the line at x, 1 centres it on x, 2 ends
// it at x.
static const u8 sTextAligns[4] = {0, 2, 1, 0};

// Adds (or, for '-', subtracts) count 16-bit values of src to dst.
void AddOrSubtractValues(const s16 *src, s16 *dst, int count, int op) {
    while (count-- != 0) {
        switch (op) {
            case '+':
                *dst++ += *src++;
                break;
            case '-':
                *dst++ -= *src++;
                break;
        }
    }
}

// Finds the command whose name starts the text, advances the text past it and returns its index.
int ReadScriptCommand(u8 **text) {
    u32 k;
    u8 *start = *text;
    const u8 *name = (const u8 *) sCommandNames;
    u32 i = 0;
    do {
        u8 c;
        k = 0;
        c = ToUpper(start[0]);
        if (c == name[0]) {
            for (;;) {
                u8 d;
                u8 *p;
                k++;
                p = start + (int) k;
                d = ToUpper(*p);
                if (d == 0 || d == ' ') {
                    *text = p;
                    return i;
                }
                if (d != name[k]) {
                    break;
                }
            }
        }
        name += 6;
        i++;
    } while (i < SCRIPT_CMD_COUNT);
    if (i >= SCRIPT_CMD_COUNT) {
        OS_Terminate();
    }
    return SCRIPT_CMD_COUNT;
}

// Stores the record for the text: its name, the count of values and the values themselves.
void SetScriptRecordFromText(const char *text) {
    u8 name[0x28];
    u8 *first;
    u8 *rest;
    u8 *dst;
    u8 *values;
    u8 *rec;
    u32 oldLen;
    u32 count = 0;
    rest = (u8 *) text;
    first = (u8 *) text;
    dst = name;
    CopyUpperName(&first, &dst);
    rec = (u8 *) FindScriptRecord(name);
    if (rec == 0) {
        oldLen = 0;
        rec = gScriptWork.mRecords;
        while (*rec != 0) {
            rec += *rec;
        }
    } else {
        oldLen = *rec;
    }
    dst = rec + 1;
    CopyUpperName(&rest, &dst);
    values = dst + 1;
    for (;;) {
        s16 v = ReadNumber(&rest);
        if (v == SCRIPT_NO_NUMBER) {
            break;
        }
        WriteS16(v, &values);
        count++;
    }
    if ((u32) (values - gScriptWork.mRecords) >= 0x800) {
        OS_Terminate();
    }
    *rec = values - rec;
    *dst = count;
    if (oldLen != 0 && *rec != oldLen) {
        OS_Terminate();
    }
}

// Copies the first word of src, upper-cased, to dst; returns the end of the copy.
char *CopyUpperWord(char *dst, const char *src) {
    u8 c;
    do {
        c = *src++;
        if (c == 0) {
            OS_Terminate();
        }
    } while (c == ' ');
    while (c != ' ' && c != 0) {
        *dst++ = ToUpper(c);
        c = *src++;
    }
    *dst = 0;
    return dst;
}

// Clears the output and parses the text into it; returns the number of values.
int ClearAndEvaluate(s16 *out, u8 *text, u32 size) {
    MIi_CpuClear16(0, out, size);
    return Evaluate(out, text);
}

// Evaluates the text: names of records and numbers, joined by signs, added into out (subtracted after '-'); out is not
// cleared first (ClearAndEvaluate does that). Returns the number of values.
int Evaluate(s16 *out, u8 *text) {
    s16 buf[0x10];
    u8 name[0x10];
    u32 count;
    u8 *dst;
    const u8 *rec;
    int n = 0;
    int have = 0;
    int off;
    u8 op = '+';
    MIi_CpuClear16(0, buf, 0x20);
    while (*text != 0) {
        if (IsNotSignOrSpace(*text) == 1) {
            have = 0;
            op = '+';
        }
        if (IsLetterOrUnderscore(*text) == 1) {
            dst = name;
            CopyUpperName(&text, &dst);
            rec = FindScriptRecord(name);
            // rec is compared with 0 twice (two compares in the code).
            if (rec == 0 && rec == 0) {
                OS_Terminate();
            }
            off = n;
            ReadScriptRecordValues(rec, buf + off, &count);
            AddOrSubtractValues(buf + off, out + off, count, op);
            have = 1;
            n += count;
        } else if (IsSign(*text) == 1 && have == 1) {
            op = *text++;
            n -= count;
        } else if (IsDigitOrSign(*text) == 1) {
            off = n;
            buf[off] = ReadNumber(&text);
            AddOrSubtractValues(buf + off, out + off, 1, op);
            count = 1;
            n++;
            have = 1;
        } else {
            text++;
        }
    }
    return n;
}

// Reads a sprite (mode 0) or a shape (mode 1) from the text into the next entry of mSprites.
void AddScriptSprite(u8 *text, int mode) {
    s16 buf[12];
    TexInfo info;
    s32 rect[4];
    ScriptSprite *sprite;
    u32 index;
    int n = ClearAndEvaluate(buf, text, 0x18);
    u32 tableValue;
    u32 shiftX;
    u32 shiftY;
    if (n < 4) {
        OS_Terminate();
    }
    if (n > 12) {
        OS_Terminate();
    }
    if (gScriptWork.mSpriteCount >= 0x20) {
        OS_Terminate();
    }
    SelectLsForSprite(gScriptWork.mSpriteCount, buf[0]);
    index = gScriptWork.mSpriteCount++;
    sprite = gScriptWork.mSprites + index;
    sprite->mId = buf[0];
    if (mode == 0) {
        sprite->mDrawId = gScriptSpriteIds[buf[0]];
        if (buf[4] & SCRIPT_SPRITE_LAYER_1) {
            sprite->mLayer = 1;
        } else {
            sprite->mLayer = 2;
        }
        GetTexInfo(sprite->mDrawId, &info, 0);
        // Both arms clear bit 4 of mUnk_21: one through the bit-field, one as a byte operation, because the
        // compiler's output keeps the test and two separate stores (with one form in both arms the compiler merges
        // the arms and drops the test).
        if (info.mUnk_12 > 1) {
            sprite->mUnk_21_4 = 0;
        } else {
            sprite->mUnk_21 &= ~0x10;
        }
    } else {
        tableValue = data_0204e214[buf[0]];
        sprite->mDrawId = 0x80000000 | (u16) tableValue;
        shiftX = 1 << (tableValue >> 24);
        shiftY = 1 << (u8) (tableValue >> 16);
        if (sprite->mDrawOrder >= 0x100) {
            OS_Terminate();
        }
        sprite->mUnk_21_4 = 0;
        sprite->mLayer = 1;
    }
    sprite->mX = buf[1];
    sprite->mY = buf[2];
    sprite->mDrawOrder = buf[3];
    sprite->mDelayTimer = buf[5];
    sprite->mShowTimer = buf[6];
    sprite->mAnimFramesRemaining = buf[7];
    sprite->mAnimFrameCount = sprite->mAnimFramesRemaining;
    sprite->mFramesPerAnimFrame = buf[8];
    sprite->mAnimFrameFrames = 0;
    sprite->mMoveEndX = buf[9] * 100;
    sprite->mMoveEndY = buf[10] * 100;
    sprite->mMovePerFrame = buf[11];
    if (buf[4] & SCRIPT_SPRITE_ANIM_LOOP) {
        sprite->mAnimLoop = 1;
    } else {
        sprite->mAnimLoop = 0;
    }
    if (sprite->mMovePerFrame != 0) {
        sprite->mX = sprite->mX * 100;
        sprite->mY = sprite->mY * 100;
        sprite->mStartX = sprite->mX;
        sprite->mStartY = sprite->mY;
        sprite->mNoReverse = 1;
    }
    if (buf[4] & SCRIPT_SPRITE_MOVE_BOUNCE) {
        sprite->mMoveBounce = 1;
    } else {
        sprite->mMoveBounce = 0;
    }
    if (buf[4] & SCRIPT_SPRITE_FLIP_ON_RETURN) {
        sprite->mFlipOnReturn = 1;
    } else {
        sprite->mFlipOnReturn = 0;
    }
    if (!(buf[4] & SCRIPT_SPRITE_SLIDE) && sprite->mMovePerFrame == 0) {
        sprite->mSlideProgress = 0x80;
        sprite->mStartX = sprite->mX;
        sprite->mStartY = sprite->mY;
        sprite->mNoReverse = 0;
        return;
    }
    if (mode == 0) {
        GetSpriteRect(sprite->mDrawId, 0, rect);
        if (buf[4] & SCRIPT_SPRITE_SLIDE_X) {
            sprite->mStartY = sprite->mY;
            if (buf[4] & 2) {
                s32 v;
                if (sprite->mDrawId & 0x10000) {
                    v = -rect[0];
                } else {
                    v = rect[2];
                }
                sprite->mStartX = -v;
            } else {
                s32 v;
                if (sprite->mDrawId & 0x10000) {
                    v = rect[2];
                } else {
                    v = -rect[0];
                }
                sprite->mStartX = v + 0x100;
            }
        } else {
            sprite->mStartX = sprite->mX;
            if (buf[4] & 2) {
                sprite->mStartY = 0xc0 - rect[1];
            } else {
                sprite->mStartY = -rect[3];
            }
        }
    } else {
        if (buf[4] & SCRIPT_SPRITE_SLIDE_X) {
            sprite->mStartY = sprite->mY;
            if (buf[4] & 2) {
                sprite->mStartX = -shiftX;
            } else {
                sprite->mStartX = 0x100;
            }
        } else {
            sprite->mStartX = sprite->mX;
            if (buf[4] & 2) {
                sprite->mStartY = 0xc0;
            } else {
                sprite->mStartY = -shiftY;
            }
        }
    }
    sprite->mSlideProgress = 0;
}

// Reads a text from the script line and appends it to mTexts, or to mUnk_68 when its seventh value is non-zero
// (nothing reads that list in this build).
void AddScriptText(u8 *text) {
    s16 buf[8];
    ScriptText *entry;
    u32 n = ClearAndEvaluate(buf, text, 0x10);
    if (n - 6 > 2) {
        OS_Terminate();
    }
    if (buf[6] == 0) {
        entry = gScriptWork.mTexts + gScriptWork.mTextCount++;
        if (gScriptWork.mTextCount >= 0x10) {
            gScriptWork.mTextCount = 0xf;
        }
    } else {
        entry = gScriptWork.mUnk_68 + gScriptWork.mUnk_24++;
        if (gScriptWork.mUnk_24 >= 4) {
            gScriptWork.mUnk_24 = 3;
        }
    }
    entry->mTextId = gScriptWork.mNextTextId++;
    entry->mX = buf[0];
    entry->mY = buf[1];
    entry->mLineWidth = buf[2];
    entry->mLineHeight = buf[3];
    entry->mAlign = sTextAligns[buf[4]];
    entry->mFontIndex = buf[5];
    if (buf[7] == 0) {
        entry->mLayer = 2;
    } else {
        entry->mLayer = 1;
    }
    entry->mCharCount = CountFileStringChars(gScriptWork.mTextFileId, entry->mTextId,
                                             (gScriptWork.mUnk_280.mTextEscapeFunc != 0 ? 2 : 0) + 1);
    entry->mCharFrameCount = gScriptWork.mCharFrameCount;
}

// Sets mCharFrameCount to 3 plus the value read from the text (Evaluate adds into the 3), or to 3 without a value.
void ScriptCmdTxtSpd(u8 *text) {
    s16 value[1] = {3};
    if (Evaluate(value, text) > 1) {
        OS_Terminate();
    }
    gScriptWork.mCharFrameCount = value[0];
}

// Sets the state to SCRIPT_STATE_END and plays the queued MUSIC track; then for each queued sound id (mapped through
// mSoundIdFunc when set, never in this build) stops it in mode 2 or plays it otherwise unless mSkipToEnd is set, and
// empties the sound list.
void PlayScriptTrackAndSounds(void) {
    u32 i;
    gScriptWork.mState = SCRIPT_STATE_END;
    if (gScriptWork.mTrackFileId > 0) {
        PlayTrack(gScriptWork.mTrackFileId, 0, gScriptWork.mTrackVolume, 0, 1, 0);
        gScriptWork.mUnk_30 = gScriptWork.mTrackVolume;
        gScriptWork.mTrackFileId = 0;
    }
    for (i = 0; i < gScriptWork.mSoundCount; i++) {
        u32 id;
        if (gScriptWork.mSoundIdFunc == 0) {
            id = gScriptWork.mSoundIds[i];
        } else {
            id = gScriptWork.mSoundIdFunc(gScriptWork.mSoundIds[i]);
        }
        if (gScriptWork.mSoundModes[i] == 2) {
            StopWave(id);
        } else if (!gScriptWork.mSkipToEnd) {
            PlayWave(id, gScriptWork.mSoundVolumes[i], gScriptWork.mSoundModes[i], 0x40, 0xffff, 0);
        }
    }
    gScriptWork.mSoundCount = 0;
}

// Reads the EOF command's own flags (mEofFlags, read by the wait and finish states) and the values that go with them:
// flag 8 plays the queued track and sounds and sets the wait timer, flag 0x10 sets the number of lines shown at a time.
void ScriptCmdEof(u8 *text) {
    s16 buf[3];
    if (ClearAndEvaluate(buf, text, 6) > 3) {
        OS_Terminate();
    }
    gScriptWork.mEofFlags = buf[0];
    if (gScriptWork.mEofFlags & 8) {
        PlayScriptTrackAndSounds();
        gScriptWork.mWaitTimer = buf[1];
    }
    gSystemState.mTextWindowLineIndex = 0;
    if (gScriptWork.mEofFlags & 0x10) {
        gSystemState.mTextWindowLineCount = buf[2];
    } else {
        gSystemState.mTextWindowLineCount = gScriptWork.mDefaultWindowLineCount;
    }
}

// Reads up to 16 values from the text into the next entry of the event queue. There is no full check: a write can
// overwrite an entry that was not read yet.
void QueueScriptEvent(u8 *text) {
    int len = ClearAndEvaluate(&gScriptWork.mEvents[gScriptWork.mUnk_280.mEventWriteIndex].mKind, text, 0x20);
    if (len > 0x10) {
        OS_Terminate();
    }
    gScriptWork.mEvents[gScriptWork.mUnk_280.mEventWriteIndex].mValueCount = len;
    gScriptWork.mUnk_280.mEventWriteIndex = (gScriptWork.mUnk_280.mEventWriteIndex + 1) & 0xf;
}

// Finds the entry of the text-name file (mTextNameFileId) whose first word, upper-cased, equals name's and stores its
// index in mNextTextId.
void ScriptCmdSync(const char *name) {
    char first[0x20];
    char second[0x20];
    int i;
    CopyUpperWord(first, name);
    for (i = 2;; i++) {
        if (GetFileValueType(gScriptWork.mTextNameFileId, i) == 4) {
            const u8 *s = GetFileString(gScriptWork.mTextNameFileId, i);
            if (s[0] != 0) {
                CopyUpperWord(second, s);
                if (StringsEqual(first, second) == 1) {
                    gScriptWork.mNextTextId = i;
                    return;
                }
            }
        }
    }
}

// Moves mLineId and mNextTextId by the two values read from the text (relative).
void ScriptCmdGoto(u8 *text) {
    s16 v[0x10];
    if (ClearAndEvaluate(v, text, 0x20) > 2) {
        OS_Terminate();
    }
    gScriptWork.mLineId += v[0];
    gScriptWork.mNextTextId += v[1];
}

// The SPRSCR (flag 1) and SHPSCR (flag 0) commands: reads an index and up to two more values. Unless mUnk_280.mUnk_40
// is set (SetupPrompt sets it for a prompt with a shared model), mScrDrawId becomes the sprite or shape table entry
// with the marker bits described at mScrDrawId, and the two values become mScrX and mScrY; while it is set the call
// stores nothing.
void SetScriptScrSprite(u8 *text, int flag) {
    s16 v[3];
    int n;
    s32 x;
    s32 sel;
    v[1] = 0;
    v[2] = 0;
    n = ClearAndEvaluate(v, text, 6);
    if (n != 1 && n != 3) {
        OS_Terminate();
    }
    {
        Unk_020c642c *o = &gScriptWork.mUnk_280;
        if (o->mUnk_40 == 0) {
            if (flag == 0) {
                x = data_0204e214[v[0]] & 0x7fff;
                sel = 2;
            } else {
                x = gScriptSpriteIds[v[0]];
                sel = 1;
            }
            o->mScrDrawId = x - (sel << 30);
            gScriptWork.mUnk_280.mScrX = v[1];
            gScriptWork.mUnk_280.mScrY = v[2];
        }
    }
}

// Reads a value and a mode; odd modes store the value in mPendingImageIndices[0] with the mode in
// mPendingImageLayers[0], even modes store it in mPendingImageIndices[1] and set mPendingImageLayers[1] to 2.
void ScriptCmdScreen(u8 *text) {
    s16 v[4];
    if (ClearAndEvaluate(v, text, 8) > 2) {
        OS_Terminate();
    }
    if (v[1] & 1) {
        gScriptWork.mUnk_280.mPendingImageIndices[0] = v[0];
        gScriptWork.mUnk_280.mPendingImageLayers[0] = v[1];
    } else {
        gScriptWork.mUnk_280.mPendingImageIndices[1] = v[0];
        gScriptWork.mUnk_280.mPendingImageLayers[1] = 2;
    }
}

// Reads the two values compared by the conditional commands.
void ScriptCmdIf(u8 *text) {
    s16 v[2];
    if (ClearAndEvaluate(v, text, 4) != 2) {
        OS_Terminate();
    }
    gScriptWork.mUnk_280.mIfValueA = v[0];
    gScriptWork.mUnk_280.mIfValueB = v[1];
}

// Adds the value to mLineId when the two compared values are equal.
void ScriptCmdEq(u8 *text) {
    s16 v[0x10];
    if (gScriptWork.mUnk_280.mIfValueA == gScriptWork.mUnk_280.mIfValueB) {
        if (ClearAndEvaluate(v, text, 0x20) != 1) {
            OS_Terminate();
        }
        gScriptWork.mLineId += v[0];
    }
}

// Adds the value to mLineId when the two compared values differ.
void ScriptCmdNe(u8 *text) {
    s16 v[0x10];
    if (gScriptWork.mUnk_280.mIfValueA != gScriptWork.mUnk_280.mIfValueB) {
        if (ClearAndEvaluate(v, text, 0x20) != 1) {
            OS_Terminate();
        }
        gScriptWork.mLineId += v[0];
    }
}

// Adds the value to mLineId when the first compared value is greater.
void ScriptCmdGt(u8 *text) {
    s16 v[8];
    if (gScriptWork.mUnk_280.mIfValueA > gScriptWork.mUnk_280.mIfValueB) {
        if (ClearAndEvaluate(v, text, 0x10) != 1) {
            OS_Terminate();
        }
        gScriptWork.mLineId += v[0];
    }
}

// Adds the value to mLineId when the first compared value is smaller.
void ScriptCmdLt(u8 *text) {
    s16 v[8];
    if (gScriptWork.mUnk_280.mIfValueA < gScriptWork.mUnk_280.mIfValueB) {
        if (ClearAndEvaluate(v, text, 0x10) != 1) {
            OS_Terminate();
        }
        gScriptWork.mLineId += v[0];
    }
}

// Reads a track index and an optional volume (default 100) into mTrackFileId and mTrackVolume; the index goes
// through the remap table when that is enabled, which it is not in this build (its first entry is 0).
void ScriptCmdMusic(u8 *text) {
    s16 v[4] = {0, 0, 0, 0};
    int n = Evaluate(v, text);
    if (n != 1 && n != 2) {
        OS_Terminate();
    }
    if (n == 1) {
        v[1] = 100;
    }
    if (gScriptTrackFileIds[0] != 0) {
        gScriptWork.mTrackFileId = gScriptTrackFileIds[v[0]];
    } else {
        gScriptWork.mTrackFileId = v[0];
    }
    gScriptWork.mTrackVolume = v[1];
}

// Appends one sound at mSoundCount: the id from the sound table, the volume (default 100) and the mode in 0..2.
void ScriptCmdSound(u8 *text) {
    s16 v[4] = {0, 0, 0, 0};
    u8 idx = gScriptWork.mSoundCount++;
    int n = Evaluate(v, text);
    int ok;
    if (n < 1 || n > 3) {
        OS_Terminate();
    }
    if (n == 1) {
        v[1] = 100;
    }
    ok = 0;
    if (v[2] >= 0 && v[2] <= 2) {
        ok = 1;
    }
    if (ok == 0) {
        OS_Terminate();
    }
    gScriptWork.mSoundIds[idx] = gScriptSoundIds[v[0]];
    gScriptWork.mSoundVolumes[idx] = v[1];
    gScriptWork.mSoundModes[idx] = v[2];
}

// Starts the script at the line named by the text, then steps mLineId back by one (the reader's increment follows).
void ScriptCmdBra(const char *name) {
    StartScriptAtName(name);
    gScriptWork.mLineId--;
}

// Reads flags; bit 0 and bit 1 each trigger SetFaded with that bit.
void ScriptCmdFade(u8 *text) {
    s16 v[1] = {0};
    if (Evaluate(v, text) != 1) {
        OS_Terminate();
    }
    if (v[0] & 1) {
        SetFaded(1);
    }
    if (v[0] & 2) {
        SetFaded(2);
    }
}

// Parses a text command (slot number, two ids, a value and up to five timing values) and fills that slot,
// or marks it unused when only the slot number is given.
void ScriptCmdSetEye(u8 *text) {
    s16 v[9] = {0};
    ScriptEye *slot;
    int n = Evaluate(v, text);
    BOOL ok;
    if (n != 1 && (n < 4 || n >= 10)) {
        OS_Terminate();
    }
    ok = 0;
    if (v[0] >= 0 && v[0] < 0x20) {
        ok = 1;
    }
    if (ok == 0) {
        OS_Terminate();
    }
    slot = &gScriptWork.mEyes[v[0]];
    if (n > 1) {
        slot->mUsed = 1;
        slot->mId = v[1];
        slot->mAltId = v[2];
        if (slot->mId < 0 && slot->mAltId < 0) {
            OS_Terminate();
        }
        if (slot->mId < -1 || slot->mAltId < -1) {
            OS_Terminate();
        }
        slot->mSpriteId = v[3];
        if (slot->mSpriteId < 0) {
            OS_Terminate();
        }
        slot->mDelayMin = 0x3c;
        slot->mDelayLimit = 0x78;
        slot->mAltFrameCount = 0x1e;
        slot->mOffsetX = 0;
        slot->mOffsetY = 0;
        if (n > 4) {
            slot->mDelayMin = v[4];
            if (n > 5) {
                slot->mDelayLimit = v[5];
                if (n > 6) {
                    slot->mAltFrameCount = v[6];
                    if (n > 7) {
                        slot->mOffsetX = v[7];
                        if (n > 8) {
                            slot->mOffsetY = v[8];
                        }
                    }
                }
            }
        }
    } else {
        slot->mUsed = 0;
    }
}

// Looks up the eye in use whose sprite id equals spriteId, returns in id the id it shows (mId, or mAltId while the
// alternate shows; -1 when no eye matches) and in dx, dy its offsets, and steps its countdown; when the countdown is -1
// it draws a new delay from mDelayMin up to mDelayLimit - 1.
void UpdateScriptEye(int spriteId, int *id, int *dx, int *dy) {
    ScriptEye *slot;
    u32 i;
    *id = -1;
    for (i = 0, slot = gScriptWork.mEyes; i < 0x20; i++, slot++) {
        if (slot->mUsed == 1 && slot->mSpriteId == spriteId) {
            *dx = slot->mOffsetX;
            *dy = slot->mOffsetY;
            *id = slot->mId;
            if (slot->mTimer == -1) {
                u16 lo;
                slot->mShowingAlt = 0;
                lo = slot->mDelayMin;
                slot->mTimer = lo + NextRandom() % (slot->mDelayLimit - lo);
                return;
            }
            if (slot->mShowingAlt == 1) {
                *id = slot->mAltId;
            }
            slot->mTimer--;
            if (slot->mTimer == 0) {
                if (slot->mShowingAlt == 0) {
                    slot->mTimer = slot->mAltFrameCount;
                    slot->mShowingAlt = 1;
                    return;
                }
                slot->mTimer = -1;
                return;
            }
            return;
        }
    }
}

// Parses a text command (slot number, a value and up to five more) and fills that Ls entry,
// or marks it unused when only the slot number is given.
void ScriptCmdSetLs(u8 *text) {
    s16 v[8] = {0};
    ScriptLs *rec;
    int n = Evaluate(v, text);
    BOOL ok;
    if (n != 1 && (n < 3 || n >= 9)) {
        OS_Terminate();
    }
    ok = 0;
    if (v[0] >= 0 && v[0] < 0x20) {
        ok = 1;
    }
    if (ok == 0) {
        OS_Terminate();
    }
    rec = &gScriptWork.mLsList[v[0]];
    if (n > 1) {
        rec->mUsed = 1;
        rec->mStillId = v[1];
        if (rec->mStillId < -1) {
            OS_Terminate();
        }
        rec->mAnimId = v[2];
        rec->mAnimFrameCount = 2;
        rec->mFramesPerAnimFrame = 2;
        rec->mOffsetX = 0;
        rec->mOffsetY = 0;
        rec->mSpriteId = -1;
        if (n > 3) {
            rec->mAnimFrameCount = v[3];
            if (n > 4) {
                rec->mFramesPerAnimFrame = v[4];
                if (n > 5) {
                    rec->mOffsetX = v[5];
                    if (n > 6) {
                        rec->mOffsetY = v[6];
                        if (n > 7) {
                            rec->mSpriteId = v[7];
                        }
                    }
                }
            }
        }
    } else {
        rec->mUsed = 0;
    }
}

// Parses a text command with no values or two (an Ls index and a sprite index) and selects them.
void ScriptCmdSetLsc(u8 *text) {
    s16 v[2] = {0};
    int n = Evaluate(v, text);
    if (n != 0 && n != 2) {
        OS_Terminate();
    }
    if (n == 2) {
        SelectLs(v[0], v[1]);
    } else {
        SelectLs(-1, -1);
    }
}

// Stores the two selected indexes (each -1 or 0..0x1f) and, when both are set, checks that the Ls entry is in use.
void SelectLs(int lsIndex, int spriteIndex) {
    BOOL ok;
    gScriptWork.mUnk_280.mLsIndex = lsIndex;
    gScriptWork.mUnk_280.mLsSpriteIndex = spriteIndex;
    ok = 0;
    if (gScriptWork.mUnk_280.mLsIndex >= -1 && gScriptWork.mUnk_280.mLsIndex < 0x20) {
        ok = 1;
    }
    if (ok == 0) {
        OS_Terminate();
    }
    ok = 0;
    if (gScriptWork.mUnk_280.mLsSpriteIndex >= -1 && gScriptWork.mUnk_280.mLsSpriteIndex < 0x20) {
        ok = 1;
    }
    if (ok == 0) {
        OS_Terminate();
    }
    if (gScriptWork.mUnk_280.mLsIndex != -1 && gScriptWork.mUnk_280.mLsSpriteIndex != -1) {
        if (gScriptWork.mLsList[gScriptWork.mUnk_280.mLsIndex].mUsed != 1) {
            OS_Terminate();
        }
    }
}

// Selects the Ls entry in use whose sprite id equals id for the sprite at index; when none matches, the previous
// selection stays.
void SelectLsForSprite(int index, int id) {
    ScriptLs *rec;
    u32 i;
    for (i = 0, rec = gScriptWork.mLsList; i < 0x20; i++, rec++) {
        if (rec->mUsed == 1 && rec->mSpriteId == id) {
            SelectLs(i, index);
            return;
        }
    }
}

// Swaps the sprite's position with its slide start and restarts the slide, so it slides back; not when mNoReverse is
// set (moving sprites, and a sprite its show timer already reversed).
void ReverseScriptSprite(ScriptSprite *rec) {
    if (!rec->mNoReverse) {
        s16 a = rec->mX;
        rec->mX = rec->mStartX;
        rec->mStartX = a;
        a = rec->mY;
        rec->mY = rec->mStartY;
        rec->mStartY = a;
        rec->mSlideProgress = 0;
    }
}

// Runs ReverseScriptSprite on every sprite.
void ReverseScriptSprites(void) {
    u32 i;
    for (i = 0; i < gScriptWork.mSpriteCount; i++) {
        ReverseScriptSprite(&gScriptWork.mSprites[i]);
    }
}

// Ends the wait after an EOF line: sets the state to SCRIPT_STATE_FINISH, clears the texts unless EOF flag 2 is set
// and reverses the sprites unless EOF flag 1 is set.
void StartEofFinish(void) {
    gScriptWork.mState = SCRIPT_STATE_FINISH;
    if (!(gScriptWork.mEofFlags & 2)) {
        gScriptWork.mTextCount = 0;
    }
    if (!(gScriptWork.mEofFlags & 1)) {
        ReverseScriptSprites();
    }
}

u8 GetScriptState(void) {
    return gScriptWork.mState;
}

void ClearScriptEvents(void) {
    gScriptWork.mUnk_280.mEventReadIndex = 0;
    gScriptWork.mUnk_280.mEventWriteIndex = 0;
}

// Takes the next event off the queue; returns 0 when it is empty.
ScriptEvent *NextScriptEvent(void) {
    u8 read = gScriptWork.mUnk_280.mEventReadIndex;
    if (read != gScriptWork.mUnk_280.mEventWriteIndex) {
        gScriptWork.mUnk_280.mEventReadIndex = (read + 1) & 0xf;
        return &gScriptWork.mEvents[read];
    }
    return NULL;
}

// Resets one camera byte and steps another through 0..0x1f.
void func_0202a1cc(void) {
    SetupOrthoSpriteView();
    gMapState.mCamera.mUnk_2dc.mPolygonAlpha = 0;
    gMapState.mCamera.mUnk_2dc.mPolygonId++;
    gMapState.mCamera.mUnk_2dc.mPolygonId &= 0x1f;
}

// Runs one frame of the script engine: applies queued screen changes, moves and draws the script sprites, builds
// and draws the message text, then reads script commands until one waits; repeats while mSkipToEnd is set (never in
// this build).
void UpdateScript(int (*callback)(void)) {
    char text[0x200];
    u8 name[0x80];
    TexInfo info;
    int dx;
    int dy;
    int id;
    u8 *line;
    BOOL flag;
    BOOL done;
    int i;
    u32 start;
    s32 flip;
    u8 saved;

    if (gScriptWork.mUnk_280.mTextEscapeFunc != 0) {
        gSystemState.mTextEscapeFunc = (u32 (*)(u32)) gScriptWork.mUnk_280.mTextEscapeFunc;
    }
    do {
        done = 0;
        flag = 0;
        if (gScriptWork.mScriptFileId == 0) {
            OS_Terminate();
        }
        gScriptWork.mSpritesBusy = 0;
        for (i = 0; i < 2; i++) {
            if (gScriptWork.mUnk_280.mPendingImageLayers[i] != 0) {
                u16 image = gScriptImageIds[gScriptWork.mUnk_280.mPendingImageIndices[i]];
                SetFaded(gScriptWork.mUnk_280.mPendingImageLayers[i]);
                LoadImage(image, gScriptWork.mUnk_280.mPendingImageLayers[i]);
                if (gScriptWork.mUnk_280.mPendingImageLayers[i] == 1) {
                    gScriptWork.mUnk_280.mScrDrawId = 0;
                    gScriptWork.mUnk_280.mLayer1ImageId = image;
                    flag = 1;
                } else if (gScriptWork.mUnk_280.mPendingImageLayers[i] == 2) {
                    gScriptWork.mUnk_280.mLayer2ImageId = image;
                } else if (gScriptWork.mUnk_280.mPendingImageLayers[i] == 3) {
                    flag = 1;
                    gScriptWork.mUnk_280.mLayer1ImageId = gScriptWork.mUnk_280.mLayer2ImageId = image;
                }
                gScriptWork.mUnk_280.mPendingImageLayers[i] = 0;
            }
            if (flag == 1 && (gSystemState.mFlags & 0x80)) {
                func_02024208();
            }
        }

        if (gScriptWork.mUnk_280.mScrDrawId != 0) {
            if (gScriptWork.mUnk_280.mScrDrawId & 0x80000000) {
                gScriptWork.mUnk_280.mScrDrawId &= 0x7fffffff;
                SetFaded(1);
                gScriptWork.mUnk_280.mUnk_30 = gScriptWork.mUnk_280.mScrDrawId;
            }
            if (!(gScriptWork.mUnk_280.mScrDrawId & 0x40000000)) {
                DrawModelFileOrtho(gScriptWork.mUnk_280.mScrDrawId, gScriptWork.mUnk_280.mScrX,
                                   gScriptWork.mUnk_280.mScrY, 0x4000, 0x1000, 0x8000, 0xff, 0);
            } else {
                u32 tex = gScriptWork.mUnk_280.mScrDrawId & 0x3fffffff;
                GetTexInfo(tex, &info, 0);
                if (info.mUnk_12 <= 1) {
                    OS_Terminate();
                }
                func_0202a1cc();
                DrawSprite(tex + 0x800000, gScriptWork.mUnk_280.mScrX, gScriptWork.mUnk_280.mScrY, 0xff, 1);
            }
        }

        for (i = 0; i < gScriptWork.mSpriteCount; i++) {
            ScriptSprite *sprite = &gScriptWork.mSprites[i];
            int x;
            int y;
            flip = 0;
            if (sprite->mDelayTimer != 0) {
                gScriptWork.mSpritesBusy = 1;
                sprite->mDelayTimer--;
                continue;
            }
            if (sprite->mShowTimer != 0) {
                gScriptWork.mSpritesBusy = 1;
                sprite->mShowTimer--;
                if (sprite->mShowTimer == 0) {
                    if (sprite->mStartX == sprite->mX && sprite->mStartY == sprite->mY) {
                        sprite->mDrawId = -1;
                    } else {
                        ReverseScriptSprite(sprite);
                        sprite->mNoReverse = 1;
                    }
                }
            }
            if (sprite->mFramesPerAnimFrame != 0 && sprite->mDrawId != -1) {
                gScriptWork.mSpritesBusy = 1;
                sprite->mAnimFrameFrames++;
                if (sprite->mAnimFrameFrames == sprite->mFramesPerAnimFrame) {
                    sprite->mDrawId++;
                    sprite->mAnimFrameFrames = 0;
                    sprite->mAnimFramesRemaining--;
                    if (sprite->mAnimFramesRemaining == 0) {
                        if (sprite->mAnimLoop == 0) {
                            sprite->mDrawId--;
                            sprite->mFramesPerAnimFrame = 0;
                        } else {
                            gScriptWork.mSpritesBusy = 0;
                            sprite->mDrawId -= sprite->mAnimFrameCount;
                            sprite->mAnimFramesRemaining = sprite->mAnimFrameCount;
                        }
                    }
                }
            }
            if (sprite->mMovePerFrame != 0) {
                s16 to = sprite->mMovePerFrame > 0 ? sprite->mMoveEndX : sprite->mStartX;
                if (sprite->mX < to) {
                    sprite->mX += abs(sprite->mMovePerFrame);
                    if (sprite->mX >= to) {
                        sprite->mX = to;
                        if (sprite->mMoveBounce == 1) {
                            sprite->mMovePerFrame = -sprite->mMovePerFrame;
                        }
                    }
                } else if (sprite->mX > to) {
                    sprite->mX -= abs(sprite->mMovePerFrame);
                    if (sprite->mX <= to) {
                        sprite->mX = to;
                        if (sprite->mMoveBounce == 1) {
                            sprite->mMovePerFrame = -sprite->mMovePerFrame;
                        }
                    }
                }
                to = sprite->mMovePerFrame > 0 ? sprite->mMoveEndY : sprite->mStartY;
                if (sprite->mY < to) {
                    sprite->mY += abs(sprite->mMovePerFrame);
                    if (sprite->mY >= to) {
                        sprite->mY = to;
                        if (sprite->mMoveBounce == 1) {
                            sprite->mMovePerFrame = -sprite->mMovePerFrame;
                        }
                    }
                } else if (sprite->mY > to) {
                    sprite->mY -= abs(sprite->mMovePerFrame);
                    if (sprite->mY <= to) {
                        sprite->mY = to;
                        if (sprite->mMoveBounce == 1) {
                            sprite->mMovePerFrame = -sprite->mMovePerFrame;
                        }
                    }
                }
                if (sprite->mMovePerFrame < 0 && sprite->mFlipOnReturn == 1) {
                    flip = 0x10000;
                }
                x = sprite->mX / 100;
                y = sprite->mY / 100;
            } else if (sprite->mSlideProgress < 0x80) {
                if (sprite->mUnk_21_4 == 0) {
                    sprite->mSlideProgress += 20;
                } else if (sprite->mUnk_21_4 > 1) {
                    sprite->mUnk_21_4--;
                } else if (gSystemState.mTransferCount == 0) {
                    sprite->mUnk_21_4 = 0;
                }
                gScriptWork.mSpritesBusy = 1;
                x = MapRangeFromZero(sprite->mSlideProgress < 0x80 ? sprite->mSlideProgress : 0x80, 0x80,
                                     sprite->mStartX, sprite->mX);
                y = MapRangeFromZero(sprite->mSlideProgress < 0x80 ? sprite->mSlideProgress : 0x80, 0x80,
                                     sprite->mStartY, sprite->mY);
                if (x == sprite->mX && y == sprite->mY) {
                    sprite->mSlideProgress = 0x80;
                }
            } else {
                x = sprite->mX;
                y = sprite->mY;
            }

            if (gScriptWork.mSkipToEnd == 0) {
                if (!(sprite->mDrawId & 0x80000000)) {
                    if (gScriptWork.mSpriteDrawFunc != 0) {
                        gScriptWork.mSpriteDrawFunc(sprite->mDrawId ^ flip, x, y, sprite->mDrawOrder, sprite->mLayer);
                    } else {
                        GetTexInfo(sprite->mDrawId, &info, 0);
                        if (info.mUnk_12 > 1) {
                            func_0202a1cc();
                            DrawSprite(sprite->mDrawId ^ (flip + 0x800000), x, y, sprite->mDrawOrder, sprite->mLayer);
                        } else {
                            DrawSprite(flip ^ sprite->mDrawId, x, y, sprite->mDrawOrder, sprite->mLayer);
                        }
                    }
                } else {
                    DrawModelFileOrtho(sprite->mDrawId & 0x7fffffff, x, y, sprite->mDrawOrder << 6, 0x1000, 0x8000,
                                       0xff, 0);
                }
            }

            UpdateScriptEye(sprite->mId, &id, &dx, &dy);
            if (id != -1) {
                id = gScriptSpriteIds[id];
                GetTexInfo(id, &info, 0);
                if (info.mUnk_12 > 1) {
                    func_0202a1cc();
                    DrawSprite(id + flip + 0x800000, x + dx, y + dy, sprite->mDrawOrder - 2, sprite->mLayer);
                } else {
                    DrawSprite(id + flip, x + dx, y + dy, sprite->mDrawOrder - 2, sprite->mLayer);
                }
            }

            if (gScriptWork.mUnk_280.mLsIndex != -1 && gScriptWork.mUnk_280.mLsSpriteIndex == i) {
                ScriptLs *rec = &gScriptWork.mLsList[gScriptWork.mUnk_280.mLsIndex];
                id = -1;
                if (gScriptWork.mUnk_280.mLsTimer != 0) {
                    gScriptWork.mUnk_280.mLsTimer--;
                    // Always true: mAnimId is 16-bit.
                    if (rec->mAnimId != -1) {
                        id = gScriptSpriteIds[rec->mAnimId] +
                             (gSystemState.mElapsedFrames / rec->mFramesPerAnimFrame) % rec->mAnimFrameCount;
                    }
                } else if (rec->mStillId != -1) {
                    id = gScriptSpriteIds[rec->mStillId];
                }
                if (id != -1) {
                    GetTexInfo(id, &info, 0);
                    if (info.mUnk_12 > 1) {
                        func_0202a1cc();
                        DrawSprite(id + flip + 0x800000, x + rec->mOffsetX, y + rec->mOffsetY, sprite->mDrawOrder - 2,
                                   sprite->mLayer);
                    } else {
                        DrawSprite(id + flip, x + rec->mOffsetX, y + rec->mOffsetY, sprite->mDrawOrder - 2,
                                   sprite->mLayer);
                    }
                }
            }
        }

        {
            u32 len;
            start = 0;
            len = 0;
            i = 0;
            for (; i < gScriptWork.mTextCount; i++) {
                ScriptText *entry = &gScriptWork.mTexts[i];
                const u8 *s;
                u16 pos;
                if (gScriptWork.mUnk_280.mTextEscapeFunc != 0) {
                    s = name;
                    CopyFileString(gScriptWork.mTextFileId, entry->mTextId, name, 1);
                } else {
                    s = GetFileString(gScriptWork.mTextFileId, entry->mTextId);
                }
                pos = gSystemState.mTextCharLimit;
                if (pos >= start && pos <= start + entry->mCharCount) {
                    gScriptWork.mCharFrames++;
                    if (gScriptWork.mCharFrames > entry->mCharFrameCount && gScriptWork.mTextIncomplete == 1) {
                        gScriptWork.mUnk_280.mLsTimer = entry->mCharFrameCount + 1;
                        gSystemState.mTextCharLimit = pos + 1;
                        if (gScriptWork.mCharSoundId != 0) {
                            PlayWave(gScriptWork.mCharSoundId, 100, 0, 0x40, 0xffff, 0);
                        }
                        gScriptWork.mCharFrames = 0;
                    }
                }
                start += entry->mCharCount;
                while (*s != 0) {
                    text[len++] = *s++;
                }
                if (len >= 0x200) {
                    OS_Terminate();
                }
            }
            text[len] = 0;

            if (gScriptWork.mSkipToEnd == 0 && len != 0) {
                ScriptText *entry = gScriptWork.mTexts;
                u8 value = 0;
                u32 extra = 0;
                if (callback != 0) {
                    value = callback();
                }
                GetTexInfo(gScriptFonts[entry->mFontIndex]->mGlyph, &info, 0);
                if (info.mUnk_12 > 1) {
                    extra = TEXT_ROTATE;
                    saved = gSystemState.mUnk_30a[0];
                    gSystemState.mUnk_30a[0] = value;
                    func_0202a1cc();
                }
                DrawTextEx(entry->mX, entry->mY + gScriptWork.mTextOffsetY, gScriptFonts, (u8) entry->mFontIndex,
                           (const u8 *) text, 0,
                           extra + (entry->mAlign + (TEXT_LINE_WINDOW | TEXT_LIMIT | TEXT_HILITE) +
                                    (gScriptWork.mUnk_280.mTextEscapeFunc != 0 ? TEXT_ESCAPES : 0)),
                           entry->mLineWidth, entry->mLineHeight, entry->mLayer);
                if (extra & TEXT_ROTATE) {
                    gSystemState.mUnk_30a[0] = saved;
                }
                gScriptWork.mTextIncomplete = gSystemState.mTextIncomplete;
            }
        }

        if (gScriptWork.mState == SCRIPT_STATE_END) {
            return;
        }
        if (gScriptWork.mState == SCRIPT_STATE_WAIT) {
            if (gScriptWork.mSkipToEnd == 1 ||
                (gScriptWork.mSpriteCount == 0 && gScriptWork.mTextCount == 0 && gScriptWork.mWaitTimer == 0)) {
                StartEofFinish();
            } else {
                if (gScriptWork.mFastText == 1 && gScriptWork.mTextIncomplete == 1) {
                    gSystemState.mTextCharLimit += 2;
                }
                if (gScriptWork.mWaitTimer != 0 && --gScriptWork.mWaitTimer == 0) {
                    StartEofFinish();
                } else if (!(gScriptWork.mEofFlags & 8) &&
                           ((gSystemState.mKeysDown & 1) ||
                            (gTouchInput.mTrail.mTouchState.mButtonState & 0x4000ffff) == 0x40000400)) {
                    if (gScriptWork.mTextIncomplete == 1) {
                        if ((gSystemState.mKeysPressed & 1) ||
                            (gTouchInput.mTrail.mTouchState.mButtonState & 0xc000ffff) == 0xc0000400) {
                            gScriptWork.mUnk_1c |= 2;
                        }
                    } else if ((gSystemState.mKeysPressed & 1) ||
                               (gTouchInput.mTrail.mTouchState.mButtonState & 0xc000ffff) == 0xc0000400) {
                        if (gSystemState.mTextWindowLineIndex + gSystemState.mTextWindowLineCount <
                            gSystemState.mTextLineCount) {
                            gSystemState.mTextWindowLineIndex += gSystemState.mTextWindowLineCount;
                        } else {
                            StartEofFinish();
                        }
                    }
                }
            }
        } else if (gScriptWork.mState == SCRIPT_STATE_FINISH) {
            if (gScriptWork.mSkipToEnd == 1 || gScriptWork.mSpritesBusy == 0) {
                if (((gMapState.mCamera.mUnk_2dc.mUnk_0c != 0 && gMapState.mCamera.mUnk_2dc.mLookAtMode == 8 &&
                      (gSystemState.mFlags & 0x80)) ||
                     (gMapState.mCamera.mUnk_2dc.mUnk_0e != 0 && (gSystemState.mFlags & 0x80))) &&
                    !(gSystemState.mFlags & 0x08000000)) {
                    continue;
                }
                if (!(gScriptWork.mEofFlags & 1)) {
                    gScriptWork.mSpriteCount = 0;
                    gScriptWork.mUnk_280.mLsIndex = gScriptWork.mUnk_280.mLsSpriteIndex = -1;
                }
                if (gScriptWork.mEofFlags & 4) {
                    gScriptWork.mState = SCRIPT_STATE_END;
                    PlayScriptTrackAndSounds();
                } else {
                    gScriptWork.mState = SCRIPT_STATE_READ;
                }
            }
        } else {
            gScriptWork.mFastText = 0;
            do {
                int kind = GetFileValueType(gScriptWork.mScriptFileId, gScriptWork.mLineId);
                // kind is compared with 0 before the test for 4 (both compares are in the code).
                if (kind != 0 && kind == 4) {
                    line = (u8 *) GetFileString(gScriptWork.mScriptFileId, gScriptWork.mLineId);
                    switch (ReadScriptCommand(&line)) {
                        case SCRIPT_CMD_LET:
                            SetScriptRecordFromText((const char *) line);
                            break;
                        case SCRIPT_CMD_SPR:
                            AddScriptSprite(line, 0);
                            break;
                        case SCRIPT_CMD_SPRX:
                            AddScriptSprite(line, 0);
                            gScriptWork.mSprites[gScriptWork.mSpriteCount - 1].mDrawId |= 0x10000;
                            break;
                        case SCRIPT_CMD_SPRSCR:
                            SetScriptScrSprite(line, 1);
                            break;
                        case SCRIPT_CMD_SHPSCR:
                            SetScriptScrSprite(line, 0);
                            break;
                        case SCRIPT_CMD_SHAPE:
                            AddScriptSprite(line, 1);
                            break;
                        case SCRIPT_CMD_TXT:
                            AddScriptText(line);
                            gSystemState.mTextCharLimit = 0;
                            break;
                        case SCRIPT_CMD_TXTSPD:
                            ScriptCmdTxtSpd(line);
                            break;
                        case SCRIPT_CMD_EOF:
                            ScriptCmdEof(line);
                            gScriptWork.mState = SCRIPT_STATE_WAIT;
                            done = 1;
                            break;
                        case SCRIPT_CMD_ESC:
                            QueueScriptEvent(line);
                            break;
                        case SCRIPT_CMD_SYNC:
                            ScriptCmdSync((const char *) line);
                            break;
                        case SCRIPT_CMD_GOTO:
                            ScriptCmdGoto(line);
                            break;
                        case SCRIPT_CMD_SCREEN:
                            ScriptCmdScreen(line);
                            break;
                        case SCRIPT_CMD_IF:
                            ScriptCmdIf(line);
                            break;
                        case SCRIPT_CMD_EQ:
                            ScriptCmdEq(line);
                            break;
                        case SCRIPT_CMD_NE:
                            ScriptCmdNe(line);
                            break;
                        case SCRIPT_CMD_GT:
                            ScriptCmdGt(line);
                            break;
                        case SCRIPT_CMD_LT:
                            ScriptCmdLt(line);
                            break;
                        case SCRIPT_CMD_MUSIC:
                            ScriptCmdMusic(line);
                            break;
                        case SCRIPT_CMD_SOUND:
                            ScriptCmdSound(line);
                            break;
                        case SCRIPT_CMD_BRA:
                            ScriptCmdBra((const char *) line);
                            break;
                        case SCRIPT_CMD_FADE:
                            ScriptCmdFade(line);
                            break;
                        case SCRIPT_CMD_SETEYE:
                            ScriptCmdSetEye(line);
                            break;
                        case SCRIPT_CMD_SETLS:
                            ScriptCmdSetLs(line);
                            break;
                        case SCRIPT_CMD_SETLSC:
                            ScriptCmdSetLsc(line);
                            break;
                    }
                }
                gScriptWork.mLineId++;
            } while (done == 0);
        }
    } while (gScriptWork.mSkipToEnd);
}
