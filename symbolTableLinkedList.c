#include "defaults.h"
#include "symbolTableLinkedList.h"

void addToList(symbolTableNodePtr *hptr, symbolTableNode node) {
    symbolTableNodePtr p1, p2;
    symbolTableNodePtr t;

    t = (symbolTableNodePtr) malloc(sizeof(symbolTableNode));
    if(!t) { // if cannot allocate memory
        printf("Cannot Build List!\n");
        exit(0);
    }

    // copy the fields from node to t
    if(!t->symbol)
        t->symbol = malloc(LABEL_LEN);

    strcpy(t->symbol, node.symbol);
    t->value = node.value;
    t->location = node.location;
    t->isExternal = node.isExternal;

    p1 = *hptr;
    p2 = NULL;
    // get to the last node
    while(p1) {
        p2 = p1;
        p1 = p1->next;
    }

    // if the last node is the head
    if(p1 == *hptr) {
        *hptr = t;
    }
    else { // add the new node to the end of the list
        p2->next = t;
        t->next = p1;
    }
}

void freeList(symbolTableNodePtr *hptr) {
    symbolTableNodePtr p;

    while (*hptr) {
        p = *hptr;
        *hptr = (*hptr)->next;
        free(p);
    }
}

// need to fix
void removeFirstNode(symbolTableNodePtr *hptr) {
    symbolTableNodePtr p;

    if(!hptr) {
        printf("Cannot Remove From Empty List!");
        return;
    }
    *hptr = (*hptr)->next;
}

void printList(symbolTableNodePtr hptr) {
    while(hptr) {
        printf("ptr: %s\n",hptr->symbol);
        hptr = hptr->next;
    }
}

/* check if the symbol already exist in the symbol table*/
Bool symbolInList(symbolTableNodePtr hptr, char *symbol) {
    while(hptr) {
        if(!strcmp(symbol,hptr->symbol))
            return True;
    }
    return False;
}