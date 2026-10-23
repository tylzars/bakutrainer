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

// One queued transfer (0x14 bytes).
typedef struct Transfer {
    // A file offset (packed with a block index for some files), or an address when mFile is 0.
    u32 mSrc;
    // A VRAM group offset is turned into an address before the copy.
    u32 mDst;
    // The stored size. Becomes mUnpackedSize once the data is unpacked; cut down when ProcessTransfers splits a
    // transfer.
    u32 mSize;
    u32 mUnpackedSize;
    u16 mFile;
    // Two format bytes.
    u8 mUnk_12;
    u8 mUnk_13;
} Transfer;

// VRAM banks mapped for one use. Per bank: control register, size in KB, LCDC address and the control value to
// restore.
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
    // Per key: 0x10 while held, then counts down after the release. Pressing the same key again within those 16 calls
    // starts a repeat (this is not auto-repeat). Only 12 entries, but key bit 13 is in the key mask: while it is held
    // the loop also uses entries 12 and 13, which are the two bytes of mKeysDown.
    u8 mKeyRepeatTimers[0xc];
    u16 mKeysDown;
    u16 mKeysPressed;
    u16 mKeysReleased;
    // A key counts only once it has read up since ResetKeys.
    u16 mKeysEnabled;
    // Keys held since a repeat press. Only UpdateKeys reads mKeysRepeatDown; nothing reads the other two.
    u16 mKeysRepeatDown;
    u16 mKeysRepeatPressed;
    u16 mKeysRepeatReleased;
    // Ring of bits 8-11 of each new press; mKeyHistoryIndex is the newest entry. Nothing reads it.
    u8 mKeyHistory[16];
    u8 mKeyHistoryIndex;
    u8 mUnk_02f;
    u8 *mHeapBoundary;
    s32 mLowHeap;
    s32 mHighHeap;
    Transfer *mTransfers;
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
    void (*mCommErrorFunc)(int);
    Unk_020c1b0c mUnk_250;
    u8 mUnk_27c[4];
    s32 mUnk_280;
    u16 *mUnk_284;
    // Bit i is set when entry i came in on the last exchange. ExchangeCommRecords writes it for the 16 comm slots,
    // UpdateSystem's other link exchange for its 2 entries; neither runs in this build. CountUnchangedSlotMask keeps
    // the previous mask and counts the calls that left it unchanged (up to 0x800); nothing else reads the count.
    u16 mLinkSlotMask;
    u16 mPrevLinkSlotMask;
    u16 mUnchangedLinkSlotMaskCount;
    u16 mUnk_28e;
    u16 mUnk_290;
    u16 mUnk_292;
    u16 mUnk_294;
    u16 mUnk_296;
    u8 *mUnk_298;
    u8 mUnk_29c[0x2a2 - 0x29c];
    // Connection tries of the comm link; UpdateCommChild enters the error case after 5.
    u8 mConnectAttemptCount;
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
    // Always 1 in this build: start-up and OpenPromptScreen set it, ClosePromptScreen restores the saved copy. So the
    // touch is never read while mTouchReleaseTimer runs.
    u8 mUseTouchReleaseTimer : 1;
    u8 mUnk_2ac_6 : 2;
    // This frame's touch, 0 or 1, after the release timer and mTouchEnabled.
    u8 mTouchSample;
    // mTouchX and mTouchY: the last valid position, kept after the release.
    u8 mTouchX;
    u8 mTouchY;
    // Set to 8 on the release; the touch reads as up while it runs. It counts down only when the panel reports no
    // touch or a sample with validity 0.
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
    // With TEXT_BOUNDS, DrawTextEx stores the bounds of the glyphs it draws, in screen coordinates (after the
    // vertical-text swap and the rotation). Nothing reads them in this build.
    s16 mTextBoundsMinX;
    s16 mTextBoundsMinY;
    s16 mTextBoundsMaxX;
    s16 mTextBoundsMaxY;
    u32 (*mTextEscapeFunc)(u32);
    u16 mUnk_308;
    u8 mUnk_30a[2];
    u16 mTextCharLimit;
    // Replaces a character no font in the chain holds. Only start-up writes it (0x20), so DrawTextEx's branch for any
    // other replacement never runs in this build.
    u16 mTextReplacementChar;
    // Ends a line in DrawTextEx and is not drawn: 10 from start-up, 0x40 once main has set it.
    u8 mTextLineBreakChar;
    u8 mUnk_311[2];
    // With TEXT_LINE_WINDOW, DrawTextEx draws (and advances the line position for) only mTextWindowLineCount lines,
    // starting at line mTextWindowLineIndex (0-based). The script code sets both.
    u8 mTextWindowLineIndex;
    u8 mTextWindowLineCount;
    // Lines laid out by the last DrawTextEx call; only the script code reads it.
    u8 mTextLineCount;
    u8 mUnk_316;
    // Cleared by each DrawTextEx call and set when TEXT_LIMIT holds a character back.
    u8 mTextIncomplete : 1;
    u8 mUnk_317_1 : 7;
    // While it is 1, ReadTextChar decodes a $<number> escape through mTextEscapeFunc. Each DrawTextEx call copies
    // TEXT_ESCAPES into it; CopyFileString sets it for its copy and restores it.
    u8 mUseTextEscapes;
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
    // Only main sets it (to 1). While it is 0 PlayTrack and SetGlobalVolume force the tracker volume to 0; the song
    // still plays.
    u8 mUseTrackerVolume;
    u8 mUseWaves;
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
u16 GetTexFileKeyCount(void);
u16 GetPlttFileKeyCount(void);
u32 func_0201c550(void);
u32 GetPaletteFileKeyEnd(void);

