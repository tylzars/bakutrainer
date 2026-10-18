#pragma once

#include "types.h"
#include <Main/Unknown/falling_blocks.h>

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

// One piece of the board: where it is drawn, which of the 12 shapes it has and how many of its cells are still free.
typedef struct ShapeRec {
    s16 mUnk_00;
    s16 mUnk_02;
    s16 mUnk_04;
    s16 mUnk_06;
} ShapeRec;

typedef struct Unk_020b13d4 {
    u8 mUnk_00;
    u8 mUnk_01;
    u8 mUnk_02;
    u8 mUnk_03;
    u8 mUnk_04[8];
    u8 mUnk_0c;
    u8 mUnk_0d;
    ShapeRec mUnk_0e[GRID_PIECES];
    u8 *mUnk_40;
    s32 mUnk_44;
    u32 mUnk_48;
    Sprite mUnk_4c;
} Unk_020b13d4;

extern u8 data_020b13c8[SHAPE_COUNT];
extern Unk_020b13d4 data_020b13d4;

int func_02015340(u8 *boards, ShapeRec *recs);
int func_0201553c(int count, ShapeRec *recs, u8 *board, int player);
int func_020156c4(u8 *board, int player, ShapeRec *recs, int x, int y);
int func_02015850(ShapeRec *recs, u8 *board, int player, u32 depth);
void func_02015960(void);
void func_020159ac(void);
void func_02015cd4(void);

void func_02015d2c(void);
void func_02015d34(void);
void func_02015d48(void);
#ifdef __cplusplus
}
#endif
