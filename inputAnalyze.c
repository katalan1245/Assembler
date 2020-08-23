#include "inputAnalyze.h"

void advanceToNextLine(FILE*);
Statement firstCheck(char*);

/* get the line and return the statement of the line */
Statement getLine(variables *variablesPtr) {
    Statement state;
    size_t len;
    memset(variablesPtr->line,'\0',LINE_LEN);
    fgets(variablesPtr->line,LINE_LEN,variablesPtr->file);
    len = strlen(variablesPtr->line);
    
    /* check if the length of the line length is valid (80 chars exclude \n\0), if invalid go to the next line */
    if(len == LINE_LEN - 1 && variablesPtr->line[len-1] != '\0' && variablesPtr->line[len-1] != '\n') { 
        advanceToNextLine(variablesPtr->file);
        return Invalid;
    }
    if(feof(variablesPtr->file)) { /* make sure if we have eof and the length is valid, to place \n at the end of the line */
        if(variablesPtr->line[80] == '\0') {
            variablesPtr->line[strlen(variablesPtr->line)] = '\n';
        }
    }

    state = firstCheck(variablesPtr->line);
    return state;
}

/* get to the next line of the file */
void advanceToNextLine(FILE *f)
{
    while(fgetc(f) != '\n');
}

/* return the statement of the sentence or invalid*/
Statement firstCheck(char *str) {
    char arr[STRING_PARTS][LINE_LEN];
    memset(arr,0,sizeof(arr[0][0])*STRING_PARTS*LINE_LEN);
    if(str[strlen(str)-1] != '\n') /* line too long */ 
        return Invalid;
    strcpy(str,strip(str));
    if(str[0] == '\n' || str[0] == '\0') /* first char is \0 */
        return Empty;
    if(str[0] == ';') /* first char is ; */
        return Comment;
    if(split(str,".",arr) == DELIM_EXIST) {
        /* check if .  is the first char, or this it comes after whitespace or : (after label and not inside string or a number) */
        if(!strcmp(arr[IMPORTANT],""))
            return Directive;
        if(isspace(arr[IMPORTANT][strlen(arr[IMPORTANT])-1]) || arr[IMPORTANT][strlen(arr[IMPORTANT]-1)] == ':')
            return Directive;
        /*return Directive;*/
    }
    return Instruction;
}

/* strip the string str from whitespaces and return the stripped string */
char *strip(char *str) {
    size_t size;
    char *copy = (char*) malloc(LINE_LEN);
    char *temp;
    char *end;
    temp = copy;
    memset(copy,'\0',LINE_LEN);
    strcpy(copy,str);
    /* if string empty, return it */
    size = strlen(copy);
    if (!size) {
        strcpy(str,copy);
        free(copy);
        return str;
    }

    /* remove all whitespaces from the end of the string */
    end = copy + size - 1;
    while(end >= copy && isspace(*end))
        --end;
    *(end + 1) = '\0';

    /* remove all whitespaces from the start of the string */
    while(copy && isspace(*copy))
        copy++;

    strcpy(str,copy);
    copy = temp;
    free(copy);
    return str;
}

/* split the string str by the delimeters delim
 * arr[0] will contain the string before the delim, and arr[1] will contain the string after
 * return if the delimeters exists, same as arr[1] = "" */
int split(char *str, char *delim, char arr[STRING_PARTS][LINE_LEN]) {
    char *tok;
    char strCopy[LINE_LEN];
    char temp[LINE_LEN];
    int i;

    strcpy(strCopy, str);

    if(!strcmp(strCopy,"")) { /* if the string is empty */
        strcpy(arr[IMPORTANT],"");
        strcpy(arr[REST],"");
        return DELIM_NOT_EXIST;
    }

    /* check if the delimeter on the first char */
    for(i=0;i<strlen(delim);i++) {
        if(str[0] == delim[i]) {
            strcpy(arr[IMPORTANT],"");
            strcpy(arr[REST],strCopy+1);
            return DELIM_EXIST;
        }
    }

    tok = strtok(strCopy, delim); /* look for the first token */
    if (tok != NULL && strlen(tok) == strlen(str)) { /* if there is no delim */
        strcpy(arr[IMPORTANT], str);
        strcpy(arr[REST], "");
        return DELIM_NOT_EXIST;
    }

    tok = strtok(NULL, delim); /* look for the next token */
    strcpy(arr[IMPORTANT], strCopy);
    strcpy(temp, (str + strlen(strCopy) + 1)); /* we want the rest of the string, and not until the next token */
    strcpy(arr[REST],temp);
    return DELIM_EXIST;
}

