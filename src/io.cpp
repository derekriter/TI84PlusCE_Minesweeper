#include <fileioc.h>
#include <debug.h>
#include "include/io.hpp"
#include "include/draw.hpp"

const char* IO::SAVE_NAME = "MSWPRSAV";
const struct IO::Save& IO::DEFAULT = {false, false, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, nullptr, nullptr, NULL};

IO::Save IO::load() {
    uint8_t handle = ti_Open(SAVE_NAME, "r");
    if(handle == 0) {
        dbg_printf("Failed to open save (LOAD), assuming defaults\n");
        return DEFAULT;
    }

    auto* data = (uint8_t*) malloc(2);
    size_t err = ti_Read(data, 1, 2, handle);
    if(err != 2) {
        dbg_printf("Unable to read save (read 1), may be corrupted. Assuming defaults\n");
        ti_Close(handle);
        free(data);
        return DEFAULT;
    }

    struct Save out = DEFAULT;
    out.saveDisabled = (bool) (data[0] & 0x1);
    out.hasGame = (bool) (data[0] & 0x2);
    out.skin = data[1];
    free(data);

    dbg_printf("--Read 1--\nSave Disabled: %u\nHas Game: %u\nSkin: %u\n", out.saveDisabled, out.hasGame, out.skin);

    if(out.skin >= Draw::SKIN_COUNT) {
        dbg_printf("Invalid save (check 1), assuming defaults\n");
        ti_Close(handle);
        return DEFAULT;
    }

    if(!out.hasGame) {
        dbg_printf("Successfully loaded save (WOG)\n");
        return out;
    }
    else {
        data = (uint8_t*) malloc(8);
        err = ti_Read(data, 1, 8, handle);
        if(err != 8) {
            dbg_printf("Unable to read save (read 2), may be corrupted. Assuming defaults\n");
            ti_Close(handle);
            free(data);
            return DEFAULT;
        }

        out.gameW = data[0];
        out.gameH = data[1];
        out.mineCount = (((uint16_t) data[2]) << 8) | (data[3]);
        out.cursorX = data[4];
        out.cursorY = data[5];
        out.scrollX = data[6];
        out.scrollY = data[7];
        free(data);

        dbg_printf("--Read 2--\nGame Dim: %ux%u\nMines: %u\nCursor: (%u, %u)\nScroll: (%u, %u)\n", out.gameW, out.gameH, out.mineCount, out.cursorX, out.cursorY, out.scrollX, out.scrollY);

        uint16_t gameArea = out.gameW * out.gameH;
        if(gameArea == 0 || out.mineCount == 0 || out.cursorX >= out.gameW || out.cursorY >= out.gameH || out.mineCount > gameArea) {
            dbg_printf("Invalid save (check 2), assuming defaults\n");
            ti_Close(handle);
            return DEFAULT;
        }

        data = (uint8_t*) malloc(gameArea);
        err = ti_Read(data, 1, gameArea, handle);
        ti_Close(handle);
        if(err != gameArea) {
            dbg_printf("Unable to read save (read 3), may be corrupted. Assuming defaults\n");
            free(data);
            return DEFAULT;
        }

        //all data will be overwritten, so it doesn't matter if data is zeroed
        out.board = (int8_t*) malloc(gameArea);
        out.mask = (uint8_t*) malloc(gameArea);
        out.flagsLeft = out.mineCount;

        uint8_t digits = Global::digitCount(gameArea);
        dbg_printf("--Read 3--\nI%*c Tile Msk  Brd\n", digits, ' ');
        for(uint16_t i = 0; i < gameArea; i++) {
            uint8_t tile = data[i];
            auto board = (int8_t) (tile & 0xF);
            uint8_t mask = tile >> 4;
            dbg_printf("%*u: 0x%02X 0x%02X 0x%02X\n", digits, i, tile, mask, board);

            if(board == 0xF)
                board = -1;
            if(board > 8 || mask > 2) {
                dbg_printf("Invalid save (check 3), assuming defaults\n");
                free(data);
                free(out.board);
                free(out.mask);
                return DEFAULT;
            }
            if(mask == 1) {
                if(out.flagsLeft > 0)
                    out.flagsLeft--;
                else {
                    dbg_printf("Invalid save (check 4), assuming defaults\n");
                    free(data);
                    free(out.board);
                    free(out.mask);
                    return DEFAULT;
                }
            }

            out.board[i] = board;
            out.mask[i] = mask;
        }

        dbg_printf("Successfully loaded save (WG)\n");
        return out;
    }
}
void IO::saveWithoutGame() {
    uint8_t handle = ti_Open(SAVE_NAME, "w");
    if(handle == 0) {
        dbg_printf("Failed to open save (WOG), aborting\n");
        return;
    }

    auto data = (uint8_t*) malloc(2);
    data[0] = 0;
    data[1] = Draw::currentSkin;

    dbg_printf("--WOG Write--\nFlags: %u%u%u%u%u%u%u%u\nSkin: %u\n", (data[0] >> 7) & 1, (data[0] >> 6) & 1, (data[0] >> 5) & 1, (data[0] >> 4) & 1, (data[0] >> 3) & 1, (data[0] >> 2) & 1, (data[0] >> 1) & 1, data[0] & 1, data[1]);

    ti_Write(data, 1, 2, handle);
    ti_Close(handle);
    free(data);
    dbg_printf("Successfully wrote to save (WOG)\n");
}
void IO::saveWithGame(const Global::GameData& gd) {
    uint8_t handle = ti_Open(SAVE_NAME, "w");
    if(handle == 0) {
        dbg_printf("Failed to open save (WG), aborting\n");
        return;
    }

    uint16_t gameArea = gd.w * gd.h;
    auto data = (uint8_t*) malloc(10 + gameArea);

    data[0] = 0b10;
    data[1] = Draw::currentSkin;
    data[2] = gd.w;
    data[3] = gd.h;
    data[4] = (uint8_t) (gd.mineCount >> 8);
    data[5] = (uint8_t) (gd.mineCount & 0xFF);
    data[6] = gd.cursorX;
    data[7] = gd.cursorY;
    data[8] = gd.scrollX;
    data[9] = gd.scrollY;

    dbg_printf("--WG Write--\nFlags: %u%u%u%u%u%u%u%u\nSkin: %u\nGame Dim: %ux%u\nMines: %u\nCursor: (%u, %u)\nScroll: (%u, %u)\n", (data[0] >> 7) & 1, (data[0] >> 6) & 1, (data[0] >> 5) & 1, (data[0] >> 4) & 1, (data[0] >> 3) & 1, (data[0] >> 2) & 1, (data[0] >> 1) & 1, data[0] & 1, data[1], data[2], data[3], (((uint16_t) data[4]) << 8) | data[5], data[6], data[7], data[8], data[9]);

    uint8_t digits = Global::digitCount(gameArea);
    dbg_printf("Game State:\nI%*c Tile Msk  Brd\n", digits, ' ');
    for(uint16_t i = 0; i < gameArea; i++) {
        uint8_t mask = gd.mask[i] << 4;
        uint8_t board = gd.board[i] & 0xF;
        data[10 + i] = mask | board;

        dbg_printf("%*u: 0x%02X 0x%02X 0x%02X\n", digits, i, data[10 + i], mask, board);
    }

    ti_Write(data, 1, 10 + gameArea, handle);
    ti_Close(handle);
    free(data);
    dbg_printf("Successfully wrote to save (WG)\n");
}
void IO::saveDisabled() {
    uint8_t handle = ti_Open(SAVE_NAME, "w");
    if(handle == 0) {
        dbg_printf("Failed to open save (D), aborting\n");
        return;
    }

    auto data = (uint8_t*) malloc(2);
    data[0] = 0b1;
    data[1] = Draw::currentSkin;

    dbg_printf("--D Write--\nFlags: %u%u%u%u%u%u%u%u\nSkin: %u\n", (data[0] >> 7) & 1, (data[0] >> 6) & 1, (data[0] >> 5) & 1, (data[0] >> 4) & 1, (data[0] >> 3) & 1, (data[0] >> 2) & 1, (data[0] >> 1) & 1, data[0] & 1, data[1]);

    ti_Write(data, 1, 2, handle);
    ti_Close(handle);
    free(data);
    dbg_printf("Successfully wrote to save (D)\n");
}
void IO::save() {
    if(!Global::saveEnabled) {
        saveDisabled();
        return;
    }

    if(Global::lastGame.w == NULL)
        saveWithoutGame();
    else
        saveWithGame(Global::lastGame);
}
