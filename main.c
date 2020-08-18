#include "defaults.h"
#include "symbolTableLinkedList.h"
#include "firstPass.h"
#include "secondPass.h"

int main(int argc, char *argv[]) {
    int i;
    struct variables *variablesPtr;
       
    for(i=1;i<argc;i++) {
        char filename[FILE_LEN+EXTENSION_LEN];
        strcpy(filename,strcat(argv[i],".as"));

        strcpy(variablesPtr->filename,filename);
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

        strcpy(variablesPtr->filename,filename);
        variablesPtr->IC=100;
        variablesPtr->DC=0;
        variablesPtr->foundError=False;
        variablesPtr->lineCounter=0;

        firstPass(variablesPtr);
        if(variablesPtr->foundError)
            continue;
        secondPass(variablesPtr);
        
        fclose(variablesPtr->file);
    }

    mini_main();
    return 0;
}
