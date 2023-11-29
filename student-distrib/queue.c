#include "queue.h"

void create_queue(){
    top = arr;
    bottom = arr;
}

void enqueue(int function){
    if(top < (int* )(arr + 6)){
        *top = function;
        top += 1;
    }
}

int dequeue(){
    if(top != bottom){
        int val = *top;
        top -= 1;
        return val;
    }
    return -1;
}