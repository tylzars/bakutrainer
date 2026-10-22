#pragma once

#include "types.h"
#include <Main/Minigame/falling_blocks.h>

#ifdef __cplusplus
extern "C" {
#endif

// A three-in-a-row game on a board of 3 columns by 4 rows. Each row is stored in four bytes (the fourth is a border
// byte), a board is 0x14 bytes (four rows and a border row), and the game keeps one board per placed piece.
// The low nibble of a cell is its state, the high nibble the piece it belongs to.
#define GRID_BYTES 0x14
#define GRID_COLS 3
#define GRID_ROWS 4
#define GRID_PIECES 6
#define SHAPE_COUNT 12
#define SHAPE_CELLS 9

#define CELL_EMPTY 0
#define CELL_FREE 1
#define CELL_HUMAN 2
#define CELL_COMPUTER 3

#define PIECE_GONE 99
#define SCORE_WIN 1000
#define SCORE_LOSS_PIECE 100
#define SCORE_NONE (-9999)

// One piece of the board: where it is drawn (pixels), which of the 12 shapes it has (PIECE_GONE once it is removed)
// and how many of its cells are still free.
typedef struct ShapeRec {
    s16 mX;
    s16 mY;
    s16 mShapeIndex;
    s16 mFreeCellCount;
} ShapeRec;

typedef struct ThreeInRow {
    // Deepest depth FindBestMove recurses to; always 4 in this build.
    u8 mSearchDepth;
    // Low 4 bits: the board cell (x + y * 4) of the move. 0x40: waiting for the next move, until the computer's search
    // or a touch replaces the whole value with the chosen cell; 0x80: the mark for that cell has started.
    u8 mMoveCell;
    u8 mMoveTimer;
    u8 mEmptyCount;
    // The player of each move after which CheckAndClearLineOrFilledShape cleared cells on this board (drawn as "H" or
    // "O"), and how many there are.
    u8 mClearPlayers[8];
    u8 mClearCount;
    u8 mUnk_0d;
    ShapeRec mShapes[GRID_PIECES];
    u8 *mBoards;
    s32 mShapeCount;
    // Its parity picks the player; it also advances with each new board, so the starting player alternates.
    u32 mTurn;
    Sprite mMarkSprite;
} ThreeInRow;

extern u8 gShapeCellCounts[SHAPE_COUNT];
extern ThreeInRow gThreeInRow;

int PlaceRandomShapes(u8 *boards, ShapeRec *recs);
int CheckAndClearLineOrFilledShape(int count, ShapeRec *recs, u8 *board, int player);
int EvaluateMove(u8 *board, int player, ShapeRec *recs, int x, int y);
int FindBestMove(ShapeRec *recs, u8 *board, int player, u32 depth);
void StartThreeInRowBoard(void);
void UpdateThreeInRowBoard(void);
void SetupThreeInRow(void);

void UpdateThreeInRow(void);
void LeaveThreeInRow(void);
void func_02015d48(void);
#ifdef __cplusplus
}
#endif
