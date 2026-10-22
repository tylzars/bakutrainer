#include <Main/System/random_angle.h>
#include <Main/System/screen_fade.h>
#include <Main/Scene/stage_info_panel.h>
#include <Main/Graphics/sprite_slots.h>
#include <Main/Unknown/game_util.h>
#include <Main/Unknown/move_object.h>
#include <Main/System/sound_control.h>
#include <Main/Minigame/falling_blocks.h>
#include <Main/Scene/mode_select.h>
#include <Main/Graphics/text_draw.h>
#include <Main/Graphics/model.h>
#include <Main/Sound/wave_player.h>
#include <Main/Graphics/sprite_script.h>
#include <Main/Graphics/script_data.h>
#include <Main/System/state_flow.h>
#include <Main/System/system_init.h>
#include <Main/Graphics/gx_pool.h>
#include <nitro/os.h>
#include <stdio.h>

Board gBoard;

int GetFallingBlocksValue(int a, int b) {
    return GetFileValue(0xd66, b + a * 12);
}

void CellToBoardPos(s8 col, s8 row, Vec2i *out) {
    out->x = (col * 20 + 0x4d) << 8;
    out->y = (row * 20 - 4) << 8;
}

void BoardPosToCell(const Vec2i *pos, s8 *col, s8 *row) {
    *col = ((pos->x >> 8) - 0x4d) / 20;
    *row = ((pos->y >> 8) + 4) / 20;
}

u16 func_020031f0(void) {
    return (NextRandom() % 10 == 0) ? 0 : RandomRange(gBoard.mUnk_896a, gBoard.mUnk_896c);
}

void func_02003218(u8 *p) {
    gBoard.mUnk_8968--;
    if (gBoard.mUnk_8968 <= 0) {
        *p += 7;
        gBoard.mUnk_8968 = func_020031f0();
    }
}

u16 func_02003240(void) {
    return RandomRange(gBoard.mUnk_8970, gBoard.mUnk_8972);
}

void func_02003258(u8 *p) {
    gBoard.mUnk_896e--;
    if (gBoard.mUnk_896e <= 0) {
        *p = 0xe;
        gBoard.mUnk_896e = func_02003240();
    }
}

u8 PickPieceType(u8 lo, u8 hi) {
    int roll = RandomRange(1, 100);
    int span = hi - lo;
    int step = 100 / (span + 1);
    int i;
    int sum = 0;

    for (i = 0; i < span; i++) {
        sum += step;
        if (roll < sum) {
            break;
        }
    }
    return lo + i;
}

// Sets the piece's type, cancels a removal marker (mUnk_110 = -1) and, for a type other than 0, starts the type's
// sprite.
void SetPieceType(Piece *p, u8 type) {
    p->mType = type;
    p->mUnk_110 = -1;
    if (type != 0) {
        u32 idx;

        if (type < 7) {
            idx = type + 12;
        } else {
            idx = type + 11;
        }
        StartSprite(&p->mSprite, data_0201abec[(u16) idx]);
    }
}

void DrawPiece(Piece *p, int x, int y, int layer, int flag) {
    int prio;
    int frame;

    if (p->mType != 0) {
        if (layer == 1) {
            prio = 0x1ff;
        } else {
            prio = 0xff;
        }
        if (p->mUnk_110 >= 0) {
            prio -= 5;
        }
        frame = UpdateSprite(&p->mSprite);
        if (layer == 2) {
            frame++;
            if (flag == 0) {
                prio--;
            }
        }
        func_0202de80(frame, x, y, prio, -1, layer);
    }
}

// Copies the piece into the cell and stores the cell in *out. In row 0 the piece in column 2 ends the round; in the
// other columns it is cleared again at once.
void PlacePiece(const Piece *src, u8 col, u8 row, int unused, Vec2i *out) {
    gBoard.mGrid[col][row] = *src;
    CellToBoardPos(col, row, &gBoard.mGrid[col][row].mPos);
    if (out != NULL) {
        out->x = col;
        out->y = row;
    }
    if (row == 0) {
        if (col == 2) {
            EndFallingBlocks(1);
        } else {
            ClearCell(col, row);
        }
    }
}

