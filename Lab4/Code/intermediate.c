#include "intermediate.h"
#include "assert.h"

int VarNo = 1;
int TempNo = 1;
int LabelNo = 1;
InterCodes InterCodeListHead = NULL;
InterCodes InterCodeListTail = NULL;
VarVarCorr VarVarCorrTable = NULL;

void AddInterCodes(InterCodes head)
{
    if(head==NULL)
    {
        return;
    }
    if(InterCodeListHead==NULL && InterCodeListTail==NULL)
    {
        InterCodeListHead = head;
        InterCodes p = head;
        while(p->next!=NULL)
        {
            p = p->next;
        }
        InterCodeListTail = p;
    }
    else
    {
        InterCodeListTail->next = head;
        head->prev = InterCodeListTail;
        InterCodes p = head;
        while(p->next!=NULL)
        {
            p = p->next;
        }
        InterCodeListTail = p;
    }
}

void PrintOperand(Operand op, FILE* f)
{
    if(op->kind==VARIABLE)
    {
        fprintf(f, "v%d", op->u.var_no);
    }
    else if(op->kind==TEMP)
    {
        fprintf(f, "t%d", op->u.var_no);
    }
    else if(op->kind==CONSTANT)
    {
        fprintf(f, "#%d", op->u.value);
    }
    else if(op->kind==OPLABEL)
    {
        fprintf(f, "label%d", op->u.var_no);
    }
    else if(op->kind==OPFUNC)
    {
        fprintf(f, "%s", op->u.name);
    }
    else if(op->kind==ARGSTRUCT)
    {
        fprintf(f, "v%d", op->u.var_no);
    }
}

void PrintInterCodes(InterCodes p, FILE* f)
{
    if(p!=NULL)
    {
        if(p->code.kind==ASSIGN)
        {
            PrintOperand(p->code.u.assign.left, f);
            fprintf(f, " := ");
            PrintOperand(p->code.u.assign.right, f);
        }
        else if(p->code.kind==GET_ADDR_ASSIGN)
        {
            PrintOperand(p->code.u.assign.left, f);
            fprintf(f, " := &");
            PrintOperand(p->code.u.assign.right, f);
        }
        else if(p->code.kind==GET_ADDR_CONTENT_ASSIGN)
        {
            PrintOperand(p->code.u.assign.left, f);
            fprintf(f, " := *");
            PrintOperand(p->code.u.assign.right, f);
        }
        else if(p->code.kind==ASSIGN_TO_ADDR)
        {
            fprintf(f, "*");
            PrintOperand(p->code.u.assign.left, f);
            fprintf(f, " := ");
            PrintOperand(p->code.u.assign.right, f);
        }
        else if(p->code.kind==ADD)
        {
            PrintOperand(p->code.u.TwoOp.result, f);
            fprintf(f, " := ");
            PrintOperand(p->code.u.TwoOp.op1, f);
            fprintf(f, " + ");
            PrintOperand(p->code.u.TwoOp.op2, f);
        }
        else if(p->code.kind==SUB)
        {
            PrintOperand(p->code.u.TwoOp.result, f);
            fprintf(f, " := ");
            PrintOperand(p->code.u.TwoOp.op1, f);
            fprintf(f, " - ");
            PrintOperand(p->code.u.TwoOp.op2, f);
        }
        else if(p->code.kind==MUL)
        {
            PrintOperand(p->code.u.TwoOp.result, f);
            fprintf(f, " := ");
            PrintOperand(p->code.u.TwoOp.op1, f);
            fprintf(f, " * ");
            PrintOperand(p->code.u.TwoOp.op2, f);
        }
        else if(p->code.kind==ICDIV)
        {
            PrintOperand(p->code.u.TwoOp.result, f);
            fprintf(f, " := ");
            PrintOperand(p->code.u.TwoOp.op1, f);
            fprintf(f, " / ");
            PrintOperand(p->code.u.TwoOp.op2, f);
        }
        else if(p->code.kind==LABEL)
        {
            fprintf(f, "LABEL ");
            PrintOperand(p->code.u.single.op, f);
            fprintf(f, " :");
        }
        else if(p->code.kind==ICFUNC)
        {
            fprintf(f, "FUNCTION ");
            PrintOperand(p->code.u.single.op, f);
            fprintf(f, " :");
        }
        else if(p->code.kind==IFGOTO)
        {
            fprintf(f, "IF ");
            PrintOperand(p->code.u.ThreeOp.op1, f);
            fprintf(f, " %s ", p->code.u.ThreeOp.relop);
            PrintOperand(p->code.u.ThreeOp.op2, f);
            fprintf(f, " GOTO ");
            PrintOperand(p->code.u.ThreeOp.label, f);
        }
        else if(p->code.kind==GOTO)
        {
            fprintf(f, "GOTO ");
            PrintOperand(p->code.u.single.op, f);
        }
        else if(p->code.kind==ICRETURN)
        {
            fprintf(f, "RETURN ");
            PrintOperand(p->code.u.single.op, f);
        }
        else if(p->code.kind==DEC)
        {
            fprintf(f, "DEC ");
            PrintOperand(p->code.u.Dec.op, f);
            fprintf(f, " %d", p->code.u.Dec.size);
        }
        else if(p->code.kind==ARG)
        {
            if(p->code.u.single.op->kind!=ARGSTRUCT)
            {
                fprintf(f, "ARG ");
                PrintOperand(p->code.u.single.op, f);
            }
            else
            {
                fprintf(f, "ARG &");
                PrintOperand(p->code.u.single.op, f);
            }
        }
        else if(p->code.kind==CALL)
        {
            PrintOperand(p->code.u.assign.left, f);
            fprintf(f, " := CALL ");
            PrintOperand(p->code.u.assign.right, f);
        }
        else if(p->code.kind==PARAM)
        {
            fprintf(f, "PARAM ");
            PrintOperand(p->code.u.single.op, f);
        }
        else if(p->code.kind==READ)
        {
            fprintf(f, "READ ");
            PrintOperand(p->code.u.single.op, f);
        }
        else if(p->code.kind==WRITE)
        {
            fprintf(f, "WRITE ");
            PrintOperand(p->code.u.single.op, f);
        }
    }
}

void PrintAllInterCodes(FILE* f)
{
    InterCodes p = InterCodeListHead;
    while(p!=NULL)
    {
        PrintInterCodes(p, f);
        fprintf(f, "\n");
        p = p->next;
    }
}

Operand new_temp()
{
    Operand p = (Operand)malloc(sizeof(struct Operand_));
    p->kind = TEMP;
    p->u.var_no = TempNo;
    ++TempNo;
    return p;
}

Operand new_var(char* VarName, bool IfPara)
{
    Operand p = (Operand)malloc(sizeof(struct Operand_));
    p->kind = VARIABLE;
    p->u.var_no = VarNo;
    p->IfPara = IfPara;
    ++VarNo;
    VarVarCorr get = (VarVarCorr)malloc(sizeof(struct VarVarCorr_));
    get->next = NULL;
    get->var = p;
    get->VarName = (char*)malloc((strlen(VarName)+1)*sizeof(char));
    strcpy(get->VarName, VarName);
    if(VarVarCorrTable==NULL)
    {
        VarVarCorrTable = get;
    }
    else
    {
        VarVarCorr move = VarVarCorrTable;
        while(move->next!=NULL)
        {
            move = move->next;
        }
        move->next = get;
    }
    return p;
}

Operand FindVar(char* VarName)
{
    VarVarCorr move = VarVarCorrTable;
    while(move!=NULL)
    {
        if(strcmp(VarName, move->VarName)==0)
        {
            return move->var;
        }
        move = move->next;
    }
    return NULL;
}

Operand new_label()
{
    Operand p = (Operand)malloc(sizeof(struct Operand_));
    p->kind = OPLABEL;
    p->u.var_no = LabelNo;
    ++LabelNo;
    return p;
}

InterCodes ConnectCodes(InterCodes code1, InterCodes code2)
{
    InterCodes move = code1;
    if(code2==NULL)
    {
        return code1;
    }
    if(code1==NULL)
    {
        return code2;
    }
    while(move->next!=NULL)
    {
        move = move->next;
    }
    move->next = code2;
    code2->prev = move;
    return code1;
}

InterCodes NewAssignInterCodes(Operand left, Operand right, int kind)
{
    if(left==NULL || right==NULL)
    {
        return NULL;
    }
    InterCodes c = (InterCodes)malloc(sizeof(struct InterCodes_));
    c->next = NULL;
    c->prev = NULL;
    c->code.kind = kind;
    c->code.u.assign.left = left;
    c->code.u.assign.right = right;
    return c;
}

