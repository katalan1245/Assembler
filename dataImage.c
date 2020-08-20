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
    strcpy(t->externSymbol,"");

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

int getSrcAdd(wordNodePtr hptr, int address) {
    while(hptr) {
        if(hptr->address == address)
            return hptr->word.code.srcAdd;
        hptr = hptr->next;
    }
    return -1;
}

int getDestAdd(wordNodePtr hptr, int address) {
    while(hptr) {
        if(hptr->address == address)
            return hptr->word.code.destAdd;
        hptr = hptr->next;
    }
    return -1;
}

int getOpcode(wordNodePtr hptr, int address) {
    while(hptr) {
        if(hptr->address == address)
            return hptr->word.code.opcode;
        hptr = hptr->next;
    }
    return -1;
}