void StartPieceRemoval(Piece *p) {
    if (p->mUnk_110 < 0 && p->mType != 0) {
        gBoard.mRemovingCount++;
        p->mUnk_110 = 1;
        p->mSprite.mUnk_14 |= 1;
        if (p->mType < 7) {
            s32 left = gBoard.mUnk_8978[p->mType - 1] - 1;

            if (left < 0) {
                left = 0;
            }
            gBoard.mUnk_8978[p->mType - 1] = left;
        }
        PlayWaveKeepAlive(0x56, 0x4b, 0, 0x40, 0xffff, 0);
        if (p->mType < 7) {
            AddCappedAt999Shifted(&gBoard.mScore, gBoard.mPieceScore, 0);
        }
        gBoard.mRemovedCount++;
    }
}

BOOL DropPiece(Piece *p) {
    s8 cell[2];

    p->mPos.y += 0x640;
    BoardPosToCell(&p->mPos, &cell[1], &cell[0]);
    if (cell[0] + 1 >= BOARD_ROWS || !IsCellEmpty(cell[1], cell[0] + 1)) {
        PlacePiece(p, cell[1], cell[0], 0, NULL);
        return TRUE;
    }
    return FALSE;
}

// Restarts the land timer when it is running (above 0) and resets remain, using one.
void ResetLandTimer(Pair *pr) {
    if (pr->mLandTimer > 0 && pr->mLandResetsRemaining != 0) {
        pr->mLandTimer = 0x12;
        pr->mLandResetsRemaining--;
    }
}

void SwapPairPieces(Pair *pr) {
    Piece tmp = pr->mPiece[0];

    pr->mPiece[0] = pr->mPiece[1];
    pr->mPiece[1] = tmp;
}

void GetPairSecondPos(const Pair *pr, Vec2i *out) {
    *out = pr->mPos;
    switch (pr->mDir) {
        case PAIR_DIR_UP:
            out->y -= 5 << 10;
            break;
        case PAIR_DIR_RIGHT:
            out->x += 5 << 10;
            break;
        case PAIR_DIR_DOWN:
            out->y += 5 << 10;
            break;
        case PAIR_DIR_LEFT:
            out->x -= 5 << 10;
            break;
    }
}

void LandPair(Pair *pr, Piece *unused) {
    s8 cell[2];

    if (pr->mLandTimer < 0) {
        pr->mLandTimer = 0x12;
        PlayWaveKeepAlive(0x57, 0x4b, 0, 0x40, 0xffff, 0);
    }
    BoardPosToCell(&pr->mPos, &cell[1], &cell[0]);
    CellToBoardPos(cell[1], cell[0], &pr->mPos);
}

void MovePairLeft(Pair *pr) {
    Vec2i pos;
    s8 cell[2];

    if (pr->mSplit == 0) {
        if (pr->mDir == PAIR_DIR_LEFT) {
            GetPairSecondPos(pr, &pos);
        } else {
            pos = pr->mPos;
        }
        BoardPosToCell(&pos, &cell[1], &cell[0]);
        if (pr->mDir == PAIR_DIR_DOWN) {
            cell[0]++;
        }
        if (cell[1] > 0 && IsCellEmpty(cell[1] - 1, cell[0])) {
            BOOL free;

            if (cell[0] >= BOARD_ROWS) {
                free = TRUE;
            } else {
                free = IsCellEmpty(cell[1] - 1, cell[0] + 1);
            }
            if (free) {
                pr->mPos.x -= 5 << 10;
                ResetLandTimer(pr);
            }
        }
        PlayWaveKeepAlive(0x10c, 0x4b, 0, 0x40, 0xffff, 0);
    }
}

