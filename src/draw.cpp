#include "include/draw.hpp"

#include <graphx.h>
#include <cstring>
#include <cstdio>

#include "gfx/gfx.h"
#include "include/global.hpp"
#include "include/menu.hpp"
#include "include/game.hpp"

const struct Draw::Skin Draw::SKINS[] = {
    {"Classic", classic_sprites_tiles, standard_title, Draw::COL_WHITE, Draw::COL_BLACK},
    {"Classic Dark", classic_dark_sprites_tiles, classic_dark_title, Draw::COL_GRAY, Draw::COL_WHITE},
    {"Plastic", plastic_sprites_tiles, plastic_title, Draw::COL_WHITE, Draw::COL_BLACK},
    {"Google", google_sprites_tiles, google_title, Draw::COL_WHITE, Draw::COL_BLACK},
    {"Words", words_sprites_tiles, standard_title, Draw::COL_WHITE, Draw::COL_BLACK},
    {"Colors", colors_sprites_tiles, colors_title, Draw::COL_WHITE, Draw::COL_BLACK},
    {"Roman", roman_sprites_tiles, standard_title, Draw::COL_WHITE, Draw::COL_BLACK}
};
const uint8_t Draw::SKIN_COUNT = 7;

bool Draw::redrawFull = false;
bool Draw::redrawPartial = false;
uint8_t Draw::currentSkin = 0;

