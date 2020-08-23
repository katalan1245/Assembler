/* one direction linked list for the label table */
#ifndef LABEL_TABLE_LINKED_LIST_H
#define LABEL_TABLE_LINKED_LIST_H
#include "defaults.h"

void addToList(labelTableNodePtr*,labelTableNode);
void freeLabelList(labelTableNodePtr*);
Bool labelInList(labelTableNodePtr,char*);
Type getLabelType(labelTableNodePtr,char*);
int getLabelAddress(labelTableNodePtr,char*);
void setType(labelTableNodePtr,char*,Type);
#endif