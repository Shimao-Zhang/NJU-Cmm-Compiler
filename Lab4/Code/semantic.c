#include <stdio.h>
#include <stdlib.h>
#include "semantic.h"
#include <assert.h>

void SemanticAnalyse(const node* root)
{
    InitializeSignTable();
    if(strcmp(root->type,"Program")==0)
        traveler(root);
    UndefinedFuncNode* p = UndefinedFuncTable;
    while(p!=NULL)
    {
        if(!p->defined)
        {
            LineNumber* move = p->LineNo;
            while(move!=NULL)
            {
                SemanticError(Undefined_Func_Stated, move->line);
                move = move->next;
            }
        }
        p = p->next;
    }
}

void SemanticError(enum SemanticErrorType ErrorType, int line)
{
    switch(ErrorType)
    {
        case Undefined_ID_Used:
        {
            printf("Error type 1 at Line %d: Variable is used but haven't been defined.\n", line);
            break;
        }
        case Undefined_Func_Called:
        {
            printf("Error type 2 at Line %d: Function is called but haven't been defined.\n", line);
            break;
        }
        case Repeat_ID_Name:
        {
            printf("Error type 3 at Line %d: Variable is defined more than once.\n", line);
            break;
        }
        case Repeat_Func_Defination:
        {
            printf("Error type 4 at Line %d: Function is defined more than once.\n", line);
            break;
        }
        case ASSIGNOP_Type_Unmatched:
        {
            printf("Error type 5 at Line %d: The expression types on both sides of the ASSIGNOP do not match.\n", line);
            break;
        }
        case Rvalue_Error:
        {
            printf("Error type 6 at Line %d: Only a rvalue-expression appears to the left of an ASSIGNOP.\n", line);
            break;
        }
        case OP_Type_Unmatched:
        {
            printf("Error type 7 at Line %d: Operand types are mismatched or operand type and operator are mismatched.\n", line);
            break;
        }
        case Return_Type_Unmatched:
        {
            printf("Error type 8 at Line %d: Return type is mismatched.\n", line);
            break;
        }
        case Func_Call_Para_Unmatched:
        {
            printf("Error type 9 at Line %d: Parameters are mismatched when calling the function.\n", line);
            break;
        }
        case Operate_Others_As_Array:
        {
            printf("Error type 10 at Line %d: Operate other types of operand as array.\n", line);
            break;
        }
        case Operate_Variable_As_Func:
        {
            printf("Error type 11 at Line %d: Operate other types of operand as function.\n", line);
            break;
        }
        case Float_Index:
        {
            printf("Error type 12 at Line %d: Use float as an array index.\n", line);
            break;
        }
        case Operate_Others_As_Struct:
        {
            printf("Error type 13 at Line %d: Operate other types of operand as structure.\n", line);
            break;
        }
        case Undefined_Struct_Member_Used:
        {
            printf("Error type 14 at Line %d: Visit an unexist structure field.\n", line);
            break;
        }
        case Struct_Member_Repeated_Or_Initialized:
        {
            printf("Error type 15 at Line %d: Structure field is defined more than once or initialized.\n", line);
            break;
        }
        case Repeated_Struct_Name:
        {
            printf("Error type 16 at Line %d: The name of the structure duplicates the name of a previously defined structure or variable.\n", line);
            break;
        }
        case Undefined_Struct:
        {
            printf("Error type 17 at Line %d: Structure is used but haven't been defined.\n", line);
            break;
        }
        case Undefined_Func_Stated:
        {
            printf("Error type 18 at Line %d: Function haven't been defined but stated.\n", line);
            break;
        }
        case Func_Statement_Conflit:
        {
            printf("Error type 19 at Line %d: Function's statement conflit with defination or other statements.\n", line);
            break;
        }
        default:
        {
            break;
        }
    }
}

void traveler(const node* root)
{
    if(root==NULL)
    {
        return;
    }
    for(int i=0; i<root->childnum; ++i)
    {
        if(strcmp(root->childarr[i]->type, "ExtDef")==0)
        {
            ExtDef(root->childarr[i]);
        }
        else
        {
            traveler(root->childarr[i]);
        }
    }
}

void ExtDef(const node* root)
{
    //printf("call ExtDef\n");
    node* spe = root->childarr[0];
    if(root->childnum==2)
    {
        Specifier(spe);
    }
    else if(root->childnum==3 && strcmp(root->childarr[1]->type, "ExtDecList")==0)
    {
        Type t = Specifier(spe);
        if(t!=NULL)
        {
            ExtDecList(root->childarr[1], t);
        }
    }
    else if(root->childnum==3 && strcmp(root->childarr[1]->type, "FunDec")==0)
    {
        if(strcmp(root->childarr[2]->value, ";")==0)
        {
            Type t = Specifier(spe);
            if(t!=NULL)
            {
                FunDec(root->childarr[1], t, false);
            }
        }
        else if(strcmp(root->childarr[2]->type, "CompSt")==0)
        {
            Type t = Specifier(spe);
            if(t!=NULL)
            {
                FunDec(root->childarr[1], t, true);
            }
            CompSt(root->childarr[2], t);
        }
    }
}