void MovePairRight(Pair *pr) {
    Vec2i pos;
    s8 cell[2];

    if (pr->mSplit == 0) {
        if (pr->mDir == PAIR_DIR_RIGHT) {
            GetPairSecondPos(pr, &pos);
        } else {
            pos = pr->mPos;
        }
        BoardPosToCell(&pos, &cell[1], &cell[0]);
        if (pr->mDir == PAIR_DIR_DOWN) {
            cell[0]++;
        }
        if (cell[1] < BOARD_COLS - 1 && IsCellEmpty(cell[1] + 1, cell[0])) {
            BOOL free;

            BOOL edge = cell[0] >= BOARD_ROWS || cell[0] <= 0;

            if (edge) {
                free = TRUE;
            } else {
                free = IsCellEmpty(cell[1] + 1, cell[0] + 1);
            }
            if (free) {
                pr->mPos.x += 5 << 10;
                ResetLandTimer(pr);
            }
        }
        PlayWaveKeepAlive(0x10c, 0x4b, 0, 0x40, 0xffff, 0);
    }
}

void TurnPairTo(Pair *pr, u8 dir) {
    s8 cell[2];
    s8 row;
    s8 row1;
    s8 row2;

    BoardPosToCell(&pr->mPos, &cell[1], &cell[0]);
    row = cell[0];
    row1 = row < BOARD_ROWS - 1 ? row + 1 : row;
    row2 = row1 < BOARD_ROWS - 1 ? row1 + 1 : row1;
    switch (dir) {
        case PAIR_DIR_UP:
            pr->mDir = dir;
            ResetLandTimer(pr);
            return;
        case PAIR_DIR_RIGHT:
            if (cell[1] < BOARD_COLS - 1 && IsCellEmpty(cell[1] + 1, row) && IsCellEmpty(cell[1] + 1, row1)) {
                pr->mDir = dir;
                return;
            }
            if (cell[1] > 0 && IsCellEmpty(cell[1] - 1, cell[0]) && IsCellEmpty(cell[1] - 1, row1)) {
                pr->mDir = dir;
                pr->mPos.x -= 5 << 10;
                ResetLandTimer(pr);
                return;
            }
            SwapPairPieces(pr);
            ResetLandTimer(pr);
            return;
        case PAIR_DIR_DOWN:
            if (row < BOARD_ROWS - 1 && IsCellEmpty(cell[1], row1) && IsCellEmpty(cell[1], row2)) {
                pr->mDir = dir;
                ResetLandTimer(pr);
                return;
            }
            pr->mDir = dir;
            pr->mPos.y -= 5 << 10;
            ResetLandTimer(pr);
            return;
        case PAIR_DIR_LEFT:
            if (cell[1] > 0 && IsCellEmpty(cell[1] - 1, cell[0]) && IsCellEmpty(cell[1] - 1, row1)) {
                pr->mDir = dir;
                ResetLandTimer(pr);
                return;
            }
            if (cell[1] < BOARD_COLS - 1 && IsCellEmpty(cell[1] + 1, cell[0]) && IsCellEmpty(cell[1] + 1, row1)) {
                pr->mDir = dir;
                pr->mPos.x += 5 << 10;
                ResetLandTimer(pr);
                return;
            }
            SwapPairPieces(pr);
            ResetLandTimer(pr);
            return;
    }
}

void TurnPairClockwise(Pair *pr) {
    if (pr->mSplit == 0) {
        TurnPairTo(pr, (pr->mDir + 1) % 4);
        PlayWaveKeepAlive(0x17b, 0x4b, 0, 0x40, 0xffff, 0);
    }
}

void TurnPairCounterclockwise(Pair *pr) {
    if (pr->mSplit == 0) {
        int dir;

        if (pr->mDir == PAIR_DIR_UP) {
            dir = PAIR_DIR_LEFT;
        } else {
            dir = (pr->mDir - 1) % 4;
        }
        TurnPairTo(pr, dir);
        PlayWaveKeepAlive(0x17b, 0x4b, 0, 0x40, 0xffff, 0);
    }
}

void PickNextPair(void) {
    Pair *pr = &gBoard.mNextPair;
    u8 type[2];

    type[1] = PickPieceType(1, gBoard.mTypeCount);
    func_02003218(&type[1]);
    func_02003258(&type[1]);
    type[0] = PickPieceType(1, gBoard.mTypeCount);
    func_02003218(&type[0]);
    func_02003258(&type[0]);
    SetPieceType(&pr->mPiece[0], type[1]);
    SetPieceType(&pr->mPiece[1], type[0]);
    pr->mDir = PAIR_DIR_UP;
    pr->mSplit = 0;
    pr->mSpeed = gBoard.mFallSpeed;
    pr->mLandTimer = -1;
    pr->mLandResetsRemaining = 3;
    pr->mPos.x = 0x37 << 10;
    pr->mPos.y = 0x45 << 8;
}

