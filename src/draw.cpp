#include "include/draw.hpp"

#include <graphx.h>
#include <cstring>
#include <cstdio>

#include "gfx/gfx.h"
#include "include/global.hpp"
#include "include/menu.hpp"
#include "include/game.hpp"

#if NO_SKINS
const struct Draw::Skin Draw::SKINS[] = {
    {"Classic", classic_sprites_tiles, standard_title, standard_arrow, Draw::COL_WHITE, Draw::COL_BLACK}
};
const uint8_t Draw::SKIN_COUNT = 1;
#else
const struct Draw::Skin Draw::SKINS[] = {
    {"Classic", classic_sprites_tiles, standard_title, standard_arrow, Draw::COL_WHITE, Draw::COL_BLACK, Draw::COL_YELLOW},
    {"Dracula", dracula_sprites_tiles, dracula_title, dracula_arrow, Draw::COL_GRAY, Draw::COL_LT_GRAY, COL_PINK}, //Dracula colors from https://draculatheme.com/contribute
    {"Plastic", plastic_sprites_tiles, plastic_title, standard_arrow, Draw::COL_WHITE, Draw::COL_BLACK, Draw::COL_YELLOW},
    {"Google", google_sprites_tiles, google_title, standard_arrow, Draw::COL_WHITE, Draw::COL_BLACK, Draw::COL_YELLOW},
    {"Words", words_sprites_tiles, standard_title, words_arrow, Draw::COL_WHITE, Draw::COL_BLACK, Draw::COL_YELLOW},
    {"Colors", colors_sprites_tiles, colors_title, colors_arrow, Draw::COL_WHITE, Draw::COL_BLACK, Draw::COL_YELLOW},
    {"Roman", roman_sprites_tiles, standard_title, standard_arrow, Draw::COL_WHITE, Draw::COL_BLACK, Draw::COL_YELLOW}
};
const uint8_t Draw::SKIN_COUNT = 7;
#endif

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

            #if !NO_SKINS
            char* skinText = (char*) malloc(strlen("Skin: ") + strlen(getSkin().name) + 1);
            sprintf(skinText, "Skin: %s", getSkin().name);
            unsigned int maxSkinWidth = 0;
            for(const struct Skin& skin : SKINS) {
                maxSkinWidth = Global::maxI((int) maxSkinWidth, (int) gfx_GetStringWidth(skin.name));
            }
            #endif

            int playX = getCenteredTextX("Play");
            int difficX = getCenteredTextX(difficText);
            #if !NO_SKINS
            int skinX = getCenteredTextX(skinText);
            #endif
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

                #if NO_SKINS
                gfx_PrintStringXY(Global::VERSION, GFX_LCD_WIDTH - gfx_GetStringWidth(Global::VERSION) - 1, GFX_LCD_HEIGHT - 9);
                #else
                const char* versionText = strcmp(getSkin().name, "Roman") == 0 ? Global::ROMAN_VERSION : Global::VERSION;
                gfx_PrintStringXY(versionText, GFX_LCD_WIDTH - gfx_GetStringWidth(versionText) - 1, GFX_LCD_HEIGHT - 9);
                #endif
            }
            else {
                gfx_SetColor(getSkin().bg);

                #if NO_SKINS
                minX = (int) ((GFX_LCD_WIDTH - maxDifficWidth) / 2 - cursorOffset);
                maxWidth = maxDifficWidth + cursorOffset;
                #else
                minX = Global::minI((int) (GFX_LCD_WIDTH - maxDifficWidth) / 2, (int) (GFX_LCD_WIDTH - maxSkinWidth) / 2) - (int) cursorOffset;
                maxWidth = Global::maxI((int) maxDifficWidth, (int) maxSkinWidth) + cursorOffset;
                #endif

                gfx_FillRectangle(minX, 100, (int) maxWidth, 68);
            }

            #if NO_SKINS
            int cursorX = (Menu::cursorPos == 0 ? playX : (Menu::cursorPos == 1 ? difficX : quitX)) - (int) cursorOffset;
            #else
            int cursorX = (Menu::cursorPos == 0 ? playX : (Menu::cursorPos == 1 ? difficX : (Menu::cursorPos == 2 ? skinX : quitX))) - (int) cursorOffset;
            #endif
            gfx_SetTextXY(cursorX, Menu::cursorPos * 20 + 100);
            gfx_PrintChar('>');

            gfx_PrintStringXY("Play", playX, 100);
            gfx_PrintStringXY(difficText, difficX, 120);
            #if NO_SKINS
            gfx_PrintStringXY("Quit", quitX, 140);
            #else
            gfx_PrintStringXY(skinText, skinX, 140);
            gfx_PrintStringXY("Quit", quitX, 160);
            #endif

            #if !NO_SKINS
            free(skinText);
            #endif

            if(redrawFull)
                gfx_SwapDraw();
            else
                gfx_BlitRectangle(gfx_buffer, minX, 100, maxWidth, 68);
            break;
        }
        case Global::GAME: {
            unsigned int windowX = (GFX_LCD_WIDTH - Game::windowW * 16) / 2;
            unsigned int windowY = (GFX_LCD_HEIGHT - Game::windowH * 16) / 2;

            if(redrawFull) {
                gfx_FillScreen(getSkin().bg);

                for(int i = 0; i < Game::boardArea; i++)
                    drawTile(windowX, windowY, i);

                gfx_SetTextFGColor(getSkin().fg);
                gfx_PrintStringXY("[del] - Quit", 1, GFX_LCD_HEIGHT - 9);

                char* flagText = (char*) malloc(Global::digitCount(Game::flagsLeft) + 1);
                sprintf(flagText, "%u", Game::flagsLeft);
                unsigned int flagWidth = gfx_GetStringWidth(flagText);

                gfx_TransparentSprite_NoClip(getSkin().sprites[SPRITE_FLAG], GFX_LCD_WIDTH - 17, 1);
                gfx_PrintStringXY(flagText, GFX_LCD_WIDTH - flagWidth - 1, 18);

                free(flagText);

                if(Game::scrollX > 0) {
                    gfx_sprite_t* tempArrow = gfx_MallocSprite(standard_arrow_width, standard_arrow_height);
                    gfx_FlipSpriteY(getSkin().arrow, tempArrow);
                    gfx_TransparentSprite_NoClip(tempArrow, windowX - standard_arrow_width, (GFX_LCD_HEIGHT - standard_arrow_height) / 2);
                    free(tempArrow);
                }
                if(Game::scrollX < (int) Game::boardW - Game::windowW) {
                    gfx_TransparentSprite_NoClip(getSkin().arrow, windowX + Game::windowW * 16, (GFX_LCD_HEIGHT - standard_arrow_height) / 2);
                }
                if(Game::scrollY > 0) {
                    gfx_sprite_t* tempArrow = gfx_MallocSprite(standard_arrow_height, standard_arrow_width);
                    gfx_RotateSpriteCC(getSkin().arrow, tempArrow);
                    gfx_TransparentSprite_NoClip(tempArrow, (GFX_LCD_WIDTH - standard_arrow_height) / 2, windowY - standard_arrow_width);
                    free(tempArrow);
                }
                if(Game::scrollY < (int) Game::boardH - Game::windowH) {
                    gfx_sprite_t* tempArrow = gfx_MallocSprite(standard_arrow_height, standard_arrow_width);
                    gfx_RotateSpriteC(getSkin().arrow, tempArrow);
                    gfx_TransparentSprite_NoClip(tempArrow, (GFX_LCD_WIDTH - standard_arrow_height) / 2, windowY + Game::windowH * 16);
                    free(tempArrow);
                }

                if(Game::currentState != Game::PLAYING) {
                    const char* restartText = "Press any key to play again";
                    unsigned int restartWidth = gfx_GetStringWidth(restartText);
                    int restartX = getCenteredTextX(restartText);

                    gfx_SetColor(getSkin().bg);
                    gfx_FillRectangle_NoClip(restartX - 1, 207, restartWidth + 1, 10);
                    gfx_SetTextFGColor(getSkin().prompt);
                    gfx_PrintStringXY(restartText, restartX, 208);
                }

                char* dimText = (char*) malloc(Global::digitCount(Game::boardW) + Global::digitCount(Game::boardH) + 2);
                sprintf(dimText, "%ux%u", Game::boardW, Game::boardH);
                gfx_PrintStringXY(dimText, 1, 1);
                free(dimText);

                gfx_SwapDraw();
            }
            else {
                int16_t blitX1 = -1, blitY1 = -1, blitX2 = -1, blitY2 = -1;
                for(int i = 0; i < Game::boardArea; i++) {
                    if(Game::redrawTiles[i]) {
                        int8_t x = (int8_t) (i % Game::boardW) - Game::scrollX;
                        int8_t y = (int8_t) (i / Game::boardW) - Game::scrollY;
                        if(x < 0 || x >= Game::windowW || y < 0 || y >= Game::windowH)
                            continue;

                        blitX1 = (int16_t) Global::minI(blitX1, x);
                        blitY1 = (int16_t) Global::minI(blitY1, y);
                        blitX2 = (int16_t) Global::maxI(blitX2, x);
                        blitY2 = (int16_t) Global::maxI(blitY2, y);
                    }
                }
                if(blitX1 == -1 && blitY1 == -1 && blitX2 == -1 && blitY2 == -1)
                    break;

                gfx_BlitRectangle(gfx_screen, blitX1 * 16 + windowX, blitY1 * 16 + windowY, (blitX2 - blitX1 + 1) * 16, (blitY2 - blitY1 + 1) * 16);

                for(int i = 0; i < Game::boardArea; i++) {
                    if(Game::redrawTiles[i])
                        drawTile(windowX, windowY, i);
                }

                gfx_BlitRectangle(gfx_buffer, blitX1 * 16 + windowX, blitY1 * 16 + windowY, (blitX2 - blitX1 + 1) * 16, (blitY2 - blitY1 + 1) * 16);
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
void Draw::drawTile(unsigned int windowX, unsigned int windowY, uint16_t loc) {
    uint8_t boardX = loc % Game::boardW;
    uint8_t boardY = loc / Game::boardW;

    if(boardX < Game::scrollX || boardX >= Game::scrollX + Game::windowW || boardY < Game::scrollY || boardY >= Game::scrollY + Game::windowH)
        return;

    unsigned int x = (boardX - Game::scrollX) * 16 + windowX;
    unsigned int y = (boardY - Game::scrollY) * 16 + windowY;

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

    if(boardX == Game::cursorX && boardY == Game::cursorY)
        gfx_TransparentSprite_NoClip(getSkin().sprites[SPRITE_CURSOR], x, y);
}
