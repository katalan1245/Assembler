#include "secondPass.h"

void secondPass(variables *variablesPtr) {
    Statement state;
    wordNodePtr wordPtr;
    char *symbol;
    char arr[STRING_PARTS][LINE_LEN];
    wordPtr = variablesPtr->codeHptr;
    variablesPtr->lineCounter = 0;

    while(!feof(variablesPtr->file)) {
        state = getLine(variablesPtr);
        if(feof(variablesPtr->file))
            break;
        variablesPtr->lineCounter++;

        strcpy(variablesPtr->line,strip(variablesPtr->line));
        if(state == Invalid)
            variablesPtr->status = LineTooLong;

        if(state == Comment || state == Empty)
            continue;
        symbol = findSymbol(variablesPtr->line);
        if(strcmp(symbol,"")) {
            split(variablesPtr->line," \t",arr);
            strcpy(variablesPtr->line, strip(arr[REST]));
        }

        if(state == Directive) {
            Type t;
            t = findEntryOrExternal(variablesPtr->line);
            if(t == Entry) {
                split(variablesPtr->line," \t",arr);
                strcpy(arr[REST],strip(arr[REST]));
                variablesPtr->status = addEntryProperty(variablesPtr->symbolHptr,arr[REST]);
            }
        }
        else if(state == Instruction) {
            secondInstruction(variablesPtr,&wordPtr);
        }

        printError(variablesPtr);
        
    }
}

/* NEED TO TAKE CARE ABOUT ALL THE OPERATIONS(STOP) BEFORE THE SPLIT! */
void secondInstruction(variables *variablesPtr,wordNodePtr *wordHptr) {
    static int tempIC = 100;
    int opcode = getOpcode(*wordHptr,tempIC);
    int srcAdd = getSrcAdd(*wordHptr,tempIC);
    int destAdd = getDestAdd(*wordHptr,tempIC);
    char arr[STRING_PARTS][LINE_LEN];
    split(variablesPtr->line," \t",arr);
    split(strip(arr[REST]),",",arr);
    variablesPtr->status = Valid;

    if(opcode <= 4) {
        if(srcAdd != 3) {
            tempIC++;
            (*wordHptr) = (*wordHptr)->next;
            if(srcAdd == 1) {
                int addr;
                addr = getSymbolAddress(variablesPtr->symbolHptr,arr[IMPORTANT]);
                if(addr == -1) {
                    variablesPtr->status = MissingLabel;
                }
                else {
                    if(getSymbolType(variablesPtr->symbolHptr,arr[IMPORTANT]) == External) {
                        (*wordHptr)->word.index = E;
                        strcpy((*wordHptr)->externSymbol,arr[IMPORTANT]);
                    }
                    else
                        (*wordHptr)->word.index = (addr<<3) + R;
                    
                }
            }
        }

        if(destAdd != 3) {
            tempIC++;
            *wordHptr = (*wordHptr)->next;
            if(destAdd == 1) {
                int addr;
                addr = getSymbolAddress(variablesPtr->symbolHptr,arr[REST]);
                if(addr == -1) {
                    variablesPtr->status = MissingLabel;
                }
                else {
                    if(getSymbolType(variablesPtr->symbolHptr,arr[REST]) == External) {
                        (*wordHptr)->word.index = E;
                        strcpy((*wordHptr)->externSymbol,arr[REST]);
                    }
                    else
                        (*wordHptr)->word.index = (addr<<3) + R;
                    
                }
            }
        }
        
    }
    else if(opcode <= 13) {
        if(destAdd != 3) {
           
            *wordHptr = (*wordHptr)->next;
            if(destAdd == 1) {
                int addr;
                addr = getSymbolAddress(variablesPtr->symbolHptr,arr[IMPORTANT]);
                if(addr == -1) {
                    variablesPtr->status = MissingLabel;
                }
                else {
                    if(getSymbolType(variablesPtr->symbolHptr,arr[IMPORTANT]) == External) {
                        (*wordHptr)->word.index = E;
                        strcpy((*wordHptr)->externSymbol,arr[IMPORTANT]);
                    }
                    else
                        (*wordHptr)->word.index = (addr<<3) + R;
                    
                }
            }
            else if(destAdd == 2) {
                int addr;
                strcpy(arr[IMPORTANT],arr[IMPORTANT]+1);
                addr = getSymbolAddress(variablesPtr->symbolHptr,arr[IMPORTANT]);
                if(addr == -1) {
                    variablesPtr->status = MissingLabel;
                }
                else {
                   if(getSymbolType(variablesPtr->symbolHptr,arr[IMPORTANT]) == External)
                        variablesPtr->status = ExternalBranching;
                    else {
                        int diff = addr - tempIC;
                        (*wordHptr)->word.index = diff < 0 ? ((int)(pow(2,21) + diff)<<3) + A : (diff<<3) + A; 
                    }
                }
            }

            tempIC++;
            
        }
    }

    tempIC++;
    *wordHptr = (*wordHptr)->next;
}

Status addEntryProperty(symbolTableNodePtr hptr,char *str) {
    if(symbolInList(hptr, str)) {
        Type t;
        t = getSymbolType(hptr, str);
        if(t == External)
            return SymbolEntryAndExtern;
        setType(hptr,str,Entry);
        return Valid;
    }
    return MissingLabel;
}