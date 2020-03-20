#ifndef H_TREE_LINKED_LIST_FUNCTIONS_H
#define H_TREE_LINKED_LIST_FUNCTIONS_H

#include "../particles/struct_particles.h"
#include "struct_tree_linked_list_node.h"


void TreeLinkedList_Targets_Construct(struct TreeLinkedListNode **p, struct Particles *targets,
                int ibeg, int iend, int maxparnode,
                double *xyzmm, int level, int *numnodes, int *numleaves);

void TreeLinkedList_Sources_Construct(struct TreeLinkedListNode **p, struct Particles *sources,
                int ibeg, int iend, int maxparnode, double *xyzmm,
                int level, int *numnodes, int *numleaves);

int TreeLinkedList_SetIndex(struct TreeLinkedListNode *p, int index);

void TreeLinkedList_Free(struct TreeLinkedListNode *p);


#endif /* H_TREE_LINKED_LIST_FUNCTIONS */
