#ifndef FIRST_PASS_H
#define FIRST_PASS_H
#include "defaults.h"
#include "inputAnalyze.h"
#include "symbolTableLinkedList.h"
#include "dataImage.h"

Status firstPass(struct variables*);
Status handleInstruction(struct variables*,Word*);
Status handleDirective(struct variables*,Word*);
Status findInstructionsErrors(char*);
void addSymbol(struct variables*,Location);
char *findSymbol(char*);
EntryOrExternal findEntryOrExternal(char*);
DataOrString findDataOrString(char*);

#endif