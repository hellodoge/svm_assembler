#ifndef SVM_ASSEMBLER_PARSER_H
#define SVM_ASSEMBLER_PARSER_H

#include <stdio.h>
#include <stdint.h>
#include "linked_list.h"

uint8_t compile_regex();
linked_node_t* get_words(char *line);
void process_mnemonic(char* line, linked_list_t *token_list, linked_list_t *literal_list, unsigned line_num);
void process_segment(char* line, linked_list_t *token_list, unsigned line_num);
void process_label(char* line, linked_list_t *token_list, linked_list_t *literal_list, unsigned line_num);

typedef struct read_file_output {
	linked_list_t *list_literals;
	linked_list_t *list_tokens;
} read_file_output_t;

read_file_output_t read_file(FILE* fp);


#endif //SVM_ASSEMBLER_PARSER_H