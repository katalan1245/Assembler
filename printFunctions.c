#include "defaults.h"

void printError(Status status, char *filename) {
    switch (status) {
        case LineTooLong:
            printf("%s,%d: Line too long.\n",filename,lineCounter);
            break;
        case Valid:
            break;
    }
}

void printWord(struct variables *variablesPtr,wordNodePtr wordNode) {
    printf("%06lu ",wordNode->address);
    printf("%06lx",word->index);
}