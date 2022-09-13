#include "register.h"

struct Reg AllReg[32];

char* RegName[32] = {"$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3", "$t0", "$t1", "$t2","$t3", "$t4",
"$t5", "$t6", "$t7", "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", "$t8", "$t9", "$k0","$k1","$gp",
"$sp", "$fp", "$ra"};

void InitAllReg()
{
    AllReg[0].kind = 1;
    AllReg[0].used = true;
    AllReg[0].num = 0;
    AllReg[1].used = true;
    AllReg[1].kind = 1;
    for(int i=2;i<32;++i)
    {
        if(i==26 || i==27)
        {
            AllReg[i].used = true;
            AllReg[i].kind = 1;
            continue;
        }
        AllReg[i].kind = 0;
        AllReg[i].used = false;
        AllReg[i].num = -1;
    }
}

int FindReg(Operand op)
{
    if(op->kind==VARIABLE)
    {
        for(int i=11;i<=25;++i)
        {
            if(AllReg[i].kind==2 && AllReg[i].num==op->u.var_no && AllReg[i].used)
            {
                return i;
            }
        }
        return -1;
    }
    else if(op->kind==TEMP)
    {
        for(int i=11;i<=25;++i)
        {
            if(AllReg[i].kind==3 && AllReg[i].num==op->u.var_no && AllReg[i].used)
            {
                return i;
            }
        }
        return -1;
    }
    return -1;
}

int FindFreeReg()
{
    for (int i = 11; i <= 25; ++i)
    {
        if (!AllReg[i].used)
        {
            return i;
        }
    }
    return -1;
}