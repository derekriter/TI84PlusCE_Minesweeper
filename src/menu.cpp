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

    if(up && !upLast) {
        cursorPos = Global::mod(cursorPos - 1, 4);
        Draw::redrawPartial = true;
    }
    else if(down && !downLast) {
        cursorPos = Global::mod(cursorPos + 1, 4);
        Draw::redrawPartial = true;
    }
    else if(select && !selectLast) {
        switch(cursorPos) {
            case 0: {
                //Play
                break;
            }
            case 1: {
                //Difficulty
                Game::currentDifficulty = static_cast<Game::Difficulty>(Global::mod(Game::currentDifficulty + 1, 4));
                Draw::redrawPartial = true;
                break;
            }
            case 2: {
                //Skin
                Draw::currentSkin = Global::mod(Draw::currentSkin + 1, Draw::skinCount);
                Draw::redrawFull = true;
                break;
            }
            default:
            case 3: {
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
