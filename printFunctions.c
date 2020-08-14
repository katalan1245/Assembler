#include "defaults.h"

void printError(Status status, char *filename) {
    switch (status) {
        case LineTooLong:
            printf("%s,%d: Line too long.\n",filename,lineCounter);
            break;
        case Valid:
            break;
    }
}