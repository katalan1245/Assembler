#include "firstPass.h"

/* Handle the first pass of the assembler */
void firstPass(struct variables *variablesPtr) {
    Statement state;
    Word word;
    variablesPtr->lineCounter=0;

    while(!feof(variablesPtr->file)) {
        variablesPtr->lineCounter++;
        state = getLine(variablesPtr->file, &variablesPtr->line);
        strcpy(variablesPtr->line,strip(variablesPtr->line));

        /* classify which statement is the line */

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

    updateTables(variablesPtr);
}

/* Handle the instruction statement, return the status of the statement
 * Fill the word, update the linked list and do anything
*/
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

        status = checkDirectiveLabel(variablesPtr,None);
        if(status != Valid)
            return status;

        addSymbol(symbol, CodeImage);
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

	if(word->code.opcode < 3)
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

    word->code.ARE = A;
    word->code.srcAdd = op1;
    word->code.destReg = op2;
    word->code.srcReg = op1 == 3 ? findReg(arr[IMPORTANT]) : 0;
    word->code.destReg = op2 == 3 ? findReg(arr[REST]) : 0;
    addWordToImage(variablesPtr->codeHptr,*word);
    variablesPtr->IC++;

    if(op1 == 0) {
        addNumberWord(variables,arr[IMPORTANT]);
        variablesPtr->IC++;
    }
    else if(op1 == 1) {
        addEmptyWord(variables);
        variablesPtr->IC++;
    }
    
    if(op2 == 0) {
        addNumberWord(variables,arr[REST]);
        variablesPtr->IC++;
    }
    else if(op2 == 1){
        addEmptyWord(variablesPtr);
		variablesPtr->IC++;
    }
    return Valid;
}

Status fillOneOperand(char *str,Word *word, struct variables *variablesPtr)
{
	int op;
    word->code.srcAdd=0;
    word->code.srcReg=0;
	strcpy(str, strip(str));

	if(!strcmp(str,""))
		return MissingOperand;
	op = findAddressMethod(str);

	if((word->code.opcode == 5 || word->code.opcode ==12) && op % 2 == 0)
		return InvalidOperand;
	if(word->code.opcode == 9 && op % 3 == 0)
		return InvalidOperand;
	if(word->code.opcode == 13 && op == 2)
		return InvalidOperand;

    word->code.ARE = A;
    word->code.destAdd = op;
    word->code.destReg = op == 3 ? findReg(str) : 0;

    addWordToImage(variablesPtr->codeHptr,*word);

	variablesPtr->IC++;

	if(op == 0) {
        addNumberWord(variables,arr[IMPORTANT]);
        variablesPtr->IC++;
    }
    else if(op != 3) {
        addEmptyWord(variables);
        variablesPtr->IC++;
    }

    return Valid;
}

