#include "defaults.h"
#include "inputAnalyze.h"

Status handleInstruction(char *str) {
    Word word;
    int bit;
    char arr[STRING_PARTS][LINE_LEN];

    // ADD THE LABEL -- THIS IS AFTER THE LABEL HAS READ

    str = strip(str);
    split(str,whitespace,arr);
    bit = findOpcode(arr[IMPORTANT]);
    if(bit == -1) {
        foundError = False;
        return UnknownOperation;
    }
    word.bits.opcode = bit;
    word.bits.funct = findFunct(arr[IMPORTANT]);

    // NEED TO ADD HERE MIUN & REGISTERS
}