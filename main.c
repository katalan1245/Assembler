#include "defaults.h"

int main(int argc, char *argv[]) {
    /*Status status;*/
    /*int i;*/
    
    Bool foundError = False;
    Bool IC = 100;
    Bool DC = 0;
    Bool lineCounter = 0;
    strcpy(whitespace," \t\n\v\f\r");

    printf("%d,%d,%d,%d",foundError,IC,DC,lineCounter);
    
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
