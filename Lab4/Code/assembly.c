#include "assembly.h"
#include "assert.h"
#include "intermediate.h"

AsmCode AsmCodeListHead = NULL;
AsmCode AsmCodeListTail = NULL;
VarOffset VarOffsetTableHead = NULL;
VarOffset VarOffsetTableTail = NULL;
int CountArg = 0;
int CountParam = 0;
int LocalOffset = 0;

int GetStackOffset(Operand op)
{
    VarOffset p = VarOffsetTableHead;
    while (p != NULL)
    {
        if (op->kind == VARIABLE)
        {
            if (p->kind == 1 && p->var_no == op->u.var_no)
            {
                return p->offset;
            }
        }
        else if (op->kind == TEMP)
        {
            if (p->kind == 2 && p->var_no == op->u.var_no)
            {
                return p->offset;
            }
        }
        p = p->next;
    }
    return 1;
}

void AddNewVarOffset(Operand op, int offset)
{
    VarOffset newone = (VarOffset)malloc(sizeof(struct VarOffset_));
    newone->next = NULL;
    newone->var_no = op->u.var_no;
    newone->offset = offset;
    if (op->kind == VARIABLE)
    {
        newone->kind = 1;
    }
    else if (op->kind == TEMP)
    {
        newone->kind = 2;
    }
    if (VarOffsetTableHead == NULL)
    {
        VarOffsetTableHead = newone;
        VarOffsetTableTail = newone;
    }
    else
    {
        VarOffsetTableTail->next = newone;
        VarOffsetTableTail = newone;
    }
}

void InsertAsmCode(char *code, bool iftab)
{
    AsmCode NewCode = (AsmCode)malloc(sizeof(struct AsmCode_));
    NewCode->code = (char *)malloc((strlen(code) + 1) * sizeof(char));
    strcpy(NewCode->code, code);
    NewCode->iftab = iftab;
    NewCode->next = NULL;
    if (AsmCodeListHead == NULL)
    {
        AsmCodeListHead = NewCode;
        AsmCodeListTail = NewCode;
    }
    else
    {
        AsmCodeListTail->next = NewCode;
        AsmCodeListTail = NewCode;
    }
}

void PrintAllAssemblyCodes(FILE *f)
{
    AsmCode p = AsmCodeListHead;
    while (p != NULL)
    {
        if (p->iftab)
        {
            fprintf(f, "\t%s\n", p->code);
        }
        else
        {
            fprintf(f, "%s\n", p->code);
        }
        p = p->next;
    }
}

void LoadToReg(int offset, int RegNo)
{
    char *codel = (char *)malloc(1000);
    sprintf(codel, "lw %s, %d($fp)", RegName[RegNo], offset);
    InsertAsmCode(codel, true);
}

void StoreToStack(int offset, int RegNo)
{
    char *codes = (char *)malloc(1000);
    sprintf(codes, "sw %s, %d($fp)", RegName[RegNo], offset);
    InsertAsmCode(codes, true);
}

void InitAsmCodeList()
{
    InsertAsmCode(".data", false);
    InsertAsmCode("_prompt: .asciiz \"Enter an integer:\"", false);
    InsertAsmCode("_ret: .asciiz \"\\n\"", false);
    InsertAsmCode(".globl main", false);
    InterCodes find = InterCodeListHead;
    char *DecCode = (char *)malloc(1000);
    while (find != NULL)
    {
        if (find->code.kind == DEC)
        {
            if (find->code.u.Dec.op->kind == VARIABLE)
            {
                sprintf(DecCode, "v%d: .space %d", find->code.u.Dec.op->u.var_no, find->code.u.Dec.size);
                InsertAsmCode(DecCode, false);
            }
            else if (find->code.u.Dec.op->kind == TEMP)
            {
                sprintf(DecCode, "t%d: .space %d", find->code.u.Dec.op->u.var_no, find->code.u.Dec.size);
                InsertAsmCode(DecCode, false);
            }
        }
        find = find->next;
    }
    InsertAsmCode(".text", false);
    InsertAsmCode("read:", false);
    InsertAsmCode("li $v0, 4", true);
    InsertAsmCode("la $a0, _prompt", true);
    InsertAsmCode("syscall", true);
    InsertAsmCode("li $v0, 5", true);
    InsertAsmCode("syscall", true);
    InsertAsmCode("jr $ra", true);
    InsertAsmCode("", false);
    InsertAsmCode("write:", false);
    InsertAsmCode("li $v0, 1", true);
    InsertAsmCode("syscall", true);
    InsertAsmCode("li $v0, 4", true);
    InsertAsmCode("la $a0, _ret", true);
    InsertAsmCode("syscall", true);
    InsertAsmCode("move $v0, $0", true);
    InsertAsmCode("jr $ra", true);
    InsertAsmCode("", false);
}

int AllocateInStack(Operand op)
{
    char *code = (char *)malloc(1000);
    sprintf(code, "addi $sp, $sp, -4");
    InsertAsmCode(code, true);
    LocalOffset -= 4;
    AddNewVarOffset(op, LocalOffset);
    return LocalOffset;
}

void SaveReg()
{
    char *savecode = (char *)malloc(100);
    for (int i = 4; i <= 25; ++i)
    {
        sprintf(savecode, "addi $sp, $sp, -4");
        InsertAsmCode(savecode, true);
        sprintf(savecode, "sw %s, 0($sp)", RegName[i]);
        InsertAsmCode(savecode, true);
    }
}

void RecoverReg()
{
    char *recovercode = (char *)malloc(100);
    for (int i = 25; i >= 4; --i)
    {
        sprintf(recovercode, "lw %s, %d($fp)", RegName[i], -8 - 4 * (i - 3));
        InsertAsmCode(recovercode, true);
    }
}

