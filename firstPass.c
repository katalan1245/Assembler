#include "firstPass.h"
#include "defaults.h"

/* Handle the first pass of the assembler */
void firstPass(variables *variablesPtr) {
    Statement state;
    Word word;
    variablesPtr->lineCounter=0;

    /* NEED TO WORK ON EOF/ \N WHILE READING A FILE, LAST LINE TOO LONG */
    while(!feof(variablesPtr->file)) {
        state = getLine(variablesPtr);
        variablesPtr->lineCounter++;
        strcpy(variablesPtr->line,strip(variablesPtr->line));
        defaultValues(variablesPtr);

        /* classify which statement is the line */

        if(state == Invalid)
            variablesPtr->status = LineTooLong;

        if(state == Empty || state == Comment)
            continue;

        if(state == Instruction)
            handleInstruction(variablesPtr,&word);

        if(state == Directive)
            handleDirective(variablesPtr,&word);
        
        if(variablesPtr->status != Valid)
            variablesPtr->foundError = True;

        printError(variablesPtr);

        if(feof(variablesPtr->file))
            break;
    }

    updateTables(variablesPtr);
    printListProp(variablesPtr);
}

/* Handle the instruction statement, return the status of the statement
 * Fill the word, update the linked list and do anything
*/
void handleInstruction(variables *variablesPtr,Word *wordPtr) {
    char *lineCopy = (char*) malloc(LINE_LEN);
    char *temp = lineCopy;
    char *back;
    strcpy(lineCopy,variablesPtr->line);
    
    /* find the label */
    back = findSymbol(lineCopy);
    strcpy(variablesPtr->symbol,back);
    free(back);
    if(strcmp(variablesPtr->symbol,"")) {
        checkSyntaxValidLabel(variablesPtr,variablesPtr->symbol,True);
        if(variablesPtr->status != Valid)
            return;

        checkDirectiveLabel(variablesPtr,variablesPtr->symbol,NoneEntOrExt);
        if(variablesPtr->status != Valid)
            return;

        addSymbol(variablesPtr, CodeImage);
        lineCopy+=strlen(variablesPtr->symbol)+1;
        strcpy(lineCopy,strip(lineCopy));
    }

    /* find the opcode and funct */
    wordPtr->code.opcode = findOpcode(lineCopy);
    wordPtr->code.funct = findFunct(lineCopy);

    if(wordPtr->code.opcode == -1)
        variablesPtr->status = UnknownOperation;

    lineCopy += wordPtr->code.opcode == 15 ? 4 : 3; /* go to the next char after the operation */
    strcpy(lineCopy,strip(lineCopy));
    if(wordPtr->code.opcode <= 4)
        fillTwoOperands(lineCopy,wordPtr,variablesPtr);
    else if(wordPtr->code.opcode <= 13)
        fillOneOperand(lineCopy,wordPtr,variablesPtr);
    else { /* operation has no operands */
        if(strcmp(lineCopy,"")) /* if there is text left raise error */
            variablesPtr->status = TextAfterCommand;
        else {
            wordPtr->code.destReg = 0;
            wordPtr->code.destAdd = 0;
            wordPtr->code.srcReg = 0;
            wordPtr->code.srcAdd = 0;
            addWordToImage(&variablesPtr->codeHptr,*wordPtr,variablesPtr->IC);
            variablesPtr->IC++;
            variablesPtr->status = Valid;
        }
    }

    lineCopy = temp;
    free(lineCopy);
}

void fillTwoOperands(char *str, Word *word, variables *variablesPtr)
{
	char arr[STRING_PARTS][LINE_LEN];
	int op1,op2;
	split(str,",",arr);

	strcpy(arr[IMPORTANT],strip(arr[IMPORTANT]));
	strcpy(arr[REST],strip(arr[REST]));
	if(!strcmp(arr[IMPORTANT],"") || !strcmp(arr[REST],""))
		variablesPtr->status = MissingOperand;
	op1 = findAddressMethod(variablesPtr,arr[IMPORTANT]);
	op2 = findAddressMethod(variablesPtr,arr[REST]);

    if(op1 == -1 || op2 == -1) {
        variablesPtr->status = InvalidOperand;
        return;
    }

	if(word->code.opcode < 3)
	{
		if(op1 == 2 || op2 == 2) {
            variablesPtr->status = InvalidOperand;
			return;
        }
        if(word->code.opcode % 2 == 0 && op2 == 0) {
			variablesPtr->status = InvalidOperand;
            return;
        }
	}
	else
	{
		if(op1 != 1 || op2 % 2 ==0) {
			variablesPtr->status = InvalidOperand;
            return;
        }
	}

    word->code.ARE = A;
    word->code.srcAdd = op1;
    word->code.destAdd = op2;
    word->code.srcReg = op1 == 3 ? findReg(arr[IMPORTANT]) : 0;
    word->code.destReg = op2 == 3 ? findReg(arr[REST]) : 0;
    addWordToImage(&variablesPtr->codeHptr,*word,variablesPtr->IC);
    variablesPtr->IC++;

    if(op1 == 0) {
        addNumberWord(variablesPtr,arr[IMPORTANT]);
        variablesPtr->IC++;
    }
    else if(op1 == 1) {
        addEmptyWord(variablesPtr);
        variablesPtr->IC++;
    }
    
    if(op2 == 0) {
        addNumberWord(variablesPtr,arr[REST]);
        variablesPtr->IC++;
    }
    else if(op2 == 1){
        addEmptyWord(variablesPtr);
		variablesPtr->IC++;
    }
    variablesPtr->status = Valid;
}