void TakeNextPair(void) {
    gBoard.mPair = gBoard.mNextPair;
    PickNextPair();
    CellToBoardPos(2, 0, &gBoard.mPair.mPos);
    gBoard.mPair.mPos.y -= 5 << 10;
    gBoard.mPairState = 0;
}

void DrawPair(Pair *pr, int a, int b) {
    Vec2i pos;

    if (a == 2 || gBoard.mPairState < 2) {
        DrawPiece(&pr->mPiece[0], pr->mPos.x >> 8, pr->mPos.y >> 8, 1, 0);
        if (pr->mSplit == 0) {
            GetPairSecondPos(pr, &pos);
            DrawPiece(&pr->mPiece[1], pos.x >> 8, pos.y >> 8, 1, 1);
            if (b != 0) {
                UpdateSprite(&gBoard.mPairMarker);
                func_0202de80(gBoard.mPairMarker.mUnk_04, pr->mPos.x >> 8, pr->mPos.y >> 8, 0xfa, -1, 1);
            }
        }
    }
}

BOOL DropPair(Pair *pr) {
    BOOL result = FALSE;
    s32 speed;
    Vec2i partner;
    Vec2i swap;
    s8 cell[4];

    if (gBoard.mPairState != 0 || gBoard.mRemovingCount != 0 || gBoard.mCount != 0) {
        return TRUE;
    }
    if (pr->mSplit != 0) {
        speed = 0x19 << 12;
    } else {
        speed = pr->mSpeed;
    }
    pr->mPos.y += (speed << 4) / 1024;
    GetPairSecondPos(pr, &partner);
    pr->mPiece[0].mPos = pr->mPos;
    pr->mPiece[1].mPos = partner;
    BoardPosToCell(&pr->mPos, &cell[3], &cell[2]);
    BoardPosToCell(&partner, &cell[1], &cell[0]);
    if (cell[2] < 0 || cell[0] < 0) {
        return FALSE;
    }
    if (cell[2] == BOARD_ROWS || cell[0] == BOARD_ROWS) {
        cell[2]--;
        cell[0]--;
    }
    if (pr->mLandTimer > 0) {
        pr->mLandTimer--;
    }
    if (cell[2] + 1 >= BOARD_ROWS || (pr->mSplit == 0 && cell[0] + 1 >= BOARD_ROWS)) {
        result = TRUE;
        if (pr->mLandTimer == 0) {
            PlacePiece(&pr->mPiece[0], cell[3], cell[2], 1, &gBoard.mFirstPieceCell);
            if (pr->mSplit == 0) {
                PlacePiece(&pr->mPiece[1], cell[1], cell[0], 1, &gBoard.mSecondPieceCell);
            } else {
                PlayWaveKeepAlive(0x57, 0x4b, 0, 0x40, 0xffff, 0);
            }
            gBoard.mPairState = 1;
            return TRUE;
        }
        LandPair(pr, (pr->mSplit != 0 || cell[2] + 1 >= BOARD_ROWS) ? &pr->mPiece[0] : &pr->mPiece[1]);
    }
    if (pr->mSplit == 0 && !IsCellEmpty(cell[1], cell[0] + 1)) {
        result = TRUE;
        if (pr->mLandTimer == 0) {
            PlacePiece(&pr->mPiece[1], cell[1], cell[0], 1, &gBoard.mSecondPieceCell);
            if (pr->mDir == PAIR_DIR_LEFT || pr->mDir == PAIR_DIR_RIGHT) {
                pr->mSplit = 1;
                pr->mSplitFrame = gSystemState.mElapsedFrames;
                pr->mLandTimer = 0;
            } else {
                PlacePiece(&pr->mPiece[0], cell[3], cell[2], 1, &gBoard.mFirstPieceCell);
                gBoard.mPairState = result;
                return result;
            }
        } else {
            LandPair(pr, &pr->mPiece[1]);
        }
    }
    if (!IsCellEmpty(cell[3], cell[2] + 1)) {
        result = TRUE;
        if (pr->mLandTimer == 0) {
            PlacePiece(&pr->mPiece[0], cell[3], cell[2], 1, &gBoard.mFirstPieceCell);
            if (pr->mSplit == 0) {
                if (pr->mDir == PAIR_DIR_LEFT || pr->mDir == PAIR_DIR_RIGHT) {
                    pr->mSplit = 1;
                    pr->mSplitFrame = gSystemState.mElapsedFrames;
                    pr->mLandTimer = 0;
                    pr->mPiece[0] = pr->mPiece[1];
                    pr->mPos = partner;
                    swap = gBoard.mFirstPieceCell;
                    gBoard.mFirstPieceCell = gBoard.mSecondPieceCell;
                    gBoard.mSecondPieceCell = swap;
                } else {
                    PlacePiece(&pr->mPiece[1], cell[1], cell[0], 1, &gBoard.mSecondPieceCell);
                    gBoard.mPairState = result;
                    return result;
                }
            } else {
                gBoard.mPairState = result;
                if (gSystemState.mElapsedFrames - pr->mSplitFrame > 1) {
                    PlayWaveKeepAlive(0x57, 0x4b, 0, 0x40, 0xffff, 0);
                }
                return TRUE;
            }
        } else {
            LandPair(pr, pr->mPiece);
        }
    }
    return result;
}

