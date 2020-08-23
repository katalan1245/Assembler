#include "symbolTableLinkedList.h"
#include "firstPass.h"
#include "secondPass.h"

int main(int argc, char *argv[]) {
    int i;
    for(i=1;i<argc;i++) {
        variables *variablesPtr;
        char filename[FILE_NAME_LEN + AS_OB_EXTENSION_LEN];
        variablesPtr = (variables*) malloc(sizeof(variables));
        sprintf(filename, "%s.as", argv[i]);

        if(i==1)
            sprintf(filename,"/home/raz/Desktop/Maman14/example.as");
        if(i==2)
            sprintf(filename,"/home/raz/Desktop/Maman14/valid.as");

        strcpy(variablesPtr->filename, argv[i]);
        /*strcpy(variablesPtr->filename,filename);*/

        variablesPtr->file = fopen(filename, "r");

        if (!variablesPtr->file) {
            printf("%s: Cannot open file!\n", filename);
            continue;
        }

        /* check if file is empty */
        fseek(variablesPtr->file, 0, SEEK_END);
        if (ftell(variablesPtr->file) == 0)
            continue;
        rewind(variablesPtr->file);

        variablesPtr->IC = 100;
        variablesPtr->DC = 0;
        variablesPtr->foundError = False;
        variablesPtr->lineCounter = 0;

        firstPass(variablesPtr);
        if (variablesPtr->foundError) {
            fclose(variablesPtr->file);
            continue;
        }
        printf("First Pass: Passed!\n");
        rewind(variablesPtr->file);
        secondPass(variablesPtr);
        if (variablesPtr->foundError) {
            fclose(variablesPtr->file);
            continue;
        }
        printf("Second Pass: Passed!\n");
        fclose(variablesPtr->file);
        createOutput(variablesPtr);
        printf("Output created!\n");
        freeSymbolList(&variablesPtr->symbolHptr);
        freeList(&variablesPtr->codeHptr);
        freeList(&variablesPtr->dataHptr);
        free(variablesPtr);
    }
    return 0;
}
