Assembler: dataImage.o  firstPass.o inputAnalyze.o assembler.o printFunctions.o secondPass.o labelTableLinkedList.o
	gcc -g dataImage.o firstPass.o inputAnalyze.o assembler.o printFunctions.o secondPass.o labelTableLinkedList.o -o Assembler -lm

dataImage.o: dataImage.c dataImage.h defaults.h
	gcc -c dataImage.c -Wall -pedantic -ansi -o dataImage.o

firstPass.o: firstPass.c firstPass.h defaults.h inputAnalyze.h labelTableLinkedList.h dataImage.h printFunctions.h
	gcc -c firstPass.c -Wall -pedantic -ansi -o firstPass.o

inputAnalyze.o: inputAnalyze.c inputAnalyze.h defaults.h
	gcc -c inputAnalyze.c -Wall -pedantic -ansi -o inputAnalyze.o

assembler.o: assembler.c labelTableLinkedList.h firstPass.h secondPass.h
	gcc -c assembler.c -Wall -pedantic -ansi -o assembler.o

printFunctions.o: printFunctions.c printFunctions.h defaults.h labelTableLinkedList.h dataImage.h
	gcc -c printFunctions.c -Wall -pedantic -ansi -o printFunctions.o

secondPass.o: secondPass.c secondPass.h defaults.h firstPass.h inputAnalyze.h labelTableLinkedList.h printFunctions.h dataImage.h
	gcc -c secondPass.c -Wall -pedantic -ansi -o secondPass.o

labelTableLinkedList.o: labelTableLinkedList.c labelTableLinkedList.h defaults.h
	gcc -c labelTableLinkedList.c -Wall -pedantic -ansi -o labelTableLinkedList.o
