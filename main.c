#include "defaults.h"
#include "symbolTableLinkedList.h"

Bool foundError = False;
int IC = 100;
int DC = 0;
int lineCounter = 0;
symbolTableNodePtr hptr;


int main(int argc, char *argv[]) {
    /*Status status;*/
    /*int i;*/
    
   

    printf("%d,%d,%d,%d\n",foundError,IC,DC,lineCounter);
    
/*
    for(i=1;i<argc;i++) {
        FILE *f = fopen(argv[i],"r");
        lineCounter = 0;

        if(!f) {
            fprintf(stderr, "%s: Cannot open file!",argv[i]);
            continue;
        }

         check if file is empty END OF COMMENT
        fseek(f,0,SEEK_END);
        if(ftell(f) == 0)
            continue;
        rewind(f);

        while(!feof(f)) {
            status = mini_main(f);

        }

        if(status != Valid)
            printError();
        
        fclose(f);
    } */

    mini_main();
    return 0;
}
