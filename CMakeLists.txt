cmake_minimum_required(VERSION 3.15)
project(svm_assembler C)

set(CMAKE_C_STANDARD 99)

add_executable(svm_assembler
        src/main.c
        src/generator.c
        src/linked_list.c
        src/literal.c
        src/mnemonics.c
        src/parser.c
        src/token.c
        src/util.c
        src/parser.h
        src/linked_list.h
        src/token.h
        src/literal.h
        src/generator.h
        src/util.h)