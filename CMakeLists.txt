cmake_minimum_required(VERSION 3.15)
project(Maman14 C)

set(CMAKE_C_STANDARD 90)

add_executable(Maman14 main.c inputAnalyze.c inputAnalyze.h symbolTableLinkedList.c symbolTableLinkedList.h defaults.h printFunctions.c printFunctions.h firstPass.c firstPass.h dataImage.c dataImage.h secondPass.c secondPass.h)