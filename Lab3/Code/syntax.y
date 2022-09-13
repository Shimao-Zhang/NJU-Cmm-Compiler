%locations
%{
    #define YYSTYPE node*
    #include "tree.h"
    #include "lex.yy.c"
    void yyerror(char* msg);
    extern int yylineno;
    int mistakes = 0;
    node* root = NULL;
%}

%start Program

 /* declared tokens */
%token INT
%token FLOAT
%token WRONG_FLOAT
%token PLUS MINUS STAR DIV
%token ID
%token TYPE
%token DOT SEMI COMMA
%token ASSIGNOP RELOP
%token AND OR
%token NOT
%token LP RP LB RB LC RC
%token STRUCT
%token RETURN
%token IF ELSE
%token WHILE

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT
%left DOT LB RB LP RP

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%
Program : ExtDefList 
{
    node** arr = (node**)malloc(sizeof(node*));
    *(arr) = $1;
    root = create_node(@$.first_line, "Program", "NULL", 1, arr);
}
;
ExtDefList : ExtDef ExtDefList 
{
    node** arr = (node**)malloc(2*sizeof(node*));
    *(arr) = $1;
    *(arr+1) = $2;
    $$ = create_node(@$.first_line, "ExtDefList", "NULL", 2, arr);
}
| /* empty */ 
{
    node** arr = (node**)malloc(sizeof(node*));
    *(arr) = create_node(@$.first_line, "empty", "NULL", 0, NULL);
    $$ = create_node(@$.first_line, "ExtDefList", "NULL", 1, arr);
}
;
ExtDef : Specifier ExtDecList SEMI 
{
    node** arr = (node**)malloc(3*sizeof(node*));
    *(arr) = $1;
    *(arr+1) = $2;
    *(arr+2) = $3;
    $$ = create_node(@$.first_line, "ExtDef", "NULL", 3, arr);
}
| Specifier SEMI 
{
    node** arr = (node**)malloc(2*sizeof(node*));
    *(arr) = $1;
    *(arr+1) = $2;
    $$ = create_node(@$.first_line, "ExtDef", "NULL", 2, arr);
}
| Specifier FunDec CompSt 
{
    node** arr = (node**)malloc(3*sizeof(node*));
    *(arr) = $1;
    *(arr+1) = $2;
    *(arr+2) = $3;
    $$ = create_node(@$.first_line, "ExtDef", "NULL", 3, arr);
}
| Specifier FunDec SEMI 
{
    node** arr = (node**)malloc(3*sizeof(node*));
    *(arr) = $1;
    *(arr+1) = $2;
    *(arr+2) = $3;
    $$ = create_node(@$.first_line, "ExtDef", "NULL", 3, arr);
}
| error SEMI 
{
    mistakes = 1;
    node** arr = (node**)malloc(2*sizeof(node*));
    *(arr) = create_node(0, "error", "NULL", 0, NULL);
    *(arr+1) = $2;
    $$ = create_node(@$.first_line, "ExtDef", "NULL", 2, arr);
}
| Specifier FunDec error RC {}
{
    mistakes = 1;
    node** arr = (node**)malloc(4*sizeof(node*));
    *(arr) = $1;
    *(arr+1) = create_node(0, "error", "NULL", 0, NULL);
    *(arr+2) = $3;
    *(arr+3) = $4;
    $$ = create_node(@$.first_line, "ExtDef", "NULL", 4, arr);
}
;
ExtDecList : VarDec 
{
    node** arr = (node**)malloc(sizeof(node*));
    *(arr) = $1;
    $$ = create_node(@$.first_line, "ExtDecList", "NULL", 1, arr);
}
| VarDec COMMA ExtDecList 
{
    node** arr = (node**)malloc(3*sizeof(node*));
    *(arr) = $1;
    *(arr+1) = $2;
    *(arr+2) = $3;
    $$ = create_node(@$.first_line, "ExtDecList", "NULL", 3, arr);
}
;

