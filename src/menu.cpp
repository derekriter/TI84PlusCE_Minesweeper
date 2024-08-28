#include "include/menu.hpp"
#include "include/global.hpp"
#include "include/draw.hpp"
#include "include/game.hpp"
#include "include/io.hpp"

#include <keypadc.h>

uint8_t Menu::cursorPos = 0;
bool Menu::upLast = true, Menu::downLast = true, Menu::selectLast = true;

void Menu::update() {
    bool up = kb_IsDown(kb_KeyUp);
    bool down = kb_IsDown(kb_KeyDown);
    bool select = kb_IsDown(kb_Key2nd) || kb_IsDown(kb_KeyEnter);

    bool canContinue = Global::lastGame.w != NULL;
    uint8_t optionCount;
    #if NO_SKINS
    optionCount = 3 + canContinue;
    #else
    optionCount = 4 + canContinue;
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
        if(cursorPos == 0 && canContinue) {
            //continue
            Global::currentScene = Global::GAME;
        }
        else if(cursorPos == canContinue) {
            //play
            Global::lastGame = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, nullptr, nullptr, NULL};
            Global::currentScene = Global::GAME;
        }
        else if(cursorPos == canContinue + 1) {
            //difficulty
            Game::currentDifficulty = static_cast<Game::Difficulty>(Global::mod(Game::currentDifficulty + 1, 4));
            Draw::redrawPartial = true;
        }
        #if !NO_SKINS
        else if(cursorPos == canContinue + 2) {
        #endif
            //Skin
            Draw::currentSkin = Global::mod(Draw::currentSkin + 1, Draw::SKIN_COUNT);
            IO::save();
            Draw::redrawFull = true;
        }
        else {
            Global::shouldClose = true;
        }
    }

    upLast = up;
    downLast = down;
    selectLast = select;
}
