#include <stdlib.h>

#include "tree.h"

PNODE node_create(PNODE left, PNODE right, u32 value, u32 weight) {
	PNODE node;
	node = (PNODE)malloc( sizeof( NODE));
	node->left   = left;
	node->right  = right;
	node->value  = value;
	node->weight = weight;
	return node;
}

void free_tree(PNODE node) {
	if(node) {
		free_tree(node->left);
		free_tree(node->right);
		free(node);
	}
}
