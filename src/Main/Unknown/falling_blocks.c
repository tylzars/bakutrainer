#include <Main/Unknown/random_angle.h>
#include <Main/Unknown/screen_fade.h>
#include <Main/Unknown/stage_info_panel.h>
#include <Main/Unknown/sprite_slots.h>
#include <Main/Unknown/game_util.h>
#include <Main/Unknown/move_object.h>
#include <Main/Unknown/sound_control.h>
#include <Main/Unknown/falling_blocks.h>
#include <Main/Unknown/mode_select.h>
#include <Main/Unknown/text_draw.h>
#include <Main/Unknown/model.h>
#include <Main/Unknown/wave_player.h>
#include <Main/Unknown/sprite_script.h>
#include <Main/Unknown/script_data.h>
#include <Main/Unknown/state_flow.h>
#include <Main/Unknown/system_init.h>
#include <Main/Unknown/gx_pool.h>
#include <nitro/os.h>
#include <stdio.h>

int func_02003194(int a, int b) {
    return func_02021e44(0xd66, b + a * 12);
}

void func_020031ac(s8 col, s8 row, Vec2i *out) {
    out->x = (col * 20 + 0x4d) << 8;
    out->y = (row * 20 - 4) << 8;
}

void func_020031c8(const Vec2i *pos, s8 *col, s8 *row) {
    *col = ((pos->x >> 8) - 0x4d) / 20;
    *row = ((pos->y >> 8) + 4) / 20;
}

u16 func_020031f0(void) {
    return (func_0201fd68() % 10 == 0) ? 0 : func_0201fd84(data_02064828.mUnk_896a, data_02064828.mUnk_896c);
}

void func_02003218(u8 *p) {
    data_02064828.mUnk_8968--;
    if (data_02064828.mUnk_8968 <= 0) {
        *p += 7;
        data_02064828.mUnk_8968 = func_020031f0();
    }
}

u16 func_02003240(void) {
    return func_0201fd84(data_02064828.mUnk_8970, data_02064828.mUnk_8972);
}

void func_02003258(u8 *p) {
    data_02064828.mUnk_896e--;
    if (data_02064828.mUnk_896e <= 0) {
        *p = 0xe;
        data_02064828.mUnk_896e = func_02003240();
    }
}

