#ifndef _SEMANTIC_H_
#define _SEMANTIC_H_

#include "tree.h"
#include "SignTable.h"
#include <stdbool.h>

typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct FuncParameterType_* FuncParameterType;

typedef enum { Int, Float, ARRAY, STRUCTURE, FUNC } kind;

typedef struct Type_{
    kind type;
    struct{
        // 基本类型
        
        // 数组类型信息包括元素类型与数组大小构成
        struct { Type ElementType; int size; } array;
        // 结构体类型信息是一个链表
        char* StructTypeName;
        FieldList structure;
        // 函数类型信息是其返回值的类型
        Type return_type;
    } u;
}Type_;

typedef struct FieldList_{
    char* name; // 域的名字
    Type type; // 域的类型
    FieldList tail; // 下一个域
}FieldList_;

enum SemanticErrorType{
    Undefined_ID_Used,
    Undefined_Func_Called,
    Repeat_ID_Name,
    Repeat_Func_Defination,
    ASSIGNOP_Type_Unmatched,
    Rvalue_Error,
    OP_Type_Unmatched,
    Return_Type_Unmatched,
    Func_Call_Para_Unmatched,
    Operate_Others_As_Array,
    Operate_Variable_As_Func,
    Float_Index,
    Operate_Others_As_Struct,
    Undefined_Struct_Member_Used,
    Struct_Member_Repeated_Or_Initialized,
    Repeated_Struct_Name,
    Undefined_Struct,
    Undefined_Func_Stated,
    Func_Statement_Conflit
};

void SemanticAnalyse(const node* root);

void SemanticError(enum SemanticErrorType ErrorType, int line);

void traveler(const node* root);

void ExtDef(const node* root);

void ExtDecList(const node* root, Type type);

Type Specifier(const node* root);

Type StructSpecifier(const node* root);

void DefListInStruct(const node* root, FieldList* AllField);

void DefListInFunc(const node* root);

void DefInStruct(const node* root, FieldList* AllField);

void DefInFunc(const node* root);

void DecListInStruct(const node* root, Type type, FieldList* AllField);

void DecListInFunc(const node* root, Type type);

void DecInStruct(const node* root, Type type, FieldList* AllField);

void DecInFunc(const node* root, Type type);

bool judge_type(Type t1, Type t2);

char* VarDecForName(const node* root);

Type VarDecForType(const node* root, Type type);

Type Exp(const node* root);

bool judge_if_lvalue(const node* root);

Type FindIDType(const node* root);

bool Args(const node* root, FuncParameterType ParaType);

FuncParameterType VarList(const node* root, bool if_statement);

FuncParameterType ParamDec(const node* root, bool if_statement);

void FunDec(const node* root, Type type, bool if_defination);

void CompSt(const node* root, Type ReturnType);

void StmtList(const node* root, Type ReturnType);

void Stmt(const node* root, Type ReturnType);

bool judge_func_para(FuncParameterType p1, FuncParameterType p2);

bool judge_if_conflict(char* name, kind type, int line);

#endif