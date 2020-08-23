#ifndef DEFAULTS_H
#define DEFAULTS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define FILE_NAME_LEN 100
#define AS_OB_EXTENSION_LEN 3
#define EXT_ENT_EXTENSION_LEN 4
#define LINE_LEN 82
#define LABEL_LEN 31
#define WORD_LEN 24
#define MAX_24_SIGNED 8388607
#define MIN_24_SIGNED -8388608
#define MAX_24_UNSIGNED 16777215
#define MAX_21_SIGNED 1048575
#define MIN_21_SIGNED -1048576
#define MAX_21_UNSIGNED 2097151

#define OPCODE_LEN 6
#define SRC_ADD_LEN 2
#define SRC_REG_LEN 3
#define DEST_ADD_LEN 2
#define DEST_REG_LEN 3
#define FUNCT_LEN 5
#define ARE_FIELD_LEN 3

#define STARTING_IC 100

typedef enum {False, True} Bool;
typedef enum {CodeImage, DataImage} Location;
typedef enum {Entry, External, NoneEntOrExt} Type;
typedef enum {DataVar, StringVar, NoneDataOrStr} DataOrString;
typedef enum {A=4,R=2,E=1} ARE;
typedef enum {Invalid, Empty, Comment, Directive, Instruction} Statement;
typedef enum {LineTooLong=0, UnknownOperation=1, LabelAlreadyExist=2,
              TextAfterCommand=3, MissingOperand=4, InvalidOperand=5,
              InvalidLabel=6, InvalidNumber=7, MultipleDirectives=8,
              ReservedLabelName=9, LabelTooLong=10, LabelInvalidStart=11,
              LabelInvalidCharacters=12, MissingWhitespace=13,
              LabelEntryAndExtern=14, LabelDefinedAndExtern=15,
              MissingLabel=16, InvalidDirectiveCommand=17,
              NoClosingQuotes=18, ExtraneousText=19, ExternalBranching=20,
              NoOpeningQuotes=21, ExtraComma=22, NumOutOfMemory=23,
              Invalid2AdressMethod=24, Invalid0AdressMethod=25,
              InvalidLeaOperands=26, InvalidOperand5_12=27,
              InvalidOperand9=28, 
              Valid=100} Status;

typedef struct node *symbolTableNodePtr;
typedef struct node {
    char symbol[LABEL_LEN];
    unsigned long address;
    Location location;
    Type type;
    symbolTableNodePtr next;
} symbolTableNode;

typedef union {
    unsigned long index;
    struct {
        unsigned int ARE:ARE_FIELD_LEN;
        unsigned int funct:FUNCT_LEN;
        unsigned int destReg:DEST_REG_LEN;
        unsigned int destAdd:DEST_ADD_LEN;
        unsigned int srcReg:SRC_REG_LEN;
        unsigned int srcAdd:SRC_ADD_LEN;
        unsigned int opcode:OPCODE_LEN;
    } code;
} Word;

typedef struct wordnode *wordNodePtr;
typedef struct wordnode {
    char externSymbol[LABEL_LEN];
    Word word;
    unsigned long address;
    wordNodePtr next;
} wordNode;

typedef struct {
        int IC;
        int DC;
        int lineCounter;
        char    filename[FILE_NAME_LEN];
        FILE *file;
        char line[LINE_LEN];
        char symbol[LABEL_LEN];
        Bool foundError;
        Status status;
        symbolTableNodePtr symbolHptr;
        wordNodePtr dataHptr;
        wordNodePtr codeHptr;
} variables;


#endif