#include "../header/stack.h"

#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Creates a stack with one element in it
 *
 * @return Node* Pointer to the beginning of the stack
 */
Node* stackInit() {
    Node* head = (Node*)malloc(sizeof(Node));
    head->depth = 0;
    head->next = NULL;

    return head;
}

/**
 * @brief Destroy the stack
 *
 * @param head adrress of the pointer to the beginning of the stack
 */
void stackDestroy(Node** head) {
    if (!head)
        return;

    if (!(*head))
        return;

    stackDestroy(&((*head)->next));
    free(*head);
    *head = NULL;
}

/**
 * @brief Pushes an element into the stack. Creates the stack if empty.
 *
 * @param head adrress of the pointer to the beginning of the stack
 */
void stackPush(Node** head) {
    if (!head)
        return;

    if (!(*head)) {  // empty list, become head
        *head = stackInit();
        return;
    }
    if (!(*head)->next) {  // last node, push
        (*head)->next = stackInit();
    } else {
        stackPush(&(*head)->next);
    }
}

/**
 * @brief Peaks at the stack
 *
 * @param head pointer to the beginning of the stack
 * @return int depth in the last element of the stack
 */
int stackPeak(Node* head) {
    if (!head)
        return -1;  // not a synchronization token

    if (!head->next)
        return head->depth;

    return stackPeak(head->next);
}

/**
 * @brief Pops the stack
 *
 * @param head adrress of the pointer to the beginning of the stack
 */
void stackPop(Node** head) {
    if (!(*head))
        return;

    if (!(*head)->next) {
        free(*head);
        (*head) = NULL;
        return;
    }

    if (!(*head)->next->next) {  // next is last element
        free((*head)->next);
        (*head)->next = NULL;
    } else {
        stackPop(&((*head)->next));
    }
}

/**
 * @brief Adds a value to all the elements in the stack
 *
 * @param head adrress of the pointer to the beginning of the stack
 * @param toAdd value to add to all elements of the stack
 */
void stackAdd(Node** head, int toAdd) {
    if (!head)
        return;

    if (!(*head))
        return;

    (*head)->depth += toAdd;
    stackAdd(&((*head)->next), toAdd);
}
