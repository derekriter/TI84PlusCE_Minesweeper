#pragma once

#include <cstdint>

namespace Global {
    enum Scene {
        MENU,
        GAME
    };

    extern const char* VERSION;
    extern const char* ROMAN_VERSION;

    extern Scene currentScene;

    int getCenteredTextX(const char* text);
    int min(int a, int b);
    int max(int a, int b);
    int mod(int a, int b);
}
