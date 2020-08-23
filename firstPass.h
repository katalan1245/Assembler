#ifndef FIRST_PASS_H
#define FIRST_PASS_H
#include "defaults.h"
#include "inputAnalyze.h"
#include "labelTableLinkedList.h"
#include "printFunctions.h"
#include "dataImage.h"

void firstPass(variables*);
void handleInstruction(variables*,Word*);
void fillTwoOperands(char*,Word*,variables*);
void fillOneOperand(char*,Word*,variables*);
void handleDirective(variables*,Word*);
void addLabel(variables*,Location);
int findAddressMethod(variables*,char*);
Status checkNum(char*);
void checkSyntaxValidLabel(variables*,char*,Bool);
void checkLabel(variables*,char*,Type);
void addNumberWord(variables*,char*);
void addStringWord(variables*,char);
void updateTables(variables*);
void defaultValues(variables*);
void addEmptyWord(variables*);
void printListProp(variables*);
#endif