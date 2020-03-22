#include "generator.h"

#include <malloc.h>
#include "token.h"
#include "linked_list.h"

linked_node_t* goto_segment(linked_node_t *node, uint8_t segment) {
	do {
		token_t *token = node->content;
		if (token->type == SEGMENT && token->value == segment)
			break;
		node = node->next;
	} while (node);
	return node ? node->next : 0;
}

generator_output_t read_text(linked_node_t *node, FILE *fp) {
	generator_output_t output;
	node = goto_segment(node, TK_SEG_TEXT);
	while (node) {
		uint16_t word = 0;
		token_t *token = node->content;
		output.line_num = token->line;
		if (token->type == SEGMENT) {
			node = goto_segment(node, TK_SEG_TEXT);
			continue;
		}
		if (token->type == LABEL) {
			token->literal->value = ftell(fp) - HEADER;
			node = node->next;
			continue;
		}
		output.return_code = -1;
		if (token->type != DIRECTIVES) return output;
		output.return_code = -2;
		SET_OP(word, token->value);
		switch (token->value) {
			case TK_ADD:
			case TK_SUB:
			case TK_MUL:
			case TK_DIV:
			case TK_MOD:
			case TK_AND:
			case TK_OR:
			case TK_CMP: {
				if (get_linked_list_len(node) < 3) return output;
				node = node->next;
				token_t *token_r1 = node->content;
				node = node->next;
				token_t *term = node->content;
				if (token_r1->type != REGISTER) return output;
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
			case TK_NOT: {
				if (get_linked_list_len(node) < 2) return output;
				node = node->next;
				token_t *token_r1 = node->content;
				linked_node_t *node_r2 = node->next;
				if (node_r2) {
					token_t *token_r2 = node_r2->content;
					if (token_r2->type == REGISTER) {
						node = node->next;
						SET_MODE(word),
								SET_R2(word, token_r2->value);
					}
				}
				if (token_r1->type != REGISTER) return output;
				SET_R1(word, token_r1->value);
				fwrite(&word,sizeof(uint16_t),1,fp);
				break;
			}
			case TK_LDR:
			case TK_STR:
			case TK_MOV: {
				if (get_linked_list_len(node) < 3) return output;
				node = node->next;
				token_t *token_r1 = node->content;
				if (token_r1->type != REGISTER) return output;
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
				} else return output;
				break;
			}
			case TK_PUSH:
			case TK_POP: {
				if (get_linked_list_len(node) < 2) return output;
				node = node->next;
				token_t *token_r1 = node->content;
				if (token_r1->type != REGISTER) return output;
				SET_R1(word, token_r1->value);
				fwrite(&word,sizeof(uint16_t),1,fp);
				break;
			}
			case TK_JMP: {
				if (get_linked_list_len(node) < 2) return output;
				node = node->next;
				token_t *token_term = node->content;
				token_t *token_conditional = node->next->content;
				if (token_conditional->type == INTEGER) {
					node = node->next;
					SET_TERM(word, token_conditional->value);
				} else SET_TERM(word, UINT8_MAX);
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
					return output;
				break;
			}
			default:
				fwrite(&word,sizeof(uint16_t),1,fp);
				break;
		}
		node = node->next;
	}
	output.return_code = 0;
	return output;
}

generator_output_t read_data(linked_node_t *node, FILE *fp) {
	generator_output_t output;
	node = goto_segment(node, TK_SEG_DATA);
	while (node) {
		token_t *token = node->content;
		output.line_num = token->line;
		if (token->type == SEGMENT) {
			node = goto_segment(node, TK_SEG_DATA);
			continue;
		}
		output.return_code = -1;
		if (token->type != DEFINE) return output;
		output.return_code = -2;
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
				} else return output;
			}
			node = node->next;
			if (!node) break;
			tmp_token = node->content;
		}
	}
	output.return_code = 0;
	return output;
}

generator_output_t read_bss(linked_node_t *node, unsigned ftell_out) {
	generator_output_t output;
	output.bss_size = 0;
	node = goto_segment(node, TK_SEG_BSS);
	while (node) {
		token_t *token = node->content;
		output.line_num = token->line;
		if (token->type == SEGMENT) {
			node = goto_segment(node, TK_SEG_BSS);
			continue;
		}
		output.return_code = -1;
		if (token->type != DEFINE) return output;
		output.return_code = -2;
		node = node->next;
		token_t *tmp_token = node->content;
		if (tmp_token->type == LITERAL) {
			literal_t *literal = tmp_token->literal;
			literal->value = ftell_out + output.bss_size - HEADER;
			node = node->next;
			tmp_token = node->content;
		}
		if (tmp_token->type != INTEGER || token->value != TK_DEF_RW)
			return output;
		output.bss_size += tmp_token->value * 2;
		node = node->next;
	}
	output.return_code = 0;
	return output;
}