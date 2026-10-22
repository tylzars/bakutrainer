#include <nitro/os.h>
#include <Main/clib.h>
#include <Main/Save/save_blocks.h>
#include <Main/System/random_angle.h>
#include <Main/System/screen_fade.h>
#include <Main/System/system_init.h>
#include <Main/System/bg_image.h>
#include <Main/System/prompt_screen.h>
#include <Main/Minigame/three_in_row.h>
#include <Main/Graphics/text_draw.h>
#include <Main/Graphics/texture_blocks.h>
#include <Main/Graphics/sprite_script.h>
#include <Main/Graphics/script_data.h>

u8 gShapeCellCounts[SHAPE_COUNT];
ThreeInRow gThreeInRow;

const u8 gShapeCellOffsets[SHAPE_CELLS] = {0, 1, 2, 4, 5, 6, 8, 9, 10};
const u8 gBoardTemplate[GRID_BYTES] = {0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1};
const u8 gShapeMasks[SHAPE_COUNT][SHAPE_CELLS] = {
    {1, 1, 0, 0, 0, 0, 0, 0, 0}, {1, 0, 0, 1, 0, 0, 0, 0, 0}, {1, 1, 1, 0, 0, 0, 0, 0, 0}, {1, 0, 0, 1, 0, 0, 1, 0, 0},
    {0, 1, 0, 1, 1, 0, 0, 0, 0}, {1, 0, 0, 1, 1, 0, 0, 0, 0}, {1, 1, 0, 1, 0, 0, 0, 0, 0}, {1, 1, 0, 0, 1, 0, 0, 0, 0},
    {1, 0, 0, 1, 1, 0, 1, 0, 0}, {1, 1, 1, 0, 1, 0, 0, 0, 0}, {0, 1, 0, 1, 1, 0, 0, 1, 0}, {0, 1, 0, 1, 1, 1, 0, 0, 0},
};

// Places random shapes, each at the first position where it fits, on a copy of the previous board (board n holds the
// first n shapes), rejecting a placement that closes in an empty cell, until the last board is full; fills recs and
// returns the number of shapes placed.
int PlaceRandomShapes(u8 *boards, ShapeRec *recs) {
    int i, j, x, y, k;
    int placed;
    int n;
    u32 r;
    u8 *board;
    const u8 *t;

    memcpy(boards, gBoardTemplate, GRID_BYTES);
    memcpy(boards + GRID_BYTES, boards, (GRID_PIECES - 1) * GRID_BYTES);
    for (j = 0; j < SHAPE_COUNT; j++) {
        int sum = 0;
        for (y = 0; y < SHAPE_CELLS; y++) {
            sum += gShapeMasks[j][y];
        }
        gShapeCellCounts[j] = sum;
    }
    placed = 0;
    do {
        r = RandomRange(0, SHAPE_COUNT - 1);
        t = gShapeMasks[r];
        for (y = 0; y < 4; y++) {
            for (x = 0; x < 3; x++) {
                i = placed * GRID_BYTES + y * 4 + x;
                board = boards + i;
                if (!((board[0] & t[0]) | (board[1] & t[1]) | (board[2] & t[2]) | (board[4] & t[3]) |
                      (board[5] & t[4]) | (board[6] & t[5]) | (board[8] & t[6]) | (board[9] & t[7]) |
                      (board[10] & t[8]))) {
                    recs[placed].mX = x * 32 + 0x54;
                    recs[placed].mY = y * 32 + 0x20;
                    recs[placed].mShapeIndex = r;
                    recs[placed].mFreeCellCount = gShapeCellCounts[r];
                    memcpy(boards + (placed * GRID_BYTES + GRID_BYTES), ((u8(*)[GRID_BYTES]) boards)[placed],
                           GRID_BYTES);
                    for (k = 0; k < SHAPE_CELLS; k++) {
                        if (t[k] != 0) {
                            board[gShapeCellOffsets[k] + GRID_BYTES] = (placed << 4) + CELL_FREE;
                        }
                    }
                    n = 0;
                    for (k = 0; k < 4; k++) {
                        for (j = 0; j < 3; j++) {
                            u8 *p = boards + ((placed + 1) * GRID_BYTES + k * 4 + j);
                            n += *p & 0xf;
                            if (*p == 0 && p[1] != 0 && p[4] != 0 && (j == 0 || p[-1] != 0) && (k == 0 || p[-4] != 0)) {
                                goto reject;
                            }
                        }
                    }
                    placed++;
                reject:
                    y = PIECE_GONE;
                    x = PIECE_GONE;
                }
            }
        }
    } while (n < SHAPE_COUNT);
    return placed;
}

