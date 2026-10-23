#pragma once

#include "types.h"
#include <Main/Scene/stage_data.h>

#ifdef __cplusplus
extern "C" {
#endif

// Sprite object embedded in each piece.
typedef struct Sprite {
    const u16 *mUnk_00;
    s32 mUnk_04;
    const u16 *mUnk_08;
    u32 mUnk_0c;
    u32 mUnk_10;
    u32 mUnk_14;
    const u16 *mUnk_18[32];
    u16 mUnk_98[32];
    u16 mUnk_d8;
    u16 mUnk_da;
    s16 mUnk_dc;
    u8 mUnk_de;
    u8 mUnk_df;
    u8 mUnk_e0;
    u8 mUnk_e1;
    u16 mUnk_e2[17];
} Sprite;

typedef struct Vec2i {
    s32 x;
    s32 y;
} Vec2i;

// One block on the board (columns 0..5, rows 0..9).
typedef struct Piece {
    u8 mType;
    Sprite mSprite;
    Vec2i mPos;
    // -1: not being removed (SetPieceType). 1: removal started (StartPieceRemoval). UpdateBoard sets 0 once the
    // sprite's mUnk_de is non-zero. On a pass where the sprite's mUnk_04 equals gBoard.mUnk_8974, UpdateBoard runs
    // RemoveMatchesAt instead of clearing the cell, so 0 does not always mean the cell was cleared.
    s16 mRemovalState;
    u8 mUnk_112[2];
} Piece;

// The falling pair of pieces.
typedef struct Pair {
    Piece mPiece[2];
    Vec2i mPos;
    u8 mDir;
    s32 mSplit;
    // Value of gSystemState.mElapsedFrames when the pair split.
    u32 mSplitFrame;
    s32 mSpeed;
    // -1 until the pair first lands; the pieces are placed when it counts down to 0.
    s16 mLandTimer;
    u8 mLandResetsRemaining;
} Pair;

#define BOARD_COLS 6
#define BOARD_ROWS 10
#define PIECE_LIST_SIZE 60

// Where the second piece of a pair sits from the first (Pair.mDir): above, right, below, left; turning clockwise
// steps through them in this order.
#define PAIR_DIR_UP 0
#define PAIR_DIR_RIGHT 1
#define PAIR_DIR_DOWN 2
#define PAIR_DIR_LEFT 3

typedef struct Board {
    u8 mTableRow;
    u8 mTypeCount;
    u8 mUnk_02[2];
    // The start value of mEndTimer, set with it in SetupFallingBlocks; never read.
    s32 mEndFrameCount;
    u32 mEndTimer;
    s32 mUnk_0c;
    u32 mScore;
    u32 mScoreThreshold;
    u8 mUnk_18;
    u8 mUnk_19[0x194 - 0x19];
    StageInfo mStage;
    u8 mUnk_1ba[0x24c - 0x1ba];
    Piece mGrid[BOARD_COLS][BOARD_ROWS];
    s32 mUnk_42fc;
    s32 mUnk_4300;
    s32 mUnk_4304;
    s32 mUnk_4308;
    u16 mRemovingCount;
    u8 mUnk_430e[0x4310 - 0x430e];
    Piece mList[PIECE_LIST_SIZE];
    u16 mCount;
    Pair mPair;
    Pair mNextPair;
    Sprite mPairMarker;
    u8 mPairState;
    u8 mUnk_8951[3];
    // Cells (x column, y row) where PlacePiece put the pair's first and second piece. Swapped when the pair splits
    // after placing its first piece.
    Vec2i mFirstPieceCell;
    Vec2i mSecondPieceCell;
    s32 mFallSpeed;
    s16 mUnk_8968;
    u16 mUnk_896a;
    u16 mUnk_896c;
    s16 mUnk_896e;
    u16 mUnk_8970;
    u16 mUnk_8972;
    s32 mUnk_8974;
    // Per piece type (index type - 1), set by SetupFallingBlocks. The types before type mTypeCount get a random share
    // of a random total, that type gets the rest, and the types after it get -1. StartPieceRemoval lowers it by 1
    // (not below 0) for types below 7. Nothing else reads it in this build.
    s32 mTypeRemovalsRemaining[6];
    s32 mPieceScore;
    s32 mRemovedCount;
} Board;

int GetFallingBlocksValue(int a, int b);
void CellToBoardPos(s8 col, s8 row, Vec2i *out);
void BoardPosToCell(const Vec2i *pos, s8 *col, s8 *row);
u16 func_020031f0(void);
void func_02003218(u8 *p);
u16 func_02003240(void);
void func_02003258(u8 *p);
u8 PickPieceType(u8 lo, u8 hi);
void SetPieceType(Piece *p, u8 type);
void DrawPiece(Piece *p, int x, int y, int layer, int flag);
void PlacePiece(const Piece *src, u8 col, u8 row, int unused, Vec2i *out);
void StartPieceRemoval(Piece *p);
BOOL DropPiece(Piece *p);
void ResetLandTimer(Pair *pr);
void SwapPairPieces(Pair *pr);
void GetPairSecondPos(const Pair *pr, Vec2i *out);
void LandPair(Pair *pr, Piece *unused);
void MovePairLeft(Pair *pr);
void MovePairRight(Pair *pr);
void TurnPairTo(Pair *pr, u8 dir);
void TurnPairClockwise(Pair *pr);
void TurnPairCounterclockwise(Pair *pr);
void PickNextPair(void);
void TakeNextPair(void);
void DrawPair(Pair *pr, int a, int b);
BOOL DropPair(Pair *pr);
void DropPairUntilLanded(Pair *pr);
BOOL IsBoardSettled(void);
void AddFallingPiece(const Piece *src);
BOOL IsCellEmpty(u8 col, u8 row);
void ClearCell(u8 col, u8 row);
void FillStartingBoard(int level);
void RemovePiecesOfType(u8 type);
void StartPieceFall(u8 col, u8 row);
BOOL PiecesMatch(u8 col, u8 row, u8 col2, u8 row2);
void RemoveMatchesAt(u8 col, u8 row);
void RemoveAllMatches(void);
void StartAllPieceFalls(void);

// The board, the falling pairs and the round's counters.
extern Board gBoard;

void DrawBoard(void);
void UpdateBoard(void);
void EndFallingBlocks(int a);
void SetupFallingBlocks(void);
void DrawFallingBlocks(void);
void UpdateFallingBlocks(void);
void LeaveFallingBlocks(void);
int GetFallingBlocksScoreThreshold(u8 tableRow);

#ifdef __cplusplus
}
#endif
