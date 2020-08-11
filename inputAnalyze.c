#include "defaults.h"
#include "inputAnalyze.h"

/* NEED TO ADD THE HANDLE FUNCTIONS */

Status mini_main(FILE *f) {
    char line[82];
    Statement state;
    fgets(line,82,f);
    strcpy(line,strip(line));
    lineCounter++;

    state = firstCheck(line);
    if(state == Empty || state == Comment)
        return Valid;
    if(state == Instruction)
        return handleInstruction(line);
    else
        return handleDirective(line);
}

Statement firstCheck(char *str) {
    char arr[STRING_PARTS][LINE_LEN];
    if(str[strlen(str)-1] != '\n')
        return Invalid;
    if(str[0] == ';')
        return Comment;
    if(str[0] == '\0')
        return Empty;
    if(split(str,":",arr) == FULL_CELL) {
        size_t len1,len2;
        len1 = strlen(arr[IMPORTANT]);
        split(str,"\"",arr);
        len2 = strlen(arr[IMPORTANT]);
        if((len1 < len2) /* if : comes before " or one of this word exist in the line */
            || strstr(arr[IMPORTANT],".data")
            || strstr(arr[IMPORTANT],".string")
            || strstr(arr[IMPORTANT],".entry")
            || strstr(arr[IMPORTANT],".extern"))
            return Instruction;
    }
    return Directive;
}

/* strip the string str from whitespaces and return the stripped string */
char *strip(char *str) {
    size_t size;
    char *end;

    /* if string empty, return it */
    size = strlen(str) - 1;
    if (!size)
        return str;

    /* remove all whitespaces from the end of the string */
    end = str + size - 1;
    while(end >= str && isspace(*end))
        --end;
    *(end + 1) = '\0';

    /* remove all whitespaces from the start of the string */
    while(str && isspace(*str))
        str++;

    return str;
}

/* split the string str by the delimeters delim
 * arr[0] will contain the string before the delim, and arr[1] will contain the string after
 * return if the delimeters exists, same as arr[1] = "" */
int split(char *str, char *delim, char arr[STRING_PARTS][LINE_LEN]) {
    char *tok;
    char strCopy[1000];

    strcpy(strCopy, str);
    tok = strtok(strCopy, delim);
    if (strlen(tok) == strlen(str)) {
        strcpy(arr[0], str);
        strcpy(arr[1], "");
        return EMPTY_CELL;
    }
    tok = strtok(NULL, delim);
    strcpy(arr[0], strCopy);
    strcpy(arr[1], (str + strlen(strCopy) + 1));
    return FULL_CELL;
}

int findOpcode(char *str) {
    char arr[STRING_PARTS][LINE_LEN];
    char oper[LINE_LEN];
    split(str,whitespace,arr);
    strcpy(oper,arr[IMPORTANT]);
    if(!strcmp(oper,"mov"))
        return 0;
    if(!strcmp(oper,"cmp"))
        return 1;
    if(!strcmp(oper,"add") || !strcmp(oper,"sub"))
        return 2;
    if(!strcmp(oper,"lea"))
        return 4;
    if(!strcmp(oper,"clr") || !strcmp(oper,"not") || !strcmp(oper,"inc") || !strcmp(oper,"dec"))
        return 5;
    if(!strcmp(oper,"jmp") || !strcmp(oper,"bne") || !strcmp(oper,"jne"))
        return 9;
    if(!strcmp(oper,"red"))
        return 12;
    if(!strcmp(oper,"prn"))
        return 13;
    if(!strcmp(oper,"rts"))
        return 14;
    if(!strcmp(oper,"stop"))
        return 15;
    return -1;
}

int findReg(char *str) {
    char arr[STRING_PARTS][LINE_LEN];
    char oper[LINE_LEN];
    split(str,whitespace,arr);
    strcpy(oper,arr[IMPORTANT]);

    if(!strcmp(oper,"r0"))
        return 0;
    if(!strcmp(oper,"r1"))
        return 1;
    if(!strcmp(oper,"r2"))
        return 2;
    if(!strcmp(oper,"r3"))
        return 3;
    if(!strcmp(oper,"r4"))
        return 4;
    if(!strcmp(oper,"r5"))
        return 5;
    if(!strcmp(oper,"r6"))
        return 6;
    if(!strcmp(oper,"r7"))
        return 7;
    return -1;
}

int findFunct(char *str) {
    char arr[STRING_PARTS][LINE_LEN];
    char oper[LINE_LEN];
    split(str,whitespace,arr);
    strcpy(oper,arr[IMPORTANT]);
    if(!strcmp(oper,"add") || !strcmp(oper,"clr") || !strcmp(oper,"jmp"))
        return 1;
    if(!strcmp(oper,"sub") || !strcmp(oper,"not") || !strcmp(oper,"bne"))
        return 2;
    if(!strcmp(oper,"inc") || !strcmp(oper,"jsr"))
        return 3;
    if(!strcmp(oper,"dec"))
        return 4;
    return 0;
}