// Returns 2 when player has three cells in a line (across, down or diagonal): every shape record is marked PIECE_GONE
// and the board cleared. Otherwise returns 1 for the first shape whose cells are all taken, clearing its cells and
// marking its record PIECE_GONE, or 0.
int CheckAndClearLineOrFilledShape(int count, ShapeRec *recs, u8 *board, int player) {
    int found = 0;
    int x, y, i, k;

    for (y = 0; y < 4; y++) {
        for (x = 0; x < 3; x++) {
            u8 *p;
            p = board + (x + y * 4);
            if ((x == 0 && (p[0] & 0xf) == player && (p[1] & 0xf) == player && (p[2] & 0xf) == player) ||
                (y < 2 && (p[0] & 0xf) == player && (p[4] & 0xf) == player && (p[8] & 0xf) == player) ||
                (x == 0 && y < 2 && (p[0] & 0xf) == player && (p[5] & 0xf) == player && (p[10] & 0xf) == player) ||
                (x == 0 && y >= 2 && (p[0] & 0xf) == player && (p[-3] & 0xf) == player && (p[-6] & 0xf) == player)) {
                y = PIECE_GONE;
                found = 1;
                x = PIECE_GONE;
            }
        }
    }
    if (found == 1) {
        for (i = 0; i < GRID_PIECES; i++) {
            recs->mShapeIndex = PIECE_GONE;
            recs++;
        }
        memset(board, 0, GRID_BYTES);
        return 2;
    }
    for (i = 0; i < count; i++) {
        const u8 *t;
        u8 *p;
        if (recs[i].mShapeIndex == PIECE_GONE) {
            continue;
        }
        t = gShapeMasks[recs[i].mShapeIndex];
        p = board + ((recs[i].mX - 0x54) / 32 + (recs[i].mY - 0x20) / 32 * 4);
        for (k = 0; k < SHAPE_CELLS; k++) {
            if (t[k] == 1 && (p[gShapeCellOffsets[k]] & 0xf) < CELL_HUMAN) {
                break;
            }
        }
        if (k == SHAPE_CELLS) {
            for (k = 0; k < SHAPE_CELLS; k++) {
                if (t[k] == 1) {
                    p[gShapeCellOffsets[k]] = 0;
                }
            }
            recs[i].mShapeIndex = PIECE_GONE;
            return 1;
        }
    }
    return 0;
}

int EvaluateMove(u8 *board, int player, ShapeRec *recs, int x, int y) {
    int hi = board[x + y * 4] >> 4;
    int row;
    int col;
    u8 *p;
    u8 c0;
    u8 c1;
    u8 c2;
    int h0;
    int h1;
    int h2;

    for (row = 0; row < 4; row++) {
        for (col = 0; col < 3; col++) {
            p = board + (col + row * 4);
            c0 = p[0];
            h0 = c0 >> 4;
            if ((c0 & 0xf) != player || (h0 != hi && recs[h0].mFreeCellCount == 0)) {
                continue;
            }
            c1 = p[1];
            c2 = p[2];
            if (col == 0 && (c1 & 0xf) == player && (c2 & 0xf) == player) {
                h1 = c1 >> 4;
                h2 = c2 >> 4;
                if ((h1 == hi || recs[h1].mFreeCellCount != 0) && (h2 == hi || recs[h2].mFreeCellCount != 0)) {
                    return SCORE_WIN;
                }
            }
            c1 = p[4];
            c2 = p[8];
            if (row < 2 && (c1 & 0xf) == player && (c2 & 0xf) == player) {
                h1 = c1 >> 4;
                h2 = c2 >> 4;
                if ((h1 == hi || recs[h1].mFreeCellCount != 0) && (h2 == hi || recs[h2].mFreeCellCount != 0)) {
                    return SCORE_WIN;
                }
            }
            c1 = p[5];
            c2 = p[10];
            if (col == 0 && row < 2 && (c1 & 0xf) == player && (c2 & 0xf) == player) {
                h1 = c1 >> 4;
                h2 = c2 >> 4;
                if ((h1 == hi || recs[h1].mFreeCellCount != 0) && (h2 == hi || recs[h2].mFreeCellCount != 0)) {
                    return SCORE_WIN;
                }
            }
            c1 = p[-3];
            c2 = p[-6];
            if (col == 0 && row >= 2 && (c1 & 0xf) == player && (c2 & 0xf) == player) {
                h1 = c1 >> 4;
                h2 = c2 >> 4;
                if ((h1 == hi || recs[h1].mFreeCellCount != 0) && (h2 == hi || recs[h2].mFreeCellCount != 0)) {
                    return SCORE_WIN;
                }
            }
        }
    }
    if (recs[hi].mFreeCellCount == 0) {
        return SCORE_LOSS_PIECE;
    }
    return 0;
}

