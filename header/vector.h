/**
 * @file vector.h
 * @brief String data type definition
 */
#ifndef VECTOR_H
#define VECTOR_H

typedef struct {
    char* str;     // string
    int size;      // string size
    int capacity;  // pointer capacity
} String;

void stringInit(String* s);                                  // initializes a string
void stringDestroy(String* s);                               // clears memory
void append(String* s, char c);                              // appends to string
void appendStr(String* s, const char* cstr);
void appendInt(String* s, int integer);
void writeToString(String* s, const char cstr[], int size);  // overwrites string
void _expand(String* s, unsigned long newCapacity);          // increase pointer capacity

#endif  // VECTOR_H