#include "parser.h"
#include "literal.h"
#include "util.h"

#include <ctype.h>
#include <regex.h>
#include <malloc.h>
#include <stdlib.h>


extern char *mnemonics[16];
extern char *registers[8];

regex_t regex_mnemonic;
regex_t regex_label;
regex_t regex_segment;
regex_t regex_empty_line;
regex_t regex_comment_line;
regex_t regex_word;
regex_t regex_digit;
regex_t regex_string;


read_file_output_t read_file(FILE* fp) {
	linked_list_t *list_tokens = create_list();
	linked_list_t *list_literals = create_list();
	read_file_output_t output;
	char *line = 0;
	size_t len;
	unsigned int line_num = 0;
	while (getline(&line, &len, fp) != -1) {
		line_num++;
		if (!regexec(&regex_label, line, 0, 0, 0)) {
			process_label(line, list_tokens, list_literals, line_num);
		} else if (!regexec(&regex_mnemonic, line, 0, 0, 0)) {
			process_mnemonic(line, list_tokens, list_literals, line_num);
		} else if (!regexec(&regex_segment, line, 0, 0, 0)) {
			process_segment(line, list_tokens, line_num);
		} else if (!regexec(&regex_empty_line, line, 0, 0, 0)
		           || !regexec(&regex_comment_line, line, 0, 0, 0)) {
			continue;
		} else {
			printf("svm-asm: syntax error: (line %d)\n", line_num);
			delete_literal_list(list_literals);
			delete_list(list_tokens);
			output.list_tokens = 0;
			free(line);
			return output;
		}
	}
	if (line) free(line);
	output.list_tokens = list_tokens;
	output.list_literals = list_literals;
	return output;
}

uint8_t compile_regex() {
	uint8_t result = regcomp(&regex_mnemonic, "^[\t ]*\\w+", REG_EXTENDED);
	result += regcomp(&regex_label, "^[\t ]*\\w+:", REG_EXTENDED);
	result += regcomp(&regex_segment, "^[\t ]*\\.\\w+", REG_EXTENDED);
	result += regcomp(&regex_empty_line, "^[\t ]*\r?\n?$", REG_EXTENDED);
	result += regcomp(&regex_comment_line, "^[\t ]*;", REG_EXTENDED);
	result += regcomp(&regex_word, "\\w+", REG_EXTENDED);
	result += regcomp(&regex_digit, "^[0-9]+$", REG_EXTENDED);
	result += regcomp(&regex_string, "\"[^\"]+\"", REG_EXTENDED);
	return result;
}

linked_node_t* get_words(char *line) {
	linked_list_t *words_list = create_list(0);
	regmatch_t match;
	regmatch_t match_string;
	while (!regexec(&regex_word, line, 1, &match, 0)) {
		if (!regexec(&regex_string, line, 1, &match_string, 0))
			if (match_string.rm_so < match.rm_so) {
				match = match_string;
				match.rm_so += sizeof(char);
				match.rm_eo -= sizeof(char);
			}
		size_t len = match.rm_eo - match.rm_so;
		char *copy = malloc(sizeof(char) * (len + 1));
		*(copy + len) = '\0';
		line += match.rm_so;
		memcpy(copy, line, len);
		line += len;
		append_node(words_list, copy);
	}
	linked_node_t *list = words_list->first_node;
	free(words_list);
	return list;
}

void process_mnemonic(char* line, linked_list_t *token_list, linked_list_t *literal_list, unsigned line_num) {
	linked_node_t *words_list = get_words(line);
	linked_node_t *current_node = words_list;
	do {
		for (int i = 0; i < 16; i++) {
			if (!strcmp(current_node->content, mnemonics[i])) {
				append_node(token_list, get_token(DIRECTIVES, i, line_num));
				goto processed;
			}
		}
		for (int i = 0; i < 8; i++) {
			if (!strcmp(current_node->content, registers[i])) {
				append_node(token_list, get_token(REGISTER, i, line_num));
				goto processed;
			}
		}
		//if (!regexec(&regex_digit, current_node->content, 1, 0, 0)) {
		if (isdigit((int)*(char*)(current_node->content))) {
			append_node(token_list, get_token(INTEGER, atoi(current_node->content), line_num)); // NOLINT(cert-err34-c)
			goto processed;
		}
		{ // SET DEFINE
			if (!strcmp(current_node->content, "DB")) {
				append_node(token_list, get_token(DEFINE, TK_DEF_DB, line_num));
				goto processed;
			} else if (!strcmp(current_node->content, "DW")) {
				append_node(token_list, get_token(DEFINE, TK_DEF_DW, line_num));
				goto processed;
			} else if (!strcmp(current_node->content, "RW")) {
				append_node(token_list, get_token(DEFINE, TK_DEF_RW, line_num));
				goto processed;
			}
		}
		process_literal(current_node, token_list, literal_list, LITERAL, line_num);

		processed:
		current_node = current_node->next;
	} while (current_node);
	delete_linked_nodes(words_list);
}

void process_segment(char* line, linked_list_t *token_list, unsigned line_num) {
	linked_node_t *segment_string = get_words(line);
	token_t *segment_token = get_token(SEGMENT, 0, line_num);
	if (!strcmp(segment_string->content, "TEXT"))
		segment_token->value = TK_SEG_TEXT;
	else if (!strcmp(segment_string->content, "DATA"))
		segment_token->value = TK_SEG_DATA;
	else if (!strcmp(segment_string->content, "BSS"))
		segment_token->value = TK_SEG_BSS;
	delete_linked_nodes(segment_string);
	append_node(token_list, segment_token);
}

void process_label(char* line, linked_list_t *token_list, linked_list_t *literal_list, unsigned line_num) {
	linked_node_t *label_string = get_words(line);
	process_literal(label_string, token_list, literal_list, LABEL, line_num);
	delete_linked_nodes(label_string);
}