int GetStructTypeSize(char* name)
{
    StructTypeNode* p = StructTypeTable;
    while(p!=NULL)
    {
        if(strcmp(p->name, name)==0)
        {
            break;
        }
        p = p->next;
    }
    if(p==NULL)
    {
        return -1;
    }
    else
    {
        FieldList move = p->type->u.structure;
        int size = 0;
        while(move!=NULL)
        {
            if(move->type->type==ARRAY)
            {
                size += 4*move->type->u.array.size;
            }
            else if(move->type->type==STRUCTURE)
            {
                size += GetStructTypeSize(move->type->u.StructTypeName);
            }
            else
            {
                size += 4;
            }
            move = move->tail;
        }
        return size;
    }
}

int GetOffset(char* StructName, char* FieldName)
{
    int offset = 0;
    StructNode* p = StructTable;
    while(p!=NULL)
    {
        if(strcmp(p->name, StructName)==0)
        {
            break;
        }
        p = p->next;
    }
    if(p==NULL)
    {
        return -1;
    }
    else if(p!=NULL)
    {
        FieldList move = p->type->u.structure;
        while(move!=NULL)
        {
            if(strcmp(move->name, FieldName)==0)
            {
                return offset;
            }
            else
            {
                if(move->type->type==STRUCTURE)
                {
                    int get = GetOffset(move->name, FieldName);
                    if(get==GetStructTypeSize(move->type->u.StructTypeName))
                    {
                        offset += get;
                    }
                    else
                    {
                        offset += get;
                        return offset;
                    }
                }
                else if(move->type->type==ARRAY)
                {
                    offset += 4*move->type->u.array.size;
                }
                else
                {
                    offset += 4;
                }
                move = move->tail;
            }
        }
    }
    return offset;
}

int GetOffsetByStructTypeName(char* StructTypeName, char* FieldName)
{
    Type type = NULL;
    StructTypeNode* p = StructTypeTable;
    while(p!=NULL)
    {
        if(strcmp(p->name, StructTypeName)==0)
        {
            break;
        }
        else
        {
            p = p->next;
        }
    }
    if(p==NULL)
    {
        return -1;
    }
    int offset = 0;
    FieldList move = p->type->u.structure;
    while (move != NULL)
    {
        if (strcmp(move->name, FieldName) == 0)
        {
            return offset;
        }
        else
        {
            if (move->type->type == STRUCTURE)
            {
                int get = GetOffset(move->name, FieldName);
                if (get == GetStructTypeSize(move->type->u.StructTypeName))
                {
                    offset += get;
                }
                else
                {
                    offset += get;
                    return offset;
                }
            }
            else if (move->type->type == ARRAY)
            {
                offset += 4 * move->type->u.array.size;
            }
            else
            {
                offset += 4;
            }
            move = move->tail;
        }
    }
}

