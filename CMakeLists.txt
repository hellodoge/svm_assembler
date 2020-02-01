cmake_minimum_required(VERSION 3.15)
project(svm_assembler C)

set(CMAKE_C_STANDARD 99)

add_executable(svm_assembler main.c parser.h mnemonics.h linked_list.h token.h literal.h generator.h)