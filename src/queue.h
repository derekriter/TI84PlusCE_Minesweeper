#pragma once

#include <stdlib.h>

struct Queue {
    int length;
    int* values;
};
void pushQueue(struct Queue* queue, int item) {
    if(queue->length == 0) {
        queue->values = (int*) malloc(sizeof(int));
        queue->values[0] = item;
    }
    else {
        queue->values = (int*) realloc(queue->values, (queue->length + 1) * sizeof(int));
        queue->values[queue->length] = item;
    }
    queue->length++;
}
int grabQueue(struct Queue* queue) {
    int grab = queue->values[0];
    for(int i = 0; i < queue->length - 1; i++) {
        queue->values[i] = queue->values[i + 1];
    }
    queue->length--;
    
    if(queue->length == 0) {
        free(queue->values);
        queue->values = NULL;
    }
    else {
        queue->values = (int*) realloc(queue->values, queue->length * sizeof(int));
    }
    
    return grab;
}