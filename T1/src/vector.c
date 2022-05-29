#include "../header/vector.h"

void stringInit(String* s) {
    s->size = 0;
    s->capacity = 4;
    s->str = (char *)malloc(s->capacity*sizeof(char));
    s->str[0] = '\0';
}

void append(String* s, char c) {
    if(s->size + 1 < s->capacity) {
        s->str[s->size++] = c;
        s->str[s->size] = '\0';
    }
    else {
        s->capacity *= 2; // doubles the capacity
        s->str = (char*)realloc(s->str, s->capacity);
    }
}