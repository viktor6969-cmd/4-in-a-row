#include <stdio.h>
#include <conio.h>
#include "Game_config.h"


// ===== Function declarations ======

// ------ Keyboard input ----
int read_key(void);   // Read a key from the keyboard and return K_* code

// ------ Score printing ----
void print_score(int drow, int first, int second);   // { drow - draws, first - Player 1 wins, second - Player 2 wins }

/*=======*/


// ===== Main function ======

int main(void) {
    int selected = 0;                 // Current selected menu option index
    int score[3] = { 0, 0, 0 };        // { score[0] - draws, score[1] - Player 1 wins, score[2] - Player 2 wins }

    // ------ Cursor control ----
    printf(ANSI_HIDE_CURSOR);          // Hide the cursor
    fflush(stdout);

    // ------ Menu render (static) ----
    ui_menu_init();
    ui_menu_draw_options(selected);

    // ------ Main loop ----
    for (;;) {
        int k = read_key();
        int temp = 0;                 // Game return result

        switch (k) {

            // ------ Menu navigation ----
        case K_UP:
            if (selected > 0) selected--;
            ui_menu_draw_options(selected);
            break;

        case K_DOWN:
            if (selected < MENU_OPTIONS - 1) selected++;
            ui_menu_draw_options(selected);
            break;

            // ------ Menu selection ----
        case K_ENTER:
            ui_menu_flash_selected(selected);
            clear_screen();

            switch (selected) {

                // ------ Game modes ----
            case 0:
            case 1:
            case 2:
                temp = start_game(selected);
                if (temp >= 0) score[temp]++;     // If the game returns a result, save it
                break;

                // ------ Statistics ----
            case 3:
                print_score(score[0], score[1], score[2]);
                break;

                // ------ Manual / help ----
            case 4:
                ui_display_manual();
                break;

                // ------ Exit option ----
            case MENU_OPTIONS - 1:
                exit(0);                          // Exit from menu
                break;

            default:
                printf("Unknown option.\n");
                break;
            }

            // ------ Return back to menu ----
            ui_menu_init();
            ui_menu_draw_options(selected);
            break;

            // ------ ESC exit ----
        case K_ESC:
            printf(ANSI_SHOW_CURSOR ANSI_RESET);
            fflush(stdout);
            return 0;

        default:
            break;
        }
    }
}

/*=======*/


// ===== Utility functions ======

// ------ Score table rendering ----
void print_score(int drow, int first, int second) {   // { drow - draws, first - Player 1 wins, second - Player 2 wins }
    // Prints the score table and waits for key press

    printf("+----------------------------------------------------+\n");
    printf(ANSI_FG_CYAN "SCORE: " ANSI_RESET);
    printf(ANSI_FG_RED "Player 1\t\t" ANSI_FG_YELLOW "Player 2\t" ANSI_FG_GRAY "Draw\n" ANSI_RESET);
    printf("+----------------------------------------------------+\n");
    printf("\t%d\t\t%d\t\t%d\n", first, second, drow);
    printf("+----------------------------------------------------+\n");
    printf(ANSI_FG_GRAY "Press any key to return...." ANSI_RESET);

    _getch();
}

/*=======*/


// ===== Input functions ======

// ------ Read key from keyboard ----
int read_key(void) {
    // Reads key press and converts to K_* codes

    int key = _getch();

    // ------ Arrow keys handling ----
    if (key == 0 || key == 224) {
        key = _getch();
        switch (key) {
        case 72: return K_UP;
        case 80: return K_DOWN;
        case 75: return K_LEFT;
        case 77: return K_RIGHT;
        default: return K_NONE;
        }
    }

    // ------ Single-byte keys handling ----
    switch (key) {
    case 13:  return K_ENTER;   // Enter
    case ' ': return K_ENTER;   // Space
    case 27:  return K_ESC;     // Esc
    case 'r':
    case 'R': return K_RESET;   // Reset
    default:  return K_NONE;
    }
}

/*=======*/
