#pragma once

#include <stdio.h>
#include <math.h>

#include <graphx.h>

void debugInt(int n) {
    char* debug = (char*) malloc((int) ((ceil(log10(n)) + 1) * sizeof(char)));
    sprintf(debug, "%d", n);
    
    gfx_FillRectangle_NoClip(100, GFX_LCD_HEIGHT - 9, GFX_LCD_WIDTH - 101, 8);
    gfx_PrintStringXY(debug, 100, GFX_LCD_HEIGHT - 9);
    
    free(debug);
}