/* returns the opcode of the str, -1 if not an opcode */
int findOpcode(char *str) {
    char arr[STRING_PARTS][LINE_LEN];
    char oper[LINE_LEN];
    split(str," \t",arr);
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
    if(!strcmp(oper,"jmp") || !strcmp(oper,"bne") || !strcmp(oper,"jsr"))
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

/* return the register number of str, -1 if not a register */
int findReg(char *str) {
    if(!strcmp(str,"r0"))
        return 0;
    if(!strcmp(str,"r1"))
        return 1;
    if(!strcmp(str,"r2"))
        return 2;
    if(!strcmp(str,"r3"))
        return 3;
    if(!strcmp(str,"r4"))
        return 4;
    if(!strcmp(str,"r5"))
        return 5;
    if(!strcmp(str,"r6"))
        return 6;
    if(!strcmp(str,"r7"))
        return 7;
    return -1;
}

/* return the funct of str, 0 if there is no funct */
int findFunct(char *str) {
    char arr[STRING_PARTS][LINE_LEN];
    char oper[LINE_LEN];
    split(str," \t",arr);
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

/* return if the str is .entry, .extern or neither */
Type findEntryOrExternal(char *str) {
    Type flag = NoneEntOrExt;
    if(!strcmp(str,".extern"))
        flag = External;
    else if(!strcmp(str,".entry"))
        flag = Entry;
    return flag;
}

/* return if the str is .data, .string or neither */
DataOrString findDataOrString(char *str) {
    DataOrString flag = NoneDataOrStr;
    if(!strcmp(str,".data"))
        flag = DataVar;
    else if(!strcmp(str,".string"))
        flag = StringVar;
    return flag;
}

/* find the first occurence of ch in the str and return the len, if not found return -1 */
int findFromEnd(char *str, char ch) {
    int i;
    for(i=strlen(str)-1;i>=0;i--) {
        if(str[i] == ch)
            return strlen(str) - i;
    }
    return -1;
}

/* takes a line, return the label, "" if there is no label */
char *findLabel(char *str) {
    char *strCopy = (char*)  malloc(LINE_LEN);
    char arr[STRING_PARTS][LINE_LEN];
    strcpy(strCopy,str);
    if(split(strCopy,":",arr) == DELIM_EXIST) { /* if there is ':' at the line */
        char checkForValid[STRING_PARTS][LINE_LEN];
        split(strCopy,"\"",checkForValid);
        if(strlen(checkForValid[IMPORTANT]) > strlen(arr[IMPORTANT])) /* check if the ':' inside a quotes */
            strcpy(strCopy,arr[IMPORTANT]);
        else
            strcpy(strCopy,"");
    }
    else
        strcpy(strCopy,"");
    return strCopy;
}

/* takes an operand, and return the addressing method, -1 if invalid */
int findAddressMethod(variables *variablesPtr, char *str) {
    if(*str == '#')
    {
    	if(checkNum(++str) == Valid)
    	{
            long num = strtol(str,NULL,10);
            if(num > MAX_21_SIGNED || num < MIN_21_SIGNED) { /* num too big or too small */
    			variablesPtr->status = NumOutOfMemory;
                return -1; /* invalid number */
            }
            return 0;
    	}
    	return -1; /* invalid number */
    }

    if(*str == '&')
    {
        checkSyntaxValidLabel(variablesPtr,++str,False);
        if(variablesPtr->status == Valid)
    		return 2;

    	return -1; /* invalid label */
    }

    if(findReg(str) != -1)
    	return 3;

    checkSyntaxValidLabel(variablesPtr,str,False);
    if(variablesPtr->status == Valid)
    	return 1;

    return -1;
}


/* takes a string and check if it is a valid integer */
Status checkNum(char *str)
{
	int i = 0;
	if(*str != '+' && *str != '-' && !isdigit(*str))
        return InvalidNumber;
	i++;

	for(; i < strlen(str); i++)
		if(!isdigit(str[i]))
			return InvalidNumber;
	return Valid;
}


/* takes a label and check the syntax of the label, if invalid puts the status in variablesPtr->status*/
void checkSyntaxValidLabel(variables *variablesPtr, char *sym, Bool checkSpace)
{
	int i;
    char str[LINE_LEN];
    strcpy(str,sym);
	if(strlen(str) > 31)
		variablesPtr->status = LabelTooLong; /*Too long*/
	
	if((*str > 'z' || *str < 'a') && (*str > 'Z' || *str < 'A'))
		variablesPtr->status = LabelInvalidStart; /*Doesn't start with a letter*/

	if(findOpcode(str) != -1 || findReg(str) != -1 || findEntryOrExternal(str) != NoneEntOrExt)
		variablesPtr->status = ReservedLabelName; /*Label with the same name of a reserved name*/

	if(checkSpace) {
        if (!isspace(variablesPtr->line[strlen(str)+1])) /* check if there is a whitespace after ':' */
            variablesPtr->status = MissingWhitespace;
    }
    
    for(i = 1; i < strlen(str); i++) { /*check that there are only letters and numbers in the label*/
		if(!isalnum(str[i])) /* is alpha numeric */
			variablesPtr->status = LabelInvalidCharacters;
	}
}