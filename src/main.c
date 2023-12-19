#define VERSION "v2.0 BETA"
#define BOARD_MINE -1
#define BOARD_CLEAR 0
#define MASK_COVERED 0
#define MASK_FLAGGED 1
#define MASK_UNCOVERED 2

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

#include <keypadc.h>

#include "debug.h"
#include "draw.h"
#include "mathutils.h"
#include "menu.h"
#include "game.h"

int inMenu = 1;

int main() {
    srand(time(NULL)); //set seed for rand
    
    setupGraphics();
    
    do {
        kb_Scan();
        
        switch(inMenu) {
            case 1:
                updateMenu();
                renderMenu();
                
                break;
            case 0:
                if(!gameWasLoaded) initGame();
                
                updateGame();
                renderGame();
                
                needToRedrawBoard = 0;
                needToRedrawMineCount = 0;
                memset(updateTargets, 0, 100 * sizeof(int)); //clear update targets (reset to all 0)
                
                break;
        }
    }
    while(!kb_IsDown(kb_KeyDel));
    
    if(gameWasLoaded) stopGame();
    
    gfx_End();
    return 0;
}