/*
    main.c - Minimal Connect 4 (Console, Windows)
    ---------------------------------------------
    Requirements met:
    - Simple menu: PvP, AI Easy, AI Hard, Score, Exit
    - Simple ASCII graphics (no ANSI, no animations)
    - Reset option during a game: press R
    - Quit to menu during a game: press Q
    - system("cls") used for clearing the screen
    - switch statements used for mode selection and actions
    - No structs (scores are plain ints)

    Controls:
    Menu:
      - Press 1..5

    In game:
      - Press 1..7 to drop in a column
      - Press R to reset the current round
      - Press Q to quit to menu

    Build (MSVC):
      cl main.c

    Build (MinGW):
      gcc main.c -o connect4.exe
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>   /* _getch() */
#include "Config.h"

/* ------------------------- UI Helpers ------------------------- */

/*
    clear_screen:
    Clears the console using system("cls").
    Windows-only but simple and matches your requirement.
*/
static void clear_screen(void) {
    system("cls");
}

/*
    press_any_key:
    Pauses execution until the user presses a key.
    (void)_getch() explicitly ignores the returned key value.
*/
static void press_any_key(void) {
    printf("\nPress any key...");
    (void)_getch();
}

/*
    print_menu:
    Prints the main menu options.
*/
static void print_menu(void) {
    clear_screen();
    puts("CONNECT 4\n");
    puts("1) Game PvP");
    puts("2) Game vs AI (easy)");
    puts("3) Game vs AI (hard)");
    puts("4) Score");
    puts("5) Exit");
    puts("\nChoose [1-5]...");
}

/*
    read_menu_choice:
    Reads menu selection from keyboard using _getch().
    Returns:
      - integer 1..5
*/
static int read_menu_choice(void) {
    for (;;) {
        int k = _getch();
        if (k >= '1' && k <= '5') return (k - '0');
    }
}

/* ------------------------- Board Ops ------------------------- */

/*
    board_reset:
    Sets every cell in board to CELL_EMPTY (0).
    Parameters:
      b - game board array [ROWS][COLS]
*/
static void board_reset(int b[ROWS][COLS]) {
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            b[r][c] = CELL_EMPTY;
        }
    }
}

/*
    board_print:
    Prints board with '.' for empty, 'X' for player 1, 'O' for player 2.
*/
static void board_print(int b[ROWS][COLS]) {
    puts("");
    puts("  1 2 3 4 5 6 7");
    for (int r = 0; r < ROWS; r++) {
        putchar('|');
        for (int c = 0; c < COLS; c++) {
            char ch = '.';
            if (b[r][c] == PLAYER_1) ch = 'X';
            else if (b[r][c] == PLAYER_2) ch = 'O';
            putchar(ch);
            putchar('|');
        }
        putchar('\n');
    }
    puts(" ---------------");
}

/*
    col_full:
    Returns 1 if column is full (top cell not empty), else 0.
*/
static int col_full(int b[ROWS][COLS], int col) {
    return b[0][col] != CELL_EMPTY;
}

/*
    drop_chip:
    Drops chip into column:
      - finds lowest empty cell in that column
      - sets it to 'player'
    Returns:
      - landing row index (0..ROWS-1) on success
      - -1 if column full
*/
static int drop_chip(int b[ROWS][COLS], int col, int player) {
    for (int r = ROWS - 1; r >= 0; r--) {
        if (b[r][col] == CELL_EMPTY) {
            b[r][col] = player;
            return r;
        }
    }
    return -1;
}

/*
    count_dir:
    Counts consecutive pieces for player p in one direction (dr,dc)
    starting adjacent to (r,c).
*/
static int count_dir(int b[ROWS][COLS], int r, int c, int dr, int dc, int p) {
    int cnt = 0;
    r += dr; c += dc;
    while (r >= 0 && r < ROWS && c >= 0 && c < COLS && b[r][c] == p) {
        cnt++;
        r += dr; c += dc;
    }
    return cnt;
}

