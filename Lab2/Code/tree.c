#include "tree.h"

node* create_node(int linenumber, char* type, char* val, int childnum, node** childarrey)
{
    node* p=(node*)malloc(sizeof(node));
    p->lineno=linenumber;
    p->childnum=childnum;
    p->childarr=childarrey;
    p->type=(char*)malloc((strlen(type)+1)*sizeof(char));
    p->value=(char*)malloc((strlen(val)+1)*sizeof(char));
    strcpy(p->type,type);
    strcpy(p->value,val);
    return p;
}

void print_tree(const node* root, int depth)
{
    if(root == NULL)
    {
        return;
    }
    if(root->childnum == 1 && strcmp(root->childarr[0]->type, "empty") == 0)
        return;
    for(int i=0;i<depth;++i)
    {
        printf("  ");
    }
    if(strcmp(root->type, "ID") == 0)
    {
        printf("ID: %s\n", root->value);
    }
    else if(strcmp(root->type, "TYPE") == 0)
    {
        printf("TYPE: %s\n", root->value);
    }
    else if(strcmp(root->type, "INT") == 0)
    {
        printf("INT: %d\n", atoi(root->value));
    }
    else if(strcmp(root->type, "FLOAT") == 0)
    {
        printf("FLOAT: %f\n", atof(root->value));
    }
    else if(root->childarr == NULL)
    {
        printf("%s\n", root->type);
    }
    else
    {
        printf("%s (%d)\n", root->type, root->lineno);
    }
    for(int i=0;i<root->childnum;++i)
    {
        print_tree(root->childarr[i], depth+1);
    }
    return;
}