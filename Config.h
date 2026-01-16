#ifndef CONFIG_H
#define CONFIG_H

/* Board size */
#define ROWS 6
#define COLS 7

/* Menu choices */
#define MENU_PVP        1
#define MENU_AI_EASY    2
#define MENU_AI_HARD    3
#define MENU_SCORE      4
#define MENU_EXIT       5

/* Game modes */
#define MODE_PVP        0
#define MODE_AI_EASY    1
#define MODE_AI_HARD    2

/* Game actions returned by read_game_action() */
#define ACT_QUIT       -1    /* Q */
#define ACT_RESET      -2    /* R */

/* Cell values */
#define CELL_EMPTY      0
#define PLAYER_1        1    /* X */
#define PLAYER_2        2    /* O (or AI) */

#endif
