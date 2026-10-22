#pragma once

#include "types.h"
#include <nitro/wm.h>
#include <nitro/fs.h>
#include <nitro/pm.h>
#include <nitro/mic.h>
#include <nitro/fx.h>

#ifdef __cplusplus
extern "C" {
#endif

// The system state block and its parts. Only the members the code touches are named.
// Block cleared together at start-up: 0x24 bytes and the object at member 0x24.
typedef struct Unk_020c1b0c {
    s32 mUnk_00;
    u32 mUnk_04;
    s32 mUnk_08;
    void *mUnk_0c;
    u8 mUnk_10[4];
    u32 mUnk_14[2];
    u32 mUnk_1c[2];
    u8 *mUnk_24;
    u8 mUnk_28[4];
} Unk_020c1b0c;

// One queued transfer (0x14 bytes): the source (a file offset, packed with a block index for some files, or an
// address when the file is 0), the destination, the stored and the unpacked size, the file and two format bytes.
typedef struct Unk_LoadEntry {
    u32 mUnk_00;
    u32 mUnk_04;
    u32 mUnk_08;
    u32 mUnk_0c;
    u16 mUnk_10;
    u8 mUnk_12;
    u8 mUnk_13;
} Unk_LoadEntry;

// VRAM banks mapped for one use: each bank's control register, its size in KB, its LCDC address and the control
// value to restore, then the number of banks.
typedef struct VramGroup {
    vu8 *mCtrl[4];
    // Set only for mTexVramGroup.
    u16 mSizeKb[4];
    u32 mAddr[4];
    u8 mCtrlValue[4];
    // Total size of the group's banks, in 16 KB units.
    u16 mSize16Kb;
    u8 mBankCount;
    u8 mUnk_2f;
} VramGroup;

// One VRAM bank: its control register, its LCDC address and the control value to restore.
typedef struct VramBank {
    vu8 *mCtrl;
    u32 mAddr;
    u8 mCtrlValue;
    u8 mUnk_9[3];
} VramBank;

typedef struct SystemState {
    void *mFileTable;
    // Per key: 0x10 while held, then counting down after the release. A press while it is still counting (the same
    // key pressed again within 16 calls of its release; not auto-repeat) starts a repeat. Only 12 entries, but key bit
    // 13 is in the key mask: while it is held the loop also uses entries 12 and 13, which are the two bytes of
    // mKeysDown.
    u8 mKeyRepeatTimers[0xc];
    u16 mKeysDown;
    u16 mKeysPressed;
    u16 mKeysReleased;
    // A key counts only once it has read up since ResetKeys.
    u16 mKeysEnabled;
    // Keys held since a repeat press; only UpdateKeys reads mKeysRepeatDown, and nothing reads mKeysRepeatPressed or
    // mKeysRepeatReleased.
    u16 mKeysRepeatDown;
    u16 mKeysRepeatPressed;
    u16 mKeysRepeatReleased;
    // Bits 8-11 of each new press, a ring with mKeyHistoryIndex at the newest entry; nothing reads it.
    u8 mKeyHistory[16];
    u8 mKeyHistoryIndex;
    u8 mUnk_02f;
    u8 *mHeapBoundary;
    s32 mLowHeap;
    s32 mHighHeap;
    Unk_LoadEntry *mTransfers;
    VramGroup mPlttVramGroup;
    VramGroup mTexVramGroup;
    VramBank mUnk_0a0;
    VramBank mUnk_0ac;
    // Per bank, its gVramBankRecords index; 0xff = not mapped.
    u8 mVramBankRecordIndex[9];
    u8 mVramBanksChanged;
    u8 mUnk_0c2[2];
    // The allocation (mMicBufferSize + 0x1f bytes); mMicParam.buffer is its 32-byte aligned address.
    void *mMicBuffer;
    u8 *mMicSamples;
    // Bytes of the capture ring, not of the mMicBuffer allocation.
    u16 mMicBufferSize;
    u8 mUnk_0ce[2];
    MICAutoParam mMicParam;
    u16 mMicBandHistory[16][8];
    // The last SumMicBands call's sums, also stored in mMicBandHistory.
    u32 mMicBandSums[8];
    u32 mMicBandHistoryTotals[8];
    // log2 of the number of samples transformed.
    u8 mMicSpectrumShift;
    u8 mUnk_22d[3];
    void (*mUnk_230)(u16);
    s32 mUnk_234;
    s32 mUnk_238;
    s32 mUnk_23c;
    void (*mUnk_240)(void);
    s32 mUnk_244;
    s32 mUnk_248;
    void (*mUnk_24c)(int);
    Unk_020c1b0c mUnk_250;
    u8 mUnk_27c[4];
    s32 mUnk_280;
    u16 *mUnk_284;
    u16 mUnk_288;
    u16 mUnk_28a;
    u16 mUnk_28c;
    u16 mUnk_28e;
    u16 mUnk_290;
    u16 mUnk_292;
    u16 mUnk_294;
    u16 mUnk_296;
    u8 *mUnk_298;
    u8 mUnk_29c[0x2a2 - 0x29c];
    u8 mUnk_2a2;
    u8 mUnk_2a3;
    s8 mUnk_2a4[2];
    u8 mUnk_2a6_0 : 1;
    u8 mUnk_2a6_1 : 1;
    u8 mUnk_2a6_2 : 6;
    u8 mUnk_2a7;
    s32 mUnk_2a8;
    u8 mTouchPressed : 1;
    u8 mTouchDown : 1;
    u8 mTouchReleased : 1;
    u8 mUnk_2ac_3 : 1;
    // Set once the screen reads up; the touch counts only while it is 1.
    u8 mTouchEnabled : 1;
    // Always 1 in this build (start-up, OpenPromptScreen and ClosePromptScreen's restore of the saved copy), so the
    // touch is never read while mTouchReleaseTimer runs.
    u8 mUseTouchReleaseTimer : 1;
    u8 mUnk_2ac_6 : 2;
    // This frame's touch, 0 or 1, after the release timer and mTouchEnabled.
    u8 mTouchSample;
    // mTouchX and mTouchY: the last valid position, kept after the release.
    u8 mTouchX;
    u8 mTouchY;
    // Set to 8 on the release; the touch reads as up while it runs. It counts down only on passes where the panel
    // reports no touch or a sample with validity 0.
    u8 mTouchReleaseTimer;
    u8 mTouchMaxX;
    u8 mTouchMaxY;
    u8 mTouchMinX;
    u8 mTouchMinY;
    u8 mUnk_2b5[0x2b8 - 0x2b5];
    s32 mUnk_2b8;
    s32 mUnk_2bc;
    s32 mUnk_2c0;
    s32 mUnk_2c4;
    s32 mUnk_2c8;
    // Written by LoadImageData; nothing reads mImageWidth or mImageHeight.
    u16 mImageWidth;
    u16 mImageHeight;
    u16 mUnk_2d0;
    u8 mUnk_2d2[2];
    s32 mBg3X;
    s32 mBg3Y;
    s32 mSubBg3X;
    s32 mSubBg3Y;
    u16 mBg3Pa;
    u16 mBg3Pd;
    // Copied to the window registers by HandleFrameIrq. Nothing writes mWin0H to mWin1V or mSubWin0H to mSubWin1V,
    // so they are 0 in this build.
    u16 mWin0H;
    u16 mWin0V;
    u16 mWin1H;
    u16 mWin1V;
    // Only the prompt screen writes mWindowBits and mSubWindowBits (0 while it is open, then the saved value), so they
    // are 0 in this build.
    u8 mWindowBits;
    u8 mUnk_2f1;
    u16 mSubWin0H;
    u16 mSubWin0V;
    u16 mSubWin1H;
    u16 mSubWin1V;
    u8 mSubWindowBits;
    u8 mUnk_2fb;
    s16 mUnk_2fc;
    s16 mUnk_2fe;
    s16 mUnk_300;
    s16 mUnk_302;
    u32 (*mUnk_304)(u32);
    u16 mUnk_308;
    u8 mUnk_30a[2];
    u16 mUnk_30c;
    u16 mUnk_30e;
    u8 mUnk_310;
    u8 mUnk_311[2];
    u8 mUnk_313;
    u8 mUnk_314;
    u8 mUnk_315;
    u8 mUnk_316;
    u8 mUnk_317_0 : 1;
    u8 mUnk_317_1 : 7;
    u8 mUnk_318;
    u8 mUnk_319[3];
    void (*mSaveCallback)(u32);
    u8 *mUnk_320;
    s32 mUnk_324;
    // The whole store (0x200 or 0x2000 bytes), split evenly into the set's blocks.
    u16 mSaveStoreSize;
    // Entry of the first field in the set's data file; each field's entry holds its largest value.
    u16 mSaveFirstField;
    // Entry of the last field. The entries after it hold the block count (+1), the tag table (+2) and the status
    // messages (+4 to +10, plus language * 10).
    u16 mSaveLastField;
    u8 mUnk_32e;
    u8 mUnk_32f;
    u16 mBackupLockId;
    u8 mUnk_332[2];
    s32 mFileTableSize;
    u32 mLoadedFileSize;
    u16 mCacheHitSlotIndex;
    u8 mUnk_33e[2];
    u16 mUnk_340[4];
    u8 mUnk_348[4];
    u8 *mUnk_34c;
    u32 mUnk_350;
    u16 mUnk_354;
    u8 mUnk_356[0x390 - 0x356];
    u32 mUnk_390[(0x42c - 0x390) / 4];
    s32 mUnk_42c;
    u8 mUnk_430[0x450 - 0x430];
    u32 mUnk_450;
    u16 mUnk_454;
    // UpdateSystem's lid-closed handling is held off while it is not 0.
    u8 mBackupTimer;
    u8 mUnk_457;
    // Bit per block WriteZeroSaveBlock wrote; only entry 0 is used and nothing reads it.
    u8 mClearedSaveBlocks[4];
    // Counted up by HandleFrameIrq, not by UpdateSystem.
    vu32 mElapsedFrames;
    u32 mUnk_460;
    void (*mUnk_464)(void *data, u32 size);
    // Called by HandleFrameIrq when set; only ever set to 0 in this build.
    void (*mFrameIrqCallback)(void);
    u32 mFlags;
    s32 mUnk_470;
    u8 mUnk_474[4];
    u32 mRandomSeed;
    // QueueTransferEx runs the queue first once it holds this many KB, or when the new entry would take it 0x40 KB
    // past that.
    u16 mTransferSizeLimitKb;
    // Maximum number of queued transfers.
    u16 mTransferLimit;
    u16 mTransferCount;
    u8 mUnk_482[2];
    // Unpacked bytes queued since the last ClearTransferQueue; dropping entries does not lower it.
    s32 mTransferSize;
    u16 mFrameStartLine;
    u16 mTransferBytesPerLine;
    s8 mUnk_48c;
    s8 mUnk_48d;
    u8 mUnk_48e;
    u8 mUnk_48f;
    u8 mFadeState[2];
    u8 mUnk_492;
    u8 mUnk_493;
    u8 mUnk_494;
    u8 mUnk_495;
    s16 mUnk_496;
    s32 mUnk_498;
    u16 mImageIds[2];
    s32 mUnk_4a0;
} SystemState;

extern SystemState gSystemState;
// A file of the start-up code; comm_link.c also reads through it.
extern FSFile gBankFile;

void InitSystem(u32 a);
void InitSystemEx(u32 a, int b, int c);
void UpdateSystemAndWait(void);
void UpdateSystem(int flags);
void func_0201c1d0(void *arg);
void HandleFrameIrq(void);
void UpdateKeys(void);
void ResetKeys(void);
void *Alloc(u32 size);
void *AllocWithFlags(u32 size, u32 flags);
int Free(void *p);
u16 func_0201c538(void);
u16 func_0201c544(void);
u32 func_0201c550(void);
u32 func_0201c564(void);

// Objects of the per-frame system code.

typedef struct Unk_PoolRange {
    u16 mUnk_0;
    u16 mUnk_2;
    u16 mUnk_4;
    u16 mUnk_6;
} Unk_PoolRange;

// A slot of the second table of a half: a link to its entry and a value.
typedef struct Unk_SlotPair {
    u16 mUnk_0;
    u16 mUnk_2;
} Unk_SlotPair;

// Entry of a half's hash table (16 bytes): the key at +8 (-1 = never used, -2 = removed) and the number of
// search paths that pass through the entry.
typedef struct Unk_HashEntry {
    u32 mUnk_0[2];
    s32 mUnk_8;
    u16 mUnk_c;
    u16 mUnk_e;
} Unk_HashEntry;

// Entry of the chain table of a half (8 bytes): a word and five link bytes.
typedef struct Unk_ChainEntry {
    u16 mUnk_0;
    u8 mUnk_2;
    u8 mUnk_3;
    u8 mUnk_4;
    u8 mUnk_5;
    u8 mUnk_6;
    u8 mUnk_7;
} Unk_ChainEntry;

// The two halves of the texture blocks (0x580 bytes each): tables for one display engine's cached blocks.
typedef struct Unk_020c2694 {
    void *mUnk_000;
    Unk_PoolRange *mUnk_004;
    Unk_HashEntry *mUnk_008;
    Unk_PoolRange *mUnk_00c;
    void *mUnk_010;
    u16 mUnk_014[0x80];
    u8 mUnk_114;
    u8 mUnk_115[0x516 - 0x115];
    u16 mUnk_516;
    Unk_ChainEntry *mUnk_518;
    u8 mUnk_51c[0x10];
    u8 *mUnk_52c;
    void *mUnk_530;
    Unk_SlotPair mUnk_534[0x10];
    u16 mUnk_574;
    u8 mUnk_576;
    u8 mUnk_577;
    u16 mUnk_578;
    u8 mUnk_57a;
    u8 mUnk_57b;
    u8 mUnk_57c;
    u8 mUnk_57d[3];
} Unk_020c2694;

// Texture blocks: a preloaded block (valid flag, its address and its offset), the palette count (the palette file's
// size in 32-byte palettes), the tables of each layer (mLayers[layer - 1]), the file offsets of a PACK block file, the
// block table (loaded from a file of its own, not from mBlockFile), the palette file, the block file, the block count
// (the first block is taken to follow the table's offset list) and the kind of the block file (1 PACK, 2 EZ10,
// 0 other).
typedef struct TextureBlocks {
    u32 mUnk_000;
    void *mUnk_004;
    u8 *mUnk_008;
    u32 mUnk_00c;
    u16 mUnk_010;
    u16 mUnk_012;
    u16 mPaletteCount;
    u8 mUnk_016;
    u8 mUnk_017;
    Unk_020c2694 mLayers[2];
    u8 mUnk_b18[4];
    u32 *mPackOffsets;
    u32 *mBlockTable;
    u16 mPaletteFile;
    u16 mBlockFile;
    s16 mUnk_b28;
    u16 mBlockCount;
    s8 mUnk_b2c;
    u8 mUnk_b2d;
    u8 mBlockFileKind;
} TextureBlocks;
extern TextureBlocks gTextureBlocks;

extern u16 data_020c9e04;
extern void *data_020c9e10;
extern u16 *data_020c9e14;
extern u8 *data_020c9e1c;
extern WMDataSharing data_020c9e60;
void QueueTransferEx(u32 file, u32 src, u32 dst, u32 size, u32 unpackedSize, u8 mode, u8 flag);
void QueueTransfer(u16 file, u32 src, u32 dst, u32 size);
void ClearTransferQueue(void);
u32 LimitTransferSize(u32 size);
void func_0201c78c(void);
void ProcessTransfers(int a);

// Bank configuration records (12 bytes each): the bank, its size in KB, the use it is mapped for (0 = the bank's
// LCDC record, which leads each bank's run), its address, then the MST and OFS values of the control register. For
// uses 5, 9, 10, 11 and 12 the address is an offset from 0, not a VRAM address. A size of 0 ends the table.
typedef struct VramBankRecord {
    u8 mBank;
    u8 mSizeKb;
    u8 mUse;
    u8 mUnk_3;
    u32 mAddr;
    u8 mUnk_8;
    u8 mUnk_9;
    u8 mUnk_a[2];
} VramBankRecord;

// Load statistics: bytes read and blocks unpacked by the transfer code.
typedef struct Unk_020c1834 {
    u8 mUnk_00[8];
    u32 mUnk_08;
    u32 mUnk_0c;
    u8 mUnk_10[0x30];
} Unk_020c1834;

typedef struct Unk_PoolNode {
    u32 mUnk_00;
    u32 mUnk_04;
    u16 mUnk_08;
    u16 mUnk_0a;
    u16 mUnk_0c;
    u8 mUnk_0e[2];
} Unk_PoolNode;

typedef struct Unk_PoolSlot {
    s32 mUnk_00;
    u16 mUnk_04;
    u16 mUnk_06;
} Unk_PoolSlot;

// An animation track: its data, the current and next frame, the blend between them and the mode.
typedef struct Unk_AnimTrack {
    void *mUnk_00;
    u32 mUnk_04;
    u32 mUnk_08;
    u8 mUnk_0c;
    u8 mUnk_0d;
    u8 mUnk_0e[2];
} Unk_AnimTrack;

// A node of a model's pose tree: mUnk_0 is its first child and mUnk_1 its next sibling (0xff = none); scale (s8),
// angles (s8, << 8), position (s16, << 5), its matrix stack slot (-1 = none) and flags.
typedef struct Unk_PoseNode {
    u8 mUnk_0;
    u8 mUnk_1;
    s8 mUnk_2;
    s8 mUnk_3;
    s8 mUnk_4;
    s8 mUnk_5;
    s8 mUnk_6;
    s8 mUnk_7;
    s16 mUnk_8;
    s16 mUnk_a;
    s16 mUnk_c;
    s8 mUnk_e;
    u8 mUnk_f;
} Unk_PoseNode;

// The graphics pools (texture blocks at 0x00, palette blocks at 0x0c: slots, hash entries and block ranges) and the
// model animation state.
typedef struct Unk_020c31dc {
    Unk_PoolNode *mUnk_00;
    Unk_PoolSlot *mUnk_04;
    Unk_PoolRange *mUnk_08;
    Unk_PoolNode *mUnk_0c;
    Unk_PoolSlot *mUnk_10;
    Unk_PoolRange *mUnk_14;
    Unk_AnimTrack mUnk_18[2];
    Unk_PoseNode *mUnk_38;
    u16 mUnk_3c;
    u16 mUnk_3e;
    s16 mUnk_40;
    u8 mUnk_42;
    u8 mUnk_43;
    u8 mUnk_44;
    u8 mUnk_45;
    u8 mUnk_46;
    u8 mUnk_47;
    u8 mUnk_48;
    u8 mUnk_49;
    u8 mUnk_4a;
    u8 mUnk_4b;
    u32 mUnk_4c;
    u32 mUnk_50;
    u16 mUnk_54;
    u16 mUnk_56;
    u16 mUnk_58;
    u16 mUnk_5a;
    u8 mUnk_5c[2];
    u16 mUnk_5e;
    u16 mUnk_60;
    u16 mUnk_62;
    u16 mUnk_64;
    u8 mUnk_66[2];
} Unk_020c31dc;

extern Unk_020c1834 data_020c1834;

// Word addresses of the bank control registers, each shared by up to four banks.
extern const u32 gVramCtrlAddrs[9];
extern const u8 gVramCtrlBitOffsets[9];
extern const VramBankRecord gVramBankRecords[];

// Hash steps of the graphics pools and the file cache: 1 and the odd primes up to 719.
extern const u16 gHashSteps[128];
#ifdef __cplusplus
}
#endif
