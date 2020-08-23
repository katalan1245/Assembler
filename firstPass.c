#include "firstPass.h"
#include "defaults.h"

void handleString(char*,variables*);
void handleData(char*,variables*);
void handleInstruction(variables*,Word*);
void fillTwoOperands(char*,Word*,variables*);
void fillOneOperand(char*,Word*,variables*);
void handleDirective(variables*,Word*);
void addLabel(variables*,Location);
void checkLabel(variables*,char*,Type);
void addNumberWord(variables*,char*);
void addStringWord(variables*,char);
void updateTables(variables*);
void addEmptyWord(variables*);

/* Handle the first pass of the assembler */
void firstPass(variables *variablesPtr) {
    Statement state;
    Word word;
    variablesPtr->lineCounter=0;

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

    if(!variablesPtr->foundError)
        updateTables(variablesPtr);
}

/* Handle the instruction statement
 * Fill the word, update the linked list and do anything it needs
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
    if(strlen(variablesPtr->label) == strlen(variablesPtr->line)-1) {
        variablesPtr->status = MissingOperation;
        return;
    }
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

    if(variablesPtr->status != Valid)
        return;

    /* find the opcode and funct */
    wordPtr->code.opcode = findOpcode(lineCopy);
    wordPtr->code.funct = findFunct(lineCopy);

    if(wordPtr->code.opcode == MINUS1_6_BITS) {
        variablesPtr->status = UnknownOperation;
        return;
    }

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

