#include "include/global.hpp"
#include "include/draw.hpp"

#include <cmath>

const char* Global::VERSION = "v2.2 BETA";
const char* Global::ROMAN_VERSION = "vII.II BETA";

Global::Scene Global::currentScene = MENU;
bool Global::shouldClose = false;

int Global::minI(int a, int b) {
    return a < b ? a : b;
}
int Global::maxI(int a, int b) {
    return a > b ? a : b;
}
unsigned int Global::minUI(unsigned int a, unsigned int b) {
    return a < b ? a : b;
}
unsigned int Global::maxUI(unsigned int a, unsigned int b) {
    return a > b ? a : b;
}
int Global::mod(int a, int b) {
    //Donald Knuth implementation using floored division
    //https://en.wikipedia.org/wiki/Modulo
    return a - b * (int) floor((double) a / (double) b);
}
unsigned int Global::digitCount(int n) {
    if(n == 0)
        return 1;

    auto digits = (unsigned int) (floor(log10(abs(n))) + 2);
    if(n < 0)
        digits++;
    return digits;
}