InterCodes translate_Exp(const node* root, Operand place)
{
    if(strcmp(root->childarr[0]->type, "LP")==0)
    {
        return translate_Exp(root->childarr[1], place);
    }
    else if(strcmp(root->childarr[0]->type, "INT")==0)
    {
        Operand op = (Operand)malloc(sizeof(struct Operand_));
        op->kind = CONSTANT;
        op->u.value = atoi(root->childarr[0]->value);
        InterCodes code1 = NewAssignInterCodes(place, op, ASSIGN);
        return code1;
    }
    else if(strcmp(root->childarr[0]->type, "ID")==0 && root->childnum==1)
    {
        Operand op = FindVar(root->childarr[0]->value);
        if(op==NULL)
        {
            op = new_var(root->childarr[0]->value, false);
        }
        InterCodes code1 = NewAssignInterCodes(place, op, ASSIGN);
        return code1;
    }
    else if(strcmp(root->childarr[1]->type, "ASSIGNOP")==0)
    {
        if(root->childarr[0]->childnum==1)
        {
            ArrNode * parr = ArrTable;
            while(parr!=NULL)
            {
                if(strcmp(parr->name, root->childarr[0]->childarr[0]->value)==0)
                {
                    break;
                }
                else
                {
                    parr = parr->next;
                }
            }
            if(parr!=NULL)
            {
                ArrNode* get = ArrTable;
                while(get!=NULL)
                {
                    if(strcmp(get->name, root->childarr[2]->childarr[0]->value)==0)
                    {
                        break;
                    }
                    else
                    {
                        get = get->next;
                    }
                }
                if(get!=NULL)
                {
                    int size1 = parr->type->u.array.size;
                    int size2 = get->type->u.array.size;
                    int min = 0;
                    if(size1<=size2)
                    {
                        min = size1;
                    }
                    else
                    {
                        min = size2;
                    }
                    Operand base1 = new_temp();
                    Operand base2 = new_temp();
                    InterCodes code1 = translate_BaseAddr(root->childarr[0], base1);
                    InterCodes code2 = translate_BaseAddr(root->childarr[2], base2);
                    InterCodes code = NULL;
                    for(int i=0;i<min;++i)
                    {
                        Operand offset = new_temp();
                        InterCodes codea = (InterCodes)malloc(sizeof(struct InterCodes_));
                        codea->next = NULL;
                        codea->prev = NULL;
                        codea->code.kind = MUL;
                        codea->code.u.TwoOp.result = offset;
                        codea->code.u.TwoOp.op1 = (Operand)malloc(sizeof(struct Operand_));
                        codea->code.u.TwoOp.op1->kind = CONSTANT;
                        codea->code.u.TwoOp.op1->u.value = i;
                        codea->code.u.TwoOp.op2 = (Operand)malloc(sizeof(struct Operand_));
                        codea->code.u.TwoOp.op2->kind = CONSTANT;
                        codea->code.u.TwoOp.op2->u.value = 4;
                        Operand addr1 = new_temp();
                        InterCodes codeb = (InterCodes)malloc(sizeof(struct InterCodes_));
                        codeb->next = NULL;
                        codeb->prev = NULL;
                        codeb->code.kind = ADD;
                        codeb->code.u.TwoOp.result = addr1;
                        codeb->code.u.TwoOp.op1 = offset;
                        codeb->code.u.TwoOp.op2 = base1;
                        Operand addr2 = new_temp();
                        InterCodes codec = (InterCodes)malloc(sizeof(struct InterCodes_));
                        codec->next = NULL;
                        codec->prev = NULL;
                        codec->code.kind = ADD;
                        codec->code.u.TwoOp.result = addr2;
                        codec->code.u.TwoOp.op1 = offset;
                        codec->code.u.TwoOp.op2 = base2;
                        Operand MiddleValue = new_temp();
                        InterCodes coded = (InterCodes)malloc(sizeof(struct InterCodes_));
                        coded->next = NULL;
                        coded->prev = NULL;
                        coded->code.kind = GET_ADDR_CONTENT_ASSIGN;
                        coded->code.u.assign.left = MiddleValue;
                        coded->code.u.assign.right = addr2;
                        InterCodes codee = (InterCodes)malloc(sizeof(struct InterCodes_));
                        codee->next = NULL;
                        codee->prev = NULL;
                        codee->code.kind = ASSIGN_TO_ADDR;
                        codee->code.u.assign.left = addr1;
                        codee->code.u.assign.right = MiddleValue;
                        coded = ConnectCodes(coded, codee);
                        codec = ConnectCodes(codec, coded);
                        codeb = ConnectCodes(codeb, codec);
                        codea = ConnectCodes(codea, codeb);
                        code = ConnectCodes(code, codea);
                    }
                    code2 = ConnectCodes(code2, code);
                    code1 = ConnectCodes(code1, code2);
                    return code1;
                }
            }
            Operand v1 = FindVar(root->childarr[0]->childarr[0]->value);
            if (v1 == NULL)
            {
                v1 = new_var(root->childarr[0]->childarr[0]->value, false);
            }
            Operand t = new_temp();
            InterCodes code1 = translate_Exp(root->childarr[2], t);
            InterCodes c1 = NewAssignInterCodes(v1, t, ASSIGN);
            InterCodes c2 = NewAssignInterCodes(place, v1, ASSIGN);
            c1 = ConnectCodes(c1, c2);
            code1 = ConnectCodes(code1, c1);
            return code1;
        }
        else if(strcmp(root->childarr[0]->childarr[1]->type, "DOT")==0)
        {
            // node* find = root->childarr[0];
            // Operand off = new_temp();
            // Operand zero = (Operand)malloc(sizeof(struct Operand_));
            // zero->kind = CONSTANT;
            // zero->u.value = 0;
            // InterCodes code = NewAssignInterCodes(off, zero, ASSIGN);
            // while(find->childarr[0]->childnum!=1)
            // {
            //     Type type = Exp(find->childarr[0]);
            //     if(type->type==ARRAY)
            //     {
            //         Operand in = new_temp();
            //         InterCodes c0 = translate_Exp(find->childarr[2], in);
            //         int s = GetStructTypeSize(type->u.array.ElementType->u.StructTypeName);
            //         Operand ssize = (Operand)malloc(sizeof(struct Operand_));
            //         ssize->kind = CONSTANT;
            //         ssize->u.value = s;
            //         Operand num = new_temp();
            //         InterCodes c1 = (InterCodes)malloc(sizeof(struct InterCodes_));
            //         c1->next = NULL;
            //         c1->prev = NULL;
            //         c1->code.kind = MUL;
            //         c1->code.u.TwoOp.result = num;
            //         c1->code.u.TwoOp.op1 = in;
            //         c1->code.u.TwoOp.op2 = ssize;
            //         InterCodes c2 = (InterCodes)malloc(sizeof(struct InterCodes_));
            //         c2->next = NULL;
            //         c2->prev = NULL;
            //         c2->code.kind = ADD;
            //         c2->code.u.TwoOp.result = off;
            //         c2->code.u.TwoOp.op1 = off;
            //         c2->code.u.TwoOp.op2 = num;
            //         c1 = ConnectCodes(c1, c2);
            //         code = ConnectCodes(code, c1);
            //     }
            //     else if(type->type==STRUCTURE)
            //     {
            //         int s = GetOffsetByStructTypeName(type->u.StructTypeName, find->childarr[2]->value);
            //         Operand ssize = (Operand)malloc(sizeof(struct Operand_));
            //         ssize->kind = CONSTANT;
            //         ssize->u.value = s;
            //         InterCodes c1 = (InterCodes)malloc(sizeof(struct InterCodes_));
            //         c1->next = NULL;
            //         c1->prev = NULL;
            //         c1->code.kind = ADD;
            //         c1->code.u.TwoOp.result = off;
            //         c1->code.u.TwoOp.op1 = off;
            //         c1->code.u.TwoOp.op2 = ssize;
            //         code = ConnectCodes(code, c1);
            //     }
            //     find = find->childarr[0];
            // }
            // return code;}}

            if (root->childarr[0]->childarr[0]->childnum == 1)
            {
                Operand v = FindVar(root->childarr[0]->childarr[0]->childarr[0]->value);
                if (v == NULL)
                {
                    v = new_var(root->childarr[0]->childarr[0]->value, false);
                }
                Operand t = new_temp();
                InterCodes code = (InterCodes)malloc(sizeof(struct InterCodes_));
                code->next = NULL;
                code->prev = NULL;
                if (!v->IfPara)
                {
                    code->code.kind = GET_ADDR_ASSIGN;
                }
                else
                {
                    code->code.kind = ASSIGN;
                }
                code->code.u.assign.left = t;
                code->code.u.assign.right = v;
                Operand offset = (Operand)malloc(sizeof(struct Operand_));
                offset->kind = CONSTANT;
                offset->u.value = GetOffset(root->childarr[0]->childarr[0]->childarr[0]->value, root->childarr[0]->childarr[2]->value);
                Operand t1 = new_temp();
                InterCodes code1 = (InterCodes)malloc(sizeof(struct InterCodes_));
                code1->next = NULL;
                code1->prev = NULL;
                code1->code.kind = ADD;
                code1->code.u.TwoOp.result = t1;
                code1->code.u.TwoOp.op1 = t;
                code1->code.u.TwoOp.op2 = offset;
                Operand t2 = new_temp();
                InterCodes code2 = translate_Exp(root->childarr[2], t2);
                InterCodes code3 = (InterCodes)malloc(sizeof(struct InterCodes_));
                code3->next = NULL;
                code3->prev = NULL;
                code3->code.kind = ASSIGN_TO_ADDR;
                code3->code.u.assign.left = t1;
                code3->code.u.assign.right = t2;
                InterCodes code4 = NewAssignInterCodes(place, t2, ASSIGN);
                code3 = ConnectCodes(code3, code4);
                code2 = ConnectCodes(code2, code3);
                code1 = ConnectCodes(code1, code2);
                code = ConnectCodes(code, code1);
                return code;
            }
            else if(root->childarr[0]->childarr[0]->childnum==4)
            {
                ArrNode* FindArr = ArrTable;
                while(FindArr!=NULL)
                {
                    if(strcmp(FindArr->name, root->childarr[0]->childarr[0]->childarr[0]->childarr[0]->value)==0)
                    {
                        break;
                    }
                    else
                    {
                        FindArr = FindArr->next;
                    }
                }
                Type type = FindArr->type->u.array.ElementType;
                int StructSize = GetStructTypeSize(type->u.StructTypeName);
                Operand ArrBase = FindVar(root->childarr[0]->childarr[0]->childarr[0]->childarr[0]->value);
                if (ArrBase == NULL)
                {
                    ArrBase = new_var(root->childarr[0]->childarr[0]->childarr[0]->childarr[0]->value, false);
                }
                Operand t0 = new_temp();
                Operand index1 = new_temp();
                Operand offset1 = new_temp();
                InterCodes code1 = translate_Exp(root->childarr[0]->childarr[0]->childarr[2], index1);
                InterCodes code2 = (InterCodes)malloc(sizeof(struct InterCodes_));
                code2->next = NULL;
                code2->prev = NULL;
                code2->code.kind = MUL;
                code2->code.u.TwoOp.result = offset1;
                code2->code.u.TwoOp.op1 = index1;
                code2->code.u.TwoOp.op2 = (Operand)malloc(sizeof(struct Operand_));
                code2->code.u.TwoOp.op2->kind = CONSTANT;
                code2->code.u.TwoOp.op2->u.value = StructSize;

                Operand base1 = new_temp();
                InterCodes code3 = (InterCodes)malloc(sizeof(struct InterCodes_));
                code3->next = NULL;
                code3->prev = NULL;
                if (!ArrBase->IfPara)
                {
                    code3->code.kind = GET_ADDR_ASSIGN;
                }
                else
                {
                    code3->code.kind = ASSIGN;
                }
                code3->code.u.assign.left = base1;
                code3->code.u.assign.right = ArrBase;
                Operand addr1 = new_temp();
                InterCodes code4 = (InterCodes)malloc(sizeof(struct InterCodes_));
                code4->next = NULL;
                code4->prev = NULL;
                code4->code.kind = ADD;
                code4->code.u.TwoOp.result = addr1;
                code4->code.u.TwoOp.op1 = base1;
                code4->code.u.TwoOp.op2 = offset1;
                Operand offset2 = (Operand)malloc(sizeof(struct Operand_));
                offset2->kind = CONSTANT;
                offset2->u.value = GetOffsetByStructTypeName(type->u.StructTypeName, root->childarr[0]->childarr[2]->value);
                Operand addr2 = new_temp();
                InterCodes code5 = (InterCodes)malloc(sizeof(struct InterCodes_));
                code5->next = NULL;
                code5->prev = NULL;
                code5->code.kind = ADD;
                code5->code.u.TwoOp.result = addr2;
                code5->code.u.TwoOp.op1 = addr1;
                code5->code.u.TwoOp.op2 = offset2;

                Operand t2 = new_temp();
                InterCodes code6 = translate_Exp(root->childarr[2], t2);
                InterCodes code7 = (InterCodes)malloc(sizeof(struct InterCodes_));
                code7->next = NULL;
                code7->prev = NULL;
                code7->code.kind = ASSIGN_TO_ADDR;
                code7->code.u.assign.left = addr2;
                code7->code.u.assign.right = t2;
                InterCodes code8 = NewAssignInterCodes(place, t2, ASSIGN);
                code7 = ConnectCodes(code7, code8);
                code6 = ConnectCodes(code6, code7);
                code5 = ConnectCodes(code5, code6);
                code4 = ConnectCodes(code4, code5);
                code3 = ConnectCodes(code3, code4);
                code2 = ConnectCodes(code2, code3);
                code1 = ConnectCodes(code1, code2);
                return code1;
            }
        }
        else if(root->childarr[0]->childnum == 4)
        {
            if(root->childarr[0]->childarr[0]->childnum==4)
            {
                printf("Cannot translate: Code contains variables of multi-dimensional array type or parameters of array type.\n");
                exit(-1);
            }
            Operand v = FindVar(root->childarr[0]->childarr[0]->value);
            if(v == NULL)
            {
                v = new_var(root->childarr[0]->childarr[0]->value, false);
            }
            Operand t = new_temp();
            InterCodes code = translate_BaseAddr(root->childarr[0]->childarr[0], t);
            Operand t0 = new_temp();
            InterCodes code1 = translate_Exp(root->childarr[0]->childarr[2], t0);
            Operand offset = new_temp();
            InterCodes code2 = (InterCodes)malloc(sizeof(struct InterCodes_));
            code2->next = NULL;
            code2->prev = NULL;
            code2->code.kind = MUL;
            code2->code.u.TwoOp.result = offset;
            code2->code.u.TwoOp.op1 = t0;
            code2->code.u.TwoOp.op2 = (Operand)malloc(sizeof(struct Operand_));
            code2->code.u.TwoOp.op2->kind = CONSTANT;
            code2->code.u.TwoOp.op2->u.value = 4;
            Operand t1 = new_temp();
            InterCodes code3 = (InterCodes)malloc(sizeof(struct InterCodes_));
            code3->next = NULL;
            code3->prev = NULL;
            code3->code.kind = ADD;
            code3->code.u.TwoOp.result = t1;
            code3->code.u.TwoOp.op1 = t;
            code3->code.u.TwoOp.op2 = offset;
            Operand t2 = new_temp();
            InterCodes code4 = translate_Exp(root->childarr[2], t2);
            InterCodes code5 = (InterCodes)malloc(sizeof(struct InterCodes_));
            code5->next = NULL;
            code5->prev = NULL;
            code5->code.kind = ASSIGN_TO_ADDR;
            code5->code.u.assign.left = t1;
            code5->code.u.assign.right = t2;
            InterCodes code6 = NewAssignInterCodes(place, t2, ASSIGN);
            code5 = ConnectCodes(code5, code6);
            code4 = ConnectCodes(code4, code5);
            code3 = ConnectCodes(code3, code4);
            code2 = ConnectCodes(code2, code3);
            code1 = ConnectCodes(code1, code2);
            code = ConnectCodes(code, code1);
            return code;
        }
    }
    else if(strcmp(root->childarr[1]->type, "PLUS")==0)
    {
        Operand t1 = new_temp();
        Operand t2 = new_temp();
        InterCodes code1 = translate_Exp(root->childarr[0], t1);
        InterCodes code2 = translate_Exp(root->childarr[2], t2);
        InterCodes code3 = NULL;
        if(place!=NULL)
        {
            code3 = (InterCodes)malloc(sizeof(struct InterCodes_));
            code3->next = NULL;
            code3->prev = NULL;
            code3->code.kind = ADD;
            code3->code.u.TwoOp.op1 = t1;
            code3->code.u.TwoOp.op2 = t2;
            code3->code.u.TwoOp.result = place;
        }
        code2 = ConnectCodes(code2, code3);
        code1 = ConnectCodes(code1, code2);
        return code1;
    }
    else if(strcmp(root->childarr[1]->type, "MINUS")==0)
    {
        Operand t1 = new_temp();
        Operand t2 = new_temp();
        InterCodes code1 = translate_Exp(root->childarr[0], t1);
        InterCodes code2 = translate_Exp(root->childarr[2], t2);
        InterCodes code3 = NULL;
        if(place!=NULL)
        {
            code3 = (InterCodes)malloc(sizeof(struct InterCodes_));
            code3->next = NULL;
            code3->prev = NULL;
            code3->code.kind = SUB;
            code3->code.u.TwoOp.op1 = t1;
            code3->code.u.TwoOp.op2 = t2;
            code3->code.u.TwoOp.result = place;
        }
        code2 = ConnectCodes(code2, code3);
        code1 = ConnectCodes(code1, code2);
        return code1;
    }
    else if(strcmp(root->childarr[1]->type, "STAR")==0)
    {
        Operand t1 = new_temp();
        Operand t2 = new_temp();
        InterCodes code1 = translate_Exp(root->childarr[0], t1);
        InterCodes code2 = translate_Exp(root->childarr[2], t2);
        InterCodes code3 = NULL;
        if(place!=NULL)
        {
            code3 = (InterCodes)malloc(sizeof(struct InterCodes_));
            code3->next = NULL;
            code3->prev = NULL;
            code3->code.kind = MUL;
            code3->code.u.TwoOp.op1 = t1;
            code3->code.u.TwoOp.op2 = t2;
            code3->code.u.TwoOp.result = place;
        }
        code2 = ConnectCodes(code2, code3);
        code1 = ConnectCodes(code1, code2);
        return code1;
    }
    else if(strcmp(root->childarr[1]->type, "DIV")==0)
    {
        Operand t1 = new_temp();
        Operand t2 = new_temp();
        InterCodes code1 = translate_Exp(root->childarr[0], t1);
        InterCodes code2 = translate_Exp(root->childarr[2], t2);
        InterCodes code3 = NULL;
        if(place!=NULL)
        {
            code3 = (InterCodes)malloc(sizeof(struct InterCodes_));
            code3->next = NULL;
            code3->prev = NULL;
            code3->code.kind = ICDIV;
            code3->code.u.TwoOp.op1 = t1;
            code3->code.u.TwoOp.op2 = t2;
            code3->code.u.TwoOp.result = place;
        }
        code2 = ConnectCodes(code2, code3);
        code1 = ConnectCodes(code1, code2);
        return code1;
    }
    else if(strcmp(root->childarr[0]->type, "MINUS")==0 && root->childnum==2)
    {
        Operand t1 = new_temp();
        InterCodes code1 = translate_Exp(root->childarr[1], t1);
        Operand op1 = (Operand)malloc(sizeof(struct Operand_));
        op1->kind = CONSTANT;
        op1->u.value = 0;
        InterCodes code2 = NULL;
        if(place!=NULL)
        {
            code2 = (InterCodes) malloc(sizeof(struct InterCodes_));
            code2->next = NULL;
            code2->prev = NULL;
            code2->code.kind = SUB;
            code2->code.u.TwoOp.op1 = op1;
            code2->code.u.TwoOp.op2 = t1;
            code2->code.u.TwoOp.result = place;
        }
        code1 = ConnectCodes(code1, code2);
        return code1;
    }
    else if(strcmp(root->childarr[1]->type, "RELOP")==0 || strcmp(root->childarr[0]->type, "NOT")==0
            || strcmp(root->childarr[1]->type, "AND")==0 || strcmp(root->childarr[1]->type, "OR")==0)
    {
        Operand label1 = new_label();
        Operand label2 = new_label();
        Operand right = (Operand)malloc(sizeof(struct Operand_));
        right->kind = CONSTANT;
        right->u.value = 0;
        InterCodes code0 = NewAssignInterCodes(place, right, ASSIGN);
        InterCodes code1 = translate_Cond(root, label1, label2);
        InterCodes code21 = (InterCodes)malloc(sizeof(struct InterCodes_));
        code21->next = NULL;
        code21->prev = NULL;
        code21->code.kind = LABEL;
        code21->code.u.single.op = label1;
        InterCodes code22 = NULL;
        if(place!=NULL)
        {
            code22 = (InterCodes)malloc(sizeof(struct InterCodes_));
            code22->next = NULL;
            code22->prev = NULL;
            code22->code.kind = ASSIGN;
            code22->code.u.assign.left = place;
            Operand get = (Operand)malloc(sizeof(struct Operand_));
            get->kind = CONSTANT;
            get->u.value = 1;
            code22->code.u.assign.right = get;
        }
        code21 = ConnectCodes(code21, code22);
        InterCodes code3 = (InterCodes)malloc(sizeof(struct InterCodes_));
        code3->next = NULL;
        code3->prev = NULL;
        code3->code.kind = LABEL;
        code3->code.u.single.op = label2;
        code21 = ConnectCodes(code21, code3);
        code1 = ConnectCodes(code1, code21);
        code0 = ConnectCodes(code0, code1);
        return code0;
    }
    else if(strcmp(root->childarr[0]->type, "ID")==0 && root->childnum==3)
    {
        InterCodes code = (InterCodes)malloc(sizeof(struct InterCodes_));
        if(strcmp(root->childarr[0]->value, "read")==0)
        {
            if(place==NULL)
            {
                return NULL;
            }
            code->next = NULL;
            code->prev = NULL;
            code->code.kind = READ;
            code->code.u.single.op = place;
            return code;
        }
        if(place!=NULL)
        {
            code->next = NULL;
            code->prev = NULL;
            code->code.kind = CALL;
            code->code.u.assign.left = place;
            Operand func = (Operand)malloc(sizeof(struct Operand_));
            func->kind = OPFUNC;
            func->u.name = (char *)malloc(sizeof(char) * (strlen(root->childarr[0]->value) + 1));
            strcpy(func->u.name, root->childarr[0]->value);
            code->code.u.assign.right = func;
        }
        else
        {
            Operand ttt = new_temp();
            code->next = NULL;
            code->prev = NULL;
            code->code.kind = CALL;
            code->code.u.assign.left = ttt;
            Operand func = (Operand)malloc(sizeof(struct Operand_));
            func->kind = OPFUNC;
            func->u.name = (char *)malloc(sizeof(char) * (strlen(root->childarr[0]->value) + 1));
            strcpy(func->u.name, root->childarr[0]->value);
            code->code.u.assign.right = func;
        }
        return code;
    }
    else if(strcmp(root->childarr[0]->type, "ID")==0 && root->childnum==4 && strcmp(root->childarr[1]->type, "LP")==0)
    {
        ArgsNode ArgsList = NULL;
        InterCodes code1 = translate_Args(root->childarr[2], &ArgsList);
        if(strcmp(root->childarr[0]->value, "write")==0)
        {
            InterCodes c1 = (InterCodes)malloc(sizeof(struct InterCodes_));
            c1->next = NULL;
            c1->prev = NULL;
            c1->code.kind = WRITE;
            assert(ArgsList!=NULL);
            c1->code.u.single.op = ArgsList->args;
            InterCodes c2 = NULL;
            if(place!=NULL)
            {
                c2 = (InterCodes)malloc(sizeof(struct InterCodes_));
                c2->next = NULL;
                c2->prev = NULL;
                c2->code.kind = ASSIGN;
                c2->code.u.assign.left = place;
                c2->code.u.assign.right = (Operand)malloc(sizeof(struct Operand_));
                c2->code.u.assign.right->kind = CONSTANT;
                c2->code.u.assign.right->u.value = 0;
            }
            c1 = ConnectCodes(c1, c2);
            code1 = ConnectCodes(code1, c1);
            return code1;
        }
        FuncNode* find = FuncTable;
        while(find!=NULL)
        {
            if(strcmp(root->childarr[0]->value, find->name)==0)
            {
                break;
            }
            find = find->next;
        }
        FuncParameterType ParaTraverse = find->ParaType;
        ArgsNode move = ArgsList;
        InterCodes code2 = (InterCodes)malloc(sizeof(struct InterCodes_));
        code2->next = NULL;
        code2->prev = NULL;
        code2->code.kind = ARG;
        code2->code.u.single.op = move->args;
        if(ParaTraverse->type->type==STRUCTURE)
        {
            code2->code.u.single.op->kind = ARGSTRUCT;
        }
        move = move->next;
        ParaTraverse = ParaTraverse->next;
        while(move!=NULL && ParaTraverse!=NULL)
        {
            InterCodes c = (InterCodes)malloc(sizeof(struct InterCodes_));
            c->next = NULL;
            c->prev = NULL;
            c->code.kind = ARG;
            c->code.u.single.op = move->args;
            if(ParaTraverse->type->type==STRUCTURE)
            {
                c->code.u.single.op->kind = ARGSTRUCT;
            }
            code2 = ConnectCodes(code2, c);
            move = move->next;
            ParaTraverse = ParaTraverse->next;
        }
        InterCodes code3 = NULL;
        if(place!=NULL)
        {
            code3 = (InterCodes)malloc(sizeof(struct InterCodes_));
            code3->next = NULL;
            code3->prev = NULL;
            code3->code.kind = CALL;
            code3->code.u.assign.left = place;
            Operand func = (Operand)malloc(sizeof(struct Operand_));
            func->kind = OPFUNC;
            func->u.name = (char *)malloc(sizeof(char) * (strlen(root->childarr[0]->value) + 1));
            strcpy(func->u.name, root->childarr[0]->value);
            code3->code.u.assign.right = func;
        }
        else
        {
            Operand ttt = new_temp();
            code3 = (InterCodes)malloc(sizeof(struct InterCodes_));
            code3->next = NULL;
            code3->prev = NULL;
            code3->code.kind = CALL;
            code3->code.u.assign.left = ttt;
            Operand func = (Operand)malloc(sizeof(struct Operand_));
            func->kind = OPFUNC;
            func->u.name = (char *)malloc(sizeof(char) * (strlen(root->childarr[0]->value) + 1));
            strcpy(func->u.name, root->childarr[0]->value);
            code3->code.u.assign.right = func;
        }
        code2 = ConnectCodes(code2, code3);
        code1 = ConnectCodes(code1, code2);
        return code1;
    }
    else if(strcmp(root->childarr[0]->type, "Exp")==0 && root->childnum==4 && strcmp(root->childarr[1]->type, "LB")==0)
    {
        if(root->childarr[0]->childnum==4)
        {
            printf("Cannot translate: Code contains variables of multi-dimensional array type or parameters of array type.\n");
            exit(-1);
        }
        Operand t1 = new_temp();
        InterCodes code0 = translate_Exp(root->childarr[2], t1);
        Operand t2 = new_temp();
        InterCodes code1 = (InterCodes)malloc(sizeof(struct InterCodes_));
        code1->next = NULL;
        code1->prev = NULL;
        code1->code.kind = MUL;
        code1->code.u.TwoOp.result = t2;
        code1->code.u.TwoOp.op1 = t1;
        code1->code.u.TwoOp.op2 = (Operand)malloc(sizeof(struct Operand_));
        code1->code.u.TwoOp.op2->kind = CONSTANT;
        code1->code.u.TwoOp.op2->u.value = 4;
        Operand baseaddr = new_temp();
        InterCodes code2 = translate_BaseAddr(root->childarr[0], baseaddr);

        Operand t = new_temp();
        InterCodes code3 = (InterCodes)malloc(sizeof(struct InterCodes_));
        code3->next = NULL;
        code3->prev = NULL;
        code3->code.kind = ADD;
        code3->code.u.TwoOp.result = t;
        code3->code.u.TwoOp.op1 = baseaddr;
        code3->code.u.TwoOp.op2 = t2;
        InterCodes code4 = NULL;
        if(place!=NULL)
        {
            code4 = (InterCodes)malloc(sizeof(struct InterCodes_));
            code4->next = NULL;
            code4->prev = NULL;
            code4->code.kind = GET_ADDR_CONTENT_ASSIGN;
            code4->code.u.assign.left = place;
            code4->code.u.assign.right = t;
        }
        code3 = ConnectCodes(code3, code4);
        code2 = ConnectCodes(code2, code3);
        code1 = ConnectCodes(code1, code2);
        code0 = ConnectCodes(code0 ,code1);
        return code0;
    }
    else if(strcmp(root->childarr[1]->type, "DOT")==0)
    {
        char* StructName = NULL;
        char* FieldName = root->childarr[2]->value;
        // node* find = root->childarr[0];
        // while(find->childnum == 3 && (find->childarr[1]->type, "DOT") == 0)
        // {
        //     find = find->childarr[0];
        // }
        if(root->childarr[0]->childnum==1)
        {
            StructName = root->childarr[0]->childarr[0]->value;
            int offset = GetOffset(StructName, FieldName);
            Operand v = FindVar(StructName);
            if (v == NULL)
            {
                v = new_var(StructName, false);
            }
            Operand t1 = new_temp();
            InterCodes code1 = (InterCodes)malloc(sizeof(struct InterCodes_));
            code1->next = NULL;
            code1->prev = NULL;
            if(!v->IfPara)
            {
                code1->code.kind = GET_ADDR_ASSIGN;
            }
            else
            {
                code1->code.kind = ASSIGN;
            }
            code1->code.u.assign.left = t1;
            code1->code.u.assign.right = v;
            Operand t2 = new_temp();
            InterCodes code2 = (InterCodes)malloc(sizeof(struct InterCodes_));
            code2->next = NULL;
            code2->prev = NULL;
            code2->code.kind = ADD;
            code2->code.u.TwoOp.result = t2;
            code2->code.u.TwoOp.op1 = t1;
            code2->code.u.TwoOp.op2 = (Operand)malloc(sizeof(struct Operand_));
            code2->code.u.TwoOp.op2->kind = CONSTANT;
            code2->code.u.TwoOp.op2->u.value = offset;
            InterCodes code3 = (InterCodes)malloc(sizeof(struct InterCodes_));
            code3->next = NULL;
            code3->prev = NULL;
            code3->code.kind = GET_ADDR_CONTENT_ASSIGN;
            code3->code.u.assign.left = place;
            code3->code.u.assign.right = t2;
            code2 = ConnectCodes(code2, code3);
            code1 = ConnectCodes(code1, code2);
            return code1;
        }
        // else if(find->childnum==4)
        // {
        ArrNode *FindArr = ArrTable;
        while (FindArr != NULL)
        {
            if (strcmp(FindArr->name, root->childarr[0]->childarr[0]->childarr[0]->value) == 0)
            {
                break;
            }
            else
            {
                FindArr = FindArr->next;
            }
        }
        Type type = FindArr->type->u.array.ElementType;
        int StructSize = GetStructTypeSize(type->u.StructTypeName);
        Operand ArrBase = FindVar(root->childarr[0]->childarr[0]->childarr[0]->value);
        if (ArrBase == NULL)
        {
            ArrBase = new_var(root->childarr[0]->childarr[0]->childarr[0]->value, false);
        }
        Operand t0 = new_temp();
        Operand index1 = new_temp();
        Operand offset1 = new_temp();
        InterCodes code1 = translate_Exp(root->childarr[0]->childarr[2], index1);
        InterCodes code2 = (InterCodes)malloc(sizeof(struct InterCodes_));
        code2->next = NULL;
        code2->prev = NULL;
        code2->code.kind = MUL;
        code2->code.u.TwoOp.result = offset1;
        code2->code.u.TwoOp.op1 = index1;
        code2->code.u.TwoOp.op2 = (Operand)malloc(sizeof(struct Operand_));
        code2->code.u.TwoOp.op2->kind = CONSTANT;
        code2->code.u.TwoOp.op2->u.value = StructSize;

        Operand base1 = new_temp();
        InterCodes code3 = (InterCodes)malloc(sizeof(struct InterCodes_));
        code3->next = NULL;
        code3->prev = NULL;
        if (!ArrBase->IfPara)
        {
            code3->code.kind = GET_ADDR_ASSIGN;
        }
        else
        {
            code3->code.kind = ASSIGN;
        }
        code3->code.u.assign.left = base1;
        code3->code.u.assign.right = ArrBase;
        Operand addr1 = new_temp();
        InterCodes code4 = (InterCodes)malloc(sizeof(struct InterCodes_));
        code4->next = NULL;
        code4->prev = NULL;
        code4->code.kind = ADD;
        code4->code.u.TwoOp.result = addr1;
        code4->code.u.TwoOp.op1 = base1;
        code4->code.u.TwoOp.op2 = offset1;
        Operand offset2 = (Operand)malloc(sizeof(struct Operand_));
        offset2->kind = CONSTANT;
        offset2->u.value = GetOffsetByStructTypeName(type->u.StructTypeName, root->childarr[2]->value);
        Operand addr2 = new_temp();
        InterCodes code5 = (InterCodes)malloc(sizeof(struct InterCodes_));
        code5->next = NULL;
        code5->prev = NULL;
        code5->code.kind = ADD;
        code5->code.u.TwoOp.result = addr2;
        code5->code.u.TwoOp.op1 = addr1;
        code5->code.u.TwoOp.op2 = offset2;

        InterCodes code6 = (InterCodes)malloc(sizeof(struct InterCodes_));
        code6->next = NULL;
        code6->prev = NULL;
        code6->code.kind = GET_ADDR_CONTENT_ASSIGN;
        code6->code.u.assign.left = place;
        code6->code.u.assign.right = addr2;
        code5 = ConnectCodes(code5, code6);
        code4 = ConnectCodes(code4, code5);
        code3 = ConnectCodes(code3, code4);
        code2 = ConnectCodes(code2, code3);
        code1 = ConnectCodes(code1, code2);
        return code1;
    }
    return NULL;
}

