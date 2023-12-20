#pragma once

#include <stdio.h>
#include <math.h>

#include <graphx.h>
#include "gfx/gfx.h"

#include "mathutils.h"

void setupGraphics() {
    gfx_Begin();
    
    gfx_SetDrawBuffer(); //using blit buffer (see https://ce-programming.github.io/toolchain/libraries/graphx.html#buffering-graphics)
    gfx_SetPalette(global_palette, sizeof_global_palette, 0);
    gfx_SetTransparentColor(0);
    gfx_SetTextFGColor(2);
    gfx_SetColor(1);
    
    gfx_FillScreen(1);
}

void drawRemainingFlags(int flagsUsed) {
    //number
    int width = gfx_GetStringWidth("00");
    gfx_FillRectangle_NoClip(GFX_LCD_WIDTH - width - 1, 18, width, 9);
    
    char* string = (char*) calloc(3, sizeof(int));
    int flagsLeft = totalMines - flagsUsed;
    sprintf(string, "%d%d", flagsLeft / 10, flagsLeft % 10);
    
    gfx_PrintStringXY(string, GFX_LCD_WIDTH - gfx_GetStringWidth(string) - 1, 18);
    free(string);
    
    //flag icon
    gfx_TransparentSprite_NoClip(sprites_tile_2, GFX_LCD_WIDTH - maxint((width - 16) / 2, 1) - 16, 1);
}
void drawRestartText() {
    char* message = "Press any key to play again";
    gfx_SetTextFGColor(3);
    gfx_PrintStringXY(message, GFX_LCD_WIDTH / 2 - gfx_GetStringWidth(message) / 2, 210);
    gfx_SetTextFGColor(2);
}
void drawTile(int x, int y, int* board, int* mask, int selX, int selY, int lost) {
    int tileX = (GFX_LCD_WIDTH - boardWidth * 16) / 2 + x * 16;
    int tileY = (GFX_LCD_HEIGHT - boardHeight * 16) / 2 + y * 16;
    
    switch(mask[boardWidth * y + x]) {
        case MASK_COVERED:
            gfx_Sprite_NoClip(sprites_tile_0, tileX, tileY);
            break;
        case MASK_FLAGGED:
            gfx_Sprite_NoClip(sprites_tile_0, tileX, tileY);
            gfx_TransparentSprite_NoClip(sprites_tile_2, tileX, tileY);
            break;
        case MASK_UNCOVERED:
            gfx_Sprite_NoClip(sprites_tile_1, tileX, tileY);
            
            if(board[boardWidth * y + x] == BOARD_MINE) {
                gfx_sprite_t* displayMode = lost ? sprites_tile_13 : sprites_tile_3;
                gfx_TransparentSprite_NoClip(displayMode, tileX, tileY);
            }
            else if(board[boardWidth * y + x] > BOARD_CLEAR)
                gfx_TransparentSprite_NoClip(sprites_tiles[4 + board[boardWidth * y + x]], tileX, tileY);
            
            break;
        default:
            exit(1);
            break;
    }
    
    if(x == selX && y == selY) {
        gfx_TransparentSprite_NoClip(sprites_tile_4, tileX, tileY);
    }
}
void drawBoard(int* board, int* mask, int selX, int selY, int flagsUsed, int lost) {
    gfx_FillScreen(1);
    
    for(int y = 0; y < boardHeight; y++) {
        for(int x = 0; x < boardWidth; x++) {
            drawTile(x, y, board, mask, selX, selY, lost);
        }
    }
    
    gfx_PrintStringXY("[del] - Quit", 1, GFX_LCD_HEIGHT - 9);
    
    drawRemainingFlags(flagsUsed);
}

int titleX = 95; //(GFX_LCD_WIDTH - title_width * 3) / 2
void drawVersion() {
    int width = gfx_GetStringWidth(VERSION);
    
    gfx_PrintStringXY(VERSION, GFX_LCD_WIDTH - width - 1, GFX_LCD_HEIGHT - 9);
}
void drawCursor(int cursorPos) {
    int width = gfx_GetCharWidth('>');
    gfx_FillRectangle_NoClip(titleX - width - 1, 100, width, 60);
    
    gfx_PrintStringXY(">", titleX - width - 1, cursorPos * 20 + 100);
}
void drawDiffic() {
    int prefixWidth = gfx_GetStringWidth("Difficulty: ");
    
    gfx_FillRectangle_NoClip(titleX + prefixWidth, 120, gfx_GetStringWidth("Intermediate"), 8); //clear by widest possible output "Intermediate"
    gfx_PrintStringXY(difficulty == DIFFIC_BEGINNER ? "Beginner" : (difficulty == DIFFIC_INTERMEDIATE ? "Intermediate" : "Expert"), titleX + prefixWidth, 120);
}
void drawMenu(int cursorPos) {
    gfx_FillScreen(1);
    
    gfx_ScaledTransparentSprite_NoClip(title, titleX, 30, 3, 3);
    drawVersion();
    
    drawCursor(cursorPos);
    gfx_PrintStringXY("Play", titleX, 100);
    gfx_PrintStringXY("Difficulty:", titleX, 120);
    drawDiffic();
    gfx_PrintStringXY("Quit", titleX, 140);
    gfx_PrintStringXY("    Controls:", titleX, 170);
    gfx_PrintStringXY("      [ARROWS] - Move", titleX, 179);
    gfx_PrintStringXY("      [2nd] or [enter] - Select", titleX, 188);
    gfx_PrintStringXY("      [alpha] - Flag", titleX, 197);
    gfx_PrintStringXY("      [del] - Quit", titleX, 206);
}