/* this function handles the instruction commands with 2 operands and fills the word if there are no errors */
void fillTwoOperands(char *str, Word *word, variables *variablesPtr)
{
	char arr[STRING_PARTS][LINE_LEN];
	char checkExtraOperand[STRING_PARTS][LINE_LEN];
	int op1,op2;
	int tokExist;
    memset(arr,'\0',sizeof(arr[0][0])*STRING_PARTS*LINE_LEN);
    memset(checkExtraOperand,'\0',sizeof(checkExtraOperand[0][0])*STRING_PARTS*LINE_LEN);

	tokExist = split(str,",",arr); /* split the two operands */
    strcpy(arr[IMPORTANT],strip(arr[IMPORTANT]));
    strcpy(arr[REST],strip(arr[REST]));

    if(tokExist == DELIM_NOT_EXIST) { /* if there is no comma */
        if(!strcmp(arr[IMPORTANT],"")) /* if the part before the comma empty - no operands */
            variablesPtr->status = NoOperands;
        else /* there is only one operand */
            variablesPtr->status = MissingOperand;
    }
    else { /* if there is a comma */
        if(!strcmp(arr[IMPORTANT],"") && !strcmp(arr[REST],"")) /* before and after the comma empty - no operands*/
            variablesPtr->status = NoOperands;
        else if(!strcmp(arr[IMPORTANT],"") || !strcmp(arr[REST],"")) /* before or after the comma empty - no operands */
            variablesPtr->status = MissingOperand;
    }

    if(split(arr[REST],",",checkExtraOperand) == DELIM_EXIST) { /* split with the next comma, if there are more */
	    if(!strcmp(checkExtraOperand[REST],"")) /* if the part after the comma is empty we have extra comma */
	        variablesPtr->status = ExtraComma;
	    else if(!strcmp(checkExtraOperand[IMPORTANT],"")) /* if the part before the comma is empty we have extra comma */
	        variablesPtr->status = ExtraComma;
	    else
            variablesPtr->status = ExtraOperand; /* we have an extra operand */
        return;
    }

	if(variablesPtr->status != Valid)
	    return;

	op1 = findAddressMethod(variablesPtr,arr[IMPORTANT]);
	op2 = findAddressMethod(variablesPtr,arr[REST]);

    if(variablesPtr->status != Valid)
        return;
    
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

/* this function handles the instruction commands with 1 operand and fills the word if there are no errors */
void fillOneOperand(char *str,Word *word, variables *variablesPtr)
{
	int op;
	char checkExtraOperand[STRING_PARTS][LINE_LEN];
    word->code.srcAdd=0;
    word->code.srcReg=0;
	strcpy(str, strip(str));

	if(!strcmp(str,"")) {
		variablesPtr->status = NoOperands;
        return;
    }

    if(split(str,",",checkExtraOperand) == DELIM_EXIST) { /* if we find a comma */
        if(!strcmp(checkExtraOperand[REST],"")) /* nothing after the comma - an extra comma */
            variablesPtr->status = ExtraComma;
        else /* we have an extra operand */
            variablesPtr->status = ExtraOperand;
        return;
    }

	op = findAddressMethod(variablesPtr,str);

    if(variablesPtr->status != Valid) {
        return;
    }

	if(op == -1) {
        variablesPtr->status = InvalidOperand;
    }

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

/* Handle the directive statement
 * Fill the word, update the linked list and do anything it needs */
void handleDirective(variables *variablesPtr, Word *wordPtr) {
    char lineCopy[LINE_LEN];
	Type type;
    DataOrString varType;
    Bool hasLabel;
	char arr[STRING_PARTS][LINE_LEN];
	char labelTemp[LINE_LEN];
    strcpy(lineCopy,variablesPtr->line);
    
    /* find a label */
    strcpy(labelTemp,findLabel(lineCopy));
    hasLabel = !strcmp(labelTemp,"") ? False : True;
    if(hasLabel) { /* flag that we found a label */
        split(lineCopy,":",arr);
        checkSyntaxValidLabel(variablesPtr,arr[IMPORTANT],False);
        if(variablesPtr->status != Valid)
            return;
        split(lineCopy, " \t", arr);
        strcpy(lineCopy, strip(arr[REST]));
    }

    /* splits the operand from the directive */
    split(lineCopy," \t",arr);
    varType = findDataOrString(arr[IMPORTANT]);
    type = findEntryOrExternal(arr[IMPORTANT]);

    strcpy(arr[REST],strip(arr[REST]));
    if(!strcmp(arr[REST],"")) { /* if there is nothing after the directive */
        variablesPtr->status = MissingOperand;
        return;
    }

    if(type == NoneEntOrExt) {
        if(varType == NoneDataOrStr) { /* an unkown '.' command */
            variablesPtr->status = InvalidDirectiveCommand;
            return;
        }
        if(hasLabel) { /* if we found a label */
            checkSyntaxValidLabel(variablesPtr,labelTemp,True);
            if(variablesPtr->status != Valid)
                return;
            strcpy(variablesPtr->label,labelTemp);
            checkLabel(variablesPtr,variablesPtr->label,NoneEntOrExt);
            if(variablesPtr->status != Valid)
                return;
            /* if everything is alright add the label do the image */
            addLabel(variablesPtr,DataImage);
        }
        
        if(varType == StringVar) {
            handleString(arr[REST],variablesPtr);
        }

        if(varType == DataVar) {
            handleData(arr[REST],variablesPtr);
        }
    }
    else if(type == Entry) {
        checkSyntaxValidLabel(variablesPtr,arr[REST],False);
        return;
    }
    else { /* type is external */
        labelTableNode node;
        checkSyntaxValidLabel(variablesPtr,arr[REST],False);
        if(variablesPtr->status != Valid)
            return;

        checkLabel(variablesPtr,arr[REST],External);
        if(variablesPtr->status != Valid)
            return;
        /* if there are no errors, we will add it to the labelHptr with address 0 */
        strcpy(node.label,arr[REST]);
        node.address = 0;
        node.location = DataImage;
        node.type = External;
        addToList(&variablesPtr->labelHptr,node);
    }

}

/* takes a string of data, and if there are no errors adds it to the data image */
void handleData(char *str, variables *variablesPtr) {
    char strCopy[LINE_LEN];
    char arr[STRING_PARTS][LINE_LEN];
    long num;
    strcpy(strCopy,str);

    while(strcmp(strCopy,"")) /* while there is data */
    {
        Word w;
        int tokExist;
        tokExist = split(strCopy,",",arr); /* if there is comma */
        strcpy(arr[IMPORTANT],strip(arr[IMPORTANT]));
        strcpy(arr[REST],strip(arr[REST]));

        if(!strcmp(arr[REST],"") && tokExist == DELIM_EXIST) { /* if there is comma but nothing after */
            variablesPtr->status = ExtraComma;
            return;
        }

        if(!strcmp(arr[IMPORTANT],"")) { /* if there is nothing before the comma */
            variablesPtr->status = MissingOperand; 
            return;
        }
        if(checkNum(arr[IMPORTANT]) != Valid) { /* if the string that represents a number is invalid number */
            variablesPtr->status = InvalidNumber;
            return;
        } 

        num = strtol(arr[IMPORTANT],NULL,10);
        if(num > MAX_24_SIGNED || num < MIN_24_SIGNED) { /* if num too big or too small */
            variablesPtr->status = NumOutOfMemory;
            return;
        }
        if(num < 0) /* if num negative, we represent it with 2's compliment method */
            num = pow(2,24) + num;

        strcpy(strCopy,arr[REST]);
        w.index = num;
        addWordToImage(&variablesPtr->dataHptr,w,variablesPtr->DC);
        variablesPtr->DC++;
    }
    variablesPtr->status = Valid;
}

/* takes a string, and if there are no errors add it to the data image */
void handleString(char *str, variables *variablesPtr) {
    int ind;
    int i;
    ind = findFromEnd(str, '\"');
    
    if(*str != '\"')
        variablesPtr->status = NoOpeningQuotes;
    else if(ind == strlen(str))
        variablesPtr->status = NoClosingQuotes;
    else if(ind != 1)
        variablesPtr->status = ExtraneousText;
    if(variablesPtr->status != Valid)
        return;

    for(i=1;i<strlen(str)-1;i++) { /* add every char of the string as a word */
        addStringWord(variablesPtr,str[i]);
        variablesPtr->DC++;
    }
    addStringWord(variablesPtr,'\0'); /* add a word of 0 to sign the end of the string*/
    variablesPtr->DC++;
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

/* takes a label and its type, and check if it is valid for use (for example to add it for the label linked list) */
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

/* takes a string that represents a number and upload it as a number to the word image (case of .data)*/
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

/* takes a string and adds each character as a word to the word image (case of .string) */
void addStringWord(variables *variablesPtr, char ch) {
    Word w;
    w.index = (int) ch;
    addWordToImage(&variablesPtr->dataHptr,w,variablesPtr->DC);
}

/* update the labelTable values of DC and the dataHptr values (adds IC for every data) */
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

/* add empty word to the codeHptr, (case of labels on the first pass) */
void addEmptyWord(variables *variablesPtr) {
    Word w;
    w.index = 0;
    addWordToImage(&variablesPtr->codeHptr,w,variablesPtr->IC);
}