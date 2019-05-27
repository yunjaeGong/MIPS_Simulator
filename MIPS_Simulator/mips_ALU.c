#include <stdio.h>
#include <stdlib.h>
#include "mips_ALU.h"

#define SHIFT 0
#define SLT 1
#define ADD_SUB 2
#define LOGIC 3
#define SLL 1
#define SRL 2
#define SRA 3

int logicOperation (int X, int Y, int C) {
    if ( C < 0 || C > 3 ) {
        printf("error in logic operation\n");
        exit (1);
    }
    if (C == 0)
        return X&Y;
    else if (C==1)
        return X|Y;
    else if (C==2)
        return X^Y;
    else
        return !(X|Y);
}

int addSubtract (int X, int Y, int C)
{   int ret;
    C &= 0b1;
    if ( C < 0 || C > 1 ) {
        printf("error in add/subract operation\n");
        //exit (1);
        return -1;
    }
    if (C == 0) { // add
        ret = X+Y;
    } else {       // subtract
        ret = X-Y;
    }
    return ret;
}

// V is 5bit shift amount
int shiftOperation (int V, unsigned Y, int C) {
    /*
     * V: Value
     * Y: Shift amount
     * C: Control
     * */
    int ret;
    if ( C < 0 || C > 3 ) {
        printf("error in shift operation\n");
        exit(1);
    }
    if (C == 0) { // No Shift
        ret = V;
    } else if (C==SLL) { // Shift left logical
        ret = V<<Y;
    } else if (C==SRL) { // Shift right logical
        ret = (unsigned int)V>>Y;
    } else if (C==SRA) { // Shift right arithmetic
        int sign = Y & (1<<31);
        if(sign == 1) {
            int a = 0;
            for(int i=0;i<Y;++i)
                a |= 1<<i;  // 변수에 이동한 비트만큼 1로 채움
            ret = (V>>Y) | (a<<(32-Y)); // 32-이동할 비트만큼 left shift
        }
        else
            ret = V>>Y;
    }
    return ret;
}

// 이 함수는 add 또는 subtract 수행 시만
// 사용하여 Z 값을 설정한다.
int checkZero (int S) {
    /*check if S is zero,
    and return 1 if it is zero
    else return 0*/
    return S==0?1:0;
}

int checkSetLess (int X, int Y) {
    /*check if X < Y,
    and return 1 if it is true
    else return 0*/
    return X<Y?1:0;
}

int ALU(int X, unsigned Y, int C, int *Z, char** op)
{
    /* X: Operand X
     * Y: Operand Y
     * C: ALU Control
     * Z: Zero Flag
     * op: Operator
     * */
    int c32, c10; // Control Upper, Control Lower
    int ret;

    c32 = (C >> 2) & 0b11; // upper bits
    c10 = C & 0b11; // lower bits
    if (c32 == SHIFT) { // shift
        ret = shiftOperation(X, Y, c10);
        // *op = "Shift";
    } else if (c32 == SLT) {  // set less
        ret = checkSetLess(X,Y);
        // *op = "SetLess";
    } else if (c32 == ADD_SUB) {  // addsubtract
        ret = addSubtract(X,Y,c10);
        *Z = checkZero(ret);
        // *op = "Add/Sub";
    } else if (c32 == LOGIC) {  // logic
        ret = logicOperation(X, Y, c10);
        // *op = "Logic";
    }
    return ret;
}
