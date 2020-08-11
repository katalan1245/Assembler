#define IMPORTANT 0
#define REST 1
#define STRING_PARTS 2
#define EMPTY_CELL 0
#define FULL_CELL 1

Statement firstCheck(char*);
char *strip(char*);
int split(char*,char*,char[STRING_PARTS][LINE_LEN]);

int findOpcode(char*);
int findReg(char*);
int findFunct(char*);