#include "../header/linkedList.h"

#include <stdlib.h>
#include <stdio.h>

Node* linkedListInit() {
    Node* head = (Node*)malloc(sizeof(Node));
    head->depth = 0;
    head->next = NULL;

    return head;
}

void linkedListDestroy(Node** head) {

    if ( linkedIsEmpty(*head) )
        return;
        
    linkedListDestroy( &((*head)->next) );
    free(*head);
}

void linkedListPushBack(Node** head) {
    if(!head)
        return;

    if ( linkedIsEmpty(*head) ) { // empty list, become head
        *head = linkedListInit();
        return;
    }
    if(!(*head)->next) { // last node, pushback
        (*head)->next = linkedListInit();
    } else {
        linkedListPushBack( &(*head)->next );
    }
}

int linkedListPeak(Node* head){
    if(linkedIsEmpty(head))
        return -1;  // not a synchronization token

    if(!head->next)
        return head->depth;  
    
    return linkedListPeak( head->next );

}


void linkedListPop(Node** head) {
    if (linkedIsEmpty(*head))
        return;
    if( !(*head)->next ) {
        free(*head);
        (*head) = NULL;
        return;
    }
    if(!(*head)->next->next) {  // next is last element
        free((*head)->next);
        (*head)->next = NULL;
    } else {
        linkedListPop(&((*head)->next));
    }
    
}

bool linkedIsEmpty(Node* head) {
    return (head == NULL);
}

void linkedListAdd(Node** head, int toAdd) {
    if (linkedIsEmpty(*head))
        return;
    (*head)->depth += toAdd;
    linkedListAdd(&((*head)->next), toAdd);
}
