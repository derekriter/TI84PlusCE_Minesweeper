#include "include/global.hpp"
#include "include/draw.hpp"

#include <cmath>

const char* Global::VERSION = "v2.2 BETA";
const char* Global::ROMAN_VERSION = "vII.II BETA";

Global::Scene Global::currentScene = MENU;
bool Global::shouldClose = false;

int Global::min(int a, int b) {
    return a < b ? a : b;
}
int Global::max(int a, int b) {
    return a > b ? a : b;
}
int Global::mod(int a, int b) {
    //Donald Knuth implementation using floored division
    //https://en.wikipedia.org/wiki/Modulo
    return a - b * (int) floor((double) a / (double) b);
}