void fillOneOperand(char *str,Word *word, variables *variablesPtr)
{
	int op;
    word->code.srcAdd=0;
    word->code.srcReg=0;
	strcpy(str, strip(str));

	if(!strcmp(str,"")) {
		variablesPtr->status = MissingOperand;
        return;
    }
	op = findAddressMethod(variablesPtr,str);

	if((word->code.opcode == 5 || word->code.opcode ==12) && op % 2 == 0)
		variablesPtr->status = InvalidOperand;
	if(word->code.opcode == 9 && op % 3 == 0)
		variablesPtr->status = InvalidOperand;
	if(word->code.opcode == 13 && op == 2)
		variablesPtr->status = InvalidOperand;

    if(variablesPtr->status != Valid)
        return;
    
    word->code.ARE = A;
    word->code.destAdd = op;
    word->code.destReg = op == 3 ? findReg(str) : 0;

    addWordToImage(&variablesPtr->codeHptr,*word,variablesPtr->IC);

	variablesPtr->IC++;

	if(op == 0) {
        addNumberWord(variablesPtr,str);
        variablesPtr->IC++;
    }
    else if(op != 3) {
        addEmptyWord(variablesPtr);
        variablesPtr->IC++;
    }

    variablesPtr->status = Valid;
}

void handleDirective(variables *variablesPtr, Word *wordPtr) {
    char lineCopy[LINE_LEN];
	Type type;
    DataOrString varType;
    Bool hasSymbol;
	char arr[STRING_PARTS][LINE_LEN];
    strcpy(lineCopy,variablesPtr->line);
    
    strcpy(variablesPtr->symbol,findSymbol(lineCopy));
    hasSymbol = !strcmp(variablesPtr->symbol,"") ? False : True;
    if(hasSymbol) {
        split(lineCopy," \t",arr);
        strcpy(lineCopy,strip(arr[REST]));
    }
    
    varType = findDataOrString(lineCopy);
    type = findEntryOrExternal(lineCopy);
    split(lineCopy," \t",arr);
    strcpy(arr[REST],strip(arr[REST]));

    if(type == NoneEntOrExt) {
        if(varType == NoneDataOrStr) {
            variablesPtr->status = InvalidDirectiveCommand;
            return;
        }
        if(hasSymbol) {
            checkSyntaxValidLabel(variablesPtr,variablesPtr->symbol,True);
            if(variablesPtr->status != Valid)
                return;
            
            checkDirectiveLabel(variablesPtr,variablesPtr->symbol,NoneEntOrExt);
            if(variablesPtr->status != Valid)
                return;

            addSymbol(variablesPtr,DataImage);
        }
        
        if(varType == StringVar) {
            int ind;
            int i;
            ind = findFromEnd(arr[REST], '\"');
            
            if(arr[REST][0] != '\"')
                variablesPtr->status = InvalidOperand;
            if(ind == strlen(arr[REST]))
                variablesPtr->status = NoClosingQuotes;
            if(ind != 1)
                variablesPtr->status = ExtraneousText;
            if(variablesPtr->status != Valid)
                return;

            for(i=1;i<strlen(arr[REST])-1;i++) {
                addStringWord(variablesPtr,arr[REST][i]);
                variablesPtr->DC++;
            }
            addStringWord(variablesPtr,'\0');
            variablesPtr->DC++;
            variablesPtr->status = Valid;
        }

        if(varType == DataVar) {
            char strCopy[LINE_LEN];
            long num;
            strcpy(strCopy,arr[REST]);
            while(strcmp(strCopy,""))
            {
                Word w;
                split(strCopy,",",arr);
                strcpy(arr[IMPORTANT],strip(arr[IMPORTANT]));
                if(checkNum(arr[IMPORTANT]) != Valid) {
                    variablesPtr->status = InvalidOperand;
                    return;
                }

                num = strtol(arr[IMPORTANT],NULL,10);
                if(num > 8388607 || num < -8388608) {
                    variablesPtr->status = InvalidOperand;
                    return;
                }

                strcpy(strCopy,strip(arr[REST]));
                w.index = num;
                addWordToImage(&variablesPtr->dataHptr,w,variablesPtr->DC);
                variablesPtr->DC++;
            }
            variablesPtr->status = Valid;
        }
    }
    else if(type == Entry) {
        checkSyntaxValidLabel(variablesPtr,arr[REST],False);
        return;
    }
    else {
        symbolTableNode node;
        checkSyntaxValidLabel(variablesPtr,arr[REST],False);
        if(variablesPtr->status != Valid)
            return;

        checkDirectiveLabel(variablesPtr,arr[REST],External);
        if(variablesPtr->status != Valid)
            return;
        strcpy(node.symbol,arr[REST]);
        node.address = 0;
        node.location = DataImage;
        node.type = External;
        addToList(&variablesPtr->symbolHptr,node);
    }
    variablesPtr->status = Valid;
}

