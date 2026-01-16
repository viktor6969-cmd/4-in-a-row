#include <stdio.h>
#include <conio.h>
#include <time.h>
#include "Game_config.h"

static void clear_line(void) {printf("\x1b[2K");}

static void cursor_goto(int row, int col) {printf("\x1b[%d;%dH", row, col);}

static int cell_screen_row(int r) {return BOARD_TOP_ROW + 1 + r * CELL_H;}

static int cell_screen_col(int c) { return BOARD_LEFT_COL + 1 + c * CELL_W; }

static void draw_turn(int player) {
    cursor_goto(TURN_ROW, 1);
    clear_line();

    printf(ANSI_FG_CYAN "Game mode: "ANSI_FG_GREEN" PVP \n\n");
    printf(ANSI_FG_CYAN "Curently playing:");
    if (player == 1) printf(ANSI_FG_RED ANSI_BRIGHT "Player 1" ANSI_RESET);
    else             printf(ANSI_FG_YELLOW ANSI_BRIGHT "Player 2" ANSI_RESET);

    fflush(stdout);
}

static void draw_message(const char* msg) {
    cursor_goto(MSG_ROW, 1);
    clear_line();
    printf(ANSI_FG_GRAY "%s" ANSI_RESET, (msg) ? (msg) : ("")); // Print the message if exist
    fflush(stdout);
}

static void draw_board_frame_static(void) {
    /* Board frame is 1 + ROWS*2 lines tall */
    cursor_goto(BOARD_TOP_ROW, BOARD_LEFT_COL);

    /* Top border */
    printf(ANSI_FG_GRAY);
    printf("+");

    /* Board body */
    for (int c = 0; c < COLS; c++) printf("---+");
    /* Rows */
    for (int r = 0; r < ROWS; r++) {
        /* Cell line */
        cursor_goto(BOARD_TOP_ROW + 1 + r * 2, BOARD_LEFT_COL);
        printf(ANSI_FG_GRAY "|");
        for (int c = 0; c < COLS; c++) printf("   |");
        printf(ANSI_RESET);

        /* Separator */
        cursor_goto(BOARD_TOP_ROW + 2 + r * 2, BOARD_LEFT_COL);
        printf(ANSI_FG_GRAY "+");
        for (int c = 0; c < COLS; c++) printf("---+");
        printf(ANSI_RESET);
    }

    cursor_goto(ARROW_ROW + 3, 0);
    printf(ANSI_FG_GRAY "\n\nLEFT/RIGHT - move\n\nENTER/SPACE - drop chip\n\nr - reset\n\nESC - quit");
    fflush(stdout);
}

static void draw_cell(int board[ROWS][COLS], int r, int c) {
    int val = board[r][c];
    int sr = cell_screen_row(r);
    int sc = cell_screen_col(c);

    cursor_goto(sr, sc);

    if (val == 1) {
        printf(ANSI_FG_RED ANSI_BRIGHT " O " ANSI_RESET);
    }
    else if (val == 2) {
        printf(ANSI_FG_YELLOW ANSI_BRIGHT " O " ANSI_RESET);
    }
    else {
        printf(ANSI_DIM " . " ANSI_RESET);
    }
    fflush(stdout);
}

static void draw_all_cells(int board[ROWS][COLS]) {
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            draw_cell(board, r, c);
        }
    }
}

static void draw_arrow(int col, int on, int player) {
    int ac = BOARD_LEFT_COL + 2 + col * CELL_W;
    cursor_goto(ARROW_ROW, ac);

    if (!on) {
        printf(" ");
        fflush(stdout);
        return;
    }

    if (player == 1) printf(ANSI_FG_RED ANSI_BRIGHT "v" ANSI_RESET);
    else             printf(ANSI_FG_YELLOW ANSI_BRIGHT "v" ANSI_RESET);

    fflush(stdout);
}


/* =========================
Logic
========================= */
static int drop_chip(int board[ROWS][COLS], int col, int player) {
    for (int r = ROWS - 1; r >= 0; r--) {
        if (board[r][col] == 0) {
            board[r][col] = player;
            return r;
        }
    }
    return -1;
}

static int count_dir(int board[ROWS][COLS], int r, int c, int dr, int dc, int player) {
    int cnt = 0;
    r += dr; c += dc;
    while (r >= 0 && r < ROWS && c >= 0 && c < COLS && board[r][c] == player) {
        cnt++;
        r += dr; c += dc;
    }
    return cnt;
}

static int check_win(int board[ROWS][COLS], int r, int c, int player) {
    int horiz = 1 + count_dir(board, r, c, 0, -1, player) + count_dir(board, r, c, 0, 1, player);
    int vert = 1 + count_dir(board, r, c, -1, 0, player) + count_dir(board, r, c, 1, 0, player);
    int diag1 = 1 + count_dir(board, r, c, -1, -1, player) + count_dir(board, r, c, 1, 1, player);
    int diag2 = 1 + count_dir(board, r, c, -1, 1, player) + count_dir(board, r, c, 1, -1, player);

    return (horiz >= 4 || vert >= 4 || diag1 >= 4 || diag2 >= 4);
}

static int check_draw(int board[ROWS][COLS]) {
    for (int c = 0; c < COLS; c++) {
        if (board[0][c] == 0) return 0;
    }
    return 1;
}