// Objects of the per-frame system code.

// Set in mUnk_0 (the run length) of a block heap entry while the run is allocated.
#define POOL_BLOCK_USED 0x8000

// An entry of a block heap (8 bytes). There is one per 512-byte (or 32-byte) VRAM block of a VRAM cache, or per unit
// of a texture layer, plus two sentinels. At the first entry of a run: mUnk_0 is the run's length (plus
// POOL_BLOCK_USED while it is allocated), mPrev/mNext link the free runs in order of size, and mUnk_6 is the start of
// the previous run in memory.
// A texture layer's cache list (TextureLayer.mNodes) uses the same type: mPrev/mNext are the list links, mUnk_0 the
// entry's hash slot and mUnk_6 the first unit of its run.
typedef struct Unk_PoolRange {
    u16 mUnk_0;
    u16 mPrev;
    u16 mNext;
    u16 mUnk_6;
} Unk_PoolRange;

// An entry of a layer's palette slot list (mUnk_534): a palette key (0xffff = unused) and the palette index it is
// loaded into.
typedef struct Unk_SlotPair {
    u16 mKey;
    u16 mPaletteIndex;
} Unk_SlotPair;

// A slot of a layer's hash table (16 bytes).
typedef struct TextureSlot {
    // The block's sixteen 4-bit palette positions (mode 0).
    u32 mUnk_0[2];
    // -1 = never used, -2 = removed.
    s32 mKey;
    // The cache list entry that holds the block.
    u16 mNodeIndex;
    // Search paths that pass through the slot.
    u16 mPathCount;
} TextureSlot;

// An entry of a layer's chain table (8 bytes).
typedef struct PaletteChainEntry {
    // Held by the first entry of a key's chain; 0xffff in the later entries, 0xfffe = free.
    u16 mKey;
    // The chain links; mChainPrev is written but never read in this build.
    u8 mChainPrev;
    u8 mChainNext;
    // Links of the list func_0202c5fc searches from the front for an entry of a slot; it moves the entry it finds to
    // the end. Entry 255 is the anchor.
    u8 mPrev;
    u8 mNext;
    // The palette position: high nibble the slot, low nibble the index in it.
    u8 mUnk_6;
    u8 mUnk_7;
} PaletteChainEntry;

