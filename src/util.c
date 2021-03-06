#include "util.h"

#include "linked_list.h"
#include "literal.h"
#include "token.h"


void process_literal(linked_node_t *current_node, linked_list_t *token_list,
                     linked_list_t *literal_list, token_type_t type, unsigned line_num) {
	linked_node_t *current_literal_node = literal_list->first_node;
	if (current_literal_node)
		do {
			literal_t *current_literal = current_literal_node->content;
			if (!strcmp(current_literal->string, current_node->content)) {
				append_node(token_list, get_literal_token(current_literal, line_num));
				((token_t*)token_list->last_node->content)->type = type;
				return;
			}
			current_literal_node = current_literal_node->next;
		} while (current_literal_node);
	append_node(token_list, 0);
	literal_t *literal = set_literal(current_node->content);
	current_node->malloc_compatible_content = 0;
	token_list->last_node->content = get_literal_token(literal, line_num);
	((token_t*)(token_list->last_node->content))->type = type;
	append_node(literal_list,literal);
}