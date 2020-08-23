#include "defaults.h"
#include "symbolTableLinkedList.h"

void addToList(symbolTableNodePtr *hptr, symbolTableNode node) {
    symbolTableNodePtr p1, p2;
    symbolTableNodePtr t;

    t = (symbolTableNodePtr) malloc(sizeof(symbolTableNode));
    if(!t) {
        printf("Cannot Build List!\n");
        exit(0);
    }

    strcpy(t->symbol, node.symbol);
    t->address = node.address;
    t->location = node.location;
    t->type = node.type;
    t->next = NULL;

    p1 = *hptr;
    p2 = NULL;
    /* get to the last node */
    while(p1) {
        p2 = p1;
        p1 = p1->next;
    }

    /* if the list empty */
    if(p1 == *hptr) {
        *hptr = t;
    }
    else { /* add the new node to the end of the list */
        p2->next = t;
        t->next = p1;
    }
}

void freeSymbolList(symbolTableNodePtr *hptr) {
    symbolTableNodePtr temp;

    while (*hptr) {
        temp = (*hptr)->next;
        free(*hptr);
        *hptr = temp;
    }
}

/* check if the symbol already exist in the symbol table*/
Bool symbolInList(symbolTableNodePtr hptr, char *symbol) {
    while(hptr) {
        if(!strcmp(symbol,hptr->symbol))
            return True;
        hptr = hptr->next;
    }
    return False;
}

/* return the type of the symbol, if not exist return None */
Type getSymbolType(symbolTableNodePtr hptr, char *symbol) {
    while(hptr) {
        if(!strcmp(symbol,hptr->symbol))
            return hptr->type;
        hptr = hptr->next;
    }
    return NoneEntOrExt;
}

int getSymbolAddress(symbolTableNodePtr hptr, char *symbol) {
    while(hptr) {
        if(!strcmp(symbol,hptr->symbol))
            return hptr->address;
        hptr = hptr->next;
    }
    return -1;
}

void setType(symbolTableNodePtr hptr, char *symbol, Type t) {
    while(hptr){
        if(!strcmp(symbol,hptr->symbol)) {
            hptr->type = t;
            return;
        }
        hptr = hptr->next;
    }
}