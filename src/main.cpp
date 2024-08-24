#include <keypadc.h>
#include "include/draw.hpp"
#include "include/global.hpp"
#include "include/menu.hpp"

int main() {
    Draw::init();

    Draw::redrawFull = true;
    while(true) {
        kb_Scan();

        if(kb_IsDown(kb_KeyDel))
            break;

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
