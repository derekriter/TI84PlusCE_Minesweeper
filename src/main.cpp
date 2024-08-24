#include <keypadc.h>
#include "include/draw.hpp"
#include "include/global.hpp"
#include "include/menu.hpp"

int main() {
    Draw::init();

    while(!Global::shouldClose) {
        kb_Scan();

        if(kb_IsDown(kb_KeyDel)) {
            Global::shouldClose = true;
            continue;
        }

        switch(Global::currentScene) {
            case Global::MENU: {
                Menu::update();
                break;
            }
            case Global::GAME: {
                break;
            }
        }

        Draw::redraw();
    }

    Draw::end();
    return 0;
}