u8 func_0200327c(u8 lo, u8 hi) {
    int roll = func_0201fd84(1, 100);
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

void func_020032b4(Piece *p, u8 type) {
    p->mType = type;
    p->mUnk_110 = -1;
    if (type != 0) {
        u32 idx;

        if (type < 7) {
            idx = type + 12;
        } else {
            idx = type + 11;
        }
        func_0202de60(&p->mSprite, data_0201abec[(u16) idx]);
    }
}

void func_020032e4(Piece *p, int x, int y, int mode, int flag) {
    int prio;
    int frame;

    if (p->mType != 0) {
        if (mode == 1) {
            prio = 0x1ff;
        } else {
            prio = 0xff;
        }
        if (p->mUnk_110 >= 0) {
            prio -= 5;
        }
        frame = func_0202db1c(&p->mSprite);
        if (mode == 2) {
            frame++;
            if (flag == 0) {
                prio--;
            }
        }
        func_0202de80(frame, x, y, prio, -1, mode);
    }
}

void func_02003338(const Piece *src, u8 col, u8 row, int unused, Vec2i *out) {
    data_02064828.mGrid[col][row] = *src;
    func_020031ac(col, row, &data_02064828.mGrid[col][row].mPos);
    if (out != NULL) {
        out->x = col;
        out->y = row;
    }
    if (row == 0) {
        if (col == 2) {
            func_0200443c(1);
        } else {
            func_02003f08(col, row);
        }
    }
}

void func_020033ac(Piece *p) {
    if (p->mUnk_110 < 0 && p->mType != 0) {
        data_02064828.mUnk_430c++;
        p->mUnk_110 = 1;
        p->mSprite.mUnk_14 |= 1;
        if (p->mType < 7) {
            s32 left = data_02064828.mUnk_8978[p->mType - 1] - 1;

            if (left < 0) {
                left = 0;
            }
            data_02064828.mUnk_8978[p->mType - 1] = left;
        }
        func_0202f110(0x56, 0x4b, 0, 0x40, 0xffff, 0);
        if (p->mType < 7) {
            func_020059d0(&data_02064828.mUnk_10, data_02064828.mUnk_8990, 0);
        }
        data_02064828.mUnk_8994++;
    }
}

BOOL func_02003430(Piece *p) {
    s8 cell[2];

    p->mPos.y += 0x640;
    func_020031c8(&p->mPos, &cell[1], &cell[0]);
    if (cell[0] + 1 >= BOARD_ROWS || !func_02003ed8(cell[1], cell[0] + 1)) {
        func_02003338(p, cell[1], cell[0], 0, NULL);
        return TRUE;
    }
    return FALSE;
}

void func_02003488(Pair *pr) {
    if (pr->mUnk_240 > 0 && pr->mUnk_242 != 0) {
        pr->mUnk_240 = 0x12;
        pr->mUnk_242--;
    }
}

void func_020034ac(Pair *pr) {
    Piece tmp = pr->mPiece[0];

    pr->mPiece[0] = pr->mPiece[1];
    pr->mPiece[1] = tmp;
}

void func_020034f8(const Pair *pr, Vec2i *out) {
    *out = pr->mPos;
    switch (pr->mDir) {
        case 0:
            out->y -= 5 << 10;
            break;
        case 1:
            out->x += 5 << 10;
            break;
        case 2:
            out->y += 5 << 10;
            break;
        case 3:
            out->x -= 5 << 10;
            break;
    }
}

void func_0200355c(Pair *pr, Piece *unused) {
    s8 cell[2];

    if (pr->mUnk_240 < 0) {
        pr->mUnk_240 = 0x12;
        func_0202f110(0x57, 0x4b, 0, 0x40, 0xffff, 0);
    }
    func_020031c8(&pr->mPos, &cell[1], &cell[0]);
    func_020031ac(cell[1], cell[0], &pr->mPos);
}

void func_020035b0(Pair *pr) {
    Vec2i pos;
    s8 cell[2];

    if (pr->mUnk_234 == 0) {
        if (pr->mDir == 3) {
            func_020034f8(pr, &pos);
        } else {
            pos = pr->mPos;
        }
        func_020031c8(&pos, &cell[1], &cell[0]);
        if (pr->mDir == 2) {
            cell[0]++;
        }
        if (cell[1] > 0 && func_02003ed8(cell[1] - 1, cell[0])) {
            BOOL free;

            if (cell[0] >= BOARD_ROWS) {
                free = TRUE;
            } else {
                free = func_02003ed8(cell[1] - 1, cell[0] + 1);
            }
            if (free) {
                pr->mPos.x -= 5 << 10;
                func_02003488(pr);
            }
        }
        func_0202f110(0x10c, 0x4b, 0, 0x40, 0xffff, 0);
    }
}

void func_02003670(Pair *pr) {
    Vec2i pos;
    s8 cell[2];

    if (pr->mUnk_234 == 0) {
        if (pr->mDir == 1) {
            func_020034f8(pr, &pos);
        } else {
            pos = pr->mPos;
        }
        func_020031c8(&pos, &cell[1], &cell[0]);
        if (pr->mDir == 2) {
            cell[0]++;
        }
        if (cell[1] < BOARD_COLS - 1 && func_02003ed8(cell[1] + 1, cell[0])) {
            BOOL free;

            BOOL edge = cell[0] >= BOARD_ROWS || cell[0] <= 0;

            if (edge) {
                free = TRUE;
            } else {
                free = func_02003ed8(cell[1] + 1, cell[0] + 1);
            }
            if (free) {
                pr->mPos.x += 5 << 10;
                func_02003488(pr);
            }
        }
        func_0202f110(0x10c, 0x4b, 0, 0x40, 0xffff, 0);
    }
}

void func_0200373c(Pair *pr, u8 dir) {
    s8 cell[2];
    s8 row;
    s8 row1;
    s8 row2;

    func_020031c8(&pr->mPos, &cell[1], &cell[0]);
    row = cell[0];
    row1 = row < BOARD_ROWS - 1 ? row + 1 : row;
    row2 = row1 < BOARD_ROWS - 1 ? row1 + 1 : row1;
    switch (dir) {
        case 0:
            pr->mDir = dir;
            func_02003488(pr);
            return;
        case 1:
            if (cell[1] < BOARD_COLS - 1 && func_02003ed8(cell[1] + 1, row) && func_02003ed8(cell[1] + 1, row1)) {
                pr->mDir = dir;
                return;
            }
            if (cell[1] > 0 && func_02003ed8(cell[1] - 1, cell[0]) && func_02003ed8(cell[1] - 1, row1)) {
                pr->mDir = dir;
                pr->mPos.x -= 5 << 10;
                func_02003488(pr);
                return;
            }
            func_020034ac(pr);
            func_02003488(pr);
            return;
        case 2:
            if (row < BOARD_ROWS - 1 && func_02003ed8(cell[1], row1) && func_02003ed8(cell[1], row2)) {
                pr->mDir = dir;
                func_02003488(pr);
                return;
            }
            pr->mDir = dir;
            pr->mPos.y -= 5 << 10;
            func_02003488(pr);
            return;
        case 3:
            if (cell[1] > 0 && func_02003ed8(cell[1] - 1, cell[0]) && func_02003ed8(cell[1] - 1, row1)) {
                pr->mDir = dir;
                func_02003488(pr);
                return;
            }
            if (cell[1] < BOARD_COLS - 1 && func_02003ed8(cell[1] + 1, cell[0]) && func_02003ed8(cell[1] + 1, row1)) {
                pr->mDir = dir;
                pr->mPos.x += 5 << 10;
                func_02003488(pr);
                return;
            }
            func_020034ac(pr);
            func_02003488(pr);
            return;
    }
}

void func_02003924(Pair *pr) {
    if (pr->mUnk_234 == 0) {
        func_0200373c(pr, (pr->mDir + 1) % 4);
        func_0202f110(0x17b, 0x4b, 0, 0x40, 0xffff, 0);
    }
}

void func_0200396c(Pair *pr) {
    if (pr->mUnk_234 == 0) {
        int dir;

        if (pr->mDir == 0) {
            dir = 3;
        } else {
            dir = (pr->mDir - 1) % 4;
        }
        func_0200373c(pr, dir);
        func_0202f110(0x17b, 0x4b, 0, 0x40, 0xffff, 0);
    }
}

void func_020039bc(void) {
    Pair *pr = &data_02064828.mUnk_8608;
    u8 type[2];

    type[1] = func_0200327c(1, data_02064828.mUnk_01);
    func_02003218(&type[1]);
    func_02003258(&type[1]);
    type[0] = func_0200327c(1, data_02064828.mUnk_01);
    func_02003218(&type[0]);
    func_02003258(&type[0]);
    func_020032b4(&pr->mPiece[0], type[1]);
    func_020032b4(&pr->mPiece[1], type[0]);
    pr->mDir = 0;
    pr->mUnk_234 = 0;
    pr->mUnk_23c = data_02064828.mUnk_8964;
    pr->mUnk_240 = -1;
    pr->mUnk_242 = 3;
    pr->mPos.x = 0x37 << 10;
    pr->mPos.y = 0x45 << 8;
}

void func_02003a5c(void) {
    data_02064828.mUnk_83c4 = data_02064828.mUnk_8608;
    func_020039bc();
    func_020031ac(2, 0, &data_02064828.mUnk_83c4.mPos);
    data_02064828.mUnk_83c4.mPos.y -= 5 << 10;
    data_02064828.mUnk_8950 = 0;
}

void func_02003aa8(Pair *pr, int a, int b) {
    Vec2i pos;

    if (a == 2 || data_02064828.mUnk_8950 < 2) {
        func_020032e4(&pr->mPiece[0], pr->mPos.x >> 8, pr->mPos.y >> 8, 1, 0);
        if (pr->mUnk_234 == 0) {
            func_020034f8(pr, &pos);
            func_020032e4(&pr->mPiece[1], pos.x >> 8, pos.y >> 8, 1, 1);
            if (b != 0) {
                func_0202db1c(&data_02064828.mUnk_884c);
                func_0202de80(data_02064828.mUnk_884c.mUnk_04, pr->mPos.x >> 8, pr->mPos.y >> 8, 0xfa, -1, 1);
            }
        }
    }
}

BOOL func_02003b3c(Pair *pr) {
    BOOL result = FALSE;
    s32 speed;
    Vec2i partner;
    Vec2i swap;
    s8 cell[4];

    if (data_02064828.mUnk_8950 != 0 || data_02064828.mUnk_430c != 0 || data_02064828.mCount != 0) {
        return TRUE;
    }
    if (pr->mUnk_234 != 0) {
        speed = 0x19 << 12;
    } else {
        speed = pr->mUnk_23c;
    }
    pr->mPos.y += (speed << 4) / 1024;
    func_020034f8(pr, &partner);
    pr->mPiece[0].mPos = pr->mPos;
    pr->mPiece[1].mPos = partner;
    func_020031c8(&pr->mPos, &cell[3], &cell[2]);
    func_020031c8(&partner, &cell[1], &cell[0]);
    if (cell[2] < 0 || cell[0] < 0) {
        return FALSE;
    }
    if (cell[2] == BOARD_ROWS || cell[0] == BOARD_ROWS) {
        cell[2]--;
        cell[0]--;
    }
    if (pr->mUnk_240 > 0) {
        pr->mUnk_240--;
    }
    if (cell[2] + 1 >= BOARD_ROWS || (pr->mUnk_234 == 0 && cell[0] + 1 >= BOARD_ROWS)) {
        result = TRUE;
        if (pr->mUnk_240 == 0) {
            func_02003338(&pr->mPiece[0], cell[3], cell[2], 1, &data_02064828.mUnk_8954);
            if (pr->mUnk_234 == 0) {
                func_02003338(&pr->mPiece[1], cell[1], cell[0], 1, &data_02064828.mUnk_895c);
            } else {
                func_0202f110(0x57, 0x4b, 0, 0x40, 0xffff, 0);
            }
            data_02064828.mUnk_8950 = 1;
            return TRUE;
        }
        func_0200355c(pr, (pr->mUnk_234 != 0 || cell[2] + 1 >= BOARD_ROWS) ? &pr->mPiece[0] : &pr->mPiece[1]);
    }
    if (pr->mUnk_234 == 0 && !func_02003ed8(cell[1], cell[0] + 1)) {
        result = TRUE;
        if (pr->mUnk_240 == 0) {
            func_02003338(&pr->mPiece[1], cell[1], cell[0], 1, &data_02064828.mUnk_895c);
            if (pr->mDir == 3 || pr->mDir == 1) {
                pr->mUnk_234 = 1;
                pr->mUnk_238 = data_020c18bc.mUnk_45c;
                pr->mUnk_240 = 0;
            } else {
                func_02003338(&pr->mPiece[0], cell[3], cell[2], 1, &data_02064828.mUnk_8954);
                data_02064828.mUnk_8950 = result;
                return result;
            }
        } else {
            func_0200355c(pr, &pr->mPiece[1]);
        }
    }
    if (!func_02003ed8(cell[3], cell[2] + 1)) {
        result = TRUE;
        if (pr->mUnk_240 == 0) {
            func_02003338(&pr->mPiece[0], cell[3], cell[2], 1, &data_02064828.mUnk_8954);
            if (pr->mUnk_234 == 0) {
                if (pr->mDir == 3 || pr->mDir == 1) {
                    pr->mUnk_234 = 1;
                    pr->mUnk_238 = data_020c18bc.mUnk_45c;
                    pr->mUnk_240 = 0;
                    pr->mPiece[0] = pr->mPiece[1];
                    pr->mPos = partner;
                    swap = data_02064828.mUnk_8954;
                    data_02064828.mUnk_8954 = data_02064828.mUnk_895c;
                    data_02064828.mUnk_895c = swap;
                } else {
                    func_02003338(&pr->mPiece[1], cell[1], cell[0], 1, &data_02064828.mUnk_895c);
                    data_02064828.mUnk_8950 = result;
                    return result;
                }
            } else {
                data_02064828.mUnk_8950 = result;
                if (data_020c18bc.mUnk_45c - pr->mUnk_238 > 1) {
                    func_0202f110(0x57, 0x4b, 0, 0x40, 0xffff, 0);
                }
                return TRUE;
            }
        } else {
            func_0200355c(pr, pr->mPiece);
        }
    }
    return result;
}

void func_02003e50(Pair *pr) {
    do {
    } while (!func_02003b3c(pr));
}

BOOL func_02003e60(void) {
    if (data_02064828.mUnk_430c == 0 && data_02064828.mCount == 0) {
        return TRUE;
    }
    return FALSE;
}

void func_02003e80(const Piece *src) {
    int i;

    for (i = 0; i < PIECE_LIST_SIZE; i++) {
        if (data_02064828.mList[i].mType == 0) {
            data_02064828.mList[i] = *src;
            data_02064828.mCount++;
            return;
        }
    }
}

BOOL func_02003ed8(u8 col, u8 row) {
    if (data_02064828.mGrid[col][row].mType == 0) {
        return TRUE;
    }
    return FALSE;
}

void func_02003f08(u8 col, u8 row) {
    data_02064828.mGrid[col][row].mType = 0;
}

// Starting contents of the board for each level, rows top to bottom, columns left to right.
const u8 data_0204e054[2][BOARD_ROWS][BOARD_COLS] = {
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

void func_02003f2c(int level) {
    int col;
    int row;

    for (col = 0; col < BOARD_COLS; col++) {
        for (row = 0; row < BOARD_ROWS; row++) {
            func_020032b4(&data_02064828.mGrid[col][row], data_0204e054[level][row][col]);
            func_020031ac(col, row, &data_02064828.mGrid[col][row].mPos);
        }
    }
}

void func_02003fac(u8 type) {
    int col;
    int row;

    for (col = 0; col < BOARD_COLS; col++) {
        for (row = 0; row < BOARD_ROWS; row++) {
            if (type == data_02064828.mGrid[col][row].mType) {
                func_020033ac(&data_02064828.mGrid[col][row]);
            }
        }
    }
}

void func_0200400c(u8 col, u8 row) {
    Piece *column = data_02064828.mGrid[col];

    if (column[row].mType != 0 && row < BOARD_ROWS - 1 && column[row + 1].mType == 0) {
        func_02003e80(&column[row]);
        func_02003f08(col, row);
    }
}

BOOL func_02004050(u8 col, u8 row, u8 col2, u8 row2) {
    Piece *a = &data_02064828.mGrid[col][row];
    Piece *b = &data_02064828.mGrid[col2][row2];

    if (a->mType == 0 || b->mType == 0) {
        return FALSE;
    }
    if (a->mUnk_110 > 0 && a->mSprite.mUnk_04 == data_02064828.mUnk_8974 && b->mType == a->mType && b->mUnk_110 < 0) {
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

void func_020040e8(u8 col, u8 row) {
    Piece *cell = &data_02064828.mGrid[col][row];

    if (cell->mType != 0) {
        if (cell->mType == 14) {
            if (row < BOARD_ROWS - 1) {
                u8 x = 0;
                u8 t = data_02064828.mGrid[col][row + 1].mType;

                if (t < 7) {
                    x = t;
                } else if (t > 7) {
                    x = t - 7;
                }
                func_02003fac(x);
                func_020033ac(cell);
                return;
            }
            func_020033ac(cell);
            return;
        }
        if (col != 0) {
            int c1 = col - 1;

            if (func_02004050(col, row, c1, row)) {
                func_020033ac(cell);
                func_020033ac(&data_02064828.mGrid[c1][row]);
            }
        }
        if (col < BOARD_COLS - 1) {
            int c1 = col + 1;

            if (func_02004050(col, row, c1, row)) {
                func_020033ac(cell);
                func_020033ac(&data_02064828.mGrid[c1][row]);
            }
        }
        if (row != 0) {
            int r1 = row - 1;

            if (func_02004050(col, row, col, r1)) {
                func_020033ac(cell);
                func_020033ac(&data_02064828.mGrid[col][r1]);
            }
        }
        if (row < BOARD_ROWS - 1) {
            int r1 = row + 1;

            if (func_02004050(col, row, col, r1)) {
                func_020033ac(cell);
                func_020033ac(&data_02064828.mGrid[col][r1]);
            }
        }
    }
}

void func_0200420c(void) {
    int col;
    int row;

    for (col = 0; col < BOARD_COLS; col++) {
        for (row = BOARD_ROWS - 1; row > 0; row--) {
            func_020040e8(col, row);
        }
    }
}

void func_02004230(void) {
    int col;
    int row;

    for (col = 0; col < BOARD_COLS; col++) {
        for (row = 0; row < BOARD_ROWS; row++) {
            func_0200400c(col, row);
        }
    }
}

// Redraws every cell and sprite at its position.
void func_02004254(void) {
    int i, j;
    for (i = 0; i < BOARD_COLS; i++) {
        for (j = 0; j < BOARD_ROWS; j++) {
            func_020032e4(&data_02064828.mGrid[i][j], data_02064828.mGrid[i][j].mPos.x >> 8,
                          data_02064828.mGrid[i][j].mPos.y >> 8, 1, 0);
        }
    }
    if (data_02064828.mCount != 0) {
        for (j = 0; j < PIECE_LIST_SIZE; j++) {
            func_020032e4(&data_02064828.mList[j], data_02064828.mList[j].mPos.x >> 8,
                          data_02064828.mList[j].mPos.y >> 8, 1, 0);
        }
    }
}

void func_02004304(void) {
    int g, k;
    if (data_02064828.mCount != 0) {
        int i;
        for (i = 0; i < PIECE_LIST_SIZE; i++) {
            if (data_02064828.mList[i].mType != 0 && func_02003430(&data_02064828.mList[i])) {
                func_020032b4(&data_02064828.mList[i], 0);
                if (--data_02064828.mCount == 0) {
                    func_02004230();
                    if (data_02064828.mCount == 0) {
                        func_0200420c();
                    }
                    return;
                }
            }
        }
    } else {
        for (g = 0; g < BOARD_COLS; g++) {
            u8 row = g;
            for (k = BOARD_ROWS - 1; k >= 0; k--) {
                if (data_02064828.mGrid[g][k].mType != 0) {
                    Piece *c = &data_02064828.mGrid[g][k];
                    if (c->mUnk_110 > 0) {
                        if (c->mSprite.mUnk_de != 0) {
                            c->mUnk_110 = 0;
                        }
                        if (c->mSprite.mUnk_04 == data_02064828.mUnk_8974) {
                            func_020040e8(row, k);
                        } else if (c->mUnk_110 == 0) {
                            func_02003f08(row, k);
                            data_02064828.mUnk_430c--;
                        }
                    } else if (data_02064828.mUnk_430c == 0) {
                        func_0200400c(row, k);
                    }
                }
            }
        }
    }
}

void func_0200443c(int a) {
    func_02005a8c(0, data_02064828.mUnk_10 >= data_02064828.mUnk_14);
}

// Starts the board screen: loads the stage settings, clears the cells and sprites and sets the counters.
void func_0200445c(void) {
    int i, j;
    s32 level;
    int total;
    int stagger;
    level = data_020b6ee0.mUnk_a8e4;
    if (level < 0) {
        level = 0;
    }
    data_02064828.mStage = data_020b1990[level].mInfo;
    data_02064828.mUnk_00 = data_02064828.mStage.mUnk_11 - 1;
    if (data_020b10fc.mUnk_08 < 0 || data_02064828.mUnk_00 >= 12) {
        OS_Terminate();
    }
    func_0200c5d8(0, data_02064828.mStage.mUnk_10 + 0x1cb, data_02064828.mStage.mUnk_13 + 0xdb3, 64, 16, 2, 130);
    func_02024208();
    func_020104c0(3, 76);
    func_0200bdcc(2, 16);
    for (i = 0; i < BOARD_COLS; i++) {
        for (j = 0; j < BOARD_ROWS; j++) {
            func_020032b4(&data_02064828.mGrid[i][j], 0);
        }
    }
    data_02064828.mUnk_4300 = ~13;
    data_02064828.mUnk_42fc = 67;
    data_02064828.mUnk_4308 = 186;
    data_02064828.mUnk_4304 = 187;
    data_02064828.mUnk_430c = 0;
    for (i = 0; i < PIECE_LIST_SIZE; i++) {
        func_020032b4(&data_02064828.mList[i], 0);
    }
    data_02064828.mCount = 0;
    data_02064828.mUnk_8974 = 3738;
    func_0202de60(&data_02064828.mUnk_884c, data_0201abec[26]);
    data_02064828.mUnk_01 = func_02003194(data_02064828.mUnk_00, 2);
    stagger = func_02003194(data_02064828.mUnk_00, 3);
    data_02064828.mUnk_08 = data_02064828.mUnk_04 = func_02003194(data_02064828.mUnk_00, 5) * 60000 / 1000;
    data_02064828.mUnk_0c = -12800;
    data_02064828.mUnk_10 = 0;
    data_02064828.mUnk_14 = func_02003194(data_02064828.mUnk_00, 6);
    data_02064828.mUnk_18 = 0;
    data_02064828.mUnk_896a = func_02003194(data_02064828.mUnk_00, 8);
    data_02064828.mUnk_896c = func_02003194(data_02064828.mUnk_00, 9);
    data_02064828.mUnk_8968 = func_020031f0();
    data_02064828.mUnk_8970 = func_02003194(data_02064828.mUnk_00, 10);
    data_02064828.mUnk_8972 = func_02003194(data_02064828.mUnk_00, 11);
    data_02064828.mUnk_896e = func_02003240();
    data_02064828.mUnk_8964 = func_02003194(data_02064828.mUnk_00, 4) << 8;
    data_02064828.mUnk_8990 = func_02003194(data_02064828.mUnk_00, 7);
    data_02064828.mUnk_8994 = 0;
    total = func_0201fd84(func_02003194(data_02064828.mUnk_00, 12), func_02003194(data_02064828.mUnk_00, 13));
    for (i = 0; i < 6; i++) {
        if (i < data_02064828.mUnk_01 - 1) {
            data_02064828.mUnk_8978[i] = func_0201fd84(total / 8, total / 2);
            total -= data_02064828.mUnk_8978[i];
        } else if (i == data_02064828.mUnk_01 - 1) {
            data_02064828.mUnk_8978[i] = total;
        } else {
            data_02064828.mUnk_8978[i] = -1;
        }
    }
    func_020039bc();
    if (stagger > 0) {
        func_02003f2c((u8) ((stagger - 1 < 1 ? stagger - 1 : 1) < 0 ? 0 : (stagger - 1 < 1 ? stagger - 1 : 1)));
    }
    func_020031ac(2, 0, &data_02064828.mUnk_83c4.mPos);
    data_02064828.mUnk_83c4.mPos.y -= 5 << 10;
    func_02005a8c(1, 0);
}

void func_02004720(void) {
    func_0201168c(data_020b6ee0.mUnk_a8e4, data_02064828.mUnk_10, data_0206d448, data_02064828.mUnk_08, 2);
    func_02003aa8(&data_02064828.mUnk_8608, 2, 0);
    func_02004254();
    func_02003aa8(&data_02064828.mUnk_83c4, 1, 1);
}

// Per-frame update of the board screen.
void func_02004768(void) {
    static s32 data_02064820;
    char buf[32];
    int r;
    int idle;
    Pair *cursor = &data_02064828.mUnk_83c4;
    func_020059cc(data_02064828.mUnk_10);
    r = func_02005bf8();
    if (r != 0) {
        func_02004720();
        idle = func_02005b1c(1);
        if (r == 2 && idle == 0) {
            func_020059b4(func_02005c04(), data_02064828.mUnk_10);
            return;
        }
        if (r == 1) {
            data_02064820 = 0;
            return;
        }
        return;
    }
    data_02064828.mUnk_0c += 0x355;
    if (data_02064828.mUnk_0c > 0x2000) {
        data_02064828.mUnk_0c = 0x2000;
    }
    if (data_02064820 == 0) {
        data_02064820 = 1;
        func_02003a5c();
    }
    if (--data_02064828.mUnk_08 == 0) {
        func_0200443c(1);
    }
    func_02003b3c(cursor);
    if (data_02064828.mUnk_8950 == 1) {
        data_02064828.mUnk_8994 = 0;
        func_020040e8(data_02064828.mUnk_8954.x, data_02064828.mUnk_8954.y);
        func_020040e8(data_02064828.mUnk_895c.x, data_02064828.mUnk_895c.y);
        data_02064828.mUnk_8950++;
    } else if (data_02064828.mUnk_8950 == 2) {
        if (func_02003e60()) {
            if (data_02064828.mUnk_8994 >= 6) {
                func_020059e8(&data_02064828.mUnk_08, 300);
                sprintf(buf, "+%d'", 5);
                func_0200d838(buf, 2, data_0201a61c, 30, data_02064828.mUnk_83c4.mPos.x >> 8,
                              data_02064828.mUnk_83c4.mPos.y >> 8, 0, -128, 100, 1, 0);
            }
            data_02064828.mUnk_8994 = 0;
            func_02003a5c();
        }
    }
    func_02004304();
    if (data_020c18bc.mUnk_12 & 0x20) {
        func_020035b0(cursor);
    } else if (data_020c18bc.mUnk_12 & 0x10) {
        func_02003670(cursor);
    }
    if (data_020c18bc.mUnk_10 & 0x80) {
        data_02064828.mUnk_83c4.mUnk_23c = 25 << 12;
    } else if (data_020c18bc.mUnk_12 & 0x40) {
        func_02003e50(cursor);
    } else {
        data_02064828.mUnk_83c4.mUnk_23c = data_02064828.mUnk_8964;
    }
    if (data_020c18bc.mUnk_12 & 1) {
        func_02003924(cursor);
    } else if (data_020c18bc.mUnk_12 & 2) {
        func_0200396c(cursor);
    }
    func_02004720();
}

void func_02004930(void) {
    func_0201f880(3);
    func_02022370();
}

int func_02004940(u8 stage) {
    return func_02003194(stage, 6);
}
