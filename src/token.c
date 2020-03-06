#include "token.h"

#include <malloc.h>
#include <stdint.h>


token_t* get_token(token_type_t type, uint16_t value, unsigned line) {
	token_t *token = malloc(sizeof(token_t));
	token->type = type;
	token->value = value;
	token->line = line;
	return token;
}

token_t* get_literal_token(literal_t *literal, unsigned line) {
	token_t *token = malloc(sizeof(token_t));
	token->type = LITERAL;
	token->literal = literal;
	token->line = line;
	return token;
}