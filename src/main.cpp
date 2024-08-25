#include <keypadc.h>
#include <time.h>

#include "include/draw.hpp"
#include "include/global.hpp"
#include "include/menu.hpp"
#include "include/game.hpp"

int main() {
    srand(time(nullptr));
    Draw::init();

    bool delLast = true;
    while(!Global::shouldClose) {
        kb_Scan();

        bool del = kb_IsDown(kb_KeyDel);
        if(del && !delLast) {
            switch(Global::currentScene) {
                case Global::MENU: {
                    Global::shouldClose = true;
                    continue;
                }
                case Global::GAME: {
                    Game::end();
                    Global::currentScene = Global::MENU;
                    Draw::redrawFull = true;
                }
            }
        }

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

        Draw::render();

        delLast = del;
    }

    Draw::end();
    return 0;
}