InterCodes translate_BaseAddr(const node* root, Operand place)
{
    if(root->childnum==1)
    {
        Operand v = FindVar(root->childarr[0]->value);
        if(v==NULL)
        {
            v = new_var(root->childarr[0]->value, false);
        }
        InterCodes code = (InterCodes)malloc(sizeof(struct InterCodes_));
        code->next = NULL;
        code->prev = NULL;
        if(!v->IfPara)
        {
            code->code.kind = GET_ADDR_ASSIGN;
        }
        else
        {
            code->code.kind = ASSIGN;
        }
        code->code.u.assign.left = place;
        code->code.u.assign.right = v;
        return code;
    }
    else if(root->childnum==3 && strcmp(root->childarr[1]->type, "DOT")==0)
    {
        node* p = root->childarr[0];
        while(strcmp(p->childarr[0]->type, "ID")!=0)
        {
            p = p->childarr[0];
        }
        int offset = GetOffset(p->childarr[0]->value, root->childarr[2]->value);
        Operand v = FindVar(p->childarr[0]->value);
        if(v==NULL)
        {
            v = new_var(p->childarr[0]->value, false);
        }
        Operand t1 = new_temp();
        InterCodes code1 = (InterCodes)malloc(sizeof(struct InterCodes_));
        code1->prev = NULL;
        code1->next = NULL;
        if (!v->IfPara)
        {
            code1->code.kind = GET_ADDR_ASSIGN;
        }
        else
        {
            code1->code.kind = ASSIGN;
        }
        code1->code.u.assign.left = t1;
        code1->code.u.assign.right = v;
        InterCodes code2 = (InterCodes)malloc(sizeof(struct InterCodes_));
        code2->next = NULL;
        code2->prev = NULL;
        code2->code.kind = ADD;
        code2->code.u.TwoOp.result = place;
        code2->code.u.TwoOp.op1 = t1;
        code2->code.u.TwoOp.op2 = (Operand)malloc(sizeof(struct Operand_));
        code2->code.u.TwoOp.op2->kind = CONSTANT;
        code2->code.u.TwoOp.op2->u.value = offset;
        code1 = ConnectCodes(code1, code2);
        return code1;
    }
}

