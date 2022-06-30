/**
 * @file vector.c
 * @brief String data type implementation
 */
#include "../header/vector.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Allocates memory for string
 *
 * @param s string
 */
void stringInit(String* s) {
    s->size = 0;
    s->capacity = 4;
    s->str = (char*)malloc(s->capacity * sizeof(char));
    s->str[0] = '\0';
}

/**
 * @brief Deallocates memory used
 *
 * @param s string
 */
void stringDestroy(String* s) {
    free(s->str);
}

/**
 * @brief Expands string capacity
 *
 * @param s string
 * @param newCapacity desired capacity
 */
void expand(String* s, unsigned long newCapacity) {
    char* newBuff = (char*)malloc(newCapacity * sizeof(char));
    for (int i = 0; i <= s->size; i++)
        newBuff[i] = s->str[i];
    free(s->str);
    s->str = newBuff;
    s->capacity = newCapacity;
}

/**
 * @brief Appends character to end of string
 *
 * @param s string
 * @param c character to be appended
 */
void append(String* s, char c) {
    if (s->size + 1 < s->capacity) {
        s->str[s->size++] = c;
        s->str[s->size] = '\0';
    } else {
        expand(s, s->capacity * 2);  // doubles the capacity
        s->str[s->size++] = c;
        s->str[s->size] = '\0';
    }
}

void appendStr(String* s, const char* cstr) {
    for (size_t i = 0; i < strlen(cstr); i++)
        append(s, cstr[i]);
}

void appendInt(String* s, int integer) {
    static char cstrInt[12];  // INT_MIN is -2.147.483.648, so it fits in 12 bits (with signal)
    sprintf(cstrInt, "%d", integer);
    appendStr(s, cstrInt);
}

/**
 * @brief Overwrites string
 *
 * @param s string
 * @param cstr text
 * @param size text size
 */
void writeToString(String* s, const char cstr[], int size) {
    if (s->capacity < size)
        expand(s, size + 1);

    for (int i = 0; i < size; i++)
        s->str[i] = cstr[i];

    s->str[size] = '\0';
    s->size = size;
}