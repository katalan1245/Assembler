/* IMPORTANT LABELS FUNCTIONS TO ADD !!!!!!!!!*/

#include "firstPass.h"

static Bool hasSymbol;
static Bool hasError = False;
static symbolTableNodePtr symbolHptr;
static wordNodePtr dataHptr;
static wordNodePtr codeHptr;

Status firstPass(struct variables *variablesPtr) {
    Statement state;
    Word word;

    while(!feof(variablesPtr->file)) {
        state = getLine(variablesPtr->file, &variablesPtr->line);
        strcpy(variablesPtr->line,strip(variablesPtr->line));

        if(state == Invalid)
            variablesPtr->status = LineTooLong;

        if(state == Instruction)
            variablesPtr->status = handleInstruction(variablesPtr->line,&word);

        if(state == Directive)
            variablesPtr->status = handleDirective(variablesPtr->line,&word);
        
        if(variablesPtr->status != Valid)
            variablesPtr->foundError = True;

        if(!variablesPtr->foundError)
            if(state == Directive)
                addWordToImage(variablesPtr->dataHptr,word);
            else
                addWordToImage(variablesPtr->codeHptr,word);

        printError(variablesPtr);
    }
    return Valid;
}

Status handleInstruction(struct variables *variablesPtr,Word *wordPtr) {
    char *symbol;
    char *lineCopy = (char*) malloc(LINE_LEN);
    Status status;
    strcpy(lineCopy,variablesPtr->line);
    
    /* find the label */
    symbol = findSymbol(lineCopy);
    if(symbol) {
        strcpy(variablesPtr->symbol,symbol);
        status = checkSyntaxValidLabel(variablesPtr);
        if(status != Valid)
            return status;

        status = checkAddValidLabel(variablesPtr);
        if(status != Valid)
            return status;

        if(strlen(symbol) >= strlen(lineCopy) || lineCopy[strlen(symbol)])
        addSymbol(symbol, Code);
        lineCopy+=strlen(symbol);
    }
    strcpy(lineCopy,strip(lineCopy));
    /* find the opcode and funct */
    wordPtr->code.opcode = findOpcode(lineCopy);
    wordPtr->code.funct = findFunct(lineCopy);

    if(wordPtr->code.opcode == -1)
        return UnknownOperation;

    lineCopy += wordPtr->code.opcode == 15 ? 4 : 3; /* go to the next char after the operation */
    strcpy(lineCopy,strip(lineCopy));
    if(wordPtr->code.opcode <= 4)
        status = fillTwoOperands(lineCopy,wordPtr,variablesPtr); // write the function
    else if(wordPtr->code.opcode <= 13)
        status = fillOneOperand(lineCopy,wordPtr); // write the function
    else /* operation has no operands */
        if(strcmp(lineCopy,"")) /* if there is text left raise error */
            return TextAfterCommand;
        else {
            variablesPtr->IC++;
            return Valid;
        }
    
}

