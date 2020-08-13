assebmly: inputAnalyze.o main.o symbolTableLinkedList.o
	gcc -g inputAnalyze.o main.o symbolTableLinkedList.o -o assebmly -lm

inputAnalyze.o: inputAnalyze.c inputAnalyze.h defaults.h
	gcc -c inputAnalyze.c -Wall -pedantic -ansi -o inputAnalyze.o

main.o: main.c defaults.h
	gcc -c main.c -Wall -pedantic -ansi -o main.o

symbolTableLinkedList.o: symbolTableLinkedList.c symbolTableLinkedList.h defaults.h
	gcc -c main.c -Wall -pedantic -ansi -o symbolTableLinkedList.o