Specifier : TYPE 
{
    node** arr = (node**)malloc(sizeof(node*));
    *(arr) = $1;
    $$ = create_node(@$.first_line, "Specifier", "NULL", 1, arr);
}
| StructSpecifier 
{
    node** arr = (node**)malloc(sizeof(node*));
    *(arr) = $1;
    $$ = create_node(@$.first_line, "Specifier", "NULL", 1, arr);
}
;
StructSpecifier : STRUCT OptTag LC DefList RC 
{
    node** arr = (node**)malloc(5*sizeof(node*));
    *(arr) = $1;
    *(arr+1) = $2;
    *(arr+2) = $3;
    *(arr+3) = $4;
    *(arr+4) = $5;
    $$ = create_node(@$.first_line, "StructSpecifier", "NULL", 5, arr);
}
| STRUCT Tag 
{
    node** arr = (node**)malloc(2*sizeof(node*));
    *(arr) = $1;
    *(arr+1) = $2;
    $$ = create_node(@$.first_line, "StructSpecifier", "NULL", 2, arr);
}
;
OptTag : ID 
{
    node** arr = (node**)malloc(sizeof(node*));
    *(arr) = $1;
    $$ = create_node(@$.first_line, "OptTag", "NULL", 1, arr);
}
| /* empty */ 
{
    node** arr = (node**)malloc(sizeof(node*));
    *(arr) = create_node(@$.first_line, "empty", "NULL", 0, NULL);
    $$ = create_node(@$.first_line, "OptTag", "NULL", 1, arr);
}
;
Tag : ID 
{
    node** arr = (node**)malloc(sizeof(node*));
    *(arr) = $1;
    $$ = create_node(@$.first_line, "Tag", "NULL", 1, arr);
}
;

VarDec : ID 
{
    node** arr = (node**)malloc(sizeof(node*));
    *(arr) = $1;
    $$ = create_node(@$.first_line, "VarDec", "NULL", 1, arr);
}
| VarDec LB INT RB 
{
    node** arr = (node**)malloc(4*sizeof(node*));
    *(arr) = $1;
    *(arr+1) = $2;
    *(arr+2) = $3;
    *(arr+3) = $4;
    $$ = create_node(@$.first_line, "VarDec", "NULL", 4, arr);
}
| error RB 
{
    mistakes = 1;
    node** arr = (node**)malloc(2*sizeof(node*));
    *(arr) = create_node(0, "error", "NULL", 0, NULL);
    *(arr+1) = $2;
    $$ = create_node(@$.first_line, "VarDec", "NULL", 2, arr);
}
;
FunDec : ID LP VarList RP 
{
    node** arr = (node**)malloc(4*sizeof(node*));
    *(arr) = $1;
    *(arr+1) = $2;
    *(arr+2) = $3;
    *(arr+3) = $4;
    $$ = create_node(@$.first_line, "FunDec", "NULL", 4, arr);
}
| ID LP RP 
{
    node** arr = (node**)malloc(3*sizeof(node*));
    *(arr) = $1;
    *(arr+1) = $2;
    *(arr+2) = $3;
    $$ = create_node(@$.first_line, "FunDec", "NULL", 3, arr);
}
| error RP 
{
    mistakes = 1;
    node** arr = (node**)malloc(2*sizeof(node*));
    *(arr) = create_node(0, "error", "NULL", 0, NULL);
    *(arr+1) = $2;
    $$ = create_node(@$.first_line, "FunDec", "NULL", 2, arr);
}
;
VarList : ParamDec COMMA VarList 
{
    node** arr = (node**)malloc(3*sizeof(node*));
    *(arr) = $1;
    *(arr+1) = $2;
    *(arr+2) = $3;
    $$ = create_node(@$.first_line, "VarList", "NULL", 3, arr);
}
| ParamDec 
{
    node** arr = (node**)malloc(sizeof(node*));
    *(arr) = $1;
    $$ = create_node(@$.first_line, "VarList", "NULL", 1, arr);
}
;
ParamDec : Specifier VarDec 
{
    node** arr = (node**)malloc(2*sizeof(node*));
    *(arr) = $1;
    *(arr+1) = $2;
    $$ = create_node(@$.first_line, "ParamDec", "NULL", 2, arr);
}
;

