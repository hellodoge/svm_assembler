#ifndef SVM_ASSEMBLER_GENERATOR_H
#define SVM_ASSEMBLER_GENERATOR_H

#define HEADER sizeof(uint16_t)

#define SET(word,data,pos,len) (word = word | (data << (16u - pos - len)))
#define SET_OP(word, operator) (SET(word, operator, 0u, 4u))
#define SET_R1(word, reg) (SET(word, reg, 4u, 3u))
#define SET_R2(word, reg) (SET(word, reg, 8, 3))
#define SET_MODE(word) (SET(word, 1u, 7u, 1u))
#define SET_TERM(word, term) (SET(word, term, 8u, 8u))

#include <stdio.h>
#include <stdint.h>
#include "linked_list.h"

linked_node_t* goto_segment(linked_node_t *node, uint8_t segment);

typedef struct generator_output {
	int return_code;
	unsigned int line_num;
} generator_output_t;

generator_output_t read_text(linked_node_t *node, FILE *fp);
generator_output_t read_data(linked_node_t *node, FILE *fp);


#endif //SVM_ASSEMBLER_GENERATOR_H
