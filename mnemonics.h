#ifndef SVM_ASSEMBLER_MNEMONICS_H
#define SVM_ASSEMBLER_MNEMONICS_H

#include <stdint.h>

char *mnemonics[16] = {
		"ADD",
		"SUB",
		"MUL",
		"DIV",
		"MOD",
		"AND",
		"OR",
		"CMP",
		"NOT",
		"LDR",
		"STR",
		"MOV",
		"PUSH",
		"POP",
		"JMP",
		"TRAP",
};

char *registers[8] = {
		"AX",
		"BX",
		"CX",
		"DX",
		"PC",
		"SP",
		"HP",
		"FLAGS",
};

#endif //SVM_ASSEMBLER_MNEMONICS_H
