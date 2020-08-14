#include "firstPass.h"

static Bool hasSymbol;
static symbolTableNodePtr symbolHptr;
static wordNodePtr dataHptr;
static wordNodePtr codeHptr;

Status firstPass(FILE *f) {
    char *str = (char*) malloc(LINE_LEN);
    Statement state;
    state = getLine(f, &str); /* get the line and the statement */
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
    char *tempStr;
    Word word;
    status = findInstructionsErrors(line);
    if(status != Valid)
        return status;
    /* find the label */
    tempStr = findSymbol(line);
    if(tempStr) {
        addSymbol(line);
        line+=strlen(tempStr);
    }
    strcpy(line,strip(line));
    /* find the opcode and funct */
    word.code.opcode = findOpcode(line);
    word.code.funct = findFunct(line);
    if(word.code.opcode == -1)
        return UnknownOperation;
    line += word.code.opcode == 15 ? 4 : 3; /* go to the next char after the operation */
    strcpy(line,strip(line));
    if(word.code.opcode <= 4)
        fillTwoOperands(line,&word); // write the function
    else if(word.code.opcode <= 13)
        fillOneOperand(line,&word); // write the function
    else /* operation has no operands */
        if(strcmp(line,"")) /* if there is text left raise error */
            return NeedlessOperands;
        else {
            IC++;
            return Valid;
        }
    
}

Status fillTwoOperands(char *str, Word *word)
{
	char arr[STRING_PARTS][LINE_LEN];
	int op1,op2;
	split(str,",",arr);

	strcpy(arr[IMPORTANT],strip(arr[IMPORTANT]));
	strcpy(arr[REST],strip(arr[REST]));
	if(!strcmp(arr[IMPORTANT],"") || !strcmp(arr[REST],""))
	{
		return MissingOperand;
	}
	op1 = findAddressMethod(arr[IMPORTANT]);
	op2 = findAddressMethod(arr[REST]);
    if(op1 == -1 || op2 == -1)
        return InvalidOperand;
    
	if((*word).code.opcode < 3)
	{
		if(op1 == 2 || op2 == 2)
			return InvalidOperand;
		if((*word).code.opcode % 2 ==0 && op2 %2 == 0)
			return InvalidOperand;
	}
	else
	{
		if(op1 != 1 || op2 % 2 ==0)
			return InvalidOperand;
	}

	IC+=1;
	if(op1 < 2)
		IC+=1;
	if(op2 < 2)
		IC+=1;
}

Status fillOneOperand(char *str,Word *word)
{
	int op;
	strcpy(str, strip(str));
	if(str == '\n' || str == '\0')
		return MissingOperand;
	op = findAddressMethod(str);

	if(((*word).code.opcode == 5 || (*word).code.opcode ==12)&& op %2 ==0)
		return InvalidOperand;
	if((*word).code.opcode == 0 && op >2)
		return InvalidOperand;
	if((*word).code.opcode == 13 && op == 2)
		return InvalidOperand;

	IC+=1;
	if(op < 2)
		IC+=1;
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
    if(symbolInList(symbolHptr,symbol))
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
    newSymbol.location = Code;
    newSymbol.next = NULL;
    newSymbol.type = findType(str,symbol);
    newSymbol.value = IC;
    addToList(&symbolHptr,newSymbol);
}

/* return the symbol, NULL if there is no symbol */
char *findSymbol(char *str) {

    return validToken(":",str);
}

/* return the type of the line */
Type findType(char *str, char *symbol) {
    char *strCopy = (char *) malloc(LINE_LEN);
    int flag = 0;
    strCopy += strlen(symbol);
    strcpy(strCopy,strip(strCopy));
    if(!strcmp(strCopy,".extern"))
        flag = External;
    else if(!strcmp(strCopy,".entry"))
        flag = Entry;
    else
        flag = None;
    free(strCopy);
    return flag;
}

int findAddressMethod(char *str) {
    if(*str == '#')
        return 0;
    if(*str == '&')
        return 2;
    if(!strcmp(str,"r0") || !strcmp(str,"r1") || !strcmp(str,"r2") || 
       !strcmp(str,"r3") || !strcmp(str,"r4") || !strcmp(str,"r5") || 
       !strcmp(str,"r6") || !strcmp(str,"r7"))
       return 3;
    return 1;
}