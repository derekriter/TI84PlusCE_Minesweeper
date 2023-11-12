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

int* generateBoard() {
    static int newBoard[100];
    
    for(int i = 0; i < 10; i++) {
        do {
            int p = randInt(0, 99);
            if(!newBoard[p]) {
                newBoard[p] = BOARD_MINE;
                break;
            }
        }
        while(true);
    }
    for(int i = 0; i < 100; i++) {
        if(newBoard[i] == BOARD_MINE) continue;
        
        newBoard[i] = (i > 9 && newBoard[i - 10] == BOARD_MINE) + (i > 9 && i % 10 < 9 && newBoard[i - 9] == BOARD_MINE) + (i % 10 < 9 && newBoard[i + 1] == BOARD_MINE) + (i < 90 && i % 10 < 9 && newBoard[i + 11] == BOARD_MINE) + (i < 90 && newBoard[i + 10] == BOARD_MINE) + (i < 90 && i % 10 > 0 && newBoard[i + 9] == BOARD_MINE) + (i % 10 > 0 && newBoard[i - 1] == BOARD_MINE) + (i > 9 && i % 10 > 0 && newBoard[i - 11] == BOARD_MINE);
    }
    
    return (int*) newBoard;
}
void reveal(int index) {
    if(mask[index] != MASK_COVERED) return;
    
    mask[index] = MASK_UNCOVERED;
    updateTargets[index] = 1;
    if(board[index] > BOARD_CLEAR) return;
    
    for(int i = 0; i < 8; i++) {
        int neighborX = index % 10, neighborY = index / 10;
        
        switch(i) {
            case 0:
                neighborY -= 1;
                break;
            case 1:
                neighborX += 1;
                neighborY -= 1;
                break;
            case 2:
                neighborX += 1;
                break;
            case 3:
                neighborX += 1;
                neighborY += 1;
                break;
            case 4:
                neighborY += 1;
                break;
            case 5:
                neighborX -= 1;
                neighborY += 1;
                break;
            case 6:
                neighborX -= 1;
                break;
            case 7:
                neighborX -= 1;
                neighborY -= 1;
                break;
            default:
                exit(1);
                break;
        }
        
        if(neighborX < 0 || neighborX > 9 || neighborY < 0 || neighborY > 9) continue;
        
        reveal(neighborX + neighborY * 10);
    }
}

int leftLast = 0, upLast = 0, rightLast = 0, downLast = 0, uncoverLast = 0, flagLast = 0;
int lastSelX = 0, lastSelY = 0;
void update() {
    if(restart) {
        if((clock() - restartTime) / CLOCKS_PER_SEC < 5) return;
        
        restart = 0;
        srand(time(NULL));
        for(int i = 0; i < 100; i++)
            mask[i] = board[i] = 0;
        board = generateBoard();
        
        leftLast = upLast = rightLast = downLast = uncoverLast = flagLast = lastSelX = lastSelY = selX = selY = minesDiscovered = 0;
        
        needToRedrawBoard = 1;
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
    
    if(uncover && !uncoverLast && mask[10 * selY + selX] == MASK_COVERED) reveal(10 * selY + selX);
    else if(flag && !flagLast && mask[10 * selY + selX] == MASK_COVERED && minesDiscovered < 10) {
        mask[10 * selY + selX] = MASK_FLAGGED;
        updateTargets[10 * selY + selX] = 1;
        minesDiscovered++;
        drawMineCount(minesDiscovered);
    }
    else if(flag && !flagLast && mask[10 * selY + selX] == MASK_FLAGGED) {
        mask[10 * selY + selX] = MASK_COVERED;
        updateTargets[10 * selY + selX] = 1;
        minesDiscovered--;
        drawMineCount(minesDiscovered);
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
    }
    
    gfx_BlitBuffer();
}

int main() {
    srand(time(NULL)); //set seed for rand
    
    setupGraphics();
    
    board = generateBoard();
    mask = (int*) calloc(100, sizeof(int));
    needToRedrawBoard = 1;
    updateTargets = (int*) calloc(100, sizeof(int));
    
    do {
        kb_Scan();
        
        update();
        render();
        
        needToRedrawBoard = 0;
        memset(updateTargets, 0, 100); //clear update targets (reset to all 0)
    }
    while(!kb_IsDown(kb_KeyDel));
    
    free(mask);
    free(updateTargets);
    
    gfx_End();
    return 0;
}