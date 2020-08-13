#ifndef FIRST_PASS_H
#define FIRST_PASS_H
#include "defaults.h"
#include "inputAnalyze.h"
#include "symbolTableLinkedList.h"
#include "dataImage.h"

Status handleInstruction(char*);
Status handleInstruction(char*);
Status findInstructionsErrors(char*);
void addSymbol(char*);
char *findSymbol(char*);
Type findType(char*,char*);

#endif