void Draw::init() {
    gfx_Begin();

    gfx_SetDrawBuffer();
    gfx_SetPalette(global_palette, sizeof_global_palette, 0);
    gfx_SetTransparentColor(COL_TRANS);
    gfx_FillScreen(Draw::getSkin().bg);

    redrawFull = true;
}
void Draw::end() {
    gfx_End();
}
void Draw::render() {
    if(!redrawFull && !redrawPartial) return;

    switch(Global::currentScene) {
        case Global::MENU: {
            const char* difficText = Game::currentDifficulty == Game::Difficulty::BEGINNER ? "Difficulty: Beginner" : (Game::currentDifficulty == Game::Difficulty::INTERMEDIATE ? "Difficulty: Intermediate" : (Game::currentDifficulty == Game::Difficulty::EXPERT ? "Difficulty: Expert" : "Difficulty: Insane"));
            const unsigned int maxDifficWidth = gfx_GetStringWidth("Difficulty: Intermediate");

            char* skinText = (char*) malloc(strlen("Skin: ") + strlen(getSkin().name) + 1);
            sprintf(skinText, "Skin: %s", getSkin().name);
            unsigned int maxSkinWidth = 0;
            for(const struct Skin& skin : SKINS) {
                maxSkinWidth = Global::maxI((int) maxSkinWidth, (int) gfx_GetStringWidth(skin.name));
            }

            int playX = getCenteredTextX("Play");
            int difficX = getCenteredTextX(difficText);
            int skinX = getCenteredTextX(skinText);
            int quitX = getCenteredTextX("Quit");

            const unsigned int cursorOffset = gfx_GetCharWidth('>') + 1;
            int minX = 0;
            unsigned int maxWidth = 0;

            gfx_SetTextFGColor(getSkin().fg);
            if(redrawFull) {
                gfx_FillScreen(getSkin().bg);

                gfx_ScaledTransparentSprite_NoClip(getSkin().title, (GFX_LCD_WIDTH - standard_title_width * 3) / 2, 30, 3, 3);

                gfx_PrintStringXY("[ARROWS] - Move", 1, GFX_LCD_HEIGHT - 36);
                gfx_PrintStringXY("[2nd] or [enter] - Select", 1, GFX_LCD_HEIGHT - 27);
                gfx_PrintStringXY("[alpha] - Flag", 1, GFX_LCD_HEIGHT - 18);
                gfx_PrintStringXY("[del] - Quit", 1, GFX_LCD_HEIGHT - 9);

                const char* versionText = strcmp(getSkin().name, "Roman") == 0 ? Global::ROMAN_VERSION : Global::VERSION;
                gfx_PrintStringXY(versionText, GFX_LCD_WIDTH - gfx_GetStringWidth(versionText) - 1, GFX_LCD_HEIGHT - 9);
            }
            else {
                gfx_SetColor(getSkin().bg);

                minX = Global::minI((int) (GFX_LCD_WIDTH - maxDifficWidth) / 2, (int) (GFX_LCD_WIDTH - maxSkinWidth) / 2) - (int) cursorOffset;
                maxWidth = Global::maxI((int) maxDifficWidth, (int) maxSkinWidth) + cursorOffset;

                gfx_FillRectangle(minX, 100, (int) maxWidth, 68);
            }

            int cursorX = (Menu::cursorPos == 0 ? playX : (Menu::cursorPos == 1 ? difficX : (Menu::cursorPos == 2 ? skinX : quitX))) - (int) cursorOffset;
            gfx_SetTextXY(cursorX, Menu::cursorPos * 20 + 100);
            gfx_PrintChar('>');

            gfx_PrintStringXY("Play", playX, 100);
            gfx_PrintStringXY(difficText, difficX, 120);
            gfx_PrintStringXY(skinText, skinX, 140);
            gfx_PrintStringXY("Quit", quitX, 160);

            free(skinText);

            if(redrawFull)
                gfx_SwapDraw();
            else
                gfx_BlitRectangle(gfx_buffer, minX, 100, maxWidth, 68);
            break;
        }
        case Global::GAME: {
            unsigned int boardX = (GFX_LCD_WIDTH - Game::boardW * 16) / 2;
            unsigned int boardY = (GFX_LCD_HEIGHT - Game::boardH * 16) / 2;

            if(redrawFull) {
                gfx_FillScreen(getSkin().bg);

                for(int i = 0; i < Game::boardArea; i++)
                    drawTile(boardX, boardY, i);

                gfx_SetTextFGColor(getSkin().fg);
                gfx_PrintStringXY("[del] - Quit", 1, GFX_LCD_HEIGHT - 9);

                char* flagText = (char*) malloc(Global::digitCount(Game::flagsLeft) + 1);
                sprintf(flagText, "%u", Game::flagsLeft);
                unsigned int flagWidth = gfx_GetStringWidth(flagText);

                gfx_TransparentSprite_NoClip(getSkin().sprites[SPRITE_FLAG], GFX_LCD_WIDTH - 17, 1);
                gfx_PrintStringXY(flagText, GFX_LCD_WIDTH - flagWidth - 1, 18);

                free(flagText);

                if(Game::currentState != Game::PLAYING) {
                    const char* restartText = "Press any key to play again";
                    unsigned int restartWidth = gfx_GetStringWidth(restartText);
                    int restartX = getCenteredTextX(restartText);

                    gfx_SetColor(getSkin().bg);
                    gfx_FillRectangle_NoClip(restartX - 1, 207, restartWidth + 1, 10);
                    gfx_SetTextFGColor(COL_YELLOW);
                    gfx_PrintStringXY(restartText, restartX, 208);
                }

                gfx_SwapDraw();
            }
            else {
                uint16_t blitX1 = Game::boardW, blitY1 = Game::boardH, blitX2 = 0, blitY2 = 0;
                for(int i = 0; i < Game::boardArea; i++) {
                    if(Game::redrawTiles[i]) {
                        blitX1 = Global::minI(blitX1, i % Game::boardW);
                        blitY1 = Global::minI(blitY1, i / Game::boardW);
                        blitX2 = Global::maxI(blitX2, i % Game::boardW);
                        blitY2 = Global::maxI(blitY2, i / Game::boardW);
                    }
                }
                gfx_BlitRectangle(gfx_screen, blitX1 * 16 + boardX, blitY1 * 16 + boardY, (blitX2 - blitX1 + 1) * 16, (blitY2 - blitY1 + 1) * 16);

                for(int i = 0; i < Game::boardArea; i++) {
                    if(Game::redrawTiles[i])
                        drawTile(boardX, boardY, i);
                }

                gfx_BlitRectangle(gfx_buffer, blitX1 * 16 + boardX, blitY1 * 16 + boardY, (blitX2 - blitX1 + 1) * 16, (blitY2 - blitY1 + 1) * 16);
            }

            if(redrawPartial)
                memset(Game::redrawTiles, false, Game::boardArea * sizeof(uint8_t));
            break;
        }
    }

    redrawFull = redrawPartial = false;
}
struct Draw::Skin Draw::getSkin() {
    return SKINS[currentSkin];
}
int Draw::getCenteredTextX(const char* text) {
    return (int) (GFX_LCD_WIDTH - gfx_GetStringWidth(text)) / 2;
}
void Draw::drawTile(unsigned int boardX, unsigned int boardY, uint16_t loc) {
    unsigned int x = loc % Game::boardW * 16 + boardX;
    unsigned int y = loc / Game::boardW * 16 + boardY;

    if(Game::currentState == Game::WON) {
        gfx_Sprite_NoClip(getSkin().sprites[SPRITE_REVEALED], x, y);

        uint8_t val = Game::board[loc];
        switch(val) {
            case Game::BOARD_MINE: {
                gfx_TransparentSprite_NoClip(getSkin().sprites[SPRITE_MINE], x, y);
                break;
            }
            case Game::BOARD_CLEAR: {
                //draw nothing
                break;
            }
            default: {
                gfx_TransparentSprite_NoClip(getSkin().sprites[SPRITE_NUMBERS + val - 1], x, y);
                break;
            }
        }
    }
    else if(Game::currentState == Game::LOST && Game::board[loc] == Game::BOARD_MINE) {
        gfx_Sprite_NoClip(getSkin().sprites[SPRITE_REVEALED], x, y);
        gfx_TransparentSprite_NoClip(getSkin().sprites[SPRITE_EXPLOSION], x, y);
    }
    else {
        switch(Game::mask[loc]) {
            default:
            case Game::MASK_COVERED: {
                gfx_Sprite_NoClip(getSkin().sprites[SPRITE_COVERED], x, y);
                break;
            }
            case Game::MASK_FLAGGED: {
                gfx_Sprite_NoClip(getSkin().sprites[SPRITE_COVERED], x, y);
                gfx_TransparentSprite_NoClip(getSkin().sprites[SPRITE_FLAG], x, y);
                break;
            }
            case Game::MASK_REVEALED: {
                gfx_Sprite_NoClip(getSkin().sprites[SPRITE_REVEALED], x, y);

                uint8_t val = Game::board[loc];
                switch(val) {
                    case Game::BOARD_MINE: {
                        gfx_TransparentSprite_NoClip(getSkin().sprites[SPRITE_MINE], x, y);
                        break;
                    }
                    case Game::BOARD_CLEAR: {
                        //draw nothing
                        break;
                    }
                    default: {
                        gfx_TransparentSprite_NoClip(getSkin().sprites[SPRITE_NUMBERS + val - 1], x, y);
                        break;
                    }
                }
            }
        }
    }

    if(loc == Game::cursorX + Game::cursorY * Game::boardW)
        gfx_TransparentSprite_NoClip(getSkin().sprites[SPRITE_CURSOR], x, y);
}
