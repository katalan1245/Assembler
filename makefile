assebmly: dataImage.o  firstPass.o inputAnalyze.o main.o printFunctions.o secondPass.o symbolTableLinkedList.o
	gcc -g dataImage.o firstPass.o inputAnalyze.o main.o printFunctions.o secondPass.o symbolTableLinkedList.o -o assebmly -lm

dataImage.o: dataImage.c dataImage.h defaults.h
	gcc -c dataImage.c -Wall -pedantic -ansi -o dataImage.o

firstPass.o: firstPass.c firstPass.h defaults.h inputAnalyze.h symbolTableLinkedList.h dataImage.h printFunctions.h
	gcc -c firstPass.c -Wall -pedantic -ansi -o firstPass.o

inputAnalyze.o: inputAnalyze.c inputAnalyze.h defaults.h
	gcc -c inputAnalyze.c -Wall -pedantic -ansi -o inputAnalyze.o

main.o: main.c symbolTableLinkedList.h firstPass.h secondPass.h
	gcc -c main.c -Wall -pedantic -ansi -o main.o

printFunctions.o: printFunctions.c printFunctions.h defaults.h symbolTableLinkedList.h dataImage.h
	gcc -c printFunctions.c -Wall -pedantic -ansi -o printFunctions.o

secondPass.o: secondPass.c secondPass.h defaults.h firstPass.h inputAnalyze.h symbolTableLinkedList.h printFunctions.h dataImage.h
	gcc -c secondPass.c -Wall -pedantic -ansi -o secondPass.o

symbolTableLinkedList.o: symbolTableLinkedList.c symbolTableLinkedList.h defaults.h
	gcc -c main.c -Wall -pedantic -ansi -o symbolTableLinkedList.o
