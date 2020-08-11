#include "defaults.h"

int main(int argc, char *argv[]) {
    IC = 100;
    DC = 0;
    lineCounter = 0;
    Status status;
    foundError = False;
    int i;
    strcpy(whitespace," \t\n\v\f\r");

    for(i=1;i<argc;i++) {
        FILE *f = fopen(argv[i],"r");
        lineCounter = 0;

        if(!f) {
            fprintf(stderr, "%s: Cannot open file!",argv[i]);
            continue;
        }

        /* check if file is empty */
        fseek(f,0,SEEK_END);
        if(ftell(f) == 0)
            continue;
        rewind(f);

        while(!feof(f)) {
            status = mini_main(f);

        }

        if(status != Valid)
            printError();
    }

    return 0;
}
