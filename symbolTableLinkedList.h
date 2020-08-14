/* one direction linked list for the symbol table */
#ifndef SYMBOL_TABLE_LINKED_LIST_H
#define SYMBOL_TABLE_LINKED_LIST_H
#include "defaults.h"

typedef enum {Code, Data} Location;
typedef enum {Entry, External, None} Type;

typedef struct node *symbolTableNodePtr;
typedef struct node {
    char *symbol;
    int value;
    Location location;
    Type type;
    symbolTableNodePtr next;
} symbolTableNode;

void addToList(symbolTableNodePtr*,symbolTableNode);
void freeList(symbolTableNodePtr*);
void removeFirstNode(symbolTableNodePtr*);
void printList(symbolTableNodePtr);
Bool symbolInList(symbolTableNodePtr,char*);


#endif