/* the the symbol from the variables ptr with the location loc to the list */
void addSymbol(variables *variablesPtr, Location loc) {
    symbolTableNode newSymbol;
    strcpy(newSymbol.symbol,variablesPtr->symbol);
    newSymbol.type = NoneEntOrExt;
    if(loc == CodeImage) {
        newSymbol.address = variablesPtr->IC;
        newSymbol.location = CodeImage;
    }
    else {
        newSymbol.address = variablesPtr->DC;
        newSymbol.location = DataImage;
    }
    addToList(&variablesPtr->symbolHptr,newSymbol);
}

int findAddressMethod(variables *variablesPtr, char *str) {
    char *ptr;
    if(*str == '#')
    {
    	if(checkNum(++str) == Valid)
    	{
            long num = strtol(str,&ptr,10);
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
        if (!isspace(variablesPtr->line[strlen(str)+1]))
            variablesPtr->status = MissingWhitespace;
    }
    
    for(i = 1; i < strlen(str); i++) { /*check that there are only letters and numbers in the label*/
		if(!isalnum(str[i])) /* is alpha numeric */
			variablesPtr->status = LabelInvalidCharacters;
	}
}

void checkDirectiveLabel(variables *variablesPtr,char *symbol,Type type) {
    if(symbolInList(variablesPtr->symbolHptr,symbol)) {
        Type symbolType;
        symbolType = getSymbolType(variablesPtr->symbolHptr, symbol);
        if(symbolType == NoneEntOrExt) {
            if(type != Entry) {
                variablesPtr->status = InvalidLabel;
                return;
            }
        }

        else if(symbolType == External){
            if(type == NoneEntOrExt) {
                variablesPtr->status = SymbolDefinedAndExtern;
                return;
            }
            if(type == Entry) {
                variablesPtr->status = SymbolEntryAndExtern;
                return;
            }
        }
    }
    variablesPtr->status = Valid;
}

void addNumberWord(variables *variablesPtr, char *str) {
    Word w;
    long num;
    str += *str == '#' ? 1 : 0;
    num = strtol(str,NULL,10);
    if(num < 0)
        w.index = ((long)pow(2,21) + num);
    else
        w.index = num;
    w.index <<= 3;
    w.index |= A;
    addWordToImage(&variablesPtr->codeHptr,w,variablesPtr->IC);
}

void addStringWord(variables *variablesPtr, char ch) {
    Word w;
    w.index = (int) ch;
    addWordToImage(&variablesPtr->dataHptr,w,variablesPtr->DC);
}

void updateTables(variables *variablesPtr) {
    symbolTableNodePtr symbolHptr = variablesPtr->symbolHptr;
    wordNodePtr wordHptr = variablesPtr->dataHptr;
    while(symbolHptr) {
        if(symbolHptr->location == DataImage && symbolHptr->type != External) {
            symbolHptr->address += variablesPtr->IC;
        }
        symbolHptr = symbolHptr->next;
    }

    while(wordHptr) {
        wordHptr->address+=variablesPtr->IC;
        wordHptr = wordHptr->next;
    }
}

void defaultValues(variables *variablesPtr) {
    variablesPtr->status = Valid;
    strcpy(variablesPtr->symbol,"");
}

void addEmptyWord(variables *variablesPtr) {
    Word w;
    w.index = 0;
    addWordToImage(&variablesPtr->codeHptr,w,variablesPtr->IC);
}

void printListProp(variables *variablesPtr) {
    wordNodePtr data = variablesPtr->dataHptr;
    wordNodePtr code = variablesPtr->codeHptr;
    while(code) {
        code = code->next;
    }
    while(data) {
        data = data->next;
    }

}