#ifndef SVM_ASSEMBLER_TOKEN_H
#define SVM_ASSEMBLER_TOKEN_H

#include "literal.h"

typedef enum types {
	INTEGER,
	DIRECTIVES,
	LABEL,
	REGISTER,
	LITERAL,
	SEGMENT,
	DEFINE,
} token_type_t;


enum tokens {
	TK_ADD = 0,
	TK_SUB,
	TK_MUL,
	TK_DIV,
	TK_MOD,
	TK_AND,
	TK_OR,
	TK_CMP,
	TK_NOT,
	TK_LDR,
	TK_STR,
	TK_MOV,
	TK_PUSH,
	TK_POP,
	TK_JMP,
	TK_TRAP,
	TK_SEG_TEXT,
	TK_SEG_DATA,
	TK_SEG_BSS,
	TK_DEF_DB,
	TK_DEF_DW,
	TK_DEF_RW,
};

typedef struct token {
	token_type_t type;
	unsigned int line;
	union {
		uint16_t value;
		literal_t *literal;
	};
} token_t;

token_t* get_token(token_type_t type, uint16_t value, unsigned line);
token_t* get_literal_token(literal_t *literal, unsigned line);

#endif //SVM_ASSEMBLER_TOKEN_H
