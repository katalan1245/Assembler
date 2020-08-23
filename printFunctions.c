#include "printFunctions.h"

void printError(variables *variablesPtr) {
    switch (variablesPtr->status) {
        case LineTooLong:
            fprintf(stdout,"%s.as:%d: Line too long.\n",variablesPtr->filename,variablesPtr->lineCounter);
            break;
        case UnknownOperation:
        	fprintf(stdout,"%s.as:%d: Unknown operation.\n",variablesPtr->filename,variablesPtr->lineCounter);
        	break;
       	case LabelAlreadyExist:
       		fprintf(stdout,"%s.as:%d: Label already exists.\n",variablesPtr->filename,variablesPtr->lineCounter);
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
       		fprintf(stdout,"%s.as:%d: Invalid Label.\n",variablesPtr->filename,variablesPtr->lineCounter);
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
       	case LabelEntryAndExtern:
       		fprintf(stdout,"%s.as:%d: A label cannot be both entry and external.\n",variablesPtr->filename,variablesPtr->lineCounter);
       		break;
       	case LabelDefinedAndExtern:
       		fprintf(stdout,"%s.as:%d: A label cannot be both external and defined in this file.\n",variablesPtr->filename,variablesPtr->lineCounter);
       		break;
       	case MissingLabel:
       		fprintf(stdout,"%s.as:%d: label doesn't exist.\n",variablesPtr->filename,variablesPtr->lineCounter);
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
        case NoOpeningQuotes:
            fprintf(stdout,"%s.as:%d: No opening quotes for the string.\n",variablesPtr->filename,variablesPtr->lineCounter);
            break;
        case ExtraComma:
            fprintf(stdout,"%s.as:%d: Extra comma.\n",variablesPtr->filename,variablesPtr->lineCounter);
            break;
        case NumOutOfMemory:
            fprintf(stdout,"%s.as:%d: The number doesn't fit inside the capacity of the memory (too big or too small).\n",variablesPtr->filename,variablesPtr->lineCounter);
            break;
        case Invalid2AdressMethod:
            fprintf(stdout,"%s.as:%d: Address method 2 valid only in branching commands (jmp,bne,jsr).\n",variablesPtr->filename,variablesPtr->lineCounter);
            break;
        case Invalid0AdressMethod:
            fprintf(stdout,"%s.as:%d: Can't use address method 0 on the second operand with this command.\n",variablesPtr->filename,variablesPtr->lineCounter);
            break;
        case InvalidLeaOperands:
            fprintf(stdout,"%s.as:%d: lea command can only use address method 1 for first operand and address methods 1,3 for second operand.\n",variablesPtr->filename,variablesPtr->lineCounter);
            break;
        case InvalidOperand5_12:
            fprintf(stdout,"%s.as:%d: clr,not,inc,dec,red can only use address methods 1,3.\n",variablesPtr->filename,variablesPtr->lineCounter);
            break;
        case InvalidOperand9:
            fprintf(stdout,"Branching commands (jmp,bne,jsr) can only use adress methods 1,2.\n");
            break;
        case Valid:
            break;
    }
}

void printWord(FILE *f,wordNodePtr wordPtr) {
    unsigned long num = MAX_24_UNSIGNED;
    num &= wordPtr->word.index;
    fprintf(f,"%07lu ",wordPtr->address);
    fprintf(f,"%06lx\n",num);
}

void printExternal(FILE *f, wordNodePtr wordPtr) {
    fprintf(f,"%s %07lu\n",wordPtr->externSymbol,wordPtr->address);
}

void printEntry(FILE *f, symbolTableNodePtr symbolPtr) {
    fprintf(f,"%s %07lu\n",symbolPtr->symbol,symbolPtr->address);
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
    fprintf(file,"%7d %d\n",variablesPtr->IC - STARTING_IC,variablesPtr->DC);
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
        symbolPtr = symbolPtr->next;
    }

    symbolPtr = variablesPtr->symbolHptr;
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