#ifndef PRINT_FUNCTIONS_H
#define PRINT_FUNCTIONS_H
#include "defaults.h"
#include "symbolTableLinkedList.h"

void printError(Status,char*);
void printWord(struct variables*,wordNodePtr);