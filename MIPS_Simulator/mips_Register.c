#include <stdio.h>
#include <stdlib.h>
#include "mips_header.h"
#include "mips_Register.h"

unsigned int R[32];
unsigned int PC, HI, LO;

unsigned int REG(unsigned int A, unsigned int V, unsigned int nRW) {
    if(A>=0 && A<REG_MAX) {
        if(nRW == READ) {
            // return endian_swap(R[A],WORD);
            return R[A];
        } else if(nRW == WRITE) {
            // R[A] = endian_swap(V,WORD);
            R[A] = V;
            return 1;
        } else {
            printf("Nor Read or Write operation: REG\n");
            return 0;
        }
    } else {
        printf("Register index out of range\n");
        return 0;
    }
}
void setPC(unsigned int val) { PC = val; }

unsigned int getRegister(unsigned int A) {
    /* return
     * R[num] / if error 0 */
    return endian_swap(REG(A,0,READ),WORD); // big to little endian
}

unsigned setRegister(unsigned A, unsigned V) {
    /* return
     *   WRITE: 1 */
    unsigned ret = REG(A,endian_swap(V,WORD),WRITE); // little to big endian
    return ret;
}
unsigned int getPC_Offset(void) { return PC - PC_INIT; }

const char* getREGname(enum REGS regs)
{
    switch (regs)
    {
        case $zero: return "$zero";
        case $at: return "$at";
        case $v0: return "$v0";
        case $v1: return "$v1";
        case $a0: return "$a0";
        case $a1: return "$a1";
        case $a2: return "$a2";
        case $a3: return "$a3";
        case $t0: return "$t0";
        case $t1: return "$t1";
        case $t2: return "$t2";
        case $t3: return "$t3";
        case $t4: return "$t4";
        case $t5: return "$t5";
        case $t6: return "$t6";
        case $t7: return "$t7";
        case $s0: return "$s0";
        case $s1: return "$s1";
        case $s2: return "$s2";
        case $s3: return "$s3";
        case $s4: return "$s4";
        case $s5: return "$s5";
        case $s6: return "$s6";
        case $s7: return "$s7";
        case $t8: return "$t8";
        case $t9: return "$t9";
        case $gp: return "$gp";
        case $sp: return "$sp";
        case $fp: return "$fp";
        case $ra: return "$ra";
        default: return "out of range";
    }
}
