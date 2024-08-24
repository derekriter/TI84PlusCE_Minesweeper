#include "include/draw.hpp"

#include <graphx.h>
#include <cstring>
#include <cstdio>

#include "gfx/gfx.h"
#include "include/global.hpp"
#include "include/menu.hpp"
#include "include/game.hpp"

const uint8_t Draw::COL_WHITE = 0;
const uint8_t Draw::COL_BLACK = 1;
const uint8_t Draw::COL_YELLOW = 2;
const uint8_t Draw::COL_GRAY = 3;
const struct Draw::Skin Draw::skins[] = {
    {"Classic", classic_sprites_tiles, standard_title, Draw::COL_WHITE, Draw::COL_BLACK},
    {"Classic Dark", classic_dark_sprites_tiles, classic_dark_title, Draw::COL_GRAY, Draw::COL_WHITE},
    {"Plastic", plastic_sprites_tiles, plastic_title, Draw::COL_WHITE, Draw::COL_BLACK},
    {"Google", google_sprites_tiles, google_title, Draw::COL_WHITE, Draw::COL_BLACK},
    {"Words", words_sprites_tiles, standard_title, Draw::COL_WHITE, Draw::COL_BLACK},
    {"Colors", colors_sprites_tiles, colors_title, Draw::COL_WHITE, Draw::COL_BLACK},
    {"Roman", roman_sprites_tiles, standard_title, Draw::COL_WHITE, Draw::COL_BLACK}
};

bool Draw::redrawFull = false;
bool Draw::redrawPartial = false;
uint8_t Draw::currentSkin = 0;

void Draw::init() {
    gfx_Begin();

    gfx_SetDrawBuffer();
    gfx_SetPalette(global_palette, sizeof_global_palette, 0);
    gfx_FillScreen(Draw::getSkin().bg);
}
void Draw::end() {
    gfx_End();
}
void Draw::redraw() {
    if(!Draw::redrawFull && !Draw::redrawPartial) return;

    switch(Global::currentScene) {
        case Global::MENU: {
            const char* difficLabel = Game::currentDifficulty == Game::Difficulty::BEGINNER ? "Beginner" : (Game::currentDifficulty == Game::Difficulty::INTERMEDIATE ? "Intermediate" : (Game::currentDifficulty == Game::Difficulty::EXPERT ? "Expert" : "Insane"));
            char* difficText = (char*) malloc(strlen("Difficulty: ") + strlen(difficLabel) + 1);
            sprintf(difficText, "Difficulty: %s", difficLabel);

            char* skinText = (char*) malloc(strlen("Skin: ") + strlen(Draw::getSkin().name) + 1);
            sprintf(skinText, "Skin: %s", Draw::getSkin().name);

            int playX = Global::getCenteredTextX("Play");
            int difficX = Global::getCenteredTextX(difficText);
            int skinX = Global::getCenteredTextX(skinText);
            int quitX = Global::getCenteredTextX("Quit");

            const unsigned int cursorOffset = gfx_GetCharWidth('>') + 1;
            int minX = 0;
            unsigned int maxWidth = 0;

            gfx_SetTextFGColor(Draw::getSkin().fg);
            if(Draw::redrawFull) {
                gfx_FillScreen(Draw::getSkin().bg);

                gfx_ScaledTransparentSprite_NoClip(Draw::getSkin().title, (GFX_LCD_WIDTH - standard_title_width * 3) / 2, 30, 3, 3);

                gfx_PrintStringXY("[ARROWS] - Move", 1, GFX_LCD_HEIGHT - 36);
                gfx_PrintStringXY("[2nd] or [enter] - Select", 1, GFX_LCD_HEIGHT - 27);
                gfx_PrintStringXY("[alpha] - Flag", 1, GFX_LCD_HEIGHT - 18);
                gfx_PrintStringXY("[del] - Quit", 1, GFX_LCD_HEIGHT - 9);

                const char* versionText = strcmp(Draw::getSkin().name, "Roman") == 0 ? Global::ROMAN_VERSION : Global::VERSION;
                gfx_PrintStringXY(versionText, GFX_LCD_WIDTH - gfx_GetStringWidth(versionText) - 1, GFX_LCD_HEIGHT - 9);
            }
            else {
                gfx_SetColor(Draw::getSkin().bg);

                minX = Global::min(playX, Global::min(difficX, Global::min(skinX, quitX))) - (int) cursorOffset;
                maxWidth = Global::max(gfx_GetStringWidth("Play"), Global::max(gfx_GetStringWidth(difficText), Global::max(gfx_GetStringWidth(skinText), gfx_GetStringWidth("Quit")))) + cursorOffset;

                gfx_FillRectangle(minX, 100, (int) maxWidth, 68);
            }

            int cursorX = (Menu::cursorPos == 0 ? playX : (Menu::cursorPos == 1 ? difficX : (Menu::cursorPos == 2 ? skinX : quitX))) - (int) cursorOffset;
            gfx_SetTextXY(cursorX, Menu::cursorPos * 20 + 100);
            gfx_PrintChar('>');

            gfx_PrintStringXY("Play", playX, 100);
            gfx_PrintStringXY(difficText, difficX, 120);
            gfx_PrintStringXY(skinText, skinX, 140);
            gfx_PrintStringXY("Quit", quitX, 160);

            free(difficText);
            free(skinText);

            if(Draw::redrawFull)
                gfx_SwapDraw();
            else
                gfx_BlitRectangle(gfx_buffer, minX, 100, maxWidth, 68);
            break;
        }
        case Global::GAME: {
            break;
        }
    }

    Draw::redrawFull = Draw::redrawPartial = false;
}
struct Draw::Skin Draw::getSkin() {
    return skins[currentSkin];
}
