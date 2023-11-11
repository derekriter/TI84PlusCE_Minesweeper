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
struct Queue updateQueue = {0, NULL};

int* generateBoard() {
    static int newBoard[100];
    
    for(int i = 0; i < 10; i++) {
        do {
            int p = randInt(0, 99);
            if(!newBoard[p]) {
                newBoard[p] = -1;
                break;
            }
        }
        while(true);
    }
    for(int i = 0; i < 100; i++) {
        if(newBoard[i] == -1) continue;
        
        newBoard[i] = (i > 9 && newBoard[i - 10] == -1) + (i > 9 && i % 10 < 9 && newBoard[i - 9] == -1) + (i % 10 < 9 && newBoard[i + 1] == -1) + (i < 90 && i % 10 < 9 && newBoard[i + 11] == -1) + (i < 90 && newBoard[i + 10] == -1) + (i < 90 && i % 10 > 0 && newBoard[i + 9] == -1) + (i % 10 > 0 && newBoard[i - 1] == -1) + (i > 9 && i % 10 > 0 && newBoard[i - 11] == -1);
    }
    
    return (int*) newBoard;
}
void floodFill(int startIndex) {
    struct Queue fillQueue = {0, NULL};
    pushQueue(&fillQueue, startIndex);
    
    int computed[100];
    
    while(fillQueue.length > 0) {
        int i = grabQueue(&fillQueue);
        
        mask[i] = 2;
        computed[i] = 1;
        pushQueue(&updateQueue, i);
        
        if(board[i]) continue;
        
        if(i > 9 && !mask[i - 10] && !computed[i - 10]) pushQueue(&fillQueue, i - 10);
        if(i > 9 && i % 10 < 9 && !mask[i - 9] && !computed[i - 9]) pushQueue(&fillQueue, i - 9);
        if(i % 10 < 9 && !mask[i + 1] && !computed[i + 1]) pushQueue(&fillQueue, i + 1);
        if(i < 90 && i % 10 < 9 && !mask[i + 11] && !computed [i + 11]) pushQueue(&fillQueue, i + 11);
        if(i < 90 && !mask[i + 10] && !computed[i + 10]) pushQueue(&fillQueue, i + 10);
        if(i < 90 && i % 10 > 0 && !mask[i + 9] && !computed[i + 9]) pushQueue(&fillQueue, i + 9);
        if(i % 10 > 0 && !mask[i - 1] && !computed[i - 1]) pushQueue(&fillQueue, i - 1);
        if(i > 9 && i % 10 > 0 && !mask[i - 11] && !computed[i - 11]) pushQueue(&fillQueue, i - 11);
    }
}

int leftLast = 0, upLast = 0, rightLast = 0, downLast = 0, uncoverLast = 0, flagLast = 0;
int lastSelX = 0, lastSelY = 0;
clock_t restartTime;
int restart = 0;
void update() {
    if(restart) {
        if((clock() - restartTime) / CLOCKS_PER_SEC < 5) return;
        
        restart = 0;
        srand(time(NULL));
        for(int i = 0; i < 100; i++)
            mask[i] = board[i] = 0;
        board = generateBoard();
        
        leftLast = upLast = rightLast = downLast = uncoverLast = flagLast = lastSelX = lastSelY = selX = selY = minesDiscovered = 0;
        updateQueue.length = 0;
        updateQueue.values = NULL;
        
        gfx_FillScreen(1);
        drawBoard(board, mask, selX, selY, minesDiscovered);
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
    
    if(uncover && !uncoverLast && !mask[10 * selY + selX]) {
        mask[10 * selY + selX] = 2;
        
        if(board[10 * selY + selX] == 0)
            floodFill(10 * selY + selX);
        else {
            if(board[10 * selY + selX] == -1) {
                restartTime = clock();
                restart = 1;
            }
            
            pushQueue(&updateQueue, 10 * selY + selX);
        }
    }
    else if(flag && !flagLast && mask[10 * selY + selX] == 0 && minesDiscovered < 10) {
        mask[10 * selY + selX] = 1;
        pushQueue(&updateQueue, 10 * selY + selX);
        minesDiscovered++;
        drawMineCount(minesDiscovered);
    }
    else if(flag && !flagLast && mask[10 * selY + selX] == 1) {
        mask[10 * selY + selX] = 0;
        pushQueue(&updateQueue, 10 * selY + selX);
        minesDiscovered--;
        drawMineCount(minesDiscovered);
    }
    
    if(selX != lastSelX || selY != lastSelY) {
        pushQueue(&updateQueue, 10 * lastSelY + lastSelX);
        pushQueue(&updateQueue, 10 * selY + selX);
    }
    
    if(updateQueue.length > 0) {
        for(int i = 0; i < updateQueue.length; i++) {
            int index = updateQueue.values[i];
            drawTile(index % 10, index / 10, board, mask, selX, selY);
        }
        updateQueue.length = 0;
        free(updateQueue.values);
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

int main() {
    srand(time(NULL)); //set seed for rand
    
    setupGraphics();
    
    board = generateBoard();
    mask = malloc(100 * sizeof(int));
    drawBoard(board, mask, selX, selY, minesDiscovered);
    
    do {
        kb_Scan();
        
        update();
    }
    while(!kb_IsDown(kb_KeyDel));
    
    free(mask);
    
    gfx_End();
    return 0;
}