static void animate_fall(int board[ROWS][COLS], int col, int to_row, int player) {
    /* Temporarily draw chip as it falls down the column */
    for (int r = 0; r <= to_row; r++) {
        /* draw falling chip at r */
        int sr = cell_screen_row(r);
        int sc = cell_screen_col(col);
        cursor_goto(sr, sc);

        if (player == 1) printf(ANSI_FG_RED ANSI_BRIGHT " O " ANSI_RESET);
        else             printf(ANSI_FG_YELLOW ANSI_BRIGHT " O " ANSI_RESET);

        fflush(stdout);
        delay_ms(125);

        /* erase if not final */
        if (r != to_row) {
            cursor_goto(sr, sc);
            printf(ANSI_DIM " . " ANSI_RESET);
            fflush(stdout);
        }
    }

    /* final cell draw uses board state */
    draw_cell(board, to_row, col);
}

static int ai_choose_column(int board[ROWS][COLS]){/* Find where to put the chip */
    int col;
    for (;;) {
        col = rand() % COLS;          // pick random column
        if (board[0][col] == 0) {     // column not full
            return col;
        }
    }
}

static int ai_choose_column_hard(int board[ROWS][COLS],int ai_player) {
    int human = (ai_player == 1) ? 2 : 1;

    /* 1) WIN NOW */
    for (int c = 0; c < COLS; c++) {
        if (board[0][c] != 0) continue;  // full column

        /* find landing row */
        int r;
        for (r = ROWS - 1; r >= 0; r--) {
            if (board[r][c] == 0) break;
        }
        if (r < 0) continue;

        board[r][c] = ai_player;
        if (check_win(board, r, c, ai_player)) {
            board[r][c] = 0;
            return c;
        }
        board[r][c] = 0;
    }

    /* 2) BLOCK HUMAN WIN */
    for (int c = 0; c < COLS; c++) {
        if (board[0][c] != 0) continue;

        int r;
        for (r = ROWS - 1; r >= 0; r--) {
            if (board[r][c] == 0) break;
        }
        if (r < 0) continue;

        board[r][c] = human;
        if (check_win(board, r, c, human)) {
            board[r][c] = 0;
            return c;
        }
        board[r][c] = 0;
    }

    /* 3) FALLBACK: center-ish preference, otherwise random valid */
    {
        int pref[COLS] = {3, 2, 4, 1, 5, 0, 6};  // center first
        for (int i = 0; i < COLS; i++) {
            int c = pref[i];
            if (board[0][c] == 0) return c;
        }
    }

    /* should not happen unless board full */
    return 0;
}
/* =========================
Public entry point
========================= */
int start_game(int mode) {
    int board[ROWS][COLS] = { 0 };
    int cursor_col = COLS / 2;
    int player = 1;

    clear_screen();
    draw_turn(player);
    draw_board_frame_static();
    draw_all_cells(board);

    /* arrow initial */
    draw_arrow(cursor_col, 1, player);
    draw_message(ANSI_FG_YELLOW "Good luck. Try not to embarrass yourself.(Press SPACE to start)");

    for (;;) {

        if (mode > 0 && player == 2) { // If we play in AI mode, and it's his turn
            int col = (mode == 1) ? (ai_choose_column(board)) : (ai_choose_column_hard(board,2));
            int row = drop_chip(board, col, player);

            // Move the user arrow to new location 
            draw_arrow(cursor_col, 0, player); 
            cursor_col = col; 
            draw_arrow(cursor_col, 1, player);

            // Drop the chip
            animate_fall(board, col, row, player);

            if (check_win(board, row, cursor_col, player)) {
                draw_turn(player);
                if (player == 1) draw_message(ANSI_FG_GREEN"You won! Press any key...");
                else             draw_message(ANSI_FG_GREEN"You lose... Press any key...");
                _getch();
                return player;
            }

            if (check_draw(board)) {
                draw_message(ANSI_FG_YELLOW "Draw! Press any key...");
                _getch();
                return 0;
            }

            player = 1;
            draw_turn(player);
            draw_arrow(cursor_col, 1, player);
            draw_message("");
            continue;
        }

        int k = read_key();

        if (k == K_ENTER) {
            int row = drop_chip(board, cursor_col, player);
            if (row == -1) {
                draw_message(ANSI_FG_RED "Column full. Pick another one." ANSI_RESET);
                continue;
            }

            /* animate fall (uses final row) */
            animate_fall(board, cursor_col, row, player);

            if (check_win(board, row, cursor_col, player)) {
                draw_turn(player);
                if (player == 1) draw_message(ANSI_FG_GREEN"Player 1 wins! Press any key...");
                else             draw_message(ANSI_FG_GREEN"Player 2 wins! Press any key...");
                _getch();
                return player;
            }

            if (check_draw(board)) {
                draw_message(ANSI_FG_YELLOW "Draw!(You both suck) Press any key...");
                _getch();
                return 0;
            }

            player = (player == 1) ? 2 : 1;
            draw_turn(player);
            draw_arrow(cursor_col, 1, player);
            draw_message("");
        }


        switch (k) {
            case K_LEFT:
                if (cursor_col > 0) {
                    draw_arrow(cursor_col, 0, player);
                    cursor_col--;
                    draw_arrow(cursor_col, 1, player);
                }
                break;

            case K_RIGHT:
                if (cursor_col < COLS - 1) {
                    draw_arrow(cursor_col, 0, player);
                    cursor_col++;
                    draw_arrow(cursor_col, 1, player);
                }
                break;

            case K_RESET: 
                for (int r = 0; r < ROWS; r++)
                    for (int c = 0; c < COLS; c++)
                        board[r][c] = 0;
                player = 1;
                cursor_col = COLS / 2;

                clear_screen();
                draw_turn(player);
                draw_board_frame_static();
                draw_all_cells(board);
                draw_arrow(cursor_col, 1, player);
                draw_message("The game has been reset.");
                break;

            case K_ESC:
                return -1;
        }
    }
    return -1;
}
