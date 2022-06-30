/**
 * @file string.c
 * @brief String data type implementation
 */
#include "../header/string.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Allocates initial memory for a short string
 *
 * @param s the string
 */
void stringInit(String* s) {
    s->size = 0;
    s->capacity = 4;
    s->str = (char*)malloc(s->capacity * sizeof(char));
    s->str[0] = '\0';
}

/**
 * @brief Deallocates string memory used
 *
 * @param s the string
 */
void stringDestroy(String* s) {
    free(s->str);
}

/**
 * @brief Appends a character to the string
 *
 * @param s the string
 * @param c the character
 */
void stringAppendChar(String* s, char c) {
    if (s->size + 1 < s->capacity) {
        s->str[s->size++] = c;
        s->str[s->size] = '\0';
    } else {
        _stringExpand(s, s->capacity * 2);  // doubles the capacity
        s->str[s->size++] = c;
        s->str[s->size] = '\0';
    }
}

/**
 * @brief Appends a C string to the string
 *
 * @param s the string
 * @param cstr the C string
 */
void stringAppendCstr(String* s, const char* cstr) {
    for (size_t i = 0; i < strlen(cstr); i++)
        stringAppendChar(s, cstr[i]);
}

/**
 * @brief Appends an integer to the string
 *
 * @param s the string
 * @param integer the integer
 */
void stringAppendInt(String* s, int integer) {
    static char cstrInt[12];  // INT_MIN is -2.147.483.648, so it fits in 12 bits (with signal)
    sprintf(cstrInt, "%d", integer);
    stringAppendCstr(s, cstrInt);
}

/**
 * @brief Overwrites string
 *
 * @param s string
 * @param cstr some C string
 * @param size C string size
 */
void stringOverwrite(String* s, const char cstr[], unsigned long size) {
    if (s->capacity < size)
        _stringExpand(s, size + 1);
    memcpy(s->str, cstr, size);
    s->str[size] = '\0';
    s->size = size;
}

/**
 * @brief Expands string capacity
 *
 * @param s the string
 * @param newCapacity the desired capacity
 */
void _stringExpand(String* s, unsigned long newCapacity) {
    char* newBuff = (char*)malloc(newCapacity * sizeof(char));
    memcpy(newBuff, s->str, s->size + 1);
    free(s->str);
    s->str = newBuff;
    s->capacity = newCapacity;
}