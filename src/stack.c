#include "../header/stack.h"

#include <stdio.h>
#include <stdlib.h>

Node* stackInit() {
    Node* head = (Node*)malloc(sizeof(Node));
    head->depth = 0;
    head->next = NULL;

    return head;
}

void stackDestroy(Node** head) {
    if (!head)
        return;

    if (!(*head))
        return;

    stackDestroy(&((*head)->next));
    free(*head);
}

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

int stackPeak(Node* head) {
    if (!head)
        return -1;  // not a synchronization token

    if (!head->next)
        return head->depth;

    return stackPeak(head->next);
}

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

void stackAdd(Node** head, int toAdd) {
    if (!head)
        return;

    if (!(*head))
        return;

    (*head)->depth += toAdd;
    stackAdd(&((*head)->next), toAdd);
}
