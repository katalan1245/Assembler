#ifndef PRINT_FUNCTIONS_H
#define PRINT_FUNCTIONS_H
#include "defaults.h"
#include "labelTableLinkedList.h"
#include "dataImage.h"

void printError(variables *);
void printWord(FILE*,wordNodePtr);
void printExternal(FILE*,wordNodePtr);
void printEntry(FILE*,labelTableNodePtr);
void createOutput(variables*);
#endif