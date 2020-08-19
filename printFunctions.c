#include "printFunctions.h"

void printError(struct variables *variablesPtr) {
    switch (status) {
        case LineTooLong:
            printf("%s.as,%d: Line too long.\n",variablesPtr->filename,variablesPtr->lineCounter);
            break;
        case UnknownOperation:
        	printf("%s.as,%d: Unknown operation.\n",variablesPtr->filename,variablesPtr->lineCounter);
        	break;
       	case SymbolAlreadyExist:
       		printf("%s.as,%d: Symbol already exists.\n",variablesPtr->filename,variablesPtr->lineCounter);
       		break;
       	case TextAfterCommand: 
       		printf("%s.as,%d: Text after command.\n",variablesPtr->filename,variablesPtr->lineCounter);
       		break;
       	case MissingOperand:
       		printf("%s.as,%d: Missing operand.\n",variablesPtr->filename,variablesPtr->lineCounter);
       		break;
       	case InvalidOperand:
       		printf("%s.as,%d: Invalid operand.\n",variablesPtr->filename,variablesPtr->lineCounter);
       		break;
       	case InvalidLabel:
       		printf("%s.as,%d: Invalid label.\n",variablesPtr->filename,variablesPtr->lineCounter);
       		break;
       	case InvalidNumber:
       		printf("%s.as,%d: Invalid number.\n",variablesPtr->filename,variablesPtr->lineCounter);
       		break;
       	case MultipleDirectives:
       		printf("%s.as,%d: Multiple directives.\n",variablesPtr->filename,variablesPtr->lineCounter);
       		break;
       	case ReservedLabelName:
       		printf("%s.as,%d: Label name is a reserved name.\n",variablesPtr->filename,variablesPtr->lineCounter);
       		break;
       	case LabelTooLong:
       		printf("%s.as,%d: Label is too long.\n",variablesPtr->filename,variablesPtr->lineCounter);
       		break;
       	case LabelInvalidStart:
       		printf("%s.as,%d: The first char in a label must be a letter.\n",variablesPtr->filename,variablesPtr->lineCounter);
       		break;
       	case LabelInvalidCharacters:
       		printf("%s.as,%d: A label can only consist of letters and numbers.\n",variablesPtr->filename,variablesPtr->lineCounter);
       		break;
       	case MissingWhitespace:
       		printf("%s.as,%d: Missing whitespace.\n",variablesPtr->filename,variablesPtr->lineCounter);
       		break;
       	case SymbolEntryAndExtern:
       		printf("%s.as,%d: A symbol cannot be both entry and external.\n",variablesPtr->filename,variablesPtr->lineCounter);
       		break;
       	case SymbolEntryAndExtern:
       		printf("%s.as,%d: A symbol cannot be both entry and external.\n",variablesPtr->filename,variablesPtr->lineCounter);
       		break;
       	case SymbolDefinedAndExtern:
       		printf("%s.as,%d: A symbol cannot be both external and defined in this file.\n",variablesPtr->filename,variablesPtr->lineCounter);
       		break;
       	case MissingLabel:
       		printf("%s.as,%d: Label doesn't exist.\n",variablesPtr->filename,variablesPtr->lineCounter);
       		break;
       	case Error:
       		printf("%s.as,%d: Error.\n",variablesPtr->filename,variablesPtr->lineCounter);
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