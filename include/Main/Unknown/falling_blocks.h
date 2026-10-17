#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Sprite object embedded in each piece.
typedef struct Sprite {
    u32 mUnk_00;
    s32 mUnk_04;
    u8 mUnk_08[0x14 - 0x08];
    u32 mUnk_14;
    u8 mUnk_18[0x104 - 0x18];
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
    s16 mUnk_110;
    u8 mUnk_112[2];
} Piece;

// The falling pair of pieces.
typedef struct Pair {
    Piece mPiece[2];
    Vec2i mPos;
    u8 mDir;
    s32 mUnk_234;
    u32 mUnk_238;
    s32 mUnk_23c;
    s16 mUnk_240;
    u8 mUnk_242;
} Pair;

#define BOARD_COLS 6
#define BOARD_ROWS 10
#define PIECE_LIST_SIZE 60

typedef struct Board {
    u8 mUnk_00;
    u8 mUnk_01;
    u8 mUnk_02[0x10 - 0x02];
    s32 mUnk_10;
    s32 mUnk_14;
    u8 mUnk_18[0x24c - 0x18];
    Piece mGrid[BOARD_COLS][BOARD_ROWS];
    u8 mUnk_42fc[0x430c - 0x42fc];
    u16 mUnk_430c;
    u8 mUnk_430e[0x4310 - 0x430e];
    Piece mList[PIECE_LIST_SIZE];
    u16 mCount;
    Pair mUnk_83c4;
    Pair mUnk_8608;
    Sprite mUnk_884c;
    u8 mUnk_8950;
    u8 mUnk_8951[3];
    Vec2i mUnk_8954;
    Vec2i mUnk_895c;
    s32 mUnk_8964;
    s16 mUnk_8968;
    u16 mUnk_896a;
    u16 mUnk_896c;
    s16 mUnk_896e;
    u16 mUnk_8970;
    u16 mUnk_8972;
    s32 mUnk_8974;
    s32 mUnk_8978[6];
    s32 mUnk_8990;
    s32 mUnk_8994;
} Board;

int func_02003194(int a, int b);
void func_020031ac(s8 col, s8 row, Vec2i *out);
void func_020031c8(const Vec2i *pos, s8 *col, s8 *row);
u16 func_020031f0(void);
void func_02003218(u8 *p);
u16 func_02003240(void);
void func_02003258(u8 *p);
u8 func_0200327c(u8 lo, u8 hi);
void func_020032b4(Piece *p, u8 type);
void func_020032e4(Piece *p, int x, int y, int mode, int flag);
void func_02003338(const Piece *src, u8 col, u8 row, int unused, Vec2i *out);
void func_020033ac(Piece *p);
BOOL func_02003430(Piece *p);
void func_02003488(Pair *pr);
void func_020034ac(Pair *pr);
void func_020034f8(const Pair *pr, Vec2i *out);
void func_0200355c(Pair *pr, Piece *unused);
void func_020035b0(Pair *pr);
void func_02003670(Pair *pr);
void func_0200373c(Pair *pr, u8 dir);
void func_02003924(Pair *pr);
void func_0200396c(Pair *pr);
void func_020039bc(void);
void func_02003a5c(void);
void func_02003aa8(Pair *pr, int a, int b);
BOOL func_02003b3c(Pair *pr);
void func_02003e50(Pair *pr);
BOOL func_02003e60(void);
void func_02003e80(const Piece *src);
BOOL func_02003ed8(u8 col, u8 row);
void func_02003f08(u8 col, u8 row);
void func_02003f2c(int level);
void func_02003fac(u8 type);
void func_0200400c(u8 col, u8 row);
BOOL func_02004050(u8 col, u8 row, u8 col2, u8 row2);
void func_020040e8(u8 col, u8 row);
void func_0200420c(void);
void func_02004230(void);

// The board, the falling pairs and the round's counters.
extern Board data_02064828;

#ifdef __cplusplus
}
#endif