// The tables of one texture layer (0x580 bytes), one per display engine: gTextureBlocks.mLayers[layer - 1].
typedef struct TextureLayer {
    // Queued entries, written to OAM by WriteOam.
    void *mOam;
    // The cache list and hash table of the blocks loaded into VRAM.
    Unk_PoolRange *mNodes;
    TextureSlot *mSlots;
    // The block heap: one entry per VRAM unit of 32 << mUnitShift bytes, plus two sentinels.
    Unk_PoolRange *mBlocks;
    void *mMats;
    // Per queued mode-0 transfer, the start of its run in mUnk_115; mUnk_114 is the next index.
    u16 mUnk_014[0x80];
    u8 mUnk_114;
    // The list of palette positions; mUnk_516 is the next index.
    u8 mUnk_115[0x516 - 0x115];
    u16 mUnk_516;
    // The chain table, its free entry count per slot and each key's first entry.
    PaletteChainEntry *mUnk_518;
    u8 mUnk_51c[0x10];
    u8 *mUnk_52c;
    // Sort keys of the queued entries.
    void *mSortKeys;
    // The palette slot list.
    Unk_SlotPair mUnk_534[0x10];
    // Used entries of mNodes.
    u16 mEntryCount;
    u8 mMatCount;
    // Also the value of the display control register's field at bit 20.
    u8 mUnitShift;
    // Units in the block heap (mBlocks has mUnitCount + 2 entries).
    u16 mUnitCount;
    u8 mOamCount;
    // The chain-table flag.
    u8 mUnk_57b;
    // Set when a hash search takes more than 20 steps; UpdateSystem then resets the layer.
    u8 mUnk_57c;
    u8 mUnk_57d[3];
} TextureLayer;