void ExtDecList(const node* root, Type type)
{
    //printf("call ExtDecList\n");
    if(root->childnum==1)
    {
        char* name = VarDecForName(root->childarr[0]);
        Type t = VarDecForType(root->childarr[0], type);
        if(t->type==Int)
        {
            judge_if_conflict(name, Int, root->lineno);
            NewVar(t, name);
        }
        else if(t->type==Float)
        {
            judge_if_conflict(name, Float, root->lineno);
            NewVar(t, name);
        }
        else if(t->type==ARRAY)
        {
            judge_if_conflict(name, ARRAY, root->lineno);
            NewArr(t, name);
        }
        else if(t->type==STRUCTURE)
        {
            judge_if_conflict(name, STRUCTURE, root->lineno);
            NewStruct(t, name);
        }
    }
    else if(root->childnum==3)
    {
        char* name = VarDecForName(root->childarr[0]);
        Type t = VarDecForType(root->childarr[0], type);
        if(t->type==Int)
        {
            judge_if_conflict(name, Int, root->lineno);
            NewVar(t, name);
        }
        else if(t->type==Float)
        {
            judge_if_conflict(name, Float, root->lineno);
            NewVar(t, name);
        }
        else if(t->type==ARRAY)
        {
            judge_if_conflict(name, ARRAY, root->lineno);
            NewArr(t, name);
        }
        else if(t->type==STRUCTURE)
        {
            judge_if_conflict(name, STRUCTURE, root->lineno);
            NewStruct(t, name);
        }
        ExtDecList(root->childarr[2], type);
    }
}

Type Specifier(const node* root)
{
    //printf("call Specifier\n");
    if(strcmp(root->childarr[0]->type, "StructSpecifier")==0)
    {
        node* struspe = root->childarr[0];
        Type type = StructSpecifier(struspe);
        return type;
    }
    else if(strcmp(root->childarr[0]->type, "TYPE")==0)
    {
        Type type = (Type)malloc(sizeof(Type_));
        if(strcmp(root->childarr[0]->value, "int")==0)
        {
            type->type = Int;
        }
        else if(strcmp(root->childarr[0]->value, "float")==0)
        {
            type->type = Float;
        }
        return type;
    }
}

Type StructSpecifier(const node* root)
{
    //printf("call StructSpecifier\n");
    if(root->childnum==2)
    {
        StructTypeNode* p = StructTypeTable;
        while(p!=NULL)
        {
            if(strcmp(p->name, root->childarr[1]->childarr[0]->value)==0)
            {
                break;
            }
            p = p->next;
        }
        if(p!=NULL)
        {
            return p->type;
        }
        else
        {
            SemanticError(Undefined_Struct, root->lineno);
            return NULL;
        }
    }
    else if(root->childnum==5)
    {
        Type t = (Type)malloc(sizeof(Type_));
        t->type = STRUCTURE;
        t->u.structure = NULL;
        t->u.StructTypeName = (char*)malloc((strlen(root->childarr[1]->childarr[0]->value)+1)*sizeof(char));
        strcpy(t->u.StructTypeName, root->childarr[1]->childarr[0]->value);
        DefListInStruct(root->childarr[3], &t->u.structure);
        StructTypeNode* p1 = StructTypeTable;
        while(p1!=NULL)
        {
            if(strcmp(p1->name, root->childarr[1]->childarr[0]->value)==0)
            {
                SemanticError(Repeated_Struct_Name, root->childarr[1]->lineno);
                return NULL;
            }
            p1 = p1->next;
        }
        VarNode* p2 = VarTable;
        while(p2!=NULL)
        {
            if(strcmp(root->childarr[1]->childarr[0]->value, p2->name)==0)
            {
                SemanticError(Repeated_Struct_Name, root->lineno);
                return NULL;
            }
            p2 = p2->next;
        }
        ArrNode* p3 = ArrTable;
        while(p3!=NULL)
        {
            if(strcmp(root->childarr[1]->childarr[0]->value, p3->name)==0)
            {
                SemanticError(Repeated_Struct_Name, root->lineno);
                return NULL;
            }
            p3 = p3->next;
        }
        StructNode* p4 = StructTable;
        while(p4!=NULL)
        {
            if(strcmp(root->childarr[1]->childarr[0]->value, p4->name)==0)
            {
                SemanticError(Repeated_Struct_Name, root->lineno);
                return NULL;
            }
            FieldList p = p4->type->u.structure;
            while(p!=NULL)
            {
                if(strcmp(p->name, root->childarr[1]->childarr[0]->value)==0)
                {
                    SemanticError(Repeated_Struct_Name, root->lineno);
                    return NULL;
                }
                p = p->tail;
            }
            p4 = p4->next;
        }
        StructTypeNode* p5 = StructTypeTable;
        while(p5!=NULL)
        {
            if(strcmp(root->childarr[1]->childarr[0]->value, p5->name)==0)
            {
                SemanticError(Repeated_Struct_Name, root->lineno);
                return NULL;
            }
            p5 = p5->next;
        }
        if(p1==NULL && p2==NULL && p3==NULL && p4==NULL && p5==NULL)
        {
            if(strcmp(t->u.StructTypeName, "NULL")!=0)
            {
                NewStructType(t, root->childarr[1]->childarr[0]->value);
            }
            return t;
        }
    }
}

void DefListInStruct(const node* root, FieldList* AllField)
{
    //printf("call DefListInStruct\n");
    if(root->childnum==1)
    {
        return;
    }
    else if(root->childnum==2)
    {
        DefInStruct(root->childarr[0], AllField);
        DefListInStruct(root->childarr[1], AllField);
    }
}

void DefListInFunc(const node* root)
{
    if(root->childnum==1)
    {
        return;
    }
    else if(root->childnum==2)
    {
        DefInFunc(root->childarr[0]);
        DefListInFunc(root->childarr[1]);
    }
}

void DefInStruct(const node* root, FieldList* AllField)
{
    node* spe = root->childarr[0];
    Type t = (Type)malloc(sizeof(Type_));
    if(strcmp(spe->childarr[0]->type, "TYPE")==0)
    {
        if(strcmp(spe->childarr[0]->value, "int")==0)
        {
            t->type = Int;
        }
        else if(strcmp(spe->childarr[0]->value, "float")==0)
        {
            t->type = Float;
        }
    }
    else if(strcmp(spe->childarr[0]->type, "StructSpecifier")==0)
    {
        t = StructSpecifier(spe->childarr[0]);
    }
    DecListInStruct(root->childarr[1], t, AllField);
}