CompSt : LC DefList StmtList RC 
{
    node** arr = (node**)malloc(4*sizeof(node*));
    *(arr) = $1;
    *(arr+1) = $2;
    *(arr+2) = $3;
    *(arr+3) = $4;
    $$ = create_node(@$.first_line, "CompSt", "NULL", 4, arr);
}
| error RC 
{
    mistakes = 1;
    node** arr = (node**)malloc(2*sizeof(node*));
    *(arr) = create_node(0, "error", "NULL", 0, NULL);
    *(arr+1) = $2;
    $$ = create_node(@$.first_line, "CompSt", "NULL", 2, arr);
}
;
StmtList : Stmt StmtList 
{
    node** arr = (node**)malloc(2*sizeof(node*));
    *(arr) = $1;
    *(arr+1) = $2;
    $$ = create_node(@$.first_line, "StmtList", "NULL", 2, arr);
}
| /* empty */ 
{
    node** arr = (node**)malloc(sizeof(node*));
    *(arr) = create_node(@$.first_line, "empty", "NULL", 0, NULL);
    $$ = create_node(@$.first_line, "StmtList", "NULL", 1, arr);
}
;
Stmt : Exp SEMI 
{
    node** arr = (node**)malloc(2*sizeof(node*));
    *(arr) = $1;
    *(arr+1) = $2;
    $$ = create_node(@$.first_line, "Stmt", "NULL", 2, arr);
}
| CompSt 
{
    node** arr = (node**)malloc(sizeof(node*));
    *(arr) = $1;
    $$ = create_node(@$.first_line, "Stmt", "NULL", 1, arr);
}
| RETURN Exp SEMI 
{
    node** arr = (node**)malloc(3*sizeof(node*));
    *(arr) = $1;
    *(arr+1) = $2;
    *(arr+2) = $3;
    $$ = create_node(@$.first_line, "Stmt", "NULL", 3, arr);
}
| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE 
{
    node** arr = (node**)malloc(5*sizeof(node*));
    *(arr) = $1;
    *(arr+1) = $2;
    *(arr+2) = $3;
    *(arr+3) = $4;
    *(arr+4) = $5;
    $$ = create_node(@$.first_line, "Stmt", "NULL", 5, arr);
}
| IF LP Exp RP Stmt ELSE Stmt 
{
    node** arr = (node**)malloc(7*sizeof(node*));
    *(arr) = $1;
    *(arr+1) = $2;
    *(arr+2) = $3;
    *(arr+3) = $4;
    *(arr+4) = $5;
    *(arr+5) = $6;
    *(arr+6) = $7;
    $$ = create_node(@$.first_line, "Stmt", "NULL", 7, arr);
}
| WHILE LP Exp RP Stmt 
{
    node** arr = (node**)malloc(5*sizeof(node*));
    *(arr) = $1;
    *(arr+1) = $2;
    *(arr+2) = $3;
    *(arr+3) = $4;
    *(arr+4) = $5;
    $$ = create_node(@$.first_line, "Stmt", "NULL", 5, arr);
}
| RETURN error SEMI 
{
    mistakes = 1;
    node** arr = (node**)malloc(3*sizeof(node*));
    *(arr) = $1;
    *(arr+1) = create_node(0, "error", "NULL", 0, NULL);
    *(arr+2) = $3;
    $$ = create_node(@$.first_line, "Stmt", "NULL", 3, arr);
}
| IF LP error RP Stmt %prec LOWER_THAN_ELSE 
{
    mistakes = 1;
    node** arr = (node**)malloc(5*sizeof(node*));
    *(arr) = $1;
    *(arr+1) = $2;
    *(arr+2) = create_node(0, "error", "NULL", 0, NULL);
    *(arr+3) = $4;
    *(arr+4) = $5;
    $$ = create_node(@$.first_line, "Stmt", "NULL", 5, arr);
}
| IF LP error RP Stmt ELSE Stmt 
{
    mistakes = 1;
    node** arr = (node**)malloc(7*sizeof(node*));
    *(arr) = $1;
    *(arr+1) = $2;
    *(arr+2) = create_node(0, "error", "NULL", 0, NULL);
    *(arr+3) = $4;
    *(arr+4) = $5;
    *(arr+5) = $6;
    *(arr+6) = $7;
    $$ = create_node(@$.first_line, "Stmt", "NULL", 7, arr);
}
| WHILE LP error RP Stmt 
{
    mistakes = 1;
    node** arr = (node**)malloc(5*sizeof(node*));
    *(arr) = $1;
    *(arr+1) = $2;
    *(arr+2) = create_node(0, "error", "NULL", 0, NULL);
    *(arr+3) = $4;
    *(arr+4) = $5;
    $$ = create_node(@$.first_line, "Stmt", "NULL", 5, arr);
}
| error SEMI 
{
    mistakes = 1;
    node** arr = (node**)malloc(2*sizeof(node*));
    *(arr) = create_node(0, "error", "NULL", 0, NULL);
    *(arr+1) = $2;
    $$ = create_node(@$.first_line, "Stmt", "NULL", 2, arr);
}
;

