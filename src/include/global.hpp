#pragma once

#include <cstdint>

#define NO_SKINS false

namespace Global {
    enum Scene {
        MENU,
        GAME
    };
    struct GameData {
        uint8_t w, h;
        uint16_t mineCount;
        uint8_t cursorX, cursorY;
        uint8_t scrollX, scrollY;
        int8_t* board;
        uint8_t* mask;
        uint16_t flagsLeft;
    };

    extern const char* VERSION;
    extern const char* ROMAN_VERSION;

    extern Scene currentScene;
    extern bool shouldClose;
    extern GameData lastGame;
    extern bool saveEnabled;

    [[nodiscard]] int minI(int a, int b);
    [[nodiscard]] int maxI(int a, int b);
    [[nodiscard]] unsigned int minUI(unsigned int a, unsigned int b);
    [[nodiscard]] unsigned int maxUI(unsigned int a, unsigned int b);
    [[nodiscard]] int mod(int a, int b);
    [[nodiscard]] unsigned int digitCount(int n);
    void loadFromIO();
}