void DefInFunc(const node* root)
{
    node* spe = root->childarr[0];
    Type t = (Type)malloc(sizeof(Type_));
    if(strcmp(spe->childarr[0]->type, "TYPE")==0)
    {
        if(strcmp(spe->childarr[0]->value, "int")==0)
        {
            t->type = Int;
        }
        else if(strcmp(spe->childarr[0]->value, "float")==0)
        {
            t->type = Float;
        }
    }
    else if(strcmp(spe->childarr[0]->type, "StructSpecifier")==0)
    {
        t = StructSpecifier(spe->childarr[0]);
    }
    DecListInFunc(root->childarr[1], t);
}

void DecListInStruct(const node* root, Type type, FieldList* AllField)
{
    if(root->childnum==1)
    {
        DecInStruct(root->childarr[0], type, AllField);
        return;
    }
    else if(root->childnum==3)
    {
        DecInStruct(root->childarr[0], type, AllField);
        DecListInStruct(root->childarr[2], type, AllField);
        return;
    }
    return;
}

void DecListInFunc(const node* root, Type type)
{
    if(root->childnum==1)
    {
        DecInFunc(root->childarr[0], type);
        return;
    }
    else if(root->childnum==3)
    {
        DecInFunc(root->childarr[0], type);
        DecListInFunc(root->childarr[2], type);
        return;
    }
    return;
}

void DecInStruct(const node* root, Type type, FieldList* AllField)
{
    if(root->childnum==3)
    {
        SemanticError(Struct_Member_Repeated_Or_Initialized, root->lineno);
        FieldList p = (FieldList)malloc(sizeof(FieldList_));
        p->type = VarDecForType(root->childarr[0], type);
        p->tail = NULL;
        p->name = (char*)malloc((strlen(VarDecForName(root->childarr[0]))+1)*sizeof(char));
        strcpy(p->name, VarDecForName(root->childarr[0]));

        /*judge if there is any name conflict*/
        FieldList judge = *AllField;
        while(judge!=NULL)
        {
            if(strcmp(judge->name, p->name)==0)
            {
                SemanticError(Struct_Member_Repeated_Or_Initialized, root->lineno);
            }
            judge = judge->tail;
        }
        if(p->type==NULL)
        {
            return;////////////////////////////////?????????????????????????????
        }
        judge_if_conflict(p->name, p->type->type, root->childarr[0]->lineno);

        FieldList move = *AllField;
        if(*AllField==NULL)
        {
            *AllField = p;
        }
        else
        {
            while(move->tail!=NULL)
            {
                move = move->tail;
            }
            move->tail = p;
        }
    }
    else if(root->childnum==1)
    {
        FieldList p = (FieldList)malloc(sizeof(FieldList_));
        p->type = VarDecForType(root->childarr[0], type);
        p->tail = NULL;
        p->name = (char*)malloc((strlen(VarDecForName(root->childarr[0]))+1)*sizeof(char));
        strcpy(p->name, VarDecForName(root->childarr[0]));
        /*judge if there is any name conflict*/
        FieldList judge = *AllField;
        while(judge!=NULL)
        {
            if(strcmp(judge->name, p->name)==0)
            {
                SemanticError(Struct_Member_Repeated_Or_Initialized, root->lineno);
            }
            judge = judge->tail;
        }
        if(p->type==NULL)
        {
            return;  ///////////////???????????????????????????????????
        }
        judge_if_conflict(p->name, p->type->type, root->childarr[0]->lineno);
        FieldList move = *AllField;
        if(*AllField==NULL)
        {
            *AllField = p;
        }
        else
        {
            while(move->tail!=NULL)
            {
                move = move->tail;
            }
            move->tail = p;
        }
    }
}

void DecInFunc(const node* root, Type type)
{
    if(root->childnum==1)
    {
        Type t = VarDecForType(root->childarr[0], type);
        char* name = VarDecForName(root->childarr[0]);
        if(t==NULL)
        {
            return;     ///////////////////////////////////
        }
        if(t->type==Int)
        {
            if(!judge_if_conflict(name, Int, root->lineno))
            {
                NewVar(t, name);
            }
        }
        else if(t->type==Float)
        {
            if(!judge_if_conflict(name, Float, root->lineno))
            {
                NewVar(t, name);
            }
        }
        else if(t->type==ARRAY)
        {
            if(!judge_if_conflict(name, ARRAY, root->lineno))
            {
                NewArr(t, name);
            }
        }
        else if(t->type==STRUCTURE)
        {
            if(!judge_if_conflict(name, STRUCTURE, root->lineno))
            {
                NewStruct(t, name);
            }
        }
    }
    else if(root->childnum==3)
    {
        Type t = VarDecForType(root->childarr[0], type);
        char* name = VarDecForName(root->childarr[0]);
        if(!judge_type(t, Exp(root->childarr[2])))
        {
            SemanticError(ASSIGNOP_Type_Unmatched, root->lineno);
        }
        if(t==NULL)
        {
            return;  ///////////////////////////////
        }
        if(t->type==Int)
        {
            if(!judge_if_conflict(name, Int, root->lineno))
            {
                NewVar(t, name);
            }
        }
        else if(t->type==Float)
        {
            if(!judge_if_conflict(name, Float, root->lineno))
            {
                NewVar(t, name);
            }
        }
        else if(t->type==ARRAY)
        {
            if(!judge_if_conflict(name, ARRAY, root->lineno))
            {
                NewArr(t, name);
            }
        }
        else if(t->type==STRUCTURE)
        {
            if(!judge_if_conflict(name, STRUCTURE, root->lineno))
            {
                NewStruct(t, name);
            }
        }
    }
}

