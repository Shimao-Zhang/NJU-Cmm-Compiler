%{
    #define YYSTYPE node*
    #include "tree.h"
	#include "syntax.tab.h"
    int yycolumn = 1;
    extern int mistakes;
    #define YY_USER_ACTION \
        yylloc.first_line = yylloc.last_line = yylineno; \
        yylloc.first_column = yycolumn; \
        yylloc.last_column = yycolumn + yyleng - 1; \
        yycolumn += yyleng;
%}
%option yylineno

digit [0-9]
spaces [ ]
INT [1-9]{digit}*|[0]
DOT "."
FLOAT {digit}+{DOT}{digit}+|(({digit}+{DOT}{digit}+|{digit}+{DOT}|{DOT}{digit}+)[Ee][+-]?{digit}+)
WRONG_FLOAT ({digit}+{DOT}{digit}+|{digit}+{DOT}|{DOT}{digit}+)[Ee]
ID [a-zA-Z_]+[a-zA-Z_0-9]*
SEMI ";"
COMMA ","
ASSIGNOP "="
RELOP ">"|"<"|">="|"<="|"=="|"!="
PLUS "+"
MINUS "-"
STAR "*"
DIV "/"
AND "&&"
OR "||"
NOT "!"
TYPE "int"|"float"
LP "("
RP ")"
LB "["
RB "]"
LC "{"
RC "}"
STRUCT "struct"
RETURN "return"
IF "if"
ELSE "else"
WHILE "while"
NEWLINE [\n\r]
TAB [\t]
%%
{spaces} {}
{TAB} { yycolumn += 3; }
{INT} { 
    yylval = create_node(yylineno, "INT", yytext, 0, NULL);
    return INT; }
{FLOAT} { 
    yylval = create_node(yylineno, "FLOAT", yytext, 0, NULL);
    return FLOAT; }
{TYPE} { 
    yylval = create_node(yylineno, "TYPE", yytext, 0, NULL);
    return TYPE; }
{SEMI} { 
    yylval = create_node(yylineno, "SEMI", yytext, 0, NULL);
    return SEMI; }
{COMMA} { 
    yylval = create_node(yylineno, "COMMA", yytext, 0, NULL);
    return COMMA; }
{RELOP} { 
    yylval = create_node(yylineno, "RELOP", yytext, 0, NULL);
    return RELOP; }
{ASSIGNOP} { 
    yylval = create_node(yylineno, "ASSIGNOP", yytext, 0, NULL);
    return ASSIGNOP; }
{PLUS} { 
    yylval = create_node(yylineno, "PLUS", yytext, 0, NULL);
    return PLUS; }
{MINUS} { 
    yylval = create_node(yylineno, "MINUS", yytext, 0, NULL);
    return MINUS; }
{STAR} { 
    yylval = create_node(yylineno, "STAR", yytext, 0, NULL);
    return STAR; }
{DIV} { 
    yylval = create_node(yylineno, "DIV", yytext, 0, NULL);
    return DIV; }
{AND} { 
    yylval = create_node(yylineno, "AND", yytext, 0, NULL);
    return AND; }
{OR} { 
    yylval = create_node(yylineno, "OR", yytext, 0, NULL);
    return OR; }
{NOT} { 
    yylval = create_node(yylineno, "NOT", yytext, 0, NULL);
    return NOT; }
{LP} { 
    yylval = create_node(yylineno, "LP", yytext, 0, NULL);
    return LP; }
{RP} { 
    yylval = create_node(yylineno, "RP", yytext, 0, NULL);
    return RP; }
{LB} { 
    yylval = create_node(yylineno, "LB", yytext, 0, NULL);
    return LB; }
{RB} { 
    yylval = create_node(yylineno, "RB", yytext, 0, NULL);
    return RB; }
{LC} { 
    yylval = create_node(yylineno, "LC", yytext, 0, NULL);
    return LC; }
{RC} { 
    yylval = create_node(yylineno, "RC", yytext, 0, NULL);
    return RC; }
{STRUCT} { 
    yylval = create_node(yylineno, "STRUCT", yytext, 0, NULL);
    return STRUCT; }
{RETURN} { 
    yylval = create_node(yylineno, "RETURN", yytext, 0, NULL);
    return RETURN; }
{IF} { 
    yylval = create_node(yylineno, "IF", yytext, 0, NULL);
    return IF; }
{ELSE} { 
    yylval = create_node(yylineno, "ELSE", yytext, 0, NULL);
    return ELSE; }
{WHILE} { 
    yylval = create_node(yylineno, "WHILE", yytext, 0, NULL);
    return WHILE; }
{DOT} { 
    yylval = create_node(yylineno, "DOT", yytext, 0, NULL);
    return DOT; }
{NEWLINE} { yycolumn = 1; }
{ID} { 
    yylval = create_node(yylineno, "ID", yytext, 0, NULL);
    return ID; }
{WRONG_FLOAT} {
    mistakes = 1;
    printf("Error type A at Line %d: Illegal floating point number \'%s\'\n", yylineno, yytext); 
    yylval = NULL;
    return WRONG_FLOAT; }
. { 
    mistakes = 1;
    printf("Error type A at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext); }
%%