#ifndef _REGISTER_H_
#define _REGISTER_H_

#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"
#include "intermediate.h"

struct Reg{
    bool used;
    int num;
    int kind;    //reserved or variable or temp or constant
    int value;
};

extern char* RegName[32];

extern struct Reg AllReg[32];

void InitAllReg();

int FindReg(Operand op);

int FindFreeReg();

#endif