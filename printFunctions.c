#include "printFunctions.h"

void printError(variables *variablesPtr) {
    switch (variablesPtr->status) {
        case LineTooLong:
            fprintf(stdout,"%s.as:%d: Line too long.\n",variablesPtr->filename,variablesPtr->lineCounter);
            break;
        case UnknownOperation:
        	fprintf(stdout,"%s.as:%d: Unknown operation.\n",variablesPtr->filename,variablesPtr->lineCounter);
        	break;
       	case SymbolAlreadyExist:
       		fprintf(stdout,"%s.as:%d: Symbol already exists.\n",variablesPtr->filename,variablesPtr->lineCounter);
       		break;
       	case TextAfterCommand: 
       		fprintf(stdout,"%s.as:%d: Text after command.\n",variablesPtr->filename,variablesPtr->lineCounter);
       		break;
       	case MissingOperand:
       		fprintf(stdout,"%s.as:%d: Missing operand.\n",variablesPtr->filename,variablesPtr->lineCounter);
       		break;
       	case InvalidOperand:
       		fprintf(stdout,"%s.as:%d: Invalid operand.\n",variablesPtr->filename,variablesPtr->lineCounter);
       		break;
       	case InvalidLabel:
       		fprintf(stdout,"%s.as:%d: Invalid label.\n",variablesPtr->filename,variablesPtr->lineCounter);
       		break;
       	case InvalidNumber:
       		fprintf(stdout,"%s.as:%d: Invalid number.\n",variablesPtr->filename,variablesPtr->lineCounter);
       		break;
       	case MultipleDirectives:
       		fprintf(stdout,"%s.as:%d: Multiple directives.\n",variablesPtr->filename,variablesPtr->lineCounter);
       		break;
       	case ReservedLabelName:
       		fprintf(stdout,"%s.as:%d: Label name is a reserved name.\n",variablesPtr->filename,variablesPtr->lineCounter);
       		break;
       	case LabelTooLong:
       		fprintf(stdout,"%s.as:%d: Label is too long.\n",variablesPtr->filename,variablesPtr->lineCounter);
       		break;
       	case LabelInvalidStart:
       		fprintf(stdout,"%s.as:%d: The first char in a label must be a letter.\n",variablesPtr->filename,variablesPtr->lineCounter);
       		break;
       	case LabelInvalidCharacters:
       		fprintf(stdout,"%s.as:%d: A label can only consist of letters and numbers.\n",variablesPtr->filename,variablesPtr->lineCounter);
       		break;
       	case MissingWhitespace:
       		fprintf(stdout,"%s.as:%d: Missing whitespace.\n",variablesPtr->filename,variablesPtr->lineCounter);
       		break;
       	case SymbolEntryAndExtern:
       		fprintf(stdout,"%s.as:%d: A symbol cannot be both entry and external.\n",variablesPtr->filename,variablesPtr->lineCounter);
       		break;
       	case SymbolDefinedAndExtern:
       		fprintf(stdout,"%s.as:%d: A symbol cannot be both external and defined in this file.\n",variablesPtr->filename,variablesPtr->lineCounter);
       		break;
       	case MissingLabel:
       		fprintf(stdout,"%s.as:%d: Label doesn't exist.\n",variablesPtr->filename,variablesPtr->lineCounter);
       		break;
        case InvalidDirectiveCommand:
          fprintf(stdout,"%s.as:%d: Invalid directive command.\n",variablesPtr->filename,variablesPtr->lineCounter);
          break;
        case NoClosingQuotes:
          fprintf(stdout,"%s.as:%d: No closing quotes for string.\n",variablesPtr->filename,variablesPtr->lineCounter);
          break;
        case ExtraneousText:
          fprintf(stdout,"%s.as:%d: Extraneous text after string.\n",variablesPtr->filename,variablesPtr->lineCounter);
          break;
       	case ExternalBranching:
       		fprintf(stdout,"%s.as:%d: Trying to jump to external label using address method 2.\n",variablesPtr->filename,variablesPtr->lineCounter);
       		break;
         case Error:
          fprintf(stdout,"%s.as:%d: Error.\n",variablesPtr->filename,variablesPtr->lineCounter);
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

void printEntry(FILE *f, symbolTableNodePtr symbolPtr) {
    fprintf(f,"%s %06lu\n",symbolPtr->symbol,symbolPtr->address);
}

void createOutput(variables *variablesPtr) {
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
        sprintf(str,"%s.ext",variablesPtr->filename);
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
        sprintf(str,"%s.ent",variablesPtr->filename);
        file = fopen(str,"w");
        while(symbolPtr) {
            if(symbolPtr->type == Entry)
                printEntry(file,symbolPtr);
            symbolPtr = symbolPtr->next;
        }
        fclose(file);
    }
}