InterCodes translate_Args(const node* root,  ArgsNode* pArgsList)
{
    if(root->childnum==1)
    {
        Operand t1 = new_temp();
        InterCodes code1 = translate_Exp(root->childarr[0], t1);
        ArgsNode NewArgs = (ArgsNode)malloc(sizeof(struct ArgsNode_));
        NewArgs->next = *pArgsList;
        NewArgs->args = t1;
        if(strcmp(root->childarr[0]->childarr[0]->type, "ID")==0)
        {
            StructNode* p = StructTable;
            while(p!=NULL)
            {
                if(strcmp(p->name, root->childarr[0]->childarr[0]->value)==0)
                {
                    break;
                }
                p = p->next;
            }
            if(p!=NULL)
            {
                NewArgs->args = FindVar(root->childarr[0]->childarr[0]->value);
            }
        }
        *pArgsList = NewArgs;
        return code1;
    }
    else if(root->childnum==3)
    {
        Operand t1 = new_temp();
        InterCodes code1 = translate_Exp(root->childarr[0], t1);
        ArgsNode NewArgs = (ArgsNode)malloc(sizeof(struct ArgsNode_));
        NewArgs->next = *pArgsList;
        NewArgs->args = t1;
        *pArgsList = NewArgs;
        if(strcmp(root->childarr[0]->childarr[0]->type, "ID")==0)
        {
            StructNode* p = StructTable;
            while(p!=NULL)
            {
                if(strcmp(p->name, root->childarr[0]->childarr[0]->value)==0)
                {
                    break;
                }
                p = p->next;
            }
            if(p!=NULL)
            {
                NewArgs->args = FindVar(root->childarr[0]->childarr[0]->value);
            }
        }
        InterCodes code2 = translate_Args(root->childarr[2], pArgsList);
        code1 = ConnectCodes(code1, code2);
        return code1;
    }
    return NULL;
}

