#define VERSION "v2.1 BETA"
#define ROMAN_VERSION "vII.I BETA"
#define BOARD_MINE (-1)
#define BOARD_CLEAR 0
#define MASK_COVERED 0
#define MASK_FLAGGED 1
#define MASK_UNCOVERED 2
#define DIFFIC_BEGINNER 0
#define DIFFIC_INTERMEDIATE 1
#define DIFFIC_EXPERT 2
#define DIFFIC_INSANE 3

#define COL_PINK 0
#define COL_WHITE 1
#define COL_BLACK 2
#define COL_YELLOW 3
#define COL_GRAY 4

#include <string.h>
#include <keypadc.h>
#include <graphx.h>
#include "gfx/gfx.h"

struct Skin {
    char* name;
    gfx_sprite_t** sprites;
    gfx_sprite_t* title;
    int bgColor;
    int fgColor;
    int restartColor;
};

int inMenu = 1;
int leftLast = 0, upLast = 0, rightLast = 0, downLast = 0, selectLast = 0, alphaLast = 0;
int shouldRedrawBoard = 0, shouldRedrawMenu = 1;
int difficulty = 0;
int boardWidth = 8, boardHeight = 8, totalMines = 10, boardArea = 64;
int skin = 0;
const struct Skin skinList[] = {
    {"Classic", classic_sprites_tiles, standard_title, COL_WHITE, COL_BLACK, COL_YELLOW},
    {"Plastic", plastic_sprites_tiles, plastic_title, COL_WHITE, COL_BLACK, COL_YELLOW},
    {"Google", google_sprites_tiles, google_title, COL_WHITE, COL_BLACK, COL_YELLOW},
    {"Classic Dark", classic_dark_sprites_tiles, classic_dark_title, COL_GRAY, COL_WHITE, COL_YELLOW},
    {"Words", words_sprites_tiles, standard_title, COL_WHITE, COL_BLACK, COL_YELLOW},
    {"Colors", colors_sprites_tiles, colors_title, COL_WHITE, COL_BLACK, COL_YELLOW},
    {"Roman", roman_sprites_tiles, standard_title, COL_WHITE, COL_BLACK, COL_YELLOW}
};

void end();

#include "draw.h"
#include "game.h"
#include "menu.h"

int main() {
    setupGraphics();
    
    int delLast = 0;
    while(1) {
        kb_Scan();
        
        if(inMenu) {
            updateMenu();
            renderMenu();
        }
        else {
            if(!gameWasLoaded) initGame();
            
            updateGame();
            renderGame();
        }
        
        int del = kb_IsDown(kb_KeyDel);
        if(del && !delLast && inMenu) break;
        else if(del && !inMenu) {
            if(gameWasLoaded) stopGame();
            inMenu = 1;
            shouldRedrawMenu = 1;
        }
        delLast = del;
    }
    end();

    return 0;
}
void end() {
    if(gameWasLoaded) stopGame();

    gfx_End();
}