DefList : Def DefList 
{
    node** arr = (node**)malloc(2*sizeof(node*));
    *(arr) = $1;
    *(arr+1) = $2;
    $$ = create_node(@$.first_line, "DefList", "NULL", 2, arr);
}
| /* empty */ 
{
    node** arr = (node**)malloc(sizeof(node*));
    *(arr) = create_node(@$.first_line, "empty", "NULL", 0, NULL);
    $$ = create_node(@$.first_line, "DefList", "NULL", 1, arr);
}
;
Def : Specifier DecList SEMI 
{
    node** arr = (node**)malloc(3*sizeof(node*));
    *(arr) = $1;
    *(arr+1) = $2;
    *(arr+2) = $3;
    $$ = create_node(@$.first_line, "Def", "NULL", 3, arr);
}
| Specifier error SEMI 
{
    mistakes = 1;
    node** arr = (node**)malloc(3*sizeof(node*));
    *(arr) = $1;
    *(arr+1) = create_node(0, "error", "NULL", 0, NULL);
    *(arr+2) = $3;
    $$ = create_node(@$.first_line, "Def", "NULL", 3, arr);
}
;
DecList : Dec 
{
    node** arr = (node**)malloc(sizeof(node*));
    *(arr) = $1;
    $$ = create_node(@$.first_line, "DecList", "NULL", 1, arr);
}
| Dec COMMA DecList 
{
    node** arr = (node**)malloc(3*sizeof(node*));
    *(arr) = $1;
    *(arr+1) = $2;
    *(arr+2) = $3;
    $$ = create_node(@$.first_line, "DecList", "NULL", 3, arr);
}
;
Dec : VarDec 
{
    node** arr = (node**)malloc(sizeof(node*));
    *(arr) = $1;
    $$ = create_node(@$.first_line, "Dec", "NULL", 1, arr);
}
| VarDec ASSIGNOP Exp 
{
    node** arr = (node**)malloc(3*sizeof(node*));
    *(arr) = $1;
    *(arr+1) = $2;
    *(arr+2) = $3;
    $$ = create_node(@$.first_line, "Dec", "NULL", 3, arr);
}
;

