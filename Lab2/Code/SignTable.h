#ifndef _SIGNTABLE_H_
#define _SIGNTABLE_H_

#include "tree.h"
#include <stdbool.h>
#include "semantic.h"

typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct FuncParameterType_* FuncParameterType;

union VarValue{
    int IntVal;
    float FloatVal;
};

typedef struct VarNode{
    Type type;
    char* name;
    struct VarNode* next;
}VarNode;

typedef struct ArrNode{
    Type type;
    char* name;
    struct ArrNode* next;
}ArrNode;

typedef struct StructNode{
    Type type;
    char* name;
    struct StructNode* next;
}StructNode;

typedef struct StructTypeNode{
    Type type;
    char* name;
    struct StructTypeNode* next;
}StructTypeNode;

typedef struct FuncParameterType_{
    Type type;
    FuncParameterType next;
}FuncParameterType_;

typedef struct FuncNode{
    Type type;
    char* name;
    bool defined;
    FuncParameterType ParaType;
    struct FuncNode* next;
}FuncNode;

typedef struct LineNumber{
    int line;
    struct LineNumber* next;
}LineNumber;

typedef struct UndefinedFuncNode{
    char* name;
    bool defined;
    struct LineNumber* LineNo;
    struct UndefinedFuncNode* next;
}UndefinedFuncNode;

extern VarNode* VarTable;

extern ArrNode* ArrTable;

extern StructNode* StructTable;

extern StructTypeNode* StructTypeTable;

extern FuncNode* FuncTable;

extern UndefinedFuncNode* UndefinedFuncTable;

void InitializeSignTable();

void NewVar(Type type, char* name);

void NewArr(Type type, char* name);

void NewStruct(Type type, char* name);

void NewStructType(Type type, char* name);

void NewFunc(Type type, char* name, FuncParameterType ParameterType, bool if_defined);

#endif