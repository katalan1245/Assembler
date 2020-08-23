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
    back = findLabel(lineCopy);
    strcpy(variablesPtr->label,back);
    free(back);
    if(strcmp(variablesPtr->label,"")) {
        checkSyntaxValidLabel(variablesPtr,variablesPtr->label,True);
        if(variablesPtr->status != Valid)
            return;

        checkLabel(variablesPtr,variablesPtr->label,NoneEntOrExt);
        if(variablesPtr->status != Valid)
            return;

        addLabel(variablesPtr, CodeImage);
        lineCopy+=strlen(variablesPtr->label)+1;
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
            variablesPtr->status = Invalid2AdressMethod;
			return;
        }
        if(word->code.opcode % 2 == 0 && op2 == 0) {
			variablesPtr->status = Invalid0AdressMethod;
            return;
        }
	}
	else /* lea command */
	{
		if(op1 != 1 || op2 % 2 ==0) {
			variablesPtr->status = InvalidLeaOperands;
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

	if((word->code.opcode == 5 || word->code.opcode ==12) && op % 2 == 0) /* clr,inc,dec,not,red, can 1,3*/
		variablesPtr->status = InvalidOperand5_12;
	else if(word->code.opcode == 9 && op % 3 == 0) /* brnaching commands, not labels */
		variablesPtr->status = InvalidOperand9;
	else if(word->code.opcode == 13 && op == 2) /* prn, address 2 method */
		variablesPtr->status = Invalid2AdressMethod;

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
    Bool hasLabel;
	char arr[STRING_PARTS][LINE_LEN];
	char labelTemp[LINE_LEN];
    strcpy(lineCopy,variablesPtr->line);
    
    strcpy(labelTemp,findLabel(lineCopy));
    hasLabel = !strcmp(labelTemp,"") ? False : True;
    if(hasLabel) {
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
        if(hasLabel) {
            checkSyntaxValidLabel(variablesPtr,labelTemp,True);
            if(variablesPtr->status != Valid)
                return;
            strcpy(variablesPtr->label,labelTemp);
            checkLabel(variablesPtr,variablesPtr->label,NoneEntOrExt);
            if(variablesPtr->status != Valid)
                return;

            addLabel(variablesPtr,DataImage);
        }
        
        if(varType == StringVar) {
            int ind;
            int i;
            ind = findFromEnd(arr[REST], '\"');
            
            if(arr[REST][0] != '\"')
                variablesPtr->status = NoOpeningQuotes;
            else if(ind == strlen(arr[REST]))
                variablesPtr->status = NoClosingQuotes;
            else if(ind != 1)
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
                int tokExist;
                tokExist = split(strCopy,",",arr);
                strcpy(arr[IMPORTANT],strip(arr[IMPORTANT]));
                strcpy(arr[REST],strip(arr[REST]));

                if(!strcmp(arr[REST],"") && tokExist == DELIM_EXIST) {
                    variablesPtr->status = ExtraComma;
                    return;
                }

                if(!strcmp(arr[IMPORTANT],"")) {
                	variablesPtr->status = MissingOperand;
                    return;
                }
                if(checkNum(arr[IMPORTANT]) != Valid) {
                    variablesPtr->status = InvalidNumber;
                    return;
                } 

                num = strtol(arr[IMPORTANT],NULL,10);
                if(num > MAX_24_SIGNED || num < MIN_24_SIGNED) {
                    variablesPtr->status = NumOutOfMemory;
                    return;
                }
                if(num < 0)
                	num = pow(2,24) + num;

                strcpy(strCopy,arr[REST]);
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
        labelTableNode node;
        checkSyntaxValidLabel(variablesPtr,arr[REST],False);
        if(variablesPtr->status != Valid)
            return;

        checkLabel(variablesPtr,arr[REST],External);
        if(variablesPtr->status != Valid)
            return;
        strcpy(node.label,arr[REST]);
        node.address = 0;
        node.location = DataImage;
        node.type = External;
        addToList(&variablesPtr->labelHptr,node);
    }
    variablesPtr->status = Valid;
}

/* the the label from the variables ptr with the location loc to the list */
void addLabel(variables *variablesPtr, Location loc) {
    labelTableNode newLabel;
    strcpy(newLabel.label,variablesPtr->label);
    newLabel.type = NoneEntOrExt;
    if(loc == CodeImage) {
        newLabel.address = variablesPtr->IC;
        newLabel.location = CodeImage;
    }
    else {
        newLabel.address = variablesPtr->DC;
        newLabel.location = DataImage;
    }
    addToList(&variablesPtr->labelHptr,newLabel);
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
            if(num > MAX_21_SIGNED || num < MIN_21_SIGNED)
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

void checkLabel(variables *variablesPtr,char *label,Type type) {
    if(labelInList(variablesPtr->labelHptr,label)) {
        Type labelType;
        labelType = getLabelType(variablesPtr->labelHptr, label);
        if(labelType == NoneEntOrExt) {
            if(type == NoneEntOrExt)
                variablesPtr->status = LabelAlreadyExist;
            else if(type == External)
                variablesPtr->status = LabelDefinedAndExtern;
            if(variablesPtr->status != Valid)
                return;
        }

        else if(labelType == External){
            if(type == NoneEntOrExt) {
                variablesPtr->status = LabelDefinedAndExtern;
                return;
            }
            if(type == Entry) {
                variablesPtr->status = LabelEntryAndExtern;
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
    labelTableNodePtr labelHptr = variablesPtr->labelHptr;
    wordNodePtr wordHptr = variablesPtr->dataHptr;
    while(labelHptr) {
        if(labelHptr->location == DataImage && labelHptr->type != External) {
            labelHptr->address += variablesPtr->IC;
        }
        labelHptr = labelHptr->next;
    }

    while(wordHptr) {
        wordHptr->address+=variablesPtr->IC;
        wordHptr = wordHptr->next;
    }
}

void defaultValues(variables *variablesPtr) {
    variablesPtr->status = Valid;
    strcpy(variablesPtr->label,"");
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