// Tries each free cell for player: EvaluateMove's score, and below mSearchDepth and short of a win, minus the other
// player's best reply after a 0 score or plus the same player's next move otherwise. Returns the best score (0 when no
// cell is free); at depth 0 it also stores the best cell in mUnk_01.
int FindBestMove(ShapeRec *recs, u8 *board, int player, u32 depth) {
    int x, y;
    int bestPos;
    int pos;
    int score;
    int other;
    int best = SCORE_NONE;

    if (player == CELL_HUMAN) {
        other = CELL_COMPUTER;
    } else {
        other = CELL_HUMAN;
    }
    for (y = 0; y < 4; y++) {
        for (x = 0; x < 3; x++) {
            pos = x + y * 4;
            if ((board[pos] & 0xf) == CELL_FREE) {
                recs[board[pos] >> 4].mFreeCellCount--;
                board[pos] = (board[pos] & 0xf0) | player;
                score = EvaluateMove(board, player, recs, x, y);
                if (depth < gThreeInRow.mSearchDepth && score < SCORE_WIN) {
                    if (score == 0) {
                        score -= FindBestMove(recs, board, other, depth + 1);
                    } else {
                        score += FindBestMove(recs, board, player, depth + 1);
                    }
                }
                if (score > best) {
                    best = score;
                    bestPos = pos;
                }
                recs[board[pos] >> 4].mFreeCellCount++;
                board[pos] = (board[pos] & 0xf0) | CELL_FREE;
            }
        }
    }
    if (depth == 0) {
        if (best == SCORE_NONE) {
            OS_Terminate();
        }
        gThreeInRow.mUnk_01 = bestPos;
    }
    return best == SCORE_NONE ? 0 : best;
}

void StartThreeInRowBoard(void) {
    gThreeInRow.mSearchDepth = 4;
    gThreeInRow.mUnk_01 = 0x40;
    gThreeInRow.mMoveTimer = 4;
    gThreeInRow.mEmptyCount = 0;
    gThreeInRow.mUnk_0c = 0;
    gThreeInRow.mTurn++;
    gThreeInRow.mMarkSprite.mUnk_de = 1;
    gThreeInRow.mShapeCount = PlaceRandomShapes(gThreeInRow.mBoards, gThreeInRow.mShapes);
    memcpy(gThreeInRow.mBoards, gThreeInRow.mBoards + gThreeInRow.mShapeCount * GRID_BYTES, GRID_BYTES);
}

