#include "symbolTableLinkedList.h"
#include "firstPass.h"
#include "secondPass.h"

int main(int argc, char *argv[]) {
    int i;
    variables *variablesPtr;
    variablesPtr = (variables*) malloc(sizeof(variables));
    for(i=1;i<argc;i++) {
        char filename[FILE_NAME_LEN+AS_OB_EXTENSION_LEN];
        /*char filename[1000]; */
        /*sprintf(filename,"%s.as",argv[i]); */
        sprintf(filename,"/home/raz/Desktop/Maman14/valid.as");
        strcpy(variablesPtr->filename,argv[i]);

        variablesPtr->file = fopen(filename,"r");

        if(!variablesPtr->file) {
            printf("%s: Cannot open file!",filename);
            continue;
        }

        /* check if file is empty */
        fseek(variablesPtr->file,0,SEEK_END);
        if(ftell(variablesPtr->file) == 0)
            continue;
        rewind(variablesPtr->file);

        variablesPtr->IC=100;
        variablesPtr->DC=0;
        variablesPtr->foundError=False;
        variablesPtr->lineCounter=0;

        firstPass(variablesPtr);
        if(variablesPtr->foundError) {
            fclose(variablesPtr->file);
            continue;
        }
        printf("second pass:\n");
        rewind(variablesPtr->file);
        secondPass(variablesPtr);
        if(variablesPtr->foundError) {
            fclose(variablesPtr->file);
            continue;
        }

        fclose(variablesPtr->file);
        createOutput(variablesPtr);
        printf("Output created!\n");
    }

    return 0;
}
