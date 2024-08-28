#pragma once

#include <cstdint>
#include <graphx.h>

namespace Draw {
    struct Skin {
        const char* name;
        gfx_sprite_t** sprites;
        gfx_sprite_t* title;
        gfx_sprite_t* arrow;
        uint8_t bg, fg, prompt;
    };

    const uint8_t COL_TRANS = 0;
    const uint8_t COL_WHITE = 1;
    const uint8_t COL_BLACK = 2;
    const uint8_t COL_YELLOW = 3;
    const uint8_t COL_GRAY = 4;
    const uint8_t COL_SILVER = 5;
    const uint8_t COL_PINK = 6;
    const uint8_t SPRITE_COVERED = 0;
    const uint8_t SPRITE_REVEALED = 1;
    const uint8_t SPRITE_FLAG = 2;
    const uint8_t SPRITE_MINE = 3;
    const uint8_t SPRITE_CURSOR = 4;
    const uint8_t SPRITE_NUMBERS = 5;
    const uint8_t SPRITE_EXPLOSION = 13;

    extern const struct Skin SKINS[];
    extern const uint8_t SKIN_COUNT;

    extern bool redrawFull;
    extern bool redrawPartial;
    extern uint8_t currentSkin;

    void init();
    void end();
    void render();
    [[nodiscard]] struct Skin getSkin();
    [[nodiscard]] int getCenteredTextX(const char* text);
    void drawTile(unsigned int windowX, unsigned int windowY, uint16_t loc);
}
