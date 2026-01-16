#include <stdio.h>
#include <conio.h>
#include <time.h>
#include "Game_config.h"


// ===== UI static data ======

const char* options[MENU_OPTIONS] = {
    "Play PvP [I have friends]",
    "Play vs AI [EZ MODE]",
    "Play vs AI [HARD MODE]",
    "Show games statistics",
    "How to play?",
    "Exit"
};

/*=======*/


// ===== Utility timing functions ======

// ------ Delay handling ----
void delay_ms(int ms) {   // { ms - delay time in milliseconds }
    // Creates a blocking delay using CPU clock ticks
    clock_t start = clock();
    clock_t wait = (clock_t)((ms * (double)CLOCKS_PER_SEC) / 1000.0);
    while ((clock() - start) < wait) {}
}

/*=======*/


// ===== Console screen control ======

// ------ Screen clearing ----
void clear_screen(void) {
    // Clears the entire console screen and resets cursor position
    printf(ANSI_CLEAR_SCREEN ANSI_CURSOR_HOME);
    fflush(stdout);
}

// ------ Cursor movements ----
void cursor_goto(int row, int col) {   // { row - needed row, col - needed column }
    // Moves the cursor to board [row, col]
    printf("\x1b[%d;%dH", row, col);
}

// ------ Line clearing ----
void clear_line(void) {
    // Clears the current console line
    printf("\x1b[2K");
}

/*=======*/


// ===== Menu rendering functions ======

// ------ Initial menu drawing ----
void ui_menu_init(void) {
    // Draws the static parts of the menu (logo, frame, controls)

    clear_screen();

    printf("+----------------------------------------------------+\n");
    printf(ANSI_FG_RED ANSI_BRIGHT);
    printf(LOGO_4_IN_A_ROW);
    printf(ANSI_RESET "\n");

    printf(ANSI_FG_YELLOW ANSI_BRIGHT "UP/DOWN move | ENTER/SPACE select | ESC quit\n\n"  ANSI_FG_WHITE);

    // Reserve vertical space for menu options
    printf("\n\n\n\n\n\n");

    printf("+----------------------------------------------------+\n");
    printf(ANSI_RESET);

    fflush(stdout);
}

// ------ Menu options rendering ----
void ui_menu_draw_options(int selected) {   // { selected - currently selected option index }
    // Draws only the menu options block and highlights the selected option

    for (int i = 0; i < MENU_OPTIONS; i++) {
        cursor_goto(MENU_TOP_ROW + i, MENU_LEFT_COL);
        clear_line();

        if (i == selected) {
            printf(ANSI_BG_RED ANSI_FG_WHITE ANSI_BRIGHT "  > %s  " ANSI_RESET, options[i]);
        }
        else {
            printf(ANSI_FG_GRAY "    %s" ANSI_RESET, options[i]);
        }
    }

    fflush(stdout);
}

// ------ Menu selection flash ----
void ui_menu_flash_selected(int selected) {   // { selected - option to flash }
    // Blinks the selected menu option by alternating background colors

    char* color;

    for (int t = 0; t < 6; t++) {
        color = (t % 2 == 1) ? (ANSI_BG_RED) : (ANSI_BG_BLUE);

        cursor_goto(MENU_TOP_ROW + selected, MENU_LEFT_COL);
        clear_line();

        printf("%s" ANSI_FG_WHITE ANSI_BRIGHT "  > %s  " ANSI_RESET,
            color, options[selected]);

        fflush(stdout);
        delay_ms(110);
    }
}

/*=======*/


// ===== Manual / help screen ======

// ------ Display how-to-play instructions ----
void ui_display_manual(void) {
    // Displays the game manual and waits for user input

    printf(ANSI_FG_CYAN ANSI_BRIGHT "HOW TO PLAY\n" ANSI_FG_GRAY);
    printf("----------------------------------------\n");

    printf(ANSI_FG_YELLOW ANSI_BRIGHT "Controls:\n\n");

    printf(ANSI_FG_WHITE "UP / DOWN / LEFT / RIGHT"
        ANSI_FG_GRAY "  - Move selector / column\n");

    printf(ANSI_FG_WHITE "ENTER or SPACE "
        ANSI_FG_GRAY "           - Drop a chip\n");

    printf(ANSI_FG_WHITE "R / r"
        ANSI_FG_GRAY "                     - Reset the game\n");

    printf(ANSI_FG_WHITE "ESC"
        ANSI_FG_GRAY "                       - Return to menu\n\n");

    printf(ANSI_FG_GREEN ANSI_BRIGHT "Goal: "
        ANSI_FG_WHITE "Connect "
        ANSI_FG_RED "4"
        ANSI_FG_WHITE " chips in a row (horizontal, vertical, or diagonal)\n\n");

    printf(ANSI_FG_GRAY "Press any key to return...");
    fflush(stdout);

    _getch();
}

/*=======*/
