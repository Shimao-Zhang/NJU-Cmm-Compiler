#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
#include "syntax.tab.h"
#include "semantic.h"
#include "SignTable.h"
#include "intermediate.h"

extern FILE* yyin;
extern int yylineno;
extern void yyrestart();
extern int yyparse();
extern int mistakes;
extern node* root;
extern void SemanticAnalyse(const node* root);
extern void translate(const node* root);

int main(int argc, char** argv)
{
    if (argc <= 2)        /* just read stdin */
    {
        return 1;
    }
    FILE *f = fopen(argv[1], "r");
    if (!f)
    {
        perror(argv[1]);
        return 1;
    }
    FILE *fw = fopen(argv[2], "wt+");
    if(!fw)
    {
        perror(argv[2]);
        return 1;
    }
    yyrestart(f);
    yylineno = 1;
    yyparse();
    if (!mistakes)
    {
        // add semantic function
        SemanticAnalyse(root);
        translate(root);
        PrintAllInterCodes(fw);
    }
    fclose(f);
    fclose(fw);
    return 0;
}
