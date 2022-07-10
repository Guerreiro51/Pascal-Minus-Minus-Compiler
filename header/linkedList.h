/**
 * @file parser.h
 * @brief Syntax analyser (parser)
 */
#ifndef LINKED_LIST
#define LINKED_LIST

#include <stdbool.h>

typedef struct _node {
    int depth;
    struct _node* next;
} Node;

Node* linkedListInit();
void linkedListDestroy(Node** head);
void linkedListPushBack(Node** head);
int linkedListPeak(Node* head);
void linkedListPop(Node** head);
void linkedListAdd(Node** head, int toAdd);

#endif  // LINKED_LIST