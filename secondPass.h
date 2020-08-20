#ifndef SECOND_PASS_H
#define SECOND_PASS_H
#include "defaults.h"
#include "firstPass.h"
#include "inputAnalyze.h"
#include "symbolTableLinkedList.h"
#include "printFunctions.h"
#include "dataImage.h"

void secondPass(variables*);
void secondInstruction(variables*,wordNodePtr*);
Status addEntryProperty(symbolTableNodePtr,char*);
#endif