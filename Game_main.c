#include <stdio.h>
#include <conio.h>
#include "Game_config.h"

/* Function Declaration */
int read_key(void); // Read a Key from a keyboard
void print_score(int drow, int first, int second);
/* Main function */
int main(void) {
    int selected = 0;
    int score[3] = { 0,0,0 };
    
    /* Hide the cursore */
    printf(ANSI_HIDE_CURSOR); 
    fflush(stdout);

    /* Draw the menu */
    ui_menu_init();
    ui_menu_draw_options(selected);

    /* Looping till exit */
    for (;;) {
        int k = read_key();
        int temp = 0;
        switch (k) {
        case K_UP:
            if (selected > 0) selected--;
            ui_menu_draw_options(selected);
            break;

        case K_DOWN:
            if (selected < MENU_OPTIONS - 1) selected++;
            ui_menu_draw_options(selected);
            break;

        case K_ENTER:
            ui_menu_flash_selected(selected);
            clear_screen();

            switch (selected) {
                case 0: 
                case 1:
                case 2: 
                    temp = start_game(selected);
                    if (temp >= 0) score[temp]++; //If the game returns a result, save it 
                    break;
                case 3: print_score(score[0],score[1],score[2]); break;
                case 4: ui_display_manual(); break;
                case MENU_OPTIONS - 1: exit(0); // exit on ESC
                default: printf("Unknown option.\n"); break;
            }

            //printf(ANSI_FG_GRAY "\nPress any key to return...");
            //_getch(); // Wait  untill the user press any key

            ui_menu_init();
            ui_menu_draw_options(selected);
            break;

        case K_ESC:
            printf(ANSI_SHOW_CURSOR ANSI_RESET);
            fflush(stdout);
            return 0;

        default:
            break;
        }
    }
}

/* Function section */

void print_score(int drow, int first, int second) {
    printf("+----------------------------------------------------+\n");
    printf(ANSI_FG_CYAN "SCORE: " ANSI_RESET);
    printf(ANSI_FG_RED "Player 1\t\t" ANSI_FG_YELLOW "Player 2\t" ANSI_FG_GRAY "Drow\n" ANSI_RESET);
    printf("+----------------------------------------------------+\n");
    printf("\t%d\t\t%d\t\t%d\n",first,second,drow);
    printf("+----------------------------------------------------+\n");
    printf(ANSI_FG_GRAY "Press any key to return....");
    _getch();
}

int read_key(void) {
    int key = _getch();

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

    switch (key) {
    case 13:  return K_ENTER;
    case ' ': return K_ENTER;
    case 27:  return K_ESC;
    case 'r':
    case 'R': return K_RESET;
    default:  return K_NONE;
    }
}