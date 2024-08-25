#include "include/menu.hpp"
#include "include/global.hpp"
#include "include/draw.hpp"
#include "include/game.hpp"

#include <keypadc.h>

uint8_t Menu::cursorPos = 0;
bool Menu::upLast = true, Menu::downLast = true, Menu::selectLast = true;

void Menu::update() {
    bool up = kb_IsDown(kb_KeyUp);
    bool down = kb_IsDown(kb_KeyDown);
    bool select = kb_IsDown(kb_Key2nd) || kb_IsDown(kb_KeyEnter);

    uint8_t optionCount;
    #if NO_SKINS
    optionCount = 3;
    #else
    optionCount = 4;
    #endif

    if(up && !upLast) {
        cursorPos = Global::mod(cursorPos - 1, optionCount);
        Draw::redrawPartial = true;
    }
    if(down && !downLast) {
        cursorPos = Global::mod(cursorPos + 1, optionCount);
        Draw::redrawPartial = true;
    }
    if(select && !selectLast) {
        switch(cursorPos) {
            case 0: {
                //Play
                Global::currentScene = Global::GAME;
                break;
            }
            case 1: {
                //Difficulty
                Game::currentDifficulty = static_cast<Game::Difficulty>(Global::mod(Game::currentDifficulty + 1, 4));
                Draw::redrawPartial = true;
                break;
            }
            #if !NO_SKINS
            case 2: {
                //Skin
                Draw::currentSkin = Global::mod(Draw::currentSkin + 1, Draw::SKIN_COUNT);
                Draw::redrawFull = true;
                break;
            }
            #endif
            default:
            #if NO_SKINS
            case 2: {
            #else
            case 3: {
            #endif
                //Quit
                Global::shouldClose = true;
                break;
            }
        }
    }

    upLast = up;
    downLast = down;
    selectLast = select;
}