void TranslateToAsmCodes()
{
    InitAsmCodeList();
    InterCodes p = InterCodeListHead;
    char *code = (char *)malloc(1000);
    while (p != NULL)
    {
        if (p->code.kind == ASSIGN)
        {
            int left = FindReg(p->code.u.assign.left);
            if (left == -1)
            {
                int offset1 = GetStackOffset(p->code.u.assign.left);
                if (offset1 <= 0)
                {
                    if (p->code.u.assign.right->kind == CONSTANT)
                    {
                        sprintf(code, "li %s, %d", RegName[8], p->code.u.assign.right->u.value);
                        InsertAsmCode(code, true);
                        StoreToStack(offset1, 8);
                    }
                    else
                    {
                        int right = FindReg(p->code.u.assign.right);
                        if (right == -1)
                        {
                            int offset2 = GetStackOffset(p->code.u.assign.right);
                            LoadToReg(offset2, 9);
                            sprintf(code, "move %s, %s", RegName[8], RegName[9]);
                            InsertAsmCode(code, true);
                            StoreToStack(offset1, 8);
                        }
                        else
                        {
                            sprintf(code, "move %s, %s", RegName[8], RegName[right]);
                            InsertAsmCode(code, true);
                            StoreToStack(offset1, 8);
                        }
                    }
                }
                else if (offset1 > 0)
                {
                    left = FindFreeReg();
                    if (left == -1)
                    {
                        offset1 = AllocateInStack(p->code.u.assign.left);
                        if (p->code.u.assign.right->kind == CONSTANT)
                        {
                            sprintf(code, "li %s, %d", RegName[8], p->code.u.assign.right->u.value);
                            InsertAsmCode(code, true);
                            StoreToStack(offset1, 8);
                        }
                        else
                        {
                            int right = FindReg(p->code.u.assign.right);
                            if (right == -1)
                            {
                                int offset2 = GetStackOffset(p->code.u.assign.right);
                                LoadToReg(offset2, 9);
                                sprintf(code, "move %s, %s", RegName[8], RegName[9]);
                                InsertAsmCode(code, true);
                                StoreToStack(offset1, 8);
                            }
                            else
                            {
                                sprintf(code, "move %s, %s", RegName[8], RegName[right]);
                                InsertAsmCode(code, true);
                                StoreToStack(offset1, 8);
                            }
                        }
                    }
                    else if (left >= 0)
                    {
                        if (p->code.u.assign.left->kind == VARIABLE)
                        {
                            AllReg[left].kind = 2;
                        }
                        else if (p->code.u.assign.left->kind == TEMP)
                        {
                            AllReg[left].kind = 3;
                        }
                        AllReg[left].num = p->code.u.assign.left->u.var_no;
                        AllReg[left].used = 1;
                        if (p->code.u.assign.right->kind == CONSTANT)
                        {
                            sprintf(code, "li %s, %d", RegName[left], p->code.u.assign.right->u.value);
                            InsertAsmCode(code, true);
                        }
                        else
                        {
                            int right = FindReg(p->code.u.assign.right);
                            if (right == -1)
                            {
                                int offset2 = GetStackOffset(p->code.u.assign.right);
                                LoadToReg(offset2, 8);
                                sprintf(code, "move %s, %s", RegName[left], RegName[8]);
                                InsertAsmCode(code, true);
                            }
                            else
                            {
                                sprintf(code, "move %s, %s", RegName[left], RegName[right]);
                                InsertAsmCode(code, true);
                            }
                        }
                    }
                }
            }
            else if (left >= 0)
            {
                if (p->code.u.assign.right->kind == CONSTANT)
                {
                    sprintf(code, "li %s, %d", RegName[left], p->code.u.assign.right->u.value);
                    InsertAsmCode(code, true);
                }
                else
                {
                    int right = FindReg(p->code.u.assign.right);
                    if (right == -1)
                    {
                        int offset2 = GetStackOffset(p->code.u.assign.right);
                        LoadToReg(offset2, 8);
                        sprintf(code, "move %s, %s", RegName[left], RegName[8]);
                        InsertAsmCode(code, true);
                    }
                    else
                    {
                        sprintf(code, "move %s, %s", RegName[left], RegName[right]);
                        InsertAsmCode(code, true);
                    }
                }
            }
        }
        else if (p->code.kind == GET_ADDR_ASSIGN)
        {
            int left = FindReg(p->code.u.assign.left);
            if (left >= 0)
            {
                if (p->code.u.assign.right->kind == VARIABLE)
                {
                    sprintf(code, "la %s, v%d", RegName[left], p->code.u.assign.right->u.var_no);
                }
                else if (p->code.u.assign.right->kind == TEMP)
                {
                    sprintf(code, "la %s, t%d", RegName[left], p->code.u.assign.right->u.var_no);
                }
                InsertAsmCode(code, true);
            }
            else if (left == -1)
            {
                int offset = GetStackOffset(p->code.u.assign.left);
                if (offset <= 0)
                {
                    if (p->code.u.assign.right->kind == VARIABLE)
                    {
                        sprintf(code, "la %s, v%d", RegName[8], p->code.u.assign.right->u.var_no);
                    }
                    else if (p->code.u.assign.right->kind == TEMP)
                    {
                        sprintf(code, "la %s, t%d", RegName[8], p->code.u.assign.right->u.var_no);
                    }
                    InsertAsmCode(code, true);
                    StoreToStack(offset, 8);
                }
                else if (offset > 0)
                {
                    offset = AllocateInStack(p->code.u.assign.left);
                    if (p->code.u.assign.right->kind == VARIABLE)
                    {
                        sprintf(code, "la %s, v%d", RegName[8], p->code.u.assign.right->u.var_no);
                    }
                    else if (p->code.u.assign.right->kind == TEMP)
                    {
                        sprintf(code, "la %s, t%d", RegName[8], p->code.u.assign.right->u.var_no);
                    }
                    InsertAsmCode(code, true);
                    StoreToStack(offset, 8);
                }
            }
        }
        else if (p->code.kind == GET_ADDR_CONTENT_ASSIGN)
        {
            int left = FindReg(p->code.u.assign.left);
            if (left == -1)
            {
                int offset1 = GetStackOffset(p->code.u.assign.left);
                if (offset1 <= 0)
                {
                    int right = FindReg(p->code.u.assign.right);
                    if (right == -1)
                    {
                        int offset2 = GetStackOffset(p->code.u.assign.right);
                        if (offset2 <= 0)
                        {
                            LoadToReg(offset2, 8);
                            sprintf(code, "lw %s, 0(%s)", RegName[9], RegName[8]);
                            InsertAsmCode(code, true);
                            StoreToStack(offset1, 9);
                        }
                    }
                    else if (right >= 0)
                    {
                        sprintf(code, "lw %s, 0(%s)", RegName[9], RegName[right]);
                        InsertAsmCode(code, true);
                        StoreToStack(offset1, 9);
                    }
                }
                else if (offset1 > 0)
                {
                    offset1 = AllocateInStack(p->code.u.assign.left);
                    int right = FindReg(p->code.u.assign.right);
                    if (right == -1)
                    {
                        int offset2 = GetStackOffset(p->code.u.assign.right);
                        if (offset2 <= 0)
                        {
                            LoadToReg(offset2, 8);
                            sprintf(code, "lw %s, 0(%s)", RegName[9], RegName[8]);
                            InsertAsmCode(code, true);
                            StoreToStack(offset1, 9);
                        }
                    }
                    else if (right >= 0)
                    {
                        sprintf(code, "lw %s, 0(%s)", RegName[9], RegName[right]);
                        InsertAsmCode(code, true);
                        StoreToStack(offset1, 9);
                    }
                }
            }
            else if (left >= 0)
            {
                int right = FindReg(p->code.u.assign.right);
                if (right == -1)
                {
                    int offset2 = GetStackOffset(p->code.u.assign.right);
                    if (offset2 <= 0)
                    {
                        LoadToReg(offset2, 8);
                        sprintf(code, "lw %s, 0(%s)", RegName[left], RegName[8]);
                        InsertAsmCode(code, true);
                    }
                }
                else if (right >= 0)
                {
                    sprintf(code, "lw %s, 0(%s)", RegName[left], RegName[right]);
                    InsertAsmCode(code, true);
                }
            }
        }
        else if (p->code.kind == ASSIGN_TO_ADDR)
        {
            int targetx = FindReg(p->code.u.assign.left);
            if (targetx == -1)
            {
                int offset1 = GetStackOffset(p->code.u.assign.left);
                if (offset1 <= 0)
                {
                    int targety = FindReg(p->code.u.assign.right);
                    if (targety == -1)
                    {
                        int offset2 = GetStackOffset(p->code.u.assign.right);
                        if (offset2 <= 0)
                        {
                            LoadToReg(offset1, 8);
                            LoadToReg(offset2, 9);
                            sprintf(code, "sw %s, 0(%s)", RegName[9], RegName[8]);
                            InsertAsmCode(code, true);
                        }
                    }
                    else if (targety >= 0)
                    {
                        LoadToReg(offset1, 8);
                        sprintf(code, "sw %s, 0(%s)", RegName[targety], RegName[8]);
                        InsertAsmCode(code, true);
                    }
                }
            }
            else if (targetx >= 0)
            {
                int targety = FindReg(p->code.u.assign.right);
                if (targety == -1)
                {
                    int offset2 = GetStackOffset(p->code.u.assign.right);
                    if (offset2 <= 0)
                    {
                        LoadToReg(offset2, 8);
                        sprintf(code, "sw %s, 0(%s)", RegName[8], RegName[targetx]);
                        InsertAsmCode(code, true);
                    }
                }
                else if (targety >= 0)
                {
                    sprintf(code, "sw %s, 0(%s)", RegName[targety], RegName[targetx]);
                    InsertAsmCode(code, true);
                }
            }
        }
        else if (p->code.kind == ADD)
        {
            int result = FindReg(p->code.u.TwoOp.result);
            if (result == -1)
            {
                int offset = GetStackOffset(p->code.u.TwoOp.result);
                if (offset <= 0)
                {
                    int first = FindReg(p->code.u.TwoOp.op1);
                    if (first == -1)
                    {
                        int offset1 = GetStackOffset(p->code.u.TwoOp.op1);
                        if (p->code.u.TwoOp.op2->kind == CONSTANT)
                        {
                            sprintf(code, "li %s, %d", RegName[10], p->code.u.TwoOp.op2->u.value);
                            InsertAsmCode(code, true);
                            LoadToReg(offset1, 9);
                            sprintf(code, "add %s, %s, %s", RegName[8], RegName[9], RegName[10]);
                            InsertAsmCode(code, true);
                            StoreToStack(offset, 8);
                        }
                        else
                        {
                            int second = FindReg(p->code.u.TwoOp.op2);
                            if (second == -1)
                            {
                                int offset2 = GetStackOffset(p->code.u.TwoOp.op2);
                                LoadToReg(offset1, 9);
                                LoadToReg(offset2, 10);
                                sprintf(code, "add %s, %s, %s", RegName[8], RegName[9], RegName[10]);
                                InsertAsmCode(code, true);
                                StoreToStack(offset, 8);
                            }
                            else if (second >= 0)
                            {
                                LoadToReg(offset1, 9);
                                sprintf(code, "add %s, %s, %s", RegName[8], RegName[9], RegName[second]);
                                InsertAsmCode(code, true);
                                StoreToStack(offset, 8);
                            }
                        }
                    }
                    else if (first >= 0)
                    {
                        if (p->code.u.TwoOp.op2->kind == CONSTANT)
                        {
                            sprintf(code, "li %s, %d", RegName[10], p->code.u.TwoOp.op2->u.value);
                            InsertAsmCode(code, true);
                            sprintf(code, "add %s, %s, %s", RegName[8], RegName[first], RegName[10]);
                            InsertAsmCode(code, true);
                            StoreToStack(offset, 8);
                        }
                        else
                        {
                            int second = FindReg(p->code.u.TwoOp.op2);
                            if (second == -1)
                            {
                                int offset2 = GetStackOffset(p->code.u.TwoOp.op2);
                                LoadToReg(offset2, 9);
                                sprintf(code, "add %s, %s, %s", RegName[8], RegName[first], RegName[9]);
                                InsertAsmCode(code, true);
                                StoreToStack(offset, 8);
                            }
                            else if (second >= 0)
                            {
                                sprintf(code, "add %s, %s, %s", RegName[8], RegName[first], RegName[second]);
                                InsertAsmCode(code, true);
                                StoreToStack(offset, 8);
                            }
                        }
                    }
                }
                else if (offset > 0)
                {
                    result = FindFreeReg();
                    if (result == -1)
                    {
                        offset = AllocateInStack(p->code.u.TwoOp.result);
                        int first = FindReg(p->code.u.TwoOp.op1);
                        if (first == -1)
                        {
                            int offset1 = GetStackOffset(p->code.u.TwoOp.op1);
                            if (p->code.u.TwoOp.op2->kind == CONSTANT)
                            {
                                sprintf(code, "li %s, %d", RegName[10], p->code.u.TwoOp.op2->u.value);
                                InsertAsmCode(code, true);
                                LoadToReg(offset1, 9);
                                sprintf(code, "add %s, %s, %s", RegName[8], RegName[9], RegName[10]);
                                InsertAsmCode(code, true);
                                StoreToStack(offset, 8);
                            }
                            else
                            {
                                int second = FindReg(p->code.u.TwoOp.op2);
                                if (second == -1)
                                {
                                    int offset2 = GetStackOffset(p->code.u.TwoOp.op2);
                                    LoadToReg(offset1, 9);
                                    LoadToReg(offset2, 10);
                                    sprintf(code, "add %s, %s, %s", RegName[8], RegName[9], RegName[10]);
                                    InsertAsmCode(code, true);
                                    StoreToStack(offset, 8);
                                }
                                else if (second >= 0)
                                {
                                    LoadToReg(offset1, 9);
                                    sprintf(code, "add %s, %s, %s", RegName[8], RegName[9], RegName[second]);
                                    InsertAsmCode(code, true);
                                    StoreToStack(offset, 8);
                                }
                            }
                        }
                        else if (first >= 0)
                        {
                            if (p->code.u.TwoOp.op2->kind == CONSTANT)
                            {
                                sprintf(code, "li %s, %d", RegName[10], p->code.u.TwoOp.op2->u.value);
                                InsertAsmCode(code, true);
                                sprintf(code, "add %s, %s, %s", RegName[8], RegName[first], RegName[10]);
                                InsertAsmCode(code, true);
                                StoreToStack(offset, 8);
                            }
                            else
                            {
                                int second = FindReg(p->code.u.TwoOp.op2);
                                if (second == -1)
                                {
                                    int offset2 = GetStackOffset(p->code.u.TwoOp.op2);
                                    LoadToReg(offset2, 9);
                                    sprintf(code, "add %s, %s, %s", RegName[8], RegName[first], RegName[9]);
                                    InsertAsmCode(code, true);
                                    StoreToStack(offset, 8);
                                }
                                else if (second >= 0)
                                {
                                    sprintf(code, "add %s, %s, %s", RegName[8], RegName[first], RegName[second]);
                                    InsertAsmCode(code, true);
                                    StoreToStack(offset, 8);
                                }
                            }
                        }
                    }
                    else if (result >= 0)
                    {
                        if (p->code.u.TwoOp.result->kind == VARIABLE)
                        {
                            AllReg[result].kind = 2;
                        }
                        else if (p->code.u.TwoOp.result->kind == TEMP)
                        {
                            AllReg[result].kind = 3;
                        }
                        AllReg[result].num = p->code.u.TwoOp.result->u.var_no;
                        AllReg[result].used = 1;
                        int first = FindReg(p->code.u.TwoOp.op1);
                        if (first == -1)
                        {
                            int offset1 = GetStackOffset(p->code.u.TwoOp.op1);
                            if (p->code.u.TwoOp.op2->kind == CONSTANT)
                            {
                                sprintf(code, "li %s, %d", RegName[10], p->code.u.TwoOp.op2->u.value);
                                InsertAsmCode(code, true);
                                LoadToReg(offset1, 9);
                                sprintf(code, "add %s, %s, %s", RegName[result], RegName[9], RegName[10]);
                                InsertAsmCode(code, true);
                            }
                            else
                            {
                                int second = FindReg(p->code.u.TwoOp.op2);
                                if (second == -1)
                                {
                                    int offset2 = GetStackOffset(p->code.u.TwoOp.op2);
                                    LoadToReg(offset1, 8);
                                    LoadToReg(offset2, 9);
                                    sprintf(code, "add %s, %s, %s", RegName[result], RegName[8], RegName[9]);
                                    InsertAsmCode(code, true);
                                }
                                else if (second >= 0)
                                {
                                    LoadToReg(offset1, 8);
                                    sprintf(code, "add %s, %s, %s", RegName[result], RegName[8], RegName[second]);
                                    InsertAsmCode(code, true);
                                }
                            }
                        }
                        else if (first >= 0)
                        {
                            if (p->code.u.TwoOp.op2->kind == CONSTANT)
                            {
                                sprintf(code, "li %s, %d", RegName[10], p->code.u.TwoOp.op2->u.value);
                                InsertAsmCode(code, true);
                                sprintf(code, "add %s, %s, %s", RegName[result], RegName[first], RegName[10]);
                                InsertAsmCode(code, true);
                            }
                            else
                            {
                                int second = FindReg(p->code.u.TwoOp.op2);
                                if (second == -1)
                                {
                                    int offset2 = GetStackOffset(p->code.u.TwoOp.op2);
                                    LoadToReg(offset2, 8);
                                    sprintf(code, "add %s, %s, %s", RegName[result], RegName[first], RegName[8]);
                                    InsertAsmCode(code, true);
                                }
                                else if (second >= 0)
                                {
                                    sprintf(code, "add %s, %s, %s", RegName[result], RegName[first], RegName[second]);
                                    InsertAsmCode(code, true);
                                }
                            }
                        }
                    }
                }
            }
            else if (result >= 0)
            {
                int first = FindReg(p->code.u.TwoOp.op1);
                if (first == -1)
                {
                    int offset1 = GetStackOffset(p->code.u.TwoOp.op1);
                    LoadToReg(offset1, 8);
                    if (p->code.u.TwoOp.op2->kind == CONSTANT)
                    {
                        sprintf(code, "li %s, %d", RegName[9], p->code.u.TwoOp.op2->u.value);
                        InsertAsmCode(code, true);
                        sprintf(code, "add %s, %s, %s", RegName[result], RegName[8], RegName[9]);
                        InsertAsmCode(code, true);
                    }
                    else
                    {
                        int second = FindReg(p->code.u.TwoOp.op2);
                        if (second == -1)
                        {
                            int offset2 = GetStackOffset(p->code.u.TwoOp.op2);
                            LoadToReg(offset2, 9);
                            sprintf(code, "add %s, %s, %s", RegName[result], RegName[8], RegName[9]);
                            InsertAsmCode(code, true);
                        }
                        else if (second >= 0)
                        {
                            sprintf(code, "add %s, %s, %s", RegName[result], RegName[8], RegName[second]);
                            InsertAsmCode(code, true);
                        }
                    }
                }
                else if (first >= 0)
                {
                    if (p->code.u.TwoOp.op2->kind == CONSTANT)
                    {
                        sprintf(code, "li %s, %d", RegName[8], p->code.u.TwoOp.op2->u.value);
                        InsertAsmCode(code, true);
                        sprintf(code, "add %s, %s, %s", RegName[result], RegName[first], RegName[8]);
                        InsertAsmCode(code, true);
                    }
                    else
                    {
                        int second = FindReg(p->code.u.TwoOp.op2);
                        if (second == -1)
                        {
                            int offset2 = GetStackOffset(p->code.u.TwoOp.op2);
                            LoadToReg(offset2, 8);
                            sprintf(code, "add %s, %s, %s", RegName[result], RegName[first], RegName[8]);
                            InsertAsmCode(code, true);
                        }
                        else if (second >= 0)
                        {
                            sprintf(code, "add %s, %s, %s", RegName[result], RegName[first], RegName[second]);
                            InsertAsmCode(code, true);
                        }
                    }
                }
            }
        }
        else if (p->code.kind == SUB)
        {
            int result = FindReg(p->code.u.TwoOp.result);
            if (result == -1)
            {
                int offset = GetStackOffset(p->code.u.TwoOp.result);
                if (offset <= 0)
                {
                    if (p->code.u.TwoOp.op1->kind == CONSTANT && p->code.u.TwoOp.op1->u.value == 0) // MINUS operantion in intercodes
                    {
                        sprintf(code, "li %s, 0", RegName[9]);
                        InsertAsmCode(code, true);
                        int second = FindReg(p->code.u.TwoOp.op2);
                        if (second == -1)
                        {
                            int offset2 = GetStackOffset(p->code.u.TwoOp.op2);
                            LoadToReg(offset2, 10);
                            sprintf(code, "sub %s, %s, %s", RegName[8], RegName[9], RegName[10]);
                            InsertAsmCode(code, true);
                            StoreToStack(offset, 8);
                        }
                        else if (second >= 0)
                        {
                            sprintf(code, "sub %s, %s, %s", RegName[8], RegName[9], RegName[second]);
                            InsertAsmCode(code, true);
                            StoreToStack(offset, 8);
                        }
                    }
                    else if (p->code.u.TwoOp.op1->kind != CONSTANT && p->code.u.TwoOp.op2->kind != CONSTANT) // ensured by intercode
                    {
                        int first = FindReg(p->code.u.TwoOp.op1);
                        if (first == -1)
                        {
                            int offset1 = GetStackOffset(p->code.u.TwoOp.op1);
                            int second = FindReg(p->code.u.TwoOp.op2);
                            if (second == -1)
                            {
                                int offset2 = GetStackOffset(p->code.u.TwoOp.op2);
                                LoadToReg(offset1, 9);
                                LoadToReg(offset2, 10);
                                sprintf(code, "sub %s, %s, %s", RegName[8], RegName[9], RegName[10]);
                                InsertAsmCode(code, true);
                                StoreToStack(offset, 8);
                            }
                            else if (second >= 0)
                            {
                                LoadToReg(offset1, 9);
                                sprintf(code, "sub %s, %s, %s", RegName[8], RegName[9], RegName[second]);
                                InsertAsmCode(code, true);
                                StoreToStack(offset, 8);
                            }
                        }
                        else if (first >= 0)
                        {
                            int second = FindReg(p->code.u.TwoOp.op2);
                            if (second == -1)
                            {
                                int offset2 = GetStackOffset(p->code.u.TwoOp.op2);
                                LoadToReg(offset2, 9);
                                sprintf(code, "sub %s, %s, %s", RegName[8], RegName[first], RegName[9]);
                                InsertAsmCode(code, true);
                                StoreToStack(offset, 8);
                            }
                            else if (second >= 0)
                            {
                                sprintf(code, "sub %s, %s, %s", RegName[8], RegName[first], RegName[second]);
                                InsertAsmCode(code, true);
                                StoreToStack(offset, 8);
                            }
                        }
                    }
                }
                else if (offset > 0)
                {
                    result = FindFreeReg();
                    if (result == -1)
                    {
                        offset = AllocateInStack(p->code.u.TwoOp.result);
                        if (p->code.u.TwoOp.op1->kind == CONSTANT && p->code.u.TwoOp.op1->u.value == 0) // MINUS operantion in intercodes
                        {
                            sprintf(code, "li %s, 0", RegName[9]);
                            InsertAsmCode(code, true);
                            int second = FindReg(p->code.u.TwoOp.op2);
                            if (second == -1)
                            {
                                int offset2 = GetStackOffset(p->code.u.TwoOp.op2);
                                LoadToReg(offset2, 10);
                                sprintf(code, "sub %s, %s, %s", RegName[8], RegName[9], RegName[10]);
                                InsertAsmCode(code, true);
                                StoreToStack(offset, 8);
                            }
                            else if (second >= 0)
                            {
                                sprintf(code, "sub %s, %s, %s", RegName[8], RegName[9], RegName[second]);
                                InsertAsmCode(code, true);
                                StoreToStack(offset, 8);
                            }
                        }
                        else if (p->code.u.TwoOp.op1->kind != CONSTANT && p->code.u.TwoOp.op2->kind != CONSTANT) // ensured by intercode
                        {
                            int first = FindReg(p->code.u.TwoOp.op1);
                            if (first == -1)
                            {
                                int offset1 = GetStackOffset(p->code.u.TwoOp.op1);
                                int second = FindReg(p->code.u.TwoOp.op2);
                                if (second == -1)
                                {
                                    int offset2 = GetStackOffset(p->code.u.TwoOp.op2);
                                    LoadToReg(offset1, 9);
                                    LoadToReg(offset2, 10);
                                    sprintf(code, "sub %s, %s, %s", RegName[8], RegName[9], RegName[10]);
                                    InsertAsmCode(code, true);
                                    StoreToStack(offset, 8);
                                }
                                else if (second >= 0)
                                {
                                    LoadToReg(offset1, 9);
                                    sprintf(code, "sub %s, %s, %s", RegName[8], RegName[9], RegName[second]);
                                    InsertAsmCode(code, true);
                                    StoreToStack(offset, 8);
                                }
                            }
                            else if (first >= 0)
                            {
                                int second = FindReg(p->code.u.TwoOp.op2);
                                if (second == -1)
                                {
                                    int offset2 = GetStackOffset(p->code.u.TwoOp.op2);
                                    LoadToReg(offset2, 9);
                                    sprintf(code, "sub %s, %s, %s", RegName[8], RegName[first], RegName[9]);
                                    InsertAsmCode(code, true);
                                    StoreToStack(offset, 8);
                                }
                                else if (second >= 0)
                                {
                                    sprintf(code, "sub %s, %s, %s", RegName[8], RegName[first], RegName[second]);
                                    InsertAsmCode(code, true);
                                    StoreToStack(offset, 8);
                                }
                            }
                        }
                    }
                    else if (result >= 0)
                    {
                        if (p->code.u.TwoOp.result->kind == VARIABLE)
                        {
                            AllReg[result].kind = 2;
                        }
                        else if (p->code.u.TwoOp.result->kind == TEMP)
                        {
                            AllReg[result].kind = 3;
                        }
                        AllReg[result].num = p->code.u.TwoOp.result->u.var_no;
                        AllReg[result].used = 1;
                        if (p->code.u.TwoOp.op1->kind == CONSTANT && p->code.u.TwoOp.op1->u.value == 0) // MINUS operantion in intercodes
                        {
                            sprintf(code, "li %s, 0", RegName[9]);
                            InsertAsmCode(code, true);
                            int second = FindReg(p->code.u.TwoOp.op2);
                            if (second == -1)
                            {
                                int offset2 = GetStackOffset(p->code.u.TwoOp.op2);
                                LoadToReg(offset2, 10);
                                sprintf(code, "sub %s, %s, %s", RegName[result], RegName[9], RegName[10]);
                                InsertAsmCode(code, true);
                            }
                            else if (second >= 0)
                            {
                                sprintf(code, "sub %s, %s, %s", RegName[result], RegName[9], RegName[second]);
                                InsertAsmCode(code, true);
                            }
                        }
                        else if (p->code.u.TwoOp.op1->kind != CONSTANT && p->code.u.TwoOp.op2->kind != CONSTANT) // ensured by intercode
                        {
                            int first = FindReg(p->code.u.TwoOp.op1);
                            if (first == -1)
                            {
                                int offset1 = GetStackOffset(p->code.u.TwoOp.op1);
                                int second = FindReg(p->code.u.TwoOp.op2);
                                if (second == -1)
                                {
                                    int offset2 = GetStackOffset(p->code.u.TwoOp.op2);
                                    LoadToReg(offset1, 8);
                                    LoadToReg(offset2, 9);
                                    sprintf(code, "sub %s, %s, %s", RegName[result], RegName[8], RegName[9]);
                                    InsertAsmCode(code, true);
                                }
                                else if (second >= 0)
                                {
                                    LoadToReg(offset1, 8);
                                    sprintf(code, "sub %s, %s, %s", RegName[result], RegName[8], RegName[second]);
                                    InsertAsmCode(code, true);
                                }
                            }
                            else if (first >= 0)
                            {
                                int second = FindReg(p->code.u.TwoOp.op2);
                                if (second == -1)
                                {
                                    int offset2 = GetStackOffset(p->code.u.TwoOp.op2);
                                    LoadToReg(offset2, 8);
                                    sprintf(code, "sub %s, %s, %s", RegName[result], RegName[first], RegName[8]);
                                    InsertAsmCode(code, true);
                                }
                                else if (second >= 0)
                                {
                                    sprintf(code, "sub %s, %s, %s", RegName[result], RegName[first], RegName[second]);
                                    InsertAsmCode(code, true);
                                }
                            }
                        }
                    }
                }
            }
            else if (result >= 0)
            {
                if (p->code.u.TwoOp.op1->kind == CONSTANT && p->code.u.TwoOp.op1->u.value == 0) // MINUS operantion in intercodes
                {
                    sprintf(code, "li %s, 0", RegName[8]);
                    InsertAsmCode(code, true);
                    int second = FindReg(p->code.u.TwoOp.op2);
                    if (second == -1)
                    {
                        int offset2 = GetStackOffset(p->code.u.TwoOp.op2);
                        LoadToReg(offset2, 9);
                        sprintf(code, "sub %s, %s, %s", RegName[result], RegName[8], RegName[9]);
                        InsertAsmCode(code, true);
                    }
                    else if (second >= 0)
                    {
                        sprintf(code, "sub %s, %s, %s", RegName[result], RegName[8], RegName[second]);
                        InsertAsmCode(code, true);
                    }
                }
                else if (p->code.u.TwoOp.op1->kind != CONSTANT && p->code.u.TwoOp.op2->kind != CONSTANT) // ensured by intercode
                {
                    int first = FindReg(p->code.u.TwoOp.op1);
                    if (first == -1)
                    {
                        int offset1 = GetStackOffset(p->code.u.TwoOp.op1);
                        LoadToReg(offset1, 8);
                        int second = FindReg(p->code.u.TwoOp.op2);
                        if (second == -1)
                        {
                            int offset2 = GetStackOffset(p->code.u.TwoOp.op2);
                            LoadToReg(offset2, 9);
                            sprintf(code, "sub %s, %s, %s", RegName[result], RegName[8], RegName[9]);
                            InsertAsmCode(code, true);
                        }
                        else if (second >= 0)
                        {
                            sprintf(code, "sub %s, %s, %s", RegName[result], RegName[8], RegName[second]);
                            InsertAsmCode(code, true);
                        }
                    }
                    else if (first >= 0)
                    {
                        int second = FindReg(p->code.u.TwoOp.op2);
                        if (second == -1)
                        {
                            int offset2 = GetStackOffset(p->code.u.TwoOp.op2);
                            LoadToReg(offset2, 8);
                            sprintf(code, "sub %s, %s, %s", RegName[result], RegName[first], RegName[8]);
                            InsertAsmCode(code, true);
                        }
                        else if (second >= 0)
                        {
                            sprintf(code, "sub %s, %s, %s", RegName[result], RegName[first], RegName[second]);
                            InsertAsmCode(code, true);
                        }
                    }
                }
            }
        }
        else if (p->code.kind == MUL)
        {
            int result = FindReg(p->code.u.TwoOp.result);
            if (result == -1)
            {
                int offset = GetStackOffset(p->code.u.TwoOp.result);
                if (offset <= 0)
                {
                    int first = FindReg(p->code.u.TwoOp.op1);
                    if (first == -1)
                    {
                        int offset1 = GetStackOffset(p->code.u.TwoOp.op1);
                        if (p->code.u.TwoOp.op2->kind == CONSTANT)
                        {
                            sprintf(code, "li %s, %d", RegName[10], p->code.u.TwoOp.op2->u.value);
                            InsertAsmCode(code, true);
                            LoadToReg(offset1, 9);
                            sprintf(code, "mul %s, %s, %s", RegName[8], RegName[9], RegName[10]);
                            InsertAsmCode(code, true);
                            StoreToStack(offset, 8);
                        }
                        else
                        {
                            int second = FindReg(p->code.u.TwoOp.op2);
                            if (second == -1)
                            {
                                int offset2 = GetStackOffset(p->code.u.TwoOp.op2);
                                LoadToReg(offset1, 9);
                                LoadToReg(offset2, 10);
                                sprintf(code, "mul %s, %s, %s", RegName[8], RegName[9], RegName[10]);
                                InsertAsmCode(code, true);
                                StoreToStack(offset, 8);
                            }
                            else if (second >= 0)
                            {
                                LoadToReg(offset1, 9);
                                sprintf(code, "mul %s, %s, %s", RegName[8], RegName[9], RegName[second]);
                                InsertAsmCode(code, true);
                                StoreToStack(offset, 8);
                            }
                        }
                    }
                    else if (first >= 0)
                    {
                        if (p->code.u.TwoOp.op2->kind == CONSTANT)
                        {
                            sprintf(code, "li %s, %d", RegName[10], p->code.u.TwoOp.op2->u.value);
                            InsertAsmCode(code, true);
                            sprintf(code, "mul %s, %s, %s", RegName[8], RegName[first], RegName[10]);
                            InsertAsmCode(code, true);
                            StoreToStack(offset, 8);
                        }
                        else
                        {
                            int second = FindReg(p->code.u.TwoOp.op2);
                            if (second == -1)
                            {
                                int offset2 = GetStackOffset(p->code.u.TwoOp.op2);
                                LoadToReg(offset2, 9);
                                sprintf(code, "mul %s, %s, %s", RegName[8], RegName[first], RegName[9]);
                                InsertAsmCode(code, true);
                                StoreToStack(offset, 8);
                            }
                            else if (second >= 0)
                            {
                                sprintf(code, "mul %s, %s, %s", RegName[8], RegName[first], RegName[second]);
                                InsertAsmCode(code, true);
                                StoreToStack(offset, 8);
                            }
                        }
                    }
                }
                else if (offset > 0)
                {
                    result = FindFreeReg();
                    if (result == -1)
                    {
                        offset = AllocateInStack(p->code.u.TwoOp.result);
                        int first = FindReg(p->code.u.TwoOp.op1);
                        if (first == -1)
                        {
                            int offset1 = GetStackOffset(p->code.u.TwoOp.op1);
                            if (p->code.u.TwoOp.op2->kind == CONSTANT)
                            {
                                sprintf(code, "li %s, %d", RegName[10], p->code.u.TwoOp.op2->u.value);
                                InsertAsmCode(code, true);
                                LoadToReg(offset1, 9);
                                sprintf(code, "mul %s, %s, %s", RegName[8], RegName[9], RegName[10]);
                                InsertAsmCode(code, true);
                                StoreToStack(offset, 8);
                            }
                            else
                            {
                                int second = FindReg(p->code.u.TwoOp.op2);
                                if (second == -1)
                                {
                                    int offset2 = GetStackOffset(p->code.u.TwoOp.op2);
                                    LoadToReg(offset1, 9);
                                    LoadToReg(offset2, 10);
                                    sprintf(code, "mul %s, %s, %s", RegName[8], RegName[9], RegName[10]);
                                    InsertAsmCode(code, true);
                                    StoreToStack(offset, 8);
                                }
                                else if (second >= 0)
                                {
                                    LoadToReg(offset1, 9);
                                    sprintf(code, "mul %s, %s, %s", RegName[8], RegName[9], RegName[second]);
                                    InsertAsmCode(code, true);
                                    StoreToStack(offset, 8);
                                }
                            }
                        }
                        else if (first >= 0)
                        {
                            if (p->code.u.TwoOp.op2->kind == CONSTANT)
                            {
                                sprintf(code, "li %s, %d", RegName[10], p->code.u.TwoOp.op2->u.value);
                                InsertAsmCode(code, true);
                                sprintf(code, "mul %s, %s, %s", RegName[8], RegName[first], RegName[10]);
                                InsertAsmCode(code, true);
                                StoreToStack(offset, 8);
                            }
                            else
                            {
                                int second = FindReg(p->code.u.TwoOp.op2);
                                if (second == -1)
                                {
                                    int offset2 = GetStackOffset(p->code.u.TwoOp.op2);
                                    LoadToReg(offset2, 9);
                                    sprintf(code, "mul %s, %s, %s", RegName[8], RegName[first], RegName[9]);
                                    InsertAsmCode(code, true);
                                    StoreToStack(offset, 8);
                                }
                                else if (second >= 0)
                                {
                                    sprintf(code, "mul %s, %s, %s", RegName[8], RegName[first], RegName[second]);
                                    InsertAsmCode(code, true);
                                    StoreToStack(offset, 8);
                                }
                            }
                        }
                    }
                    else if (result >= 0)
                    {
                        if (p->code.u.TwoOp.result->kind == VARIABLE)
                        {
                            AllReg[result].kind = 2;
                        }
                        else if (p->code.u.TwoOp.result->kind == TEMP)
                        {
                            AllReg[result].kind = 3;
                        }
                        AllReg[result].num = p->code.u.TwoOp.result->u.var_no;
                        AllReg[result].used = 1;
                        int first = FindReg(p->code.u.TwoOp.op1);
                        if (first == -1)
                        {
                            int offset1 = GetStackOffset(p->code.u.TwoOp.op1);
                            if (p->code.u.TwoOp.op2->kind == CONSTANT)
                            {
                                sprintf(code, "li %s, %d", RegName[10], p->code.u.TwoOp.op2->u.value);
                                InsertAsmCode(code, true);
                                LoadToReg(offset1, 9);
                                sprintf(code, "mul %s, %s, %s", RegName[result], RegName[9], RegName[10]);
                                InsertAsmCode(code, true);
                            }
                            else
                            {
                                int second = FindReg(p->code.u.TwoOp.op2);
                                if (second == -1)
                                {
                                    int offset2 = GetStackOffset(p->code.u.TwoOp.op2);
                                    LoadToReg(offset1, 8);
                                    LoadToReg(offset2, 9);
                                    sprintf(code, "mul %s, %s, %s", RegName[result], RegName[8], RegName[9]);
                                    InsertAsmCode(code, true);
                                }
                                else if (second >= 0)
                                {
                                    LoadToReg(offset1, 8);
                                    sprintf(code, "mul %s, %s, %s", RegName[result], RegName[8], RegName[second]);
                                    InsertAsmCode(code, true);
                                }
                            }
                        }
                        else if (first >= 0)
                        {
                            if (p->code.u.TwoOp.op2->kind == CONSTANT)
                            {
                                sprintf(code, "li %s, %d", RegName[10], p->code.u.TwoOp.op2->u.value);
                                InsertAsmCode(code, true);
                                sprintf(code, "mul %s, %s, %s", RegName[result], RegName[first], RegName[10]);
                                InsertAsmCode(code, true);
                            }
                            else
                            {
                                int second = FindReg(p->code.u.TwoOp.op2);
                                if (second == -1)
                                {
                                    int offset2 = GetStackOffset(p->code.u.TwoOp.op2);
                                    LoadToReg(offset2, 8);
                                    sprintf(code, "mul %s, %s, %s", RegName[result], RegName[first], RegName[8]);
                                    InsertAsmCode(code, true);
                                }
                                else if (second >= 0)
                                {
                                    sprintf(code, "mul %s, %s, %s", RegName[result], RegName[first], RegName[second]);
                                    InsertAsmCode(code, true);
                                }
                            }
                        }
                    }
                }
            }
            else if (result >= 0)
            {
                int first = FindReg(p->code.u.TwoOp.op1);
                if (first == -1)
                {
                    int offset1 = GetStackOffset(p->code.u.TwoOp.op1);
                    LoadToReg(offset1, 8);
                    if (p->code.u.TwoOp.op2->kind == CONSTANT)
                    {
                        sprintf(code, "li %s, %d", RegName[9], p->code.u.TwoOp.op2->u.value);
                        InsertAsmCode(code, true);
                        sprintf(code, "mul %s, %s, %s", RegName[result], RegName[8], RegName[9]);
                        InsertAsmCode(code, true);
                    }
                    else
                    {
                        int second = FindReg(p->code.u.TwoOp.op2);
                        if (second == -1)
                        {
                            int offset2 = GetStackOffset(p->code.u.TwoOp.op2);
                            LoadToReg(offset2, 9);
                            sprintf(code, "mul %s, %s, %s", RegName[result], RegName[8], RegName[9]);
                            InsertAsmCode(code, true);
                        }
                        else if (second >= 0)
                        {
                            sprintf(code, "mul %s, %s, %s", RegName[result], RegName[8], RegName[second]);
                            InsertAsmCode(code, true);
                        }
                    }
                }
                else if (first >= 0)
                {
                    if (p->code.u.TwoOp.op2->kind == CONSTANT)
                    {
                        sprintf(code, "li %s, %d", RegName[8], p->code.u.TwoOp.op2->u.value);
                        InsertAsmCode(code, true);
                        sprintf(code, "mul %s, %s, %s", RegName[result], RegName[first], RegName[8]);
                        InsertAsmCode(code, true);
                    }
                    else
                    {
                        int second = FindReg(p->code.u.TwoOp.op2);
                        if (second == -1)
                        {
                            int offset2 = GetStackOffset(p->code.u.TwoOp.op2);
                            LoadToReg(offset2, 8);
                            sprintf(code, "mul %s, %s, %s", RegName[result], RegName[first], RegName[8]);
                            InsertAsmCode(code, true);
                        }
                        else if (second >= 0)
                        {
                            sprintf(code, "mul %s, %s, %s", RegName[result], RegName[first], RegName[second]);
                            InsertAsmCode(code, true);
                        }
                    }
                }
            }
        }
        else if (p->code.kind == ICDIV)
        {
            int result = FindReg(p->code.u.TwoOp.result);
            if (result == -1)
            {
                int offset = GetStackOffset(p->code.u.TwoOp.result);
                if (offset <= 0)
                {
                    if (p->code.u.TwoOp.op1->kind != CONSTANT && p->code.u.TwoOp.op2->kind != CONSTANT) // ensured by intercode
                    {
                        int first = FindReg(p->code.u.TwoOp.op1);
                        if (first == -1)
                        {
                            int offset1 = GetStackOffset(p->code.u.TwoOp.op1);
                            int second = FindReg(p->code.u.TwoOp.op2);
                            if (second == -1)
                            {
                                LoadToReg(offset1, 9);
                                int offset2 = GetStackOffset(p->code.u.TwoOp.op2);
                                LoadToReg(offset2, 10);
                                sprintf(code, "div %s, %s)", RegName[9], RegName[10]);
                                InsertAsmCode(code, true);
                                sprintf(code, "mflo %s", RegName[8]);
                                InsertAsmCode(code, true);
                                StoreToStack(offset, 8);
                            }
                            else if (second >= 0)
                            {
                                LoadToReg(offset1, 9);
                                sprintf(code, "div %s, %s", RegName[9], RegName[second]);
                                InsertAsmCode(code, true);
                                sprintf(code, "mflo %s", RegName[8]);
                                InsertAsmCode(code, true);
                                StoreToStack(offset, 8);
                            }
                        }
                        else if (first >= 0)
                        {
                            int second = FindReg(p->code.u.TwoOp.op2);
                            if (second == -1)
                            {
                                int offset2 = GetStackOffset(p->code.u.TwoOp.op2);
                                LoadToReg(offset2, 9);
                                sprintf(code, "div %s, %s", RegName[first], RegName[9]);
                                InsertAsmCode(code, true);
                                sprintf(code, "mflo %s", RegName[8]);
                                InsertAsmCode(code, true);
                                StoreToStack(offset, 8);
                            }
                            else if (second >= 0)
                            {
                                sprintf(code, "div %s, %s", RegName[first], RegName[second]);
                                InsertAsmCode(code, true);
                                sprintf(code, "mflo %s", RegName[8]);
                                InsertAsmCode(code, true);
                                StoreToStack(offset, 8);
                            }
                        }
                    }
                }
                else if (offset > 0)
                {
                    result = FindFreeReg();
                    if (result == -1)
                    {
                        offset = AllocateInStack(p->code.u.TwoOp.result);
                        if (p->code.u.TwoOp.op1->kind != CONSTANT && p->code.u.TwoOp.op2->kind != CONSTANT) // ensured by intercode
                        {
                            int first = FindReg(p->code.u.TwoOp.op1);
                            if (first == -1)
                            {
                                int offset1 = GetStackOffset(p->code.u.TwoOp.op1);
                                int second = FindReg(p->code.u.TwoOp.op2);
                                if (second == -1)
                                {
                                    int offset2 = GetStackOffset(p->code.u.TwoOp.op2);
                                    LoadToReg(offset1, 9);
                                    LoadToReg(offset2, 10);
                                    sprintf(code, "div %s, %s", RegName[9], RegName[10]);
                                    InsertAsmCode(code, true);
                                    sprintf(code, "mflo %s", RegName[8]);
                                    InsertAsmCode(code, true);
                                    StoreToStack(offset, 8);
                                }
                                else if (second >= 0)
                                {
                                    LoadToReg(offset1, 9);
                                    sprintf(code, "div %s, %s", RegName[9], RegName[second]);
                                    InsertAsmCode(code, true);
                                    sprintf(code, "mflo %s", RegName[8]);
                                    InsertAsmCode(code, true);
                                    StoreToStack(offset, 8);
                                }
                            }
                            else if (first >= 0)
                            {
                                int second = FindReg(p->code.u.TwoOp.op2);
                                if (second == -1)
                                {
                                    int offset2 = GetStackOffset(p->code.u.TwoOp.op2);
                                    LoadToReg(offset2, 9);
                                    sprintf(code, "div %s, %s", RegName[first], RegName[9]);
                                    InsertAsmCode(code, true);
                                    sprintf(code, "mflo %s", RegName[8]);
                                    InsertAsmCode(code, true);
                                    StoreToStack(offset, 8);
                                }
                                else if (second >= 0)
                                {
                                    sprintf(code, "div %s, %s", RegName[first], RegName[second]);
                                    InsertAsmCode(code, true);
                                    sprintf(code, "mflo %s", RegName[8]);
                                    InsertAsmCode(code, true);
                                    StoreToStack(offset, 8);
                                }
                            }
                        }
                    }
                    else if (result >= 0)
                    {
                        if (p->code.u.TwoOp.result->kind == VARIABLE)
                        {
                            AllReg[result].kind = 2;
                        }
                        else if (p->code.u.TwoOp.result->kind == TEMP)
                        {
                            AllReg[result].kind = 3;
                        }
                        AllReg[result].num = p->code.u.TwoOp.result->u.var_no;
                        AllReg[result].used = 1;
                        if (p->code.u.TwoOp.op1->kind != CONSTANT && p->code.u.TwoOp.op2->kind != CONSTANT) // ensured by intercode
                        {
                            int first = FindReg(p->code.u.TwoOp.op1);
                            if (first == -1)
                            {
                                int offset1 = GetStackOffset(p->code.u.TwoOp.op1);
                                LoadToReg(offset1, 8);
                                int second = FindReg(p->code.u.TwoOp.op2);
                                if (second == -1)
                                {
                                    int offset2 = GetStackOffset(p->code.u.TwoOp.op2);
                                    LoadToReg(offset2, 9);
                                    sprintf(code, "div %s, %s", RegName[8], RegName[9]);
                                    InsertAsmCode(code, true);
                                    sprintf(code, "mflo %s", RegName[result]);
                                    InsertAsmCode(code, true);
                                }
                                else if (second >= 0)
                                {
                                    sprintf(code, "div %s, %s", RegName[8], RegName[second]);
                                    InsertAsmCode(code, true);
                                    sprintf(code, "mflo %s", RegName[result]);
                                    InsertAsmCode(code, true);
                                }
                            }
                            else if (first >= 0)
                            {
                                int second = FindReg(p->code.u.TwoOp.op2);
                                if (second == -1)
                                {
                                    int offset2 = GetStackOffset(p->code.u.TwoOp.op2);
                                    LoadToReg(offset2, 8);
                                    sprintf(code, "div %s, %s", RegName[first], RegName[8]);
                                    InsertAsmCode(code, true);
                                    sprintf(code, "mflo %s", RegName[result]);
                                    InsertAsmCode(code, true);
                                }
                                else if (second >= 0)
                                {
                                    sprintf(code, "div %s, %s", RegName[first], RegName[second]);
                                    InsertAsmCode(code, true);
                                    sprintf(code, "mflo %s", RegName[result]);
                                    InsertAsmCode(code, true);
                                }
                            }
                        }
                    }
                }
            }
            else if (result >= 0)
            {
                if (p->code.u.TwoOp.op1->kind != CONSTANT && p->code.u.TwoOp.op2->kind != CONSTANT) // ensured by intercode
                {
                    int first = FindReg(p->code.u.TwoOp.op1);
                    if (first == -1)
                    {
                        int offset1 = GetStackOffset(p->code.u.TwoOp.op1);
                        LoadToReg(offset1, 8);
                        int second = FindReg(p->code.u.TwoOp.op2);
                        if (second == -1)
                        {
                            int offset2 = GetStackOffset(p->code.u.TwoOp.op2);
                            LoadToReg(offset2, 9);
                            sprintf(code, "div %s, %s", RegName[8], RegName[9]);
                            InsertAsmCode(code, true);
                            sprintf(code, "mflo %s", RegName[result]);
                            InsertAsmCode(code, true);
                        }
                        else if (second >= 0)
                        {
                            sprintf(code, "div %s, %s", RegName[8], RegName[second]);
                            InsertAsmCode(code, true);
                            sprintf(code, "mflo %s", RegName[result]);
                            InsertAsmCode(code, true);
                        }
                    }
                    else if (first >= 0)
                    {
                        int second = FindReg(p->code.u.TwoOp.op2);
                        if (second == -1)
                        {
                            int offset2 = GetStackOffset(p->code.u.TwoOp.op2);
                            LoadToReg(offset2, 8);
                            sprintf(code, "div %s, %s", RegName[first], RegName[8]);
                            InsertAsmCode(code, true);
                            sprintf(code, "mflo %s", RegName[result]);
                            InsertAsmCode(code, true);
                        }
                        else if (second >= 0)
                        {
                            sprintf(code, "div %s, %s", RegName[first], RegName[second]);
                            InsertAsmCode(code, true);
                            sprintf(code, "mflo %s", RegName[result]);
                            InsertAsmCode(code, true);
                        }
                    }
                }
            }
        }
        else if (p->code.kind == LABEL)
        {
            sprintf(code, "label%d:", p->code.u.single.op->u.var_no);
            InsertAsmCode(code, false);
        }
        else if (p->code.kind == ICFUNC)
        {
            sprintf(code, "%s:", p->code.u.single.op->u.name);
            InsertAsmCode(code, false);
            if (strcmp(p->code.u.single.op->u.name, "main") == 0)
            {
                LocalOffset = 0;
                sprintf(code, "move $fp, $sp");
                InsertAsmCode(code, true);
            }
            else
            {
                LocalOffset = -8;
            }
        }
        else if (p->code.kind == IFGOTO)
        {
            int targetx = FindReg(p->code.u.ThreeOp.op1);
            if (targetx == -1)
            {
                int offset1 = GetStackOffset(p->code.u.ThreeOp.op1);
                if (offset1 <= 0)
                {
                    LoadToReg(offset1, 8);
                    int targety = FindReg(p->code.u.ThreeOp.op2);
                    if (targety == -1)
                    {
                        int offset2 = GetStackOffset(p->code.u.ThreeOp.op2);
                        if (offset2 <= 0)
                        {
                            LoadToReg(offset2, 9);
                            if (strcmp(p->code.u.ThreeOp.relop, "==") == 0)
                            {
                                sprintf(code, "beq %s, %s, label%d", RegName[8], RegName[9], p->code.u.ThreeOp.label->u.var_no);
                                InsertAsmCode(code, true);
                            }
                            else if (strcmp(p->code.u.ThreeOp.relop, "!=") == 0)
                            {
                                sprintf(code, "bne %s, %s, label%d", RegName[8], RegName[9], p->code.u.ThreeOp.label->u.var_no);
                                InsertAsmCode(code, true);
                            }
                            else if (strcmp(p->code.u.ThreeOp.relop, ">") == 0)
                            {
                                sprintf(code, "bgt %s, %s, label%d", RegName[8], RegName[9], p->code.u.ThreeOp.label->u.var_no);
                                InsertAsmCode(code, true);
                            }
                            else if (strcmp(p->code.u.ThreeOp.relop, "<") == 0)
                            {
                                sprintf(code, "blt %s, %s, label%d", RegName[8], RegName[9], p->code.u.ThreeOp.label->u.var_no);
                                InsertAsmCode(code, true);
                            }
                            else if (strcmp(p->code.u.ThreeOp.relop, ">=") == 0)
                            {
                                sprintf(code, "bge %s, %s, label%d", RegName[8], RegName[9], p->code.u.ThreeOp.label->u.var_no);
                                InsertAsmCode(code, true);
                            }
                            else if (strcmp(p->code.u.ThreeOp.relop, "<=") == 0)
                            {
                                sprintf(code, "ble %s, %s, label%d", RegName[8], RegName[9], p->code.u.ThreeOp.label->u.var_no);
                                InsertAsmCode(code, true);
                            }
                        }
                        else if (p->code.u.ThreeOp.op2->kind == CONSTANT)
                        {
                            sprintf(code, "li %s, %d", RegName[9], p->code.u.ThreeOp.op2->u.value);
                            InsertAsmCode(code, true);
                            if (strcmp(p->code.u.ThreeOp.relop, "==") == 0)
                            {
                                sprintf(code, "beq %s, %s, label%d", RegName[8], RegName[9], p->code.u.ThreeOp.label->u.var_no);
                                InsertAsmCode(code, true);
                            }
                            else if (strcmp(p->code.u.ThreeOp.relop, "!=") == 0)
                            {
                                sprintf(code, "bne %s, %s, label%d", RegName[8], RegName[9], p->code.u.ThreeOp.label->u.var_no);
                                InsertAsmCode(code, true);
                            }
                            else if (strcmp(p->code.u.ThreeOp.relop, ">") == 0)
                            {
                                sprintf(code, "bgt %s, %s, label%d", RegName[8], RegName[9], p->code.u.ThreeOp.label->u.var_no);
                                InsertAsmCode(code, true);
                            }
                            else if (strcmp(p->code.u.ThreeOp.relop, "<") == 0)
                            {
                                sprintf(code, "blt %s, %s, label%d", RegName[8], RegName[9], p->code.u.ThreeOp.label->u.var_no);
                                InsertAsmCode(code, true);
                            }
                            else if (strcmp(p->code.u.ThreeOp.relop, ">=") == 0)
                            {
                                sprintf(code, "bge %s, %s, label%d", RegName[8], RegName[9], p->code.u.ThreeOp.label->u.var_no);
                                InsertAsmCode(code, true);
                            }
                            else if (strcmp(p->code.u.ThreeOp.relop, "<=") == 0)
                            {
                                sprintf(code, "ble %s, %s, label%d", RegName[8], RegName[9], p->code.u.ThreeOp.label->u.var_no);
                                InsertAsmCode(code, true);
                            }
                        }
                    }
                    else if (targety >= 0)
                    {
                        if (strcmp(p->code.u.ThreeOp.relop, "==") == 0)
                        {
                            sprintf(code, "beq %s, %s, label%d", RegName[8], RegName[targety], p->code.u.ThreeOp.label->u.var_no);
                            InsertAsmCode(code, true);
                        }
                        else if (strcmp(p->code.u.ThreeOp.relop, "!=") == 0)
                        {
                            sprintf(code, "bne %s, %s, label%d", RegName[8], RegName[targety], p->code.u.ThreeOp.label->u.var_no);
                            InsertAsmCode(code, true);
                        }
                        else if (strcmp(p->code.u.ThreeOp.relop, ">") == 0)
                        {
                            sprintf(code, "bgt %s, %s, label%d", RegName[8], RegName[targety], p->code.u.ThreeOp.label->u.var_no);
                            InsertAsmCode(code, true);
                        }
                        else if (strcmp(p->code.u.ThreeOp.relop, "<") == 0)
                        {
                            sprintf(code, "blt %s, %s, label%d", RegName[8], RegName[targety], p->code.u.ThreeOp.label->u.var_no);
                            InsertAsmCode(code, true);
                        }
                        else if (strcmp(p->code.u.ThreeOp.relop, ">=") == 0)
                        {
                            sprintf(code, "bge %s, %s, label%d", RegName[8], RegName[targety], p->code.u.ThreeOp.label->u.var_no);
                            InsertAsmCode(code, true);
                        }
                        else if (strcmp(p->code.u.ThreeOp.relop, "<=") == 0)
                        {
                            sprintf(code, "ble %s, %s, label%d", RegName[8], RegName[targety], p->code.u.ThreeOp.label->u.var_no);
                            InsertAsmCode(code, true);
                        }
                    }
                }
                else if (p->code.u.ThreeOp.op1->kind == CONSTANT)
                {
                    sprintf(code, "li %s, %d", RegName[8], p->code.u.ThreeOp.op1->u.value);
                    InsertAsmCode(code, true);
                    int targety = FindReg(p->code.u.ThreeOp.op2);
                    if (targety == -1)
                    {
                        int offset2 = GetStackOffset(p->code.u.ThreeOp.op2);
                        if (offset2 <= 0)
                        {
                            LoadToReg(offset2, 9);
                            if (strcmp(p->code.u.ThreeOp.relop, "==") == 0)
                            {
                                sprintf(code, "beq %s, %s, label%d", RegName[8], RegName[9], p->code.u.ThreeOp.label->u.var_no);
                                InsertAsmCode(code, true);
                            }
                            else if (strcmp(p->code.u.ThreeOp.relop, "!=") == 0)
                            {
                                sprintf(code, "bne %s, %s, label%d", RegName[8], RegName[9], p->code.u.ThreeOp.label->u.var_no);
                                InsertAsmCode(code, true);
                            }
                            else if (strcmp(p->code.u.ThreeOp.relop, ">") == 0)
                            {
                                sprintf(code, "bgt %s, %s, label%d", RegName[8], RegName[9], p->code.u.ThreeOp.label->u.var_no);
                                InsertAsmCode(code, true);
                            }
                            else if (strcmp(p->code.u.ThreeOp.relop, "<") == 0)
                            {
                                sprintf(code, "blt %s, %s, label%d", RegName[8], RegName[9], p->code.u.ThreeOp.label->u.var_no);
                                InsertAsmCode(code, true);
                            }
                            else if (strcmp(p->code.u.ThreeOp.relop, ">=") == 0)
                            {
                                sprintf(code, "bge %s, %s, label%d", RegName[8], RegName[9], p->code.u.ThreeOp.label->u.var_no);
                                InsertAsmCode(code, true);
                            }
                            else if (strcmp(p->code.u.ThreeOp.relop, "<=") == 0)
                            {
                                sprintf(code, "ble %s, %s, label%d", RegName[8], RegName[9], p->code.u.ThreeOp.label->u.var_no);
                                InsertAsmCode(code, true);
                            }
                        }
                        else if (p->code.u.ThreeOp.op2->kind == CONSTANT)
                        {
                            sprintf(code, "li %s, %d", RegName[9], p->code.u.ThreeOp.op2->u.value);
                            InsertAsmCode(code, true);
                            if (strcmp(p->code.u.ThreeOp.relop, "==") == 0)
                            {
                                sprintf(code, "beq %s, %s, label%d", RegName[8], RegName[9], p->code.u.ThreeOp.label->u.var_no);
                                InsertAsmCode(code, true);
                            }
                            else if (strcmp(p->code.u.ThreeOp.relop, "!=") == 0)
                            {
                                sprintf(code, "bne %s, %s, label%d", RegName[8], RegName[9], p->code.u.ThreeOp.label->u.var_no);
                                InsertAsmCode(code, true);
                            }
                            else if (strcmp(p->code.u.ThreeOp.relop, ">") == 0)
                            {
                                sprintf(code, "bgt %s, %s, label%d", RegName[8], RegName[9], p->code.u.ThreeOp.label->u.var_no);
                                InsertAsmCode(code, true);
                            }
                            else if (strcmp(p->code.u.ThreeOp.relop, "<") == 0)
                            {
                                sprintf(code, "blt %s, %s, label%d", RegName[8], RegName[9], p->code.u.ThreeOp.label->u.var_no);
                                InsertAsmCode(code, true);
                            }
                            else if (strcmp(p->code.u.ThreeOp.relop, ">=") == 0)
                            {
                                sprintf(code, "bge %s, %s, label%d", RegName[8], RegName[9], p->code.u.ThreeOp.label->u.var_no);
                                InsertAsmCode(code, true);
                            }
                            else if (strcmp(p->code.u.ThreeOp.relop, "<=") == 0)
                            {
                                sprintf(code, "ble %s, %s, label%d", RegName[8], RegName[9], p->code.u.ThreeOp.label->u.var_no);
                                InsertAsmCode(code, true);
                            }
                        }
                    }
                    else if (targety >= 0)
                    {
                        if (strcmp(p->code.u.ThreeOp.relop, "==") == 0)
                        {
                            sprintf(code, "beq %s, %s, label%d", RegName[8], RegName[targety], p->code.u.ThreeOp.label->u.var_no);
                            InsertAsmCode(code, true);
                        }
                        else if (strcmp(p->code.u.ThreeOp.relop, "!=") == 0)
                        {
                            sprintf(code, "bne %s, %s, label%d", RegName[8], RegName[targety], p->code.u.ThreeOp.label->u.var_no);
                            InsertAsmCode(code, true);
                        }
                        else if (strcmp(p->code.u.ThreeOp.relop, ">") == 0)
                        {
                            sprintf(code, "bgt %s, %s, label%d", RegName[8], RegName[targety], p->code.u.ThreeOp.label->u.var_no);
                            InsertAsmCode(code, true);
                        }
                        else if (strcmp(p->code.u.ThreeOp.relop, "<") == 0)
                        {
                            sprintf(code, "blt %s, %s, label%d", RegName[8], RegName[targety], p->code.u.ThreeOp.label->u.var_no);
                            InsertAsmCode(code, true);
                        }
                        else if (strcmp(p->code.u.ThreeOp.relop, ">=") == 0)
                        {
                            sprintf(code, "bge %s, %s, label%d", RegName[8], RegName[targety], p->code.u.ThreeOp.label->u.var_no);
                            InsertAsmCode(code, true);
                        }
                        else if (strcmp(p->code.u.ThreeOp.relop, "<=") == 0)
                        {
                            sprintf(code, "ble %s, %s, label%d", RegName[8], RegName[targety], p->code.u.ThreeOp.label->u.var_no);
                            InsertAsmCode(code, true);
                        }
                    }
                }
            }
            else if (targetx >= 0)
            {
                int targety = FindReg(p->code.u.ThreeOp.op2);
                if (targety == -1)
                {
                    int offset2 = GetStackOffset(p->code.u.ThreeOp.op2);
                    if (offset2 <= 0)
                    {
                        LoadToReg(offset2, 9);
                        if (strcmp(p->code.u.ThreeOp.relop, "==") == 0)
                        {
                            sprintf(code, "beq %s, %s, label%d", RegName[targetx], RegName[9], p->code.u.ThreeOp.label->u.var_no);
                            InsertAsmCode(code, true);
                        }
                        else if (strcmp(p->code.u.ThreeOp.relop, "!=") == 0)
                        {
                            sprintf(code, "bne %s, %s, label%d", RegName[targetx], RegName[9], p->code.u.ThreeOp.label->u.var_no);
                            InsertAsmCode(code, true);
                        }
                        else if (strcmp(p->code.u.ThreeOp.relop, ">") == 0)
                        {
                            sprintf(code, "bgt %s, %s, label%d", RegName[targetx], RegName[9], p->code.u.ThreeOp.label->u.var_no);
                            InsertAsmCode(code, true);
                        }
                        else if (strcmp(p->code.u.ThreeOp.relop, "<") == 0)
                        {
                            sprintf(code, "blt %s, %s, label%d", RegName[targetx], RegName[9], p->code.u.ThreeOp.label->u.var_no);
                            InsertAsmCode(code, true);
                        }
                        else if (strcmp(p->code.u.ThreeOp.relop, ">=") == 0)
                        {
                            sprintf(code, "bge %s, %s, label%d", RegName[targetx], RegName[9], p->code.u.ThreeOp.label->u.var_no);
                            InsertAsmCode(code, true);
                        }
                        else if (strcmp(p->code.u.ThreeOp.relop, "<=") == 0)
                        {
                            sprintf(code, "ble %s, %s, label%d", RegName[targetx], RegName[9], p->code.u.ThreeOp.label->u.var_no);
                            InsertAsmCode(code, true);
                        }
                    }
                    else if (p->code.u.ThreeOp.op2->kind == CONSTANT)
                    {
                        sprintf(code, "li %s, %d", RegName[9], p->code.u.ThreeOp.op2->u.value);
                        InsertAsmCode(code, true);
                        if (strcmp(p->code.u.ThreeOp.relop, "==") == 0)
                        {
                            sprintf(code, "beq %s, %s, label%d", RegName[targetx], RegName[9], p->code.u.ThreeOp.label->u.var_no);
                            InsertAsmCode(code, true);
                        }
                        else if (strcmp(p->code.u.ThreeOp.relop, "!=") == 0)
                        {
                            sprintf(code, "bne %s, %s, label%d", RegName[targetx], RegName[9], p->code.u.ThreeOp.label->u.var_no);
                            InsertAsmCode(code, true);
                        }
                        else if (strcmp(p->code.u.ThreeOp.relop, ">") == 0)
                        {
                            sprintf(code, "bgt %s, %s, label%d", RegName[targetx], RegName[9], p->code.u.ThreeOp.label->u.var_no);
                            InsertAsmCode(code, true);
                        }
                        else if (strcmp(p->code.u.ThreeOp.relop, "<") == 0)
                        {
                            sprintf(code, "blt %s, %s, label%d", RegName[targetx], RegName[9], p->code.u.ThreeOp.label->u.var_no);
                            InsertAsmCode(code, true);
                        }
                        else if (strcmp(p->code.u.ThreeOp.relop, ">=") == 0)
                        {
                            sprintf(code, "bge %s, %s, label%d", RegName[targetx], RegName[9], p->code.u.ThreeOp.label->u.var_no);
                            InsertAsmCode(code, true);
                        }
                        else if (strcmp(p->code.u.ThreeOp.relop, "<=") == 0)
                        {
                            sprintf(code, "ble %s, %s, label%d", RegName[targetx], RegName[9], p->code.u.ThreeOp.label->u.var_no);
                            InsertAsmCode(code, true);
                        }
                    }
                }
                else if (targety >= 0)
                {
                    if (strcmp(p->code.u.ThreeOp.relop, "==") == 0)
                    {
                        sprintf(code, "beq %s, %s, label%d", RegName[targetx], RegName[targety], p->code.u.ThreeOp.label->u.var_no);
                        InsertAsmCode(code, true);
                    }
                    else if (strcmp(p->code.u.ThreeOp.relop, "!=") == 0)
                    {
                        sprintf(code, "bne %s, %s, label%d", RegName[targetx], RegName[targety], p->code.u.ThreeOp.label->u.var_no);
                        InsertAsmCode(code, true);
                    }
                    else if (strcmp(p->code.u.ThreeOp.relop, ">") == 0)
                    {
                        sprintf(code, "bgt %s, %s, label%d", RegName[targetx], RegName[targety], p->code.u.ThreeOp.label->u.var_no);
                        InsertAsmCode(code, true);
                    }
                    else if (strcmp(p->code.u.ThreeOp.relop, "<") == 0)
                    {
                        sprintf(code, "blt %s, %s, label%d", RegName[targetx], RegName[targety], p->code.u.ThreeOp.label->u.var_no);
                        InsertAsmCode(code, true);
                    }
                    else if (strcmp(p->code.u.ThreeOp.relop, ">=") == 0)
                    {
                        sprintf(code, "bge %s, %s, label%d", RegName[targetx], RegName[targety], p->code.u.ThreeOp.label->u.var_no);
                        InsertAsmCode(code, true);
                    }
                    else if (strcmp(p->code.u.ThreeOp.relop, "<=") == 0)
                    {
                        sprintf(code, "ble %s, %s, label%d", RegName[targetx], RegName[targety], p->code.u.ThreeOp.label->u.var_no);
                        InsertAsmCode(code, true);
                    }
                }
            }
        }
        else if (p->code.kind == GOTO)
        {
            sprintf(code, "j label%d", p->code.u.single.op->u.var_no);
            InsertAsmCode(code, true);
        }
        else if (p->code.kind == ICRETURN)
        {
            CountParam = 0;
            int target = FindReg(p->code.u.single.op);
            if (target == -1)
            {
                int offset = GetStackOffset(p->code.u.single.op);
                if (offset <= 0)
                {
                    LoadToReg(offset, 8);
                    sprintf(code, "move $v0, %s", RegName[8]);
                    InsertAsmCode(code, true);
                }
            }
            else if (target >= 0)
            {
                sprintf(code, "move $v0, %s", RegName[target]);
                InsertAsmCode(code, true);
            }
            sprintf(code, "jr $ra");
            InsertAsmCode(code, true);
        }
        else if (p->code.kind == DEC)
        {
            p = p->next;
            continue;
        }
        else if (p->code.kind == ARG)
        {
            int target = FindReg(p->code.u.single.op);
            if (target == -1)
            {
                int offset = GetStackOffset(p->code.u.single.op);
                if (offset <= 0)
                {
                    if (CountArg >= 4)
                    {
                        sprintf(code, "addi $sp, $sp, -4");
                        InsertAsmCode(code, true);
                        LoadToReg(offset, 8);
                        sprintf(code, "sw %s, 0($sp)", RegName[8]);
                        InsertAsmCode(code, true);
                        ++CountArg;
                    }
                    else if (CountArg < 4)
                    {
                        LoadToReg(offset, 8);
                        sprintf(code, "move $a%d, %s", CountArg, RegName[8]);
                        InsertAsmCode(code, true);
                        AllReg[CountArg + 4].used = true;
                        ++CountArg;
                    }
                }
            }
            else if (target >= 0)
            {
                if (CountArg >= 4)
                {
                    sprintf(code, "addi $sp, $sp, -4");
                    InsertAsmCode(code, true);
                    sprintf(code, "sw %s, 0($sp)", RegName[target]);
                    InsertAsmCode(code, true);
                    ++CountArg;
                }
                else if (CountArg < 4)
                {
                    sprintf(code, "move $a%d, %s", CountArg, RegName[target]);
                    InsertAsmCode(code, true);
                    AllReg[CountArg + 4].used = true;
                    ++CountArg;
                }
            }
        }
        else if (p->code.kind == CALL)
        {
            sprintf(code, "addi $sp, $sp, -4");
            InsertAsmCode(code, true);
            sprintf(code, "sw $ra, 0($sp)");
            InsertAsmCode(code, true);
            sprintf(code, "addi $sp, $sp, -4");
            InsertAsmCode(code, true);
            sprintf(code, "sw $fp, 0($sp)");
            InsertAsmCode(code, true);
            sprintf(code, "addi $fp, $sp, 8");
            InsertAsmCode(code, true);
            SaveReg();
            sprintf(code, "jal %s", p->code.u.assign.right->u.name);
            InsertAsmCode(code, true);
            RecoverReg();
            sprintf(code, "lw $ra, -4($fp)");
            InsertAsmCode(code, true);
            sprintf(code, "move $sp, $fp");
            InsertAsmCode(code, true);
            sprintf(code, "lw $fp, -8($fp)");
            InsertAsmCode(code, true);
            CountArg = 0;
            int left = FindReg(p->code.u.assign.left);
            if (left == -1)
            {
                int offset = GetStackOffset(p->code.u.assign.left);
                if (offset > 0)
                {
                    left = FindFreeReg();
                    if (left == -1)
                    {
                        offset = AllocateInStack(p->code.u.assign.left);
                        sprintf(code, "move %s, $v0", RegName[8]);
                        InsertAsmCode(code, true);
                        StoreToStack(offset, 8);
                    }
                    if (left >= 0)
                    {
                        if (p->code.u.assign.left->kind == VARIABLE)
                        {
                            AllReg[left].kind = 2;
                        }
                        else if (p->code.u.assign.left->kind == TEMP)
                        {
                            AllReg[left].kind = 3;
                        }
                        AllReg[left].num = p->code.u.assign.left->u.var_no;
                        AllReg[left].used = 1;
                        sprintf(code, "move %s, $v0", RegName[left]);
                        InsertAsmCode(code, true);
                    }
                }
                else if (offset <= 0)
                {
                    StoreToStack(offset, 2);
                }
            }
            else if (left >= 0)
            {
                sprintf(code, "move %s, $v0", RegName[left]);
                InsertAsmCode(code, true);
            }
            for (int i = 4; i <= 7; ++i)
            {
                AllReg[i].used = false;
            }
        }
        else if (p->code.kind == PARAM)
        {
            ++CountParam;
            p = p->next;
            while (p->code.kind == PARAM)
            {
                ++CountParam;
                p = p->next;
            }
            p = p->prev;
            InterCodes moveback = p;
            int count = 0;
            while (count < CountParam)
            {
                if (count < 4)
                {
                    int target = FindReg(moveback->code.u.single.op);
                    if (target == -1)
                    {
                        int offset = GetStackOffset(moveback->code.u.single.op);
                        if (offset > 0)
                        {
                            offset = AllocateInStack(moveback->code.u.single.op);
                            StoreToStack(offset, count + 4);
                        }
                        else if (offset <= 0)
                        {
                            StoreToStack(offset, count + 4);
                        }
                    }
                    else if (target >= 0)
                    {
                        sprintf(code, "move %s, $a%d", RegName[target], count);
                        InsertAsmCode(code, true);
                    }
                }
                else if (count >= 4)
                {
                    int target = FindReg(moveback->code.u.single.op);
                    if (target == -1)
                    {
                        int offset = GetStackOffset(moveback->code.u.single.op);
                        if (offset > 0)
                        {
                            offset = AllocateInStack(moveback->code.u.single.op);
                            LoadToReg((CountParam - 1 - count) * 4 + 8, 8);
                            StoreToStack(offset, 8);
                        }
                        else if (offset <= 0)
                        {
                            LoadToReg((CountParam - 1 - count) * 4 + 8, 8);
                            StoreToStack(offset, 8);
                        }
                    }
                    else if (target >= 0)
                    {
                        LoadToReg((CountParam - 1 - count) * 4 + 8, target);
                    }
                }
                ++count;
                moveback = moveback->prev;
            }
        }
        else if (p->code.kind == READ)
        {
            sprintf(code, "addi $sp, $sp, -4");
            InsertAsmCode(code, true);
            sprintf(code, "sw $ra, 0($sp)");
            InsertAsmCode(code, true);
            sprintf(code, "jal read");
            InsertAsmCode(code, true);
            sprintf(code, "lw $ra, 0($sp)");
            InsertAsmCode(code, true);
            sprintf(code, "addi $sp, $sp, 4");
            InsertAsmCode(code, true);
            int target = FindReg(p->code.u.single.op);
            if (target == -1)
            {
                int offset = GetStackOffset(p->code.u.single.op);
                if (offset > 0)
                {
                    target = FindFreeReg();
                    if (target == -1)
                    {
                        offset = AllocateInStack(p->code.u.single.op);
                        StoreToStack(offset, 2);
                    }
                    else if (target >= 0)
                    {
                        if (p->code.u.single.op->kind == VARIABLE)
                        {
                            AllReg[target].kind = 2;
                        }
                        else if (p->code.u.single.op->kind == TEMP)
                        {
                            AllReg[target].kind = 3;
                        }
                        AllReg[target].num = p->code.u.single.op->u.var_no;
                        AllReg[target].used = 1;
                        sprintf(code, "move %s, $v0", RegName[target]);
                        InsertAsmCode(code, true);
                    }
                }
                else if (offset <= 0)
                {
                    StoreToStack(offset, 2);
                }
            }
            else if (target >= 0)
            {
                sprintf(code, "move %s, $v0", RegName[target]);
                InsertAsmCode(code, true);
            }
        }
        else if (p->code.kind == WRITE)
        {
            int target = FindReg(p->code.u.single.op);
            if (target == -1)
            {
                int offset = GetStackOffset(p->code.u.single.op);
                if (offset <= 0)
                {
                    LoadToReg(offset, 4);
                }
            }
            else if (target >= 0)
            {
                sprintf(code, "move $a0, %s", RegName[target]);
                InsertAsmCode(code, true);
            }
            sprintf(code, "addi $sp, $sp, -4");
            InsertAsmCode(code, true);
            sprintf(code, "sw $ra, 0($sp)");
            InsertAsmCode(code, true);
            sprintf(code, "jal write");
            InsertAsmCode(code, true);
            sprintf(code, "lw $ra, 0($sp)");
            InsertAsmCode(code, true);
            sprintf(code, "addi $sp, $sp, 4");
            InsertAsmCode(code, true);
        }
        p = p->next;
    }
}