// one direction linked list for the symbol table

typedef enum {code, data} Location;
typedef enum {entry, external} Type;

typedef struct node *symbolTableNodePtr;
typedef struct node {
    char *symbol;
    int value;
    Location location;
    Type type;
    symbolTableNodePtr next;
} symbolTableNode;

void addToList(symbolTableNodePtr *, symbolTableNode);
void freeList(symbolTableNodePtr *);
void removeFirstNode(symbolTableNodePtr *);
void printList(symbolTableNodePtr);