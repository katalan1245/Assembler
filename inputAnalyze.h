#ifndef INPUT_ANALYZE_H
#define INPUT_ANALYZE_H

#define IMPORTANT 0
#define REST 1
#define STRING_PARTS 2
#define DELIM_EXIST 0
#define DELIM_NOT_EXIST 1

Statement getLine(FILE *,char **str);
Statement firstCheck(char*);
char *strip(char*);
int split(char*,char*,char[STRING_PARTS][LINE_LEN]);

int findOpcode(char*);
int findReg(char*);
int findFunct(char*);
int strInArray(char*,char[STRING_PARTS][LINE_LEN]);
char *findSymbol(char*);

#endif