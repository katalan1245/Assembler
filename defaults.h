#ifndef DEFAULTS_H
#define DEFAULTS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define LINE_LEN 82
#define LABEL_LEN 31
#define WORD_LEN 24

#define OPCODE_LEN 6
#define SRC_ADD_LEN 2
#define SRC_REG_LEN 3
#define DEST_ADD_LEN 2
#define DEST_REG_LEN 3
#define FUNCT_LEN 5
#define ARE_FIELD_LEN 1

#define STARTING_IC 100

typedef enum {False, True} Bool;

/*
typedef struct {
    union {
        struct {
            unsigned int E:ARE_FIELD_LEN;
            unsigned int R:ARE_FIELD_LEN;
            unsigned int A:ARE_FIELD_LEN;
            unsigned int funct:FUNCT_LEN;
            unsigned int destReg:DEST_REG_LEN;
            unsigned int destAdd:DEST_ADD_LEN;
            unsigned int srcReg:SRC_REG_LEN;
            unsigned int srcAdd:SRC_ADD_LEN;
            unsigned int opcode:OPCODE_LEN;
        } code;
        struct {
            long address:WORD_LEN;
            Bool useARE;
        } wordData;
    } data;
    enum {Code, wordData} type;
} Word;
*/


typedef union {
    struct {
        unsigned long index:24;
        Bool userARE;
    } data;
    struct {
        unsigned int E:ARE_FIELD_LEN;
        unsigned int R:ARE_FIELD_LEN;
        unsigned int A:ARE_FIELD_LEN;
        unsigned int funct:FUNCT_LEN;
        unsigned int destReg:DEST_REG_LEN;
        unsigned int destAdd:DEST_ADD_LEN;
        unsigned int srcReg:SRC_REG_LEN;
        unsigned int srcAdd:SRC_ADD_LEN;
        unsigned int opcode:OPCODE_LEN;
    } code;
} Word;

extern int IC;
extern int DC;
extern int lineCounter;
extern Bool foundError; 
extern char whitespace[7];

typedef enum {A=4,R=2,E=1} ARE;
typedef enum {Invalid, Empty, Comment, Directive, Instruction} Statement;
typedef enum {LineTooLong=0, UnknownOperation=1, SymbolAlreadyExist=2,
              NeedlessOperands=3, MissingOperand=4, InvalidOperand=5, Valid=100} Status;




#endif