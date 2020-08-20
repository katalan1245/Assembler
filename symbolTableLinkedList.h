/* one direction linked list for the symbol table */
#ifndef SYMBOL_TABLE_LINKED_LIST_H
#define SYMBOL_TABLE_LINKED_LIST_H
#include "defaults.h"

void addToList(symbolTableNodePtr*,symbolTableNode);
void freeSymbolList(symbolTableNodePtr*);
Bool symbolInList(symbolTableNodePtr,char*);
Type getSymbolType(symbolTableNodePtr,char*);
int getSymbolAddress(symbolTableNodePtr,char*);
void setType(symbolTableNodePtr,char*,Type);
#endif