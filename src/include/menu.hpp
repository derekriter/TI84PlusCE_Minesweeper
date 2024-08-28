#pragma once

#include <cstdint>

namespace Menu {
    extern uint8_t cursorPos;
    extern bool upLast, downLast, selectLast, toggleLast;

    void update();
}
