#include "dataImage.h"

void addWordToImage(wordNodePtr *hptr, Word word) {
    wordNodePtr p1,p2;
    wordNodePtr t;
    wordNode node;

    node.word = word;
    if(word.type == Code)
        node.address = IC++;
    else
        node.address = DC++;

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
    word->type = wordData;
    word->data.wordData.address = address;
    if(word->data.wordData.useARE) {
        word->data.wordData.address <<= 3;
        word->data.wordData.address |= 4*A + 2*R + E;
    }
}

void fillWordCode(Word *word, int opcode, int srcAdd, int srcReg, int destAdd, int destReg, int funct, int A, int R, int E) {
    word->type = Code;
    word->data.code.opcode = opcode;
    word->data.code.srcAdd = srcAdd;
    word->data.code.srcReg = srcReg;
    word->data.code.destAdd = srcAdd;
    word->data.code.destReg = srcReg;
    word->data.code.funct = funct;
    word->data.code.A = A;
    word->data.code.R = R;
    word->data.code.E = E;
}