void DropPairUntilLanded(Pair *pr) {
    do {
    } while (!DropPair(pr));
}

BOOL IsBoardSettled(void) {
    if (gBoard.mRemovingCount == 0 && gBoard.mCount == 0) {
        return TRUE;
    }
    return FALSE;
}

void AddFallingPiece(const Piece *src) {
    int i;

    for (i = 0; i < PIECE_LIST_SIZE; i++) {
        if (gBoard.mList[i].mType == 0) {
            gBoard.mList[i] = *src;
            gBoard.mCount++;
            return;
        }
    }
}

BOOL IsCellEmpty(u8 col, u8 row) {
    if (gBoard.mGrid[col][row].mType == 0) {
        return TRUE;
    }
    return FALSE;
}

void ClearCell(u8 col, u8 row) {
    gBoard.mGrid[col][row].mType = 0;
}

// Starting contents of the board for each level, rows top to bottom, columns left to right.
const u8 gStartingBoards[2][BOARD_ROWS][BOARD_COLS] = {
    {
        {0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0},
        {1, 2, 1, 1, 1, 3},
    },
    {
        {0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0},
        {0, 0, 2, 2, 0, 0},
        {2, 2, 2, 2, 2, 2},
    },
};

void FillStartingBoard(int level) {
    int col;
    int row;

    for (col = 0; col < BOARD_COLS; col++) {
        for (row = 0; row < BOARD_ROWS; row++) {
            SetPieceType(&gBoard.mGrid[col][row], gStartingBoards[level][row][col]);
            CellToBoardPos(col, row, &gBoard.mGrid[col][row].mPos);
        }
    }
}

void RemovePiecesOfType(u8 type) {
    int col;
    int row;

    for (col = 0; col < BOARD_COLS; col++) {
        for (row = 0; row < BOARD_ROWS; row++) {
            if (type == gBoard.mGrid[col][row].mType) {
                StartPieceRemoval(&gBoard.mGrid[col][row]);
            }
        }
    }
}

void StartPieceFall(u8 col, u8 row) {
    Piece *column = gBoard.mGrid[col];

    if (column[row].mType != 0 && row < BOARD_ROWS - 1 && column[row + 1].mType == 0) {
        AddFallingPiece(&column[row]);
        ClearCell(col, row);
    }
}

