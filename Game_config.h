/* =========== Used functions ===========*/

int read_key(void);
/* ---------- UI functions ---------- */
void delay_ms(int ms);
void clear_screen(void);
void ui_menu_init(void);
void ui_display_manual(void);
void ui_menu_draw_options(int selected);
void ui_menu_flash_selected(int selected);

/* ---------- PvP functions ---------*/
int start_game(int mode);

/* ======================================*/

/* ---------- menu ---------- */
#define MENU_OPTIONS 6

/* ---------- key codes ---------- */
#define K_NONE   -1
#define K_UP      0
#define K_DOWN    1
#define K_LEFT    2
#define K_RIGHT   3
#define K_ENTER   4
#define K_ESC     5
#define K_RESET   6


/* =========================
Board constants
========================= */
#define ROWS 6
#define COLS 7

/* Layout anchors (tweak if you want it more centered) */
#define BOARD_TOP_ROW   3
#define BOARD_LEFT_COL  32

#define ARROW_ROW       (BOARD_TOP_ROW - 1)
#define TURN_ROW        2
#define MSG_ROW         18

#define CELL_W 4
#define CELL_H 2




/*--------------Menu layout---------------*/
/* layout */
#define MENU_TOP_ROW  11
#define MENU_LEFT_COL 2

/* ---------- ANSI ---------- */
#define ANSI_CLEAR_SCREEN "\x1b[2J"
#define ANSI_CURSOR_HOME  "\x1b[H"
#define ANSI_RESET        "\x1b[0m"

#define ANSI_HIDE_CURSOR  "\x1b[?25l"
#define ANSI_SHOW_CURSOR  "\x1b[?25h"

#define ANSI_BRIGHT       "\x1b[1m"
#define ANSI_DIM          "\x1b[2m"

#define ANSI_FG_WHITE     "\x1b[37m"
#define ANSI_FG_GRAY      "\x1b[90m"
#define ANSI_FG_RED       "\x1b[31m"
#define ANSI_FG_YELLOW    "\x1b[33m"
#define ANSI_FG_CYAN      "\x1b[36m"
#define ANSI_FG_GREEN     "\x1b[32m"


#define ANSI_BG_RED       "\x1b[41m"
#define ANSI_BG_BLUE      "\x1b[44m"

/* ---------- Logo ---------- */
#define LOGO_4_IN_A_ROW \
" _____                             _       ___ \n" \
"/  __ \\                           | |     /   |\n" \
"| /  \\/ ___  _ __  _ __   ___  ___| |_   / /| |\n" \
"| |    / _ \\| '_ \\| '_ \\ / _ \\/ __| __| / /_| |\n" \
"| \\__/\\ (_) | | | | | | |  __/ (__| |_  \\___  |\n" \
" \\____/\\___/|_| |_|_| |_|\\___|\\___|\\__|     |_/\n"
