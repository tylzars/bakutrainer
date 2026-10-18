#include <nitro/os.h>
#include <Main/clib.h>
#include <Main/Unknown/save_blocks.h>
#include <Main/Unknown/random_angle.h>
#include <Main/Unknown/screen_fade.h>
#include <Main/Unknown/system_init.h>
#include <Main/Unknown/bg_image.h>
#include <Main/Unknown/prompt_screen.h>
#include <Main/Unknown/three_in_row.h>
#include <Main/Unknown/text_draw.h>
#include <Main/Unknown/texture_blocks.h>
#include <Main/Unknown/sprite_script.h>
#include <Main/Unknown/script_data.h>

u8 data_020b13c8[SHAPE_COUNT];
Unk_020b13d4 data_020b13d4;

const u8 data_020500d8[SHAPE_CELLS] = {0, 1, 2, 4, 5, 6, 8, 9, 10};
const u8 data_020500e4[GRID_BYTES] = {0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1};
const u8 data_020500f8[SHAPE_COUNT][SHAPE_CELLS] = {
    {1, 1, 0, 0, 0, 0, 0, 0, 0}, {1, 0, 0, 1, 0, 0, 0, 0, 0}, {1, 1, 1, 0, 0, 0, 0, 0, 0}, {1, 0, 0, 1, 0, 0, 1, 0, 0},
    {0, 1, 0, 1, 1, 0, 0, 0, 0}, {1, 0, 0, 1, 1, 0, 0, 0, 0}, {1, 1, 0, 1, 0, 0, 0, 0, 0}, {1, 1, 0, 0, 1, 0, 0, 0, 0},
    {1, 0, 0, 1, 1, 0, 1, 0, 0}, {1, 1, 1, 0, 1, 0, 0, 0, 0}, {0, 1, 0, 1, 1, 0, 0, 1, 0}, {0, 1, 0, 1, 1, 1, 0, 0, 0},
};

int func_02015340(u8 *boards, ShapeRec *recs) {
    int i, j, x, y, k;
    int placed;
    int n;
    u32 r;
    u8 *board;
    const u8 *t;
    u8 *cur;

    memcpy(boards, data_020500e4, GRID_BYTES);
    memcpy(boards + GRID_BYTES, boards, (GRID_PIECES - 1) * GRID_BYTES);
    for (i = 0; i < SHAPE_COUNT; i++) {
        int sum = 0;
        for (j = 0; j < SHAPE_CELLS; j++) {
            sum += data_020500f8[i][j];
        }
        data_020b13c8[i] = sum;
    }
    placed = 0;
    cur = boards;
    do {
        r = func_0201fd84(0, SHAPE_COUNT - 1);
        t = data_020500f8[r];
        for (y = 0; y < 4; y++) {
            for (x = 0; x < 3; x++) {
                board = boards + placed * GRID_BYTES + y * 4 + x;
                if (!((boards[placed * GRID_BYTES + y * 4 + x] & t[0]) | (board[1] & t[1]) | (board[2] & t[2]) |
                      (board[4] & t[3]) | (board[5] & t[4]) | (board[6] & t[5]) | (board[8] & t[6]) |
                      (board[9] & t[7]) | (board[10] & t[8]))) {
                    recs[placed].mUnk_00 = x * 32 + 0x54;
                    recs[placed].mUnk_02 = y * 32 + 0x20;
                    recs[placed].mUnk_04 = r;
                    recs[placed].mUnk_06 = data_020b13c8[r];
                    memcpy(boards + placed * GRID_BYTES + GRID_BYTES, cur, GRID_BYTES);
                    for (k = 0; k < SHAPE_CELLS; k++) {
                        if (t[k] != 0) {
                            board[data_020500d8[k] + GRID_BYTES] = (placed << 4) | CELL_FREE;
                        }
                    }
                    n = 0;
                    for (j = 0; j < 4; j++) {
                        for (k = 0; k < 3; k++) {
                            u8 *p = boards + placed * GRID_BYTES + GRID_BYTES + j * 4 + k;
                            n += *p & 0xf;
                            if (*p == 0 && p[1] != 0 && p[4] != 0 && (k == 0 || p[-1] != 0) && (j == 0 || p[-4] != 0)) {
                                goto reject;
                            }
                        }
                    }
                    placed++;
                    cur += GRID_BYTES;
                reject:
                    y = PIECE_GONE;
                    x = PIECE_GONE;
                }
            }
        }
    } while (n < SHAPE_COUNT);
    return placed;
}

