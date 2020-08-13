#ifndef DATA_IMAGE_H
#define DATA_IMAGE_H
#include "defaults.h"

typedef struct wordnode *wordNodePtr;
typedef struct wordnode {
    Word word;
    unsigned long address;
    wordNodePtr next;
} wordNode;

void addWordToImage(wordNodePtr*,Word);
void freeList(wordNodePtr*);
void removeFirstNode(wordNodePtr*);
void fillWordData(Word*,long,ARE);
void fillWordCode(Word*,int,int,int,int,int,int,int,int,int);
#endif