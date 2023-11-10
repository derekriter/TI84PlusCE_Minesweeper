#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

#include <keypadc.h>
#include <graphx.h>

#include "gfx/gfx.h"

struct Queue {
    int length;
    int* values;
};
void pushQueue(struct Queue* queue, int item) {
    if(queue->length == 0) {
        queue->values = malloc(sizeof(int));
        queue->values[0] = item;
    }
    else {
        queue->values = realloc(queue->values, (queue->length + 1) * sizeof(int));
        queue->values[queue->length] = item;
    }
    queue->length++;
}
int grabQueue(struct Queue* queue) {
    int grab = queue->values[0];
    for(int i = 0; i < queue->length - 1; i++) {
        queue->values[i] = queue->values[i + 1];
    }
    queue->length--;
    
    if(queue->length == 0) {
        free(queue->values);
        queue->values = NULL;
    }
    else {
        queue->values = realloc(queue->values, queue->length * sizeof(int));
    }
    
    return grab;
}

int selX = 0, selY = 0;
int minesDiscovered = 0;
int* board = NULL;
int* mask = NULL;
struct Queue updateQueue = {0, NULL};

int randInt(int min, int max) {
    return rand() % (max - min + 1) + min;
}
int mod(int a, int b) {
    return a - b * floor((double) a / b);
}
void debugInt(int n) {
    char* debug = malloc((int) ((ceil(log10(n)) + 1) * sizeof(char)));
    sprintf(debug, "%d", n);
    
    gfx_FillRectangle_NoClip(100, GFX_LCD_HEIGHT - 9, GFX_LCD_WIDTH - 101, 8);
    gfx_PrintStringXY(debug, 100, GFX_LCD_HEIGHT - 9);
    
    free(debug);
}

void drawMineCount() {
    int width = gfx_GetStringWidth("10/10");
    gfx_FillRectangle_NoClip(GFX_LCD_WIDTH - width - 1, 1, width, 9);
    
    char* minesString = (char*) malloc((int) ((ceil(log10(minesDiscovered)) + 1) * sizeof(char)) + 3);
    sprintf(minesString, "%d/10", minesDiscovered);
    
    gfx_PrintStringXY(minesString, GFX_LCD_WIDTH - gfx_GetStringWidth(minesString) - 1, 1);
    free(minesString);
}
void drawTile(int x, int y) {
    if(!board || !mask) exit(-1);
    
    int tileX = 80 + x * 16;
    int tileY = 40 + y * 16;
    
    switch(mask[10 * y + x]) {
        case 0:
            gfx_Sprite_NoClip(sprites_tile_0, tileX, tileY);
            break;
        
        case 1:
            gfx_Sprite_NoClip(sprites_tile_0, tileX, tileY);
            gfx_TransparentSprite_NoClip(sprites_tile_2, tileX, tileY);
            break;
        
        case 2:
            gfx_Sprite_NoClip(sprites_tile_1, tileX, tileY);
            
            if(board[10 * y + x] == -1)
                gfx_TransparentSprite_NoClip(sprites_tile_3, tileX, tileY);
            else if(board[10 * y + x] > 0)
                gfx_TransparentSprite_NoClip(sprites_tiles[4 + board[10 * y + x]], tileX, tileY);
            
            break;
    }
    
    if(x == selX && y == selY) {
        gfx_TransparentSprite_NoClip(sprites_tile_4, tileX, tileY);
        debugInt(board[10 * selY + selX]);
    }
}
void drawBoard() {
    if(!board || !mask) exit(-1);
    
    for(int y = 0; y < 10; y++) {
        for(int x = 0; x < 10; x++) {
            drawTile(x, y);
        }
    }
    
    gfx_PrintStringXY("[ARROWS] - Select", 1, 1);
    gfx_PrintStringXY("[2nd] - Uncover", 1, 11);
    gfx_PrintStringXY("[Alpha] - Flag", 1, 21);
    gfx_PrintStringXY("[Del] - Quit", 1, GFX_LCD_HEIGHT - 9);
    
    drawMineCount();
}
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
        drawBoard();
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
        drawMineCount();
    }
    else if(flag && !flagLast && mask[10 * selY + selX] == 1) {
        mask[10 * selY + selX] = 0;
        pushQueue(&updateQueue, 10 * selY + selX);
        minesDiscovered--;
        drawMineCount();
    }
    
    if(selX != lastSelX || selY != lastSelY) {
        pushQueue(&updateQueue, 10 * lastSelY + lastSelX);
        pushQueue(&updateQueue, 10 * selY + selX);
    }
    
    if(updateQueue.length > 0) {
        for(int i = 0; i < updateQueue.length; i++) {
            int index = updateQueue.values[i];
            drawTile(index % 10, index / 10);
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
    srand(time(NULL));
    
    gfx_Begin();
    gfx_SetPalette(global_palette, sizeof_global_palette, 0);
    gfx_SetTransparentColor(0);
    gfx_SetTextFGColor(2);
    gfx_SetColor(1);
    
    gfx_FillScreen(1);
    board = generateBoard();
    mask = malloc(100 * sizeof(int));
    drawBoard();
    
    do {
        kb_Scan();
        
        update();
    }
    while(!kb_IsDown(kb_KeyDel));
    
    free(mask);
    
    gfx_End();
    return 0;
}