InterCodes translate_Stmt(const node* root)
{
    if(strcmp(root->childarr[0]->type, "Exp")==0)
    {
        return translate_Exp(root->childarr[0], NULL);
    }
    else if(strcmp(root->childarr[0]->type, "CompSt")==0)
    {
        return translate_CompSt(root->childarr[0]);
    }
    else if(strcmp(root->childarr[0]->type, "RETURN")==0)
    {
        Operand t1 = new_temp();
        InterCodes code1 = translate_Exp(root->childarr[1], t1);
        InterCodes code2 = (InterCodes)malloc(sizeof(struct InterCodes_));
        code2->next = NULL;
        code2->prev = NULL;
        code2->code.kind = ICRETURN;
        code2->code.u.single.op = t1;
        code1 = ConnectCodes(code1, code2);
        return code1;
    }
    else if(strcmp(root->childarr[0]->type, "IF")==0 && root->childnum==5)
    {
        Operand label1 = new_label();
        Operand label2 = new_label();
        InterCodes code1 = translate_Cond(root->childarr[2], label1, label2);
        InterCodes code2 = translate_Stmt(root->childarr[4]);
        InterCodes l1 = (InterCodes)malloc(sizeof(struct InterCodes_));
        l1->next = NULL;
        l1->prev = NULL;
        l1->code.kind = LABEL;
        l1->code.u.single.op = label1;
        InterCodes l2 = (InterCodes)malloc(sizeof(struct InterCodes_));
        l2->next = NULL;
        l2->prev = NULL;
        l2->code.kind = LABEL;
        l2->code.u.single.op = label2;
        code2 = ConnectCodes(code2, l2);
        l1 = ConnectCodes(l1, code2);
        code1 = ConnectCodes(code1, l1);
        return code1;
    }
    else if(strcmp(root->childarr[0]->type, "IF")==0 && root->childnum==7)
    {
        Operand label1 = new_label();
        Operand label2 = new_label();
        Operand label3 = new_label();
        InterCodes code1 = translate_Cond(root->childarr[2], label1, label2);
        InterCodes code2 = translate_Stmt(root->childarr[4]);
        InterCodes code3 = translate_Stmt(root->childarr[6]);
        InterCodes l1 = (InterCodes)malloc(sizeof(struct InterCodes_));
        l1->next = NULL;
        l1->prev = NULL;
        l1->code.kind = LABEL;
        l1->code.u.single.op = label1;
        InterCodes l2 = (InterCodes)malloc(sizeof(struct InterCodes_));
        l2->next = NULL;
        l2->prev = NULL;
        l2->code.kind = LABEL;
        l2->code.u.single.op = label2;
        InterCodes l3 = (InterCodes)malloc(sizeof(struct InterCodes_));
        l3->next = NULL;
        l3->prev = NULL;
        l3->code.kind = LABEL;
        l3->code.u.single.op = label3;
        InterCodes g3 = (InterCodes)malloc(sizeof(struct InterCodes_));
        g3->next = NULL;
        g3->prev = NULL;
        g3->code.kind = GOTO;
        g3->code.u.single.op = label3;
        code3 = ConnectCodes(code3, l3);
        l2 = ConnectCodes(l2, code3);
        g3 = ConnectCodes(g3, l2);
        code2 = ConnectCodes(code2, g3);
        l1 = ConnectCodes(l1, code2);
        code1 = ConnectCodes(code1, l1);
        return code1;
    }
    else if(strcmp(root->childarr[0]->type, "WHILE")==0)
    {
        Operand label1 = new_label();
        Operand label2 = new_label();
        Operand label3 = new_label();
        InterCodes code1 = translate_Cond(root->childarr[2], label2, label3);
        InterCodes code2 = translate_Stmt(root->childarr[4]);
        InterCodes l1 = (InterCodes)malloc(sizeof(struct InterCodes_));
        l1->next = NULL;
        l1->prev = NULL;
        l1->code.kind = LABEL;
        l1->code.u.single.op = label1;
        InterCodes l2 = (InterCodes)malloc(sizeof(struct InterCodes_));
        l2->next = NULL;
        l2->prev = NULL;
        l2->code.kind = LABEL;
        l2->code.u.single.op = label2;
        InterCodes l3 = (InterCodes)malloc(sizeof(struct InterCodes_));
        l3->next = NULL;
        l3->prev = NULL;
        l3->code.kind = LABEL;
        l3->code.u.single.op = label3;
        InterCodes g1 = (InterCodes)malloc(sizeof(struct InterCodes_));
        g1->next = NULL;
        g1->prev = NULL;
        g1->code.kind = GOTO;
        g1->code.u.single.op = label1;
        g1 = ConnectCodes(g1, l3);
        code2 = ConnectCodes(code2, g1);
        l2 = ConnectCodes(l2, code2);
        code1 = ConnectCodes(code1, l2);
        l1 = ConnectCodes(l1, code1);
        return l1;
    }
    return NULL;
}

