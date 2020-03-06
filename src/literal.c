#include "literal.h"

#include <stdio.h>
#include <malloc.h>
#include "linked_list.h"


literal_t* set_literal(char *string) {
	literal_t *literal = malloc(sizeof(literal_t));
	literal->string = string;
	literal->fpos_list = create_list();
	return literal;
}

void delete_literal_list(linked_list_t *list) {
	linked_node_t *node = list->first_node;
	while (node) {
		literal_t *literal = node->content;
		free(literal->string);
		delete_list(literal->fpos_list);
		node = node->next;
	}
	delete_list(list);
}

void insert_literal_value(literal_t *literal, FILE *fp) {
	linked_node_t *node = literal->fpos_list->first_node;
	while (node) {
		fsetpos(fp,node->content);
		fwrite(&literal->value, sizeof(uint16_t), 1, fp);
		node = node->next;
	}
}

void insert_literals_values(linked_node_t *node_literal, FILE *fp) {
	while (node_literal) {
		literal_t *literal = node_literal->content;
		insert_literal_value(literal, fp);
		node_literal = node_literal->next;
	}
}