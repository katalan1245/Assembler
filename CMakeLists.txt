cmake_minimum_required(VERSION 3.15)
project(Maman14 C)

set(CMAKE_C_STANDARD 90)

add_executable(Maman14 main.c inputAnalyze.c inputAnalyze.h symbolTableLinkedList.c symbolTableLinkedList.h defaults.h printFunctions.c basesChanging.c basesChanging.h InstructionStatement.c InstructionStatement.h firstPass.c)