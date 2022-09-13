#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
#include "syntax.tab.h"
#include "semantic.h"
#include "SignTable.h"

extern FILE* yyin;
extern int yylineno;
extern void yyrestart();
extern int yyparse();
extern int mistakes;
extern node* root;
extern void SemanticAnalyse(const node* root);

int main(int argc, char** argv)
{
    if (argc < 2)        /* just read stdin */
    {
        return 1;
    }
    for (int i = 1; i < argc; i++)
    {
        FILE *f = fopen(argv[i], "r");
        if (!f)
        {
            perror(argv[i]);
            return 1;
        }
        yyrestart(f);
        yylineno=1;
        yyparse();
        if(!mistakes)
        {
            //add semantic function
            SemanticAnalyse(root);
        }
        fclose(f);
    }
    return 0;
}
