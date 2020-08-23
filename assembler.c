#include "labelTableLinkedList.h"
#include "firstPass.h"
#include "secondPass.h"

/* raz katlan and omer haimzon 
   example is valid
   first_pass_errors invalid
   second_pass_errors invalid */
void freeVars(variables*);
int main(int argc, char *argv[]) {
    int i;
    for(i=1;i<argc;i++) {
        variables *variablesPtr;
        char filename[FILE_NAME_LEN + AS_OB_EXTENSION_LEN];
        variablesPtr = (variables*) malloc(sizeof(variables));
        sprintf(filename, "%s.as", argv[i]);

        strcpy(variablesPtr->filename, argv[i]);

        variablesPtr->file = fopen(filename, "r");


        if (!variablesPtr->file) {
            printf("%s: Cannot open file!\n", filename);
            continue;
        }

        /* check if file is empty */
        fseek(variablesPtr->file, 0, SEEK_END);
        if(ftell(variablesPtr->file) == 0)
            continue;
        rewind(variablesPtr->file);

        variablesPtr->IC = 100;
        variablesPtr->DC = 0;
        variablesPtr->foundError = False;
        variablesPtr->lineCounter = 0;
        variablesPtr->codeHptr = NULL;
        variablesPtr->dataHptr = NULL;
        variablesPtr->labelHptr = NULL;

        firstPass(variablesPtr);
        if (variablesPtr->foundError) {
            printf("\n");
            fclose(variablesPtr->file);
            freeVars(variablesPtr);
            continue;
        }
        printf("First Pass: Passed!\n");
        rewind(variablesPtr->file);
        secondPass(variablesPtr);
        if (variablesPtr->foundError) {
            printf("\n");
            fclose(variablesPtr->file);
            freeVars(variablesPtr);
            continue;
        }
        printf("Second Pass: Passed!\n");
        fclose(variablesPtr->file);
        createOutput(variablesPtr);
        printf("Output created!\n");
        freeVars(variablesPtr);
        printf("\n");
    }
    return 0;
}

/* free the variables */
void freeVars(variables *variablesPtr) {
    freeLabelList(&variablesPtr->labelHptr);
    freeList(&variablesPtr->codeHptr);
    freeList(&variablesPtr->dataHptr);
    free(variablesPtr);
}

/* set the default values of variables */
void defaultValues(variables *variablesPtr) {
    variablesPtr->status = Valid;
    strcpy(variablesPtr->label,"");
}