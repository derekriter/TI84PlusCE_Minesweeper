#pragma once

#include <keypadc.h>
#include <graphx.h>

#include "draw.h"
#include "mathutils.h"

int cursorPos = 0;
int shouldRedrawCursor = 0;
int shouldRedrawDiffic = 0;

void updateMenu() {
    int up = kb_IsDown(kb_KeyUp);
    int down = kb_IsDown(kb_KeyDown);
    int select = kb_IsDown(kb_Key2nd) || kb_IsDown(kb_KeyEnter);
    
    if(up && !upLast) {
        cursorPos = mod(cursorPos - 1, 4);
        shouldRedrawCursor = 1;
    }
    else if(down && !downLast) {
        cursorPos = mod(cursorPos + 1, 4);
        shouldRedrawCursor = 1;
    }
    else if(select && !selectLast) {
        switch(cursorPos) {
            case 0:
                //play
                inMenu = 0;
                shouldRedrawBoard = 1;
                
                break;
            case 1:
                //change difficulty
                difficulty = mod(difficulty + 1, 3);
                switch(difficulty) {
                    case DIFFIC_BEGINNER:
                        boardWidth = 8;
                        boardHeight = 8;
                        totalMines = 10;
                        break;
                    case DIFFIC_INTERMEDIATE:
                        boardWidth = 11;
                        boardHeight = 11;
                        totalMines = 20;
                        break;
                    case DIFFIC_EXPERT:
                        boardWidth = 16;
                        boardHeight = 13;
                        totalMines = 40;
                        break;
                }
                boardArea = boardWidth * boardHeight;
                shouldRedrawDiffic = 1;
                
                break;
            case 2:
                //change skin
                skin = mod(skin + 1, sizeof(skinList) / sizeof(struct Skin));
                gfx_SetTextFGColor(skinList[skin].fgColor);
                gfx_SetColor(skinList[skin].bgColor);
                shouldRedrawMenu = 1;
                
                break;
            case 3:
                //quit
                end();
                exit(0);
                
                break;
        }
    }
    
    upLast = up;
    downLast = down;
    selectLast = select;
}
void renderMenu() {
    int updated = 0;
    if(shouldRedrawMenu) {
        drawMenu(cursorPos);
        updated = 1;
    }
    else {
        if(shouldRedrawCursor) {
            drawCursor(cursorPos);
            updated = 1;
        }
        if(shouldRedrawDiffic) {
            drawDiffic();
            updated = 1;
        }
    }
    
    if(updated) gfx_BlitBuffer();
    
    shouldRedrawMenu = shouldRedrawCursor = shouldRedrawDiffic = 0;
}
