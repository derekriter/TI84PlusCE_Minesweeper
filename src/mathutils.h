#pragma once

#include <math.h>
#include <stdlib.h>

int randInt(int min, int max) {
    return rand() % (max - min + 1) + min;
}
int mod(int a, int b) {
    return a - b * floor((double) a / b);
}
int maxint(int a, int b) {
    return a > b ? a : b;
}
void fillIntArray(int* array, int length, int value) {
    for(int i = 0; i < length; i++)
        array[i] = value;
}
