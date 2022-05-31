#ifndef VECTOR_H
#define VECTOR_H

typedef struct {
    char* str;
    int size;
    int capacity;
} String;

void stringInit(String* s);
void expand(String* s, unsigned long newCapacity);
void append(String* s, char c);
void writeToString(String* s, const char cstr[], int size);

#endif // VECTOR_H