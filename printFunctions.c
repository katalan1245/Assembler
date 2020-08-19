#include "printFunctions.h"

void printError(struct variables *variablesPtr) {
    switch (status) {
        case LineTooLong:
            printf("%s.as,%d: Line too long.\n",variablesPtr->filename,variablesPtr->lineCounter);
            break;
        case Valid:
            break;
    }
}

void printWord(FILE *f,wordNodePtr wordPtr) {
    unsigned long num = MAX_24_UNSIGNED;
    num &= wordPtr->word.index;
    fprintf(f,"%06lu ",wordPtr->address);
    fprintf(f,"%06lx\n",num);
}

void printExternal(FILE *f, wordNodePtr wordPtr) {
    fprintf(f,"%s %06lu\n",wordPtr->externSymbol,wordPtr->address);
}

void printEntry(File *f, symbolTableNodePtr symbolPtr) {
    fprintf(f,"%s %06lu\n",symbolPtr->symbol,symbolPtr->address);
}

void createOutput(struct variables *variablesPtr) {
    Bool hasEntry = False;
    Bool hasExternal = False;
    FILE *file;
    char str[FILE_NAME_LEN + EXT_ENT_EXTENSION_LEN];
    wordNodePtr dataHptr = variablesPtr->dataHptr;
    wordNodePtr codeHptr = variablesPtr->codeHptr;
    symbolTableNodePtr symbolPtr = variablesPtr->symbolHptr;

    sprintf(str,"%s.ob",variablesPtr->filename);
    file = fopen(str,"w");
    fprintf(file,"%6d %d",variablesPtr->IC - STARTING_IC,variablesPtr->DC);
    while(codeHptr) {
        if(strcmp(codeHptr->externSymbol,""))
            hasExternal = True;
        printWord(file,codeHptr);
        codeHptr = codeHptr->next;
    }

    while(dataHptr) {
        printWord(file,dataHptr);
        dataHptr = dataHptr->next;
    }
    fclose(file);

    codeHptr = variablesPtr->codeHptr;
    dataHptr = variablesPtr->dataHptr;
    
    if(hasExternal) {
        fprintf(str,"%s.ext",variablesPtr->filename);
        file = fopen(str,"w");
        while(codeHptr) {
            if(strcmp(codeHptr->externSymbol,""))
                printExternal(file,codeHptr);
            codeHptr = codeHptr->next;
        }
        fclose(file);
    }

    while(symbolPtr) {
        if(symbolPtr->type == Entry)
            hasEntry = True;
    }

    if(hasEntry) {
        fprintf(str,"%s.ent",variablesPtr->filename);
        file = fopen(str,"w");
        while(symbolPtr) {
            if(symbolPtr->type == Entry)
                printEntry(file,symbolPtr);
            symbolPtr = symbolPtr->next;
        }
        fclose(file);
    }
}