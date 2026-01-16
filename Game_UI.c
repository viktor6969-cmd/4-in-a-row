#include <stdio.h>
#include <conio.h>
#include <time.h>
#include "Game_config.h"


const char* options[MENU_OPTIONS] = {
    "Play PvP [I have friends]",
    "Play vs AI [EZ MODE]",
    "Play vs AI [HARD MODE]",
    "Show games statistics",
    "How to play?",
    "Exit"
};

void delay_ms(int ms) {
    clock_t start = clock();
    clock_t wait = (clock_t)((ms * (double)CLOCKS_PER_SEC) / 1000.0);
    while ((clock() - start) < wait) {}
}

void clear_screen(void) {
    printf(ANSI_CLEAR_SCREEN ANSI_CURSOR_HOME);
    fflush(stdout);
}

void cursor_goto(int row, int col) {
    printf("\x1b[%d;%dH", row, col);
}

void clear_line(void) {
    printf("\x1b[2K");
}



void ui_menu_init(void) {

    clear_screen();

    printf("+----------------------------------------------------+\n");
    printf(ANSI_FG_RED ANSI_BRIGHT);
    printf(LOGO_4_IN_A_ROW);
    printf(ANSI_RESET "\n");
    printf(ANSI_FG_YELLOW ANSI_BRIGHT "UP/DOWN move | ENTER/SPACE select | ESC quit\n\n" ANSI_FG_WHITE);
    printf("\n\n\n\n\n\n");
    printf("+----------------------------------------------------+\n");
    printf(ANSI_RESET);

    fflush(stdout);
}

void ui_menu_draw_options(int selected) {
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

void ui_menu_flash_selected(int selected) {
    char *color;
    for (int t=0; t < 6; t++) {
        color = (t % 2 == 1) ? (ANSI_BG_RED) : (ANSI_BG_BLUE);
        cursor_goto(MENU_TOP_ROW + selected, MENU_LEFT_COL);
        clear_line();
        printf("%s" ANSI_FG_WHITE ANSI_BRIGHT "  > %s  " ANSI_RESET, color, options[selected]);
        fflush(stdout);
        delay_ms(110);
    }
}

void ui_display_manual(void) {

    printf(ANSI_FG_CYAN ANSI_BRIGHT "HOW TO PLAY\n" ANSI_FG_GRAY);
    printf("----------------------------------------\n");
    printf(ANSI_FG_YELLOW ANSI_BRIGHT "Controls:\n\n");
    printf(ANSI_FG_WHITE "UP / DOWN / LEFT / RIGHT" ANSI_FG_GRAY "  - Move selector / column\n");
    printf(ANSI_FG_WHITE "ENTER or SPACE " ANSI_FG_GRAY "           - Drop a chip\n");
    printf(ANSI_FG_WHITE "R / r" ANSI_FG_GRAY "                     - Reset the game\n");
    printf(ANSI_FG_WHITE "ESC" ANSI_FG_GRAY "                       - Return to menu\n\n");
    printf(ANSI_FG_GREEN ANSI_BRIGHT "Goal: "ANSI_FG_WHITE "Connect " ANSI_FG_RED "4" ANSI_FG_WHITE " chips in a row(horizontal, vertical, or diagonal)\n\n");
    printf(ANSI_FG_GRAY "Press any key to return...");
    fflush(stdout);
    _getch();
}
