#ifndef PRINT_FUNCTIONS_H
#define PRINT_FUNCTIONS_H
#include "defaults.h"
#include "symbolTableLinkedList.h"
#include "dataImage.h"

void printError(variables *);
void printWord(FILE*,wordNodePtr);
void printExternal(FILE*,wordNodePtr);
void printEntry(FILE*,symbolTableNodePtr);
void createOutput(variables*);
#endif PRINT_FUNCTIONS_H