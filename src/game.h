#pragma once

#include <stdlib.h>
#include <time.h>

#include <keypadc.h>
#include <graphx.h>

#include "mathutils.h"
#include "draw.h"

int selX = 0, selY = 0;
int lastSelX = 0, lastSelY = 0;
int minesFlagged = 0;
int* board = NULL;
int* mask = NULL;
int lost = 0;
int restart = 0;
int* updateTargets = NULL;
int shouldRedrawMineCount = 0;
int gameWasLoaded = 0;

void initGame() {
    gameWasLoaded = 1;
    
    srand(time(NULL));
    board = NULL;
    mask = (int*) malloc(100 * sizeof(int));
    updateTargets = (int*) malloc(100 * sizeof(int));
    shouldRedrawMineCount = lastSelX = lastSelY = selX = selY = minesFlagged = lost = restart = 0;
    
    fillIntArray(mask, 100, 0);
    fillIntArray(updateTargets, 100, 0);
}
void stopGame() {
    gameWasLoaded = 0;
    
    free(board);
    free(mask);
    free(updateTargets);
    
    board = mask = updateTargets = NULL;
}

int getNeighborTile(int loc, int neighborIndex) {
    int neighborX = loc % 10, neighborY = loc / 10;
    
    switch(neighborIndex) {
        case 0:
            neighborX -= 1;
            neighborY -= 1;
            break;
        case 1:
            neighborY -= 1;
            break;
        case 2:
            neighborX += 1;
            neighborY -= 1;
            break;
        case 3:
            neighborX += 1;
            break;
        case 4:
            neighborX += 1;
            neighborY += 1;
            break;
        case 5:
            neighborY += 1;
            break;
        case 6:
            neighborX -= 1;
            neighborY += 1;
            break;
        case 7:
            neighborX -= 1;
            break;
        default:
            return -1;
    }
    
    if(neighborX < 0 || neighborX > 9 || neighborY < 0 || neighborY > 9) return -1;
    return neighborX + 10 * neighborY;
}
int* generateBoard() {
    int* newBoard = (int*) malloc(100 * sizeof(int));
    fillIntArray(newBoard, 100, 0);
    
    for(int i = 0; i < 10; i++) {
        do {
            int loc = randInt(0, 99);
            int locX = loc % 10;
            int locY = loc / 10;
            
            if(newBoard[loc] != BOARD_MINE && abs(locX - selX) >= 2 && abs(locY - selY) >= 2) {
                newBoard[loc] = BOARD_MINE;
                
                for(int i = 0; i < 8; i++) {
                    int neighborLoc = getNeighborTile(loc, i);
                    if(neighborLoc == -1) continue;
                    
                    if(newBoard[neighborLoc] == BOARD_MINE) continue;
                    newBoard[neighborLoc]++;
                }
                
                break;
            }
        }
        while(true);
    }
    
    return newBoard;
}
void reveal(int index, int isOriginal) {
    if(mask[index] == MASK_UNCOVERED && isOriginal && board[index] > BOARD_CLEAR) {
        int known = 0;
        for(int i = 0; i < 8; i++) {
            int neighborIndex = getNeighborTile(index, i);
            if(neighborIndex == -1) continue;
            known += mask[neighborIndex] == MASK_FLAGGED;
        }
        if(known >= board[index]) {
            for(int i = 0; i < 8; i++) {
                int neighborIndex = getNeighborTile(index, i);\
                if(neighborIndex == -1) continue;
                if(mask[neighborIndex] == MASK_COVERED) {
                    reveal(neighborIndex, 0);
                    if(board[neighborIndex] == BOARD_MINE) {
                        lost = 1;
                        restart = 1;
                    }
                }
            }
        }
        
        return;
    }
    else if(mask[index] != MASK_COVERED) return;
    
    mask[index] = MASK_UNCOVERED;
    updateTargets[index] = 1;
    if(board[index] != BOARD_CLEAR) return;
    
    for(int i = 0; i < 8; i++) {
        int neighborLoc = getNeighborTile(index, i);
        if(neighborLoc == -1) continue;
        
        reveal(neighborLoc, 0);
    }
}
int gameIsCompleted() {
    for(int i = 0; i < 100; i++) {
        if(board[i] == BOARD_MINE) {
            if(mask[i] != MASK_UNCOVERED) continue;
        }
        else if(mask[i] == MASK_UNCOVERED) continue;
        
        return 0;
    }
    
    return 1;
}

