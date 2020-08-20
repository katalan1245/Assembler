#ifndef FIRST_PASS_H
#define FIRST_PASS_H
#include "defaults.h"
#include "inputAnalyze.h"
#include "symbolTableLinkedList.h"
#include "dataImage.h"

void firstPass(variables*);
void handleInstruction(variables*,Word*);
void fillTwoOperands(char*,Word*,variables*);
void fillOneOperand(char*,Word*,variables*);
void handleDirective(variables*,Word*);
void addSymbol(variables*,Location);
int findAddressMethod(variables*,char*);
Status checkNum(char*);
void checkSyntaxValidLabel(variables*);
void checkDirectiveLabel(variables*,Type);
void addNumberWord(variables*,char*);
void addStringWord(variables*,char);
void updateTables(variables*);
void defaultValues(variables*);
#endif