Status fillTwoOperands(char *str, Word *word, struct variables *variablesPtr)
{
	char arr[STRING_PARTS][LINE_LEN];
	int op1,op2;
	split(str,",",arr);

	strcpy(arr[IMPORTANT],strip(arr[IMPORTANT]));
	strcpy(arr[REST],strip(arr[REST]));
	if(!strcmp(arr[IMPORTANT],"") || !strcmp(arr[REST],""))
		return MissingOperand;
	op1 = findAddressMethod(arr[IMPORTANT]);
	op2 = findAddressMethod(arr[REST]);

    if(op1 == -1 || op2 == -1)
        return InvalidOperand;

	if((*word).code.opcode < 3)
	{
		if(op1 == 2 || op2 == 2)
			return InvalidOperand;
        if(word->code.opcode % 2 == 0 && op2 == 0)
			return InvalidOperand;
	}
	else
	{
		if(op1 != 1 || op2 % 2 ==0)
			return InvalidOperand;
	}

    
    word->code.srcAdd = op1;
    word->code.destReg = op2;
    word->code.srcReg = op1 == 3 ? findReg(arr[IMPORTANT]) : 0;
    word->code.destReg = op2 == 3 ? findReg(arr[IMPORTANT]) : 0;
	variablesPtr->IC++;
	if(op1 <= 2)
		variablesPtr->IC++;
	if(op2 <= 2)
		variablesPtr->IC++;
        
    addWordToImage(variablesPtr->codeHptr,*word);
    return Valid;
    /* MAINLY FINISHED WITH 2 OPERANDS */

Status fillOneOperand(char *str,Word *word, struct variables *variablesPtr)
{ /* NEED TO WORK ON 1 OPERAND */
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

	variablesPtr->IC+=1;
	if(op < 2)
		variablesPtr->IC+=1;
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

    split(line," \t",arr);
    strcpy(line, strip(arr[REST]));

    if(type == None)
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
    	long num;
    	while(line)
    	{
    		split(line,',',arr);
    		strcpy(arr[IMPORTANT],strip(arr[IMPORTANT]));
    		if(!checkNum(arr[IMPORTANT]))
    			return InvalidOperand;

    		num = atol(arr[IMPORTANT]);
    		if(num > 8388607 || num < -8388608)
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

/* the the symbol from the variables ptr with the location loc to the list */
void addSymbol(struct variables *variablesPtr, Location loc) {
    symbolTableNode newSymbol;
    newSymbol.symbol = variablesPtr->symbol;
    newSymbol.next = NULL;
    newSymbol.type = findType(variablesPtr);
    if(loc == Code) {
        newSymbol.address = variablesPtr->IC;
        newSymbol.location = Code;
    }
    else {
        newSymbol.address = variablesPtr->DC;
        newSymbol.location = Data;
    }
    addToList(&variablesPtr->symbolHptr,newSymbol);
}

/* return the symbol, NULL if there is no symbol */
char *findSymbol(char *str) {
    return validToken(":",str);
}

int findAddressMethod(char *str) {
    char *ptr;
    if(*str == '#')
    {
    	if(checkNum(++str) == Valid);
    	{
            long num = strtol(str,ptr,10);
            if(*ptr != '\0')
                return -1; /* invalid number */
            if(num > 8388607 || num < -8388608)
    			return -1; /* invalid number */
            return 0;
    	}
    	return -1; /* invalid number */
    }

    if(*str == '&')
    {
        
    	if(checkSyntaxValidLabel(str+1) == Valid)
    		return 2;

    	return -1; /* invalid label */
    }

    if(findReg(str) != -1)
    	return 3;

    if(checkValidLabel(*(str+1)))
    	return 1;

    return -1;
}

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

Status checkSyntaxValidLabel(struct variables *variablesPtr)
{
	int i;
    char str[LABEL_LEN];
    strcpy(str,variablesPtr->symbol);
	if(strlen(variablesPtr->symbol) > 31)
		return LabelTooLong; /*Too long*/
	
	if((*str > 'z' || *str < 'a') && (*str > 'Z' || *str < 'A'))
		return LabelInvalidStart; /*Doesn't start with a letter*/

	if(findOpcode(str) != -1 || findReg(str) != -1 || findType(str) != None)
		return ReservedLabelName; /*Label with the same name of a reserved name*/

    if(variablesPtr->line[strlen(str)] != " " || variablesPtr->line[strlen(str)] != "\t")
        return MissingWhitespace;

    for(i = 1; i < strlen(str); i++) { /*check that there are only letters and numbers in the label*/
		if(isalnum(str[i])) /* is alpha numeric */
			return LabelInvalidCharacters;
	}

	return Valid; /*valid*/
}


Status checkAddValidLabel(struct variables *variablesPtr,Type type) {
    
    if(symbolInList(variablesPtr->symbolHptr,variablesPtr->symbol))
        return SymbolAlreadyExist;

    if(strlen(variablesPtr->symbol) == strlen(variablesPtr->line))
        return MissingOperand;

    
    return Valid;
}

Status checkDirectiveLabel(struct variables *variablesPtr,Type type) {
    if(symbolInList(variablesPtr->symbolHptr,variablesPtr->symbol)) {
        Type symbolType;
        symbolType = getSymbolType(variablesPtr->symbolHptr, variablesPtr->symbol);
        if((symbolType == External && type == Entry) || (symbolType == Entry && type == External))
            return SymbolEntryAndExtern;
    }
    return Valid;
            
}