Status handleDirective(struct variables *variablesPtr, Word *wordPtr) {
	char *symbol;
    char lineCopy[LINE_LEN];
	Type type;
    DataOrString varType;
    Bool hasSymbol;
	char arr[STRING_PARTS][LINE_LEN];
    strcpy(lineCopy,variablesPtr->line);
    
    split(lineCopy," \t",arr);
    symbol = findSymbol(arr[IMPORTANT]);
    hasSymbol = symbol == NULL ? False : True;
    strcpy(lineCopy,strip(arr[REST]));
    varType = findDataOrString(lineCopy);
    type = findEntryOrExternal(lineCopy);
    split(lineCopy," \t",arr);
    strcpy(arr[REST],strip(arr[REST])

    if(type == None) {
        if(varType == None)
            return MissingDirective;
        if(hasSymbol) {
            strcpy(variablesPtr->symbol,symbol);
            status = checkSyntaxValidLabel(variablesPtr);
            if(status != Valid)
                return status;
            
            status = checkDirectiveLabel(arr[REST],None);
            if(status != Valid)
                return status;
            addSymbol(variablesPtr,DataImage);
        }
        
        if(varType == StringVar) {
            int ind;
            int i;
            ind = indFromEnd(arr[REST], '\"');
            
            if(arr[REST][0] != '\"')
                return InvalidOperand;
            if(ind == strlen(arr[REST]))
                return NoClosingQuotes;
            if(ind != 1)
                return TextAfterQuotes;
            
            for(i=1;i<strlen(arr[REST])-2;i++) {
                addStringWord(variablesPtr->dataHptr,arr[REST][i]);
                variablesPtr->DC++;
            }
            addStringWord(variablesPtr->dataHptr,'\0');
            return Valid;
        }

        if(varType == DataVar) {
            char strCopy[LINE_LEN];
            long num;
            strcpy(strCopy,arr[REST]);
            while(!strcmp(strCopy,""))
            {
                Word w;
                split(strCopy,',',arr);
                strcpy(arr[IMPORTANT],strip(arr[IMPORTANT]));
                if(checkNum(arr[IMPORTANT] != Valid))
                    return InvalidOperand;

                num = strtol(arr[IMPORTANT],NULL,10);
                if(num > 8388607 || num < -8388608)
                    return InvalidOperand;
                strcpy(strCopy,strip(arr[REST]));
                w.index = num;
                addWordToImage(variablesPtr->dataHptr,w);
                variablesPtr->DC++;
            }
            return Valid;
        }
    }
    else if(type == Entry)
        return checkSyntaxValidLabel(arr[REST]);
    else {
        symbolTableNode node;
        status = checkSyntaxValidLabel(arr[REST]);
        if(status != Valid)
            return status;
        status = checkDirectiveLabel(arr[REST],External);
        if(status != Valid)
            return status;
        strcpy(node.symbol,arr[REST]);
        node.address = 0;
        node.location = DataImage;
        node.type = External;
        addToList(variablesPtr->symbolHptr,node);
        return Valid;
    }

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
        newSymbol.location = CodeImage;
    }
    else {
        newSymbol.address = variablesPtr->DC;
        newSymbol.location = DataImage;
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

    if(!isspace(variablesPtr->line[strlen(str)]))
        return MissingWhitespace
    

    for(i = 1; i < strlen(str); i++) { /*check that there are only letters and numbers in the label*/
		if(isalnum(str[i])) /* is alpha numeric */
			return LabelInvalidCharacters;
	}

	return Valid; /*valid*/
}


Status checkAddValidLabel(struct variables *variablesPtr,Type type) {
    
    if(symbolInList(variablesPtr->symbolHptr,variablesPtr->symbol)) {
        if(getSymbolType(variablesPtr->symbolHptr,variablesPtr->symbol) == None)
    }
    if(strlen(variablesPtr->symbol) == strlen(variablesPtr->line))
        return MissingOperand;

    
    return Valid;
}

Status checkDirectiveLabel(struct variables *variablesPtr,Type type) {
    if(symbolInList(variablesPtr->symbolHptr,variablesPtr->symbol)) {
        Type symbolType;
        symbolType = getSymbolType(variablesPtr->symbolHptr, variablesPtr->symbol);
        if(symbolType == None) {
            if(type != Entry)
                return InvalidLabel;
        }

        else if(symbolType == External){
            if(type == None)
                return SymbolDefinedAndExtern;
            if(type == Entry)
                return SymbolEntryAndExtern;
        }
    }
    return Valid;
}

void addNumberWord(struct variables *variablesPtr, char *str) {
    Word w;
    long num;
    num = strtol(arr[IMPORTANT],NULL,10);
    if(num < 0)
        w.index = (long) (pow(2,21) + num);
    w.index <<= 3;
    w.index |= A;
    addWordToImage(variablesPtr->codeHptr,*word);
}

void addStringWord(struct variables *variablesPtr, char ch) {
    Word w;
    w.index = (int) ch;
    addWordToImage(variablesPtr->dataHptr,w);
}

void updateTables(struct variables *variablesPtr) {
    symbolTableNodePtr symbolHptr = variablesPtr->symbolHptr;
    wordNodePtr wordHptr = variablesPtr->dataHptr;
    while(symbolHptr) {
        if(symbolHptr->location == DataImage) {
            symbolHptr->address += variablesPtr->IC;
        }
        symbolHptr = hptr->next;
    }

    while(wordHptr) {
        wordHptr->address+=wordHptr->IC;
        wordHptr = wordHptr->next;
    }
}