InterCodes translate_CompSt(const node* root)
{
    InterCodes deflist = translate_DefList(root->childarr[1]);
    InterCodes stmtlist = translate_StmtList(root->childarr[2]);
    deflist = ConnectCodes(deflist, stmtlist);
    return deflist;
}

InterCodes translate_DefList(const node* root)
{
    if(root->childnum==1)
    {
        return NULL;
    }
    else if(root->childnum==2)
    {
        InterCodes code1 = translate_Def(root->childarr[0]);
        InterCodes code2 = translate_DefList(root->childarr[1]);
        if(code1==NULL)
        {
            return code2;
        }
        if(code2==NULL)
        {
            return code1;
        }
        code1 = ConnectCodes(code1, code2);
        return code1;
    }
}

InterCodes translate_Def(const node* root)
{
    if(strcmp(root->childarr[0]->childarr[0]->type, "TYPE")==0)
        return translate_DecList(root->childarr[1]);
    else if(strcmp(root->childarr[0]->childarr[0]->type, "StructSpecifier")==0)
    {
        return translate_DecList_Struct(root->childarr[1], root->childarr[0]->childarr[0]->childarr[1]->childarr[0]->value);
    }
}

InterCodes translate_DecList_Struct(const node* root, char* StructTypeName)
{
    if(root->childnum==1)
    {
        InterCodes code1 = translate_Dec_Struct(root->childarr[0], StructTypeName);
        return code1;
    }
    else if(root->childnum==3)
    {
        InterCodes code1 = translate_Dec_Struct(root->childarr[0], StructTypeName);
        InterCodes code2 = translate_DecList_Struct(root->childarr[2], StructTypeName);
        code1 = ConnectCodes(code1, code2);
        return code1;
    }
}

InterCodes translate_DecList(const node* root)
{
    if(root->childnum==1)
    {
        InterCodes code1 = translate_Dec(root->childarr[0]);
        return code1;
    }
    else if(root->childnum==3)
    {
        InterCodes code1 = translate_Dec(root->childarr[0]);
        InterCodes code2 = translate_DecList(root->childarr[2]);
        code1 = ConnectCodes(code1, code2);
        return code1;
    }
}

InterCodes translate_Dec_Struct(const node* root, char* StructTypeName)
{
    if(root->childnum==1)
    {
        if(root->childarr[0]->childnum==1)
        {
            Operand v = FindVar(root->childarr[0]->childarr[0]->value);
            if(v==NULL)
            {
                v = new_var(root->childarr[0]->childarr[0]->value, false);
            }
            InterCodes code = (InterCodes)malloc(sizeof(struct InterCodes_));
            code->next = NULL;
            code->prev = NULL;
            code->code.kind = DEC;
            code->code.u.Dec.op = v;
            code->code.u.Dec.size = GetStructTypeSize(StructTypeName);
            return code;
        }
        else if(root->childarr[0]->childnum==4)
        {
            Operand v = FindVar(root->childarr[0]->childarr[0]->childarr[0]->value);
            if(v==NULL)
            {
                v = new_var(root->childarr[0]->childarr[0]->childarr[0]->value, false);
            }
            InterCodes code = (InterCodes)malloc(sizeof(struct InterCodes_));
            code->next = NULL;
            code->prev = NULL;
            code->code.kind = DEC;
            code->code.u.Dec.op = v;
            code->code.u.Dec.size = atoi(root->childarr[0]->childarr[2]->value)*GetStructTypeSize(StructTypeName);
            return code;
        }
    }
}

InterCodes translate_Dec(const node* root)
{
    if(root->childnum==1)
    {
        if(root->childarr[0]->childnum==1)
        {
            return NULL;
        }
        else if(root->childarr[0]->childnum==4)
        {
            if(root->childarr[0]->childarr[0]->childnum==4)
            {
                printf("Cannot translate: Code contains variables of multi-dimensional array type or parameters of array type.\n");
                exit(-1);
            }
            Operand v = FindVar(root->childarr[0]->childarr[0]->childarr[0]->value);
            if(v==NULL)
            {
                v = new_var(root->childarr[0]->childarr[0]->childarr[0]->value, false);
            }
            InterCodes code = (InterCodes)malloc(sizeof(struct InterCodes_));
            code->next = NULL;
            code->prev = NULL;
            code->code.kind = DEC;
            code->code.u.Dec.op = v;
            code->code.u.Dec.size = 4*atoi(root->childarr[0]->childarr[2]->value);
            return code;
        }
    }
    else if(root->childnum==3)
    {
        return translate_VarDecWithAssign(root->childarr[0], root->childarr[2]);
    }
}

InterCodes translate_VarDecWithAssign(const node* root, const node* Exp)
{
    if(root->childnum==1)
    {
        Operand left = FindVar(root->childarr[0]->value);
        if(left==NULL)
        {
            left = new_var(root->childarr[0]->value, false);
        }
        Operand t1 = new_temp();
        InterCodes code1 = translate_Exp(Exp, t1);
        InterCodes code2 = (InterCodes)malloc(sizeof(struct InterCodes_));
        code2->next = NULL;
        code2->prev = NULL;
        code2->code.kind = ASSIGN;
        code2->code.u.assign.left = left;
        code2->code.u.assign.right = t1;
        code1 = ConnectCodes(code1, code2);
        return code1;
    }
    else if(root->childnum==4)
    {
        if(root->childarr[0]->childnum==4)
        {
            printf("Cannot translate: Code contains variables of multi-dimensional array type or parameters of array type.\n");
            exit(-1);
        }
        return NULL;
    }
    return NULL;
}

