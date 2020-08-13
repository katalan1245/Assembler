#include "firstPass.h"

static Bool hasSymbol;

Status firstPass(FILE *f) {
    char *str = (char*) malloc(LINE_LEN);
    Statement state;
    state = getLine(f, &str);
    strcpy(str,strip(str));
    if(state == Instruction)
        return handleInstruction(str);
    if(state == Directive)
        return handleDirective(str);
    if(state == Invalid)
        return LineTooLong;
    return Valid;
}

Status handleInstruction(char *line) {
    Status status;
    status = findInstructionsErrors(line);
    if(status != Valid)
        return status;
    
    if(findSymbol(line))
        addSymbol(line);
    
    /* NOW ANALYZE */
}

Status handleDirective(char *line) {
    return Valid;
}

/* return the error, or valid if there are no errors */
Status findInstructionsErrors(char *str) {
    char *strCopy = (char*) malloc(LINE_LEN);
    char *symbol;
    char arr[STRING_PARTS][LINE_LEN];
    strcpy(strCopy,str);
    /* detect if symbol already exist */
    symbol = findSymbol(symbol);
    if(symbolInList(hptr,symbol))
        return SymbolAlreadyExist;
    /* detect unknown operation */
    strCopy+=strlen(symbol);
    strcpy(strCopy,strip(strCopy));
    split(strCopy,whitespace,arr);
    if(findOpcode(arr[IMPORTANT]) == -1)
        return UnknownOperation;

    return Valid;    
}

void addSymbol(char *str) {
    char *symbol = findSymbol(str);
    symbolTableNode newSymbol;
    newSymbol.symbol = symbol;
    newSymbol.location = code;
    newSymbol.next = NULL;
    newSymbol.isExternal = isExternal(str,strlen(newSymbol.symbol));
    newSymbol.value = IC;
    addToList(&hptr,newSymbol);
}