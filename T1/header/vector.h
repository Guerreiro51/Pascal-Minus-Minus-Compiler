#ifndef VECTOR_H
#define VECTOR_H

typedef struct {
    char* str;
    int size;
    int capacity;
} String;

void stringInit(String* s);
void append(String* s, char c);


#endif // VECTOR_H