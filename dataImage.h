#ifndef DATA_IMAGE_H
#define DATA_IMAGE_H
#include "defaults.h"

void addWordToImage(wordNodePtr*,Word);
void freeList(wordNodePtr*);
int getDestAdd(wordNodePtr,int);
int getOpcode(wordNodePtr,int);
#endif