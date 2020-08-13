/* one direction linked list for the symbol table */
#ifndef SYMBOL_TABLE_LINKED_LIST_H
#define SYMBOL_TABLE_LINKED_LIST_H

typedef enum {code, data} Location;
typedef enum {entry, external} Type;

typedef struct node *symbolTableNodePtr;
typedef struct node {
    char *symbol;
    int value;
    Location location;
    Bool isExternal;
    symbolTableNodePtr next;
} symbolTableNode;

extern symbolTableNodePtr hptr;

void addToList(symbolTableNodePtr *, symbolTableNode);
void freeList(symbolTableNodePtr *);
void removeFirstNode(symbolTableNodePtr *);
void printList(symbolTableNodePtr);
Bool symbolInList(symbolTableNodePtr, char *);


#endif