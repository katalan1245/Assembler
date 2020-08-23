#include "secondPass.h"

void fillWord(variables *,wordNodePtr *, char *, int);
void secondPassInstruction(variables*,wordNodePtr*);
Status addEntryProperty(labelTableNodePtr,char*);
static int tempIC = 100;

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

        variablesPtr->lineCounter++;
        defaultValues(variablesPtr);

        strcpy(variablesPtr->line,strip(variablesPtr->line));
        if(state == Invalid)
            variablesPtr->status = LineTooLong;

        if(state == Comment || state == Empty)
            continue;
        back = findLabel(variablesPtr->line);
        strcpy(label,back);
        free(back);

        if(strcmp(label,"")) { /* if there is a label */
            split(variablesPtr->line," \t",arr);
            strcpy(variablesPtr->line, strip(arr[REST]));
        }

        if(state == Directive && variablesPtr->status == Valid) { /* if directive on seconds pass, we only need to deal with entry */
            Type t;
            split(variablesPtr->line," \t",arr); /* split between the dircetive and operand */
            t = findEntryOrExternal(arr[IMPORTANT]);
            if(t == Entry) { /* we already deal with extern */
                split(variablesPtr->line," \t",arr);
                strcpy(arr[REST],strip(arr[REST]));
                variablesPtr->status = addEntryProperty(variablesPtr->labelHptr,arr[REST]);
            }
        }
        else if(state == Instruction && variablesPtr->status == Valid) {
            secondPassInstruction(variablesPtr,&variablesPtr->codeHptr);
        }

        if(variablesPtr->status != Valid)
            variablesPtr->foundError = True;

        printError(variablesPtr);

        if(feof(variablesPtr->file))
            break;
    }
    tempIC = 100;
    variablesPtr->codeHptr = wordPtr;
}

/* Handle the instruction statement on the second pass */
void secondPassInstruction(variables *variablesPtr,wordNodePtr *wordHptr) {
    int opcode,srcAdd,destAdd;
    char arr[STRING_PARTS][LINE_LEN];
    opcode = getOpcode(variablesPtr->codeHptr,tempIC);
    srcAdd = getSrcAdd(*wordHptr,tempIC);
    destAdd = getDestAdd(*wordHptr,tempIC);
    memset(arr,0, sizeof(arr[0][0])*STRING_PARTS*LINE_LEN);

    split(variablesPtr->line," \t",arr); /* skip the command */
    split(arr[REST],",",arr); /* split the operands */

    strcpy(arr[IMPORTANT],strip(arr[IMPORTANT]));
    strcpy(arr[REST],strip(arr[REST]));

    if(opcode <= 4) { /* two operands */
        if(srcAdd != 3) {
            tempIC++;
            (*wordHptr) = (*wordHptr)->next;
            fillWord(variablesPtr,wordHptr,arr[IMPORTANT],srcAdd);
        }

        if(destAdd != 3) {
            tempIC++;
            (*wordHptr) = (*wordHptr)->next;
            fillWord(variablesPtr,wordHptr,arr[REST],destAdd);
        }
        
    }
    else if(opcode <= 13) {
        if(destAdd != 3) {
            *wordHptr = (*wordHptr)->next;
            if(destAdd == 1) {
                fillWord(variablesPtr,wordHptr,arr[IMPORTANT],destAdd);
                
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

/* fill the word based on address method 1 */
void fillWord(variables *variablesPtr,wordNodePtr *wordHptr, char *str, int addMethod) {
    if(addMethod == 1) {
        int addr;
        addr = getLabelAddress(variablesPtr->labelHptr,str);
        if(addr == -1) {
            variablesPtr->status = MissingLabel;
        }
        else {
            if(getLabelType(variablesPtr->labelHptr,str) == External) {
                (*wordHptr)->word.index = E;
                strcpy((*wordHptr)->externLabel,str);
            }
            else
                (*wordHptr)->word.index = (addr<<3) + R;
            
        }
    }
} 

/* add entry property to the label str */
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