void updateGame() {
    int left = kb_IsDown(kb_KeyLeft);
    int up = kb_IsDown(kb_KeyUp);
    int right = kb_IsDown(kb_KeyRight);
    int down = kb_IsDown(kb_KeyDown);
    int select = kb_IsDown(kb_Key2nd) || kb_IsDown(kb_KeyEnter);
    int alpha = kb_IsDown(kb_KeyAlpha);
    
    if(restart) {
        if(kb_AnyKey() && !selectLast) {
            srand(time(NULL));
            memset(mask, MASK_COVERED, 100 * sizeof(int));
            free(board);
            board = NULL;
            
            lost = restart = lastSelX = lastSelY = selX = selY = minesFlagged = 0;
            
            shouldRedrawBoard = 1;
        }
        
        leftLast = left;
        upLast = up;
        rightLast = right;
        downLast = down;
        selectLast = select;
        alphaLast = alpha;
        return;
    }
    
    if(left && !leftLast) selX = mod(selX - 1, 10);
    else if(up && !upLast) selY = mod(selY - 1, 10);
    else if(right && !rightLast) selX = mod(selX + 1, 10);
    else if(down && !downLast) selY = mod(selY + 1, 10);
    
    if(select && !selectLast) {
        if(board == NULL) board = generateBoard();
        
        reveal(10 * selY + selX, 1);
        if(board[selX + 10 * selY] == BOARD_MINE) {
            lost = 1;
            restart = 1;
        }
        else if(gameIsCompleted()) {
            restart = 1;
            fillIntArray(mask, 100, MASK_UNCOVERED);
            shouldRedrawBoard = 1;
        }
    }
    else if(alpha && !alphaLast && mask[10 * selY + selX] == MASK_COVERED && minesFlagged < 10) {
        mask[10 * selY + selX] = MASK_FLAGGED;
        updateTargets[10 * selY + selX] = 1;
        minesFlagged++;
        shouldRedrawMineCount = 1;
    }
    else if(alpha && !alphaLast && mask[10 * selY + selX] == MASK_FLAGGED) {
        mask[10 * selY + selX] = MASK_COVERED;
        updateTargets[10 * selY + selX] = 1;
        minesFlagged--;
        shouldRedrawMineCount = 1;
    }
    
    if(selX != lastSelX || selY != lastSelY) {
        updateTargets[10 * lastSelY + lastSelX] = 1;
        updateTargets[10 * selY + selX] = 1;
    }
    
    leftLast = left;
    upLast = up;
    rightLast = right;
    downLast = down;
    selectLast = select;
    alphaLast = alpha;
    
    lastSelX = selX;
    lastSelY = selY;
}
void renderGame() {
    if(mask == NULL) return;
    
    int updated = 0;
    if(shouldRedrawBoard) {
        drawBoard(board, mask, selX, selY, minesFlagged, lost);
        updated = 1;
    }
    else {
        for(int i = 0; i < 100; i++) {
            if(!updateTargets[i]) continue;
            drawTile(i % 10, i / 10, board, mask, selX, selY, lost);
            
            updated = 1;
        }
        
        if(shouldRedrawMineCount) {
            drawRemainingFlags(minesFlagged);
            updated = 1;
        }
    }
    
    if(restart) {
        drawRestartText();
        updated = 1;
    }
    
    if(updated) gfx_BlitBuffer();
    
    shouldRedrawBoard = 0;
    shouldRedrawMineCount = 0;
    memset(updateTargets, 0, 100 * sizeof(int)); //clear update targets (reset to all 0)
}
