#ifndef SVM_ASSEMBLER_LITERAL_H
#define SVM_ASSEMBLER_LITERAL_H

#include <string.h>
#include <stdio.h>
#include "linked_list.h"

typedef struct literal {
	char *string;
	linked_list_t *fpos_list;
	uint16_t value;
} literal_t;

literal_t* set_literal(char *string);
void delete_literal_list(linked_list_t *list);
void insert_literal_value(literal_t *literal, FILE *fp);
void insert_literals_values(linked_node_t *node_literal, FILE *fp);


#endif //SVM_ASSEMBLER_LITERAL_H
