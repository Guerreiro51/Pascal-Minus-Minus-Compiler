#include "../header/vector.h"
#include <stdlib.h>

void stringInit(String* s) {
    s->size = 0;
    s->capacity = 4;
    s->str = (char *)malloc(s->capacity*sizeof(char));
    s->str[0] = '\0';
}

void expand(String* s, unsigned long newCapacity) {
    char* newBuff = (char *)malloc(newCapacity*sizeof(char));
    for(int i = 0; i <= s->size; i++)
        newBuff[i] = s->str[i];
    free(s->str);
    s->str = newBuff;
    s->capacity = newCapacity;
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

void writeToString(String* s, const char cstr[], int size) {
    if(s->capacity < size)
        expand(s, size + 1);

    for(int i = 0; i < size; i++) 
        s->str[i] = cstr[i];
}