#include "include/game.hpp"
#include "include/draw.hpp"
#include "include/global.hpp"

#include <cstdint>
#include <cstdlib>
#include <keypadc.h>
#include <cstring>
#include <cmath>

Game::Difficulty Game::currentDifficulty = BEGINNER;
bool Game::hasInited = false;
uint8_t* Game::board = nullptr;
uint8_t* Game::mask = nullptr;
uint8_t Game::boardW = 0;
uint8_t Game::boardH = 0;
uint16_t Game::boardArea = 0;
uint16_t Game::totalMines = 0;
uint8_t Game::cursorX = 0;
uint8_t Game::cursorY = 0;
bool* Game::redrawTiles = nullptr;
bool Game::upLast = true, Game::leftLast = true, Game::downLast = true, Game::rightLast = true, Game::selectLast = true, Game::flagLast = true;
uint16_t Game::flagsLeft = 0;
Game::State Game::currentState = Game::PLAYING;

void Game::init() {
    if(hasInited)
        return;

    switch(currentDifficulty) {
        case BEGINNER: {
            boardW = 8;
            boardH = 8;
            totalMines = 10;
            break;
        }
        case INTERMEDIATE: {
            boardW = 11;
            boardH = 11;
            totalMines = 20;
            break;
        }
        case EXPERT: {
            boardW = 16;
            boardH = 13;
            totalMines = 40;
            break;
        }
        case INSANE: {
            boardW = 16;
            boardH = 13;
            totalMines = 104;
            break;
        }
    }
    boardArea = boardW * boardH;
    flagsLeft = totalMines;

    cursorX = (boardW - 1) / 2;
    cursorY = (boardH - 1) / 2;
    upLast = leftLast = downLast = rightLast = selectLast = flagLast = true;
    currentState = Game::PLAYING;

    mask = (uint8_t*) calloc(boardArea, sizeof(uint8_t));
    redrawTiles = (bool*) calloc(boardArea, sizeof(bool));

    Draw::redrawFull = true;
    hasInited = true;
}
void Game::end() {
    if(!hasInited)
        return;

    if(board != nullptr) {
        free(board);
        board = nullptr;
    }
    if(mask != nullptr) {
        free(mask);
        mask = nullptr;
    }
    if(redrawTiles != nullptr) {
        free(redrawTiles);
        redrawTiles = nullptr;
    }

    hasInited = false;
}
void Game::update() {
    if(!hasInited)
        init();

    if(currentState != Game::PLAYING) {
        if(kb_AnyKey() && !selectLast) {
            Game::end();
            Game::init();
        }

        selectLast = kb_IsDown(kb_Key2nd) || kb_IsDown(kb_KeyEnter);
        return;
    }

    bool up = kb_IsDown(kb_KeyUp);
    bool left = kb_IsDown(kb_KeyLeft);
    bool down = kb_IsDown(kb_KeyDown);
    bool right = kb_IsDown(kb_KeyRight);
    bool select = kb_IsDown(kb_Key2nd) || kb_IsDown(kb_KeyEnter);
    bool flag = kb_IsDown(kb_KeyAlpha);

    if(up && !upLast) {
        redrawTiles[cursorX + cursorY * boardW] = true;
        cursorY = Global::mod(cursorY - 1, boardH);
        redrawTiles[cursorX + cursorY * boardW] = true;
        Draw::redrawPartial = true;
    }
    if(left && !leftLast) {
        redrawTiles[cursorX + cursorY * boardW] = true;
        cursorX = Global::mod(cursorX - 1, boardW);
        redrawTiles[cursorX + cursorY * boardW] = true;
        Draw::redrawPartial = true;
    }
    if(down && !downLast) {
        redrawTiles[cursorX + cursorY * boardW] = true;
        cursorY = Global::mod(cursorY + 1, boardH);
        redrawTiles[cursorX + cursorY * boardW] = true;
        Draw::redrawPartial = true;
    }
    if(right && !rightLast) {
        redrawTiles[cursorX + cursorY * boardW] = true;
        cursorX = Global::mod(cursorX + 1, boardW);
        redrawTiles[cursorX + cursorY * boardW] = true;
        Draw::redrawPartial = true;
    }
    uint16_t cursorLoc = cursorX + cursorY * boardW;

    if(select && !selectLast) {
        if(board == nullptr)
            genBoard();

        reveal(cursorLoc);
    }
    if(flag && !flagLast && mask[cursorLoc] != MASK_REVEALED) {
        if(mask[cursorLoc] == MASK_COVERED && flagsLeft > 0) {
            mask[cursorLoc] = MASK_FLAGGED;
            flagsLeft--;
            redrawTiles[cursorLoc] = true;
            Draw::redrawFull = true;
        }
        else if(mask[cursorLoc] == MASK_FLAGGED) {
            mask[cursorLoc] = MASK_COVERED;
            flagsLeft++;
            redrawTiles[cursorLoc] = true;
            Draw::redrawFull = true;
        }
    }

    upLast = up;
    leftLast = left;
    downLast = down;
    rightLast = right;
    selectLast = select;
    flagLast = flag;
}
int16_t Game::getNeighborTile(uint16_t tile, uint8_t neighbor) {
    auto nX = (int16_t) (tile % boardW);
    auto nY = (int16_t) (tile / boardW);

    switch(neighbor) {
        default:
        case 0: {
            nX--;
            nY--;
            break;
        }
        case 1: {
            nY--;
            break;
        }
        case 2: {
            nX++;
            nY--;
            break;
        }
        case 3: {
            nX++;
            break;
        }
        case 4: {
            nX++;
            nY++;
            break;
        }
        case 5: {
            nY++;
            break;
        }
        case 6: {
            nX--;
            nY++;
            break;
        }
        case 7: {
            nX--;
            break;
        }
    }

    if(nX < 0 || nX >= boardW || nY < 0 || nY >= boardH)
        return -1;
    return nX + nY * boardW;
}
void Game::genBoard() {
    if(board != nullptr)
        memset(board, 0, boardArea * sizeof(uint8_t));
    else
        board = (uint8_t*) calloc(boardArea, sizeof(uint8_t));

    for(int i = 0; i < totalMines; i++) {
        do {
            auto loc = (uint16_t) round((double) rand() / (double) RAND_MAX * (boardArea - 1));
            uint8_t locX = loc % boardW;
            uint8_t locY = loc / boardW;

            if(board[loc] != BOARD_MINE && (abs(locX - cursorX) >= 2 || abs(locY - cursorY) >= 2)) {
                board[loc] = BOARD_MINE;

                for(int j = 0; j < 8; j++) {
                    int16_t nLoc = getNeighborTile(loc, j);

                    if(nLoc == -1 || board[nLoc] == BOARD_MINE)
                        continue;
                    board[nLoc]++;
                }
                break;
            }
        }
        while(true);
    }
}
void Game::reveal(uint16_t loc, bool isOG) {
    if(mask[loc] == MASK_REVEALED && isOG && board[loc] > BOARD_CLEAR) {
        uint8_t known = 0;
        for(int i = 0; i < 8; i++) {
            int16_t nLoc = getNeighborTile(loc, i);

            if(nLoc == -1)
                continue;
            known += mask[nLoc] == MASK_FLAGGED;
        }

        if(known >= board[loc]) {
            for(int i = 0; i < 8; i++) {
                int16_t nLoc = getNeighborTile(loc, i);

                if(nLoc == -1)
                    continue;
                if(mask[nLoc] == MASK_COVERED)
                    reveal(nLoc, false);
            }
        }
    }
    else if(mask[loc] != MASK_COVERED)
        return;

    mask[loc] = MASK_REVEALED;
    redrawTiles[loc] = true;
    Draw::redrawPartial = true;

    if(board[loc] == BOARD_CLEAR) {
        for(int i = 0; i < 8; i++) {
            int16_t nLoc = getNeighborTile(loc, i);

            if(nLoc == -1)
                continue;
            reveal(nLoc, false);
        }
    }

    //only run once after recursion has finished
    if(isOG) {
        bool won = true;
        for(int i = 0; i < boardArea; i++) {
            if(board[i] == BOARD_MINE) {
                if(mask[i] != MASK_REVEALED)
                    continue;
                else {
                    currentState = Game::LOST;
                    Draw::redrawFull = true;
                    return;
                }
            }
            if(mask[i] == MASK_REVEALED)
                continue;

            won = false;
        }
        if(won) {
            currentState = Game::WON;
            Draw::redrawFull = true;
        }
    }
}