BOOL PiecesMatch(u8 col, u8 row, u8 col2, u8 row2) {
    Piece *a = &gBoard.mGrid[col][row];
    Piece *b = &gBoard.mGrid[col2][row2];

    if (a->mType == 0 || b->mType == 0) {
        return FALSE;
    }
    if (a->mUnk_110 > 0 && a->mSprite.mUnk_04 == gBoard.mUnk_8974 && b->mType == a->mType && b->mUnk_110 < 0) {
        return TRUE;
    }
    if (a->mType < 7 && b->mType == a->mType + 7) {
        return TRUE;
    }
    if (a->mType > 7 && a->mType != 14 && (b->mType == a->mType - 7 || b->mType == a->mType)) {
        return TRUE;
    }
    return FALSE;
}

void RemoveMatchesAt(u8 col, u8 row) {
    Piece *cell = &gBoard.mGrid[col][row];

    if (cell->mType != 0) {
        if (cell->mType == 14) {
            if (row < BOARD_ROWS - 1) {
                u8 x = 0;
                u8 t = gBoard.mGrid[col][row + 1].mType;

                if (t < 7) {
                    x = t;
                } else if (t > 7) {
                    x = t - 7;
                }
                RemovePiecesOfType(x);
                StartPieceRemoval(cell);
                return;
            }
            StartPieceRemoval(cell);
            return;
        }
        if (col != 0) {
            int c1 = col - 1;

            if (PiecesMatch(col, row, c1, row)) {
                StartPieceRemoval(cell);
                StartPieceRemoval(&gBoard.mGrid[c1][row]);
            }
        }
        if (col < BOARD_COLS - 1) {
            int c1 = col + 1;

            if (PiecesMatch(col, row, c1, row)) {
                StartPieceRemoval(cell);
                StartPieceRemoval(&gBoard.mGrid[c1][row]);
            }
        }
        if (row != 0) {
            int r1 = row - 1;

            if (PiecesMatch(col, row, col, r1)) {
                StartPieceRemoval(cell);
                StartPieceRemoval(&gBoard.mGrid[col][r1]);
            }
        }
        if (row < BOARD_ROWS - 1) {
            int r1 = row + 1;

            if (PiecesMatch(col, row, col, r1)) {
                StartPieceRemoval(cell);
                StartPieceRemoval(&gBoard.mGrid[col][r1]);
            }
        }
    }
}

// Removes the matches around every cell of rows 9..1 (row 0 is handled by PlacePiece).
void RemoveAllMatches(void) {
    int col;
    int row;

    for (col = 0; col < BOARD_COLS; col++) {
        for (row = BOARD_ROWS - 1; row > 0; row--) {
            RemoveMatchesAt(col, row);
        }
    }
}

void StartAllPieceFalls(void) {
    int col;
    int row;

    for (col = 0; col < BOARD_COLS; col++) {
        for (row = 0; row < BOARD_ROWS; row++) {
            StartPieceFall(col, row);
        }
    }
}

// Redraws every cell, and every falling piece of mList while there are any, at its position.
void DrawBoard(void) {
    int i, j;
    for (i = 0; i < BOARD_COLS; i++) {
        for (j = 0; j < BOARD_ROWS; j++) {
            DrawPiece(&gBoard.mGrid[i][j], gBoard.mGrid[i][j].mPos.x >> 8, gBoard.mGrid[i][j].mPos.y >> 8, 1, 0);
        }
    }
    if (gBoard.mCount != 0) {
        for (j = 0; j < PIECE_LIST_SIZE; j++) {
            DrawPiece(&gBoard.mList[j], gBoard.mList[j].mPos.x >> 8, gBoard.mList[j].mPos.y >> 8, 1, 0);
        }
    }
}

// Advances the falling pieces of mList while any are active, otherwise the cells of the board.
void UpdateBoard(void) {
    int i, k;
    if (gBoard.mCount != 0) {
        for (i = 0; i < PIECE_LIST_SIZE; i++) {
            if (gBoard.mList[i].mType != 0 && DropPiece(&gBoard.mList[i])) {
                SetPieceType(&gBoard.mList[i], 0);
                if (--gBoard.mCount == 0) {
                    StartAllPieceFalls();
                    if (gBoard.mCount == 0) {
                        RemoveAllMatches();
                    }
                    return;
                }
            }
        }
    } else {
        for (i = 0; i < BOARD_COLS; i++) {
            for (k = BOARD_ROWS - 1; k >= 0; k--) {
                if (gBoard.mGrid[i][k].mType != 0) {
                    Piece *c = &gBoard.mGrid[i][k];
                    if (c->mUnk_110 > 0) {
                        if (c->mSprite.mUnk_de != 0) {
                            c->mUnk_110 = 0;
                        }
                        if (c->mSprite.mUnk_04 == gBoard.mUnk_8974) {
                            RemoveMatchesAt(i, k);
                        } else if (c->mUnk_110 == 0) {
                            ClearCell(i, k);
                            gBoard.mRemovingCount--;
                        }
                    } else if (gBoard.mRemovingCount == 0) {
                        StartPieceFall(i, k);
                    }
                }
            }
        }
    }
}

