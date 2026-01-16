#include <stdio.h>
#include <conio.h>
#include <time.h>
#include "Game_config.h"


// ===== UI helper functions ======

// ------ Line clearing ----
static void clear_line(void) {
    // Clears the current console line
    printf("\x1b[2K");
}

// ------ Cursor movements ----
static void cursor_goto(int row, int col) {   // { row - needed row, col - needed column }
    // Moves the cursor to board [row, col]
    printf("\x1b[%d;%dH", row, col);
}

// ------ Board cell to screen coordinate mapping ----
static int cell_screen_row(int r) {   // { r - board row index }
    // Converts board row index to screen row position
    return BOARD_TOP_ROW + 1 + r * CELL_H;
}

static int cell_screen_col(int c) {   // { c - board column index }
    // Converts board column index to screen column position
    return BOARD_LEFT_COL + 1 + c * CELL_W;
}

// ------ UI status line rendering ----
static void draw_turn(int player) {   // { player - current player (1/2) }
    // Prints current player's turn
    cursor_goto(TURN_ROW + 1, 1);
    clear_line();

    printf(ANSI_FG_CYAN "Currently playing: ");
    if (player == 1) printf(ANSI_FG_RED ANSI_BRIGHT "Player 1" ANSI_RESET);
    else             printf(ANSI_FG_YELLOW ANSI_BRIGHT "Player 2" ANSI_RESET);

    fflush(stdout);
}

static void draw_message(const char* msg) {   // { msg - message to print (can be NULL) }
    // Prints a message line (empty if msg is NULL)
    cursor_goto(MSG_ROW, 1);
    clear_line();

    printf(ANSI_FG_GRAY "%s" ANSI_RESET, (msg) ? (msg) : (""));
    fflush(stdout);
}

// ------ Board frame rendering ----
static void draw_board_frame_static(int mode) {   // { mode - 0 PvP, 1 AI EZ, 2 AI HARD }
    // Draws the board frame and the controls text (static UI)

    char* mode_name;

    if (!mode) mode_name = "\x1b[32mPvP";
    else mode_name = (mode == 1) ? ("\x1b[37mAI lvl \x1b[33mEZ") : ("\x1b[37mAI lvl \x1b[31mHARD");

    cursor_goto(TURN_ROW, 1);
    printf(ANSI_FG_CYAN "Game mode: %s \n\n", mode_name);

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
    printf(ANSI_FG_GRAY "\nLEFT/RIGHT - move\n\nENTER/SPACE - drop chip\n\nr - reset\n\nESC - quit");
    fflush(stdout);
}

// ------ Board cells rendering ----
static void draw_cell(int board[ROWS][COLS], int r, int c) {   // { board - board matrix, r - row, c - col }
    // Draws a single cell based on board value (0 empty, 1 P1, 2 P2)

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

static void draw_all_cells(int board[ROWS][COLS]) {   // { board - board matrix }
    // Draws all board cells (full refresh of chips)
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            draw_cell(board, r, c);
        }
    }
}

