#ifndef SVM_ASSEMBLER_GENERATOR_H
#define SVM_ASSEMBLER_GENERATOR_H

#define HEADER sizeof(uint16_t)

#define SET(word,data,pos,len) (word = word | (data << (16u - pos - len)))
#define SET_OP(word, operator) (SET(word, operator, 0u, 4u))
#define SET_R1(word, reg) (SET(word, reg, 4u, 3u))
#define SET_R2(word, reg) (SET(word, reg, 8, 3))
#define SET_MODE(word) (SET(word, 1u, 7u, 1u))
#define SET_TERM(word, term) (SET(word, term, 8u, 8u))

#include "token.h"

linked_node_t* goto_segment(linked_node_t *node, uint8_t segment) {
	do {
		token_t *token = node->content;
		if (token->type == SEGMENT && token->value == segment)
			break;
		node = node->next;
	} while (node);
	return node ? node->next : 0;
}

int read_text(linked_node_t *node, FILE *fp) {
	node = goto_segment(node, TK_SEG_TEXT);
	while (node) {
		uint16_t word = 0;
		token_t *token = node->content;
		if (token->type == SEGMENT) {
			node = goto_segment(node, TK_SEG_TEXT);
			continue;
		}
		if (token->type == LABEL) {
			token->literal->value = ftell(fp) - HEADER;
			node = node->next;
			continue;
		}
		if (token->type != DIRECTIVES) return -1;
		SET_OP(word, token->value);
		switch (token->value) {
			case TK_ADD:
			case TK_SUB:
			case TK_MUL:
			case TK_DIV:
			case TK_MOD:
			case TK_AND:
			case TK_OR:
			case TK_CMP:
			case TK_NOT: {
				if (get_linked_list_len(node) < 3) return -2;
				node = node->next;
				token_t *token_r1 = node->content;
				node = node->next;
				token_t *term = node->content;
				if (token_r1->type != REGISTER) return -2;
				SET_R1(word, token_r1->value);
				if (term->type == INTEGER) {
					SET_TERM(word, term->value);
					SET_MODE(word);
				} else if (term->type == REGISTER) {
					SET_R2(word, term->value);
				}
				fwrite(&word,sizeof(uint16_t),1,fp);
				break;
			}
			case TK_LDR:
			case TK_STR:
			case TK_MOV: {
				if (get_linked_list_len(node) < 3) return -2;
				node = node->next;
				token_t *token_r1 = node->content;
				if (token_r1->type != REGISTER) return -2;
				SET_R1(word, token_r1->value);
				node = node->next;
				token_t *term = node->content;
				if (term->type == REGISTER) {
					SET_MODE(word);
					SET_R2(word,term->value);
					fwrite(&word,sizeof(uint16_t),1,fp);
				} else if (term->type == LITERAL) {
					fwrite(&word,sizeof(uint16_t),1,fp);
					append_node(term->literal->fpos_list, malloc(sizeof(fpos_t)));
					fgetpos(fp, term->literal->fpos_list->last_node->content);
					fseek(fp, 2, SEEK_CUR);
				} else if (term->type == INTEGER) {
					fwrite(&word,sizeof(uint16_t),1,fp);
					fwrite(&term->value,sizeof(uint16_t),1,fp);
				} else return -2;
				break;
			}
			case TK_PUSH:
			case TK_POP: {
				if (get_linked_list_len(node) < 2) return -2;
				node = node->next;
				token_t *token_r1 = node->content;
				if (token_r1->type != REGISTER) return -2;
				SET_R1(word, token_r1->value);
				fwrite(&word,sizeof(uint16_t),1,fp);
				break;
			}
			case TK_JMP: {
				if (get_linked_list_len(node) < 2) return -2;
				node = node->next;
				token_t *token_term = node->content;
				token_t *token_unconditional = node->next->content;
				uint8_t unconditional = 0;
				if (token_unconditional->type == INTEGER) {
					node = node->next;
					unconditional = token_unconditional->value;
				}
				if (unconditional) SET_TERM(word, 1u);
				if (token_term->type == REGISTER) {
					SET_MODE(word);
					SET_R1(word, token_term->value);
				}
				fwrite(&word,sizeof(uint16_t),1,fp);
				if (token_term->type == LITERAL) {
					literal_t *literal = token_term->literal;
					append_node(literal->fpos_list, malloc(sizeof(fpos_t)));
					fgetpos(fp, literal->fpos_list->last_node->content);
					fseek(fp, 2, SEEK_CUR);
				}
				if (token_term->type == INTEGER) {
					fwrite(&token_term->value,sizeof(uint16_t),1,fp);
				} else if (token_term->type != LITERAL && token_term->type != REGISTER)
					return -2;
				break;
			}
			default:
				fwrite(&word,sizeof(uint16_t),1,fp);
				break;
		}
		node = node->next;
	}
	return 0;
}

int read_data(linked_node_t *node, FILE *fp) {
	node = goto_segment(node, TK_SEG_DATA);
	while (node) {
		token_t *token = node->content;
		if (token->type == SEGMENT) {
			node = goto_segment(node, TK_SEG_DATA);
			continue;
		}
		if (token->type != DEFINE) return -1;
		node = node->next;
		token_t *tmp_token = node->content;
		if (tmp_token->type == LITERAL) {
			literal_t *literal = tmp_token->literal;
			literal->value = ftell(fp) - HEADER;
			node = node->next;
			tmp_token = node->content;
		}
		while (tmp_token->type == LITERAL || tmp_token->type == INTEGER) {
			if (tmp_token->type == LITERAL)
				fprintf(fp, "%s", tmp_token->literal->string);
			else {
				if (token->value == TK_DEF_DB) {
					uint8_t value = tmp_token->value;
					fwrite(&value, sizeof(uint8_t), 1, fp);
				} else if (token->value == TK_DEF_DW) {
					uint16_t value = tmp_token->value;
					fwrite(&value, sizeof(uint16_t), 1, fp);
				} else return -2;
			}
			node = node->next;
			if (!node) break;
			tmp_token = node->content;
		}
	}
	return 0;
}



#endif //SVM_ASSEMBLER_GENERATOR_H
