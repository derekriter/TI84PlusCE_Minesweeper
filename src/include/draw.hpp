#pragma once

#include <cstdint>
#include <graphx.h>

namespace Draw {
    struct Skin {
        const char* name;
        gfx_sprite_t** sprites;
        gfx_sprite_t* title;
        uint8_t bg, fg;
    };

    extern const uint8_t COL_WHITE;
    extern const uint8_t COL_BLACK;
    extern const uint8_t COL_YELLOW;
    extern const uint8_t COL_GRAY;
    extern const struct Skin skins[];

    extern bool redrawFull;
    extern bool redrawPartial;
    extern uint8_t currentSkin;

    void init();
    void end();
    void redraw();
    struct Skin getSkin();
}
