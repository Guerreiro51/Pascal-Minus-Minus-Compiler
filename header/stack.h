/**
 * @file stack.h
 * @brief Stack implementation to be used to save followers
 */
#ifndef STACK_H
#define STACK_H

#include <stdbool.h>

typedef struct _node {
    int depth;
    struct _node* next;
} Node;

Node* stackInit();
void stackDestroy(Node** head);
void stackPush(Node** head);
int stackPeak(Node* head);
void stackPop(Node** head);
void stackAdd(Node** head, int toAdd);

#endif  // STACK_H