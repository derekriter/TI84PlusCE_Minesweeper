#include "include/global.hpp"
#include "include/draw.hpp"
#include "include/io.hpp"

#include <cmath>

#if NO_SKINS
const char* Global::VERSION = "v3.0 NS BETA";
const char* Global::ROMAN_VERSION = nullptr;
#else
const char* Global::VERSION = "v3.0 BETA";
const char* Global::ROMAN_VERSION = "vIII BETA";
#endif

Global::Scene Global::currentScene = MENU;
bool Global::shouldClose = false;
Global::GameData Global::lastGame = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, nullptr, nullptr, NULL};
bool Global::saveEnabled = true;

int Global::minI(int a, int b) {
    return a < b ? a : b;
}
int Global::maxI(int a, int b) {
    return a > b ? a : b;
}
unsigned int Global::minUI(unsigned int a, unsigned int b) {
    return a < b ? a : b;
}
unsigned int Global::maxUI(unsigned int a, unsigned int b) {
    return a > b ? a : b;
}
int Global::mod(int a, int b) {
    //Donald Knuth implementation using floored division
    //https://en.wikipedia.org/wiki/Modulo
    return a - b * (int) floor((double) a / (double) b);
}
unsigned int Global::digitCount(int n) {
    if(n == 0)
        return 1;

    auto digits = (unsigned int) (floor(log10(abs(n))) + 1);
    if(n < 0)
        digits++;
    return digits;
}
void Global::loadFromIO() {
    const IO::Save save = IO::load();
    if(save.hasGame) {
        Global::lastGame = {
            save.gameW, save.gameH,
            save.mineCount,
            save.cursorX, save.cursorY,
            save.scrollX, save.scrollY,
            save.board, save.mask,
            save.flagsLeft
        };
    }
    #if !NO_SKINS
    Draw::currentSkin = save.skin;
    #endif
    saveEnabled = !save.saveDisabled;
}
