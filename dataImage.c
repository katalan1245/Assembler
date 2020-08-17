#include "dataImage.h"

void addWordToImage(wordNodePtr *hptr, Word word) {
    wordNodePtr p1,p2;
    wordNodePtr t;
    wordNode node;

    node.word = word;

    t = (wordNodePtr) malloc(sizeof(wordNode));
    if(!t) {
        printf("Malloc Failed- Cannot Build List!");
        exit(0);
    }

    /* copy fields from node to t */
    t->address = node.address;
    t->word = node.word;

    p1 = *hptr;
    p2 = NULL;
    /* get to the last node */
    while(p1) {
        p2 = p1;
        p1 = p1->next;
    }
    /* if the list is empty */
    if(p1 == *hptr)
        *hptr = t;
    else { /* add the new node to the end of the list */
        p2->next = t;
        t->next = p1;
    }
}

void freeList(wordNodePtr *hptr) {
    wordNodePtr p;

    while(*hptr) {
        p = *hptr;
        *hptr = (*hptr)->next;
        free(p);
    }
}

void removeFirstNode(wordNodePtr *hptr) {
    wordNodePtr p;

    if(!hptr) {
        printf("Cannot Remove From Empty List!");
        return;
    }
    *hptr = (*hptr)->next;
}

void fillWordData(Word *word, long address, ARE flag) {
    word->data.index = address;
    if(word->data.useARE) {
        word->data.index <<= 3;
        word->data.index |= flag;
    }
}

void fillWordCode(Word *word, int opcode, int srcAdd, int srcReg, int destAdd, int destReg, int funct, int A, int R, int E) {
    word->code.opcode = opcode;
    word->code.srcAdd = srcAdd;
    word->code.srcReg = srcReg;
    word->code.destAdd = srcAdd;
    word->code.destReg = srcReg;
    word->code.funct = funct;
    word->code.A = A;
    word->code.R = R;
    word->code.E = E;
}