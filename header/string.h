/**
 * @file string.h
 * @brief String data type definition
 */
#ifndef STRING_H
#define STRING_H

typedef struct {
    char* str;               // string
    unsigned long size;      // string size
    unsigned long capacity;  // pointer capacity
} String;

void stringInit(String* s);
void stringDestroy(String* s);

void stringAppendChar(String* s, char c);
void stringAppendCstr(String* s, const char* cstr);
void stringAppendInt(String* s, int integer);

void stringOverwrite(String* s, const char cstr[], unsigned long size);

void _stringExpand(String* s, unsigned long newCapacity);

#endif  // STRING_H