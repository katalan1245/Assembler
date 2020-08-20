#ifndef DATA_IMAGE_H
#define DATA_IMAGE_H
#include "defaults.h"

void addWordToImage(wordNodePtr*,Word,unsigned long);
void freeList(wordNodePtr*);
int getDestAdd(wordNodePtr,int);
int getOpcode(wordNodePtr,int);
int getSrcAdd(wordNodePtr,int);
#endif