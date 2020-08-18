#ifndef DEFAULTS_H
#define DEFAULTS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define FILE_NAME_LEN 100
#define EXTENSION_LEN 3
#define LINE_LEN 82
#define LABEL_LEN 31
#define WORD_LEN 24

#define OPCODE_LEN 6
#define SRC_ADD_LEN 2
#define SRC_REG_LEN 3
#define DEST_ADD_LEN 2
#define DEST_REG_LEN 3
#define FUNCT_LEN 5
#define ARE_FIELD_LEN 3

#define STARTING_IC 100

typedef enum {False, True} Bool;

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

struct variables{
        int IC;
        int DC;
        int lineCounter;
        char filename[FILE_LEN];
        FILE *file;
        char line[LINE_LEN];
        char symbol[LABEL_LEN];
        Bool foundError;
        Status status;
        symbolTableNodePtr symbolHptr;
        wordNodePtr dataHptr;
        wordNodePtr codeHptr;
};

typedef enum {Entry, External, None} Type;
typedef enum {Data, String, None} DataOrString;
typedef enum {A=4,R=2,E=1} ARE;
typedef enum {Invalid, Empty, Comment, Directive, Instruction} Statement;
typedef enum {LineTooLong=0, UnknownOperation=1, SymbolAlreadyExist=2,
              TextAfterCommand=3, MissingOperand=4, InvalidOperand=5,
              InvalidLabel=6, InvalidNumber=7, MultipleDirectives=8,
              ReservedLabelName=9, LabelTooLong=10, LabelInvalidStart=11,
              LabelInvalidCharacters=12, MissingWhitespace=13,
              SymbolEntryAndExtern=14, SymbolDefinedAndExtern=15,
              Valid=100,Error=-1} Status;


#endif