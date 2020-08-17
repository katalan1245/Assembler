/* one direction linked list for the symbol table */
#ifndef SYMBOL_TABLE_LINKED_LIST_H
#define SYMBOL_TABLE_LINKED_LIST_H
#include "defaults.h"

typedef enum {Code, Data} Location;

typedef struct node *symbolTableNodePtr;
typedef struct node {
    char *symbol;
    int address;
    Location location;
    EntryOrExternal type;
    Bool defined;
    symbolTableNodePtr next;
} symbolTableNode;

void addToList(symbolTableNodePtr*,symbolTableNode);
void freeSymbolList(symbolTableNodePtr*);
void removeFirstSymbolNode(symbolTableNodePtr*);
void printList(symbolTableNodePtr);
Bool symbolInList(symbolTableNodePtr,char*);
EntryOrExternal getSymbolType(symbolTableNodePtr,char*);

#endif