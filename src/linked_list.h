#ifndef SVM_ASSEMBLER_LINKED_LIST_H
#define SVM_ASSEMBLER_LINKED_LIST_H

#include <malloc.h>
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

linked_node_t* create_node(void* content) {
	linked_node_t *node = malloc(sizeof(linked_node_t));
	node->content = content;
	node->malloc_compatible_content = 1;
	node->next = 0;
	return node;
}

linked_node_t* add_node(linked_node_t *previous_node, void *content) {
	linked_node_t *node = create_node(content);
	previous_node->next = node;
	return node;
}

void delete_node(linked_node_t *node) {
	if (node->malloc_compatible_content)
		free(node->content);
	free(node);
}

void delete_linked_nodes(linked_node_t *node) {
	do {
		linked_node_t *next_node = node->next;
		delete_node(node);
		node = next_node;
	} while (node != 0);
}

void delete_list(linked_list_t *list) {
	if (list->first_node)
		delete_linked_nodes(list->first_node);
	free(list);
}

void append_node(linked_list_t *list, void *content) {
	if (list->first_node) {
		add_node(list->last_node, content);
		list->last_node = list->last_node->next;
	} else {
		list->first_node = create_node(content);
		list->last_node = list->first_node;
	}
}

linked_list_t* create_list() {
	linked_list_t *list = malloc(sizeof(linked_list_t));
	list->first_node = 0;
	return list;
}

int get_linked_list_len(linked_node_t *node) {
	int len = 0;
	while (node) {
		len++;
		node = node->next;
	}
	return len;
}

#endif //SVM_ASSEMBLER_LINKED_LIST_H