void UpdateThreeInRowBoard(void) {
    ThreeInRow *s = &gThreeInRow;
    int i;
    int player;
    int cell;
    int result;
    int x, y;

    s->mEmptyCount = 0;
    for (i = 0; i < 16; i++) {
        if ((i & 3) != 3 && (s->mBoards[i] & 0xf) == 0) {
            s->mEmptyCount++;
        }
    }
    if (s->mEmptyCount == GRID_COLS * GRID_ROWS) {
        DrawText(10, 20, data_0201a6d0, "touche screen to continue", 0, 240, 16, 2);
        if (gSystemState.mTouchPressed != 1) {
            return;
        }
        StartThreeInRowBoard();
    }
    if (s->mUnk_01 & 0x40) {
        if (s->mMoveTimer == 0) {
            if (s->mTurn & 1) {
                DrawText(10, 20, data_0201a6d0, "Ordi", 0, 240, 16, 2);
                FindBestMove(s->mShapes, s->mBoards, CELL_COMPUTER, 0);
            } else {
                DrawText(10, 20, data_0201a6d0, "Humain", 0, 240, 16, 2);
                if (gSystemState.mTouchPressed == 1) {
                    x = gSystemState.mTouchX - 0x54;
                    y = gSystemState.mTouchY - 0x20;
                    cell = x / 32 + y / 32 * 4;
                    if (x >= 0 && x < 0x60 && y >= 0 && y < 0x80 && (s->mBoards[cell] & 0xf) == CELL_FREE) {
                        s->mUnk_01 = cell;
                    }
                }
            }
        } else {
            s->mMoveTimer--;
        }
    }
    for (i = 0; i < s->mShapeCount; i++) {
        if (s->mShapes[i].mShapeIndex != PIECE_GONE) {
            DrawSprite(0xf90 + s->mShapes[i].mShapeIndex, s->mShapes[i].mX, s->mShapes[i].mY, 5, 2);
        }
    }
    for (y = 0; y < 4; y++) {
        for (x = 0, i = 0x54; x < 3; x++, i += 32) {
            switch (s->mBoards[y * 4 + x] & 0xf) {
                case 2:
                    DrawSprite(0xf9d, i, y * 32 + 0x20, 4, 2);
                    break;
                case 3:
                    DrawSprite(0xf9c, i, y * 32 + 0x20, 4, 2);
                    break;
            }
        }
    }
    for (i = 0; i < s->mUnk_0c; i++) {
        if (s->mUnk_04[i] == CELL_HUMAN) {
            DrawText(20, 30 + i * 12, data_0201a6d0, "H", 0, 240, 16, 2);
        } else {
            DrawText(20, 30 + i * 12, data_0201a6d0, "O", 0, 240, 16, 2);
        }
    }
    if (s->mUnk_01 < 0xf) {
        cell = s->mUnk_01 & 0xf;
        player = (s->mTurn & 1) ? CELL_COMPUTER : CELL_HUMAN;
        StartSprite(&s->mMarkSprite, (void *) (data_0201aa60 + 37)[player]);
        s->mUnk_01 |= 0x80;
        s->mBoards[cell] = (s->mBoards[cell] & 0xf0) | player;
    }
    if (s->mMarkSprite.mUnk_de == 1) {
        return;
    }
    cell = s->mUnk_01 & 0xf;
    DrawSprite(UpdateSprite(&s->mMarkSprite), (cell & 3) * 32 + 0x54, (cell >> 2) * 32 + 0x20, 0, 2);
    if (s->mMarkSprite.mUnk_de == 1) {
        player = (s->mTurn & 1) ? CELL_COMPUTER : CELL_HUMAN;
        s->mShapes[s->mBoards[cell] >> 4].mFreeCellCount--;
        result = CheckAndClearLineOrFilledShape(s->mShapeCount, s->mShapes, s->mBoards, player);
        if (result == 0) {
            s->mTurn++;
        } else {
            s->mUnk_04[s->mUnk_0c++] = player;
        }
        if (result != 2) {
            s->mUnk_01 |= 0x40;
        }
        s->mMoveTimer = 4;
    }
}

void SetupThreeInRow(void) {
    ReleaseTextureLayers();
    SetupTextureLayer(0x10cb, 0x10ca, 0x10cc, 0, 2);
    SetLcdSwap(1);
    LoadImage(0xde8, 1);
    LoadImage(0xde7, 2);
    FadeClearAndWait(2);
    gThreeInRow.mBoards = Alloc(GRID_PIECES * GRID_BYTES);
    gThreeInRow.mTurn = 0;
    StartThreeInRowBoard();
}

void UpdateThreeInRow(void) {
    UpdateThreeInRowBoard();
}

void LeaveThreeInRow(void) {
    Free(&gThreeInRow.mBoards);
    func_020102f8();
}

void func_02015d48(void) {}
