#ifndef SVM_ASSEMBLER_UTIL_H
#define SVM_ASSEMBLER_UTIL_H

#include "linked_list.h"
#include "literal.h"
#include "token.h"

void process_literal(linked_node_t *current_node, linked_list_t *token_list,
                     linked_list_t *literal_list, token_type_t type, unsigned line_num);


#endif //SVM_ASSEMBLER_UTIL_H