// ------ Arrow / cursor rendering ----
static void draw_arrow(int col, int on, int player) {   // { col - column index, on - 1 draw / 0 erase, player - 1/2 }
    // Draws or clears the "v" arrow above the selected column

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

/*=======*/


// ===== Game logic functions ======

// ------ Chip dropping ----
static int drop_chip(int board[ROWS][COLS], int col, int player) {   // { board - board matrix, col - chosen column, player - 1/2 }
    // Drops a chip into the requested column. Returns landing row, or -1 if column is full.
    for (int r = ROWS - 1; r >= 0; r--) {
        if (board[r][col] == 0) {
            board[r][col] = player;
            return r;
        }
    }
    return -1;
}

// ------ Win detection helpers ----
static int count_dir(int board[ROWS][COLS], int r, int c, int dr, int dc, int player) {   // { dr/dc - direction step }
    // Counts same-player chips in a direction from (r,c) (excluding starting cell)
    int cnt = 0;
    r += dr; c += dc;
    while (r >= 0 && r < ROWS && c >= 0 && c < COLS && board[r][c] == player) {
        cnt++;
        r += dr; c += dc;
    }
    return cnt;
}

static int check_win(int board[ROWS][COLS], int r, int c, int player) {   // { r/c - last placed position }
    // Checks if the last move at (r,c) created a 4-in-a-row
    int horiz = 1 + count_dir(board, r, c, 0, -1, player) + count_dir(board, r, c, 0, 1, player);
    int vert = 1 + count_dir(board, r, c, -1, 0, player) + count_dir(board, r, c, 1, 0, player);
    int diag1 = 1 + count_dir(board, r, c, -1, -1, player) + count_dir(board, r, c, 1, 1, player);
    int diag2 = 1 + count_dir(board, r, c, -1, 1, player) + count_dir(board, r, c, 1, -1, player);

    return (horiz >= 4 || vert >= 4 || diag1 >= 4 || diag2 >= 4);
}

static int check_draw(int board[ROWS][COLS]) {   // { board - board matrix }
    // Returns 1 if the board is full (draw), else 0
    for (int c = 0; c < COLS; c++) {
        if (board[0][c] == 0) return 0;
    }
    return 1;
}

/*=======*/


// ===== Game animation functions ======

// ------ Falling chip animation ----
static void animate_fall(int board[ROWS][COLS], int col, int to_row, int player) {   // { col - column, to_row - final row, player - 1/2 }
    // Temporarily draws a falling chip until it reaches the final row

    for (int r = 0; r <= to_row; r++) {
        int sr = cell_screen_row(r);
        int sc = cell_screen_col(col);
        cursor_goto(sr, sc);

        if (player == 1) printf(ANSI_FG_RED ANSI_BRIGHT " O " ANSI_RESET);
        else             printf(ANSI_FG_YELLOW ANSI_BRIGHT " O " ANSI_RESET);

        fflush(stdout);
        delay_ms(125);

        if (r != to_row) {
            cursor_goto(sr, sc);
            printf(ANSI_DIM " . " ANSI_RESET);
            fflush(stdout);
        }
    }

    /* Final cell draw uses board state */
    draw_cell(board, to_row, col);
}

/*=======*/


// ===== AI functions ======

// ------ AI random (EZ mode) ----
static int ai_choose_column(int board[ROWS][COLS]) {   // { board - board matrix }
    // Finds a random non-full column
    int col;
    for (;;) {
        col = rand() % COLS;
        if (board[0][col] == 0) {
            return col;
        }
    }
}

// ------ AI hard mode ----
static int ai_choose_column_hard(int board[ROWS][COLS], int ai_player) {   // { board - board matrix, ai_player - AI player id (1/2) }
    // Hard AI: win if possible, block human win, otherwise prefer center columns

    int human = (ai_player == 1) ? 2 : 1;

    /* 1) WIN NOW */
    for (int c = 0; c < COLS; c++) {
        if (board[0][c] != 0) continue;

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
        int pref[COLS] = { 3, 2, 4, 1, 5, 0, 6 };
        for (int i = 0; i < COLS; i++) {
            int c = pref[i];
            if (board[0][c] == 0) return c;
        }
    }

    return 0;
}

/*=======*/


// ===== Game entry point ======

// ------ Start game loop ----
int start_game(int mode) {   // { mode - 0 PvP, 1 AI EZ, 2 AI HARD }
    // Main game loop. Returns: -1 (quit), 0 (draw), 1 (player 1 win), 2 (player 2 win)

    int board[ROWS][COLS] = { 0 };
    int cursor_col = COLS / 2;
    int player = 1;

    clear_screen();
    draw_turn(player);
    draw_board_frame_static(mode);
    draw_all_cells(board);

    /* Arrow initial */
    draw_arrow(cursor_col, 1, player);

    draw_message(ANSI_FG_YELLOW "Good luck. Try not to embarrass yourself. (Press SPACE to start)");

    for (;;) {

        // ------ AI turn handling ----
        if (mode > 0 && player == 2) {
            int col = (mode == 1) ? (ai_choose_column(board)) : (ai_choose_column_hard(board, 2));
            int row = drop_chip(board, col, player);

            draw_arrow(cursor_col, 0, player);
            cursor_col = col;
            draw_arrow(cursor_col, 1, player);

            animate_fall(board, col, row, player);

            if (check_win(board, row, cursor_col, player)) {
                draw_turn(player);
                if (player == 1) draw_message(ANSI_FG_GREEN "You won! Press any key...");
                else             draw_message(ANSI_FG_GREEN "You lose... Press any key...");
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

        // ------ Human input handling ----
        int k = read_key();

        if (k == K_ENTER) {
            int row = drop_chip(board, cursor_col, player);
            if (row == -1) {
                draw_message(ANSI_FG_RED "Column full. Pick another one." ANSI_RESET);
                continue;
            }

            animate_fall(board, cursor_col, row, player);

            if (check_win(board, row, cursor_col, player)) {
                draw_turn(player);
                if (player == 1) draw_message(ANSI_FG_GREEN "Player 1 wins! Press any key...");
                else             draw_message(ANSI_FG_GREEN "Player 2 wins! Press any key...");
                _getch();
                return player;
            }

            if (check_draw(board)) {
                draw_message(ANSI_FG_YELLOW "Draw! (You both suck) Press any key...");
                _getch();
                return 0;
            }

            player = (player == 1) ? 2 : 1;
            draw_turn(player);
            draw_arrow(cursor_col, 1, player);
            draw_message("");
        }

        // ------ Movement / control keys ----
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
            draw_board_frame_static(mode);
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

/*=======*/
