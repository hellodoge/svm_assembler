#ifndef SVM_ASSEMBLER_LINKED_LIST_H
#define SVM_ASSEMBLER_LINKED_LIST_H

#include <stdint.h>

typedef struct linked_node {
	void *content;
	uint8_t malloc_compatible_content;
	struct linked_node *next;
} linked_node_t;

typedef struct linked_list {
	linked_node_t *first_node;
	linked_node_t *last_node;
} linked_list_t;

linked_node_t* create_node(void* content);
linked_node_t* add_node(linked_node_t *previous_node, void *content);
void delete_node(linked_node_t *node);
void delete_linked_nodes(linked_node_t *node);
void delete_list(linked_list_t *list);
void append_node(linked_list_t *list, void *content);
linked_list_t* create_list();
int get_linked_list_len(linked_node_t *node);


#endif //SVM_ASSEMBLER_LINKED_LIST_H
