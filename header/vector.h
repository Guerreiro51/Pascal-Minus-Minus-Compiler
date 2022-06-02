#ifndef VECTOR_H
#define VECTOR_H

typedef struct {
    char* str;
    int size;
    int capacity;
} String;

void stringInit(String* s);
void append(String* s, char c);
void writeToString(String* s, const char cstr[], int size);
void _expand(String* s, unsigned long newCapacity);

#endif  // VECTOR_H