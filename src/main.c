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
#include <graphx.h>

int inMenu = 1;
int leftLast = 0, upLast = 0, rightLast = 0, downLast = 0, selectLast = 0, alphaLast = 0;
int shouldRedrawBoard = 0, shouldRedrawMenu = 1;

void end();

#include "debug.h"
#include "draw.h"
#include "mathutils.h"
#include "game.h"
#include "menu.h"

void end() {
    if(gameWasLoaded) stopGame();
    
    gfx_End();
}

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
