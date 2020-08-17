#include "firstPass.h"

static Bool hasSymbol;
static symbolTableNodePtr symbolHptr;
static wordNodePtr dataHptr;
static wordNodePtr codeHptr;

Status firstPass(FILE *f) {
    char str[LINE_LEN];
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
    /*Status status;*/
    char *tempStr;
    Word word;
    /*
    status = findInstructionsErrors(line);
    if(status != Valid)
        return status;
    */
    /* find the label */
    tempStr = findSymbol(line);
    if(tempStr) {
        if(!checkValidLabel(tempStr) || symbolInList(symbolHptr,tempStr))
        	return InvalidLabel;

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
	char *tempStr;
	Type type;
	char arr[STRING_PARTS][LINE_LEN];

	tempStr = findSymbol(line);
    if(tempStr) {
    	type = findType(line, tempStr);
        if(type != External && type != Entry)
        {
        	if(!checkValidLabel(tempStr) || symbolInList(symbolHptr,tempStr))
        		return InvalidLabel;

        	addSymbol(line);
        }

        line+=strlen(tempStr);
    }
    else
    	type = findType(line, "");

    if(*line != ' ')
    	return -1;

    strcpy(line,strip(line));

    split(line,whitespace,arr);
    strcpy(line, strip(arr[REST]));

    if(type == none)
    	return InvalidDirective;

    else if(type == External || type == Entry)
    {
    	int oper = findAddressMethod(line);
    	if(oper != 1)
    		return InvalidOperand;
    	/*need to add to list*/
    }
    else if(type == String)
    {
    	int ind = findFromEnd(line, '\"');
    	if(*line != '\"')
    		return InvalidOperand;
    	if(ind == strlen(line))
    		return NoClosingQuotes;
    	if(ind != 1)
    		return TextAfterQuotes;
    	DC+=strlen(line)-2;
    }
    else 
    {
    	while(line)
    	{
    		split(line,',',arr);
    		strcpy(arr[IMPORTANT],strip(arr[IMPORTANT]));
    		if(!checkNum(arr[IMPORTANT]))
    			return InvalidOperand;
    		DC++;
    		strcpy(line,strip(arr[REST]));
    	}
    }

    return Valid;
}

/* return the error, or valid if there are no errors */
Status findInstructionsErrors(char *str) {
    char strCopy[LINE_LEN];
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

int findAddressMethod(char *str) {
    if(*str == '#')
    {
    	if(checkNum(*(str+1)))
    	{
    		long num = atol(*(str+1));
    		if(num >= -1048576 && num <= 1048575)
    			return 0;
    	}
    	return InvalidNumber;
    }

    if(*str == '&')
    {
    	if(checkValidLabel(*(str+1)))
    		return 2;

    	return InvalidLabel;
    }

    if(findReg(str) != -1)
    	return 3;

    if(checkValidLabel(*(str+1)))
    	return 1;

    return InvalidLabel;
}

int checkNum(char *str)
{
	int i = 0;
	if(*str == '+' || *str == '-')
		i++;

	for(; i < strlen(str); i++)
	{
		if(str[i] < '0' || str[i] > '9')
			return 0;
	}
	return 1;
}

int checkValidLabel(char *str)
{
	int i;
	if(strlen(str) > 31)
		return 0;/*Too long*/
	
	if((*str > 'z' || *str < 'a') && (*str > 'Z' || *str < 'A'))
		return 0;/*Doesn't start with a letter*/

	if(findReg(str) != -1 || findOpcode(str) != -1)
		return 0;/*Label with the same name of a saved name*/

	for(i = 1; i < strlen(str); i++)/*check that there are only letters and numbers in the label*/
	{
		if((str[i] > '9' || str[i] < '0') && (str[i] > 'z' || str[i] < 'a') && (str[i] > 'Z' || str[i] < 'A'))
			return 0;
	}
	return 1;/*valid*/
}
