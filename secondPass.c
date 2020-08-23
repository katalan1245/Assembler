#include "secondPass.h"

static int tempIC = 100;
/* NEED TO TAKE CARE ABOUT IC AND DC ALL FUNCTIONS! ******************/
void secondPass(variables *variablesPtr) {
    Statement state;
    wordNodePtr wordPtr = variablesPtr->codeHptr;
    char label[LABEL_LEN];
    char *back;
    char arr[STRING_PARTS][LINE_LEN];
    /*wordPtr = variablesPtr->codeHptr;*/
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
        back = findLabel(variablesPtr->line);
        strcpy(label,back);
        free(back);
        if(strcmp(label,"")) {
            split(variablesPtr->line," \t",arr);
            strcpy(variablesPtr->line, strip(arr[REST]));
        }

        if(state == Directive) {
            Type t;
            t = findEntryOrExternal(variablesPtr->line);
            if(t == Entry) {
                split(variablesPtr->line," \t",arr);
                strcpy(arr[REST],strip(arr[REST]));
                variablesPtr->status = addEntryProperty(variablesPtr->labelHptr,arr[REST]);
            }
        }
        else if(state == Instruction) {
            secondInstruction(variablesPtr,&variablesPtr->codeHptr);
        }

        if(variablesPtr->status != Valid)
            variablesPtr->foundError = True;

        printError(variablesPtr);
        
    }
    tempIC = 100;
    variablesPtr->codeHptr = wordPtr;
}

void secondInstruction(variables *variablesPtr,wordNodePtr *wordHptr) {
    int opcode = getOpcode(variablesPtr->codeHptr,tempIC); /* *wordHptr */
    int srcAdd = getSrcAdd(*wordHptr,tempIC);
    int destAdd = getDestAdd(*wordHptr,tempIC);
    char arr[STRING_PARTS][LINE_LEN];
    memset(arr,0, sizeof(arr[0][0])*STRING_PARTS*LINE_LEN);
    split(variablesPtr->line," \t",arr);
    split(arr[REST],",",arr);
    strcpy(arr[IMPORTANT],strip(arr[IMPORTANT]));
    strcpy(arr[REST],strip(arr[REST]));
    variablesPtr->status = Valid;

    if(opcode <= 4) {
        if(srcAdd != 3) {
            tempIC++;
            (*wordHptr) = (*wordHptr)->next;
            if(srcAdd == 1) {
                int addr;
                addr = getLabelAddress(variablesPtr->labelHptr,arr[IMPORTANT]);
                if(addr == -1) {
                    variablesPtr->status = MissingLabel;
                }
                else {
                    if(getLabelType(variablesPtr->labelHptr,arr[IMPORTANT]) == External) {
                        (*wordHptr)->word.index = E;
                        strcpy((*wordHptr)->externLabel,arr[IMPORTANT]);
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
                addr = getLabelAddress(variablesPtr->labelHptr,arr[REST]);
                if(addr == -1) {
                    variablesPtr->status = MissingLabel;
                }
                else {
                    if(getLabelType(variablesPtr->labelHptr,arr[REST]) == External) {
                        (*wordHptr)->word.index = E;
                        strcpy((*wordHptr)->externLabel,arr[REST]);
                    }
                    else {
                        (*wordHptr)->word.index = (addr << 3) + R;
                    }
                }
            }
        }
        
    }
    else if(opcode <= 13) {
        if(destAdd != 3) {
           
            *wordHptr = (*wordHptr)->next;
            if(destAdd == 1) {
                int addr;
                addr = getLabelAddress(variablesPtr->labelHptr,arr[IMPORTANT]);
                if(addr == -1) {
                    variablesPtr->status = MissingLabel;
                }
                else {
                    if(getLabelType(variablesPtr->labelHptr,arr[IMPORTANT]) == External) {
                        (*wordHptr)->word.index = E;
                        strcpy((*wordHptr)->externLabel,arr[IMPORTANT]);
                    }
                    else
                        (*wordHptr)->word.index = (addr<<3) + R;
                    
                }
            }
            else if(destAdd == 2) {
                int addr;
                strcpy(arr[IMPORTANT],arr[IMPORTANT]+1);
                addr = getLabelAddress(variablesPtr->labelHptr,arr[IMPORTANT]);
                if(addr == -1) {
                    variablesPtr->status = MissingLabel;
                }
                else {
                   if(getLabelType(variablesPtr->labelHptr,arr[IMPORTANT]) == External)
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

Status addEntryProperty(labelTableNodePtr hptr,char *str) {
    if(labelInList(hptr, str)) {
        Type t;
        t = getLabelType(hptr, str);
        if(t == External)
            return LabelEntryAndExtern;
        setType(hptr,str,Entry);
        return Valid;
    }
    return MissingLabel;
}
