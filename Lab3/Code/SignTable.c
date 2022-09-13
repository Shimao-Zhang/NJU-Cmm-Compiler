#include "SignTable.h"

VarNode* VarTable = NULL;

ArrNode* ArrTable = NULL;

StructNode* StructTable = NULL;

StructTypeNode* StructTypeTable = NULL;

FuncNode* FuncTable = NULL;

UndefinedFuncNode* UndefinedFuncTable = NULL;

void InitializeSignTable()
{
    VarTable = NULL;
    ArrTable = NULL;
    StructTable = NULL;
    StructTypeTable = NULL;
    FuncTable = NULL;
    UndefinedFuncTable = NULL;
    Type ReadType = (Type)malloc(sizeof(struct Type_));
    ReadType->type = FUNC;
    ReadType->u.return_type = (Type)malloc(sizeof(struct Type_));
    ReadType->u.return_type->type = Int;
    NewFunc(ReadType, "read", NULL, true);
    Type WriteType = (Type)malloc(sizeof(struct Type_));
    WriteType->type = FUNC;
    WriteType->u.return_type = Int;
    FuncParameterType WritePara = (FuncParameterType)malloc(sizeof(struct FuncParameterType_));
    WritePara->next = NULL;
    WritePara->type = (Type)malloc(sizeof(struct Type_));
    WritePara->type->type = Int;
    NewFunc(WriteType, "write", WritePara, true);
}

void NewVar(Type type, char* name)
{
    //printf("call NewVar\n");
    VarNode* p = VarTable;
    VarNode* node = (VarNode*)malloc(sizeof(VarNode));
    node->type = type;
    node->name = (char*)malloc(strlen(name)*sizeof(char));
    strcpy(node->name, name);
    node->next = NULL;
    if(p!=NULL)
    {
        while(p->next!=NULL)
        {
            p = p->next;
        }
        p->next = node;
    }
    else
    {
        VarTable = node;
    }
}

void NewArr(Type type, char* name)
{
    ArrNode* p = ArrTable;
    ArrNode* node = (ArrNode*)malloc(sizeof(ArrNode));
    node->type = type;
    node->name = (char*)malloc(strlen(name)*sizeof(char));
    strcpy(node->name, name);
    node-> next = NULL;
    if(p!=NULL)
    {
        while(p->next!=NULL)
        {
            p = p->next;
        }
        p->next = node;
    }
    else
    {
        ArrTable = node;
    }
}

void NewStruct(Type type, char* name)
{
    StructNode* p = StructTable;
    StructNode* node = (StructNode*)malloc(sizeof(StructNode));
    node->type = type;
    node->name = (char*)malloc(strlen(name)*sizeof(char));
    strcpy(node->name, name);
    node-> next = NULL;
    if(p!=NULL)
    {
        while(p->next!=NULL)
        {
            p = p->next;
        }
        p->next = node;
    }
    else
    {
        StructTable = node;
    }
}

void NewStructType(Type type, char* name)
{
    StructTypeNode* p = StructTypeTable;
    StructTypeNode* node = (StructTypeNode*)malloc(sizeof(StructTypeNode));
    node->type = type;
    node->name = (char*)malloc(strlen(name)*sizeof(char));
    strcpy(node->name, name);
    node-> next = NULL;
    if(p!=NULL)
    {
        while(p->next!=NULL)
        {
            p = p->next;
        }
        p->next = node;
    }
    else
    {
        StructTypeTable = node;
    }
}

void NewFunc(Type type, char* name, FuncParameterType ParameterType, bool if_defined)
{
    //printf("call NewFunc, name:%s\n", name);
    FuncNode* p = FuncTable;
    FuncNode* node = (FuncNode*)malloc(sizeof(FuncNode));
    node->type = type;
    node->name = (char*)malloc(strlen(name)*sizeof(char));
    strcpy(node->name, name);
    node->ParaType = ParameterType;
    node->defined = if_defined;
    node->next = NULL;
    if(p!=NULL)
    {
        while(p->next!=NULL)
        {
            p = p->next;
        }
        p->next = node;
    }
    else
    {
        FuncTable = node;
    }
}