Exp : Exp ASSIGNOP Exp 
{
    node** arr = (node**)malloc(3*sizeof(node*));
    *(arr) = $1;
    *(arr+1) = $2;
    *(arr+2) = $3;
    $$ = create_node(@$.first_line, "Exp", "NULL", 3, arr);
}
| Exp AND Exp 
{
    node** arr = (node**)malloc(3*sizeof(node*));
    *(arr) = $1;
    *(arr+1) = $2;
    *(arr+2) = $3;
    $$ = create_node(@$.first_line, "Exp", "NULL", 3, arr);
}
| Exp OR Exp 
{
    node** arr = (node**)malloc(3*sizeof(node*));
    *(arr) = $1;
    *(arr+1) = $2;
    *(arr+2) = $3;
    $$ = create_node(@$.first_line, "Exp", "NULL", 3, arr);
}
| Exp RELOP Exp 
{
    node** arr = (node**)malloc(3*sizeof(node*));
    *(arr) = $1;
    *(arr+1) = $2;
    *(arr+2) = $3;
    $$ = create_node(@$.first_line, "Exp", "NULL", 3, arr);
}
| Exp PLUS Exp 
{
    node** arr = (node**)malloc(3*sizeof(node*));
    *(arr) = $1;
    *(arr+1) = $2;
    *(arr+2) = $3;
    $$ = create_node(@$.first_line, "Exp", "NULL", 3, arr);
}
| Exp MINUS Exp 
{
    node** arr = (node**)malloc(3*sizeof(node*));
    *(arr) = $1;
    *(arr+1) = $2;
    *(arr+2) = $3;
    $$ = create_node(@$.first_line, "Exp", "NULL", 3, arr);
}
| Exp STAR Exp 
{
    node** arr = (node**)malloc(3*sizeof(node*));
    *(arr) = $1;
    *(arr+1) = $2;
    *(arr+2) = $3;
    $$ = create_node(@$.first_line, "Exp", "NULL", 3, arr);
}
| Exp DIV Exp 
{
    node** arr = (node**)malloc(3*sizeof(node*));
    *(arr) = $1;
    *(arr+1) = $2;
    *(arr+2) = $3;
    $$ = create_node(@$.first_line, "Exp", "NULL", 3, arr);
}
| LP Exp RP 
{
    node** arr = (node**)malloc(3*sizeof(node*));
    *(arr) = $1;
    *(arr+1) = $2;
    *(arr+2) = $3;
    $$ = create_node(@$.first_line, "Exp", "NULL", 3, arr);
}
| MINUS Exp 
{
    node** arr = (node**)malloc(2*sizeof(node*));
    *(arr) = $1;
    *(arr+1) = $2;
    $$ = create_node(@$.first_line, "Exp", "NULL", 2, arr);
}
| NOT Exp 
{
    node** arr = (node**)malloc(2*sizeof(node*));
    *(arr) = $1;
    *(arr+1) = $2;
    $$ = create_node(@$.first_line, "Exp", "NULL", 2, arr);
}
| ID LP Args RP 
{
    node** arr = (node**)malloc(4*sizeof(node*));
    *(arr) = $1;
    *(arr+1) = $2;
    *(arr+2) = $3;
    *(arr+3) = $4;
    $$ = create_node(@$.first_line, "Exp", "NULL", 4, arr);
}
| ID LP RP 
{
    node** arr = (node**)malloc(3*sizeof(node*));
    *(arr) = $1;
    *(arr+1) = $2;
    *(arr+2) = $3;
    $$ = create_node(@$.first_line, "Exp", "NULL", 3, arr);
}
| Exp LB Exp RB 
{
    node** arr = (node**)malloc(4*sizeof(node*));
    *(arr) = $1;
    *(arr+1) = $2;
    *(arr+2) = $3;
    *(arr+3) = $4;
    $$ = create_node(@$.first_line, "Exp", "NULL", 4, arr);
}
| Exp DOT ID 
{
    node** arr = (node**)malloc(3*sizeof(node*));
    *(arr) = $1;
    *(arr+1) = $2;
    *(arr+2) = $3;
    $$ = create_node(@$.first_line, "Exp", "NULL", 3, arr);
}
| ID 
{
    node** arr = (node**)malloc(sizeof(node*));
    *(arr) = $1;
    $$ = create_node(@$.first_line, "Exp", "NULL", 1, arr);
}
| INT 
{
    node** arr = (node**)malloc(sizeof(node*));
    *(arr) = $1;
    $$ = create_node(@$.first_line, "Exp", "NULL", 1, arr);
}
| FLOAT 
{
    node** arr = (node**)malloc(sizeof(node*));
    *(arr) = $1;
    $$ = create_node(@$.first_line, "Exp", "NULL", 1, arr);
}
| WRONG_FLOAT 
{
    node** arr = (node**)malloc(sizeof(node*));
    *(arr) = $1;
    $$ = create_node(@$.first_line, "Exp", "NULL", 1, arr);
}
;
Args : Exp COMMA Args 
{
    node** arr = (node**)malloc(3*sizeof(node*));
    *(arr) = $1;
    *(arr+1) = $2;
    *(arr+2) = $3;
    $$ = create_node(@$.first_line, "Args", "NULL", 3, arr);
}
| Exp 
{
    node** arr = (node**)malloc(sizeof(node*));
    *(arr) = $1;
    $$ = create_node(@$.first_line, "Args", "NULL", 1, arr);
}
;

%%

void yyerror(char* msg)
{
    printf("Error type B at Line %d: Syntax Error\n", yylineno);
}