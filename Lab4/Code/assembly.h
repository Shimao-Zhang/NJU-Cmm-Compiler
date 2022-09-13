#ifndef _ASSEMBLY_H_
#define _ASSEMBLY_H_

#include "tree.h"
#include "stdbool.h"
#include "register.h"

typedef struct AsmCode_* AsmCode;
typedef struct VarOffset_* VarOffset;

struct AsmCode_{
    bool iftab;
    char* code;
    struct AsmCode_ *next;
};

struct VarOffset_{
    int kind;       //Variable or Temp
    int var_no;
    int offset;
    struct VarOffset_ *next;
};

extern AsmCode AsmCodeListHead;

extern AsmCode AsmCodeListTail;

extern VarOffset VarOffsetTableHead;

extern VarOffset VarOffsetTableTail;

extern int CountArg;

extern int CountParam;

extern int LocalOffset;

int GetStackOffset(Operand op);

void AddNewVarOffset(Operand op, int offset);

void InsertAsmCode(char* code, bool iftab);

void PrintAllAssemblyCodes(FILE* f);

void LoadToReg(int offset, int RegNo);

void StoreToStack(int offset, int RegNo);

void InitAsmCodeList();

void TranslateToAsmCodes();

#endif