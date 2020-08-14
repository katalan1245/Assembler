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
    word.type = Code;
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
    word.data.code.opcode = findOpcode(line);
    word.data.code.funct = findFunct(line);
    if(word.data.code.opcode == -1)
        return UnknownOperation;
    line += word.data.code.opcode == 15 ? 4 : 3;
    strcpy(line,strip(line));
    if(word.data.code.opcode <= 4)
        fillTwoOperands(line,&word); // write the function
    else if(word.data.code.opcode <= 13)
        fillOneOperand(line,&word); // write the function
    else /* operation has no operands */
        if(strcmp(line,"")) /* if there is text left raise error */
            return NeedlessOperands;
        else { /* we can write the word */
            fillWordCode(&word,word.data.code.opcode,0,0,0,0,0,1,0,0);
            addWordToImage(codeHptr, word);
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
	if((*word).data.code.opcode < 3)
	{
		if(op1 == 2 || op2 == 2)
			return InvalidOperand;
		if((*word).data.code.opcode % 2 ==0 && op2 %2 == 0)
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

	if(((*word).data.code.opcode == 5 || (*word).data.code.opcode ==12)&& op %2 ==0)
		return InvalidOperand;
	if((*word).data.code.opcode == 0 && op >2)
		return InvalidOperand;
	if((*word).data.code.opcode == 13 && op == 2)
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
    if(!strncmp(strCopy,".extern",7))
        flag = External;
    else if(!strncmp(strCopy,".entry",6))
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
    if(!strncmp(str,"r0",2) || !strncmp(str,"r1",2) || !strncmp(str,"r2",2) || 
       !strncmp(str,"r3",2) || !strncmp(str,"r4",2) || !strncmp(str,"r5",2) || 
       !strncmp(str,"r6",2) || !strncmp(str,"r7",2))
       return 3;
    return 1;
}