void EndFallingBlocks(int a) {
    StartModeSlide(0, gBoard.mScore >= gBoard.mScoreThreshold);
}

// Starts the board screen: loads the stage settings, clears the cells and the falling-piece list and sets the
// counters.
void SetupFallingBlocks(void) {
    int i, j;
    s32 level;
    int total;
    int stagger;
    level = data_020b6ee0.mSelectedStageIndex;
    if (level < 0) {
        level = 0;
    }
    gBoard.mStage = gStages[level].mInfo;
    gBoard.mTableRow = gBoard.mStage.mGrade - 1;
    if (data_020b10fc.mTableRow < 0 || gBoard.mTableRow >= 12) {
        OS_Terminate();
    }
    func_0200c5d8(0, gBoard.mStage.mKind + 0x1cb, gBoard.mStage.mStageId + 0xdb3, 64, 16, 2, 130);
    func_02024208();
    RequestTrack(3, 76);
    func_0200bdcc(2, 16);
    for (i = 0; i < BOARD_COLS; i++) {
        for (j = 0; j < BOARD_ROWS; j++) {
            SetPieceType(&gBoard.mGrid[i][j], 0);
        }
    }
    gBoard.mUnk_4300 = ~13;
    gBoard.mUnk_42fc = 67;
    gBoard.mUnk_4308 = 186;
    gBoard.mUnk_4304 = 187;
    gBoard.mRemovingCount = 0;
    for (i = 0; i < PIECE_LIST_SIZE; i++) {
        SetPieceType(&gBoard.mList[i], 0);
    }
    gBoard.mCount = 0;
    gBoard.mUnk_8974 = 3738;
    StartSprite(&gBoard.mPairMarker, data_0201abec[26]);
    gBoard.mTypeCount = GetFallingBlocksValue(gBoard.mTableRow, 2);
    stagger = GetFallingBlocksValue(gBoard.mTableRow, 3);
    gBoard.mEndTimer = gBoard.mEndFrameCount = GetFallingBlocksValue(gBoard.mTableRow, 5) * 60000 / 1000;
    gBoard.mUnk_0c = -12800;
    gBoard.mScore = 0;
    gBoard.mScoreThreshold = GetFallingBlocksValue(gBoard.mTableRow, 6);
    gBoard.mUnk_18 = 0;
    gBoard.mUnk_896a = GetFallingBlocksValue(gBoard.mTableRow, 8);
    gBoard.mUnk_896c = GetFallingBlocksValue(gBoard.mTableRow, 9);
    gBoard.mUnk_8968 = func_020031f0();
    gBoard.mUnk_8970 = GetFallingBlocksValue(gBoard.mTableRow, 10);
    gBoard.mUnk_8972 = GetFallingBlocksValue(gBoard.mTableRow, 11);
    gBoard.mUnk_896e = func_02003240();
    gBoard.mFallSpeed = GetFallingBlocksValue(gBoard.mTableRow, 4) << 8;
    gBoard.mPieceScore = GetFallingBlocksValue(gBoard.mTableRow, 7);
    gBoard.mRemovedCount = 0;
    total = RandomRange(GetFallingBlocksValue(gBoard.mTableRow, 12), GetFallingBlocksValue(gBoard.mTableRow, 13));
    for (i = 0; i < 6; i++) {
        if (i < gBoard.mTypeCount - 1) {
            gBoard.mUnk_8978[i] = RandomRange(total / 8, total / 2);
            total -= gBoard.mUnk_8978[i];
        } else if (i == gBoard.mTypeCount - 1) {
            gBoard.mUnk_8978[i] = total;
        } else {
            gBoard.mUnk_8978[i] = -1;
        }
    }
    PickNextPair();
    if (stagger > 0) {
        FillStartingBoard((u8) ((stagger - 1 < 1 ? stagger - 1 : 1) < 0 ? 0 : (stagger - 1 < 1 ? stagger - 1 : 1)));
    }
    CellToBoardPos(2, 0, &gBoard.mPair.mPos);
    gBoard.mPair.mPos.y -= 5 << 10;
    StartModeSlide(1, 0);
}

