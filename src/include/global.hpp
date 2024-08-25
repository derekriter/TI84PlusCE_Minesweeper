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
    extern bool shouldClose;

    int minI(int a, int b);
    int maxI(int a, int b);
    unsigned int minUI(unsigned int a, unsigned int b);
    unsigned int maxUI(unsigned int a, unsigned int b);
    int mod(int a, int b);
    unsigned int digitCount(int n);
}
