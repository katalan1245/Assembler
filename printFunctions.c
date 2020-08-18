#include "defaults.h"

void printError(struct variables *variablesPtr) {
    switch (status) {
        case LineTooLong:
            printf("%s,%d: Line too long.\n",variablesPtr->filename,variablesPtr->lineCounter);
            break;
        case Valid:
            break;
    }
}

void printAddressWord(struct variables *variablesPtr,wordNodePtr wordNode) {
    fprintf(variablesPtr->file,"%06lu ",wordNode->address);
    fprintf(variablesPtr->file,"%06lx",word->index);
}

void printCodeWode(struct variables *variablesPtr,wordNodePtr wordNode) {
    char str[LINE_LEN];
    sprintf(str,"%s",)
}