void DrawFallingBlocks(void) {
    func_0201168c(data_020b6ee0.mSelectedStageIndex, gBoard.mScore, gModeSelectState.mScoreThreshold, gBoard.mEndTimer,
                  2);
    DrawPair(&gBoard.mNextPair, 2, 0);
    DrawBoard();
    DrawPair(&gBoard.mPair, 1, 1);
}

void UpdateFallingBlocks(void) {
    static s32 data_02064824;
    static s32 data_02064820;
    char buf[32];
    int slideState;
    int busy;
    Pair *cursor = &gBoard.mPair;
    func_020059cc(gBoard.mScore);
    slideState = GetModeSlideState();
    if (slideState != MODE_SLIDE_STATE_NONE) {
        DrawFallingBlocks();
        busy = UpdateModeSlide(1);
        if (slideState == MODE_SLIDE_STATE_END && busy == 0) {
            StartResultScreen(GetScoreThresholdMet(), gBoard.mScore);
            return;
        }
        if (slideState == MODE_SLIDE_STATE_START) {
            data_02064820 = 0;
            return;
        }
        return;
    }
    gBoard.mUnk_0c += 0x355;
    if (gBoard.mUnk_0c > 0x2000) {
        gBoard.mUnk_0c = 0x2000;
    }
    if (data_02064820 == 0) {
        data_02064820 = 1;
        TakeNextPair();
    }
    if (--gBoard.mEndTimer == 0) {
        EndFallingBlocks(1);
    }
    DropPair(cursor);
    if (gBoard.mPairState == 1) {
        gBoard.mRemovedCount = 0;
        RemoveMatchesAt(gBoard.mFirstPieceCell.x, gBoard.mFirstPieceCell.y);
        RemoveMatchesAt(gBoard.mSecondPieceCell.x, gBoard.mSecondPieceCell.y);
        gBoard.mPairState++;
    } else if (gBoard.mPairState == 2) {
        if (IsBoardSettled()) {
            if (gBoard.mRemovedCount >= 6) {
                AddCappedAt5999(&gBoard.mEndTimer, 300);
                sprintf(buf, "+%d'", 5);
                AddTextObjSlot(buf, 2, data_0201a61c, 30, gBoard.mPair.mPos.x >> 8, gBoard.mPair.mPos.y >> 8, 0, -128,
                               100, 1, 0);
            }
            gBoard.mRemovedCount = 0;
            TakeNextPair();
        }
    }
    UpdateBoard();
    if (gSystemState.mKeysPressed & 0x20) {
        MovePairLeft(cursor);
    } else if (gSystemState.mKeysPressed & 0x10) {
        MovePairRight(cursor);
    }
    if (gSystemState.mKeysDown & 0x80) {
        gBoard.mPair.mSpeed = 25 << 12;
    } else if (gSystemState.mKeysPressed & 0x40) {
        DropPairUntilLanded(cursor);
    } else {
        gBoard.mPair.mSpeed = gBoard.mFallSpeed;
    }
    if (gSystemState.mKeysPressed & 1) {
        TurnPairClockwise(cursor);
    } else if (gSystemState.mKeysPressed & 2) {
        TurnPairCounterclockwise(cursor);
    }
    DrawFallingBlocks();
}

void LeaveFallingBlocks(void) {
    SetFaded(3);
    ReleaseGxPool();
}

int GetFallingBlocksScoreThreshold(u8 tableRow) {
    return GetFallingBlocksValue(tableRow, 6);
}