bool judge_type(Type t1, Type t2)
{
    //printf("call judge_type\n");
    if(t1==NULL || t2==NULL)
    {
        return false;
    }
    if(t1->type!=t2->type)
    {
        return false;
    }
    else if(t1->type==t2->type)
    {
        if(t1->type==Int || t1->type==Float)
        {
            return true;
        }
        else if(t1->type==ARRAY)
        {
            Type move1 = t1->u.array.ElementType;
            Type move2 = t2->u.array.ElementType;
            while(move1!=NULL && move2!=NULL)
            {
                if((move1->type==Int || move1->type==Float) && move2->type==ARRAY)
                {
                    return false;
                }
                else if(move1->type==ARRAY && (move2->type==Int || move2->type==Float))
                {
                    return false;
                }
                else
                {
                    if(move1->type!=move2->type)
                    {
                        return false;
                    }
                }
                move1 = move1->u.array.ElementType;
                move2 = move2->u.array.ElementType;
            }
            if(move1==NULL && move2==NULL)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else if(t1->type==STRUCTURE)
        {
            if(strcmp(t1->u.StructTypeName, t2->u.StructTypeName)==0)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }
    return false;
}

char* VarDecForName(const node* root)
{
    node* p = root;
    while(strcmp(p->childarr[0]->type, "ID")!=0)
    {
        p = p->childarr[0];
    }
    //printf("call VarDecForName, name:%s\n", p->childarr[0]->value);
    return p->childarr[0]->value;
}

Type VarDecForType(const node* root, Type type)
{
    //printf("call VarDecForType\n");
    Type t;
    if(root->childnum==1)
    {
        t = type;
    }
    else if(root->childnum==4)
    {
        Type newnode = (Type)malloc(sizeof(Type_));
        newnode->type = ARRAY;
        newnode->u.array.ElementType = type;
        newnode->u.array.size = atoi(root->childarr[2]->value);
        t = VarDecForType(root->childarr[0], newnode);
    }
    return t;
}

Type Exp(const node* root)
{
    //printf("call Exp, line %d\n", root->childarr[0]->lineno);
    Type type = (Type)malloc(sizeof(Type_));
    if(root->childnum==1)
    {
        if(strcmp(root->childarr[0]->type, "INT")==0)
        {
            //printf("Int: %s\n", root->childarr[0]->value);
            type->type = Int;
        }
        else if(strcmp(root->childarr[0]->type, "FLOAT")==0)
        {
            type->type = Float;
        }
        else if(strcmp(root->childarr[0]->type, "ID")==0)
        {
            //printf("ID:%s,line %d\n", root->childarr[0]->value, root->childarr[0]->lineno);
            type = FindIDType(root->childarr[0]);
            if(type==NULL)
            {
                SemanticError(Undefined_ID_Used, root->lineno);
            }
        }
    }
    else if(root->childnum==2)
    {
        if(strcmp(root->childarr[0]->type, "MINUS")==0)
        {
            Type t = Exp(root->childarr[1]);
            if(t==NULL)
            {
                SemanticError(OP_Type_Unmatched, root->lineno);
                type = NULL;
            }
            else if(t->type==Int || t->type==Float)
            {
                type = t;
            }
            else
            {
                SemanticError(OP_Type_Unmatched, root->lineno);
                type = NULL;
            }
        }
        else if(strcmp(root->childarr[0]->type, "NOT")==0)
        {
            Type t = Exp(root->childarr[1]);
            if(t==NULL)
            {
                SemanticError(OP_Type_Unmatched, root->lineno);
                type = NULL;
            }
            else if(t->type==Int)
            {
                type->type = Int;
            }
            else
            {
                SemanticError(OP_Type_Unmatched, root->lineno);
                type = NULL;
            }
        }
    }
    else if(root->childnum==3)
    {
        if(strcmp(root->childarr[1]->type, "ASSIGNOP")==0)
        {
            if(judge_type(Exp(root->childarr[0]), Exp(root->childarr[2])))
            {
                if(!judge_if_lvalue(root->childarr[0]))
                {
                    SemanticError(Rvalue_Error, root->lineno);
                    type = NULL;
                }
                else
                {
                    type = Exp(root->childarr[0]);
                }
            }
            else
            {
                SemanticError(ASSIGNOP_Type_Unmatched, root->lineno);
                type = NULL;
            }
        }
        else if(strcmp(root->childarr[1]->type, "AND")==0 || strcmp(root->childarr[1]->type, "OR")==0)
        {
            Type t0 = Exp(root->childarr[0]);
            Type t2 = Exp(root->childarr[2]);
            if(t0==NULL || t2==NULL)
            {
                SemanticError(OP_Type_Unmatched, root->lineno);
                type = NULL;
            }
            else if(t0->type==Int && t2->type==Int)
            {
                type->type = Int;
            }
            else
            {
                SemanticError(OP_Type_Unmatched, root->lineno);
                type = NULL;
            }
        }
        else if(strcmp(root->childarr[1]->type, "RELOP")==0)
        {
            Type t0 = Exp(root->childarr[0]);
            Type t2 = Exp(root->childarr[2]);
            if(t0==NULL || t2==NULL)
            {
                SemanticError(OP_Type_Unmatched, root->lineno);
                type = NULL;
            }
            else if(t0->type==Int && t2->type==Int)
            {
                type->type = Int;
            }
            else if(t0->type==Float && t2->type==Float)
            {
                type->type = Int;
            }
            else
            {
                SemanticError(OP_Type_Unmatched, root->lineno);
                type = NULL;
            }
        }
        else if(strcmp(root->childarr[1]->type, "PLUS")==0 || strcmp(root->childarr[1]->type, "MINUS")==0 || strcmp(root->childarr[1]->type, "STAR")==0)
        {
            Type t0 = Exp(root->childarr[0]);
            Type t2 = Exp(root->childarr[2]);
            if(t0==NULL || t2==NULL)
            {
                SemanticError(OP_Type_Unmatched, root->lineno);
                type = NULL;
            }
            else if(judge_type(t0, t2))
            {
                if(t0->type==Int)
                {
                    type->type = Int;
                }
                else if(t0->type==Float)
                {
                    type->type = Float;
                }
                else
                {
                    SemanticError(OP_Type_Unmatched, root->lineno);
                    type = NULL;
                }
            }
            else
            {
                SemanticError(OP_Type_Unmatched, root->childarr[0]->lineno);
                type = NULL;
            }
        }
        else if(strcmp(root->childarr[1]->type, "Exp")==0)
        {
            type = Exp(root->childarr[1]);
        }
        else if(strcmp(root->childarr[1]->type, "LP")==0)
        {
            if(FindIDType(root->childarr[0])==NULL)
            {
                SemanticError(Undefined_Func_Called, root->lineno);
                type = NULL;
            }
            else if(FindIDType(root->childarr[0])->type!=FUNC)
            {
                SemanticError(Operate_Variable_As_Func, root->lineno);
                type = NULL;
            }
            else
            {
                type = FindIDType(root->childarr[0])->u.return_type;
            }
        }
        else if(strcmp(root->childarr[1]->type, "DOT")==0)
        {
            Type t = Exp(root->childarr[0]);
            if(t==NULL)
            {
                SemanticError(Operate_Others_As_Struct, root->lineno);
                type = NULL;
            }
            else if(t->type!=STRUCTURE)
            {
                SemanticError(Operate_Others_As_Struct, root->lineno);
                type = NULL;
            }
            else
            {
                FieldList p = t->u.structure;
                while(p!=NULL)
                {
                    if(strcmp(p->name, root->childarr[2]->value)==0)
                    {
                        type = p->type;
                        break;
                    }
                    p = p->tail;
                }
                if(p==NULL)
                {
                    SemanticError(Undefined_Struct_Member_Used, root->lineno);
                    type = NULL;
                }
            }
        }
    }
    else if(root->childnum==4)
    {
        if(strcmp(root->childarr[0]->type, "ID")==0)
        {
            if(FindIDType(root->childarr[0])==NULL)
            {
                SemanticError(Undefined_Func_Called, root->lineno);
                type = NULL;
            }
            else if(FindIDType(root->childarr[0])->type!=FUNC)
            {
                SemanticError(Operate_Variable_As_Func, root->lineno);
                type = NULL;
            }
            else
            {
                FuncNode* pmove = FuncTable;
                while(pmove!=NULL)
                {
                    if(strcmp(root->childarr[0]->value, pmove->name)==0)
                    {
                        if(!pmove->defined)
                        {
                            SemanticError(Undefined_Func_Called, root->lineno);
                        }
                        break;
                    }
                    pmove = pmove->next;
                }
                FuncNode* p = FuncTable;
                FuncParameterType ParaType = NULL;
                while(p!=NULL)
                {
                    if(strcmp(root->childarr[0]->value, p->name)==0)
                    {
                        ParaType = p->ParaType;
                        break;
                    }
                    p = p->next;
                }
                if(ParaType!=NULL)
                {
                    if(!Args(root->childarr[2], ParaType))
                    {
                        SemanticError(Func_Call_Para_Unmatched, root->lineno);
                        type = FindIDType(root->childarr[0])->u.return_type;
                    }
                    else
                    {
                        type = FindIDType(root->childarr[0])->u.return_type;
                    }
                }
                else
                {
                    SemanticError(Func_Call_Para_Unmatched, root->lineno);
                    type = FindIDType(root->childarr[0])->u.return_type;
                }
            }
        }
        else if(strcmp(root->childarr[0]->type, "Exp")==0)
        {
            Type t0 = Exp(root->childarr[0]);
            if(t0==NULL)
            {
                SemanticError(Operate_Others_As_Array, root->lineno);
                type = NULL;
            }
            else if(t0->type!=ARRAY)
            {
                SemanticError(Operate_Others_As_Array, root->lineno);
                type = NULL;
            }
            else
            {
                Type t2 = Exp(root->childarr[2]);
                if(t2==NULL)
                {
                    SemanticError(Float_Index, root->lineno);
                    type = NULL;
                }
                else if(t2->type!=Int)
                {
                    SemanticError(Float_Index, root->lineno);
                    type = NULL;
                }
                else
                {
                    type = Exp(root->childarr[0])->u.array.ElementType;
                }
            }
        }
    }
    return type;
}

bool judge_if_lvalue(const node* root)
{
    //printf("call judge_if_lvalue\n");
    if(root->childnum==1)
    {
        if(strcmp(root->childarr[0]->type, "ID")==0)
        {
            return true;
        }
    }
    if(root->childnum==3)
    {
        if(strcmp(root->childarr[0]->type, "LP")==0 && strcmp(root->childarr[2]->type, "RP")==0)
        {
            return judge_if_lvalue(root->childarr[1]);
        }
        if(strcmp(root->childarr[0]->type, "Exp")==0 && strcmp(root->childarr[1]->type, "DOT")==0 && strcmp(root->childarr[2]->type, "ID")==0)
        {
            return true;
        }
    }
    if(root->childnum==4)
    {
        if(strcmp(root->childarr[0]->type, "Exp")==0 && strcmp(root->childarr[1]->type, "LB")==0 && strcmp(root->childarr[2]->type, "Exp")==0 && strcmp(root->childarr[3]->type, "RB")==0)
        {
            return true;
        }
    }
    return false;
}

Type FindIDType(const node* root)
{
    //printf("call FindIDType\n");
    Type type;
    VarNode* p1 = VarTable;
    while(p1!=NULL)
    {
        if(strcmp(root->value, p1->name)==0)
        {
            type = p1->type;
            return type;
        }
        p1 = p1->next;
    }
    ArrNode* p2 = ArrTable;
    while(p2!=NULL)
    {
        if(strcmp(root->value, p2->name)==0)
        {
            type = p2->type;
            return type;
        }
        p2 = p2->next;
    }
    StructNode* p3 = StructTable;
    while(p3!=NULL)
    {
        if(strcmp(root->value, p3->name)==0)
        {
            type = p3->type;
            return type;
        }
        p3 = p3->next;
    }
    FuncNode* p4 = FuncTable;
    while(p4!=NULL)
    {
        if(strcmp(root->value, p4->name)==0)
        {
            type = p4->type;
            return type;
        }
        p4 = p4->next;
    }
    return NULL;
}

bool Args(const node* root, FuncParameterType ParaType)
{
    //printf("call Args\n");
    if(ParaType==NULL)
    {
        return false;
    }
    if(root->childnum==1)
    {
        if(judge_type(Exp(root->childarr[0]), ParaType->type))
        {
            if(ParaType->next==NULL)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    else if(root->childnum==3)
    {
        if(judge_type(Exp(root->childarr[0]), ParaType->type))
        {
            return Args(root->childarr[2], ParaType->next);
        }
        else
        {
            return false;
        }
    }
}

FuncParameterType VarList(const node* root, bool if_statement)
{
    //printf("call VarList\n");
    FuncParameterType p = ParamDec(root->childarr[0], if_statement);
    if(root->childnum==1)
    {
        if(p==NULL)
        {
            return NULL;
        }
        p->next = NULL;
        return p;
    }
    else if(root->childnum==3)
    {
        if(p==NULL)
        {
            return VarList(root->childarr[2], if_statement);
        }
        p->next = VarList(root->childarr[2], if_statement);
        return p;
    }
    return p;
}

FuncParameterType ParamDec(const node* root, bool if_statement)
{
    //printf("call ParamDec\n");
    FuncParameterType p = (FuncParameterType)malloc(sizeof(FuncParameterType_));
    Type t = Specifier(root->childarr[0]);
    t = VarDecForType(root->childarr[1], t);
    char* name = VarDecForName(root->childarr[1]);
    p->type = t;
    p->next = NULL;
    if(if_statement)
    {
        return p;
    }
    if(t==NULL)
    {
        return NULL;
    }
    if(!judge_if_conflict(name, t->type, root->lineno))
    {
        if(t->type==Int || t->type==Float)
        {
            NewVar(t, name);
        }
        else if(t->type==ARRAY)
        {
            NewArr(t, name);
        }
        else if(t->type==STRUCTURE)
        {
            NewStruct(t, name);
        }
    }
    return p;
}

void FunDec(const node* root, Type type, bool if_defination)
{
    //printf("call FunDec\n");
    FuncNode* p = (FuncNode*)malloc(sizeof(FuncNode));
    p->type = (Type)malloc(sizeof(Type_));
    p->type->type = FUNC;
    p->type->u.return_type = type;
    p->name = (char*)malloc((strlen(root->childarr[0]->value)+1)*sizeof(char));
    strcpy(p->name, root->childarr[0]->value);
    if(root->childnum==3)
    {
        p->ParaType = NULL;
        p->next = NULL;
        if(if_defination)
        {
            FuncNode* pf = FuncTable;
            if(pf==NULL)
            {
                NewFunc(p->type, p->name, p->ParaType, true);
            }
            else
            {
                while (pf != NULL)
                {
                    if (strcmp(pf->name, p->name) == 0)
                    {
                        if (pf->defined == true)
                        {
                            SemanticError(Repeat_Func_Defination, root->lineno);
                        }
                        else
                        {
                            pf->defined = true;
                            UndefinedFuncNode* move = UndefinedFuncTable;
                            while(move!=NULL)
                            {
                                if(strcmp(p->name, move->name)==0)
                                {
                                    move->defined = true;
                                    break;
                                }
                                move = move->next;
                            }
                            if (!judge_type(p->type->u.return_type, pf->type->u.return_type) || !judge_func_para(p->ParaType, pf->ParaType))
                            {
                                SemanticError(Func_Statement_Conflit, root->lineno); // hanghao???????
                            }
                        }
                        break;
                    }
                    pf = pf->next;
                }
                if(pf==NULL)
                {
                    NewFunc(p->type, p->name, p->ParaType, true);
                }
            }
        }
        else
        {
            FuncNode* pf = FuncTable;
            while(pf!=NULL)
            {
                if(strcmp(pf->name, p->name)==0)
                {
                    if(!pf->defined)
                    {
                        UndefinedFuncNode* move = UndefinedFuncTable;
                        if(move==NULL)
                        {
                            UndefinedFuncNode* get = (UndefinedFuncNode*)malloc(sizeof(UndefinedFuncNode));
                            get->defined = false;
                            get->next = NULL;
                            get->name = (char*)malloc(sizeof(char)*(strlen(p->name)+1));
                            strcpy(get->name, p->name);
                            get->LineNo = (LineNumber*)malloc(sizeof(LineNumber));
                            get->LineNo->line = root->lineno;
                            get->LineNo->next = NULL;
                            UndefinedFuncTable = get;
                        }
                        else
                        {
                            while(move!=NULL)
                            {
                                if(strcmp(p->name, move->name)==0)
                                {
                                    LineNumber* l = (LineNumber*)malloc(sizeof(LineNumber));
                                    l->line = root->lineno;
                                    l->next = NULL;
                                    LineNumber* pl = move->LineNo;
                                    if(pl==NULL)
                                    {
                                        move->LineNo = l;
                                    }
                                    else
                                    {
                                        while(pl->next!=NULL)
                                        {
                                            pl = pl->next;
                                        }
                                        pl->next = l;
                                    }
                                    break;
                                }
                                move = move->next;
                            }
                            if(move==NULL)
                            {
                                UndefinedFuncNode* get = (UndefinedFuncNode*)malloc(sizeof(UndefinedFuncNode));
                                get->defined = false;
                                get->next = NULL;
                                get->name = (char*)malloc(sizeof(char)*(strlen(p->name)+1));
                                strcpy(get->name, p->name);
                                get->LineNo = (LineNumber*)malloc(sizeof(LineNumber));
                                get->LineNo->line = root->lineno;
                                get->LineNo->next = NULL;
                                UndefinedFuncNode* find = UndefinedFuncTable;
                                while(find->next!=NULL)
                                {
                                    find = find->next;
                                }
                                find->next = get;
                            }
                        }
                    }
                    if(!judge_type(p->type->u.return_type, pf->type->u.return_type) || !judge_func_para(p->ParaType, pf->ParaType))
                    {
                        SemanticError(Func_Statement_Conflit, root->lineno);
                    }
                    break;
                }
                pf = pf->next;
            }
            if(pf==NULL)
            {
                NewFunc(p->type, p->name, p->ParaType, false);
                UndefinedFuncNode* get = (UndefinedFuncNode*)malloc(sizeof(UndefinedFuncNode));
                get->defined = false;
                get->next = NULL;
                get->name = (char*)malloc(sizeof(char)*(strlen(p->name)+1));
                strcpy(get->name, p->name);
                get->LineNo = (LineNumber*)malloc(sizeof(LineNumber));
                get->LineNo->line = root->lineno;
                get->LineNo->next = NULL;
                if(UndefinedFuncTable==NULL)
                {
                    UndefinedFuncTable = get;
                }
                else
                {
                    UndefinedFuncNode* find = UndefinedFuncTable;
                    while(find->next!=NULL)
                    {
                        find = find->next;
                    }
                    find->next = get;
                }
            }
        }
    }
    else if(root->childnum==4)
    {
        p->next = NULL;
        if(if_defination)
        {
            p->ParaType = VarList(root->childarr[2], false);
            FuncNode* pf = FuncTable;
            if(pf==NULL)
            {
                NewFunc(p->type, p->name, p->ParaType, true);
            }
            else
            {
                while (pf != NULL)
                {
                    if (strcmp(pf->name, p->name) == 0)
                    {
                        if (pf->defined == true)
                        {
                            SemanticError(Repeat_Func_Defination, root->lineno);
                        }
                        else
                        {
                            pf->defined = true;
                            UndefinedFuncNode* move = UndefinedFuncTable;
                            while(move!=NULL)
                            {
                                if(strcmp(p->name, move->name)==0)
                                {
                                    move->defined = true;
                                    break;
                                }
                                move = move->next;
                            }
                            if (!judge_type(p->type->u.return_type, pf->type->u.return_type) || !judge_func_para(p->ParaType, pf->ParaType))
                            {
                                SemanticError(Func_Statement_Conflit, root->lineno); // hanghao???????
                            }
                        }
                        break;
                    }
                    pf = pf->next;
                }
                if(pf==NULL)
                {
                    NewFunc(p->type, p->name, p->ParaType, true);
                }
            }
        }
        else
        {
            p->ParaType = VarList(root->childarr[2], true);
            FuncNode* pf = FuncTable;
            while(pf!=NULL)
            {
                if(strcmp(pf->name, p->name)==0)
                {
                    if(!pf->defined)
                    {
                        UndefinedFuncNode* move = UndefinedFuncTable;
                        if(move==NULL)
                        {
                            UndefinedFuncNode* get = (UndefinedFuncNode*)malloc(sizeof(UndefinedFuncNode));
                            get->defined = false;
                            get->next = NULL;
                            get->name = (char*)malloc(sizeof(char)*(strlen(p->name)+1));
                            strcpy(get->name, p->name);
                            get->LineNo = (LineNumber*)malloc(sizeof(LineNumber));
                            get->LineNo->line = root->lineno;
                            get->LineNo->next = NULL;
                            UndefinedFuncTable = get;
                        }
                        else
                        {
                            while(move!=NULL)
                            {
                                if(strcmp(p->name, move->name)==0)
                                {
                                    LineNumber* l = (LineNumber*)malloc(sizeof(LineNumber));
                                    l->line = root->lineno;
                                    l->next = NULL;
                                    LineNumber* pl = move->LineNo;
                                    if(pl==NULL)
                                    {
                                        move->LineNo = l;
                                    }
                                    else
                                    {
                                        while(pl->next!=NULL)
                                        {
                                            pl = pl->next;
                                        }
                                        pl->next = l;
                                    }
                                    break;
                                }
                                move = move->next;
                            }
                            if(move==NULL)
                            {
                                UndefinedFuncNode* get = (UndefinedFuncNode*)malloc(sizeof(UndefinedFuncNode));
                                get->defined = false;
                                get->next = NULL;
                                get->name = (char*)malloc(sizeof(char)*(strlen(p->name)+1));
                                strcpy(get->name, p->name);
                                get->LineNo = (LineNumber*)malloc(sizeof(LineNumber));
                                get->LineNo->line = root->lineno;
                                get->LineNo->next = NULL;
                                UndefinedFuncNode* find = UndefinedFuncTable;
                                while(find->next!=NULL)
                                {
                                    find = find->next;
                                }
                                find->next = get;
                            }
                        }
                    }
                    if(!judge_type(p->type->u.return_type, pf->type->u.return_type) || !judge_func_para(p->ParaType, pf->ParaType))
                    {
                        SemanticError(Func_Statement_Conflit, root->lineno);
                    }
                    break;
                }
                pf = pf->next;
            }
            if(pf==NULL)
            {
                NewFunc(p->type, p->name, p->ParaType, false);
                UndefinedFuncNode* get = (UndefinedFuncNode*)malloc(sizeof(UndefinedFuncNode));
                get->defined = false;
                get->next = NULL;
                get->name = (char*)malloc(sizeof(char)*(strlen(p->name)+1));
                strcpy(get->name, p->name);
                get->LineNo = (LineNumber*)malloc(sizeof(LineNumber));
                get->LineNo->line = root->lineno;
                get->LineNo->next = NULL;
                if(UndefinedFuncTable==NULL)
                {
                    UndefinedFuncTable = get;
                }
                else
                {
                    UndefinedFuncNode* find = UndefinedFuncTable;
                    while(find->next!=NULL)
                    {
                        find = find->next;
                    }
                    find->next = get;
                }
            }
        }
    }
}

void CompSt(const node* root, Type ReturnType)
{
    //printf("call CompSt\n");
    DefListInFunc(root->childarr[1]);
    StmtList(root->childarr[2], ReturnType);
}

void StmtList(const node* root, Type ReturnType)
{
    if(root->childnum==1)
    {
        return;
    }
    else if(root->childnum==2)
    {
        Stmt(root->childarr[0], ReturnType);
        StmtList(root->childarr[1], ReturnType);
    }
}

void Stmt(const node* root, Type ReturnType)
{
    if(root->childnum==1)
    {
        CompSt(root->childarr[0], ReturnType);
    }
    else if(root->childnum==2)
    {
        Exp(root->childarr[0]);
    }
    else if(root->childnum==3)
    {
        if(!judge_type(ReturnType, Exp(root->childarr[1])))
        {
            SemanticError(Return_Type_Unmatched, root->lineno);
        }
    }
    else if(root->childnum==5)
    {
        if(strcmp(root->childarr[0]->type, "IF")==0)
        {
            Type t = Exp(root->childarr[2]);
            if(t==NULL)
            {
                SemanticError(OP_Type_Unmatched, root->childarr[2]->lineno);
            }
            else if(t->type!=Int)
            {
                SemanticError(OP_Type_Unmatched, root->childarr[2]->lineno);
            }
            Stmt(root->childarr[4], ReturnType);
        }
        if(strcmp(root->childarr[0]->type, "WHILE")==0)
        {
            Type t2 = Exp(root->childarr[2]);
            if(t2==NULL)
            {
                SemanticError(OP_Type_Unmatched, root->childarr[2]->lineno);
            }
            else if(t2->type!=Int)
            {
                SemanticError(OP_Type_Unmatched, root->childarr[2]->lineno);
            }
            Stmt(root->childarr[4], ReturnType);
        }
    }
    else if(root->childnum==7)
    {
        if(strcmp(root->childarr[0]->type, "IF")==0)
        {
            Type t2 = Exp(root->childarr[2]);
            if(t2==NULL)
            {
                SemanticError(OP_Type_Unmatched, root->childarr[2]->lineno);
            }
            else if(t2->type!=Int)
            {
                SemanticError(OP_Type_Unmatched, root->childarr[2]->lineno);
            }
            Stmt(root->childarr[4], ReturnType);
            Stmt(root->childarr[6], ReturnType);
        }
    }
}

bool judge_func_para(FuncParameterType p1, FuncParameterType p2)
{
    while(p1 != NULL || p2 != NULL)
    {
        if(p1==NULL&&p2!=NULL || p1!=NULL&&p2==NULL)
        {
            return false;
        }
        if(!judge_type(p1->type, p2->type))
        {
            return false;
        }
        p1 = p1->next;
        p2 = p2->next;
    }
    return true;
}

bool judge_if_conflict(char* name, kind type, int line)
/*not including two variables in the same struture and function statement confliction*/
{
    //printf("call judge_if_conflict\n");
    bool mistakes = false;
    if(type==Int || type==Float || type==ARRAY || type==STRUCTURE)
    {
        VarNode* p1 = VarTable;
        while(p1!=NULL)
        {
            if(strcmp(name, p1->name)==0)
            {
                SemanticError(Repeat_ID_Name, line);
                mistakes = true;
                break;
            }
            p1 = p1->next;
        }
        ArrNode* p2 = ArrTable;
        while(p2!=NULL)
        {
            if(strcmp(name, p2->name)==0)
            {
                SemanticError(Repeat_ID_Name, line);
                mistakes = true;
                break;
            }
            p2 = p2->next;
        }
        StructNode* p3 = StructTable;
        while(p3!=NULL)
        {
            if(strcmp(name, p3->name)==0)
            {
                SemanticError(Repeat_ID_Name, line);
                mistakes = true;
                break;
            }
            FieldList p = p3->type->u.structure;
            while(p!=NULL)
            {
                if(strcmp(p->name, name)==0)
                {
                    SemanticError(Repeat_ID_Name, line);
                    mistakes = true;
                    break;
                }
                p = p->tail;
            }
            if(mistakes)
            {
                break;
            }
            p3 = p3->next;
        }
        StructTypeNode* p4 = StructTypeTable;
        while(p4!=NULL)
        {
            if(strcmp(name, p4->name)==0)
            {
                SemanticError(Repeat_ID_Name, line);
                mistakes = true;
                break;
            }
            p4 = p4->next;
        }
    }
    return mistakes;
}