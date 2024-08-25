#pragma once

#include <cstdint>

namespace Game {
    enum Difficulty {
        BEGINNER = 0,
        INTERMEDIATE = 1,
        EXPERT = 2,
        INSANE = 3
    };

    const uint8_t MASK_COVERED = 0;
    const uint8_t MASK_FLAGGED = 1;
    const uint8_t MASK_REVEALED = 2;
    const uint8_t BOARD_MINE = -1;
    const uint8_t BOARD_CLEAR = 0;

    extern Difficulty currentDifficulty;
    extern bool hasInited;
    extern uint8_t* board;
    extern uint8_t* mask;
    extern uint8_t boardW;
    extern uint8_t boardH;
    extern uint16_t boardArea;
    extern uint16_t totalMines;
    extern uint8_t cursorX;
    extern uint8_t cursorY;
    extern bool* redrawTiles;
    extern bool upLast, leftLast, downLast, rightLast, selectLast, flagLast;
    extern uint16_t flagsLeft;

    void init();
    void end();
    void update();
    int16_t getNeighborTile(uint16_t tile, uint8_t neighbor);
    void genBoard();
}
