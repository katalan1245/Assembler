cmake_minimum_required(VERSION 3.10.2)
project(Assembler C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(Assembler
        main.c
        dataImage.c
        dataImage.h
        defaults.h
        firstPass.c
        firstPass.h
        inputAnalyze.c
        inputAnalyze.h
        printFunctions.c
        printFunctions.h
        secondPass.c
        secondPass.h
        symbolTableLinkedList.c
        symbolTableLinkedList.h)
