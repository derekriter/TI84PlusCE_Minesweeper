#pragma once

#include <cstdint>
#include "global.hpp"

namespace IO {
    struct Save {
        bool saveDisabled;
        bool hasGame;
        uint8_t skin;
        uint8_t gameW, gameH;
        uint16_t mineCount;
        uint8_t cursorX, cursorY;
        uint8_t scrollX, scrollY;
        int8_t* board;
        uint8_t* mask;
        uint16_t flagsLeft;
    };

    extern const char* SAVE_NAME;
    extern const struct Save& DEFAULT;

    /*
     * =====================
     * Save Format
     * =====================
     * 0: flags (uint8_t)
     * 1: skin (uint8_t)
     * ?2: gameW (uint8_t)
     * ?3: gameH (uint8_t)
     * ?4: mineCount (uint16_t)
     * ?6: cursorX (uint8_t)
     * ?7: cursorY (uint8_t)
     * ?8: scrollX (uint8_t)
     * ?9: scrollY (uint8_t)
     * ?10+: gameState (uint8_t[])
     *
     * =====================
     * Flags Format
     * =====================
     * 000000GD
     * D: Save disabled
     * G: Has game
     *
     * =====================
     * Game State Format
     * =====================
     * Per Byte:
     * 00MMBBBB
     * M (2 bits): The mask for the tile. 0 = covered, 1 = flagged, 2 = uncovered
     * B (4 bits): The board for the title. 0 = empty, 1-8 = numbers, 15 = mine
     */

    [[nodiscard]] Save load();
    void saveWithoutGame();
    void saveWithGame(const Global::GameData& gd);
    void saveDisabled();
    void save();
}