int func_0201553c(int count, ShapeRec *recs, u8 *board, int player) {
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
            recs->mUnk_04 = PIECE_GONE;
            recs++;
        }
        memset(board, 0, GRID_BYTES);
        return 2;
    }
    for (i = 0; i < count; i++) {
        const u8 *t;
        u8 *p;
        if (recs[i].mUnk_04 == PIECE_GONE) {
            continue;
        }
        t = data_020500f8[recs[i].mUnk_04];
        p = board + ((recs[i].mUnk_00 - 0x54) / 32 + (recs[i].mUnk_02 - 0x20) / 32 * 4);
        for (k = 0; k < SHAPE_CELLS; k++) {
            if (t[k] == 1 && (p[data_020500d8[k]] & 0xf) < CELL_HUMAN) {
                break;
            }
        }
        if (k == SHAPE_CELLS) {
            for (k = 0; k < SHAPE_CELLS; k++) {
                if (t[k] == 1) {
                    p[data_020500d8[k]] = 0;
                }
            }
            recs[i].mUnk_04 = PIECE_GONE;
            return 1;
        }
    }
    return 0;
}

int func_020156c4(u8 *board, int player, ShapeRec *recs, int x, int y) {
    int hi = board[x + y * 4] >> 4;
    int row;
    int col;
    u8 *p;
    u8 c0;
    u8 c1;
    u8 c2;

    for (row = 0; row < 4; row++) {
        for (col = 0; col < 3; col++) {
            p = board + (col + row * 4);
            c0 = p[0];
            if ((c0 & 0xf) != player || !(c0 >> 4 == hi || recs[c0 >> 4].mUnk_06 != 0)) {
                continue;
            }
            c1 = p[1];
            c2 = p[2];
            if (col == 0 && (c1 & 0xf) == player && (c2 & 0xf) == player &&
                (c1 >> 4 == hi || recs[c1 >> 4].mUnk_06 != 0) && (c2 >> 4 == hi || recs[c2 >> 4].mUnk_06 != 0)) {
                return SCORE_WIN;
            }
            c1 = p[4];
            c2 = p[8];
            if (row < 2 && (c1 & 0xf) == player && (c2 & 0xf) == player &&
                (c1 >> 4 == hi || recs[c1 >> 4].mUnk_06 != 0) && (c2 >> 4 == hi || recs[c2 >> 4].mUnk_06 != 0)) {
                return SCORE_WIN;
            }
            c1 = p[5];
            c2 = p[10];
            if (col == 0 && row < 2 && (c1 & 0xf) == player && (c2 & 0xf) == player &&
                (c1 >> 4 == hi || recs[c1 >> 4].mUnk_06 != 0) && (c2 >> 4 == hi || recs[c2 >> 4].mUnk_06 != 0)) {
                return SCORE_WIN;
            }
            c1 = p[-3];
            c2 = p[-6];
            if (col == 0 && row >= 2 && (c1 & 0xf) == player && (c2 & 0xf) == player &&
                (c1 >> 4 == hi || recs[c1 >> 4].mUnk_06 != 0) && (c2 >> 4 == hi || recs[c2 >> 4].mUnk_06 != 0)) {
                return SCORE_WIN;
            }
        }
    }
    if (recs[hi].mUnk_06 == 0) {
        return SCORE_LOSS_PIECE;
    }
    return 0;
}

int func_02015850(ShapeRec *recs, u8 *board, int player, u32 depth) {
    int best = SCORE_NONE;
    int bestPos;
    int other;
    int x, y;
    int pos;
    int score;

    if (player == CELL_HUMAN) {
        other = CELL_COMPUTER;
    } else {
        other = CELL_HUMAN;
    }
    for (y = 0; y < 4; y++) {
        for (x = 0; x < 3; x++) {
            pos = x + y * 4;
            if ((board[pos] & 0xf) == CELL_FREE) {
                recs[board[pos] >> 4].mUnk_06--;
                board[pos] = (board[pos] & 0xf0) | player;
                score = func_020156c4(board, player, recs, x, y);
                if (depth < data_020b13d4.mUnk_00 && score < SCORE_WIN) {
                    if (score == 0) {
                        score -= func_02015850(recs, board, other, depth + 1);
                    } else {
                        score += func_02015850(recs, board, player, depth + 1);
                    }
                }
                if (score > best) {
                    best = score;
                    bestPos = pos;
                }
                recs[board[pos] >> 4].mUnk_06++;
                board[pos] = (board[pos] & 0xf0) | CELL_FREE;
            }
        }
    }
    if (depth == 0) {
        if (best == SCORE_NONE) {
            OS_Terminate();
        }
        data_020b13d4.mUnk_01 = bestPos;
    }
    if (best == SCORE_NONE) {
        best = 0;
    }
    return best;
}

void func_02015960(void) {
    data_020b13d4.mUnk_00 = 4;
    data_020b13d4.mUnk_01 = 0x40;
    data_020b13d4.mUnk_02 = 4;
    data_020b13d4.mUnk_03 = 0;
    data_020b13d4.mUnk_0c = 0;
    data_020b13d4.mUnk_48++;
    data_020b13d4.mUnk_4c.mUnk_de = 1;
    data_020b13d4.mUnk_44 = func_02015340(data_020b13d4.mUnk_40, data_020b13d4.mUnk_0e);
    memcpy(data_020b13d4.mUnk_40, data_020b13d4.mUnk_40 + data_020b13d4.mUnk_44 * GRID_BYTES, GRID_BYTES);
}