// Texture blocks. The first members hold a preloaded block: a valid flag, its address and its offset.
typedef struct TextureBlocks {
    u32 mUnk_000;
    void *mUnk_004;
    u8 *mUnk_008;
    u32 mUnk_00c;
    u16 mUnk_010;
    u16 mUnk_012;
    // The palette file's size in 32-byte palettes.
    u16 mPaletteCount;
    u8 mUnk_016;
    u8 mUnk_017;
    TextureLayer mLayers[2];
    u8 mUnk_b18[4];
    // File offsets of a PACK block file.
    u32 *mPackOffsets;
    // Loaded from a file of its own, not from mBlockFile.
    u32 *mBlockTable;
    u16 mPaletteFile;
    u16 mBlockFile;
    s16 mUnk_b28;
    // The first block is taken to follow the table's offset list.
    u16 mBlockCount;
    s8 mUnk_b2c;
    u8 mUnk_b2d;
    // 1 PACK, 2 EZ10, 0 other.
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
void ProcessTransfers(int flags);

// Bank configuration records (12 bytes each): the bank, its size in KB, the use it is mapped for, its address, then
// the MST and OFS values of the control register. Use 0 is the bank's LCDC record, which leads each bank's run. For
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

// Node of a VRAM cache (16 bytes). Node 0 is the list's end and node 1 its front, so node 1's mNext is the least
// recently used entry. Node 1's mPrev also heads the chain of free nodes.
typedef struct VramCacheNode {
    // Byte offset of the entry's blocks from the start of the VRAM group.
    u32 mOffset;
    // In bytes; written for the palette cache only, never read in this build.
    u32 mSize;
    u16 mSlotIndex;
    u16 mPrev;
    u16 mNext;
    u8 mUnk_0e[2];
} VramCacheNode;

// Slot of a VRAM cache's hash table (8 bytes): the key (-1 = never used, -2 = removed; with no path count a removed
// slot never becomes -1 again) and the node that holds the entry.
typedef struct VramCacheSlot {
    s32 mKey;
    u16 mNodeIndex;
    u16 mUnk_06;
} VramCacheSlot;

// An animation track.
typedef struct AnimTrack {
    void *mData;
    u32 mFrameIndex;
    u32 mNextFrameIndex;
    // Out of 0x100.
    u8 mNextWeight;
    // A copy of the animation's flags. Its only reader (bits 0-1, in ApplyAnimTrack's combining path) does not run
    // in this build.
    u8 mFlags;
    u8 mUnk_0e[2];
} AnimTrack;

// A node of a model's pose tree: child and sibling links (0xff = none), scale (s8), angles (s8, << 8), position
// (s16, << 5), its matrix stack slot (-1 = none) and flags.
typedef struct PoseNode {
    u8 mFirstChild;
    u8 mNextSibling;
    s8 mScaleX;
    s8 mScaleY;
    s8 mScaleZ;
    s8 mAngleX;
    s8 mAngleY;
    s8 mAngleZ;
    s16 mPosX;
    s16 mPosY;
    s16 mPosZ;
    s8 mMtxStoreIndex;
    u8 mUnk_f;
} PoseNode;

// The two VRAM caches (texture and palette) and the state of the model draw and pose walk (mAnimTracks to mUnk_4b).
// Each cache has nodes, hash-table slots and a heap of 512-byte (or 32-byte) VRAM blocks (not the block file).
typedef struct GxState {
    VramCacheNode *mTexNodes;
    VramCacheSlot *mTexSlots;
    Unk_PoolRange *mTexBlocks;
    VramCacheNode *mPlttNodes;
    VramCacheSlot *mPlttSlots;
    Unk_PoolRange *mPlttBlocks;
    AnimTrack mAnimTracks[2];
    PoseNode *mPoseNodes;
    // The map entry DrawModelFileEx moves a model to while its mUnk_172_0 is set. With 0xffff it instead moves the
    // model so that node mNodeIndex lands on mNodePos. MC_SET_3C writes it and MC_SET_3E sets 0xffff; no model script
    // in this build uses either.
    u16 mPlaceEntryId;
    // The map entry StartModelAnim moves a model towards after MC_SET_3E, which writes it.
    u16 mMoveEntryId;
    // 0x100000 / the mode-2 draw scale (fx32): 1 / scale in units of 1/256.
    s16 mInverseScale;
    // ComputePoseMatrices multiplies this node's translation by mInverseScale / 256. That cancels the mode-2 draw
    // scale for the offset only; the node's rotation and scale, and its children, still get it. DrawModelFileEx sets
    // it to 0xff (none), or, in mode 2 with a scale other than 0x1000 and a DrawParams mUnk_42 other than -1, to that
    // node.
    u8 mInverseScaleNodeIndex;
    u8 mUnk_43;
    // The shift byte of the last model drawn in a mode other than 1, 3 and 5: positions read from the tracks are
    // shifted right by it, and GetRelativeScale scales relative to it.
    u8 mModelShift;
    u8 mUnk_45;
    // ComputePoseMatrices stops at this node once it has stored its matrix, skipping its children and later
    // siblings, and resets it to 0xff. While mUnk_45 is set, track 1's positions are blended in only for nodes
    // numbered above it.
    u8 mStopNodeIndex;
    u8 mUnk_47;
    u8 mTrack1Blend;
    // Goes up each time a track is filled with data; the same track can count twice, and clearing a track does not
    // lower it. While it is 0 the pose walk uses the nodes' own values.
    u8 mTrackFillCount;
    // The highest node number ComputePoseMatrices has reached since DrawModelFileEx zeroed it; nothing reads it.
    u8 mHighestNodeIndex;
    u8 mUnk_4b;
    // Counted by FindOrLoadTex; never read in this build.
    u32 mTexLookupCount;
    u32 mTexProbeCount;
    u16 mTexFile;
    u16 mPlttFile;
    // The file's stored size less the offset in its header word 1, divided by 4; the keys of the other sources are
    // counted from here.
    u16 mTexFileKeyCount;
    u16 mPlttFileKeyCount;
    u8 mUnk_5c[2];
    u16 mPlttEntryCount;
    u16 mTexEntryCount;
    u16 mTexBlockCount;
    u16 mPlttBlockCount;
    u8 mUnk_66[2];
} GxState;

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
