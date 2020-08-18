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
    t->address = node.address;
    t->location = node.location;
    t->type = node.type;

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
    while (*hptr) {
        *hptr = (*hptr)->next;
    }
}

// need to fix
void removeFirstSymbolNode(symbolTableNodePtr *hptr) {

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

/* return the type of the symbol, if not exist return None */
Type getSymbolType(symbolTableNodePtr hptr, char *symbol) {
    while(hptr) {
        if(!strcmp(symbol,hptr->symbol))
            return hptr->type;
    }
    return None;
}

int getSymbolAddress(symbolTableNodePtr hptr, char *symbol) {
    while(hptr) {
        if(!strcmp(symbol,hptr->symbol))
            return hptr->address;
    }
    return -1
}

/* add the property type to the symbol str 
Status addEntryProperty(symbolTableNodePtr hptr, char *symbol) {
    Bool stop;
    stop = False;
    if(symbolInList(hptr,symbol)) {
        while(hptr && !stop) {
            if(!strcmp(symbol,hptr->symbol)) {
                if(hptr->type != None && hptr->type != type) /* if it is not it self or none, we have double directive 
                    return MultipleDirectives;
                hptr->type = type;
                stop = True;
            }
            hptr = hptr->next;
        }
    }
} */