/*
    check_win:
    Checks whether last move at (r,c) by player p created 4-in-a-row.
    Directions checked:
      - horizontal, vertical, both diagonals
    Returns 1 if win, else 0.
*/
static int check_win(int b[ROWS][COLS], int r, int c, int p) {
    int h = 1 + count_dir(b, r, c, 0, -1, p) + count_dir(b, r, c, 0, 1, p);
    int v = 1 + count_dir(b, r, c, -1, 0, p) + count_dir(b, r, c, 1, 0, p);
    int d1 = 1 + count_dir(b, r, c, -1, -1, p) + count_dir(b, r, c, 1, 1, p);
    int d2 = 1 + count_dir(b, r, c, -1, 1, p) + count_dir(b, r, c, 1, -1, p);
    return (h >= 4 || v >= 4 || d1 >= 4 || d2 >= 4);
}

/*
    check_draw:
    Returns 1 if the board is full (no more moves), else 0.
*/
static int check_draw(int b[ROWS][COLS]) {
    for (int c = 0; c < COLS; c++) {
        if (b[0][c] == CELL_EMPTY) return 0;
    }
    return 1;
}

/* ------------------------- Game Input ------------------------- */

/*
    read_game_action:
    Reads a single key and returns:
      - 0..6 if user pressed '1'..'7' (column index)
      - ACT_RESET if 'r'/'R'
      - ACT_QUIT  if 'q'/'Q'
    Any other key is ignored (keeps waiting).
*/
static int read_game_action(void) {
    for (;;) {
        int k = _getch();

        if (k >= '1' && k <= '7') return (k - '1');

        switch (k) {
        case 'q':
        case 'Q':
            return ACT_QUIT;

        case 'r':
        case 'R':
            return ACT_RESET;

        default:
            break;
        }
    }
}

/* ------------------------- AI ------------------------- */

/*
    landing_row_of:
    Returns the row where a chip would land in column col without modifying board.
    Returns -1 if full.
*/
static int landing_row_of(int b[ROWS][COLS], int col) {
    for (int r = ROWS - 1; r >= 0; r--) {
        if (b[r][col] == CELL_EMPTY) return r;
    }
    return -1;
}

/*
    ai_choose_easy:
    Random valid column.
*/
static int ai_choose_easy(int b[ROWS][COLS]) {
    for (;;) {
        int c = rand() % COLS;
        if (!col_full(b, c)) return c;
    }
}

/*
    ai_choose_hard:
    “Hard” AI heuristic:
      1) If AI can win in one move -> play it
      2) Else if human can win in one move -> block it
      3) Else prefer center columns
*/
static int ai_choose_hard(int b[ROWS][COLS], int ai_player) {
    int human = (ai_player == PLAYER_1) ? PLAYER_2 : PLAYER_1;

    /* 1) win now */
    for (int c = 0; c < COLS; c++) {
        if (col_full(b, c)) continue;
        int r = landing_row_of(b, c);
        b[r][c] = ai_player;
        if (check_win(b, r, c, ai_player)) { b[r][c] = CELL_EMPTY; return c; }
        b[r][c] = CELL_EMPTY;
    }

    /* 2) block human */
    for (int c = 0; c < COLS; c++) {
        if (col_full(b, c)) continue;
        int r = landing_row_of(b, c);
        b[r][c] = human;
        if (check_win(b, r, c, human)) { b[r][c] = CELL_EMPTY; return c; }
        b[r][c] = CELL_EMPTY;
    }

    /* 3) center preference */
    {
        int pref[COLS] = { 3, 2, 4, 1, 5, 0, 6 };
        for (int i = 0; i < COLS; i++) {
            int c = pref[i];
            if (!col_full(b, c)) return c;
        }
    }

    return ai_choose_easy(b);
}

/* ------------------------- Game Core ------------------------- */

