#ifndef _TREE_H_
#define _TREE_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct node{
    int lineno;
    int childnum;
    struct node** childarr;
    char* type;
    char* value;
}node;

node* create_node(int linenumber, char* type, char* val, int childnum, node** childarrey);

void print_tree(node* root, int depth);

#endif