InterCodes translate_StmtList(const node* root)
{
    if(root->childnum==1)
    {
        return NULL;
    }
    else if(root->childnum==2)
    {
        InterCodes code1 = translate_Stmt(root->childarr[0]);
        InterCodes code2 = translate_StmtList(root->childarr[1]);
        code1 = ConnectCodes(code1, code2);
        return code1;
    }
}

InterCodes translate_Cond(const node* root, Operand LabelTrue, Operand LabelFalse)
{
    if(root->childnum<2)
    {
        Operand t1 = new_temp();
        InterCodes code1 = translate_Exp(root, t1);
        InterCodes code2 = (InterCodes)malloc(sizeof(struct InterCodes_));
        code2->next = NULL;
        code2->prev = NULL;
        code2->code.kind = IFGOTO;
        code2->code.u.ThreeOp.op1 = t1;
        code2->code.u.ThreeOp.op2 = (Operand)malloc(sizeof(struct Operand_));
        code2->code.u.ThreeOp.op2->kind = CONSTANT;
        code2->code.u.ThreeOp.op2->u.value = 0;
        code2->code.u.ThreeOp.label = LabelTrue;
        code2->code.u.ThreeOp.relop = (char*)malloc(10);
        strcpy(code2->code.u.ThreeOp.relop, "!=");
        InterCodes code3 = (InterCodes)malloc(sizeof(struct InterCodes_));
        code3->next = NULL;
        code3->prev = NULL;
        code3->code.kind = GOTO;
        code3->code.u.single.op = LabelFalse;
        code2 = ConnectCodes(code2, code3);
        code1 = ConnectCodes(code1, code2);
        return code1;
    }
    else if(strcmp(root->childarr[1]->type, "RELOP")==0)
    {
        Operand t1 = new_temp();
        Operand t2 = new_temp();
        InterCodes code1 = translate_Exp(root->childarr[0], t1);
        InterCodes code2 = translate_Exp(root->childarr[2], t2);
        char* relop = (char*)malloc((strlen(root->childarr[1]->value)+1)*sizeof(char));
        strcpy(relop, root->childarr[1]->value);
        InterCodes code3 = (InterCodes)malloc(sizeof(struct InterCodes_));
        code3->next = NULL;
        code3->prev = NULL;
        code3->code.kind = IFGOTO;
        code3->code.u.ThreeOp.op1 = t1;
        code3->code.u.ThreeOp.op2 = t2;
        code3->code.u.ThreeOp.label = LabelTrue;
        code3->code.u.ThreeOp.relop = (char*)malloc((strlen(relop)+1)*sizeof(char));
        strcpy(code3->code.u.ThreeOp.relop, relop);
        InterCodes code4 = (InterCodes)malloc(sizeof(struct InterCodes_));
        code4->next = NULL;
        code4->prev = NULL;
        code4->code.kind = GOTO;
        code4->code.u.single.op = LabelFalse;
        code3 = ConnectCodes(code3, code4);
        code2 = ConnectCodes(code2, code3);
        code1 = ConnectCodes(code1, code2);
        return code1;
    }
    else if(strcmp(root->childarr[0]->type, "NOT")==0)
    {
        return translate_Cond(root->childarr[1], LabelFalse, LabelTrue);
    }
    else if(strcmp(root->childarr[1]->type, "AND")==0)
    {
        Operand label1 = new_label();
        InterCodes code1 = translate_Cond(root->childarr[0], label1, LabelFalse);
        InterCodes code2 = translate_Cond(root->childarr[2], LabelTrue, LabelFalse);
        InterCodes l = (InterCodes)malloc(sizeof(struct InterCodes_));
        l->next = NULL;
        l->prev = NULL;
        l->code.kind = LABEL;
        l->code.u.single.op = label1;
        l = ConnectCodes(l, code2);
        code1 = ConnectCodes(code1, l);
        return code1;
    }
    else if(strcmp(root->childarr[1]->type, "OR")==0)
    {
        Operand label1 = new_label();
        InterCodes code1 = translate_Cond(root->childarr[0], LabelTrue, label1);
        InterCodes code2 = translate_Cond(root->childarr[2], LabelTrue, LabelFalse);
        InterCodes l = (InterCodes)malloc(sizeof(struct InterCodes_));
        l->next = NULL;
        l->prev = NULL;
        l->code.kind = LABEL;
        l->code.u.single.op = label1;
        l = ConnectCodes(l, code2);
        code1 = ConnectCodes(code1, l);
        return code1;
    }
    else
    {
        Operand t1 = new_temp();
        InterCodes code1 = translate_Exp(root, t1);
        InterCodes code2 = (InterCodes)malloc(sizeof(struct InterCodes_));
        code2->next = NULL;
        code2->prev = NULL;
        code2->code.kind = IFGOTO;
        code2->code.u.ThreeOp.op1 = t1;
        code2->code.u.ThreeOp.op2 = (Operand)malloc(sizeof(struct Operand_));
        code2->code.u.ThreeOp.op2->kind = CONSTANT;
        code2->code.u.ThreeOp.op2->u.value = 0;
        code2->code.u.ThreeOp.label = LabelTrue;
        code2->code.u.ThreeOp.relop = (char*)malloc(10);
        strcpy(code2->code.u.ThreeOp.relop, "!=");
        InterCodes code3 = (InterCodes)malloc(sizeof(struct InterCodes_));
        code3->next = NULL;
        code3->prev = NULL;
        code3->code.kind = GOTO;
        code3->code.u.single.op = LabelFalse;
        code2 = ConnectCodes(code2, code3);
        code1 = ConnectCodes(code1, code2);
        return code1;
    }
    return NULL;
}

InterCodes translate_ExtDef(const node* root)
{
    if(strcmp(root->childarr[1]->type, "FunDec")==0)
    {
        InterCodes code1 = (InterCodes)malloc(sizeof(struct InterCodes_));
        code1->next = NULL;
        code1->prev = NULL;
        code1->code.kind = ICFUNC;
        code1->code.u.single.op = (Operand)malloc(sizeof(struct Operand_));
        code1->code.u.single.op->kind = OPFUNC;
        code1->code.u.single.op->u.name = (char*)malloc(sizeof(char)*(strlen(root->childarr[1]->childarr[0]->value)+1));
        strcpy(code1->code.u.single.op->u.name, root->childarr[1]->childarr[0]->value);
        InterCodes code2 = NULL;
        if(root->childarr[1]->childnum==4)
        {
            code2 = translate_VarList(root->childarr[1]->childarr[2]);
        }
        InterCodes code3 = translate_CompSt(root->childarr[2]);
        code2 = ConnectCodes(code2, code3);
        code1 = ConnectCodes(code1, code2);
        return code1;
    }
    else
    {
        return NULL;
    }
    return NULL;
}

InterCodes translate_VarList(const node* root)
{
    if(root->childnum==1)
    {
        return translate_ParamDec(root->childarr[0]);
    }
    else if(root->childnum==3)
    {
        InterCodes code1 = translate_ParamDec(root->childarr[0]);
        InterCodes code2 = translate_VarList(root->childarr[2]);
        code1 = ConnectCodes(code1, code2);
        return code1;
    }
    return NULL;
}

InterCodes translate_ParamDec(const node* root)
{
    if(root->childarr[1]->childnum==4)
    {
        printf("Cannot translate: Code contains variables of multi-dimensional array type or parameters of array type.\n");
        exit(-1);
    }
    Operand v = FindVar(root->childarr[1]->childarr[0]->value);
    if(v==NULL)
    {
        v = new_var(root->childarr[1]->childarr[0]->value, true);
    }
    InterCodes code = (InterCodes)malloc(sizeof(struct InterCodes_));
    code->next = NULL;
    code->prev = NULL;
    code->code.kind = PARAM;
    code->code.u.single.op = v;
    return code;
}

void TranslateToInterCodes(const node* root)
{
    if(root==NULL)
    {
        return;
    }
    for(int i=0; i<root->childnum; ++i)
    {
        if(strcmp(root->childarr[i]->type, "ExtDef")==0)
        {
            InterCodes code = translate_ExtDef(root->childarr[i]);
            AddInterCodes(code);
        }
        else
        {
            TranslateToInterCodes(root->childarr[i]);
        }
    }
}