#ifndef _INTERMEDIATE_H_
#define _INTERMEDIATE_H_

#include "tree.h"
#include "SignTable.h"
#include "semantic.h"

typedef struct Operand_* Operand;
typedef struct InterCodes_* InterCodes;
typedef struct TempVarCorr_* TempVarCorr;
typedef struct VarVarCorr_* VarVarCorr;
typedef struct ArgsNode_* ArgsNode;

struct Operand_{
    enum { VARIABLE, TEMP, CONSTANT, OPLABEL, OPFUNC, ARGSTRUCT } kind;
    union {
        int var_no;
        int value;
        char* name;
    } u;
    bool IfPara;
};

struct VarVarCorr_{
    Operand var;
    char* VarName;
    struct VarVarCorr_ *next;
};

struct InterCode{
    enum { ASSIGN, GET_ADDR_ASSIGN, GET_ADDR_CONTENT_ASSIGN, ASSIGN_TO_ADDR, ADD, SUB, MUL, ICDIV, 
            LABEL, ICFUNC, IFGOTO, GOTO, ICRETURN, DEC, ARG, CALL, PARAM, READ, WRITE } kind;
    union {
        struct { Operand op; } single;
        struct { Operand left, right; } assign;
        struct { Operand result, op1; } OneOp;
        struct { Operand result, op1, op2; } TwoOp;
        struct { Operand op1, op2, label; char* relop; } ThreeOp;
        struct { Operand op; int size; } Dec;
    } u;
};

struct InterCodes_{
    struct InterCode code;
    struct InterCodes_ *prev, *next;
};

struct ArgsNode_{
    Operand args;
    struct ArgsNode_ *next;
};

extern int VarNo;
extern int TempNo;
extern int LabelNo;
extern InterCodes InterCodeListHead;
extern InterCodes InterCodeListTail;
extern VarVarCorr VarVarCorrTable;

void AddInterCodes(InterCodes head);

void PrintOperand(Operand op, FILE *f);

void PrintInterCodes(InterCodes p, FILE *f);

void PrintAllInterCodes(FILE *f);

Operand new_temp();

Operand new_var(char* VarName, bool IfPara);

Operand FindVar(char* VarName);

Operand new_label();

InterCodes ConnectCodes(InterCodes code1, InterCodes code2);

InterCodes NewAssignInterCodes(Operand left, Operand right, int kind);

int GetStructTypeSize(char* name);

int GetOffset(char* StructName, char* FieldName);

int GetOffsetByStructTypeName(char* StructTypeName, char* FieldName);

InterCodes translate_Exp(const node* root, Operand place);

InterCodes translate_Args(const node* root,  ArgsNode* pArgsList);

InterCodes translate_Stmt(const node* root);

InterCodes translate_CompSt(const node* root);

InterCodes translate_DefList(const node* root);

InterCodes translate_Def(const node* root);

InterCodes translate_DecList_Struct(const node* root, char* StructTypeName);

InterCodes translate_DecList(const node* root);

InterCodes translate_Dec_Struct(const node* root, char* StructTypeName);

InterCodes translate_Dec(const node* root);

InterCodes translate_VarDecWithAssign(const node* root, const node* Exp);

InterCodes translate_StmtList(const node* root);

InterCodes translate_Cond(const node* root, Operand LabelTrue, Operand LabelFalse);

InterCodes translate_ExtDef(const node* root);

InterCodes translate_VarList(const node* root);

InterCodes translate_ParamDec(const node* root);

InterCodes translate_BaseAddr(const node* root, Operand place);

void TranslateToInterCodes(const node* root);

#endif