/*
    play_game:
    Plays one round, but supports reset using an outer loop.
    Parameters:
      mode  - MODE_PVP / MODE_AI_EASY / MODE_AI_HARD
      score_p1 - pointer to Player1/You wins counter
      score_p2 - pointer to Player2/AI wins counter
      score_d  - pointer to draws counter

    Behavior:
      - Q: quit to menu (returns)
      - R: reset board (restarts round, does not return to menu)
*/
static void play_game(int mode, int* score_p1, int* score_p2, int* score_d) {
    int b[ROWS][COLS];
    int player;

    for (;;) { /* outer loop: restart round on reset */
        board_reset(b);
        player = PLAYER_1;

        for (;;) { /* turn loop */
            clear_screen();

            /* Header by mode */
            switch (mode) {
            case MODE_PVP:
                puts("PvP (X=Player1, O=Player2)");
                break;
            case MODE_AI_EASY:
                puts("Vs AI (easy) (You=X, AI=O)");
                break;
            case MODE_AI_HARD:
                puts("Vs AI (hard) (You=X, AI=O)");
                break;
            default:
                puts("Unknown mode");
                break;
            }

            puts("Controls: 1-7 drop | R reset | Q quit\n");

            /* Turn line */
            if (player == PLAYER_1) puts("Turn: Player 1 (X)");
            else puts(mode == MODE_PVP ? "Turn: Player 2 (O)" : "Turn: Computer (O)");

            board_print(b);

            /* Decide the action/column */
            int action;

            switch (mode) {
            case MODE_PVP:
                action = read_game_action();
                break;

            case MODE_AI_EASY:
            case MODE_AI_HARD:
                if (player == PLAYER_2) {
                    action = (mode == MODE_AI_HARD) ? ai_choose_hard(b, PLAYER_2) : ai_choose_easy(b);
                }
                else {
                    action = read_game_action();
                }
                break;

            default:
                action = ACT_QUIT;
                break;
            }

            /* Handle action */
            switch (action) {
            case ACT_QUIT:
                return;

            case ACT_RESET:
                /* restart round */
                break;

            default: {
                int col = action;
                int row = drop_chip(b, col, player);
                if (row == -1) continue; /* full column */

                if (check_win(b, row, col, player)) {
                    clear_screen();
                    board_print(b);

                    switch (mode) {
                    case MODE_PVP:
                        if (player == PLAYER_1) { puts("\nPlayer 1 wins!"); (*score_p1)++; }
                        else { puts("\nPlayer 2 wins!"); (*score_p2)++; }
                        break;

                    case MODE_AI_EASY:
                    case MODE_AI_HARD:
                        if (player == PLAYER_1) { puts("\nYou win!"); (*score_p1)++; }
                        else { puts("\nComputer wins!"); (*score_p2)++; }
                        break;

                    default:
                        break;
                    }

                    press_any_key();
                    return;
                }

                if (check_draw(b)) {
                    clear_screen();
                    board_print(b);
                    puts("\nDraw.");
                    (*score_d)++;
                    press_any_key();
                    return;
                }

                player = (player == PLAYER_1) ? PLAYER_2 : PLAYER_1;
                continue;
            }
            }

            /* ACT_RESET reached */
            break; /* break turn loop -> outer loop restarts round */
        }
    }
}

/*
    show_score:
    Prints the current score counters.
*/
static void show_score(int score_p1, int score_p2, int score_d) {
    clear_screen();
    puts("SCORE (this run only)\n");
    printf("Player 1 / You wins : %d\n", score_p1);
    printf("Player 2 / AI wins  : %d\n", score_p2);
    printf("Draws               : %d\n", score_d);
    press_any_key();
}

/* ------------------------- main ------------------------- */

int main(void) {
    int score_p1 = 0;
    int score_p2 = 0;
    int score_d = 0;

    srand((unsigned)time(NULL));

    for (;;) {
        print_menu();
        int choice = read_menu_choice();

        switch (choice) {
        case MENU_PVP:
            play_game(MODE_PVP, &score_p1, &score_p2, &score_d);
            break;

        case MENU_AI_EASY:
            play_game(MODE_AI_EASY, &score_p1, &score_p2, &score_d);
            break;

        case MENU_AI_HARD:
            play_game(MODE_AI_HARD, &score_p1, &score_p2, &score_d);
            break;

        case MENU_SCORE:
            show_score(score_p1, score_p2, score_d);
            break;

        case MENU_EXIT:
            clear_screen();
            puts("Bye.");
            return 0;

        default:
            break;
        }
    }
}