void func_020159ac(void) {
    Unk_020b13d4 *s = &data_020b13d4;
    int i;
    int player;
    int cell;
    int result;
    int x, y;

    s->mUnk_03 = 0;
    for (i = 0; i < 16; i++) {
        if ((i & 3) != 3 && (s->mUnk_40[i] & 0xf) == 0) {
            s->mUnk_03++;
        }
    }
    if (s->mUnk_03 == GRID_COLS * GRID_ROWS) {
        func_020215fc(10, 20, data_0201a6d0, "touche screen to continue", 0, 240, 16, 2);
        if (data_020c18bc.mUnk_2ac_0 != 1) {
            return;
        }
        func_02015960();
    }
    if (s->mUnk_01 & 0x40) {
        if (s->mUnk_02 == 0) {
            if (s->mUnk_48 & 1) {
                func_020215fc(10, 20, data_0201a6d0, "Ordi", 0, 240, 16, 2);
                func_02015850(s->mUnk_0e, s->mUnk_40, CELL_COMPUTER, 0);
            } else {
                func_020215fc(10, 20, data_0201a6d0, "Humain", 0, 240, 16, 2);
                if (data_020c18bc.mUnk_2ac_0 == 1) {
                    x = data_020c18bc.mUnk_2ae - 0x54;
                    y = data_020c18bc.mUnk_2af - 0x20;
                    cell = x / 32 + y / 32 * 4;
                    if (x >= 0 && x < 0x60 && y >= 0 && y < 0x80 && (s->mUnk_40[cell] & 0xf) == CELL_FREE) {
                        s->mUnk_01 = cell;
                    }
                }
            }
        } else {
            s->mUnk_02--;
        }
    }
    for (i = 0; i < s->mUnk_44; i++) {
        if (s->mUnk_0e[i].mUnk_04 != PIECE_GONE) {
            func_0202cf08(0xf90 + s->mUnk_0e[i].mUnk_04, s->mUnk_0e[i].mUnk_00, s->mUnk_0e[i].mUnk_02, 5, 2);
        }
    }
    for (y = 0; y < 4; y++) {
        for (x = 0; x < 3; x++) {
            switch (s->mUnk_40[y * 4 + x] & 0xf) {
                case 2:
                    func_0202cf08(0xf9d, x * 32 + 0x54, y * 32 + 0x20, 4, 2);
                    break;
                case 3:
                    func_0202cf08(0xf9c, x * 32 + 0x54, y * 32 + 0x20, 4, 2);
                    break;
            }
        }
    }
    for (i = 0; i < s->mUnk_0c; i++) {
        if (s->mUnk_04[i] == CELL_HUMAN) {
            func_020215fc(20, 30 + i * 12, data_0201a6d0, "H", 0, 240, 16, 2);
        } else {
            func_020215fc(20, 30 + i * 12, data_0201a6d0, "O", 0, 240, 16, 2);
        }
    }
    if (s->mUnk_01 < 0xf) {
        cell = s->mUnk_01 & 0xf;
        player = (s->mUnk_48 & 1) ? CELL_COMPUTER : CELL_HUMAN;
        func_0202de60(&s->mUnk_4c, (void *) (data_0201aa60 + 37)[player]);
        s->mUnk_01 |= 0x80;
        s->mUnk_40[cell] = (s->mUnk_40[cell] & 0xf0) | player;
    }
    if (s->mUnk_4c.mUnk_de == 1) {
        return;
    }
    cell = s->mUnk_01 & 0xf;
    func_0202cf08(func_0202db1c(&s->mUnk_4c), (cell & 3) * 32 + 0x54, (cell >> 2) * 32 + 0x20, 0, 2);
    if (s->mUnk_4c.mUnk_de == 1) {
        player = (s->mUnk_48 & 1) ? CELL_COMPUTER : CELL_HUMAN;
        s->mUnk_0e[s->mUnk_40[cell] >> 4].mUnk_06--;
        result = func_0201553c(s->mUnk_44, s->mUnk_0e, s->mUnk_40, player);
        if (result == 0) {
            s->mUnk_48++;
        } else {
            s->mUnk_04[s->mUnk_0c++] = player;
        }
        if (result != 2) {
            s->mUnk_01 |= 0x40;
        }
        s->mUnk_02 = 4;
    }
}

void func_02015cd4(void) {
    func_0202c1c4();
    func_0202bdd8(0x10cb, 0x10ca, 0x10cc, 0, 2);
    func_02020854(1);
    func_0201dde0(0xde8, 1);
    func_0201dde0(0xde7, 2);
    func_0201f830(2);
    data_020b13d4.mUnk_40 = func_0201c430(GRID_PIECES * GRID_BYTES);
    data_020b13d4.mUnk_48 = 0;
    func_02015960();
}

void func_02015d2c(void) {
    func_020159ac();
}

void func_02015d34(void) {
    func_0201c4f0(&data_020b13d4.mUnk_40);
    func_020102f8();
}

void func_02015d48(void) {}
