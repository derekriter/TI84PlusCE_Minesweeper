#include <keypadc.h>
#include <time.h>

#include "include/draw.hpp"
#include "include/global.hpp"
#include "include/menu.hpp"
#include "include/game.hpp"

int main() {
    srand(time(nullptr));
    Global::loadFromIO();
    Draw::init();

    bool delLast = true;
    while(!Global::shouldClose) {
        kb_Scan();

        switch(Global::currentScene) {
            case Global::MENU: {
                Menu::update();
                break;
            }
            case Global::GAME: {
                Game::update();
                break;
            }
        }

        bool del = kb_IsDown(kb_KeyDel);
        if(del && !delLast) {
            switch(Global::currentScene) {
                case Global::MENU: {
                    Global::shouldClose = true;
                    continue;
                }
                case Global::GAME: {
                    Game::end(true);
                    Global::currentScene = Global::MENU;
                    Menu::cursorPos = 0;
                    Global::loadFromIO();

                    Draw::redrawFull = true;
                    break;
                }
            }
        }

        Draw::render();

        delLast = del;
    }

    Draw::end();
    return 0;
}
