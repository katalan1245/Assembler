#include "secondPass.h"

Status secondPass(FILE *f) {
    Word memoryArray[IC+DC-STARTING_IC];
    char symbol[LABEL_LEN];
    char *str = (char*) malloc(LINE_LEN);
    Type type;
    Statement state;
    state = getLine(f, &str);
    strcpy(str,strip(str));
    if(state == Empty || state == Comment || state == Directive)
        return Valid;
    strcpy(symbol, findSymbol(str));
    type = findType(str,symbol);
    if(type == External)
        return Valid;
    else if(type == Entry) {
        str+=strlen(symbol);
        strcpy(str,strip(str));
        str+=strlen(".entry");
        strcpy(str,strip(str));
        addEntryProperty(str);
    }
}

Status addEntryProperty(char *str) {
    if(symbolInList(symbolHptr,str))
}