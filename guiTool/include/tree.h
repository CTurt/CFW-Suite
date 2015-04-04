#pragma once

#include "types.h"

typedef struct _NODE NODE, *PNODE;

typedef struct _NODE {
	PNODE left;
	PNODE right;
	u32 value;
	u32 weight;
} NODE, *PNODE;

NODE* node_create(NODE *left, NODE *right, u32 value, u32 weight);
void free_tree(NODE *node);
