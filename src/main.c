#define BOARD_MINE -1
#define BOARD_CLEAR 0
#define MASK_COVERED 0
#define MASK_FLAGGED 1
#define MASK_UNCOVERED 2

#define VERSION "v1.0 BETA"

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

#include <keypadc.h>

#include "debug.h"
#include "queue.h"
#include "draw.h"
#include "mathutils.h"

int selX = 0, selY = 0;
int minesDiscovered = 0;
int* board = NULL;
int* mask = NULL;
clock_t restartTime;
int restart = 0;
int* updateTargets = NULL;
int needToRedrawBoard = 0;
int needToRedrawMineCount = 0;

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
    int* newBoard = (int*) calloc(100, sizeof(int));
    
    for(int i = 0; i < 10; i++) {
        do {
            int loc = randInt(0, 99);
            if(newBoard[loc] != BOARD_MINE && loc != selX + 10 * selY) {
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
void reveal(int index) {
    if(mask[index] != MASK_COVERED) return;
    
    mask[index] = MASK_UNCOVERED;
    updateTargets[index] = 1;
    if(board[index] != BOARD_CLEAR) return;
    
    for(int i = 0; i < 8; i++) {
        int neighborLoc = getNeighborTile(index, i);
        if(neighborLoc == -1) continue;
        
        reveal(neighborLoc);
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
void fillIntArray(int* array, int length, int value) {
    for(int i = 0; i < length; i++)
        array[i] = value;
}

int leftLast = 0, upLast = 0, rightLast = 0, downLast = 0, uncoverLast = 0, flagLast = 0;
int lastSelX = 0, lastSelY = 0;
void update() {
    if(restart) {
        if((clock() - restartTime) / CLOCKS_PER_SEC < 3) return;
        
        srand(time(NULL));
        memset(mask, MASK_COVERED, 100 * sizeof(int));
        free(board);
        board = NULL;
        
        restart = leftLast = upLast = rightLast = downLast = uncoverLast = flagLast = lastSelX = lastSelY = selX = selY = minesDiscovered = 0;
        
        needToRedrawBoard = 1;
        return;
    }
    
    int left = kb_IsDown(kb_KeyLeft);
    int up = kb_IsDown(kb_KeyUp);
    int right = kb_IsDown(kb_KeyRight);
    int down = kb_IsDown(kb_KeyDown);
    int uncover = kb_IsDown(kb_Key2nd);
    int flag = kb_IsDown(kb_KeyAlpha);
    
    if(left && !leftLast) selX = mod(selX - 1, 10);
    else if(up && !upLast) selY = mod(selY - 1, 10);
    else if(right && !rightLast) selX = mod(selX + 1, 10);
    else if(down && !downLast) selY = mod(selY + 1, 10);
    
    if(uncover && !uncoverLast && mask[10 * selY + selX] == MASK_COVERED) {
        if(board == NULL) board = generateBoard();
        
        reveal(10 * selY + selX);
        if(board[selX + 10 * selY] == BOARD_MINE) {
            restart = 1;
            restartTime = clock();
        }
        else if(gameIsCompleted()) {
            restart = 1;
            restartTime = clock();
            fillIntArray(mask, 100, MASK_UNCOVERED);
            needToRedrawBoard = 1;
        }
    }
    else if(flag && !flagLast && mask[10 * selY + selX] == MASK_COVERED && minesDiscovered < 10) {
        mask[10 * selY + selX] = MASK_FLAGGED;
        updateTargets[10 * selY + selX] = 1;
        minesDiscovered++;
        needToRedrawMineCount = 1;
    }
    else if(flag && !flagLast && mask[10 * selY + selX] == MASK_FLAGGED) {
        mask[10 * selY + selX] = MASK_COVERED;
        updateTargets[10 * selY + selX] = 1;
        minesDiscovered--;
        needToRedrawMineCount = 1;
    }
    
    if(selX != lastSelX || selY != lastSelY) {
        updateTargets[10 * lastSelY + lastSelX] = 1;
        updateTargets[10 * selY + selX] = 1;
    }
    
    leftLast = left;
    upLast = up;
    rightLast = right;
    downLast = down;
    uncoverLast = uncover;
    flagLast = flag;
    
    lastSelX = selX;
    lastSelY = selY;
}
void render() {
    if(needToRedrawBoard) {
        drawBoard(board, mask, selX, selY, minesDiscovered);
    }
    else {
        for(int i = 0; i < 100; i++) {
            if(!updateTargets[i]) continue;
            drawTile(i % 10, i / 10, board, mask, selX, selY);
        }
        
        if(needToRedrawMineCount) drawMineCount(minesDiscovered);
    }
    
    gfx_BlitBuffer();
}

int main() {
    srand(time(NULL)); //set seed for rand
    
    setupGraphics();
    
    mask = (int*) calloc(100, sizeof(int));
    needToRedrawBoard = 1;
    updateTargets = (int*) calloc(100, sizeof(int));
    
    do {
        kb_Scan();
        
        update();
        render();
        
        needToRedrawBoard = 0;
        needToRedrawMineCount = 0;
        memset(updateTargets, 0, 100 * sizeof(int)); //clear update targets (reset to all 0)
    }
    while(!kb_IsDown(kb_KeyDel));
    
    free(board);
    free(mask);
    free(updateTargets);
    
    gfx_End();
    return 0;
}